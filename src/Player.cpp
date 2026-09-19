#include "Player.hpp"

#include "AnmIdx.hpp"
#include "AnmManager.hpp"
#include "AsciiManager.hpp"
#include "BombData.hpp"
#include "Chain.hpp"
#include "Controller.hpp"
#include "EffectManager.hpp"
#include "EaglerOptions.hpp"
#include "EnemyManager.hpp"
#include "FileSystem.hpp"
#include "GameManager.hpp"
#include "GameWindow.hpp"
#include "Gui.hpp"
#include "PracticeRuntime.hpp"
#include "ReplayExtension.hpp"
#include "Rng.hpp"
#include "ScreenEffect.hpp"
#include "SoundPlayer.hpp"
#include "Stage.hpp"
#include "Touch.hpp"
#include "ZunMath.hpp"
#include "dxutil.hpp"
#include "utils.hpp"
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#include "multiplayer/GameplaySession.hpp"
#endif
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#include <eagler/netplay/NetplayInput.hpp>
#include "netplay/NetplaySideEffects.hpp"
#include "netplay/Th07RollbackState.hpp"
#endif

#include <algorithm>

ShtFunc1 g_ShtFireFuncs[6] = {
    NULL,
    ShtData::FireBulletDefault,
    ShtData::FireOrbBulletUnfocused,
    ShtData::FireOrbBulletFocused,
    ShtData::FireHomingBullet,
    ShtData::FireRotatingOrbBullet,
};

ShtFunc2 g_ShtUpdateFuncs[6] = {
    NULL,
    ShtData::UpdateHomingBullet,
    ShtData::UpdateHomingBulletFocused,
    ShtData::UpdateUpwardAcceleratingBullet,
    ShtData::UpdateOrbLaser,
    ShtData::UpdatePlayerLaser,
};

ShtFunc3 g_ShtDrawFuncs[2] = {
    NULL,
    ShtData::DrawBulletWithTrail,
};

ShtFunc4 g_ShtHitFuncs[4] = {
    NULL,
    ShtData::OnMissileHit,
    ShtData::SpawnHitParticles,
    NULL,
};

const char *g_ShooterTable[6] = {
    "data/ply00a.sht", "data/ply00b.sht", "data/ply01a.sht",
    "data/ply01b.sht", "data/ply02a.sht", "data/ply02b.sht",
};

namespace
{
// Effect 24's native ANM mapping.  Keep the optional always-visible marker
// outside EffectManager so a display preference can never consume an effect
// slot or advance the gameplay RNG.  The normal focus effect below remains
// completely vanilla.
constexpr i32 EAGLER_HITBOX_ANM = 0x2c2;
// Multiplayer readability only. Measure each remote ship against this
// machine's local player: opaque at 100 px and beyond, linearly fading over
// 50..100 px, and held at 20% opacity inside 50 px. The draw path must clamp
// both ANM interpolation endpoints; changing only the current color makes
// SetRenderStateForVm lerp from an opaque prevColor every presentation frame.
constexpr f32 REMOTE_PLAYER_FADE_START_DISTANCE = 100.0f;
constexpr f32 REMOTE_PLAYER_FADE_FULL_DISTANCE = 50.0f;
constexpr i32 REMOTE_PLAYER_FADE_MIN_ALPHA = 51;
constexpr u32 LOCAL_PLAYER_LOCATOR_COLOR = 0x20ffffff;
constexpr f32 LOCAL_PLAYER_LOCATOR_THICKNESS = 1.0f;
void ClampVmAlpha(AnmVm *vm, u8 alpha);
AnmVm g_EaglerHitboxVm;
bool g_EaglerHitboxVmActive = false;

bool CanSampleRawTouchForPlayer(const Player *player)
{
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    return !MultiplayerGameplay::IsMultiplayer() ||
           (player &&
            player->initParam == MultiplayerGameplay::GetLocalPlayerSlot());
#else
    (void)player;
    return true;
#endif
}

void DrawLocalPlayerLocator(const Player *player, const ZunVec3 &drawPlayerPos)
{
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (!player || !MultiplayerGameplay::IsMultiplayer() ||
        !EaglerOptions::EnhanceLocalPlayerVisibility() ||
        player->initParam != MultiplayerGameplay::GetLocalPlayerSlot() ||
        !g_GameManager.notInMenu || g_GameManager.replay ||
        g_GameManager.isInPauseMenu || g_GameManager.isInRetryMenu)
        return;

    const f32 left = g_GameManager.arcadeRegionTopLeftPos.x;
    const f32 top = g_GameManager.arcadeRegionTopLeftPos.y;
    const f32 right = left + g_GameManager.arcadeRegionSize.x;
    const f32 bottom = top + g_GameManager.arcadeRegionSize.y;
    const f32 x = left + drawPlayerPos.x;
    const f32 y = top + drawPlayerPos.y;
    const f32 halfThickness = LOCAL_PLAYER_LOCATOR_THICKNESS * 0.5f;

    // Screen-space presentation only. Keep both strokes strictly inside the
    // 384x448 arcade region so the locator can never leak into the side UI or
    // any outer menu surface.
    ZunRect horizontal{left, y - halfThickness, right, y + halfThickness};
    ZunRect vertical{x - halfThickness, top, x + halfThickness, bottom};
    ScreenEffect::DrawSquare(&horizontal, 0xffffffff);
    ScreenEffect::DrawSquare(&vertical, 0xffffffff);
#else
    (void)player;
    (void)drawPlayerPos;
#endif
}

bool PlayerUsedTouch(const Player *player)
{
#ifdef TH_ENABLE_NETPLAY
    if (Netplay::Input::PlayerButtonOverridesActive())
        return Netplay::Input::PlayerTouchUsed(player ? player->initParam : 0);
#endif
    return CanSampleRawTouchForPlayer(player) && Touch::WasUsedThisRun();
}

bool PlayerUsedTouchToBomb(const Player *player)
{
#ifdef TH_ENABLE_NETPLAY
    if (Netplay::Input::PlayerButtonOverridesActive())
        return Netplay::Input::PlayerTouchBomb(player ? player->initParam : 0);
#endif
    return CanSampleRawTouchForPlayer(player) && Touch::UsedTouchToBomb();
}

#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
struct RemotePresentationState
{
    bool valid = false;
    u64 lastTickNs = 0;
    ZunVec3 position{};
};

RemotePresentationState g_RemotePresentation[TH07_MULTI_MAX_PLAYERS];
ZunVec3 g_RemoteDrawOffsets[TH07_MULTI_MAX_PLAYERS];

ZunVec3 PresentRemotePlayer(Player *player, const ZunVec3 &target)
{
    if (!player || !MultiplayerGameplay::IsMultiplayer() ||
        player->initParam >= TH07_MULTI_MAX_PLAYERS ||
        player->initParam == MultiplayerGameplay::GetLocalPlayerSlot())
    {
        if (player && player->initParam < TH07_MULTI_MAX_PLAYERS)
        {
            g_RemotePresentation[player->initParam] = {};
            g_RemoteDrawOffsets[player->initParam] = {};
        }
        return target;
    }

    RemotePresentationState &state = g_RemotePresentation[player->initParam];
    const u64 now = SDL_GetTicksNS();
    const f32 dx = target.x - state.position.x;
    const f32 dy = target.y - state.position.y;
    const f32 distanceSq = dx * dx + dy * dy;
    const bool snap = !state.valid || distanceSq > 96.0f * 96.0f ||
                      player->playerState == PLAYER_STATE_DEAD ||
                      player->playerState == PLAYER_STATE_SPAWNING;
    if (snap)
    {
        state.position = target;
    }
    else
    {
        const f32 elapsed = std::clamp(
            static_cast<f32>(now - state.lastTickNs) / 1000000000.0f, 0.0f, 0.05f);
        const f32 blend = 1.0f - expf(-elapsed / 0.028f);
        state.position = state.position.Lerp(target, blend);

        // Smooth rollback corrections, but never let presentation trail the
        // authoritative position by more than twelve game pixels.
        const f32 lagX = target.x - state.position.x;
        const f32 lagY = target.y - state.position.y;
        const f32 lagSq = lagX * lagX + lagY * lagY;
        if (lagSq > 12.0f * 12.0f)
        {
            const f32 scale = 12.0f / sqrtf(lagSq);
            state.position.x = target.x - lagX * scale;
            state.position.y = target.y - lagY * scale;
        }
    }
    state.position.z = target.z;
    state.lastTickNs = now;
    state.valid = true;
    g_RemoteDrawOffsets[player->initParam] = {
        state.position.x - target.x, state.position.y - target.y, 0.0f};
    return state.position;
}
#endif

void RestartEaglerHitboxVm()
{
    const Rng savedRng = g_Rng;
    g_AnmManager->SetAnmIdxAndExecuteScript(&g_EaglerHitboxVm, EAGLER_HITBOX_ANM);
    g_Rng = savedRng;
    g_EaglerHitboxVm.zWriteDisable = 1;
    g_EaglerHitboxVm.color.color = 0xffffffff;
    g_EaglerHitboxVm.UpdatePrev();
    g_EaglerHitboxVmActive = true;
}

void UpdateEaglerHitboxVm(const Player *player)
{
    const bool nativeFocusEffect = player->focusEffect && player->focusEffect->inUseFlag &&
                                   player->focusEffect->effectId == 24;
    if (!EaglerOptions::AlwaysShowHitbox() || nativeFocusEffect)
    {
        g_EaglerHitboxVmActive = false;
        return;
    }

    if (!g_EaglerHitboxVmActive)
    {
        RestartEaglerHitboxVm();
        return;
    }

    g_EaglerHitboxVm.UpdatePrev();
    const Rng savedRng = g_Rng;
    const bool finished = g_AnmManager->ExecuteScript(&g_EaglerHitboxVm) != 0;
    g_Rng = savedRng;
    if (finished)
        RestartEaglerHitboxVm();
}

void DrawEaglerHitboxVm(const Player *player)
{
    if (!g_EaglerHitboxVmActive || !EaglerOptions::AlwaysShowHitbox() ||
        g_GameManager.isInRetryMenu)
        return;
    ZunVec3 drawPos = player->prevPos.Lerp(player->pos, g_RenderAlpha);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (MultiplayerGameplay::IsMultiplayer() &&
        player->initParam < TH07_MULTI_MAX_PLAYERS)
    {
        drawPos += g_RemoteDrawOffsets[player->initParam];
    }
#endif
    g_EaglerHitboxVm.pos = {g_GameManager.arcadeRegionTopLeftPos.x + drawPos.x,
                            g_GameManager.arcadeRegionTopLeftPos.y + drawPos.y, 0.0f};
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    const ZunColor originalColor = g_EaglerHitboxVm.color;
    const ZunColor originalPrevColor = g_EaglerHitboxVm.prevColor;
    const ZunColor originalColor2 = g_EaglerHitboxVm.color2;
    const ZunColor originalPrevColor2 = g_EaglerHitboxVm.prevColor2;
    if (MultiplayerGameplay::IsMultiplayer())
    {
        const u8 alpha = GetPlayerOverlapAlpha(player);
        ClampVmAlpha(&g_EaglerHitboxVm, alpha);
    }
#endif
    g_AnmManager->Draw(&g_EaglerHitboxVm);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    g_EaglerHitboxVm.color = originalColor;
    g_EaglerHitboxVm.prevColor = originalPrevColor;
    g_EaglerHitboxVm.color2 = originalColor2;
    g_EaglerHitboxVm.prevColor2 = originalPrevColor2;
#endif
}
} // namespace

const char *g_ShooterTableFocus[6] = {
    "data/ply00as.sht", "data/ply00bs.sht", "data/ply01as.sht",
    "data/ply01bs.sht", "data/ply02as.sht", "data/ply02bs.sht",
};

#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
Player g_Players[TH07_MULTI_MAX_PLAYERS];
bool g_PlayerActive[TH07_MULTI_MAX_PLAYERS] = {true, false, false};
i32 g_cherryMaxGrazeGrowth[TH07_MULTI_MAX_PLAYERS] = {0, 0, 0};
i32 g_cherryMaxBreakGrowth[TH07_MULTI_MAX_PLAYERS] = {0, 0, 0};
i32 g_powerGiveTaps[TH07_MULTI_MAX_PLAYERS] = {0, 0, 0};
i32 g_powerGiveWindow[TH07_MULTI_MAX_PLAYERS] = {0, 0, 0};
i32 g_teamWipeRetryFrames = 0;

namespace
{
bool g_SharedBorderTransition = false;
constexpr f32 PLAYER_SPIRIT_DRIFT_SPEED = 0.2f;
constexpr i32 LIFE_GIVE_WAIT_RELEASE_TOKEN = -1;
constexpr f32 REVIVE_PRESENTATION_BASE_ALPHA = 80.0f;
constexpr f32 REVIVE_PRESENTATION_ACTIVE_ALPHA = 254.0f;
constexpr f32 REVIVE_PRESENTATION_FADE_SECONDS = 0.20f;
struct RevivePresentationState
{
    bool valid = false;
    bool wasSpirit = false;
    u64 lastTickNs = 0;
    f32 alpha = REVIVE_PRESENTATION_BASE_ALPHA;
};
RevivePresentationState g_RevivePresentation[TH07_MULTI_MAX_PLAYERS];

bool IsSharedBorderParticipant(const Player *player)
{
    return player && player->playerState != PLAYER_STATE_ELIMINATED &&
           player->playerState != PLAYER_STATE_SPIRIT;
}

void ClearSharedBorderState(Player *player)
{
    if (!player)
        return;
    player->hasBorder = BORDER_NONE;
    player->playerState = PLAYER_STATE_INVULNERABLE;
    player->invulnerabilityTimer = 40;
    player->borderInvulnerabilityTime = 40;
    if (player->borderEffect)
    {
        player->borderEffect->inUseFlag = 0;
        player->borderEffect = nullptr;
    }
}

Player *GetSharedBorderOwner()
{
    for (u8 playerId = 0; playerId < TH07_MULTI_MAX_PLAYERS; ++playerId)
    {
        Player *player = &g_Players[playerId];
        if (IsPlayerSlotActive(playerId) &&
            player->hasBorder == BORDER_ACTIVE &&
            player->playerState == PLAYER_STATE_BORDER)
            return player;
    }
    return nullptr;
}

bool IsPlayerActiveForProximity(const Player *player)
{
    return player && IsPlayerSlotActive(player->initParam) &&
           !MultiplayerGameplay::IsPlayerTemporarilyAbsent(player->initParam) &&
           (player->playerState == PLAYER_STATE_ALIVE ||
            player->playerState == PLAYER_STATE_INVULNERABLE ||
            player->playerState == PLAYER_STATE_BORDER);
}

bool IsPlayerActiveForLifeTransfer(const Player *player)
{
    return IsPlayerActiveForProximity(player);
}

Player *SelectPowerTransferReceiver(const Player *giver)
{
    if (!giver || !IsPlayerSlotActive(giver->initParam))
        return nullptr;

    Player *best = nullptr;
    i32 bestPower = 0;
    for (u8 playerId = 0; playerId < TH07_MULTI_MAX_PLAYERS; ++playerId)
    {
        if (playerId == giver->initParam || !IsPlayerSlotActive(playerId))
            continue;
        Player *candidate = &g_Players[playerId];
        if (!IsPlayerActiveForProximity(candidate))
            continue;
        const i32 power = GetPlayerPower(playerId);
        if (power >= 128)
            continue;
        const f32 dx = giver->pos.x - candidate->pos.x;
        const f32 dy = giver->pos.y - candidate->pos.y;
        if (dx * dx + dy * dy > 400.0f)
            continue;
        if (!best || power < bestPower ||
            (power == bestPower && playerId < best->initParam))
        {
            best = candidate;
            bestPower = power;
        }
    }
    return best;
}

bool IsPowerTransferArmed(const Player *giver)
{
    return giver && GetActivePlayerCount() >= 2 &&
           IsPlayerActiveForProximity(giver) &&
           GetPlayerPower(giver->initParam) >= POWER_GIVE_AMOUNT &&
           SelectPowerTransferReceiver(giver) != nullptr;
}

void UpdatePowerTransfer(Player *giver)
{
    if (!giver || giver->initParam >= TH07_MULTI_MAX_PLAYERS)
        return;

    const u8 id = giver->initParam;
    if (!IsPowerTransferArmed(giver))
    {
        g_powerGiveTaps[id] = 0;
        g_powerGiveWindow[id] = 0;
        return;
    }
    if (g_powerGiveWindow[id] > 0 && --g_powerGiveWindow[id] == 0)
        g_powerGiveTaps[id] = 0;
    if (!WAS_PRESSED_PLAYER(giver, TH_BUTTON_SHOOT))
        return;

    ++g_powerGiveTaps[id];
    g_powerGiveWindow[id] = POWER_GIVE_TAP_WINDOW;
    if (g_powerGiveTaps[id] < POWER_GIVE_TAPS_REQUIRED)
    {
        g_SoundPlayer.PlaySoundByIdx(SOUND_ITEM_GET, 0);
        return;
    }

    g_powerGiveTaps[id] = 0;
    g_powerGiveWindow[id] = 0;
    Player *receiver = SelectPowerTransferReceiver(giver);
    if (!receiver || GetPlayerPower(id) < POWER_GIVE_AMOUNT)
        return;

    AddPlayerPower(id, -POWER_GIVE_AMOUNT);
    for (i32 index = 0; index < 6; ++index)
    {
        ZunVec3 spawn = giver->pos;
        spawn.x += (f32)((index % 3) - 1) * 10.0f;
        spawn.y += (f32)((index / 3) - 1) * 8.0f;
        g_ItemManager.SpawnItem(
            &spawn, index < 2 ? ITEM_POWER_BIG : ITEM_POWER_SMALL,
            GetLifeTransferSpawnState(receiver->initParam));
    }
    g_Gui.powerDisplayUpdateFrames = 2;
    g_SoundPlayer.PlaySoundByIdx(SOUND_POWERUP, 0);
}

Player *SelectLifeTransferReceiver(const Player *giver)
{
    if (!giver || !IsPlayerSlotActive(giver->initParam))
        return nullptr;

    Player *best = nullptr;
    bool bestIsSpirit = false;
    i32 bestLives = 0;
    for (u8 playerId = 0; playerId < TH07_MULTI_MAX_PLAYERS; ++playerId)
    {
        if (playerId == giver->initParam || !IsPlayerSlotActive(playerId))
            continue;
        Player *candidate = &g_Players[playerId];
        const bool isSpirit = candidate->playerState == PLAYER_STATE_SPIRIT;
        const i32 lives = GetPlayerLives(playerId);
        const bool canHoldLife =
            IsPlayerActiveForLifeTransfer(candidate) && lives < 8;
        if (!isSpirit && !canHoldLife)
            continue;
        const f32 dx = giver->pos.x - candidate->pos.x;
        const f32 dy = giver->pos.y - candidate->pos.y;
        if (dx * dx + dy * dy > 400.0f)
            continue;
        if (!best || (isSpirit && !bestIsSpirit) ||
            (isSpirit == bestIsSpirit && lives < bestLives) ||
            (isSpirit == bestIsSpirit && lives == bestLives &&
             playerId < best->initParam))
        {
            best = candidate;
            bestIsSpirit = isSpirit;
            bestLives = lives;
        }
    }
    return best;
}

i32 SelectLowestLifeRecipient(u8 excludedPlayerId)
{
    i32 bestId = -1;
    i32 bestLives = 0;
    for (u8 playerId = 0; playerId < TH07_MULTI_MAX_PLAYERS; ++playerId)
    {
        if (playerId == excludedPlayerId || !IsPlayerSlotActive(playerId) ||
            !IsPlayerActiveForLifeTransfer(&g_Players[playerId]))
            continue;
        const i32 lives = GetPlayerLives(playerId);
        if (bestId < 0 || lives < bestLives)
        {
            bestId = playerId;
            bestLives = lives;
        }
    }
    return bestId;
}

void UpdateLifeTransfer(Player *giver)
{
    if (!giver || GetActivePlayerCount() < 2 ||
        !IsPlayerActiveForLifeTransfer(giver))
    {
        if (giver)
        {
            giver->lifeGiveTimer = 0;
            giver->lifeGiveTargetToken = 0;
        }
        return;
    }

    if (giver->lifeGiveTargetToken == LIFE_GIVE_WAIT_RELEASE_TOKEN)
    {
        giver->lifeGiveTimer = 0;
        if (!giver->isFocus)
            giver->lifeGiveTargetToken = 0;
        return;
    }

    Player *receiver = SelectLifeTransferReceiver(giver);
    if (!receiver)
    {
        giver->lifeGiveTimer = 0;
        giver->lifeGiveTargetToken = 0;
        return;
    }
    if (giver->lifeGiveTargetToken != receiver->initParam + 1)
    {
        giver->lifeGiveTimer = 0;
        giver->lifeGiveTargetToken = receiver->initParam + 1;
    }

    if (g_powerGiveTaps[giver->initParam] > 0)
    {
        giver->lifeGiveTimer = 0;
        return;
    }
    if (!giver->isFocus || IS_PRESSED_PLAYER(giver, TH_BUTTON_SHOOT))
    {
        giver->lifeGiveTimer = 0;
        giver->lifeGiveTargetToken = 0;
        return;
    }

    g_SoundPlayer.PlaySoundByIdx(SOUND_ITEM_GET, 0);
    if (++giver->lifeGiveTimer < 90 || GetPlayerLives(giver->initParam) <= 0)
        return;

    if (receiver->playerState == PLAYER_STATE_SPIRIT)
    {
        AddPlayerLives(giver->initParam, -1);
        receiver->playerState = PLAYER_STATE_INVULNERABLE;
        receiver->optionState = OPTION_UNFOCUSED;
        receiver->invulnerabilityTimer = 120;
        receiver->respawnTimer = receiver->shooterData->initialRespawnTimer;
        receiver->bulletGracePeriod = 60;
        receiver->playerSprite.color.color = 0xffffffff;
        g_Gui.lifeDisplayUpdateFrames = 2;
        giver->lifeGiveTimer = 0;
        giver->lifeGiveTargetToken = LIFE_GIVE_WAIT_RELEASE_TOKEN;
        return;
    }

    giver->lifeGiveTimer = 0;
    giver->lifeGiveTargetToken = 0;
    Item *lifeItem = g_ItemManager.SpawnItem(
        &giver->pos, ITEM_LIFE,
        GetLifeTransferSpawnState(receiver->initParam));
    if (lifeItem != &g_ItemManager.items[1100])
    {
        AddPlayerLives(giver->initParam, -1);
        g_Gui.lifeDisplayUpdateFrames = 2;
        giver->lifeGiveTargetToken = LIFE_GIVE_WAIT_RELEASE_TOKEN;
        g_SoundPlayer.PlaySoundByIdx(SOUND_DIR_CHANGING, 0);
    }
}

bool IsPlayerActivelyBeingRevived(const Player *receiver)
{
    if (!receiver || receiver->playerState != PLAYER_STATE_SPIRIT)
        return false;
    const i32 token = receiver->initParam + 1;
    for (u8 playerId = 0; playerId < TH07_MULTI_MAX_PLAYERS; ++playerId)
    {
        const Player *giver = &g_Players[playerId];
        if (giver == receiver || !IsPlayerActiveForLifeTransfer(giver) ||
            GetPlayerLives(playerId) <= 0)
            continue;
        if (giver->lifeGiveTargetToken == token && giver->lifeGiveTimer > 0)
            return true;
    }
    return false;
}

u8 GetPlayerRescuePresentationAlpha(const Player *player, u8 normalAlpha)
{
    if (!player || player->initParam >= TH07_MULTI_MAX_PLAYERS)
        return normalAlpha;
    RevivePresentationState &state = g_RevivePresentation[player->initParam];
    const bool spirit = player->playerState == PLAYER_STATE_SPIRIT;
    const u64 now = SDL_GetTicksNS();

    if (!state.valid)
    {
        state.valid = true;
        state.wasSpirit = spirit;
        state.lastTickNs = now;
        state.alpha = spirit ? REVIVE_PRESENTATION_BASE_ALPHA : normalAlpha;
    }

    // First presented frame after a successful rescue is exactly 100%.
    if (state.wasSpirit && !spirit)
    {
        state.alpha = 255.0f;
        state.lastTickNs = now;
        state.wasSpirit = false;
        g_SoundPlayer.PlaySoundByIdx(SOUND_EXTEND, 0);
        return 255;
    }

    const f32 elapsed = std::clamp(
        static_cast<f32>(now - state.lastTickNs) / 1000000000.0f, 0.0f, 0.05f);
    state.lastTickNs = now;
    state.wasSpirit = spirit;

    const f32 target = spirit
                           ? (IsPlayerActivelyBeingRevived(player)
                                  ? REVIVE_PRESENTATION_ACTIVE_ALPHA
                                  : REVIVE_PRESENTATION_BASE_ALPHA)
                           : static_cast<f32>(normalAlpha);
    const f32 speed = (REVIVE_PRESENTATION_ACTIVE_ALPHA - REVIVE_PRESENTATION_BASE_ALPHA) /
                      REVIVE_PRESENTATION_FADE_SECONDS;
    if (state.alpha < target)
        state.alpha = std::min(target, state.alpha + speed * elapsed);
    else if (state.alpha > target)
        state.alpha = std::max(target, state.alpha - speed * elapsed);

    const i32 maxAlpha = spirit ? 254 : 255;
    return static_cast<u8>(std::clamp<i32>((i32)(state.alpha + 0.5f), 0, maxAlpha));
}

void UpdateSpiritState(Player *player)
{
    player->playerSprite.color.color = 0x50ffffff;
    player->pos.x += player->previousHorizontalSpeed;
    player->pos.y += player->previousVerticalSpeed;

    const f32 minX = g_GameManager.playerMovementAreaTopLeftPos.x;
    const f32 maxX = minX + g_GameManager.playerMovementAreaSize.x;
    const f32 minY = g_GameManager.playerMovementAreaTopLeftPos.y + 300.0f;
    const f32 maxY = g_GameManager.playerMovementAreaTopLeftPos.y +
                     g_GameManager.playerMovementAreaSize.y - 32.0f;
    if (player->pos.x < minX)
    {
        player->pos.x = minX;
        player->previousHorizontalSpeed = fabsf(player->previousHorizontalSpeed);
    }
    else if (player->pos.x > maxX)
    {
        player->pos.x = maxX;
        player->previousHorizontalSpeed = -fabsf(player->previousHorizontalSpeed);
    }
    if (player->pos.y < minY)
    {
        player->pos.y = minY;
        player->previousVerticalSpeed = fabsf(player->previousVerticalSpeed);
    }
    else if (player->pos.y > maxY)
    {
        player->pos.y = maxY;
        player->previousVerticalSpeed = -fabsf(player->previousVerticalSpeed);
    }
}

bool IsProximityFadeTarget(const Player *player)
{
    if (!player || GetActivePlayerCount() < 2 ||
        !IsPlayerSlotActive(player->initParam))
        return false;
    // Local co-op and hosted sessions use the same rule: the local ship is an
    // input cue and stays opaque; only other ships fade when they overlap it.
    return player->initParam != MultiplayerGameplay::GetLocalPlayerSlot();
}

u8 CalculatePlayerOverlapAlpha(const Player *player)
{
    if (!IsProximityFadeTarget(player) || !IsPlayerActiveForProximity(player))
        return 255;

    const u8 localPlayerId = MultiplayerGameplay::GetLocalPlayerSlot();
    if (localPlayerId >= TH07_MULTI_MAX_PLAYERS ||
        !IsPlayerSlotActive(localPlayerId))
        return 255;

    Player *localPlayer = &g_Players[localPlayerId];
    if (!IsPlayerActiveForProximity(localPlayer))
        return 255;

    const f32 dx = player->pos.x - localPlayer->pos.x;
    const f32 dy = player->pos.y - localPlayer->pos.y;
    f32 distance = sqrtf(dx * dx + dy * dy);
    if (distance >= REMOTE_PLAYER_FADE_START_DISTANCE)
        return 255;
    if (distance < REMOTE_PLAYER_FADE_FULL_DISTANCE)
        distance = REMOTE_PLAYER_FADE_FULL_DISTANCE;

    const f32 fadeProgress =
        (distance - REMOTE_PLAYER_FADE_FULL_DISTANCE) /
        (REMOTE_PLAYER_FADE_START_DISTANCE - REMOTE_PLAYER_FADE_FULL_DISTANCE);
    return (u8)std::clamp<i32>(
        (i32)(fadeProgress * (255 - REMOTE_PLAYER_FADE_MIN_ALPHA)) +
            REMOTE_PLAYER_FADE_MIN_ALPHA,
        0, 255);
}

void ClampVmAlpha(AnmVm *vm, u8 alpha)
{
    auto clamp = [alpha](ZunColor &color) {
        if (alpha < color.bytes.a)
            color.bytes.a = alpha;
    };
    clamp(vm->color);
    clamp(vm->prevColor);
    clamp(vm->color2);
    clamp(vm->prevColor2);
}

void DrawPowerTransferPrompt(const Player *giver)
{
    if (!giver || !MultiplayerGameplay::IsMultiplayer() ||
        giver->initParam >= TH07_MULTI_MAX_PLAYERS ||
        !IsPowerTransferArmed(giver) ||
        g_powerGiveTaps[giver->initParam] < POWER_GIVE_PROMPT_AFTER)
        return;

    ZunVec3 position =
        giver->prevPos.Lerp(giver->pos, g_RenderAlpha) +
        GetPlayerPresentationOffset(giver->initParam);
    position.x += g_GameManager.arcadeRegionTopLeftPos.x - 16.0f;
    position.y += g_GameManager.arcadeRegionTopLeftPos.y + 16.0f;
    position.z = 0.48f;
    const Float2 oldScale = g_AsciiManager.scale;
    const u32 oldColor = g_AsciiManager.color;
    const i32 oldGui = g_AsciiManager.isGui;
    const i32 oldSelected = g_AsciiManager.isSelected;
    g_AsciiManager.scale = {0.5f, 0.5f};
    g_AsciiManager.color = 0xffa0ffa0;
    g_AsciiManager.isGui = 1;
    g_AsciiManager.isSelected = 0;
    AsciiManager::AddFormatText(&g_AsciiManager, &position, "P %d/%d",
                                (int)g_powerGiveTaps[giver->initParam],
                                (int)POWER_GIVE_TAPS_REQUIRED);
    g_AsciiManager.scale = oldScale;
    g_AsciiManager.color = oldColor;
    g_AsciiManager.isGui = oldGui;
    g_AsciiManager.isSelected = oldSelected;
}

void DrawLifeTransferPrompt(const Player *giver)
{
    if (!giver || GetPlayerLives(giver->initParam) <= 0 ||
        giver->lifeGiveTargetToken == LIFE_GIVE_WAIT_RELEASE_TOKEN ||
        !SelectLifeTransferReceiver(giver) || !giver->isFocus ||
        IS_PRESSED_PLAYER(giver, TH_BUTTON_SHOOT) ||
        g_powerGiveTaps[giver->initParam] > 0)
        return;

    ZunVec3 position =
        giver->prevPos.Lerp(giver->pos, g_RenderAlpha) +
        GetPlayerPresentationOffset(giver->initParam);
    position.x += g_GameManager.arcadeRegionTopLeftPos.x - 14.0f;
    position.y += g_GameManager.arcadeRegionTopLeftPos.y - 22.0f;
    position.z = 0.48f;
    const Float2 oldScale = g_AsciiManager.scale;
    const u32 oldColor = g_AsciiManager.color;
    const i32 oldGui = g_AsciiManager.isGui;
    const i32 oldSelected = g_AsciiManager.isSelected;
    g_AsciiManager.scale = {0.6f, 0.6f};
    g_AsciiManager.color = 0xffffff00;
    g_AsciiManager.isGui = 1;
    g_AsciiManager.isSelected = 0;
    AsciiManager::AddFormatText(&g_AsciiManager, &position, "%d%%",
                                giver->lifeGiveTimer * 100 / 90);
    g_AsciiManager.scale = oldScale;
    g_AsciiManager.color = oldColor;
    g_AsciiManager.isGui = oldGui;
    g_AsciiManager.isSelected = oldSelected;
}

constexpr i32 STAGE_INTRO_NAME_FRAMES = 240;
constexpr f32 STAGE_INTRO_NAME_SCALE = 0.48f;

bool IsStageIntroActive()
{
    return g_GameManager.notInMenu &&
           (i32)g_GameManager.framesThisStage < STAGE_INTRO_NAME_FRAMES;
}

void DrawStageIntroPlayerName(const Player *player)
{
    static const u32 nameColors[TH07_MULTI_MAX_PLAYERS] = {
        0xffffffff, 0xffa0d0ff, 0xffa8ffa8};

    if (!player || !MultiplayerGameplay::IsMultiplayer() ||
        !MultiplayerGameplay::ShouldShowStagePlayerNames() ||
        player->initParam >= TH07_MULTI_MAX_PLAYERS || !IsStageIntroActive())
        return;

    const char *name = MultiplayerGameplay::GetPlayerName(player->initParam);
    if (!name || name[0] == '\0')
        return;

    const f32 labelWidth =
        (f32)strlen(name) * 8.0f * STAGE_INTRO_NAME_SCALE;
    ZunVec3 position =
        player->prevPos.Lerp(player->pos, g_RenderAlpha) +
        GetPlayerPresentationOffset(player->initParam);
    position.x -= labelWidth * 0.5f;
    position.y -= 22.0f + (f32)player->initParam * 9.0f;
    if (position.x < 2.0f)
        position.x = 2.0f;
    if (position.x + labelWidth > g_GameManager.arcadeRegionSize.x - 2.0f)
        position.x = g_GameManager.arcadeRegionSize.x - 2.0f - labelWidth;
    position.x += g_GameManager.arcadeRegionTopLeftPos.x;
    position.y += g_GameManager.arcadeRegionTopLeftPos.y;
    position.z = 0.48f;

    const Float2 oldScale = g_AsciiManager.scale;
    const u32 oldColor = g_AsciiManager.color;
    const i32 oldGui = g_AsciiManager.isGui;
    const i32 oldSelected = g_AsciiManager.isSelected;
    g_AsciiManager.scale = {STAGE_INTRO_NAME_SCALE, STAGE_INTRO_NAME_SCALE};
    g_AsciiManager.color = nameColors[player->initParam];
    g_AsciiManager.isGui = 1;
    g_AsciiManager.isSelected = 0;
    AsciiManager::AddFormatText(&g_AsciiManager, &position, "%s", name);
    g_AsciiManager.scale = oldScale;
    g_AsciiManager.color = oldColor;
    g_AsciiManager.isGui = oldGui;
    g_AsciiManager.isSelected = oldSelected;
}

i32 UpdateMultiplayerDeath(Player *player)
{
    f32 invulnScale;
    i32 cherryPenalty;

    if (player->respawnTimer != 0)
    {
        if (player->hasBorder == BORDER_ACTIVE)
        {
            player->BreakBorder();
            return 0;
        }
        --player->respawnTimer;
        if (PracticeRuntime::OverlayAutoBomb())
            g_CurFrameRawInput = TH_BUTTON_BOMB;
        if (player->respawnTimer == 0)
        {
            g_ReplayManager->replayEventFlags |= 4;
            g_GameManager.powerItemCountForScore = 0;
            g_EnemyManager.spellcardInfo.captureScore = 0;
            g_EnemyManager.spellcardInfo.isCapturing = 0;
            g_GameManager.CheckGameIntegrityOnDeath(1);

            if (GetPlayerLives(player->initParam) > 0)
            {
                if (!PracticeRuntime::OverlayInfinitePower())
                {
                    if (GetPlayerPower(player->initParam) <= 16)
                        SetPlayerPower(player->initParam, 0);
                    else
                        AddPlayerPower(player->initParam, -16);
                }
                g_ItemManager.SpawnItem(&player->pos, ITEM_POWER_BIG, 2);
                for (i32 i = 0; i < 5; ++i)
                    g_ItemManager.SpawnItem(&player->pos, ITEM_POWER_SMALL, 2);
                g_Gui.powerDisplayUpdateFrames = 2;

                cherryPenalty =
                    (f32)(g_GameManager.cherry - g_GameManager.globals->cherryStart) *
                    player->shooterData->cherryPenaltyMultiplier;
                const i32 character =
                    MultiplayerGameplay::GetPlayerCharacter(player->initParam);
                const i32 cap = character == CHAR_SAKUYA ? 60000 : 100000;
                if (cherryPenalty > cap)
                    cherryPenalty = cap;
                cherryPenalty -= cherryPenalty % 10;
                g_GameManager.cherry -= cherryPenalty;
                g_Gui.pointDisplayUpdateFrames = 2;
                g_ItemManager.ActivateAllItems();
            }
            else
            {
                if (!PracticeRuntime::OverlayInfinitePower())
                    SetPlayerPower(player->initParam, 0);
                for (i32 i = 0; i < 5; ++i)
                    g_ItemManager.SpawnItem(&player->pos, ITEM_FULL_POWER, 2);
                g_Gui.powerDisplayUpdateFrames = 2;
            }
            g_GameManager.DecreaseSubrank(GetMultiplayerRankPenalty(1600));
        }
        return 0;
    }

    invulnScale = player->invulnerabilityTimer.AsFloat() / 30.0f;
    player->playerSprite.scale.y = 3.0f * invulnScale + 1.0f;
    player->playerSprite.scale.x = 1.0f - invulnScale;
    player->playerSprite.color.color =
        (u32)(255.0f - player->invulnerabilityTimer.AsFloat() * 255.0f / 30.0f)
            << 24 |
        0xffffff;
    player->playerSprite.blendMode = 1;
    player->previousHorizontalSpeed = 0.0f;
    player->previousVerticalSpeed = 0.0f;
    if (player->invulnerabilityTimer.GetCurrent() < 30)
        return 0;

    player->playerState = PLAYER_STATE_SPAWNING;
    if (MultiplayerGameplay::GetPlayerCount() >= 3)
    {
        player->pos.x = g_GameManager.arcadeRegionSize.x / 2.0f +
                                   ((i32)player->initParam - 1) * 48.0f;
    }
    else
    {
        player->pos.x = g_GameManager.arcadeRegionSize.x / 2.0f +
                                   (player->initParam == 0 ? -32.0f : 32.0f);
    }
    player->pos.y = g_GameManager.arcadeRegionSize.y - 64.0f;
    player->pos.z = 0.2f;
    player->prevPos = player->pos;
    player->invulnerabilityTimer = 0;
    player->playerSprite.scale.x = 3.0f;
    player->playerSprite.scale.y = 3.0f;
    g_AnmManager->SetAnmIdxAndExecuteScript(
        &player->playerSprite, GetPlayerAnmScript(player, 1024));

    if (GetPlayerLives(player->initParam) <= 0)
    {
        player->playerState = PLAYER_STATE_SPIRIT;
        player->optionState = OPTION_HIDDEN;
        player->isFocus = 0;
        player->lifeGiveTimer = 0;
        player->lifeGiveTargetToken = 0;
        player->bulletGracePeriod = 10;
        player->previousHorizontalSpeed =
            (g_Rng.GetRandomU16() & 1) ? PLAYER_SPIRIT_DRIFT_SPEED
                                       : -PLAYER_SPIRIT_DRIFT_SPEED;
        player->previousVerticalSpeed =
            (g_Rng.GetRandomU16() & 1) ? PLAYER_SPIRIT_DRIFT_SPEED
                                       : -PLAYER_SPIRIT_DRIFT_SPEED;
        SetPlayerBombs(player->initParam, 3);
        g_Gui.bombDisplayUpdateFrames = 2;
        const i32 recipientId = SelectLowestLifeRecipient(player->initParam);
        if (recipientId >= 0)
        {
            g_ItemManager.SpawnItem(
                &player->pos, ITEM_LIFE,
                GetLifeTransferSpawnState((u8)recipientId));
        }
        player->playerSprite.color.color = 0x50ffffff;

        // Individual players remain as Spirits so a surviving teammate can
        // revive them. A total wipe gets a short deterministic grace period so
        // the room can issue Restart before TH07 enters its original retry/end
        // lifecycle. 180 logical ticks is three seconds at TH07's 60 Hz sim.
        bool teamWiped = true;
        bool hasParticipant = false;
        for (u8 playerId = 0; playerId < TH07_MULTI_MAX_PLAYERS; ++playerId)
        {
            if (!IsPlayerSlotActive(playerId))
                continue;
            hasParticipant = true;
            const i8 state = g_Players[playerId].playerState;
            if (state != PLAYER_STATE_SPIRIT && state != PLAYER_STATE_ELIMINATED)
            {
                teamWiped = false;
                break;
            }
        }
        if (hasParticipant && teamWiped && g_teamWipeRetryFrames <= 0)
            g_teamWipeRetryFrames = 180;
        return 0;
    }

    if (!PracticeRuntime::OverlayInfiniteLives())
        AddPlayerLives(player->initParam, -1);
    g_Gui.lifeDisplayUpdateFrames = 2;
    SetPlayerBombs(player->initParam, (i32)player->shooterData->initialBombs);
    g_Gui.bombDisplayUpdateFrames = 2;
    return 1;
}
} // namespace

bool VerifyThreePlayerLifeTransferSelectionRules()
{
    if (!MultiplayerGameplay::IsMultiplayer() || GetActivePlayerCount() != 3 ||
        !g_GameManager.globals)
        return false;

    Player &giver = g_Player;
    Player &player2 = g_Player2;
    Player &player3 = g_Player3;
    const ZunVec3 savedPosition2 = player2.pos;
    const ZunVec3 savedPosition3 = player3.pos;
    const i8 savedState2 = player2.playerState;
    const i8 savedState3 = player3.playerState;
    const i32 savedLives2 = GetPlayerLives(1);
    const i32 savedLives3 = GetPlayerLives(2);

    player2.pos = giver.pos;
    player2.pos.x += 10.0f;
    player3.pos = giver.pos;
    player3.pos.x -= 10.0f;

    player2.playerState = PLAYER_STATE_ALIVE;
    player3.playerState = PLAYER_STATE_SPIRIT;
    SetPlayerLives(1, 0);
    SetPlayerLives(2, 7);
    Player *spiritWinner = SelectLifeTransferReceiver(&giver);

    player3.playerState = PLAYER_STATE_ALIVE;
    SetPlayerLives(1, 2);
    SetPlayerLives(2, 1);
    Player *lowLifeWinner = SelectLifeTransferReceiver(&giver);

    SetPlayerLives(1, 1);
    SetPlayerLives(2, 1);
    Player *slotWinner = SelectLifeTransferReceiver(&giver);

    player2.pos = savedPosition2;
    player3.pos = savedPosition3;
    player2.playerState = savedState2;
    player3.playerState = savedState3;
    SetPlayerLives(1, savedLives2);
    SetPlayerLives(2, savedLives3);

    return spiritWinner == &player3 && lowLifeWinner == &player3 &&
           slotWinner == &player2;
}

u8 GetPlayerOverlapAlpha(const Player *player)
{
    return CalculatePlayerOverlapAlpha(player);
}

ZunVec3 GetPlayerPresentationOffset(u8 playerId)
{
    if (!MultiplayerGameplay::IsMultiplayer() ||
        playerId >= TH07_MULTI_MAX_PLAYERS ||
        playerId == MultiplayerGameplay::GetLocalPlayerSlot())
        return {};
    return g_RemoteDrawOffsets[playerId];
}

Player *GetPlayerById(u8 playerId)
{
    return playerId < TH07_MULTI_MAX_PLAYERS ? &g_Players[playerId] : nullptr;
}

const Player *GetPlayerByIdConst(u8 playerId)
{
    return playerId < TH07_MULTI_MAX_PLAYERS ? &g_Players[playerId] : nullptr;
}

bool IsPlayerSlotActive(u8 playerId)
{
    return playerId < TH07_MULTI_MAX_PLAYERS && g_PlayerActive[playerId];
}

u8 GetActivePlayerMask()
{
    u8 mask = 0;
    for (u8 playerId = 0; playerId < TH07_MULTI_MAX_PLAYERS; ++playerId)
    {
        if (g_PlayerActive[playerId])
            mask |= static_cast<u8>(1u << playerId);
    }
    return mask;
}

i32 GetActivePlayerCount()
{
    i32 count = 0;
    for (u8 playerId = 0; playerId < TH07_MULTI_MAX_PLAYERS; ++playerId)
        count += g_PlayerActive[playerId] ? 1 : 0;
    return count;
}

void UpdateTeamWipeRetryCountdown()
{
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (!MultiplayerGameplay::IsMultiplayer() || g_teamWipeRetryFrames <= 0)
        return;

    // The grace window is gameplay time, not wall-clock time. Do not consume
    // it while the synchronized pause/retry UI or TH07's ordinary time-stop is
    // active; otherwise opening Pause after a team wipe could make Retry fire
    // behind the menu.
    if (g_GameManager.isPaused || g_GameManager.isInPauseMenu ||
        g_GameManager.isInRetryMenu || g_GameManager.isTimeStopped)
        return;

    bool hasParticipant = false;
    bool teamWiped = true;
    for (u8 playerId = 0; playerId < TH07_MULTI_MAX_PLAYERS; ++playerId)
    {
        if (!IsPlayerSlotActive(playerId))
            continue;
        hasParticipant = true;
        const i8 state = g_Players[playerId].playerState;
        if (state != PLAYER_STATE_SPIRIT && state != PLAYER_STATE_ELIMINATED)
        {
            teamWiped = false;
            break;
        }
    }

    if (!hasParticipant || !teamWiped)
    {
        g_teamWipeRetryFrames = 0;
        return;
    }

    if (--g_teamWipeRetryFrames <= 0)
    {
        g_teamWipeRetryFrames = 0;
        g_GameManager.isInRetryMenu = 1;
    }
#endif
}

bool IsAnyActivePlayerBombing()
{
    for (u8 playerId = 0; playerId < TH07_MULTI_MAX_PLAYERS; ++playerId)
    {
        if (IsPlayerSlotActive(playerId) && g_Players[playerId].bombInfo.isInUse)
            return true;
    }
    return false;
}

bool IsSharedBorderActive()
{
    for (u8 playerId = 0; playerId < TH07_MULTI_MAX_PLAYERS; ++playerId)
    {
        const Player &player = g_Players[playerId];
        if (g_PlayerActive[playerId] &&
            player.hasBorder == BORDER_ACTIVE &&
            player.playerState == PLAYER_STATE_BORDER)
        {
            return true;
        }
    }
    return false;
}

void ActivateSharedBorder()
{
    if (g_SharedBorderTransition)
        return;

    g_SharedBorderTransition = true;
    for (u8 playerId = 0; playerId < TH07_MULTI_MAX_PLAYERS; ++playerId)
    {
        Player &player = g_Players[playerId];
        if (g_PlayerActive[playerId] &&
            IsSharedBorderParticipant(&player) &&
            player.hasBorder != BORDER_ACTIVE)
        {
            player.ActivateBorder();
        }
    }
    g_SharedBorderTransition = false;
}

Player *GetClosestActivePlayer(ZunVec3 *position)
{
    if (!position)
        return &g_Player;

    Player *closest = nullptr;
    f32 closestDistance = 0.0f;
    for (u8 playerId = 0; playerId < TH07_MULTI_MAX_PLAYERS; ++playerId)
    {
        Player &player = g_Players[playerId];
        if (!g_PlayerActive[playerId] ||
            MultiplayerGameplay::IsPlayerTemporarilyAbsent(playerId) ||
            (player.playerState != PLAYER_STATE_ALIVE &&
             player.playerState != PLAYER_STATE_INVULNERABLE &&
             player.playerState != PLAYER_STATE_BORDER))
        {
            continue;
        }

        const f32 dx = player.pos.x - position->x;
        const f32 dy = player.pos.y - position->y;
        const f32 distance = dx * dx + dy * dy;
        // Exact ties intentionally stay with the lower slot, matching the
        // upstream multiplayer rule on every peer.
        if (!closest || distance < closestDistance)
        {
            closest = &player;
            closestDistance = distance;
        }
    }
    return closest ? closest : &g_Player;
}

i32 GetPlayerAnmScript(const Player *player, i32 script)
{
    if (!player || player->initParam == 0)
        return script;
    return script +
           (player->initParam == 1 ? ANM_OFFSET_PLAYER2 : ANM_OFFSET_PLAYER3) -
           ANM_OFFSET_PLAYER;
}

i32 GetPlayerEffectSlot(const Player *player, i32 p1Slot)
{
    if (!player || player->initParam == 0)
        return p1Slot;

    const i32 base = player->initParam == 1 ? 5 : 9;
    switch (p1Slot)
    {
    case 0:
        return base;
    case 2:
        return base + 1;
    case 3:
        return base + 2;
    case 4:
        return base + 3;
    default:
        return p1Slot;
    }
}
#else
Player g_Player;
#endif

void DefaultFireBulletCallback(Player *player, PlayerBullet *bullet, ShtEntry *shtEntry)
{
    if (shtEntry->option == 0)
    {
        bullet->pos = player->pos;
    }
    else
    {
        bullet->pos = player->optionsPosition[shtEntry->option - 1];
    }
    *bullet->GetPosX() += shtEntry->offset.x;
    *bullet->GetPosY() += shtEntry->offset.y;
    bullet->pos.z = 0.495f;
    bullet->prevPos = bullet->pos;
    bullet->hitboxSize.x = shtEntry->hitboxSize.x;
    bullet->hitboxSize.y = shtEntry->hitboxSize.y;
    bullet->hitboxSize.z = 1.0f;
    bullet->prevAngle = bullet->angle = shtEntry->angle;
    bullet->speed = shtEntry->speed;
    bullet->velocity.x = cosf(shtEntry->angle) * shtEntry->speed;
    bullet->velocity.y = sinf(shtEntry->angle) * shtEntry->speed;
    bullet->timer = 0;
    bullet->bulletState2 = shtEntry->bulletState2;
    bullet->damage = shtEntry->damage;
    if (shtEntry->soundIdx >= 0)
    {
        g_SoundPlayer.PlaySoundByIdx(shtEntry->soundIdx, 0);
    }
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    g_AnmManager->SetAnmIdxAndExecuteScript(
        &bullet->vm, GetPlayerAnmScript(player, shtEntry->anmFileIdx));
#else
    g_AnmManager->SetAnmIdxAndExecuteScript(&bullet->vm, shtEntry->anmFileIdx);
#endif
}

i32 ShtData::FireBulletDefault(Player *player, PlayerBullet *bullet, i32 fireTime,
                               ShtEntry *shtEntry)
{
    if (fireTime % shtEntry->fireInterval == shtEntry->fireOffset)
    {
        DefaultFireBulletCallback(player, bullet, shtEntry);
        return 1;
    }
    return 0;
}

i32 ShtData::FireOrbBulletUnfocused(Player *player, PlayerBullet *bullet, i32 fireTime,
                                    ShtEntry *shtEntry)
{
    (void)fireTime;

    i32 fireOffset = shtEntry->fireOffset;

    if (player->timers[fireOffset].bullet)
    {
        if (player->shtEntries[fireOffset] != shtEntry)
        {
            player->timers[fireOffset].bullet->vm.pendingInterrupt = 1;
            player->timers[fireOffset].bullet = NULL;
        }
        return 0;
    }

    if (player->optionState != OPTION_UNFOCUSED)
    {
        return 0;
    }

    player->timers[fireOffset].timer = shtEntry->fireInterval;
    player->timers[fireOffset].bullet = bullet;
    bullet->timerIdx = fireOffset;
    bullet->optionId = (i16)shtEntry->option;
    bullet->offset.x = shtEntry->offset.x;
    bullet->offset.y = shtEntry->offset.y;
    DefaultFireBulletCallback(player, bullet, shtEntry);
    player->shtEntries[fireOffset] = shtEntry;
    return 1;
}

i32 ShtData::FireOrbBulletFocused(Player *player, PlayerBullet *bullet, i32 fireTime,
                                  ShtEntry *shtEntry)
{
    (void)fireTime;

    i32 fireOffset = shtEntry->fireOffset;

    if (player->timers[fireOffset].bullet)
    {
        if (player->shtEntries[fireOffset] != shtEntry)
        {
            player->timers[fireOffset].bullet->vm.pendingInterrupt = 1;
            player->timers[fireOffset].bullet = NULL;
        }
        return 0;
    }

    if (player->optionState != OPTION_FOCUSED)
    {
        return 0;
    }

    player->timers[fireOffset].timer = 999;
    player->timers[fireOffset].bullet = bullet;
    bullet->timerIdx = fireOffset;
    bullet->optionId = (i16)shtEntry->option;
    bullet->offset.x = shtEntry->offset.x;
    bullet->offset.y = shtEntry->offset.y;
    bullet->trailLength = shtEntry->fireInterval;
    DefaultFireBulletCallback(player, bullet, shtEntry);
    for (i32 i = ARRAY_SIZE_SIGNED(bullet->posHistory) - 1; i >= 0; i--)
    {
        bullet->posHistory[i].x = -999.0f;
    }
    bullet->pos.x = -999.0f;
    player->shtEntries[fireOffset] = shtEntry;
    return 1;
}

i32 ShtData::FireHomingBullet(Player *player, PlayerBullet *bullet, i32 fireTime,
                              ShtEntry *shtEntry)
{
    f32 angle;
    f32 speed;

    if (fireTime % shtEntry->fireInterval == shtEntry->fireOffset)
    {
        DefaultFireBulletCallback(player, bullet, shtEntry);
        if (player->sakuyaTargetPosition.x > -100.0f)
        {
            angle = utils::AddNormalizeAngle(atan2f(player->sakuyaTargetPosition.y - bullet->pos.y,
                                                    player->sakuyaTargetPosition.x - bullet->pos.x),
                                             shtEntry->angle + ZUN_PI / 2.0f);
            speed = shtEntry->speed * 1.5f;
            (*(ZunVec3 *)&bullet->velocity).FromAngleMagnitude(angle, speed);
            bullet->angle = angle;
        }
        return 1;
    }

    return 0;
}

i32 ShtData::FireRotatingOrbBullet(Player *player, PlayerBullet *bullet, i32 fireTime,
                                   ShtEntry *shtEntry)
{
    f32 angle;
    f32 speed;

    if (fireTime % shtEntry->fireInterval == shtEntry->fireOffset)
    {
        DefaultFireBulletCallback(player, bullet, shtEntry);
        angle = utils::AddNormalizeAngle(player->optionAngle, shtEntry->angle + ZUN_PI / 2.0f);
        speed = shtEntry->speed;
        (*(ZunVec3 *)&bullet->velocity).FromAngleMagnitude(angle, speed);
        bullet->angle = angle;

        return 1;
    }

    return 0;
}

i32 ShtData::UpdateHomingBullet(Player *player, PlayerBullet *bullet)
{
    f32 length;
    f32 x;
    f32 y;

    if (bullet->bulletState == 1)
    {
        if (player->positionOfLastEnemyHit.x > -100.0f && bullet->timer.GetCurrent() < 40 &&
            bullet->timer.HasTicked())
        {
            x = player->positionOfLastEnemyHit.x - bullet->pos.x;
            y = player->positionOfLastEnemyHit.y - bullet->pos.y;
            length = sqrtf(x * x + y * y) / (bullet->speed / 4.0f);

            if (length < 1.0f)
            {
                length = 1.0f;
            }

            x = x / length + bullet->velocity.x;
            y = y / length + bullet->velocity.y;
            length = sqrtf(x * x + y * y);

            bullet->speed = length > 10.0f ? 10.0f : length;

            if (bullet->speed < 1.0f)
            {
                bullet->speed = 1.0f;
            }

            bullet->velocity.x = x * bullet->speed / length;
            bullet->velocity.y = y * bullet->speed / length;
        }
        else
        {
            if (bullet->speed < 10.0f)
            {
                bullet->speed += 1.0f / 3.0f;
                x = bullet->velocity.x;
                y = bullet->velocity.y;
                length = sqrtf(x * x + y * y);
                bullet->velocity.x = x * bullet->speed / length;
                bullet->velocity.y = y * bullet->speed / length;
            }
        }
    }
    return 0;
}

i32 ShtData::UpdateHomingBulletFocused(Player *player, PlayerBullet *bullet)
{
    f32 length;
    f32 x;
    f32 y;

    if (bullet->bulletState == 1)
    {
        if (player->positionOfLastEnemyHit.x > -100.0f && bullet->timer.GetCurrent() < 40 &&
            bullet->timer.HasTicked())
        {
            x = player->positionOfLastEnemyHit.x - bullet->pos.x;
            y = player->positionOfLastEnemyHit.y - bullet->pos.y;
            length = sqrtf(x * x + y * y) / (bullet->speed / 4.0f);
            if (length < 1.0f)
            {
                length = 1.0f;
            }
            x = x / length + bullet->velocity.x;
            y = y / length + bullet->velocity.y;
            length = sqrtf(x * x + y * y);
            bullet->speed = length > 18.0f ? 18.0f : length;
            if (bullet->speed < 1.0f)
            {
                bullet->speed = 1.0f;
            }
            bullet->velocity.x = x * bullet->speed / length;
            bullet->velocity.y = y * bullet->speed / length;
        }
        else
        {
            if (bullet->speed < 18.0f)
            {
                bullet->speed = bullet->speed + 0.6f;
                x = bullet->velocity.x;
                y = bullet->velocity.y;
                length = sqrtf(x * x + y * y);
                bullet->velocity.x = x * bullet->speed / length;
                bullet->velocity.y = y * bullet->speed / length;
            }
        }
    }
    return 0;
}

i32 ShtData::UpdateUpwardAcceleratingBullet(Player *player, PlayerBullet *bullet)
{
    (void)player;

    if (bullet->bulletState == 1)
    {
        bullet->velocity.y = bullet->velocity.y - (g_Rng.GetRandomFloatInRange(0.1f) + 0.27f);
    }
    return 0;
}

i32 ShtData::UpdateOrbLaser(Player *player, PlayerBullet *bullet)
{
    if (player->timers[bullet->timerIdx].bullet != bullet && bullet->vm.isStopped)
    {
        bullet->vm.pendingInterrupt = 1;
    }
    if ((g_Gui.HasCurrentMsgIdx() || player->bombInfo.isInUse) &&
        player->timers[bullet->timerIdx].timer.GetCurrent() > 20)
    {
        player->timers[bullet->timerIdx].timer = 20;
    }
    if (player->timers[bullet->timerIdx].timer <= 0)
    {
        player->timers[bullet->timerIdx].timer = 0;
        player->timers[bullet->timerIdx].bullet = NULL;
        bullet->bulletState = 0;
        return 1;
    }

    if (player->timers[bullet->timerIdx].timer <= 70 && bullet->vm.isStopped)
    {
        bullet->vm.pendingInterrupt = 1;
    }
    bullet->pos = player->optionsPosition[bullet->optionId - 1];
    bullet->pos.x += bullet->offset.x;
    bullet->pos.z = 0.44f;
    if (player->playerState == PLAYER_STATE_DEAD)
    {
        return 1;
    }
    else
    {
        bullet->vm.scale.y = bullet->pos.y / 14.0f;
        bullet->hitboxSize.y = bullet->pos.y;
        bullet->pos.y = bullet->pos.y / 2.0f;
        return 0;
    }
}

i32 ShtData::UpdatePlayerLaser(Player *player, PlayerBullet *bullet)
{
    i32 i;

    if (player->timers[bullet->timerIdx].bullet != bullet && bullet->vm.isStopped)
    {
        bullet->vm.pendingInterrupt = 1;
    }
    if ((g_Gui.HasCurrentMsgIdx() || player->bombInfo.isInUse) &&
        player->timers[bullet->timerIdx].timer.GetCurrent() > 20)
    {
        player->timers[bullet->timerIdx].timer = 20;
    }
    if (player->timers[bullet->timerIdx].timer <= 0)
    {
        player->timers[bullet->timerIdx].timer = 0;
        bullet->bulletState = 0;
        player->timers[bullet->timerIdx].bullet = NULL;
        return 1;
    }

    if (player->timers[bullet->timerIdx].timer <= 70 && bullet->vm.isStopped)
    {
        bullet->vm.pendingInterrupt = 1;
    }
    for (i = 0; i < bullet->trailLength; i++)
    {
        if (bullet->posHistory[i].x >= -900.0f)
        {
            player->bombDamageBoxes[i + 96].pos = bullet->posHistory[i];
            player->bombDamageBoxes[i + 96].lifetime = 1;
            player->bombDamageBoxes[i + 96].size = bullet->hitboxSize;
        }
    }
    for (i = ARRAY_SIZE_SIGNED(bullet->posHistory) - 1; i > 0; i--)
    {
        bullet->posHistory[i] = bullet->posHistory[i - 1];
    }
    bullet->posHistory[0] = bullet->pos;
    if (player->playerState == PLAYER_STATE_DEAD)
    {
        return 1;
    }
    else
    {
        bullet->pos = player->pos;
        bullet->pos.x += bullet->offset.x;
        bullet->pos.z = 0.44f;
        bullet->vm.scale.y = (bullet->pos.y + 64.0f) / 14.0f;
        bullet->hitboxSize.y = player->pos.y + 64.0f;
        bullet->pos.y = bullet->pos.y / 2.0f - 32.0f;
        return 0;
    }
}

i32 ShtData::DrawBulletWithTrail(Player *player, PlayerBullet *bullet)
{
    (void)player;

    i32 i;
    i32 origAlpha;
    i32 origPrevAlpha;

    origAlpha = bullet->vm.color.bytes.a;
    origPrevAlpha = bullet->vm.prevColor.bytes.a;
    for (i = 0; i < bullet->trailLength; i++)
    {
        if (bullet->posHistory[i].x == -999.0f)
        {
            break;
        }

        ZunVec3 prevPos = bullet->posHistory[i];
        if (i + 1 < 16 && bullet->posHistory[i + 1].x != -999.0f)
        {
            prevPos = bullet->posHistory[i + 1];
        }

        bullet->vm.pos = prevPos.Lerp(bullet->posHistory[i], g_RenderAlpha);

        bullet->vm.prevColor.bytes.a = bullet->vm.color.bytes.a =
            origAlpha - origAlpha * i / bullet->trailLength;

        *bullet->GetVmPosX() += g_GameManager.arcadeRegionTopLeftPos.x;
        *bullet->GetVmPosY() += g_GameManager.arcadeRegionTopLeftPos.y;

        g_AnmManager->Draw(&bullet->vm);
    }
    bullet->vm.color.bytes.a = origAlpha;
    bullet->vm.prevColor.bytes.a = origPrevAlpha;
    return 0;
}

i32 ShtData::OnMissileHit(Player *player, PlayerBullet *bullet, ZunVec3 *pos)
{
    (void)player;

    f32 angle;

    if (bullet->bulletState == 2)
    {
        if (bullet->timer.GetCurrent() % 2 != 0)
        {
            return 1;
        }
        bullet->damage = bullet->damage / 3;
        if (bullet->damage == 0)
        {
            bullet->damage = 1;
        }
        bullet->velocity.x *= 0.88f;
        bullet->velocity.y *= 0.88f;
    }
    else
    {
        angle = g_Rng.GetRandomFloatInRange(1.5707964f) - 2.3561945f;
        i32 missileAnmIdx = bullet->vm.anmFileIdx;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        if (MultiplayerGameplay::IsMultiplayer())
        {
            missileAnmIdx -= GetPlayerAnmScript(player, ANM_OFFSET_PLAYER) -
                             ANM_OFFSET_PLAYER;
        }
#endif
        switch (missileAnmIdx)
        {
        case 1089:
            bullet->hitboxSize.x = 32.0f;
            bullet->hitboxSize.y = 32.0f;
            (*(ZunVec3 *)&bullet->velocity).FromAngleMagnitude(angle, 4.0f);
            break;
        case 1090:
            bullet->hitboxSize.x = 42.0;
            bullet->hitboxSize.y = 42.0;
            (*(ZunVec3 *)&bullet->velocity).FromAngleMagnitude(angle, 4.0f);
            break;
        case 1091:
            bullet->hitboxSize.x = 48.0f;
            bullet->hitboxSize.y = 48.0f;
            (*(ZunVec3 *)&bullet->velocity).FromAngleMagnitude(angle, 4.0f);
            break;
        case 1092:
            bullet->hitboxSize.x = 56.0f;
            bullet->hitboxSize.y = 56.0f;
            (*(ZunVec3 *)&bullet->velocity).FromAngleMagnitude(angle, 4.0f);
            break;
        case 1093:
            bullet->hitboxSize.x = 48.0f;
            bullet->hitboxSize.y = 48.0f;
            (*(ZunVec3 *)&bullet->velocity).FromAngleMagnitude(angle, 6.0f);
            break;
        case 1094:
            bullet->hitboxSize.x = 64.0f;
            bullet->hitboxSize.y = 64.0f;
            (*(ZunVec3 *)&bullet->velocity).FromAngleMagnitude(angle, 6.0f);
            break;
        case 1095:
            bullet->hitboxSize.x = 80.0f;
            bullet->hitboxSize.y = 80.0f;
            (*(ZunVec3 *)&bullet->velocity).FromAngleMagnitude(angle, 6.0f);
            break;
        case 1096:
            bullet->hitboxSize.x = 96.0f;
            bullet->hitboxSize.y = 96.0f;
            (*(ZunVec3 *)&bullet->velocity).FromAngleMagnitude(angle, 6.0f);
        }
    }
    if (bullet->timer.GetCurrent() % 6 == 0)
    {
        g_EffectManager.SpawnEffect(5, pos, 1, 0xffffffff);
    }
    return 0;
}

i32 ShtData::SpawnHitParticles(Player *player, PlayerBullet *bullet, ZunVec3 *pos)
{
    ZunVec3 particlePos;

    player->bombParticleTime++;
    if (player->bombParticleTime % 8 == 0)
    {
        particlePos = *pos;
        particlePos.x = bullet->pos.x;
        g_EffectManager.SpawnEffect(5, &particlePos, 1, 0xffffffff);
    }
    return 0;
}

void Player::SpawnBullets(Player *player, u32 timer)
{
    ShtEntry *entry;
    i32 ret;
    PlayerBullet *bullet;
    ShtLevel *level;
    i32 i;

    level = !player->isFocus ? player->shooterData->levels : player->shooterDataFocus->levels;

#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    const i32 currentPower = MultiplayerGameplay::IsMultiplayer()
                                 ? GetPlayerPower(player->initParam)
                                 : (i32)g_GameManager.globals->currentPower;
#else
    const i32 currentPower = (i32)g_GameManager.globals->currentPower;
#endif
    while (currentPower >= level->requiredPower)
    {
        level++;
    }

    entry = level->entry;
    bullet = player->bullets;
    for (i = 0; i < ARRAY_SIZE_SIGNED(player->bullets); i++, bullet++)
    {
        if (bullet->bulletState != 0)
        {
            continue;
        }

#ifdef TH_ENABLE_NETPLAY
        Netplay::Th07Rollback::TouchPlayerBullet(bullet);
#endif

    loop_with_goto_for_some_reason:
        if (entry->fireCallback)
        {
            ret = entry->fireCallback(player, bullet, timer, entry);
        }
        else
        {
            ret = ShtData::FireBulletDefault(player, bullet, timer, entry);
        }
        if (ret == 1)
        {
            bullet->vm.zWriteDisable = 1;
            bullet->bulletState = 1;
            bullet->shtEntry = entry;
            bullet->updateCallback = bullet->shtEntry->updateCallback;
            bullet->drawCallback = bullet->shtEntry->drawCallback;
            bullet->hitCallback = bullet->shtEntry->hitCallback;
        }
        entry++;
        if (entry->fireInterval < 0)
        {
            return;
        }

        if (!ret)
        {
            goto loop_with_goto_for_some_reason;
        }
    }
}

void Player::UpdateShots()
{
    PlayerBullet *bullet;
    i32 i;

    if (this->optionState != OPTION_FOCUSED && this->timers[2].bullet)
    {
        this->timers[2].bullet->bulletState = 0;
        this->timers[2].bullet = NULL;
    }
    if (this->optionState != OPTION_UNFOCUSED)
    {
        if (this->timers[0].bullet)
        {
            this->timers[0].bullet->vm.pendingInterrupt = 1;
            this->timers[0].bullet = NULL;
        }
        if (this->timers[1].bullet)
        {
            this->timers[1].bullet->vm.pendingInterrupt = 1;
            this->timers[1].bullet = NULL;
        }
    }
    if (this->playerState == PLAYER_STATE_DEAD)
    {
        for (i = 0; i < ARRAY_SIZE_SIGNED(this->timers); i++)
        {
            if (this->timers[i].bullet)
            {
                this->timers[i].bullet->bulletState = 0;
                this->timers[i].bullet = NULL;
            }
        }
    }
    for (i = 0; i < ARRAY_SIZE_SIGNED(this->timers); i++)
    {
        if (!this->timers[i].bullet)
        {
            continue;
        }
        if (this->timers[i].timer.GetCurrent() > 0 && this->timers[i].timer.GetCurrent() < 999)
        {
            this->timers[i].timer--;
        }
        if (this->fireBulletTimer.GetCurrent() < 0 && this->timers[i].timer.GetCurrent() > 50)
        {
            this->timers[i].timer = 50;
        }
        if (this->timers[i].timer.GetCurrent() == 0)
        {
            this->timers[i].bullet = NULL;
        }
    }
    bullet = this->bullets;
    for (i = 0; i < ARRAY_SIZE_SIGNED(this->bullets); i++, bullet++)
    {
        if (bullet->bulletState == 0)
        {
            continue;
        }

        bullet->prevPos = bullet->pos;
        bullet->prevAngle = bullet->angle;

        if (bullet->updateCallback && bullet->updateCallback(this, bullet))
        {
            bullet->bulletState = 0;
            continue;
        }

        *bullet->GetPosX() += bullet->velocity.x * g_Supervisor.effectiveFramerateMultiplier;
        *bullet->GetPosY() += bullet->velocity.y * g_Supervisor.effectiveFramerateMultiplier;
        if (bullet->bulletState2 != 4 && bullet->bulletState2 != 5 &&
            !g_GameManager.IsInBounds(bullet->pos.x, bullet->pos.y, bullet->vm.sprite->widthPx,
                                      bullet->vm.sprite->heightPx))
        {
            bullet->bulletState = 0;
        }
        if (g_AnmManager->ExecuteScript(&bullet->vm))
        {
            bullet->bulletState = 0;
        }
        bullet->timer++;
    }
}

void Player::DrawBullets()
{
    PlayerBullet *bullet;
    i32 i;

    bullet = this->bullets;
    for (i = 0; i < ARRAY_SIZE_SIGNED(this->bullets); i++, bullet++)
    {
        if (bullet->bulletState != 1)
        {
            continue;
        }

        if (bullet->vm.autoRotate)
        {
            f32 angle = utils::AddNormalizeAngle(
                utils::LerpAngle(bullet->prevAngle, bullet->angle, g_RenderAlpha), ZUN_PI / 2.0f);
            bullet->vm.rotation.z = angle;
            bullet->vm.prevRotation.z = angle;
            bullet->vm.updateRotation = 1;
        }

        ZunVec3 drawPos = bullet->prevPos.Lerp(bullet->pos, g_RenderAlpha);
        bullet->vm.pos.x = g_GameManager.arcadeRegionTopLeftPos.x + drawPos.x;
        bullet->vm.pos.y = g_GameManager.arcadeRegionTopLeftPos.y + drawPos.y;
        bullet->vm.pos.z = 0.4f;
        g_AnmManager->Draw(&bullet->vm);
        if (bullet->drawCallback)
        {
            bullet->drawCallback(this, bullet);
        }
    }
}

void Player::DrawBulletExplosions()
{
    PlayerBullet *bullet;
    i32 i;

    bullet = this->bullets;
    for (i = 0; i < ARRAY_SIZE_SIGNED(this->bullets); i++, bullet++)
    {
        if (bullet->bulletState != 2)
        {
            continue;
        }

        if (bullet->vm.autoRotate)
        {
            f32 angle = utils::AddNormalizeAngle(bullet->angle, ZUN_PI / 2.0f);
            bullet->vm.rotation.z = angle;
            bullet->vm.updateRotation = 1;
        }

        ZunVec3 drawPos = bullet->prevPos.Lerp(bullet->pos, g_RenderAlpha);
        bullet->vm.pos.x = g_GameManager.arcadeRegionTopLeftPos.x + drawPos.x;
        bullet->vm.pos.y = g_GameManager.arcadeRegionTopLeftPos.y + drawPos.y;
        bullet->vm.pos.z = 0.4f;
        g_AnmManager->Draw(&bullet->vm);
    }
}

i32 Player::UpdateFireBulletTimer()
{
    if (this->fireBulletTimer.GetCurrent() < 0)
    {
        return 0;
    }
    bool marisaB = g_GameManager.character == CHAR_MARISA &&
                   g_GameManager.shotType == 1;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (MultiplayerGameplay::IsMultiplayer())
    {
        marisaB = MultiplayerGameplay::GetPlayerCharacter(this->initParam) == CHAR_MARISA &&
                  MultiplayerGameplay::GetPlayerShot(this->initParam) == 1;
    }
#endif
    if (this->fireBulletTimer.HasTicked() &&
        (!this->bombInfo.isInUse || !marisaB))
    {
        SpawnBullets(this, this->fireBulletTimer.GetCurrent());
    }
    this->fireBulletTimer++;
    if (this->fireBulletTimer.GetCurrent() >= 30 || this->playerState == PLAYER_STATE_DEAD ||
        this->playerState == PLAYER_STATE_SPAWNING)
    {

        this->fireBulletTimer = -1;
    }
    return 0;
}

void Player::StartFireBulletTimer()
{
    if (this->fireBulletTimer.GetCurrent() < 0)
    {
        this->fireBulletTimer = 0;
    }
}

i32 Player::CalcDamageToEnemy(ZunVec3 *center, ZunVec3 *size, i32 *param_3)
{
    ZunVec3 bulletTopLeft;

    i32 damage;
    ZunVec3 enemyTopLeft;
    ZunVec3 bulletBottomRight;
    ZunVec3 enemyBottomRight;
    i32 i;
    PlayerBullet *bullet;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    i32 bombDamage = 0;
#endif

    damage = 0;
    if (!this->invulnerabilityTimer.HasTicked())
    {
        return 0;
    }

    enemyTopLeft.x = center->x - size->x * 0.5f;
    enemyTopLeft.y = center->y - size->y * 0.5f;
    enemyBottomRight.x = center->x + size->x * 0.5f;
    enemyBottomRight.y = center->y + size->y * 0.5f;

    bullet = this->bullets;
    if (param_3)
    {
        *param_3 = 0;
    }
    for (i = 0; i < ARRAY_SIZE_SIGNED(this->bullets); i++, bullet++)
    {
        if (bullet->bulletState == 0 || (bullet->bulletState != 1 && bullet->bulletState2 != 3))
        {
            continue;
        }

        SetVecCorners(&bulletTopLeft, &bulletBottomRight, &bullet->pos, &bullet->hitboxSize);

        if (bulletTopLeft.y > enemyBottomRight.y || bulletTopLeft.x > enemyBottomRight.x ||
            bulletBottomRight.y < enemyTopLeft.y || bulletBottomRight.x < enemyTopLeft.x)
        {
            continue;
        }

        if (bullet->bulletState2 == 4 || bullet->bulletState2 == 5)
        {
            if (bullet->timer.current % 2 != 0)
            {
                continue;
            }
        }
        if (bullet->hitCallback && bullet->hitCallback(this, bullet, center))
        {
            continue;
        }

        if (!this->bombInfo.isInUse)
        {
            damage += bullet->damage;
        }
        else
        {
            damage += bullet->damage / 3 != 0 ? bullet->damage / 3 : 1;
        }
        if (bullet->bulletState2 != 4 && bullet->bulletState2 != 5)
        {
            if (bullet->bulletState == 1)
            {
                g_AnmManager->SetAnmIdxAndExecuteScript(&bullet->vm, bullet->vm.anmFileIdx + 32);
                g_EffectManager.SpawnEffect(5, &bullet->pos, 1, 0xffffffff);
                bullet->pos.z = 0.1f;
            }
            bullet->bulletState = 2;
            if (bullet->bulletState2 != 3)
            {
                bullet->velocity.x /= 8.0f;
                bullet->velocity.y /= 8.0f;
            }
        }
    }
    for (i = 0; i < ARRAY_SIZE_SIGNED(this->bombDamageBoxes); i++)
    {
        if (this->bombDamageBoxes[i].size.x <= 0.0f)
        {
            continue;
        }

        bulletTopLeft = this->bombDamageBoxes[i].pos - this->bombDamageBoxes[i].size / 2.0f;
        bulletBottomRight = this->bombDamageBoxes[i].pos + this->bombDamageBoxes[i].size / 2.0f;

        if (bulletTopLeft.x > enemyBottomRight.x || bulletBottomRight.x < enemyTopLeft.x ||
            bulletTopLeft.y > enemyBottomRight.y || bulletBottomRight.y < enemyTopLeft.y)
        {
            continue;
        }

#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        bombDamage += this->bombDamageBoxes[i].lifetime;
#else
        damage += this->bombDamageBoxes[i].lifetime;
#endif
        this->bombDamageBoxes[i].damage += this->bombDamageBoxes[i].lifetime;
        this->bombParticleTime++;
        if (this->bombParticleTime % 4 == 0)
        {
            if (i < ARRAY_SIZE_SIGNED(this->bombClearBoxes))
            {
                g_EffectManager.SpawnEffect(3, center, 1, 0xffffffff);
            }
            else
            {
                g_EffectManager.SpawnEffect(5, center, 1, 0xffffffff);
            }
        }
        if (this->bombInfo.isInUse && param_3)
        {
            *param_3 = 1;
        }
    }
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    damage += (i32)((f32)bombDamage * GetMultiplayerBombDamageMultiplier());
#endif
    return damage;
}

void Player::RebuildBombBoxCache()
{
    this->numActiveBombClearBoxes = 0;
    this->dirtyBombBoxes = false;

    for (i32 i = 0; i < 96; i++)
    {
        BombClearBox *bomb = &this->bombClearBoxes[i];
        if (bomb->size.x != 0.0f)
        {
            CachedBombClearBox *c =
                &this->activeBombClearBoxesCache[this->numActiveBombClearBoxes++];
            c->isBox = true;
            c->minX = bomb->pos.x - bomb->size.x * 0.5f;
            c->maxX = bomb->pos.x + bomb->size.x * 0.5f;
            c->minY = bomb->pos.y - bomb->size.y * 0.5f;
            c->maxY = bomb->pos.y + bomb->size.y * 0.5f;
            c->itemType = bomb->itemType;
        }
        else if (bomb->radius != 0.0f)
        {
            CachedBombClearBox *c =
                &this->activeBombClearBoxesCache[this->numActiveBombClearBoxes++];
            c->isBox = false;
            c->cx = bomb->pos.x;
            c->cy = bomb->pos.y;
            c->radiusSq = bomb->radius * bomb->radius;
            c->itemType = bomb->itemType;
        }
    }
}

i32 Player::CalcBombCollision(ZunVec3 *center, ZunVec3 *size)
{
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (MultiplayerGameplay::IsMultiplayer() &&
        !IsPlayerActiveForProximity(this))
        return 0;
#endif
    if (this->dirtyBombBoxes)
    {
        RebuildBombBoxCache();
    }

    if (this->numActiveBombClearBoxes == 0)
    {
        return 0;
    }

    f32 halfW = size->x * 0.5f;
    f32 halfH = size->y * 0.5f;
    f32 bulletMinX = center->x - halfW;
    f32 bulletMinY = center->y - halfH;
    f32 bulletMaxX = center->x + halfW;
    f32 bulletMaxY = center->y + halfH;

    for (i32 i = 0; i < this->numActiveBombClearBoxes; i++)
    {
        const CachedBombClearBox &c = this->activeBombClearBoxesCache[i];
        if (c.isBox)
        {
            if (!(c.minX > bulletMaxX || c.maxX < bulletMinX || c.minY > bulletMaxY ||
                  c.maxY < bulletMinY))
            {
                this->itemType = c.itemType;
                return 2;
            }
        }
        else
        {
            f32 dx = center->x - c.cx;
            f32 dy = center->y - c.cy;
            if (dx * dx + dy * dy < c.radiusSq)
            {
                this->itemType = c.itemType;
                return 2;
            }
        }
    }

    return 0;
}

i32 Player::CalcKillboxCollision(ZunVec3 *center, ZunVec3 *size)
{
    ZunVec3 killboxBottomRight;
    ZunVec3 killboxTopLeft;

#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (MultiplayerGameplay::IsMultiplayer() &&
        !IsPlayerActiveForProximity(this))
        return 0;
#endif

    this->itemType = ITEM_POINT_BULLET;
    if (CalcBombCollision(center, size))
    {
        return 2;
    }

    killboxTopLeft.x = center->x - size->x / 2.0f;
    killboxTopLeft.y = center->y - size->y / 2.0f;
    killboxBottomRight.x = center->x + size->x / 2.0f;
    killboxBottomRight.y = center->y + size->y / 2.0f;
    if (this->hitboxTopLeft.x > killboxBottomRight.x ||
        this->hitboxTopLeft.y > killboxBottomRight.y ||
        this->hitboxBottomRight.x < killboxTopLeft.x ||
        this->hitboxBottomRight.y < killboxTopLeft.y)
    {
        return 0;
    }

    g_ReplayManager->replayEventFlags = g_ReplayManager->replayEventFlags | 2;
    if (this->playerState == PLAYER_STATE_BORDER)
    {
        this->BreakBorder();
        return 1;
    }
    if (this->playerState != PLAYER_STATE_ALIVE)
    {
        return 1;
    }

    g_GameManager.RerollRng();
    Die();
    return 1;
}

i32 Player::CheckGraze(ZunVec3 *center, ZunVec3 *size)
{
    ZunVec3 bulletBottomRight;
    ZunVec3 bulletTopLeft;

#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (MultiplayerGameplay::IsMultiplayer() &&
        !IsPlayerActiveForProximity(this))
        return 0;
#endif

    this->itemType = ITEM_POINT_BULLET;

    if (CalcBombCollision(center, size))
    {
        return PLAYER_COLLISION_BOMB;
    }

    bulletTopLeft.x = center->x - size->x / 2.0f - 20.0f;
    bulletTopLeft.y = center->y - size->y / 2.0f - 20.0f;
    bulletBottomRight.x = center->x + size->x / 2.0f + 20.0f;
    bulletBottomRight.y = center->y + size->y / 2.0f + 20.0f;

    if (this->playerState == PLAYER_STATE_DEAD || this->playerState == PLAYER_STATE_SPAWNING)
    {
        return PLAYER_COLLISION_NONE;
    }

    if (this->grazeTopLeft.x > bulletBottomRight.x || this->grazeBottomRight.x < bulletTopLeft.x ||
        this->grazeTopLeft.y > bulletBottomRight.y || this->grazeBottomRight.y < bulletTopLeft.y)
    {
        return PLAYER_COLLISION_NONE;
    }

    ScoreGraze(center);
    return PLAYER_COLLISION_HIT;
}

i32 Player::CalcItemBoxCollision(ZunVec3 *center, ZunVec3 *size)
{
    ZunVec3 itemBottomRight;
    ZunVec3 itemTopLeft;

#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (MultiplayerGameplay::IsMultiplayer() &&
        !IsPlayerActiveForProximity(this))
        return 0;
#endif

    if (this->playerState != PLAYER_STATE_ALIVE && this->playerState != PLAYER_STATE_INVULNERABLE &&
        this->playerState != PLAYER_STATE_BORDER)
    {
        return PLAYER_COLLISION_NONE;
    }

    itemTopLeft = *center - *size / 2.0f;
    itemBottomRight = *center + *size / 2.0f;

    if (this->grabItemTopLeft.x > itemBottomRight.x ||
        this->grabItemBottomRight.x < itemTopLeft.x ||
        this->grabItemTopLeft.y > itemBottomRight.y || this->grabItemBottomRight.y < itemTopLeft.y)
    {
        return 0;
    }

    return 1;
}

i32 Player::CalcLaserHitbox(ZunVec3 *center, ZunVec3 *size, ZunVec3 *origin, f32 rotation,
                            i32 canGraze)
{
    ZunVec3 playerRelativeTopLeft;
    ZunVec3 playerRelativeBottomRight;
    ZunVec3 laserTopLeft;
    ZunVec3 laserBottomRight;

#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (MultiplayerGameplay::IsMultiplayer() &&
        !IsPlayerActiveForProximity(this))
        return 0;
#endif

    laserTopLeft = this->pos - *origin;
    utils::Rotate(&laserBottomRight, &laserTopLeft, rotation);
    laserBottomRight.z = 0;
    laserTopLeft = laserBottomRight + *origin;
    playerRelativeTopLeft = laserTopLeft - this->hitboxSize;
    playerRelativeBottomRight = laserTopLeft + this->hitboxSize;

    laserTopLeft = *center - *size / 2.0f;
    laserBottomRight = *center + *size / 2.0f;
    if (!(playerRelativeTopLeft.x > laserBottomRight.x ||
          playerRelativeBottomRight.x < laserTopLeft.x ||
          playerRelativeTopLeft.y > laserBottomRight.y ||
          playerRelativeBottomRight.y < laserTopLeft.y))
    {
        goto LASER_COLLISION;
    }

    if (!canGraze)
    {
        return PLAYER_COLLISION_NONE;
    }

    laserTopLeft.x -= 48.0f;
    laserTopLeft.y -= 48.0f;
    laserBottomRight.x += 48.0f;
    laserBottomRight.y += 48.0f;
    if (playerRelativeTopLeft.x > laserBottomRight.x ||
        playerRelativeBottomRight.x < laserTopLeft.x ||
        playerRelativeTopLeft.y > laserBottomRight.y ||
        playerRelativeBottomRight.y < laserTopLeft.y)
    {
        return PLAYER_COLLISION_NONE;
    }

    if (this->playerState == PLAYER_STATE_DEAD || this->playerState == PLAYER_STATE_SPAWNING)
    {
        return PLAYER_COLLISION_NONE;
    }

    ScoreGraze(&this->pos);
    return PLAYER_COLLISION_BOMB;

LASER_COLLISION:
    g_ReplayManager->replayEventFlags = g_ReplayManager->replayEventFlags | 2;
    if (this->playerState == PLAYER_STATE_BORDER)
    {
        // this is already a member function of Player though
        this->BreakBorder();
        return 1;
    }
    if (this->playerState != PLAYER_STATE_ALIVE)
    {
        return 0;
    }

    g_GameManager.RerollRng();
    Die();
    return 1;
}

void Player::ScoreGraze(ZunVec3 *param_1)
{
    ZunVec3 grazePos;

    if (!this->bombInfo.isInUse)
    {
        if (g_GameManager.globals->grazeInStage < 9999)
        {
            g_GameManager.globals->grazeInStage++;
        }
        if (g_GameManager.globals->grazeInTotal < 999999)
        {
            g_GameManager.globals->grazeInTotal++;
        }
    }
    grazePos = (this->pos + *param_1) / 2.0f;
    if (this->hasBorder == BORDER_ACTIVE)
    {
        if (this->isFocus)
        {
            g_EffectManager.SpawnEffect(8, &grazePos, 1, 0xffffffff);
        }
        else
        {
            g_EffectManager.SpawnEffect(8, &grazePos, 3, 0xffff8080);
        }
    }
    else
    {
        g_EffectManager.SpawnEffect(8, &grazePos, 1, 0xffffffff);
    }
    g_GameManager.IncreaseSubrank(6);
    g_Gui.grazeDisplayUpdateFrames = 2;
    g_SoundPlayer.PlaySoundByIdx(SOUND_GRAZE, 0);
    g_EnemyManager.spellcardInfo.grazeBonusScore =
        g_EnemyManager.spellcardInfo.grazeBonusScore + 2500 +
        (g_GameManager.cherry - g_GameManager.globals->cherryStart) / 1500 * 20;
    g_GameManager.AddScore(2000);
    if (this->hasBorder == BORDER_ACTIVE)
    {
        const i32 grazeGrowth = this->isFocus ? 30 : 80;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        if (MultiplayerGameplay::IsMultiplayer() &&
            this->initParam < TH07_MULTI_MAX_PLAYERS)
            g_cherryMaxGrazeGrowth[this->initParam] += grazeGrowth;
#endif
        if (this->isFocus)
        {
            g_GameManager.IncreaseCherryMax(grazeGrowth);
            g_GameManager.IncreaseCherry(grazeGrowth);
        }
        else
        {
            g_GameManager.IncreaseCherryMax(grazeGrowth);
            g_GameManager.IncreaseCherry(grazeGrowth);
        }
    }
}

void Player::Die()
{
    g_GameManager.RegenerateGameIntegrityCsum();
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    g_EffectManager.SpawnSpecialEffect(12, &this->pos,
                                GetPlayerEffectSlot(this, 3), 1, 0xff4040ff);
#else
    g_EffectManager.SpawnSpecialEffect(12, &this->pos, 3, 1, 0xff4040ff);
#endif
    g_EffectManager.SpawnEffect(6, &this->pos, 16, 0xffffffff);
    // Upstream THOverlay F1 (th07 mMuteki) patches only the immediate written
    // by Player::Die at 0x43EE14: DEAD(2) -> INVULNERABLE(3). Keep all other
    // hit feedback/timers exactly on the vanilla path.
    this->playerState = PracticeRuntime::OverlayInvincible() ? PLAYER_STATE_INVULNERABLE : PLAYER_STATE_DEAD;
    this->invulnerabilityTimer = 0;
    g_SoundPlayer.PlaySoundByIdx(SOUND_PICHUN, 0);

    // touch controls are a bit more tricky to deathbomb since your finger doesn't rest on a key
    // that you can physically actuate the moment you need to deathbomb, so because im just such
    // a nice person there's a 5 frame leniency for touch users (ONLY FOR IF YOU BOMBED WITH
    // TOUCH!!!!!)
    const bool touchUsed = PlayerUsedTouch(this);
    this->respawnTimer = this->shooterData->initialRespawnTimer +
                         (touchUsed ? Touch::DEATHBOMB_TOLERANCE : 0);
}

i32 Player::HandlePlayerInputs()
{
    f32 angleStep;
    f32 targetOffsetX;
    f32 targetOffsetY;
    f32 t;
    f32 optionOffsetX;
    f32 optionOffsetY;
    f32 horizontalSpeed;
    f32 verticalSpeed;

    f32 touchDx;
    f32 touchDy;
    f32 joystickX;
    f32 joystickY;
    bool touchFocus;
    bool sampledReplayTouch = false;
    bool touchUnlimited = false;
    const bool replayPlayback = g_GameManager.replay != 0;

    horizontalSpeed = 0.0f;
    verticalSpeed = 0.0f;
    this->playerDirection = MOVEMENT_NONE;

    if (IS_PRESSED_PLAYER(this, TH_BUTTON_UP))
    {
        this->playerDirection = MOVEMENT_UP;
        if (IS_PRESSED_PLAYER(this, TH_BUTTON_LEFT))
        {
            this->playerDirection = MOVEMENT_UP_LEFT;
        }
        if (IS_PRESSED_PLAYER(this, TH_BUTTON_RIGHT))
        {
            this->playerDirection = MOVEMENT_UP_RIGHT;
        }
    }
    else if (IS_PRESSED_PLAYER(this, TH_BUTTON_DOWN))
    {
        this->playerDirection = MOVEMENT_DOWN;
        if (IS_PRESSED_PLAYER(this, TH_BUTTON_LEFT))
        {
            this->playerDirection = MOVEMENT_DOWN_LEFT;
        }
        if (IS_PRESSED_PLAYER(this, TH_BUTTON_RIGHT))
        {
            this->playerDirection = MOVEMENT_DOWN_RIGHT;
        }
    }
    else
    {
        if (IS_PRESSED_PLAYER(this, TH_BUTTON_LEFT))
        {
            this->playerDirection = MOVEMENT_LEFT;
        }
        if (IS_PRESSED_PLAYER(this, TH_BUTTON_RIGHT))
        {
            this->playerDirection = MOVEMENT_RIGHT;
        }
    }

    if (IS_PRESSED_PLAYER(this, TH_BUTTON_FOCUS))
    {
        this->isFocus = 1;
        switch (this->playerDirection)
        {
        case MOVEMENT_RIGHT:
            horizontalSpeed = this->shooterData->speedFocus;
            break;
        case MOVEMENT_LEFT:
            horizontalSpeed = -this->shooterData->speedFocus;
            break;
        case MOVEMENT_UP:
            verticalSpeed = -this->shooterData->speedFocus;
            break;
        case MOVEMENT_DOWN:
            verticalSpeed = this->shooterData->speedFocus;
            break;
        case MOVEMENT_UP_LEFT:
            horizontalSpeed = -this->shooterData->speedDiagonalFocus;
            verticalSpeed = horizontalSpeed;
            break;
        case MOVEMENT_DOWN_LEFT:
            verticalSpeed = this->shooterData->speedDiagonalFocus;
            horizontalSpeed = -verticalSpeed;
            break;
        case MOVEMENT_UP_RIGHT:
            horizontalSpeed = this->shooterData->speedDiagonalFocus;
            verticalSpeed = -horizontalSpeed;
            break;
        case MOVEMENT_DOWN_RIGHT:
            horizontalSpeed = this->shooterData->speedDiagonalFocus;
            verticalSpeed = horizontalSpeed;
            break;
        case MOVEMENT_NONE:
            break;
        }
    }
    else
    {
        this->isFocus = FALSE;
        switch (this->playerDirection)
        {
        case MOVEMENT_RIGHT:
            horizontalSpeed = this->shooterData->speed;
            break;
        case MOVEMENT_LEFT:
            horizontalSpeed = -this->shooterData->speed;
            break;
        case MOVEMENT_UP:
            verticalSpeed = -this->shooterData->speed;
            break;
        case MOVEMENT_DOWN:
            verticalSpeed = this->shooterData->speed;
            break;
        case MOVEMENT_UP_LEFT:
            horizontalSpeed = -this->shooterData->speedDiagonal;
            verticalSpeed = horizontalSpeed;
            break;
        case MOVEMENT_DOWN_LEFT:
            verticalSpeed = this->shooterData->speedDiagonal;
            horizontalSpeed = -verticalSpeed;
            break;
        case MOVEMENT_UP_RIGHT:
            horizontalSpeed = this->shooterData->speedDiagonal;
            verticalSpeed = -horizontalSpeed;
            break;
        case MOVEMENT_DOWN_RIGHT:
            horizontalSpeed = this->shooterData->speedDiagonal;
            verticalSpeed = horizontalSpeed;
            break;
        case MOVEMENT_NONE:
            break;
        }
    }

    const bool speculative =
#ifdef TH_ENABLE_NETPLAY
        Netplay::SideEffects::IsSpeculative();
#else
        false;
#endif
    if (!speculative)
        ReplayExtension::BeginInputFrame();
    if (replayPlayback && ReplayExtension::GetPlaybackJoystick(&joystickX, &joystickY))
    {
        const f32 maxSpeed = this->isFocus ? this->shooterData->speedFocus : this->shooterData->speed;
        horizontalSpeed = joystickX * maxSpeed;
        verticalSpeed = joystickY * maxSpeed;
        this->playerDirection = MOVEMENT_NONE;
    }
#ifdef TH_ENABLE_NETPLAY
    else if (replayPlayback && ReplayExtension::MultiplayerPlaybackActive() &&
             Netplay::Input::ReplayJoystick(this->initParam, &joystickX, &joystickY))
    {
        const f32 maxSpeed = this->isFocus ? this->shooterData->speedFocus : this->shooterData->speed;
        horizontalSpeed = joystickX * maxSpeed;
        verticalSpeed = joystickY * maxSpeed;
        this->playerDirection = MOVEMENT_NONE;
    }
#endif
    else if (!replayPlayback &&
#ifdef TH_ENABLE_NETPLAY
             (Netplay::Input::ReplayJoystick(this->initParam, &joystickX, &joystickY) ||
#endif
              (
#ifdef TH_ENABLE_NETPLAY
               !Netplay::Input::PlayerButtonOverridesActive() &&
#endif
               CanSampleRawTouchForPlayer(this) &&
               Touch::GetFreeJoystickVector(&joystickX, &joystickY))
#ifdef TH_ENABLE_NETPLAY
             )
#endif
    )
    {
#ifdef TH_ENABLE_NETPLAY
        if (!Netplay::Input::ReplayOverrideActive())
            Netplay::Input::CaptureJoystick(joystickX, joystickY);
#endif
        if (!speculative)
            ReplayExtension::CaptureJoystick(joystickX, joystickY);
        const f32 maxSpeed = this->isFocus ? this->shooterData->speedFocus : this->shooterData->speed;
        horizontalSpeed = joystickX * maxSpeed;
        verticalSpeed = joystickY * maxSpeed;
        this->playerDirection = MOVEMENT_NONE;
    }
    else
    {
        bool sampledNetplayTouch = false;
        const bool haveDirectTouch =
            (replayPlayback &&
             (sampledReplayTouch =
                  ReplayExtension::GetPlaybackDirectTouch(&touchDx, &touchDy, &touchUnlimited))) ||
#ifdef TH_ENABLE_NETPLAY
            (replayPlayback && ReplayExtension::MultiplayerPlaybackActive() &&
             (sampledNetplayTouch = Netplay::Input::ReplayDirectTouch(
                  this->initParam, &touchDx, &touchDy, &touchUnlimited))) ||
#endif
            (!replayPlayback &&
#ifdef TH_ENABLE_NETPLAY
             ((sampledNetplayTouch =
                   Netplay::Input::ReplayDirectTouch(
                       this->initParam, &touchDx, &touchDy, &touchUnlimited)) ||
#endif
              (
#ifdef TH_ENABLE_NETPLAY
                  !Netplay::Input::PlayerButtonOverridesActive() &&
#endif
                  CanSampleRawTouchForPlayer(this) &&
                  Touch::GetPlayerDelta(&touchDx, &touchDy))
#ifdef TH_ENABLE_NETPLAY
             )
#endif
            );
        if (haveDirectTouch)
        {
            if (!sampledReplayTouch && !sampledNetplayTouch)
            {
                touchUnlimited = Touch::IsUnlimited();
#ifdef TH_ENABLE_NETPLAY
                Netplay::Input::CaptureDirectTouch(touchDx, touchDy, touchUnlimited);
#endif
            }
            // Sample the touch owner's input on the same fixed game tick as
            // vanilla replay input. Raw touch events remain a separate stream
            // for semantics and the replay touch-position overlay.
            if (!speculative && !sampledReplayTouch)
                ReplayExtension::CaptureDirectTouch(touchDx, touchDy, touchUnlimited);

            const bool sampledLogicalTouch = sampledReplayTouch || sampledNetplayTouch;
            const bool incrementalLogicalTouch =
#ifdef TH_ENABLE_NETPLAY
                sampledNetplayTouch && Netplay::Input::UsesIncrementalDirectTouch(this->initParam);
#else
                false;
#endif
            const bool consumeSynchronizedLocalTouch =
#ifdef TH_ENABLE_NETPLAY
                sampledNetplayTouch && !incrementalLogicalTouch && !speculative &&
                this->initParam == MultiplayerGameplay::GetLocalPlayerSlot();
#else
                false;
#endif
            f32 focusRatio = 1.0f;
            if (!touchUnlimited && this->isFocus && this->shooterData &&
                this->shooterData->speed != 0.0f)
            {
                focusRatio = this->shooterData->speedFocus / this->shooterData->speed;
            }

            f32 reqGameDx = touchDx * focusRatio;
            f32 reqGameDy = touchDy * focusRatio;

        f32 minX = g_GameManager.playerMovementAreaTopLeftPos.x;
        f32 maxX =
            g_GameManager.playerMovementAreaTopLeftPos.x + g_GameManager.playerMovementAreaSize.x;
        f32 minY = g_GameManager.playerMovementAreaTopLeftPos.y;
        f32 maxY =
            g_GameManager.playerMovementAreaTopLeftPos.y + g_GameManager.playerMovementAreaSize.y;

        f32 targetX = this->pos.x + reqGameDx;
        f32 targetY = this->pos.y + reqGameDy;

        if (targetX < minX)
        {
            reqGameDx = minX - this->pos.x;
        }
        else if (targetX > maxX)
        {
            reqGameDx = maxX - this->pos.x;
        }

        if (targetY < minY)
        {
            reqGameDy = minY - this->pos.y;
        }
        else if (targetY > maxY)
        {
            reqGameDy = maxY - this->pos.y;
        }

            if (focusRatio != 0.0f &&
                (!sampledLogicalTouch || consumeSynchronizedLocalTouch))
            {
                Touch::SetPlayerDelta(reqGameDx / focusRatio, reqGameDy / focusRatio);
            }
#ifdef TH_ENABLE_NETPLAY
            if (focusRatio != 0.0f && incrementalLogicalTouch)
                Netplay::Input::SetDirectTouchRemainder(this->initParam,
                    reqGameDx / focusRatio, reqGameDy / focusRatio);
#endif

        f32 hx = this->horizontalMovementSpeedMultiplierDuringBomb *
                 g_Supervisor.effectiveFramerateMultiplier;
        f32 vy = this->verticalMovementSpeedMultiplierDuringBomb *
                 g_Supervisor.effectiveFramerateMultiplier;

        f32 requestedHorizontalSpeed = hx != 0.0f ? reqGameDx / hx : 0.0f;
        f32 requestedVerticalSpeed = vy != 0.0f ? reqGameDy / vy : 0.0f;

        f32 currentSpeedSq = requestedHorizontalSpeed * requestedHorizontalSpeed +
                             requestedVerticalSpeed * requestedVerticalSpeed;

        f32 maxSpeed = this->isFocus ? this->shooterData->speedFocus : this->shooterData->speed;
        if (touchUnlimited)
        {
            maxSpeed = sqrtf(currentSpeedSq);
        }

        if (!touchUnlimited && currentSpeedSq > maxSpeed * maxSpeed && currentSpeedSq > 0.0f)
        {
            f32 currentSpeed = sqrtf(currentSpeedSq);
            horizontalSpeed = (requestedHorizontalSpeed / currentSpeed) * maxSpeed;
            verticalSpeed = (requestedVerticalSpeed / currentSpeed) * maxSpeed;
        }
        else
        {
            horizontalSpeed = requestedHorizontalSpeed;
            verticalSpeed = requestedVerticalSpeed;
        }

        f32 consumedGameDx = 0.0f;
        f32 consumedGameDy = 0.0f;

        if (hx != 0.0f)
        {
            consumedGameDx = horizontalSpeed * hx;
        }
        if (vy != 0.0f)
        {
            consumedGameDy = verticalSpeed * vy;
        }

            if (focusRatio != 0.0f &&
                (!sampledLogicalTouch || consumeSynchronizedLocalTouch))
            {
                if (!touchUnlimited && currentSpeedSq > maxSpeed * maxSpeed &&
                    currentSpeedSq > 0.0f)
                {
                    f32 consumeX = (hx != 0.0f) ? consumedGameDx / focusRatio : touchDx;
                    f32 consumeY = (vy != 0.0f) ? consumedGameDy / focusRatio : touchDy;
                    Touch::ConsumePlayerDelta(consumeX, consumeY);
                }
                else
                {
                    Touch::SetPlayerDelta(0.0f, 0.0f);
                }
            }

#ifdef TH_ENABLE_NETPLAY
            if (focusRatio != 0.0f && incrementalLogicalTouch)
            {
                if (!touchUnlimited && currentSpeedSq > maxSpeed * maxSpeed && currentSpeedSq > 0.0f)
                    Netplay::Input::ConsumeDirectTouchRemainder(this->initParam,
                        hx != 0.0f ? consumedGameDx / focusRatio : touchDx,
                        vy != 0.0f ? consumedGameDy / focusRatio : touchDy);
                else
                    Netplay::Input::SetDirectTouchRemainder(this->initParam, 0.0f, 0.0f);
            }
#endif
            this->playerDirection = MOVEMENT_NONE;

        // this is actually pretty useless since playerdirection handling is above which we
        // completely ignored in the touch handling path. the actual part that handles what
        // direction the playersprite faces is below
            const f32 dirDeadzone = 0.01f;
            bool left = touchDx < -dirDeadzone;
            bool right = touchDx > dirDeadzone;
            bool up = touchDy < -dirDeadzone;
            bool down = touchDy > dirDeadzone;

            if (up)
            {
                this->playerDirection = MOVEMENT_UP;
                if (left)
                {
                    this->playerDirection = MOVEMENT_UP_LEFT;
                }
                else if (right)
                {
                    this->playerDirection = MOVEMENT_UP_RIGHT;
                }
            }
            else if (down)
            {
                this->playerDirection = MOVEMENT_DOWN;
                if (left)
                {
                    this->playerDirection = MOVEMENT_DOWN_LEFT;
                }
                else if (right)
                {
                    this->playerDirection = MOVEMENT_DOWN_RIGHT;
                }
            }
            else if (left)
            {
                this->playerDirection = MOVEMENT_LEFT;
            }
            else if (right)
            {
                this->playerDirection = MOVEMENT_RIGHT;
            }
        }
    }

    if (horizontalSpeed < 0.0f && this->previousHorizontalSpeed >= 0.0f)
    {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        g_AnmManager->SetAnmIdxAndExecuteScript(
            &this->playerSprite, GetPlayerAnmScript(this, 1025));
#else
        g_AnmManager->SetAnmIdxAndExecuteScript(&this->playerSprite, 1025);
#endif
    }
    else if (horizontalSpeed == 0.0f && this->previousHorizontalSpeed < 0.0f)
    {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        g_AnmManager->SetAnmIdxAndExecuteScript(
            &this->playerSprite, GetPlayerAnmScript(this, 1026));
#else
        g_AnmManager->SetAnmIdxAndExecuteScript(&this->playerSprite, 1026);
#endif
    }

    if (horizontalSpeed > 0.0f && this->previousHorizontalSpeed <= 0.0f)
    {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        g_AnmManager->SetAnmIdxAndExecuteScript(
            &this->playerSprite, GetPlayerAnmScript(this, 1027));
#else
        g_AnmManager->SetAnmIdxAndExecuteScript(&this->playerSprite, 1027);
#endif
    }
    else if (horizontalSpeed == 0.0f && this->previousHorizontalSpeed > 0.0f)
    {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        g_AnmManager->SetAnmIdxAndExecuteScript(
            &this->playerSprite, GetPlayerAnmScript(this, 1028));
#else
        g_AnmManager->SetAnmIdxAndExecuteScript(&this->playerSprite, 1028);
#endif
    }

    this->previousHorizontalSpeed = horizontalSpeed;
    this->previousVerticalSpeed = verticalSpeed;
    this->velocity.x = horizontalSpeed * this->horizontalMovementSpeedMultiplierDuringBomb *
                       g_Supervisor.effectiveFramerateMultiplier;
    this->velocity.y = verticalSpeed * this->verticalMovementSpeedMultiplierDuringBomb *
                       g_Supervisor.effectiveFramerateMultiplier;
    *GetPosX() += this->velocity.x;
    *GetPosY() += this->velocity.y;

    if (this->pos.x < g_GameManager.playerMovementAreaTopLeftPos.x)
    {
        this->pos.x = g_GameManager.playerMovementAreaTopLeftPos.x;
    }
    else if (this->pos.x >
             g_GameManager.playerMovementAreaTopLeftPos.x + g_GameManager.playerMovementAreaSize.x)
    {
        this->pos.x =
            g_GameManager.playerMovementAreaTopLeftPos.x + g_GameManager.playerMovementAreaSize.x;
    }

    if (this->pos.y < g_GameManager.playerMovementAreaTopLeftPos.y)
    {
        this->pos.y = g_GameManager.playerMovementAreaTopLeftPos.y;
    }
    else if (this->pos.y >
             g_GameManager.playerMovementAreaTopLeftPos.y + g_GameManager.playerMovementAreaSize.y)
    {
        this->pos.y =
            g_GameManager.playerMovementAreaTopLeftPos.y + g_GameManager.playerMovementAreaSize.y;
    }

    this->hitboxTopLeft = this->pos - this->hitboxSize;
    this->hitboxBottomRight = this->pos + this->hitboxSize;
    this->grazeTopLeft = this->pos - this->grazeSize;
    this->grazeBottomRight = this->pos + this->grazeSize;
    this->grabItemTopLeft = this->pos - this->grabItemSize;
    this->grabItemBottomRight = this->pos + this->grabItemSize;
    this->optionsPosition[0] = this->pos;
    this->optionsPosition[1] = this->pos;
    optionOffsetX = optionOffsetY = 0.0f;

    bool sakuyaB = g_GameManager.character == CHAR_SAKUYA &&
                   g_GameManager.shotType == 1;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (MultiplayerGameplay::IsMultiplayer())
    {
        sakuyaB = MultiplayerGameplay::GetPlayerCharacter(this->initParam) == CHAR_SAKUYA &&
                  MultiplayerGameplay::GetPlayerShot(this->initParam) == 1;
    }
#endif
    if (!sakuyaB)
    {
        switch (this->optionState)
        {
        case OPTION_HIDDEN:
            this->focusMovementTimer = 0;
            break;
        case OPTION_UNFOCUSED:
            optionOffsetX = 24.0f;
            this->focusMovementTimer = 0;
            if (this->isFocus)
            {
                this->optionState = OPTION_FOCUSING;
                this->focusEffect = g_EffectManager.SpawnSpecialEffect(
                    24, &this->pos,
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
                    GetPlayerEffectSlot(this, 2),
#else
                    2,
#endif
                    1, 0xffffffff);
            }
            else
            {
                break;
            }
        CASE_OPTION_FOCUSING:
        case OPTION_FOCUSING:
            this->focusMovementTimer++;
            t = this->focusMovementTimer.AsFloat() / 8.0f;
            optionOffsetY = -32.0f + (1.0f - t) * 32.0f;
            t *= t;
            optionOffsetX = -16.0f * t + 24.0f;
            if (this->focusMovementTimer >= 8)
            {
                this->optionState = OPTION_FOCUSED;
            }
            if (!this->isFocus)
            {
                this->optionState = OPTION_UNFOCUSING;
                this->focusMovementTimer = 8 - this->focusMovementTimer.GetCurrent();
                if (this->focusEffect)
                {
                    this->focusEffect->vm.SetInterrupt(1);
                }
                goto CASE_OPTION_UNFOCUSING;
            }
            break;
        case OPTION_FOCUSED:
            optionOffsetX = 8.0f;
            optionOffsetY = -32.0f;
            this->focusMovementTimer = 0;
            if (!this->isFocus)
            {
                this->optionState = OPTION_UNFOCUSING;
                if (this->focusEffect)
                {
                    this->focusEffect->vm.SetInterrupt(1);
                }
                goto CASE_OPTION_UNFOCUSING;
            }
            break;
        CASE_OPTION_UNFOCUSING:
        case OPTION_UNFOCUSING:
            this->focusMovementTimer++;
            t = this->focusMovementTimer.AsFloat() / 8.0f;
            optionOffsetY = -32.0f + 32.0f * t;
            t *= t;
            t = 1.0f - t;
            optionOffsetX = -16.0f * t + 24.0f;
            if (this->focusMovementTimer >= 8)
            {
                this->optionState = OPTION_UNFOCUSED;
            }
            if (this->isFocus)
            {
                this->optionState = OPTION_FOCUSING;
                this->focusMovementTimer = 8 - this->focusMovementTimer.GetCurrent();
                this->focusEffect = g_EffectManager.SpawnSpecialEffect(
                    24, &this->pos,
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
                    GetPlayerEffectSlot(this, 2),
#else
                    2,
#endif
                    1, 0xffffffff);
                goto CASE_OPTION_FOCUSING;
            }
        }
        this->optionsPosition[0].x -= optionOffsetX;
        this->optionsPosition[1].x += optionOffsetX;
        this->optionsPosition[0].y += optionOffsetY;
        this->optionsPosition[1].y += optionOffsetY;
    }
    else
    {
        switch (this->optionState)
        {
        case OPTION_HIDDEN:
            this->focusMovementTimer = 0;
            break;
        case OPTION_UNFOCUSED:
            optionOffsetX = cosf(this->optionAngle + 1.5707964f) * 24.0f;
            optionOffsetY = sinf(this->optionAngle + 1.5707964f) * 24.0f;
            this->focusMovementTimer = 0;
            if (this->isFocus)
            {
                this->optionState = OPTION_FOCUSING;
                this->focusEffect = g_EffectManager.SpawnSpecialEffect(
                    24, &this->pos,
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
                    GetPlayerEffectSlot(this, 2),
#else
                    2,
#endif
                    1, 0xffffffff);
                goto CASE_OPTION_FOCUSING_2;
            }
            this->optionsPosition[0].x -= optionOffsetX;
            this->optionsPosition[1].x += optionOffsetX;
            this->optionsPosition[0].y -= optionOffsetY;
            this->optionsPosition[1].y += optionOffsetY;
            break;
        CASE_OPTION_FOCUSING_2:
        case OPTION_FOCUSING:
            if (!this->isFocus)
            {
                this->optionState = OPTION_UNFOCUSING;
                this->focusMovementTimer = 8 - this->focusMovementTimer.GetCurrent();
                if (this->focusEffect)
                {
                    this->focusEffect->vm.SetInterrupt(1);
                }
                goto CASE_OPTION_UNFOCUSING_2;
            }
            this->focusMovementTimer++;
            t = this->focusMovementTimer.AsFloat() / 8.0f;
            optionOffsetX = cosf(this->optionAngle + ZUN_PI / 2.0f) * 24.0f;
            optionOffsetY = sinf(this->optionAngle + ZUN_PI / 2.0f) * 24.0f;
            targetOffsetX = cosf(this->optionAngle + ZUN_PI / 14.0f) * 24.0f;
            targetOffsetY = sinf(this->optionAngle + ZUN_PI / 14.0f) * 24.0f;
            targetOffsetX = (targetOffsetX - optionOffsetX) * t + optionOffsetX;
            targetOffsetY = (targetOffsetY - optionOffsetY) * t + optionOffsetY;
            this->optionsPosition[1].x += targetOffsetX;
            this->optionsPosition[1].y += targetOffsetY;
            targetOffsetX = cosf(this->optionAngle - ZUN_PI / 14.0f) * 24.0f;
            targetOffsetY = sinf(this->optionAngle - ZUN_PI / 14.0f) * 24.0f;
            targetOffsetX = (targetOffsetX + optionOffsetX) * t - optionOffsetX;
            targetOffsetY = (targetOffsetY + optionOffsetY) * t - optionOffsetY;
            if (this->focusMovementTimer >= 8)
            {
                this->optionState = OPTION_FOCUSED;
            }
            this->optionsPosition[0].x += targetOffsetX;
            this->optionsPosition[0].y += targetOffsetY;
            break;
        case OPTION_FOCUSED:
            this->focusMovementTimer = 0;
            if (!this->isFocus)
            {
                this->optionState = OPTION_UNFOCUSING;
                if (this->focusEffect)
                {
                    this->focusEffect->vm.SetInterrupt(1);
                }
                goto CASE_OPTION_UNFOCUSING_2;
            }
            targetOffsetX = cosf(this->optionAngle + ZUN_PI / 14.0f) * 24.0f;
            targetOffsetY = sinf(this->optionAngle + ZUN_PI / 14.0f) * 24.0f;
            this->optionsPosition[1].x += targetOffsetX;
            this->optionsPosition[1].y += targetOffsetY;
            targetOffsetX = cosf(this->optionAngle - ZUN_PI / 14.0f) * 24.0f;
            targetOffsetY = sinf(this->optionAngle - ZUN_PI / 14.0f) * 24.0f;
            this->optionsPosition[0].x += targetOffsetX;
            this->optionsPosition[0].y += targetOffsetY;
            break;
        CASE_OPTION_UNFOCUSING_2:
        case OPTION_UNFOCUSING:
            if (this->isFocus)
            {
                this->optionState = OPTION_FOCUSING;
                this->focusMovementTimer = 8 - this->focusMovementTimer.GetCurrent();
                this->focusEffect = g_EffectManager.SpawnSpecialEffect(
                    24, &this->pos,
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
                    GetPlayerEffectSlot(this, 2),
#else
                    2,
#endif
                    1, 0xffffffff);
                goto CASE_OPTION_FOCUSING_2;
            }
            this->focusMovementTimer++;
            t = 1.0f - this->focusMovementTimer.AsFloat() / 8.0f;
            optionOffsetX = cosf(this->optionAngle + 1.5707964f) * 24.0f;
            optionOffsetY = sinf(this->optionAngle + 1.5707964f) * 24.0f;
            targetOffsetX = cosf(this->optionAngle + 0.22439948f) * 24.0f;
            targetOffsetY = sinf(this->optionAngle + 0.22439948f) * 24.0f;
            targetOffsetX = (targetOffsetX - optionOffsetX) * t + optionOffsetX;
            targetOffsetY = (targetOffsetY - optionOffsetY) * t + optionOffsetY;
            this->optionsPosition[1].x += targetOffsetX;
            this->optionsPosition[1].y += targetOffsetY;
            targetOffsetX = cosf(this->optionAngle - 0.22439948f) * 24.0f;
            targetOffsetY = sinf(this->optionAngle - 0.22439948f) * 24.0f;
            targetOffsetX = (targetOffsetX + optionOffsetX) * t - optionOffsetX;
            targetOffsetY = (targetOffsetY + optionOffsetY) * t - optionOffsetY;
            if (this->focusMovementTimer >= 8)
            {
                this->optionState = OPTION_UNFOCUSED;
            }
            this->optionsPosition[0].x += targetOffsetX;
            this->optionsPosition[0].y += targetOffsetY;
            break;
        }
    }
    if (IS_PRESSED_PLAYER(this, TH_BUTTON_SHOOT) && !g_Gui.HasCurrentMsgIdx())
    {
        if (!g_GameManager.CheckGameIntegrity())
        {
            StartFireBulletTimer();
        }
        if (!IS_PRESSED_PLAYER(this, TH_BUTTON_FOCUS))
        {
            if (this->velocity.x != 0.0f)
            {
                angleStep = -(this->velocity.x / 4.0f) * ZUN_PI / 5.0f / 10.0f;
                this->optionAngle -= angleStep;
                if (this->optionAngle < -(7.0f * ZUN_PI / 10.0f))
                {
                    this->optionAngle = -(7.0f * ZUN_PI / 10.0f);
                }
                else if (this->optionAngle > -ZUN_3PI / 10.0f)
                {
                    this->optionAngle = -ZUN_3PI / 10.0f;
                }
            }
            else
            {
                if (fabsf(this->optionAngle - (-ZUN_PI / 2.0f)) > ZUN_PI / 100.0f)
                {
                    angleStep = this->optionAngle < -ZUN_PI / 2.0f
                                    ? ZUN_PI / 50.0f * g_Supervisor.effectiveFramerateMultiplier
                                    : -ZUN_PI / 50.0f * g_Supervisor.effectiveFramerateMultiplier;
                    this->optionAngle += angleStep;
                }
                else
                {
                    this->optionAngle = -ZUN_PI / 2.0f;
                }
            }
        }
    }
    return 0;
}

void Player::UpdateBombProjectiles()
{
    BombClearBox *bomb;
    i32 i;

    for (i = 0; i < ARRAY_SIZE_SIGNED(this->bombDamageBoxes); i++)
    {
        this->bombDamageBoxes[i].size.x = 0.0f;
    }
    bomb = this->bombClearBoxes;
    for (i = 0; i < ARRAY_SIZE_SIGNED(this->bombClearBoxes); i++, bomb++)
    {
        if (bomb->lifetime <= 0)
        {
            bomb->radius = 0.0f;
            bomb->size.x = 0.0f;
        }
        else
        {
            bomb->lifetime--;
            bomb->radius += bomb->radiusGrowth;
        }
    }
    this->dirtyBombBoxes = true;
}

void Player::UpdateBorderAndBombState()
{
    if (this->hasBorder != BORDER_NONE && !this->bombInfo.isInUse &&
        IS_PRESSED_PLAYER(this, TH_BUTTON_BOMB))
    {
        BreakBorder();
        this->isBombing = 0;
        g_ItemManager.RemoveAllItems();
    }
    else
    {
        if (this->hasBorder == BORDER_READY)
        {
            ActivateBorder();
        }
        if (this->borderInvulnerabilityTime != 0)
        {
            this->borderInvulnerabilityTime--;
        }
        if (this->bombInfo.isInUse)
        {
            if (this->bombInfo.bombTimer.HasTicked())
            {
                PlayerBombInfo::SubtractCherryDrain(this->bombInfo.cherryDrain);
                g_Gui.pointDisplayUpdateFrames = 2;
            }
            if (!this->bombInfo.isFocus)
            {
                this->bombInfo.bombCalc(this);
            }
            else
            {
                this->bombInfo.bombFocusCalc(this);
            }
        }
        else
        {
            // THOverlay F6 does not call the bomb routine directly. Upstream
            // patches the bomb test at 0x440B8E from g_CurFrameGameInput
            // (0x4B9E50) to g_LastFrameRawInput (0x4B9E54). UpdateDeath below
            // synthesizes Bomb into the current raw word, so it is consumed by
            // this normal vanilla bomb path on the following tick.
            const bool bombPressed = this->initParam == 0 && PracticeRuntime::OverlayAutoBomb()
                                         ? ((g_LastFrameRawInput & TH_BUTTON_BOMB) != 0)
                                         : IS_PRESSED_PLAYER(this, TH_BUTTON_BOMB);
            i32 bombsAvailable = (i32)g_GameManager.globals->bombsRemaining;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
            if (MultiplayerGameplay::IsMultiplayer())
                bombsAvailable = GetPlayerBombs(this->initParam);
#endif
            if (!g_GameManager.CheckGameIntegrity() && !g_Gui.HasCurrentMsgIdx() &&
                this->respawnTimer != 0 && 0 < bombsAvailable &&
                this->borderInvulnerabilityTime == 0 && bombPressed)
            {
                if (this->playerState == PLAYER_STATE_DEAD)
                {
                    const bool touchUsed = PlayerUsedTouch(this);
                    const bool touchBomb = PlayerUsedTouchToBomb(this);
                    i32 minRequiredTimer = (touchUsed && !touchBomb)
                                               ? Touch::DEATHBOMB_TOLERANCE
                                               : 0;
                    if (this->respawnTimer <= minRequiredTimer)
                    {
                        return;
                    }
                }
                g_ReplayManager->replayEventFlags |= 1;
                g_GameManager.AddBombsUsed(1);
                // THOverlay F3 patches the -1 immediate at 0x440BC7 to 0.
                if (!PracticeRuntime::OverlayInfiniteBombs())
                {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
                    if (MultiplayerGameplay::IsMultiplayer())
                        AddPlayerBombs(this->initParam, -1);
                    else
#endif
                        g_GameManager.AddBombsRemaining(-1);
                }
                g_Gui.bombDisplayUpdateFrames = 2;
#ifdef TH_ENABLE_NETPLAY
                Netplay::Th07Rollback::TouchPlayerBombInfo(&this->bombInfo);
#endif
                this->bombInfo.isFocus = (i32)this->isFocus;
                this->bombInfo.isInUse = 1;
                this->isBombing = 1;
                this->bombInfo.bombTimer = 0;
                this->bombInfo.bombDuration = 999;
                if (!this->bombInfo.isFocus)
                {
                    this->bombInfo.bombCalc(this);
                }
                else
                {
                    this->bombInfo.bombFocusCalc(this);
                }
                g_EnemyManager.spellcardInfo.captureScore = 0;
                g_EnemyManager.spellcardInfo.isCapturing = 0;
                g_GameManager.DecreaseSubrank(
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
                    MultiplayerGameplay::IsMultiplayer()
                        ? GetMultiplayerRankPenalty(200)
                        : 200
#else
                    200
#endif
                );
                g_EnemyManager.spellcardInfo.usedBomb = g_EnemyManager.spellcardInfo.isActive;
                this->respawnTimer += 6;
                if (this->respawnTimer > this->shooterData->initialRespawnTimer)
                {
                    this->respawnTimer = this->shooterData->initialRespawnTimer;
                }
            }
            else
            {
                this->isBombing = FALSE;
            }
        }
    }
}

i32 Player::UpdateDeath()
{
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (MultiplayerGameplay::IsMultiplayer())
        return UpdateMultiplayerDeath(this);
#endif
    f32 invulnScale;
    i32 cherryPenalty;

    if (this->respawnTimer != 0)
    {
        if (this->hasBorder == BORDER_ACTIVE)
        {
            BreakBorder();
            return 0;
        }
        this->respawnTimer--;
        // Remaining F6 patches replace the vanilla respawnTimer load/sub/store
        // at 0x440D2C..0x440D3D with a direct decrement plus
        //   g_CurFrameRawInput = TH_BUTTON_BOMB (2).
        // The next tick's patched bomb test above reads this through
        // g_LastFrameRawInput, preserving the upstream one-tick ownership.
        if (PracticeRuntime::OverlayAutoBomb())
            g_CurFrameRawInput = TH_BUTTON_BOMB;
        if (this->respawnTimer == 0)
        {
            g_ReplayManager->replayEventFlags |= 4;
            g_GameManager.powerItemCountForScore = 0;
            g_EnemyManager.spellcardInfo.captureScore = 0;
            g_EnemyManager.spellcardInfo.isCapturing = 0;
            g_GameManager.CheckGameIntegrityOnDeath(1);
            if ((i32)g_GameManager.globals->livesRemaining > 0)
            {
                // THOverlay F4 NOPs the currentPower=0 store at 0x440DBF and
                // changes the -16 immediate at 0x440DD3 to 0. Typed portable
                // equivalent: preserve the entire death path but do not alter
                // power while the option is active.
                if (!PracticeRuntime::OverlayInfinitePower())
                {
                    if ((i32)g_GameManager.globals->currentPower <= 16)
                    {
                        g_GameManager.globals->currentPower = 0.0f;
                        g_GameManager.RegenerateGameIntegrityCsum();
                    }
                    else
                    {
                        g_GameManager.AddCurrentPower(-16);
                    }
                }
                g_ItemManager.SpawnItem(&this->pos, ITEM_POWER_BIG, 2);
                g_ItemManager.SpawnItem(&this->pos, ITEM_POWER_SMALL, 2);
                g_ItemManager.SpawnItem(&this->pos, ITEM_POWER_SMALL, 2);
                g_ItemManager.SpawnItem(&this->pos, ITEM_POWER_SMALL, 2);
                g_ItemManager.SpawnItem(&this->pos, ITEM_POWER_SMALL, 2);
                g_ItemManager.SpawnItem(&this->pos, ITEM_POWER_SMALL, 2);
                g_Gui.powerDisplayUpdateFrames = 2;
                cherryPenalty = (f32)(g_GameManager.cherry - g_GameManager.globals->cherryStart) *
                                g_Player.shooterData->cherryPenaltyMultiplier;
                if (g_GameManager.character != CHAR_SAKUYA)
                {
                    if (cherryPenalty > 100000)
                    {
                        cherryPenalty = 100000;
                    }
                }
                else if (cherryPenalty > 60000)
                {
                    cherryPenalty = 60000;
                }
                cherryPenalty -= cherryPenalty % 10;
                g_GameManager.cherry -= cherryPenalty;
                g_Gui.pointDisplayUpdateFrames = 2;
                g_ItemManager.ActivateAllItems();
            }
            else
            {
                if (!PracticeRuntime::OverlayInfinitePower())
                {
                    g_GameManager.globals->currentPower = 0.0f;
                    g_GameManager.RegenerateGameIntegrityCsum();
                }
                g_ItemManager.SpawnItem(&this->pos, ITEM_FULL_POWER, 2);
                g_ItemManager.SpawnItem(&this->pos, ITEM_FULL_POWER, 2);
                g_ItemManager.SpawnItem(&this->pos, ITEM_FULL_POWER, 2);
                g_ItemManager.SpawnItem(&this->pos, ITEM_FULL_POWER, 2);
                g_ItemManager.SpawnItem(&this->pos, ITEM_FULL_POWER, 2);
                g_Gui.powerDisplayUpdateFrames = 2;
            }
            g_GameManager.DecreaseSubrank(1600);
        }
    }
    else
    {
        invulnScale = this->invulnerabilityTimer.AsFloat() / 30.0f;
        this->playerSprite.scale.y = 3.0f * invulnScale + 1.0f;
        this->playerSprite.scale.x = 1.0f - 1.0f * invulnScale;
        this->playerSprite.color.color =
            (u32)(255.0f - this->invulnerabilityTimer.AsFloat() * 255.0f / 30.0f) << 24 | 0xffffff;
        this->playerSprite.blendMode = 1;
        this->previousHorizontalSpeed = 0.0f;
        this->previousVerticalSpeed = 0.0f;
        if (this->invulnerabilityTimer.GetCurrent() >= 30)
        {
            this->playerState = PLAYER_STATE_SPAWNING;
            this->pos.x = g_GameManager.arcadeRegionSize.x / 2.0f;
            this->pos.y = g_GameManager.arcadeRegionSize.y - 64.0f;
            this->pos.z = 0.2f;
            this->prevPos = this->pos;
            this->invulnerabilityTimer = 0;
            this->playerSprite.scale.x = 3.0f;
            this->playerSprite.scale.y = 3.0f;
            g_AnmManager->SetAnmIdxAndExecuteScript(&this->playerSprite, ANM_SCRIPT_PLAYER_IDLE);
            if ((i32)g_GameManager.globals->livesRemaining <= 0)
            {
                g_GameManager.isInRetryMenu = 1;
            }
            else
            {
                // THOverlay F2 patches the -1 immediate at 0x44116B to 0.
                if (!PracticeRuntime::OverlayInfiniteLives())
                    g_GameManager.AddLivesRemaining(-1);
                g_Gui.lifeDisplayUpdateFrames = 2;
                g_GameManager.SetBombsRemainingAndComputeCsum(g_Player.shooterData->initialBombs);
                g_Gui.bombDisplayUpdateFrames = 2;
                return 1;
            }
        }
    }
    return 0;
}

void Player::Respawn()
{
    this->bulletGracePeriod = 60;
    f32 invulnScale = 1.0f - this->invulnerabilityTimer.AsFloat() / 30.0f;
    this->playerSprite.scale.y = 2.0f * invulnScale + 1.0f;
    this->playerSprite.scale.x = 1.0f - 1.0f * invulnScale;
    this->playerSprite.blendMode = 1;
    this->verticalMovementSpeedMultiplierDuringBomb = 1.0f;
    this->horizontalMovementSpeedMultiplierDuringBomb = 1.0f;
    this->playerSprite.color.color =
        this->invulnerabilityTimer.GetCurrent() * 255 / 30 << 24 | 0xffffff;
    this->respawnTimer = 0;
    if (this->invulnerabilityTimer.GetCurrent() >= 30)
    {
        this->playerState = PLAYER_STATE_INVULNERABLE;
        this->playerSprite.scale.x = 1.0f;
        this->playerSprite.scale.y = 1.0f;
        this->playerSprite.color.color = 0xffffffff;
        this->playerSprite.blendMode = 0;
        this->invulnerabilityTimer = 240;
        this->respawnTimer = this->shooterData->initialRespawnTimer;
    }
}

void Player::UpdateState()
{
    ZunColor color;

    if (this->bulletGracePeriod != 0)
    {
        this->bulletGracePeriod--;
        g_BulletManager.RemoveAllBullets(0);
    }
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (MultiplayerGameplay::IsMultiplayer() &&
        this->playerState == PLAYER_STATE_SPIRIT)
    {
        UpdateSpiritState(this);
        return;
    }
#endif
    if (this->playerState == PLAYER_STATE_INVULNERABLE)
    {
        if (this->effect)
        {
            this->effect->pos = this->pos;
        }
        this->invulnerabilityTimer--;
        if (this->invulnerabilityTimer.GetCurrent() <= 0)
        {
            if (this->effect)
            {
                this->effect->inUseFlag = 0;
                this->effect = NULL;
            }
            this->playerState = PLAYER_STATE_ALIVE;
            this->invulnerabilityTimer = 0;
            this->playerSprite.color.color = 0xffffffff;
        }
        else
        {
            if (this->invulnerabilityTimer.GetCurrent() % 8 < 2)
            {
                this->playerSprite.color.color = 0xff404040;
            }
            else
            {
                this->playerSprite.color.color = 0xffffffff;
            }
        }
    }
    else if (this->playerState == PLAYER_STATE_BORDER)
    {
        if (this->borderEffect)
        {
            this->borderEffect->pos = this->pos;
        }
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        Player *sharedBorderOwner = MultiplayerGameplay::IsMultiplayer()
                                        ? GetSharedBorderOwner()
                                        : this;
        if (!sharedBorderOwner)
            sharedBorderOwner = this;
        if (sharedBorderOwner == this)
#endif
        {
            const i32 threshold =
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
                MultiplayerGameplay::IsMultiplayer() ? GetSharedBorderThreshold() : 50000;
#else
                50000;
#endif
            g_GameManager.cherryPlus =
                this->invulnerabilityTimer.GetCurrent() * threshold /
                this->borderTimer.GetCurrent();
            if (g_GameManager.cherryPlus < 0)
                g_GameManager.cherryPlus = 0;
            g_GameManager.cherryPlus += g_GameManager.globals->cherryStart;
        }
        this->invulnerabilityTimer--;
        if (this->invulnerabilityTimer.GetCurrent() <= 0)
        {
            this->playerSprite.color.color = 0xffffffff;
            BreakBorderNaturally();
        }
        else
        {
            if (this->invulnerabilityTimer.GetCurrent() % 4 < 2)
            {
                this->playerSprite.color.color = 0xffff0000;
            }
            else
            {
                this->playerSprite.color.color = 0xffffffff;
            }
            color.bytes.a = 128;
            if (this->invulnerabilityTimer >= 510)
            {
                color.bytes.r = color.bytes.g = color.bytes.b =
                    128 - (540 - this->invulnerabilityTimer.GetCurrent()) * 80 / 30;
            }
            else if (this->invulnerabilityTimer < 30)
            {
                color.bytes.r = color.bytes.g = color.bytes.b =
                    128 - this->invulnerabilityTimer.GetCurrent() * 80 / 30;
            }
            else
            {
                color.bytes.r = color.bytes.g = color.bytes.b = 48;
            }
            g_Stage.SmoothBlendColor(color);
        }
    }
    else
    {
        this->invulnerabilityTimer++;
    }
}

void Player::BreakBorderNaturally()
{
    i32 cherryDiff;

#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (MultiplayerGameplay::IsMultiplayer() && !g_SharedBorderTransition &&
        GetActivePlayerCount() > 1 && IsSharedBorderActive())
    {
        g_SharedBorderTransition = true;
        this->BreakBorderNaturally();
        for (u8 playerId = 0; playerId < TH07_MULTI_MAX_PLAYERS; ++playerId)
        {
            Player *other = &g_Players[playerId];
            if (other != this && IsPlayerSlotActive(playerId) &&
                (other->hasBorder == BORDER_ACTIVE ||
                 other->playerState == PLAYER_STATE_BORDER))
                ClearSharedBorderState(other);
        }
        g_SharedBorderTransition = false;
        return;
    }
    if (MultiplayerGameplay::IsMultiplayer() &&
        this->initParam < TH07_MULTI_MAX_PLAYERS)
        g_cherryMaxBreakGrowth[this->initParam] += 10000;
#endif

    g_GameManager.IncreaseCherryMax(10000);
    g_GameManager.IncreaseCherry(10000);
    cherryDiff = g_GameManager.cherry - g_GameManager.globals->cherryStart;
    cherryDiff *= 10;
    g_GameManager.AddScore(cherryDiff);
    g_Gui.ShowStatusPopup(cherryDiff, 4);
    g_GameManager.cherryPlus = g_GameManager.globals->cherryStart;
    g_SoundPlayer.PlaySoundByIdx(SOUND_BORDER_BREAK, 0);
    if (this->playerState == PLAYER_STATE_SPAWNING)
    {
        this->playerSprite.scale.x = 1.0f;
        this->playerSprite.scale.y = 1.0f;
        this->playerSprite.color.color = 0xffffffff;
        this->playerSprite.blendMode = 0;
        this->invulnerabilityTimer = 240;
        this->respawnTimer = this->shooterData->initialRespawnTimer;
    }
    this->playerState = PLAYER_STATE_INVULNERABLE;
    this->invulnerabilityTimer = 40;
    this->borderInvulnerabilityTime = 40;
    this->hasBorder = BORDER_NONE;
    if (this->borderEffect)
    {
        this->borderEffect->inUseFlag = 0;
        this->borderEffect = NULL;
    }
}

BombClearBox *Player::SpawnBombProjectile(ZunVec3 *centerPosition, f32 sizeX, f32 sizeY,
                                          i32 itemType)
{
    BombClearBox *bomb;
    i32 i;

    bomb = this->bombClearBoxes;
    for (i = 0; i < ARRAY_SIZE_SIGNED(this->bombClearBoxes) - 1; i++, bomb++)
    {
        if (bomb->size.x == 0.0f && bomb->radius == 0.0f)
        {
            break;
        }
    }
    bomb->pos.x = centerPosition->x;
    bomb->pos.y = centerPosition->y;
    bomb->size.x = sizeX;
    bomb->size.y = sizeY;
    bomb->lifetime = 0;
    bomb->itemType = itemType;
    this->dirtyBombBoxes = true;
    return bomb;
}

BombClearBox *Player::SpawnGrowingBomb(ZunVec3 *pos, f32 radius, f32 radiusGrowth, i32 lifetime,
                                       i32 itemType)
{
    BombClearBox *bomb;
    i32 i;

    bomb = this->bombClearBoxes;
    for (i = 0; i < ARRAY_SIZE_SIGNED(this->bombClearBoxes) - 1; i++, bomb++)
    {
        if (bomb->size.x == 0.0f && bomb->radius == 0.0f)
        {
            break;
        }
    }
    bomb->pos.x = pos->x;
    bomb->pos.y = pos->y;
    bomb->radius = radius;
    bomb->radiusGrowth = radiusGrowth;
    bomb->lifetime = lifetime;
    bomb->itemType = itemType;
    this->dirtyBombBoxes = true;
    return bomb;
}

void Player::ActivateBorder()
{
    Effect *spawnedEffect;

#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (MultiplayerGameplay::IsMultiplayer() && !g_SharedBorderTransition &&
        GetActivePlayerCount() > 1)
    {
        ActivateSharedBorder();
        return;
    }
    if (MultiplayerGameplay::IsMultiplayer() &&
        (this->playerState == PLAYER_STATE_ELIMINATED ||
         this->playerState == PLAYER_STATE_SPIRIT))
        return;
#endif

    if (this->bombInfo.isInUse ||
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        (!g_SharedBorderTransition && g_Gui.HasCurrentMsgIdx())
#else
        g_Gui.HasCurrentMsgIdx()
#endif
    )
    {
        this->hasBorder = BORDER_READY;
        return;
    }

    switch (this->playerState)
    {
    case PLAYER_STATE_SPAWNING:
    case PLAYER_STATE_INVULNERABLE:
        this->hasBorder = BORDER_READY;
        break;
    case PLAYER_STATE_DEAD:
        if (this->respawnTimer != 0)
        {
            BreakBorder();
            return;
        }

        this->hasBorder = BORDER_READY;
        break;
    default:
        this->invulnerabilityTimer = 540;
        this->borderTimer = this->invulnerabilityTimer;
        this->hasBorder = BORDER_ACTIVE;
        this->playerState = PLAYER_STATE_BORDER;
        if (this->borderEffect)
        {
            this->borderEffect->inUseFlag = 0;
        }
        if (this->effect)
        {
            this->effect->inUseFlag = 0;
            this->effect = NULL;
        }
        spawnedEffect = g_EffectManager.SpawnSpecialEffect(
            28, &this->pos,
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
            GetPlayerEffectSlot(this, 4),
#else
            4,
#endif
            1, 0xffffffff);
        spawnedEffect->vm.interpStartTimes[4] = 0;
        spawnedEffect->vm.interpEndTimes[4] = this->invulnerabilityTimer.GetCurrent();
        spawnedEffect->vm.easeModes[4] = 0;
        spawnedEffect->vm.scaleInterpInitial.y = 1.0f;
        spawnedEffect->vm.scaleInterpInitial.x = 1.0f;
        spawnedEffect->vm.scaleInterpFinal.x = 0.25f;
        spawnedEffect->vm.scaleInterpFinal.y = 0.25f;
        spawnedEffect->vm.intVars1[0] = this->invulnerabilityTimer.GetCurrent();
        spawnedEffect->vm.angleVel.z *= -1.0f;
        this->borderEffect = spawnedEffect;
        g_Gui.ShowStatusPopup(0, 2);
        g_SoundPlayer.PlaySoundByIdx(SOUND_BORDER_ACTIVATE, 0);
        g_SoundPlayer.PlaySoundByIdx(SOUND_BORDER_ACTIVATE2, 0);
        g_ReplayManager->replayEventFlags |= 8;
        break;
    }
}

void Player::BreakBorder()
{
    f32 angle;
    i32 i;
    Effect *effect;

#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (MultiplayerGameplay::IsMultiplayer() && !g_SharedBorderTransition &&
        GetActivePlayerCount() > 1 && IsSharedBorderActive())
    {
        g_SharedBorderTransition = true;
        this->BreakBorder();
        for (u8 playerId = 0; playerId < TH07_MULTI_MAX_PLAYERS; ++playerId)
        {
            Player *other = &g_Players[playerId];
            if (other != this && IsPlayerSlotActive(playerId) &&
                (other->hasBorder == BORDER_ACTIVE ||
                 other->playerState == PLAYER_STATE_BORDER))
                ClearSharedBorderState(other);
        }
        g_SharedBorderTransition = false;
        return;
    }
#endif

    if (this->borderEffect)
    {
        this->borderEffect->inUseFlag = 0;
        this->borderEffect = NULL;
    }
    effect = g_EffectManager.SpawnSpecialEffect(
        28, &this->pos,
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        GetPlayerEffectSlot(this, 4),
#else
        4,
#endif
        1, 0xffffffff);
    effect->vm.interpStartTimes[4] = 0;
    effect->vm.interpEndTimes[4] = 30;
    effect->vm.easeModes[4] = 0;
    effect->vm.scaleInterpInitial.x = 0.0625f;
    effect->vm.scaleInterpInitial.y = 0.0625f;
    effect->vm.scaleInterpFinal.x = 1.3f;
    effect->vm.scaleInterpFinal.y = 1.3f;
    effect->vm.interpStartTimes[2] = 0;
    effect->vm.interpEndTimes[2] = 30;
    effect->vm.easeModes[2] = 1;
    effect->vm.colorInterpInitialColor.bytes.a = effect->vm.color.bytes.a;
    effect->vm.colorInterpFinalColor.bytes.a = 0;
    effect->vm.intVars1[0] = 30;
    this->borderEffect = effect;
    g_EnemyManager.spellcardInfo.captureScore = 0;
    g_EnemyManager.spellcardInfo.isCapturing = 0;
    this->hasBorder = BORDER_NONE;
    this->playerState = PLAYER_STATE_INVULNERABLE;
    this->invulnerabilityTimer = 40;
    this->borderInvulnerabilityTime = 40;
    g_GameManager.cherryPlus = g_GameManager.globals->cherryStart;
    // Upstream th07_border_break @ 0x441DA4 is inside BreakBorder(), after
    // the forced-break state transition and before the bomb-clear effect.
    // Natural border expiration uses BreakBorderNaturally() and must not count.
    PracticeRuntime::RecordBorderBreak();
    SpawnGrowingBomb(&this->pos, 32.0f, 16.0f, 50, 8);
    angle = -ZUN_PI;
    for (i = 0; i < 32; i++, angle += 0.19634955f)
    {
        effect = g_EffectManager.SpawnEffect(29, &this->pos, 1, 0xffffffff);
        effect->direction.x = cosf(angle);
        effect->direction.y = sinf(angle);
    }
    g_SoundPlayer.PlaySoundByIdx(SOUND_BOMB_MARISA_A_FOCUS, 0);
    g_SoundPlayer.PlaySoundByIdx(SOUND_BORDER_BREAK, 0);
    g_ReplayManager->replayEventFlags = g_ReplayManager->replayEventFlags | 0x10;
}

void Player::UpdateUI()
{
    this->positionOfLastEnemyHit = ZunVec3(-999.0f, -999.0f, 0.0f);
    this->sakuyaTargetPosition = ZunVec3(-999.0f, -999.0f, 0.0f);
    this->targetingEnemy = 0;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (MultiplayerGameplay::IsMultiplayer() && this->initParam != 0)
        return;
#endif
    if (this->pos.y >= 400.0f)
    {
        if (g_AsciiManager.GetFadeState() != 2 && this->pos.x < 160.0f)
        {
            g_AsciiManager.cherryGauge.pendingInterrupt = 2;
            g_AsciiManager.uiFadeState = 2;
        }
        else if (g_AsciiManager.GetFadeState() == 2 && this->pos.x > 160.0f)
        {
            g_AsciiManager.cherryGauge.pendingInterrupt = 3;
            g_AsciiManager.uiFadeState = 3;
        }
    }
    else if (g_AsciiManager.GetFadeState() == 2)
    {
        g_AsciiManager.cherryGauge.pendingInterrupt = 3;
        g_AsciiManager.uiFadeState = 3;
    }
}

u32 Player::OnUpdate(Player *arg)
{
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (MultiplayerGameplay::IsMultiplayer())
    {
        const i32 expectedPlayerId = static_cast<i32>(arg - &g_Players[0]);
        if (expectedPlayerId >= 0 &&
            expectedPlayerId < TH07_MULTI_MAX_PLAYERS)
        {
            // Rollback restores gameplay state, but chain ownership is fixed
            // by the slot in g_Players. Keep the input lane tied to that
            // stable owner even if an older/corrupt snapshot carried another
            // initParam value.
            arg->initParam = static_cast<u8>(expectedPlayerId);
            if (!IsPlayerSlotActive(static_cast<u8>(expectedPlayerId)))
                return CHAIN_CALLBACK_RESULT_CONTINUE;
        }
    }
#endif
    arg->UpdatePrev();

    arg->prevPos = arg->pos;
    arg->prevOptionsPosition[0] = arg->optionsPosition[0];
    arg->prevOptionsPosition[1] = arg->optionsPosition[1];
    if (g_GameManager.isTimeStopped)
    {
        return CHAIN_CALLBACK_RESULT_CONTINUE;
    }
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (MultiplayerGameplay::IsMultiplayer())
    {
        if (arg->playerState == PLAYER_STATE_ELIMINATED)
        {
            arg->UpdateShots();
            return CHAIN_CALLBACK_RESULT_CONTINUE;
        }
    }
#endif
    arg->UpdateBombProjectiles();
    arg->UpdateBorderAndBombState();
    if (arg->playerState == PLAYER_STATE_DEAD)
    {
        if (arg->UpdateDeath())
        {
            goto WHAT;
        }
        else
        {
            goto WHY;
        }
    }
    if (arg->playerState == PLAYER_STATE_SPAWNING)
    {
    WHAT:
        arg->Respawn();
    }
WHY:
    arg->UpdateState();
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (MultiplayerGameplay::IsMultiplayer())
    {
        UpdateLifeTransfer(arg);
        UpdatePowerTransfer(arg);
    }
#endif
    if (arg->playerState != PLAYER_STATE_DEAD &&
        arg->playerState != PLAYER_STATE_SPAWNING
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        && (!MultiplayerGameplay::IsMultiplayer() ||
            arg->playerState != PLAYER_STATE_SPIRIT)
#endif
    )
    {
        arg->HandlePlayerInputs();
    }
    g_AnmManager->ExecuteScript(&arg->playerSprite);
    if (arg->optionState != OPTION_HIDDEN)
    {
        g_AnmManager->ExecuteScript(&arg->optionsSprite[0]);
        g_AnmManager->ExecuteScript(&arg->optionsSprite[1]);
    }
    arg->UpdateShots();
    arg->UpdateFireBulletTimer();
    arg->UpdateUI();
#ifdef TH_ENABLE_NETPLAY
    if (!Netplay::SideEffects::IsSpeculative())
        UpdateEaglerHitboxVm(arg);
#else
    UpdateEaglerHitboxVm(arg);
#endif
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

u32 Player::OnDrawHighPrio(Player *arg)
{
    ZunColor color;

#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (MultiplayerGameplay::IsMultiplayer() &&
        !IsPlayerSlotActive(arg->initParam))
        return CHAIN_CALLBACK_RESULT_CONTINUE;
#endif
    arg->DrawBullets();
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (MultiplayerGameplay::IsMultiplayer() &&
        arg->playerState == PLAYER_STATE_ELIMINATED)
        return CHAIN_CALLBACK_RESULT_CONTINUE;
#endif
    if (arg->bombInfo.isInUse)
    {
        if (!arg->bombInfo.isFocus)
        {
            arg->bombInfo.draw(arg);
        }
        else
        {
            arg->bombInfo.drawFocus(arg);
        }
    }
    if (!g_GameManager.isInRetryMenu)
    {
        ZunVec3 drawPlayerPos = arg->prevPos.Lerp(arg->pos, g_RenderAlpha);
        ZunVec3 drawOptionsPos[2] = {
            arg->prevOptionsPosition[0].Lerp(arg->optionsPosition[0], g_RenderAlpha),
            arg->prevOptionsPosition[1].Lerp(arg->optionsPosition[1], g_RenderAlpha)};
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        const ZunVec3 logicalDrawPlayerPos = drawPlayerPos;
        drawPlayerPos = PresentRemotePlayer(arg, logicalDrawPlayerPos);
        const ZunVec3 remoteDrawOffset = drawPlayerPos - logicalDrawPlayerPos;
        drawOptionsPos[0] += remoteDrawOffset;
        drawOptionsPos[1] += remoteDrawOffset;
#endif
        arg->playerSprite.pos.x = g_GameManager.arcadeRegionTopLeftPos.x + drawPlayerPos.x;
        arg->playerSprite.pos.y = g_GameManager.arcadeRegionTopLeftPos.y + drawPlayerPos.y;
        arg->playerSprite.pos.z = 0.0f;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        const ZunColor originalPlayerColor = arg->playerSprite.color;
        const ZunColor originalPlayerPrevColor = arg->playerSprite.prevColor;
        const ZunColor originalPlayerColor2 = arg->playerSprite.color2;
        const ZunColor originalPlayerPrevColor2 = arg->playerSprite.prevColor2;
        if (MultiplayerGameplay::IsMultiplayer())
        {
            if (arg->initParam != 0 &&
                MultiplayerGameplay::ShouldTintPlayer(arg->initParam))
            {
                arg->playerSprite.color.color =
                    (arg->playerSprite.color.color & 0xff000000) | 0x0080ffff;
            }
            const u8 normalAlpha = GetPlayerOverlapAlpha(arg);
            const u8 presentationAlpha = GetPlayerRescuePresentationAlpha(arg, normalAlpha);
            if (arg->playerState == PLAYER_STATE_SPIRIT)
            {
                arg->playerSprite.color.bytes.a = presentationAlpha;
                arg->playerSprite.prevColor.bytes.a = presentationAlpha;
            }
            else
            {
                ClampVmAlpha(&arg->playerSprite, presentationAlpha);
            }
            if (MultiplayerGameplay::IsPlayerTemporarilyAbsent(arg->initParam))
            {
                const u8 currentAlpha = (u8)(arg->playerSprite.color.color >> 24);
                const u8 absentAlpha = currentAlpha < 0x50 ? currentAlpha : 0x50;
                arg->playerSprite.color.color =
                    (arg->playerSprite.color.color & 0x00ffffff) |
                    ((u32)absentAlpha << 24);
            }
        }
#endif
        DrawLocalPlayerLocator(arg, drawPlayerPos);
        g_AnmManager->DrawNoRotation(&arg->playerSprite);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        arg->playerSprite.color = originalPlayerColor;
        arg->playerSprite.prevColor = originalPlayerPrevColor;
        arg->playerSprite.color2 = originalPlayerColor2;
        arg->playerSprite.prevColor2 = originalPlayerPrevColor2;
#endif
        if (arg->optionState != OPTION_HIDDEN &&
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
            (!MultiplayerGameplay::IsMultiplayer() ||
             !MultiplayerGameplay::IsPlayerTemporarilyAbsent(arg->initParam)) &&
#endif
            (arg->playerState == PLAYER_STATE_ALIVE || arg->playerState == PLAYER_STATE_BORDER ||
             arg->playerState == PLAYER_STATE_INVULNERABLE))
        {
            arg->optionsSprite[0].pos.x =
                g_GameManager.arcadeRegionTopLeftPos.x + drawOptionsPos[0].x;
            arg->optionsSprite[0].pos.y =
                g_GameManager.arcadeRegionTopLeftPos.y + drawOptionsPos[0].y;
            arg->optionsSprite[0].pos.z = 0.0f;
            arg->optionsSprite[1].pos.x =
                g_GameManager.arcadeRegionTopLeftPos.x + drawOptionsPos[1].x;
            arg->optionsSprite[1].pos.y =
                g_GameManager.arcadeRegionTopLeftPos.y + drawOptionsPos[1].y;
            arg->optionsSprite[1].pos.z = 0.0f;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
            const ZunColor originalOptionColor0 = arg->optionsSprite[0].color;
            const ZunColor originalOptionPrevColor0 = arg->optionsSprite[0].prevColor;
            const ZunColor originalOptionColor20 = arg->optionsSprite[0].color2;
            const ZunColor originalOptionPrevColor20 = arg->optionsSprite[0].prevColor2;
            const ZunColor originalOptionColor1 = arg->optionsSprite[1].color;
            const ZunColor originalOptionPrevColor1 = arg->optionsSprite[1].prevColor;
            const ZunColor originalOptionColor21 = arg->optionsSprite[1].color2;
            const ZunColor originalOptionPrevColor21 = arg->optionsSprite[1].prevColor2;
            if (MultiplayerGameplay::IsMultiplayer())
            {
                const u8 proximityAlpha = GetPlayerOverlapAlpha(arg);
                ClampVmAlpha(&arg->optionsSprite[0], proximityAlpha);
                ClampVmAlpha(&arg->optionsSprite[1], proximityAlpha);
            }
#endif
            g_AnmManager->Draw(&arg->optionsSprite[0]);
            g_AnmManager->Draw(&arg->optionsSprite[1]);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
            arg->optionsSprite[0].color = originalOptionColor0;
            arg->optionsSprite[0].prevColor = originalOptionPrevColor0;
            arg->optionsSprite[0].color2 = originalOptionColor20;
            arg->optionsSprite[0].prevColor2 = originalOptionPrevColor20;
            arg->optionsSprite[1].color = originalOptionColor1;
            arg->optionsSprite[1].prevColor = originalOptionPrevColor1;
            arg->optionsSprite[1].color2 = originalOptionColor21;
            arg->optionsSprite[1].prevColor2 = originalOptionPrevColor21;
#endif
        }
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        if (MultiplayerGameplay::IsMultiplayer())
        {
            DrawLifeTransferPrompt(arg);
            DrawPowerTransferPrompt(arg);
            DrawStageIntroPlayerName(arg);
        }
#endif
    }
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

u32 Player::OnDrawLowPrio(Player *arg)
{
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (MultiplayerGameplay::IsMultiplayer() && !IsPlayerSlotActive(arg->initParam))
        return CHAIN_CALLBACK_RESULT_CONTINUE;
#endif
    arg->DrawBulletExplosions();
    DrawEaglerHitboxVm(arg);
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

f32 Player::AngleToPlayer(ZunVec3 *pos)
{
    f32 y;
    f32 x;

    x = this->pos.x - pos->x;
    y = this->pos.y - pos->y;
    if (y == 0.0f && x == 0.0f)
    {
        return ZUN_PI / 2.0f;
    }
    else
    {
        return atan2f(y, x);
    }
}

ZunResult Player::AddedCallback(Player *arg)
{
    PlayerBullet *bullet;
    i32 i;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    i32 loadoutIndex = g_GameManager.shotTypeAndCharacter;
    i32 playerCharacter = g_GameManager.character;
    i32 playerAnmFile = ANM_FILE_PLAYER;
    i32 playerAnmOffset = ANM_OFFSET_PLAYER;

    if (MultiplayerGameplay::IsMultiplayer())
    {
        playerCharacter = MultiplayerGameplay::GetPlayerCharacter(arg->initParam);
        const i32 playerShot = MultiplayerGameplay::GetPlayerShot(arg->initParam);
        loadoutIndex = playerCharacter * 2 + playerShot;
        if (arg->initParam == 1)
        {
            playerAnmFile = ANM_FILE_PLAYER2;
            playerAnmOffset = ANM_OFFSET_PLAYER2;
        }
        else if (arg->initParam == 2)
        {
            playerAnmFile = ANM_FILE_PLAYER3;
            playerAnmOffset = ANM_OFFSET_PLAYER3;
        }
    }
#else
    const i32 loadoutIndex = g_GameManager.shotTypeAndCharacter;
    const i32 playerCharacter = g_GameManager.character;
    const i32 playerAnmFile = ANM_FILE_PLAYER;
    const i32 playerAnmOffset = ANM_OFFSET_PLAYER;
#endif

    g_EaglerHitboxVmActive = false;

    if (ShtData::LoadShtData(&arg->shooterData,
                             g_ShooterTable[loadoutIndex]) != ZUN_SUCCESS)
    {
        return ZUN_ERROR;
    }

    if (ShtData::LoadShtData(&arg->shooterDataFocus,
                             g_ShooterTableFocus[loadoutIndex]) !=
        ZUN_SUCCESS)
    {
        return ZUN_ERROR;
    }

    if ((u32)(g_Supervisor.curState != 3 && g_Supervisor.curState != 11 &&
              g_Supervisor.curState != 12))
    {
        const char *playerAnmPath;
        switch (playerCharacter)
        {
        case CHAR_REIMU:
            playerAnmPath = "data/player00.anm";
            break;
        case CHAR_MARISA:
            playerAnmPath = "data/player01.anm";
            break;
        case CHAR_SAKUYA:
            playerAnmPath = "data/player02.anm";
            break;
        default:
            playerAnmPath = "data/player00.anm";
            break;
        }
        if (g_AnmManager->LoadAnms(playerAnmFile, playerAnmPath, playerAnmOffset) != ZUN_SUCCESS)
        {
            return ZUN_ERROR;
        }
    }
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    g_AnmManager->SetAnmIdxAndExecuteScript(&arg->playerSprite,
                                             GetPlayerAnmScript(arg, 1024));
    if (MultiplayerGameplay::IsMultiplayer())
    {
        if (MultiplayerGameplay::GetPlayerCount() >= 3)
        {
            arg->pos.x = g_GameManager.arcadeRegionSize.x / 2.0f +
                                    (static_cast<i32>(arg->initParam) - 1) * 48.0f;
        }
        else
        {
            arg->pos.x = g_GameManager.arcadeRegionSize.x / 2.0f +
                                    (arg->initParam == 0 ? -32.0f : 32.0f);
        }
    }
    else
    {
        arg->pos.x = g_GameManager.arcadeRegionSize.x / 2.0f;
    }
#else
    g_AnmManager->SetAnmIdxAndExecuteScript(&arg->playerSprite, 1024);
    arg->pos.x = g_GameManager.arcadeRegionSize.x / 2.0f;
#endif
    arg->pos.y = g_GameManager.arcadeRegionSize.y - 64.0f;
    arg->pos.z = 0.49f;
    arg->optionsPosition[0].z = 0.49f;
    arg->optionsPosition[1].z = 0.49f;
    arg->prevPos = arg->pos;
    arg->prevOptionsPosition[0] = arg->optionsPosition[0];
    arg->prevOptionsPosition[1] = arg->optionsPosition[1];

    for (i = 0; i < 112; i++)
    {
        arg->bombDamageBoxes[i].size.x = 0.0f;
    }
    for (i = 0; i < 16; i++) // this looks kinda suspect but thats basically what the old code did
    {
        arg->bombClearBoxes[i].size.x = 0.0f;
    }
    arg->hitboxSize.y = arg->shooterData->hitboxRadius / 2.0f;
    arg->hitboxSize.x = arg->hitboxSize.y;
    arg->hitboxSize.z = 5.0f;
    arg->grazeSize.y = arg->shooterData->grabItemRadius / 2.0f;
    arg->grazeSize.x = arg->grazeSize.y;
    arg->grazeSize.z = 5.0f;
    arg->grabItemSize.x = 12.0f;
    arg->grabItemSize.y = 12.0f;
    arg->grabItemSize.z = 5.0f;
    arg->playerDirection = MOVEMENT_NONE;
    arg->playerState = PLAYER_STATE_SPAWNING;
    arg->invulnerabilityTimer = 120;
    arg->optionState = OPTION_UNFOCUSED;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    g_AnmManager->SetAnmIdxAndExecuteScript(&arg->optionsSprite[0],
                                             GetPlayerAnmScript(arg, 1152));
    g_AnmManager->SetAnmIdxAndExecuteScript(&arg->optionsSprite[1],
                                             GetPlayerAnmScript(arg, 1153));
#else
    g_AnmManager->SetAnmIdxAndExecuteScript(&arg->optionsSprite[0], 1152);
    g_AnmManager->SetAnmIdxAndExecuteScript(&arg->optionsSprite[1], 1153);
#endif
    bullet = arg->bullets;
    for (i = 0; i < 96; i++, bullet++)
    {
        bullet->bulletState = 0;
    }
    arg->fireBulletTimer = -1;
    arg->bombInfo.bombCalc = g_BombData[loadoutIndex].calc;
    arg->bombInfo.draw = g_BombData[loadoutIndex].draw;
    arg->bombInfo.bombFocusCalc = g_BombData[loadoutIndex].calcFocus;
    arg->bombInfo.drawFocus = g_BombData[loadoutIndex].drawFocus;
    arg->bombInfo.isInUse = 0;
    arg->dirtyBombBoxes = true;
    arg->numActiveBombClearBoxes = 0;
    arg->optionAngle = -1.5707964f;
    arg->verticalMovementSpeedMultiplierDuringBomb = 1.0f;
    arg->horizontalMovementSpeedMultiplierDuringBomb = 1.0f;
    arg->respawnTimer = arg->shooterData->initialRespawnTimer;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (arg->initParam == 0)
#endif
    {
        if ((u32)(g_Supervisor.curState != 3 && g_Supervisor.curState != 11 &&
                  g_Supervisor.curState != 12))
        {
            g_AsciiManager.cherryGauge.pendingInterrupt = 1;
            g_AsciiManager.uiFadeState = 1;
        }
        g_AsciiManager.GetBossMarker(0)->pendingInterrupt = 2;
        g_AsciiManager.GetBossMarker(1)->pendingInterrupt = 2;
        g_AsciiManager.GetBossMarker(2)->pendingInterrupt = 2;
    }
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (MultiplayerGameplay::IsMultiplayer())
    {
        if (arg->initParam == MultiplayerGameplay::GetPlayerCount() - 1 &&
            g_GameManager.cherryPlus >= g_GameManager.globals->cherryStart +
                                            GetSharedBorderThreshold())
        {
            g_GameManager.cherryPlus = g_GameManager.globals->cherryStart +
                                       GetSharedBorderThreshold();
            ActivateSharedBorder();
        }
    }
    else
#endif
    if (g_GameManager.cherryPlus >= g_GameManager.globals->cherryStart + 50000)
    {
        g_GameManager.cherryPlus = g_GameManager.globals->cherryStart + 50000;
        g_Player.ActivateBorder();
    }
    return ZUN_SUCCESS;
}

ZunResult Player::DeletedCallback(Player *arg)
{
    if ((u32)(g_Supervisor.curState != 3 && g_Supervisor.curState != 11 &&
              g_Supervisor.curState != 12))
    {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        const i32 playerAnmFile = arg->initParam == 0
                                      ? ANM_FILE_PLAYER
                                      : (arg->initParam == 1 ? ANM_FILE_PLAYER2
                                                             : ANM_FILE_PLAYER3);
        g_AnmManager->ReleaseAnm(playerAnmFile);
        if (arg->initParam == 0)
#else
        g_AnmManager->ReleaseAnm(10);
#endif
        {
            g_AsciiManager.cherryGauge.pendingInterrupt = 99;
            g_AsciiManager.uiFadeState = 99;
            g_AsciiManager.GetBossMarker(0)->pendingInterrupt = 99;
            g_AsciiManager.GetBossMarker(1)->pendingInterrupt = 99;
            g_AsciiManager.GetBossMarker(2)->pendingInterrupt = 99;
        }
    }
    SAFE_DELETE_ARRAY(arg->shooterData->levels);
    SAFE_DELETE_ARRAY(arg->shooterData->entries);
    SAFE_DELETE(arg->shooterData);
    SAFE_DELETE_ARRAY(arg->shooterDataFocus->levels);
    SAFE_DELETE_ARRAY(arg->shooterDataFocus->entries);
    SAFE_DELETE(arg->shooterDataFocus);
    return ZUN_SUCCESS;
}

#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
static ZunResult RegisterOnePlayer(Player *mgr, u8 playerId)
{
    memset(mgr, 0, sizeof(Player));
    mgr->invulnerabilityTimer = 0;
    mgr->initParam = playerId;
    mgr->calcChain = g_Chain.CreateElem((ChainCallback)Player::OnUpdate);
    mgr->drawChain1 = g_Chain.CreateElem((ChainCallback)Player::OnDrawHighPrio);
    mgr->drawChain2 = g_Chain.CreateElem((ChainCallback)Player::OnDrawLowPrio);
    mgr->calcChain->arg = mgr;
    mgr->drawChain1->arg = mgr;
    mgr->drawChain2->arg = mgr;
    mgr->calcChain->addedCallback = (ChainLifecycleCallback)Player::AddedCallback;
    mgr->calcChain->deletedCallback = (ChainLifecycleCallback)Player::DeletedCallback;
    if (g_Chain.AddToCalcChain(mgr->calcChain, 8))
    {
        return ZUN_ERROR;
    }
    g_Chain.AddToDrawChain(mgr->drawChain1, 6);
    g_Chain.AddToDrawChain(mgr->drawChain2, 8);
    return ZUN_SUCCESS;
}
#endif

ZunResult Player::RegisterChain(u32 param_1)
{
#ifdef TH_ENABLE_NETPLAY
    Netplay::Input::ResetDirectTouchStates();
#endif
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    // Multiplayer Replay is still a multiplayer gameplay session. The Replay
    // menu has already restored playerCount/loadouts from the EAGX metadata,
    // and ReplayManager feeds every recorded FrameInput lane during playback.
    // Do not collapse that session back to the vanilla single g_Player just
    // because g_GameManager.replay is set.
    if (MultiplayerGameplay::IsMultiplayer())
    {
        const bool preserveResources = g_Supervisor.curState == 3;
        // A partial 8-tap Power-transfer gesture is stage-local interaction,
        // not run state.  Never carry a few taps/windows through a stage load
        // where direct later-stage Replay would necessarily start from zero.
        for (u8 playerId = 0; playerId < TH07_MULTI_MAX_PLAYERS; ++playerId)
        {
            g_powerGiveTaps[playerId] = 0;
            g_powerGiveWindow[playerId] = 0;
        }
        if (!preserveResources)
        {
            ResetPlayerContributionStats();
            g_teamWipeRetryFrames = 0;
        }

        for (u8 playerId = 0; playerId < TH07_MULTI_MAX_PLAYERS; ++playerId)
        {
            g_PlayerActive[playerId] = playerId == 0 ||
                                       (playerId < MultiplayerGameplay::GetPlayerCount() &&
                                        MultiplayerGameplay::IsPlayerActive(playerId) &&
                                        !MultiplayerGameplay::IsPlayerPermanentlyDeparted(playerId));
        }

        for (u8 playerId = 0; playerId < TH07_MULTI_MAX_PLAYERS; ++playerId)
        {
            if (!g_PlayerActive[playerId])
            {
                continue;
            }
            if (RegisterOnePlayer(&g_Players[playerId], playerId) != ZUN_SUCCESS)
            {
                return ZUN_ERROR;
            }
            if (playerId > 0 && !preserveResources)
            {
                ResetMultiplayerPlayerResources(playerId);
            }
        }
        return ZUN_SUCCESS;
    }
#endif
    Player *mgr = &g_Player;
    memset(mgr, 0, sizeof(Player));
    mgr->invulnerabilityTimer = 0;
    mgr->initParam = param_1;
    mgr->calcChain = g_Chain.CreateElem((ChainCallback)OnUpdate);
    mgr->drawChain1 = g_Chain.CreateElem((ChainCallback)OnDrawHighPrio);
    mgr->drawChain2 = g_Chain.CreateElem((ChainCallback)OnDrawLowPrio);
    mgr->calcChain->arg = mgr;
    mgr->drawChain1->arg = mgr;
    mgr->drawChain2->arg = mgr;
    mgr->calcChain->addedCallback = (ChainLifecycleCallback)AddedCallback;
    mgr->calcChain->deletedCallback = (ChainLifecycleCallback)DeletedCallback;
    if (g_Chain.AddToCalcChain(mgr->calcChain, 8))
    {
        return ZUN_ERROR;
    }

    g_Chain.AddToDrawChain(mgr->drawChain1, 6);
    g_Chain.AddToDrawChain(mgr->drawChain2, 8);
    return ZUN_SUCCESS;
}

void Player::CutChain()
{
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (MultiplayerGameplay::IsMultiplayer())
    {
        for (u8 playerId = 0; playerId < TH07_MULTI_MAX_PLAYERS; ++playerId)
        {
            Player &player = g_Players[playerId];
            if (player.calcChain)
            {
                g_Chain.Cut(player.calcChain);
                player.calcChain = NULL;
            }
            if (player.drawChain1)
            {
                g_Chain.Cut(player.drawChain1);
                player.drawChain1 = NULL;
            }
            if (player.drawChain2)
            {
                g_Chain.Cut(player.drawChain2);
                player.drawChain2 = NULL;
            }
            g_PlayerActive[playerId] = playerId == 0;
        }
        return;
    }
#endif
    g_Chain.Cut(g_Player.calcChain);
    g_Player.calcChain = NULL;
    g_Chain.Cut(g_Player.drawChain1);
    g_Player.drawChain1 = NULL;
    g_Chain.Cut(g_Player.drawChain2);
    g_Player.drawChain2 = NULL;
}

ZunResult ShtData::LoadShtData(ShtData **data, const char *shtPath)
{
    u8 *rawFile = FileSystem::OpenFile(shtPath, 0);
    if (!rawFile)
    {
        return ZUN_ERROR;
    }

    ShtRawData *rawData = (ShtRawData *)rawFile;

    ShtData *parsed = new ShtData;
    memcpy(parsed, rawData, offsetof(ShtRawData, levels));

    parsed->levels = new ShtLevel[parsed->numLevels];

    i32 totalEntries = 0;
    for (i32 i = 0; i < parsed->numLevels; i++)
    {
        ShtRawEntry *re = (ShtRawEntry *)(rawFile + rawData->levels[i].entryOffset);
        while (re->fireInterval >= 0)
        {
            totalEntries++;
            re++;
        }
        totalEntries++;
    }

    parsed->entries = new ShtEntry[totalEntries];

    i32 entryIdx = 0;
    for (i32 i = 0; i < parsed->numLevels; i++)
    {
        parsed->levels[i].requiredPower = rawData->levels[i].requiredPower;
        parsed->levels[i].entry = &parsed->entries[entryIdx];

        ShtRawEntry *re = (ShtRawEntry *)(rawFile + rawData->levels[i].entryOffset);
        while (re->fireInterval >= 0)
        {
            ShtEntry *e = &parsed->entries[entryIdx++];
            e->fireInterval = re->fireInterval;
            e->fireOffset = re->fireOffset;
            e->offset = re->offset;
            e->hitboxSize = re->hitboxSize;
            e->angle = re->angle;
            e->speed = re->speed;
            e->damage = re->damage;
            e->option = re->option;
            e->bulletState2 = re->bulletState2;
            e->anmFileIdx = re->anmFileIdx;
            e->soundIdx = re->soundIdx;
            e->fireCallback = re->fireCallback < 6 ? g_ShtFireFuncs[re->fireCallback] : NULL;
            e->updateCallback =
                re->updateCallback < 6 ? g_ShtUpdateFuncs[re->updateCallback] : NULL;
            e->drawCallback = re->drawCallback < 2 ? g_ShtDrawFuncs[re->drawCallback] : NULL;
            e->hitCallback = re->hitCallback < 4 ? g_ShtHitFuncs[re->hitCallback] : NULL;
            re++;
        }
        parsed->entries[entryIdx].fireInterval = -1;
        entryIdx++;
    }

    free(rawFile);
    *data = parsed;
    return ZUN_SUCCESS;
}
