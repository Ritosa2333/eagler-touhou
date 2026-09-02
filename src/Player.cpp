#include "Player.hpp"

#include <algorithm>
#include <cmath>
#include <cstring>

#include "AnmManager.hpp"
#include "AnmVm.hpp"
#include "AsciiManager.hpp"
#include "BombData.hpp"
#include "BulletData.hpp"
#include "BulletManager.hpp"
#include "ChainPriorities.hpp"
#include "EclManager.hpp"
#include "EffectManager.hpp"
#include "EaglerOptions.hpp"
#include "EnemyManager.hpp"
#include "GameManager.hpp"
#include "GameWindow.hpp"
#include "Gui.hpp"
#include "ItemManager.hpp"
#include "PracticeRuntime.hpp"
#include "ReplayExtension.hpp"
#include "Rng.hpp"
#include "ScreenEffect.hpp"
#include "SoundPlayer.hpp"
#include "Supervisor.hpp"
#include "Touch.hpp"
#include "i18n.hpp"
#include "utils.hpp"
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#include "multiplayer/GameplaySession.hpp"
#include "netplay/NetplayInput.hpp"
#endif
#ifdef TH_ENABLE_NETPLAY
#include "netplay/NetplaySideEffects.hpp"
#endif

#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
Player g_Players[TH06_MULTI_MAX_PLAYERS];
bool g_PlayerActive[TH06_MULTI_MAX_PLAYERS] = {false, false, false};
i32 g_teamWipeRetryFrames = 0;
#else
Player g_Player;
#endif

#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
namespace
{
constexpr f32 REMOTE_PLAYER_FADE_START_DISTANCE = 100.0f;
constexpr f32 REMOTE_PLAYER_FADE_FULL_DISTANCE = 50.0f;
constexpr u8 REMOTE_PLAYER_FADE_MIN_ALPHA = 51;
constexpr f32 LOCAL_PLAYER_LOCATOR_THICKNESS = 1.0f;

struct RemotePresentationState
{
    bool valid = false;
    u64 lastTickNs = 0;
    ZunVec3 position{};
};

RemotePresentationState g_RemotePresentation[TH06_MULTI_MAX_PLAYERS];
ZunVec3 g_RemoteDrawOffsets[TH06_MULTI_MAX_PLAYERS];

constexpr f32 REVIVE_PRESENTATION_BASE_ALPHA = 80.0f;
constexpr f32 REVIVE_PRESENTATION_ACTIVE_ALPHA = 254.0f;
constexpr f32 REVIVE_PRESENTATION_FADE_SECONDS = 0.20f;
struct RevivePresentationState
{
    bool valid = false;
    bool wasRevivable = false;
    u64 lastTickNs = 0;
    f32 alpha = REVIVE_PRESENTATION_BASE_ALPHA;
};
RevivePresentationState g_RevivePresentation[TH06_MULTI_MAX_PLAYERS];

ZunVec3 PresentRemotePlayer(Player *player, const ZunVec3 &target)
{
    if (!player || !MultiplayerGameplay::IsMultiplayer() ||
        player->initParam >= TH06_MULTI_MAX_PLAYERS ||
        player->initParam == MultiplayerGameplay::GetLocalPlayerSlot())
    {
        if (player && player->initParam < TH06_MULTI_MAX_PLAYERS)
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

        // Presentation may soften a rollback correction, but it may never
        // become authoritative or trail logical collision state by more than
        // twelve game pixels.
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

void DrawLocalPlayerLocator(const Player *player, const ZunVec3 &drawPlayerPos)
{
    if (!player || !MultiplayerGameplay::IsMultiplayer() ||
        !EaglerOptions::EnhanceLocalPlayerVisibility() ||
        player->initParam != MultiplayerGameplay::GetLocalPlayerSlot() ||
        g_GameManager.isInGameMenu || g_GameManager.isInRetryMenu || g_GameManager.isInReplay)
        return;

    const f32 left = g_GameManager.arcadeRegionTopLeftPos.x;
    const f32 top = g_GameManager.arcadeRegionTopLeftPos.y;
    const f32 right = left + g_GameManager.arcadeRegionSize.x;
    const f32 bottom = top + g_GameManager.arcadeRegionSize.y;
    const f32 x = left + drawPlayerPos.x;
    const f32 y = top + drawPlayerPos.y;
    const f32 halfThickness = LOCAL_PLAYER_LOCATOR_THICKNESS * 0.5f;
    ZunRect horizontal{left, y - halfThickness, right, y + halfThickness};
    ZunRect vertical{x - halfThickness, top, x + halfThickness, bottom};
    ScreenEffect::DrawSquare(&horizontal, COLOR_WHITE);
    ScreenEffect::DrawSquare(&vertical, COLOR_WHITE);
}

u8 GetPlayerOverlapAlpha(const Player *player)
{
    if (!player || !MultiplayerGameplay::IsMultiplayer() ||
        player->initParam >= TH06_MULTI_MAX_PLAYERS ||
        player->initParam == MultiplayerGameplay::GetLocalPlayerSlot() ||
        !IsPlayerGameplayActive(player->initParam))
        return 255;

    const u8 localPlayerId = MultiplayerGameplay::GetLocalPlayerSlot();
    if (localPlayerId >= TH06_MULTI_MAX_PLAYERS ||
        !IsPlayerGameplayActive(localPlayerId))
        return 255;

    const Player &localPlayer = g_Players[localPlayerId];
    const f32 dx = player->positionCenter.x - localPlayer.positionCenter.x;
    const f32 dy = player->positionCenter.y - localPlayer.positionCenter.y;
    f32 distance = sqrtf(dx * dx + dy * dy);
    if (distance >= REMOTE_PLAYER_FADE_START_DISTANCE)
        return 255;
    if (distance < REMOTE_PLAYER_FADE_FULL_DISTANCE)
        distance = REMOTE_PLAYER_FADE_FULL_DISTANCE;

    const f32 fadeProgress =
        (distance - REMOTE_PLAYER_FADE_FULL_DISTANCE) /
        (REMOTE_PLAYER_FADE_START_DISTANCE - REMOTE_PLAYER_FADE_FULL_DISTANCE);
    return static_cast<u8>(std::clamp<i32>(
        static_cast<i32>(fadeProgress * (255 - REMOTE_PLAYER_FADE_MIN_ALPHA)) +
            REMOTE_PLAYER_FADE_MIN_ALPHA,
        0, 255));
}

void ClampVmAlpha(AnmVm *vm, u8 alpha)
{
    if (!vm)
        return;
    const u8 currentAlpha = static_cast<u8>(COLOR_ALPHA(vm->color));
    const u8 previousAlpha = static_cast<u8>(COLOR_ALPHA(vm->prevColor));
    if (alpha < currentAlpha)
        vm->color = COLOR_SET_ALPHA(vm->color, alpha);
    if (alpha < previousAlpha)
        vm->prevColor = COLOR_SET_ALPHA(vm->prevColor, alpha);
}

u8 PlayerCharacter(const Player *player)
{
    return MultiplayerGameplay::IsMultiplayer()
               ? MultiplayerGameplay::GetPlayerCharacter(player->initParam)
               : g_GameManager.character;
}

bool IsTerminalPlayerState(const Player *player);

void UpdateTeamWipeRetryCountdownImpl()
{
    if (!MultiplayerGameplay::IsMultiplayer())
    {
        g_teamWipeRetryFrames = 0;
        return;
    }
    if (g_GameManager.isInGameMenu || g_GameManager.isInRetryMenu ||
        g_GameManager.isTimeStopped)
        return;

    bool hasParticipant = false;
    bool teamWiped = true;
    for (u8 playerId = 0; playerId < TH06_MULTI_MAX_PLAYERS; ++playerId)
    {
        if (!g_PlayerActive[playerId])
            continue;
        hasParticipant = true;
        if (!IsTerminalPlayerState(&g_Players[playerId]))
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

    // Keep TH07's proven deterministic three-second logical-frame grace, but
    // TH06 multiplayer deliberately has no Continue.  Once the grace expires,
    // take the same Result path vanilla TH06 uses when Continue is unavailable;
    // never expose Retry/Continue for even one presentation frame.
    if (g_teamWipeRetryFrames <= 0)
    {
        g_teamWipeRetryFrames = 180;
        return;
    }
    if (--g_teamWipeRetryFrames <= 0)
    {
        g_teamWipeRetryFrames = 0;
        g_GameManager.isInRetryMenu = 0;
        g_GameManager.guiScore = g_GameManager.score;
        g_Supervisor.curState = SUPERVISOR_STATE_RESULTSCREEN_FROMGAME;
    }
}

f32 PlayerSpawnX(const Player *player)
{
    if (!MultiplayerGameplay::IsMultiplayer() || !player)
        return g_GameManager.arcadeRegionSize.x / 2.0f;
    if (MultiplayerGameplay::GetPlayerCount() >= 3)
        return g_GameManager.arcadeRegionSize.x / 2.0f +
               (static_cast<i32>(player->initParam) - 1) * 48.0f;
    return g_GameManager.arcadeRegionSize.x / 2.0f +
           (player->initParam == 0 ? -32.0f : 32.0f);
}

u8 PlayerShot(const Player *player)
{
    return MultiplayerGameplay::IsMultiplayer()
               ? MultiplayerGameplay::GetPlayerShot(player->initParam)
               : g_GameManager.shotType;
}

i32 PlayerLoadoutIndex(const Player *player)
{
    return static_cast<i32>(PlayerShot(player)) + static_cast<i32>(PlayerCharacter(player)) * 2;
}

i32 PlayerAnmFile(const Player *player)
{
    switch (player->initParam)
    {
    case 1: return ANM_FILE_PLAYER2;
    case 2: return ANM_FILE_PLAYER3;
    default: return ANM_FILE_PLAYER;
    }
}

i32 PlayerAnmOffset(const Player *player)
{
    switch (player->initParam)
    {
    case 1: return ANM_OFFSET_PLAYER2;
    case 2: return ANM_OFFSET_PLAYER3;
    default: return ANM_OFFSET_PLAYER;
    }
}

bool CanSampleRawTouchForPlayer(const Player *player)
{
    return !MultiplayerGameplay::IsMultiplayer() ||
           (player && player->initParam == MultiplayerGameplay::GetLocalPlayerSlot());
}

bool PlayerUsedTouch(const Player *player)
{
    if (Netplay::Input::PlayerButtonOverridesActive())
        return Netplay::Input::PlayerTouchUsed(player ? player->initParam : 0);
    return CanSampleRawTouchForPlayer(player) && Touch::WasUsedThisRun();
}

bool PlayerUsedTouchToBomb(const Player *player)
{
    if (Netplay::Input::PlayerButtonOverridesActive())
        return Netplay::Input::PlayerTouchBomb(player ? player->initParam : 0);
    return CanSampleRawTouchForPlayer(player) && Touch::UsedTouchToBomb();
}

i32 NormalizePlayerAnmScript(const Player *player, i32 script)
{
    return script - PlayerAnmOffset(player) + ANM_OFFSET_PLAYER;
}

constexpr i32 LIFE_GIVE_HOLD_FRAMES = 90;
constexpr i32 LIFE_GIVE_WAIT_RELEASE_TOKEN = -1;
constexpr f32 RESOURCE_TRANSFER_DISTANCE_SQ = 20.0f * 20.0f;
constexpr i32 POWER_GIVE_TAPS_REQUIRED = 8;
constexpr i32 POWER_GIVE_TAP_WINDOW = 24;
constexpr i32 POWER_GIVE_AMOUNT = 20;
constexpr i32 POWER_GIVE_PROMPT_AFTER = 4;
constexpr f32 REVIVABLE_DRIFT_SPEED = 0.2f;

bool IsTerminalPlayerState(const Player *player)
{
    return player && (player->playerState == PLAYER_STATE_REVIVABLE ||
                      player->playerState == PLAYER_STATE_ELIMINATED);
}

bool IsLivingTransferPlayer(const Player *player)
{
    return player && IsPlayerGameplayActive(player->initParam) &&
           (player->playerState == PLAYER_STATE_ALIVE ||
            player->playerState == PLAYER_STATE_INVULNERABLE);
}

bool InTransferRange(const Player *giver, const Player *receiver)
{
    if (!giver || !receiver)
        return false;
    const f32 dx = giver->positionCenter.x - receiver->positionCenter.x;
    const f32 dy = giver->positionCenter.y - receiver->positionCenter.y;
    return dx * dx + dy * dy <= RESOURCE_TRANSFER_DISTANCE_SQ;
}

Player *SelectPowerTransferReceiver(const Player *giver)
{
    if (!giver || !IsLivingTransferPlayer(giver))
        return nullptr;
    Player *best = nullptr;
    i32 bestPower = 0;
    for (u8 playerId = 0; playerId < TH06_MULTI_MAX_PLAYERS; ++playerId)
    {
        if (playerId == giver->initParam)
            continue;
        Player *candidate = &g_Players[playerId];
        if (!IsLivingTransferPlayer(candidate) || !InTransferRange(giver, candidate))
            continue;
        const i32 power = GetPlayerPower(playerId);
        if (power >= 128)
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

Player *SelectLifeTransferReceiver(const Player *giver)
{
    if (!giver || !IsLivingTransferPlayer(giver))
        return nullptr;
    Player *best = nullptr;
    bool bestRevivable = false;
    i32 bestLives = 0;
    for (u8 playerId = 0; playerId < TH06_MULTI_MAX_PLAYERS; ++playerId)
    {
        if (playerId == giver->initParam || !IsPlayerGameplayActive(playerId))
            continue;
        Player *candidate = &g_Players[playerId];
        const bool revivable = candidate->playerState == PLAYER_STATE_REVIVABLE;
        const bool living = IsLivingTransferPlayer(candidate);
        const i32 lives = GetPlayerLives(playerId);
        if ((!revivable && (!living || lives >= 8)) || !InTransferRange(giver, candidate))
            continue;
        if (!best || (revivable && !bestRevivable) ||
            (revivable == bestRevivable && lives < bestLives) ||
            (revivable == bestRevivable && lives == bestLives &&
             playerId < best->initParam))
        {
            best = candidate;
            bestRevivable = revivable;
            bestLives = lives;
        }
    }
    return best;
}

i32 SelectLowestLifeRecipient(u8 excludedPlayerId)
{
    i32 bestId = -1;
    i32 bestLives = 0;
    for (u8 playerId = 0; playerId < TH06_MULTI_MAX_PLAYERS; ++playerId)
    {
        if (playerId == excludedPlayerId || !IsPlayerGameplayActive(playerId) ||
            !IsLivingTransferPlayer(&g_Players[playerId]))
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

bool IsPlayerActivelyBeingRevived(const Player *receiver)
{
    if (!receiver || receiver->playerState != PLAYER_STATE_REVIVABLE)
        return false;
    const i32 token = receiver->initParam + 1;
    for (u8 playerId = 0; playerId < TH06_MULTI_MAX_PLAYERS; ++playerId)
    {
        const Player *giver = &g_Players[playerId];
        if (giver == receiver || !IsLivingTransferPlayer(giver) ||
            GetPlayerLives(playerId) <= 0)
            continue;
        if (giver->lifeGiveTargetToken == token && giver->lifeGiveTimer > 0)
            return true;
    }
    return false;
}

u8 GetPlayerRescuePresentationAlpha(const Player *player, u8 normalAlpha)
{
    if (!player || player->initParam >= TH06_MULTI_MAX_PLAYERS)
        return normalAlpha;
    RevivePresentationState &state = g_RevivePresentation[player->initParam];
    const bool revivable = player->playerState == PLAYER_STATE_REVIVABLE;
    const u64 now = SDL_GetTicksNS();

    if (!state.valid)
    {
        state.valid = true;
        state.wasRevivable = revivable;
        state.lastTickNs = now;
        state.alpha = revivable ? REVIVE_PRESENTATION_BASE_ALPHA : normalAlpha;
    }

    // First presented frame after a successful rescue is exactly 100%.
    if (state.wasRevivable && !revivable)
    {
        state.alpha = 255.0f;
        state.lastTickNs = now;
        state.wasRevivable = false;
        g_SoundPlayer.PlaySoundByIdx(SOUND_1UP);
        return 255;
    }

    const f32 elapsed = std::clamp(
        static_cast<f32>(now - state.lastTickNs) / 1000000000.0f, 0.0f, 0.05f);
    state.lastTickNs = now;
    state.wasRevivable = revivable;

    const f32 target = revivable
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

    const i32 maxAlpha = revivable ? 254 : 255;
    return static_cast<u8>(std::clamp<i32>(static_cast<i32>(state.alpha + 0.5f), 0, maxAlpha));
}

void UpdateRevivableState(Player *player)
{
    if (!player || player->playerState != PLAYER_STATE_REVIVABLE)
        return;
    player->playerSprite.scaleX = 1.0f;
    player->playerSprite.scaleY = 1.0f;
    player->playerSprite.flags.blendMode = AnmVmBlendMode_InvSrcAlpha;
    player->playerSprite.color = COLOR_SET_ALPHA(COLOR_WHITE, 80);
    player->positionCenter.x +=
        player->previousHorizontalSpeed * g_Supervisor.effectiveFramerateMultiplier;
    player->positionCenter.y +=
        player->previousVerticalSpeed * g_Supervisor.effectiveFramerateMultiplier;

    const f32 minX = g_GameManager.playerMovementAreaTopLeftPos.x;
    const f32 maxX = minX + g_GameManager.playerMovementAreaSize.x;
    const f32 minY = g_GameManager.playerMovementAreaTopLeftPos.y + 300.0f;
    const f32 maxY = g_GameManager.playerMovementAreaTopLeftPos.y +
                     g_GameManager.playerMovementAreaSize.y - 32.0f;
    if (player->positionCenter.x < minX)
    {
        player->positionCenter.x = minX;
        player->previousHorizontalSpeed = fabsf(player->previousHorizontalSpeed);
    }
    else if (player->positionCenter.x > maxX)
    {
        player->positionCenter.x = maxX;
        player->previousHorizontalSpeed = -fabsf(player->previousHorizontalSpeed);
    }
    if (player->positionCenter.y < minY)
    {
        player->positionCenter.y = minY;
        player->previousVerticalSpeed = fabsf(player->previousVerticalSpeed);
    }
    else if (player->positionCenter.y > maxY)
    {
        player->positionCenter.y = maxY;
        player->previousVerticalSpeed = -fabsf(player->previousVerticalSpeed);
    }
}

void ResetTransferInputState(Player *player)
{
    if (!player)
        return;
    player->lifeGiveTimer = 0;
    player->lifeGiveTargetToken = 0;
    player->powerGiveTaps = 0;
    player->powerGiveWindow = 0;
}

void RevivePlayerFromTeammate(Player *receiver)
{
    if (!receiver || receiver->playerState != PLAYER_STATE_REVIVABLE)
        return;
    receiver->playerState = PLAYER_STATE_INVULNERABLE;
    receiver->orbState = ORB_UNFOCUSED;
    receiver->invulnerabilityTimer.SetCurrent(120);
    receiver->respawnTimer = 6;
    receiver->bombInfo.isInUse = 0;
    receiver->bulletGracePeriod = 60;
    receiver->previousHorizontalSpeed = 0.0f;
    receiver->previousVerticalSpeed = 0.0f;
    receiver->playerSprite.scaleX = 1.0f;
    receiver->playerSprite.scaleY = 1.0f;
    receiver->playerSprite.color = COLOR_WHITE;
    receiver->playerSprite.flags.blendMode = AnmVmBlendMode_InvSrcAlpha;
    ResetTransferInputState(receiver);
}

void UpdatePowerTransfer(Player *giver)
{
    if (!giver || !IsLivingTransferPlayer(giver))
    {
        if (giver)
        {
            giver->powerGiveTaps = 0;
            giver->powerGiveWindow = 0;
        }
        return;
    }
    Player *receiver = SelectPowerTransferReceiver(giver);
    if (!receiver || GetPlayerPower(giver->initParam) < POWER_GIVE_AMOUNT)
    {
        giver->powerGiveTaps = 0;
        giver->powerGiveWindow = 0;
        return;
    }
    if (giver->powerGiveWindow > 0 && --giver->powerGiveWindow == 0)
        giver->powerGiveTaps = 0;
    if (!WAS_PRESSED_PLAYER(giver, TH_BUTTON_SHOOT))
        return;

    ++giver->powerGiveTaps;
    giver->powerGiveWindow = POWER_GIVE_TAP_WINDOW;
    if (giver->powerGiveTaps < POWER_GIVE_TAPS_REQUIRED)
        return;

    giver->powerGiveTaps = 0;
    giver->powerGiveWindow = 0;
    receiver = SelectPowerTransferReceiver(giver);
    if (!receiver || GetPlayerPower(giver->initParam) < POWER_GIVE_AMOUNT ||
        !g_ItemManager.CanSpawnItems(6))
        return;
    AddPlayerPower(giver->initParam, -POWER_GIVE_AMOUNT);
    const i32 transferState = GetItemTransferStateForPlayer(receiver->initParam);
    for (i32 index = 0; index < 6; ++index)
    {
        ZunVec3 spawn = giver->positionCenter;
        spawn.x += static_cast<f32>((index % 3) - 1) * 10.0f;
        spawn.y += static_cast<f32>((index / 3) - 1) * 8.0f;
        g_ItemManager.SpawnItem(
            &spawn, index < 2 ? ITEM_POWER_BIG : ITEM_POWER_SMALL, transferState);
    }
    g_Gui.flags.flag2 = 2;
    g_SoundPlayer.PlaySoundByIdx(SOUND_POWERUP);
}

void UpdateLifeTransfer(Player *giver)
{
    if (!giver || !IsLivingTransferPlayer(giver))
    {
        if (giver)
        {
            giver->lifeGiveTimer = 0;
            giver->lifeGiveTargetToken = 0;
        }
        return;
    }

    // One continuous Focus hold may complete at most one life transfer.
    // After a successful rescue/gift, require a real Shift release before
    // another transfer can begin so a held key cannot silently spend a second life.
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
    const i32 targetToken = receiver->initParam + 1;
    if (giver->lifeGiveTargetToken != targetToken)
    {
        giver->lifeGiveTimer = 0;
        giver->lifeGiveTargetToken = targetToken;
    }
    if (giver->powerGiveTaps > 0 || !giver->isFocus ||
        IS_PRESSED_PLAYER(giver, TH_BUTTON_SHOOT))
    {
        giver->lifeGiveTimer = 0;
        return;
    }
    if (++giver->lifeGiveTimer < LIFE_GIVE_HOLD_FRAMES ||
        GetPlayerLives(giver->initParam) <= 0)
        return;

    if (receiver->playerState == PLAYER_STATE_REVIVABLE)
    {
        AddPlayerLives(giver->initParam, -1);
        RevivePlayerFromTeammate(receiver);
        g_Gui.flags.flag0 = 2;
        giver->lifeGiveTimer = 0;
        giver->lifeGiveTargetToken = LIFE_GIVE_WAIT_RELEASE_TOKEN;
        return;
    }

    giver->lifeGiveTimer = 0;
    giver->lifeGiveTargetToken = 0;
    if (!g_ItemManager.CanSpawnItems(1))
        return;
    AddPlayerLives(giver->initParam, -1);
    g_ItemManager.SpawnItem(
        &giver->positionCenter, ITEM_LIFE,
        GetItemTransferStateForPlayer(receiver->initParam));
    g_Gui.flags.flag0 = 2;
    giver->lifeGiveTargetToken = LIFE_GIVE_WAIT_RELEASE_TOKEN;
}

void DrawPowerTransferPrompt(const Player *giver)
{
    if (!giver || !MultiplayerGameplay::IsMultiplayer() ||
        giver->initParam >= TH06_MULTI_MAX_PLAYERS ||
        !IsLivingTransferPlayer(giver) ||
        GetPlayerPower(giver->initParam) < POWER_GIVE_AMOUNT ||
        !SelectPowerTransferReceiver(giver) ||
        giver->powerGiveTaps < POWER_GIVE_PROMPT_AFTER)
        return;

    ZunVec3 position =
        giver->prevPositionCenter.Lerp(giver->positionCenter, g_RenderAlpha) +
        g_RemoteDrawOffsets[giver->initParam];
    position.x += g_GameManager.arcadeRegionTopLeftPos.x - 16.0f;
    position.y += g_GameManager.arcadeRegionTopLeftPos.y + 16.0f;
    position.z = 0.48f;

    const ZunVec2 oldScale = g_AsciiManager.scale;
    const ZunColor oldColor = g_AsciiManager.color;
    const u32 oldGui = g_AsciiManager.isGui;
    const bool oldSelected = g_AsciiManager.isSelected;
    g_AsciiManager.scale = ZunVec2(0.5f, 0.5f);
    g_AsciiManager.color = 0xffa0ffa0;
    g_AsciiManager.isGui = 1;
    g_AsciiManager.isSelected = false;
    g_AsciiManager.AddFormatText(&position, "P %d/%d",
                                 giver->powerGiveTaps,
                                 POWER_GIVE_TAPS_REQUIRED);
    g_AsciiManager.scale = oldScale;
    g_AsciiManager.color = oldColor;
    g_AsciiManager.isGui = oldGui;
    g_AsciiManager.isSelected = oldSelected;
}

void DrawLifeTransferPrompt(const Player *giver)
{
    if (!giver || !MultiplayerGameplay::IsMultiplayer() ||
        giver->initParam >= TH06_MULTI_MAX_PLAYERS ||
        GetPlayerLives(giver->initParam) <= 0 ||
        giver->lifeGiveTargetToken == LIFE_GIVE_WAIT_RELEASE_TOKEN ||
        !SelectLifeTransferReceiver(giver) || !giver->isFocus ||
        IS_PRESSED_PLAYER(giver, TH_BUTTON_SHOOT) || giver->powerGiveTaps > 0)
        return;

    ZunVec3 position =
        giver->prevPositionCenter.Lerp(giver->positionCenter, g_RenderAlpha) +
        g_RemoteDrawOffsets[giver->initParam];
    position.x += g_GameManager.arcadeRegionTopLeftPos.x - 14.0f;
    position.y += g_GameManager.arcadeRegionTopLeftPos.y - 22.0f;
    position.z = 0.48f;

    const ZunVec2 oldScale = g_AsciiManager.scale;
    const ZunColor oldColor = g_AsciiManager.color;
    const u32 oldGui = g_AsciiManager.isGui;
    const bool oldSelected = g_AsciiManager.isSelected;
    g_AsciiManager.scale = ZunVec2(0.6f, 0.6f);
    g_AsciiManager.color = 0xffffff00;
    g_AsciiManager.isGui = 1;
    g_AsciiManager.isSelected = false;
    g_AsciiManager.AddFormatText(
        &position, "%d%%",
        std::clamp(giver->lifeGiveTimer * 100 / LIFE_GIVE_HOLD_FRAMES, 0, 100));
    g_AsciiManager.scale = oldScale;
    g_AsciiManager.color = oldColor;
    g_AsciiManager.isGui = oldGui;
    g_AsciiManager.isSelected = oldSelected;
}

constexpr i32 STAGE_INTRO_NAME_FRAMES = 240;
constexpr f32 STAGE_INTRO_NAME_SCALE = 0.48f;

void DrawStageIntroPlayerName(const Player *player)
{
    static const ZunColor nameColors[TH06_MULTI_MAX_PLAYERS] = {
        0xffffffff, 0xffa0d0ff, 0xffa8ffa8};

    if (!player || !MultiplayerGameplay::IsMultiplayer() ||
        !MultiplayerGameplay::ShouldShowStagePlayerNames() ||
        player->initParam >= TH06_MULTI_MAX_PLAYERS ||
        !IsPlayerGameplayActive(player->initParam) ||
        !g_GameManager.isInMenu ||
        static_cast<i32>(g_GameManager.gameFrames) >= STAGE_INTRO_NAME_FRAMES)
        return;

    const char *name = MultiplayerGameplay::GetPlayerName(player->initParam);
    if (!name || name[0] == '\0')
        return;

    const f32 labelWidth =
        static_cast<f32>(std::strlen(name)) * 8.0f * STAGE_INTRO_NAME_SCALE;
    ZunVec3 position =
        player->prevPositionCenter.Lerp(player->positionCenter, g_RenderAlpha) +
        g_RemoteDrawOffsets[player->initParam];
    position.x -= labelWidth * 0.5f;
    position.y -= 22.0f + static_cast<f32>(player->initParam) * 9.0f;
    if (position.x < 2.0f)
        position.x = 2.0f;
    if (position.x + labelWidth > g_GameManager.arcadeRegionSize.x - 2.0f)
        position.x = g_GameManager.arcadeRegionSize.x - 2.0f - labelWidth;
    position.x += g_GameManager.arcadeRegionTopLeftPos.x;
    position.y += g_GameManager.arcadeRegionTopLeftPos.y;
    position.z = 0.48f;

    const ZunVec2 oldScale = g_AsciiManager.scale;
    const ZunColor oldColor = g_AsciiManager.color;
    const u32 oldGui = g_AsciiManager.isGui;
    const bool oldSelected = g_AsciiManager.isSelected;
    g_AsciiManager.scale = ZunVec2(STAGE_INTRO_NAME_SCALE, STAGE_INTRO_NAME_SCALE);
    g_AsciiManager.color = nameColors[player->initParam];
    g_AsciiManager.isGui = 1;
    g_AsciiManager.isSelected = false;
    g_AsciiManager.AddFormatText(&position, "%s", name);
    g_AsciiManager.scale = oldScale;
    g_AsciiManager.color = oldColor;
    g_AsciiManager.isGui = oldGui;
    g_AsciiManager.isSelected = oldSelected;
}
} // namespace

void UpdateTeamWipeRetryCountdown()
{
    UpdateTeamWipeRetryCountdownImpl();
}

Player *GetPlayerById(u8 playerId)
{
    return playerId < TH06_MULTI_MAX_PLAYERS ? &g_Players[playerId] : nullptr;
}

const Player *GetPlayerByIdConst(u8 playerId)
{
    return playerId < TH06_MULTI_MAX_PLAYERS ? &g_Players[playerId] : nullptr;
}

bool IsPlayerActive(u8 playerId)
{
    return playerId < TH06_MULTI_MAX_PLAYERS && g_PlayerActive[playerId];
}

bool IsPlayerGameplayActive(u8 playerId)
{
    return IsPlayerActive(playerId) &&
           !MultiplayerGameplay::IsPlayerTemporarilyAbsent(playerId);
}

i32 GetActivePlayerCount()
{
    i32 count = 0;
    for (u8 playerId = 0; playerId < TH06_MULTI_MAX_PLAYERS; ++playerId)
        count += g_PlayerActive[playerId] ? 1 : 0;
    return count;
}

bool IsPlayerTerminal(u8 playerId)
{
    return playerId < TH06_MULTI_MAX_PLAYERS &&
           IsTerminalPlayerState(&g_Players[playerId]);
}

Player *GetClosestActivePlayer(const ZunVec3 *position)
{
    if (!position)
        return &g_Player;

    Player *closest = nullptr;
    f32 closestDistance = 0.0f;
    for (u8 playerId = 0; playerId < TH06_MULTI_MAX_PLAYERS; ++playerId)
    {
        Player &player = g_Players[playerId];
        if (!IsPlayerGameplayActive(playerId) ||
            (player.playerState != PLAYER_STATE_ALIVE &&
             player.playerState != PLAYER_STATE_INVULNERABLE))
            continue;

        const f32 dx = player.positionCenter.x - position->x;
        const f32 dy = player.positionCenter.y - position->y;
        const f32 distance = dx * dx + dy * dy;
        if (!closest || distance < closestDistance)
        {
            closest = &player;
            closestDistance = distance;
        }
    }
    // Exact-distance ties stay with the lower stable slot because the loop is
    // ordered and only strict improvements replace the current target.
    return closest ? closest : &g_Player;
}

i32 GetPlayerAnmScript(const Player *player, i32 script)
{
    if (!player || player->initParam == 0)
        return script;
    return script + PlayerAnmOffset(player) - ANM_OFFSET_PLAYER;
}
#endif

// This is TH07 etama2's original texture used by effect 24. Load its extracted
// PNG through TH06's normal decoder so TH07's embedded pixel layout is not
// misinterpreted by TH06.
static void DrawEaglerHitbox(const Player *player, const ZunVec3 &center)
{
    constexpr i32 TEXTURE_SLOT = 63;
    constexpr i32 SPRITE_SLOT = 1900;
    static AnmVm vm;
    static bool initialized = false;
    static bool unavailable = false;
    if (!initialized && !unavailable)
    {
        const ZunResult result = g_AnmManager->LoadTexture(
            TEXTURE_SLOT, "eagler-hitbox.png", TEX_FMT_A8R8G8B8, 0x00000000, true);
        if (result != ZUN_SUCCESS)
        {
            unavailable = true;
            return;
        }
        AnmLoadedSprite sprite = {};
        sprite.sourceFileIndex = TEXTURE_SLOT;
        sprite.startPixelInclusive = ZunVec2(0.0f, 112.0f);
        sprite.endPixelInclusive = ZunVec2(64.0f, 176.0f);
        sprite.textureWidth = sprite.widthPx = 256.0f;
        sprite.textureHeight = sprite.heightPx = 256.0f;
        sprite.spriteId = SPRITE_SLOT;
        g_AnmManager->LoadSprite(SPRITE_SLOT, &sprite);
        g_AnmManager->InitializeAndSetSprite(&vm, SPRITE_SLOT);
        vm.scaleX = vm.scaleY = vm.prevScaleX = vm.prevScaleY = 1.0f;
        vm.flags.blendMode = AnmVmBlendMode_InvSrcAlpha;
        initialized = true;
    }
    vm.pos = vm.prevPos = center;
    const f32 angle = (static_cast<f32>(g_GameManager.gameFrames) + g_RenderAlpha) * 0.03141593f;
    vm.rotation.z = vm.prevRotation.z = angle;
    vm.color = COLOR_SET_ALPHA(vm.color, 255);
    vm.prevColor = COLOR_SET_ALPHA(vm.prevColor, 255);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    const ZunColor originalColor = vm.color;
    const ZunColor originalPrevColor = vm.prevColor;
    if (MultiplayerGameplay::IsMultiplayer())
        ClampVmAlpha(&vm, GetPlayerOverlapAlpha(player));
#endif
    g_AnmManager->Draw(&vm);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    vm.color = originalColor;
    vm.prevColor = originalPrevColor;
#endif
}

static const CharacterData g_CharData[4] = {
    /* ReimuA  */ {4.0, 2.0, 4.0, 2.0, Player::FireBulletReimuA, Player::FireBulletReimuA},
    /* ReimuB  */ {4.0, 2.0, 4.0, 2.0, Player::FireBulletReimuB, Player::FireBulletReimuB},
    /* MarisaA */ {5.0, 2.5, 5.0, 2.5, Player::FireBulletMarisaA, Player::FireBulletMarisaA},
    /* MarisaB */ {5.0, 2.5, 5.0, 2.5, Player::FireBulletMarisaB, Player::FireBulletMarisaB},
};

Player::Player()
{
}

ZunResult Player::RegisterChain(u8 unk)
{
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (unk >= TH06_MULTI_MAX_PLAYERS)
        return ZUN_ERROR;
    if (unk == 0 && g_Supervisor.curState != SUPERVISOR_STATE_GAMEMANAGER_REINIT)
        g_teamWipeRetryFrames = 0;
    Player *p = &g_Players[unk];
#else
    Player *p = &g_Player;
#endif
    std::memset(p, 0, sizeof(Player));

    p->invulnerabilityTimer.InitializeForPopup();
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    p->initParam = unk;
    g_PlayerActive[unk] = true;
#else
    p->unk_9e1 = unk;
#endif
    p->chainCalc = g_Chain.CreateElem((ChainCallback)Player::OnUpdate);
    p->chainDraw1 = g_Chain.CreateElem((ChainCallback)Player::OnDrawHighPrio);
    p->chainDraw2 = g_Chain.CreateElem((ChainCallback)Player::OnDrawLowPrio);
    p->chainCalc->arg = p;
    p->chainDraw1->arg = p;
    p->chainDraw2->arg = p;
    p->chainCalc->addedCallback = (ChainAddedCallback)Player::AddedCallback;
    p->chainCalc->deletedCallback = (ChainDeletedCallback)Player::DeletedCallback;
    if (g_Chain.AddToCalcChain(p->chainCalc, TH_CHAIN_PRIO_CALC_PLAYER))
    {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        g_PlayerActive[unk] = false;
#endif
        return ZUN_ERROR;
    }
    g_Chain.AddToDrawChain(p->chainDraw1, TH_CHAIN_PRIO_DRAW_LOW_PRIO_PLAYER);
    g_Chain.AddToDrawChain(p->chainDraw2, TH_CHAIN_PRIO_DRAW_HIGH_PRIO_PLAYER);
    return ZUN_SUCCESS;
}

void Player::CutChain()
{
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    for (u8 playerId = 0; playerId < TH06_MULTI_MAX_PLAYERS; ++playerId)
    {
        if (!g_PlayerActive[playerId])
            continue;
        Player &player = g_Players[playerId];
        g_Chain.Cut(player.chainCalc);
        player.chainCalc = NULL;
        g_Chain.Cut(player.chainDraw1);
        player.chainDraw1 = NULL;
        g_Chain.Cut(player.chainDraw2);
        player.chainDraw2 = NULL;
        g_PlayerActive[playerId] = false;
    }
#else
    g_Chain.Cut(g_Player.chainCalc);
    g_Player.chainCalc = NULL;
    g_Chain.Cut(g_Player.chainDraw1);
    g_Player.chainDraw1 = NULL;
    g_Chain.Cut(g_Player.chainDraw2);
    g_Player.chainDraw2 = NULL;
#endif
    return;
}

ZunResult Player::AddedCallback(Player *p)
{
    PlayerBullet *curBullet;
    i32 idx;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    const u8 playerCharacter = PlayerCharacter(p);
    const i32 loadoutIndex = PlayerLoadoutIndex(p);
    const i32 playerAnmFile = PlayerAnmFile(p);
    const i32 playerAnmOffset = PlayerAnmOffset(p);
#else
    const u8 playerCharacter = g_GameManager.character;
    const i32 loadoutIndex = g_GameManager.CharacterShotType();
#endif

    switch (playerCharacter)
    {
    case CHARA_REIMU:
        // This is likely an inline function from g_Supervisor returning an i32.
        if ((i32)(g_Supervisor.curState != SUPERVISOR_STATE_GAMEMANAGER_REINIT) &&
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
            g_AnmManager->LoadAnm(playerAnmFile, "data/player00.anm", playerAnmOffset) != ZUN_SUCCESS)
#else
            g_AnmManager->LoadAnm(ANM_FILE_PLAYER, "data/player00.anm", ANM_OFFSET_PLAYER) != ZUN_SUCCESS)
#endif
        {
            return ZUN_ERROR;
        }
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        g_AnmManager->SetAndExecuteScriptIdx(&p->playerSprite, GetPlayerAnmScript(p, ANM_SCRIPT_PLAYER_IDLE));
#else
        g_AnmManager->SetAndExecuteScriptIdx(&p->playerSprite, ANM_SCRIPT_PLAYER_IDLE);
#endif
        break;
    case CHARA_MARISA:
        if ((i32)(g_Supervisor.curState != SUPERVISOR_STATE_GAMEMANAGER_REINIT) &&
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
            g_AnmManager->LoadAnm(playerAnmFile, "data/player01.anm", playerAnmOffset) != ZUN_SUCCESS)
#else
            g_AnmManager->LoadAnm(ANM_FILE_PLAYER, "data/player01.anm", ANM_OFFSET_PLAYER) != ZUN_SUCCESS)
#endif
        {
            return ZUN_ERROR;
        }
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        g_AnmManager->SetAndExecuteScriptIdx(&p->playerSprite, GetPlayerAnmScript(p, ANM_SCRIPT_PLAYER_IDLE));
#else
        g_AnmManager->SetAndExecuteScriptIdx(&p->playerSprite, ANM_SCRIPT_PLAYER_IDLE);
#endif
        break;
    }
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    p->positionCenter.x = PlayerSpawnX(p);
#else
    p->positionCenter.x = g_GameManager.arcadeRegionSize.x / 2.0f;
#endif
    p->positionCenter.y = g_GameManager.arcadeRegionSize.y - 64.0f;
    p->positionCenter.z = 0.49;
    p->prevPositionCenter = p->positionCenter;
    p->prevOrbsPosition[0] = p->orbsPosition[0];
    p->prevOrbsPosition[1] = p->orbsPosition[1];
    p->orbsPosition[0].z = 0.49;
    p->orbsPosition[1].z = 0.49;
    for (idx = 0; idx < ARRAY_SIZE_SIGNED(p->bombRegionSizes); idx++)
    {
        p->bombRegionSizes[idx].x = 0.0;
    }
    p->hitboxSize.x = 1.25;
    p->hitboxSize.y = 1.25;
    p->hitboxSize.z = 5.0;
    p->grabItemSize.x = 12.0;
    p->grabItemSize.y = 12.0;
    p->grabItemSize.z = 5.0;
    p->playerDirection = MOVEMENT_NONE;
    std::memcpy(&p->characterData, &g_CharData[loadoutIndex], sizeof(CharacterData));
    p->characterData.diagonalMovementSpeed = p->characterData.orthogonalMovementSpeed / ZUN_SQRTF(2.0);
    p->characterData.diagonalMovementSpeedFocus = p->characterData.orthogonalMovementSpeedFocus / ZUN_SQRTF(2.0);
    p->fireBulletCallback = p->characterData.fireBulletCallback;
    p->fireBulletFocusCallback = p->characterData.fireBulletFocusCallback;
    // Upstream th06_cancel_muteki replaces the vanilla SPAWNING assignment
    // inside Player::AddedCallback for advanced Practice/Practice Replay.
    p->playerState = PracticeRuntime::AdvancedActive() ? PLAYER_STATE_ALIVE : PLAYER_STATE_SPAWNING;
    p->invulnerabilityTimer.SetCurrent(120);
    p->orbState = ORB_HIDDEN;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    g_AnmManager->SetAndExecuteScriptIdx(&p->orbsSprite[0], GetPlayerAnmScript(p, ANM_SCRIPT_PLAYER_ORB_LEFT));
    g_AnmManager->SetAndExecuteScriptIdx(&p->orbsSprite[1], GetPlayerAnmScript(p, ANM_SCRIPT_PLAYER_ORB_RIGHT));
#else
    g_AnmManager->SetAndExecuteScriptIdx(&p->orbsSprite[0], ANM_SCRIPT_PLAYER_ORB_LEFT);
    g_AnmManager->SetAndExecuteScriptIdx(&p->orbsSprite[1], ANM_SCRIPT_PLAYER_ORB_RIGHT);
#endif
    for (curBullet = &p->bullets[0], idx = 0; idx < ARRAY_SIZE_SIGNED(p->bullets); idx++, curBullet++)
    {
        curBullet->bulletState = 0;
    }
    p->fireBulletTimer.SetCurrent(-1);
    p->bombInfo.calc = g_BombData[loadoutIndex].calc;
    p->bombInfo.draw = g_BombData[loadoutIndex].draw;
    p->bombInfo.isInUse = 0;
    for (idx = 0; idx < ARRAY_SIZE_SIGNED(p->laserTimer); idx++)
    {
        p->laserTimer[idx].InitializeForPopup();
    }
    p->verticalMovementSpeedMultiplierDuringBomb = 1.0;
    p->horizontalMovementSpeedMultiplierDuringBomb = 1.0;
    // th06_set_deathbomb_timer patches this exact vanilla 8 to 6 whenever
    // thPracParam.mode is active.
    p->respawnTimer = PracticeRuntime::AdvancedActive() ? 6 : 8;
    return ZUN_SUCCESS;
}

ZunResult Player::DeletedCallback(Player *p)
{
    if ((i32)(g_Supervisor.curState != SUPERVISOR_STATE_GAMEMANAGER_REINIT))
    {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        g_AnmManager->ReleaseAnm(PlayerAnmFile(p));
#else
        g_AnmManager->ReleaseAnm(ANM_FILE_PLAYER);
#endif
    }
    return ZUN_SUCCESS;
}

void Player::SyncRenderState(Player *p)
{
    p->prevPositionCenter = p->positionCenter;
    p->prevOrbsPosition[0] = p->orbsPosition[0];
    p->prevOrbsPosition[1] = p->orbsPosition[1];
    p->playerSprite.UpdatePrev();
    for (AnmVm &vm : p->orbsSprite)
    {
        vm.UpdatePrev();
    }
    p->bombInfo.UpdatePrev();
    for (PlayerBullet &bullet : p->bullets)
    {
        if (bullet.bulletState != BULLET_STATE_UNUSED)
        {
            bullet.prevPosition = bullet.position;
            bullet.sprite.UpdatePrev();
        }
    }
}

ChainCallbackResult Player::OnUpdate(Player *p)
{
    f32 scaleFactor1, scaleFactor2;
    i32 idx;
    ZunVec3 lastEnemyHit;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    const bool multiplayer = MultiplayerGameplay::IsMultiplayer();
    const i32 livesRemaining = multiplayer ? GetPlayerLives(p->initParam) : g_GameManager.livesRemaining;
    const i32 bombsRemaining = multiplayer ? GetPlayerBombs(p->initParam) : g_GameManager.bombsRemaining;
    const i32 minRequiredDeathbombTimer =
        (PlayerUsedTouch(p) && !PlayerUsedTouchToBomb(p)) ? Touch::DEATHBOMB_TOLERANCE : 0;
#else
    const i32 livesRemaining = g_GameManager.livesRemaining;
    const i32 bombsRemaining = g_GameManager.bombsRemaining;
    const i32 minRequiredDeathbombTimer =
        (Touch::WasUsedThisRun() && !Touch::UsedTouchToBomb()) ? Touch::DEATHBOMB_TOLERANCE : 0;
#endif

    // Keep interpolation endpoints synchronized even when Sakuya's in-game
    // time stop prevents the player simulation from advancing. Returning
    // before these snapshots left stale prev* values that were re-interpolated
    // on every high-refresh presentation frame, producing visible jitter.
    SyncRenderState(p);
    if (g_GameManager.isTimeStopped)
    {
        return CHAIN_CALLBACK_RESULT_CONTINUE;
    }
    for (idx = 0; idx < ARRAY_SIZE_SIGNED(p->bombRegionSizes); idx++)
    {
        p->bombRegionSizes[idx].x = 0.0;
    }
    for (idx = 0; idx < ARRAY_SIZE_SIGNED(p->bombProjectiles); idx++)
    {
        p->bombProjectiles[idx].sizeX = 0.0;
    }
    if (p->bombInfo.isInUse)
    {
        p->bombInfo.calc(p);
    }
    else
    {
        // THOverlay F6 does not invoke the bomb routine directly.  Upstream
        // patches 0x428989/0x4289B4 so the normal bomb path consumes the
        // previous frame's Bomb bit while AutoBomb is active.  The DEAD path
        // below synthesizes that bit into g_CurFrameInput, so it becomes
        // g_LastFrameInput on the following fixed tick.
        const bool bombPressed =
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
            (!multiplayer && PracticeRuntime::OverlayAutoBomb())
                ? ((g_LastFrameInput & TH_BUTTON_BOMB) != 0)
                : WAS_PRESSED_PLAYER(p, TH_BUTTON_BOMB);
#else
            PracticeRuntime::OverlayAutoBomb()
                ? ((g_LastFrameInput & TH_BUTTON_BOMB) != 0)
                : WAS_PRESSED(TH_BUTTON_BOMB);
#endif
        if (!g_Gui.HasCurrentMsgIdx() && p->respawnTimer != 0 && 0 < bombsRemaining &&
            bombPressed && p->bombInfo.calc != NULL &&
             (p->playerState != PLAYER_STATE_DEAD || p->respawnTimer > minRequiredDeathbombTimer))
        {
            g_GameManager.bombsUsed++;
            if (!PracticeRuntime::OverlayInfiniteBombs())
            {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
                if (multiplayer)
                    SetPlayerBombs(p->initParam, bombsRemaining - 1);
                else
#endif
                    g_GameManager.bombsRemaining--;
            }
            g_Gui.flags.flag1 = 2;
            p->bombInfo.isInUse = 1;
            p->bombInfo.timer.SetCurrent(0);
            p->bombInfo.duration = 999;
#ifdef TH_ENABLE_NETPLAY
            Netplay::SideEffects::BombStartScope bombStartSoundScope;
#endif
            p->bombInfo.calc(p);
            g_EnemyManager.spellcardInfo.isCapturing = false;
            g_GameManager.DecreaseSubrank(
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
                multiplayer ? GetMultiplayerRankPenalty(200) :
#endif
                200);
            g_EnemyManager.spellcardInfo.usedBomb = g_EnemyManager.spellcardInfo.isActive != 0;
        }
    }
    if (p->playerState == PLAYER_STATE_DEAD)
    {
        if (p->respawnTimer != 0)
        {
            p->respawnTimer--;
            // Remaining F6 patches at 0x428A94/0x428A9D replace the vanilla
            // load/sub/store with a direct decrement followed by
            //     *(u16*)INPUT_ADDR = TH_BUTTON_BOMB;
            // Preserve that one-tick input ownership exactly.  This is an
            // assignment, not OR: upstream overwrites the current raw word.
            if (
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
                !multiplayer &&
#endif
                PracticeRuntime::OverlayAutoBomb())
                g_CurFrameInput = TH_BUTTON_BOMB;
            if (p->respawnTimer == 0)
            {
                g_GameManager.powerItemCountForScore = 0;
                if (livesRemaining > 0)
                {
                    g_ItemManager.SpawnItem(&p->positionCenter, ITEM_POWER_BIG, 2);
                    g_ItemManager.SpawnItem(&p->positionCenter, ITEM_POWER_SMALL, 2);
                    g_ItemManager.SpawnItem(&p->positionCenter, ITEM_POWER_SMALL, 2);
                    g_ItemManager.SpawnItem(&p->positionCenter, ITEM_POWER_SMALL, 2);
                    g_ItemManager.SpawnItem(&p->positionCenter, ITEM_POWER_SMALL, 2);
                    g_ItemManager.SpawnItem(&p->positionCenter, ITEM_POWER_SMALL, 2);
                    if (!PracticeRuntime::OverlayInfinitePower())
                    {
                        const i32 currentPower =
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
                            multiplayer ? GetPlayerPower(p->initParam) : g_GameManager.currentPower;
#else
                            g_GameManager.currentPower;
#endif
                        if (currentPower <= 16)
                        {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
                            if (multiplayer)
                                SetPlayerPower(p->initParam, 0);
                            else
#endif
                                g_GameManager.currentPower = 0;
                        }
                        else
                        {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
                            if (multiplayer)
                                SetPlayerPower(p->initParam, currentPower - 16);
                            else
#endif
                                g_GameManager.currentPower -= 16;
                        }
                    }
                    g_Gui.flags.flag2 = 2;
                }
                else
                {
                    g_ItemManager.SpawnItem(&p->positionCenter, ITEM_FULL_POWER, 2);
                    g_ItemManager.SpawnItem(&p->positionCenter, ITEM_FULL_POWER, 2);
                    g_ItemManager.SpawnItem(&p->positionCenter, ITEM_FULL_POWER, 2);
                    g_ItemManager.SpawnItem(&p->positionCenter, ITEM_FULL_POWER, 2);
                    g_ItemManager.SpawnItem(&p->positionCenter, ITEM_FULL_POWER, 2);
                    if (!PracticeRuntime::OverlayInfinitePower())
                    {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
                        if (multiplayer)
                            SetPlayerPower(p->initParam, 0);
                        else
#endif
                            g_GameManager.currentPower = 0;
                    }
                    g_Gui.flags.flag2 = 2;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
                    // `extraLives = -1` is TH06's single-player terminal
                    // sentinel for score-based extends. A single teammate
                    // reaching zero stock must not disable the team's shared
                    // score-extend progression while somebody can still play.
                    if (!multiplayer)
#endif
                        g_GameManager.extraLives = -1;
                }
                g_GameManager.DecreaseSubrank(
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
                    multiplayer ? GetMultiplayerRankPenalty(1600) :
#endif
                    1600);
            }
        }
        else
        {
            scaleFactor1 = p->invulnerabilityTimer.AsFramesFloat() / 30.0f;
            p->playerSprite.scaleY = 3.0f * scaleFactor1 + 1.0f;
            p->playerSprite.scaleX = 1.0f - 1.0f * scaleFactor1;
            p->playerSprite.color =
                COLOR_SET_ALPHA(COLOR_WHITE, (u32)(255.0f - p->invulnerabilityTimer.AsFramesFloat() * 255.0f / 30.0f));
            p->playerSprite.flags.blendMode = AnmVmBlendMode_One;
            p->previousHorizontalSpeed = 0.0f;
            p->previousVerticalSpeed = 0.0f;
            if (p->invulnerabilityTimer.AsFrames() >= 30)
            {
                p->playerState = PLAYER_STATE_SPAWNING;
                p->positionCenter.x =
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
                    PlayerSpawnX(p);
#else
                    g_GameManager.arcadeRegionSize.x / 2.0f;
#endif
                p->positionCenter.y = g_GameManager.arcadeRegionSize.y - 64.0f;
                p->positionCenter.z = 0.2;
                p->invulnerabilityTimer.SetCurrent(0);
                p->playerSprite.scaleX = 3.0;
                p->playerSprite.scaleY = 3.0;
                g_AnmManager->SetAndExecuteScriptIdx(
                    &p->playerSprite,
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
                    GetPlayerAnmScript(p, ANM_SCRIPT_PLAYER_IDLE)
#else
                    ANM_SCRIPT_PLAYER_IDLE
#endif
                );
                if (livesRemaining <= 0)
                {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
                    if (multiplayer)
                    {
                        p->playerState = PLAYER_STATE_REVIVABLE;
                        p->bombInfo.isInUse = 0;
                        p->orbState = ORB_HIDDEN;
                        p->bulletGracePeriod = 0;
                        const i32 respawnBombs =
                            (g_GameManager.difficulty < 4 &&
                             g_GameManager.isInPracticeMode == 0)
                                ? g_Supervisor.defaultConfig.bombCount
                                : 3;
                        SetPlayerBombs(p->initParam, respawnBombs);
                        g_Gui.flags.flag1 = 2;
                        p->previousHorizontalSpeed =
                            (g_Rng.GetRandomU16() & 1) ? REVIVABLE_DRIFT_SPEED
                                                       : -REVIVABLE_DRIFT_SPEED;
                        p->previousVerticalSpeed =
                            (g_Rng.GetRandomU16() & 1) ? REVIVABLE_DRIFT_SPEED
                                                       : -REVIVABLE_DRIFT_SPEED;
                        p->isFocus = 0;
                        ResetTransferInputState(p);
                        const i32 recipientId = SelectLowestLifeRecipient(p->initParam);
                        if (recipientId >= 0)
                        {
                            g_ItemManager.SpawnItem(
                                &p->positionCenter, ITEM_LIFE,
                                GetItemTransferStateForPlayer(static_cast<u8>(recipientId)));
                        }
                    }
                    else
#endif
                    g_GameManager.isInRetryMenu = 1;
                }
                else
                {
                    // th06_track_miss is immediately before the stock
                    // decrement, after the deathbomb window has expired.
                    PracticeRuntime::RecordTrackerMiss();
                    if (!PracticeRuntime::OverlayInfiniteLives())
                    {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
                        if (multiplayer)
                            SetPlayerLives(p->initParam, livesRemaining - 1);
                        else
#endif
                            g_GameManager.livesRemaining--;
                    }
                    g_Gui.flags.flag0 = 2;
                    if (g_GameManager.difficulty < 4 && g_GameManager.isInPracticeMode == 0)
                    {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
                        if (multiplayer)
                            SetPlayerBombs(p->initParam, g_Supervisor.defaultConfig.bombCount);
                        else
#endif
                            g_GameManager.bombsRemaining = g_Supervisor.defaultConfig.bombCount;
                    }
                    else
                    {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
                        if (multiplayer)
                            SetPlayerBombs(p->initParam, 3);
                        else
#endif
                            g_GameManager.bombsRemaining = 3;
                    }
                    g_Gui.flags.flag1 = 2;
                    goto spawning;
                }
            }
        }
    }
    else if (p->playerState == PLAYER_STATE_SPAWNING)
    {
    spawning:
        p->bulletGracePeriod = 90;
        scaleFactor2 = 1.0f - p->invulnerabilityTimer.AsFramesFloat() / 30.0f;
        p->playerSprite.scaleY = 2.0f * scaleFactor2 + 1.0f;
        p->playerSprite.scaleX = 1.0f - 1.0f * scaleFactor2;
        p->playerSprite.flags.blendMode = AnmVmBlendMode_One;
        p->verticalMovementSpeedMultiplierDuringBomb = 1.0;
        p->horizontalMovementSpeedMultiplierDuringBomb = 1.0;
        p->playerSprite.color = COLOR_SET_ALPHA(COLOR_WHITE, p->invulnerabilityTimer.AsFrames() * 255 / 30);
        p->respawnTimer = 0;
        if (30 <= p->invulnerabilityTimer.AsFrames())
        {
            p->playerState = PLAYER_STATE_INVULNERABLE;
            p->playerSprite.scaleX = 1.0;
            p->playerSprite.scaleY = 1.0;
            p->playerSprite.color = COLOR_WHITE;
            p->playerSprite.flags.blendMode = AnmVmBlendMode_InvSrcAlpha;
            p->invulnerabilityTimer.SetCurrent(240);
            p->respawnTimer = 6;
        }
    }
    if (p->bulletGracePeriod != 0)
    {
        p->bulletGracePeriod--;
        g_BulletManager.RemoveAllBullets(0);
    }
    if (p->playerState == PLAYER_STATE_INVULNERABLE)
    {
        p->invulnerabilityTimer.Decrement(1);
        if (p->invulnerabilityTimer.AsFrames() <= 0)
        {
            p->playerState = PLAYER_STATE_ALIVE;
            p->invulnerabilityTimer.SetCurrent(0);
            p->playerSprite.flags.colorOp = AnmVmColorOp_Modulate;
            p->playerSprite.color = COLOR_WHITE;
        }
        else if (p->invulnerabilityTimer.AsFrames() % 8 < 2)
        {
            p->playerSprite.flags.colorOp = AnmVmColorOp_Add;
            p->playerSprite.color = 0xff404040;
        }
        else
        {
            p->playerSprite.flags.colorOp = AnmVmColorOp_Modulate;
            p->playerSprite.color = COLOR_WHITE;
        }
    }
    else
    {
        p->invulnerabilityTimer.Tick();
    }
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (multiplayer && p->playerState == PLAYER_STATE_REVIVABLE)
        UpdateRevivableState(p);
#endif
    if (p->playerState == PLAYER_STATE_ALIVE || p->playerState == PLAYER_STATE_INVULNERABLE)
    {
        p->HandlePlayerInputs();
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        if (multiplayer)
        {
            UpdatePowerTransfer(p);
            UpdateLifeTransfer(p);
        }
#endif
    }
    g_AnmManager->ExecuteScript(&p->playerSprite);
    Player::UpdatePlayerBullets(p);
    if (p->orbState != ORB_HIDDEN)
    {
        g_AnmManager->ExecuteScript(&p->orbsSprite[0]);
        g_AnmManager->ExecuteScript(&p->orbsSprite[1]);
    }
    lastEnemyHit.x = -999.0;
    lastEnemyHit.y = -999.0;
    lastEnemyHit.z = 0.0;
    p->positionOfLastEnemyHit = lastEnemyHit;
    Player::UpdateFireBulletsTimer(p);
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

i32 Player::CalcDamageToEnemy(const ZunVec3 *enemyPos, const ZunVec3 *enemyHitboxSize, bool *hitWithLazerDuringBomb)
{
    ZunVec3 bulletTopLeft;
    i32 damage;
    ZunVec3 enemyTopLeft;
    i32 idx;
    PlayerBullet *bullet;

    ZunVec3 bulletBottomRight;
    ZunVec3 enemyBottomRight;

    damage = 0;

    ZunVec3::SetVecCorners(&enemyTopLeft, &enemyBottomRight, enemyPos, enemyHitboxSize);
    bullet = &this->bullets[0];
    if (hitWithLazerDuringBomb)
    {
        *hitWithLazerDuringBomb = false;
    }
    for (idx = 0; idx < ARRAY_SIZE_SIGNED(this->bullets); idx++, bullet++)
    {
        if (bullet->bulletState == BULLET_STATE_UNUSED ||
            bullet->bulletState != BULLET_STATE_FIRED && bullet->bulletType != BULLET_TYPE_2)
        {
            continue;
        }

        ZunVec3::SetVecCorners(&bulletTopLeft, &bulletBottomRight, &bullet->position, &bullet->size);

        if (bulletTopLeft.y > enemyBottomRight.y || bulletTopLeft.x > enemyBottomRight.x ||
            bulletBottomRight.y < enemyTopLeft.y || bulletBottomRight.x < enemyTopLeft.x)
        {
            continue;
        }
        /* Bullet is hitting the enemy */
        if (!this->bombInfo.isInUse)
        {
            damage += bullet->damage;
        }
        else
        {
            damage += bullet->damage / 3 != 0 ? bullet->damage / 3 : 1;
        }

        if (bullet->bulletType == BULLET_TYPE_2)
        {
            bullet->damage = bullet->damage / 4;
            if (bullet->damage == 0)
            {
                bullet->damage = 1;
            }
            switch (
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
                NormalizePlayerAnmScript(this, bullet->sprite.anmFileIndex)
#else
                bullet->sprite.anmFileIndex
#endif
            )
            {
            case ANM_SCRIPT_PLAYER_MARISA_A_ORB_BULLET_1:
                bullet->size.x = 32.0f;
                bullet->size.y = 32.0f;
                break;
            case ANM_SCRIPT_PLAYER_MARISA_A_ORB_BULLET_2:
                bullet->size.x = 42.0f;
                bullet->size.y = 42.0f;
                break;
            case ANM_SCRIPT_PLAYER_MARISA_A_ORB_BULLET_3:
                bullet->size.x = 48.0f;
                bullet->size.y = 48.0f;
                break;
            case ANM_SCRIPT_PLAYER_MARISA_A_ORB_BULLET_4:
                bullet->size.x = 48.0f;
                bullet->size.y = 48.0f;
            }
            if (bullet->unk_140.AsFrames() % 6 == 0)
            {
                g_EffectManager.SpawnParticles(PARTICLE_EFFECT_UNK_5, &bullet->position, 1, COLOR_WHITE);
            }
        }

        if (bullet->bulletType != BULLET_TYPE_LASER)
        {
            if (bullet->bulletState == BULLET_STATE_FIRED)
            {
                g_AnmManager->SetAndExecuteScriptIdx(&bullet->sprite, bullet->sprite.anmFileIndex + 0x20);
                g_EffectManager.SpawnParticles(PARTICLE_EFFECT_UNK_5, &bullet->position, 1, COLOR_WHITE);
                bullet->position.z = 0.1;
            }
            bullet->bulletState = BULLET_STATE_COLLIDED;
            bullet->velocity.x /= 8.0f;
            bullet->velocity.y /= 8.0f;
        }
        else
        {
            this->unk_9e4++;
            if (this->unk_9e4 % 8 == 0)
            {
                bulletTopLeft = *enemyPos;
                bulletTopLeft.x = bullet->position.x;

                g_EffectManager.SpawnParticles(PARTICLE_EFFECT_UNK_5, &bulletTopLeft, 1, COLOR_WHITE);
            }
        }
    }
    for (idx = 0; idx < ARRAY_SIZE_SIGNED(this->bombRegionSizes); idx++)
    {
        if (this->bombRegionSizes[idx].x <= 0.0f)
        {
            continue;
        }

        bulletTopLeft = this->bombRegionPositions[idx] - this->bombRegionSizes[idx] / 2.0f;
        bulletBottomRight = this->bombRegionPositions[idx] + this->bombRegionSizes[idx] / 2.0f;
        if (bulletTopLeft.x > enemyBottomRight.x || bulletBottomRight.x < enemyTopLeft.x ||
            bulletTopLeft.y > enemyBottomRight.y || bulletBottomRight.y < enemyTopLeft.y)
        {
            continue;
        }
        damage += this->bombRegionDamages[idx];
        this->unk_838[idx] += this->bombRegionDamages[idx];
        this->unk_9e4++;
        if (this->unk_9e4 % 4 == 0)
        {
            g_EffectManager.SpawnParticles(PARTICLE_EFFECT_UNK_3, enemyPos, 1, COLOR_WHITE);
        }
        if (this->bombInfo.isInUse && hitWithLazerDuringBomb)
        {
            *hitWithLazerDuringBomb = true;
        }
    }
    return damage;
}

void Player::UpdatePlayerBullets(Player *player)
{
    ZunVec2 vector;
    PlayerBullet *bullet;
    f32 vecLength;
    i32 idx;

    for (idx = 0; idx < ARRAY_SIZE_SIGNED(player->laserTimer); idx++)
    {
        if (player->laserTimer[idx].AsFrames() != 0)
        {
            player->laserTimer[idx].Decrement(1);
        }
    }
    bullet = &player->bullets[0];
    for (idx = 0; idx < ARRAY_SIZE_SIGNED(player->bullets); idx++, bullet++)
    {
        if (bullet->bulletState == BULLET_STATE_UNUSED)
        {
            continue;
        }

        switch (bullet->bulletType)
        {
        case BULLET_TYPE_1:
            if (bullet->bulletState == BULLET_STATE_FIRED)
            {
                if (player->positionOfLastEnemyHit.x > -100.0f && bullet->unk_140.AsFrames() < 40 &&
                    bullet->unk_140.HasTicked())
                {
                    vector.x = player->positionOfLastEnemyHit.x - bullet->position.x;
                    vector.y = player->positionOfLastEnemyHit.y - bullet->position.y;

                    vecLength = vector.VectorLength() / (bullet->unk_134.y / 4.0f);
                    if (vecLength < 1.0f)
                    {
                        vecLength = 1.0f;
                    }

                    vector.x = vector.x / vecLength + bullet->velocity.x;
                    vector.y = vector.y / vecLength + bullet->velocity.y;

                    vecLength = vector.VectorLengthF64();

                    bullet->unk_134.y = ZUN_MIN(vecLength, 10.0f);

                    if (bullet->unk_134.y < 1.0f)
                    {
                        bullet->unk_134.y = 1.0f;
                    }

                    bullet->velocity.x = (vector.x * bullet->unk_134.y) / vecLength;
                    bullet->velocity.y = (vector.y * bullet->unk_134.y) / vecLength;
                }
                else
                {
                    if (bullet->unk_134.y < 10.0f)
                    {
                        bullet->unk_134.y += 0.33333333f;
                        vector.x = bullet->velocity.x;
                        vector.y = bullet->velocity.y;
                        vecLength = vector.VectorLengthF64();
                        bullet->velocity.x = vector.x * bullet->unk_134.y / vecLength;
                        bullet->velocity.y = vector.y * bullet->unk_134.y / vecLength;
                    }
                }
            }

            break;

        case BULLET_TYPE_2:
            if (bullet->bulletState == BULLET_STATE_FIRED)
            {
                bullet->velocity.y -= 0.3f;
            }
            break;
        case BULLET_TYPE_LASER:

            if (player->laserTimer[bullet->unk_152] == 70)
            {
                bullet->sprite.pendingInterrupt = 1;
            }
            else if (player->laserTimer[bullet->unk_152] == 1)
            {
                bullet->sprite.pendingInterrupt = 1;
            }

            bullet->position = player->orbsPosition[bullet->spawnPositionIdx - 1];

            bullet->position.x += bullet->sidewaysMotion;
            bullet->position.y /= 2.0f;
            bullet->position.z = 0.44f;

            bullet->sprite.scaleY = (bullet->position.y * 2) / 14.0f;

            bullet->size.y = bullet->position.y * 2;
            break;
        }

        bullet->MoveHorizontal(&bullet->position.x);

        bullet->MoveVertical(&bullet->position.y);

        bullet->sprite.pos.z = bullet->position.z;
        if (bullet->bulletType != BULLET_TYPE_LASER &&
            !g_GameManager.IsInBounds(bullet->position.x, bullet->position.y, bullet->sprite.sprite->widthPx,
                                      bullet->sprite.sprite->heightPx))
        {
            bullet->bulletState = BULLET_STATE_UNUSED;
        }

        if (g_AnmManager->ExecuteScript(&bullet->sprite))
        {
            bullet->bulletState = BULLET_STATE_UNUSED;
        }
        bullet->unk_140.Tick();
    }
}

ChainCallbackResult Player::OnDrawHighPrio(Player *p)
{
    Player::DrawBullets(p);
    if (p->bombInfo.isInUse != 0 && p->bombInfo.draw != NULL)
    {
        p->bombInfo.draw(p);
    }
    const ZunVec3 logicalDrawPlayerPosition =
        p->prevPositionCenter.Lerp(p->positionCenter, g_RenderAlpha);
    ZunVec3 drawPlayerPosition = logicalDrawPlayerPosition;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    drawPlayerPosition = PresentRemotePlayer(p, logicalDrawPlayerPosition);
    const ZunVec3 remoteDrawOffset = drawPlayerPosition - logicalDrawPlayerPosition;
#endif
    const ZunVec3 playerSpritePos = p->playerSprite.pos;
    p->playerSprite.pos.x = g_GameManager.arcadeRegionTopLeftPos.x + drawPlayerPosition.x;
    p->playerSprite.pos.y = g_GameManager.arcadeRegionTopLeftPos.y + drawPlayerPosition.y;
    p->playerSprite.pos.z = 0.49;
    if (!g_GameManager.isInRetryMenu
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        && p->playerState != PLAYER_STATE_ELIMINATED
#endif
    )
    {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        const ZunColor originalPlayerColor = p->playerSprite.color;
        const ZunColor originalPlayerPrevColor = p->playerSprite.prevColor;
        if (MultiplayerGameplay::IsMultiplayer())
        {
            const u8 normalAlpha = GetPlayerOverlapAlpha(p);
            const u8 presentationAlpha = GetPlayerRescuePresentationAlpha(p, normalAlpha);
            if (p->playerState == PLAYER_STATE_REVIVABLE)
            {
                p->playerSprite.color = COLOR_SET_ALPHA(p->playerSprite.color, presentationAlpha);
                p->playerSprite.prevColor = COLOR_SET_ALPHA(p->playerSprite.prevColor, presentationAlpha);
            }
            else
            {
                ClampVmAlpha(&p->playerSprite, presentationAlpha);
            }
        }
#endif
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        if (MultiplayerGameplay::IsMultiplayer())
            DrawLocalPlayerLocator(p, drawPlayerPosition);
#endif
        g_AnmManager->DrawNoRotation(&p->playerSprite);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        p->playerSprite.color = originalPlayerColor;
        p->playerSprite.prevColor = originalPlayerPrevColor;
#endif
        if (p->orbState != ORB_HIDDEN &&
            (p->playerState == PLAYER_STATE_ALIVE || p->playerState == PLAYER_STATE_INVULNERABLE))
        {
            const ZunVec3 orb0Pos = p->orbsSprite[0].pos;
            const ZunVec3 orb1Pos = p->orbsSprite[1].pos;
            p->orbsSprite[0].pos = p->prevOrbsPosition[0].Lerp(p->orbsPosition[0], g_RenderAlpha);
            p->orbsSprite[1].pos = p->prevOrbsPosition[1].Lerp(p->orbsPosition[1], g_RenderAlpha);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
            p->orbsSprite[0].pos += remoteDrawOffset;
            p->orbsSprite[1].pos += remoteDrawOffset;
#endif
            f32 *x1 = &p->orbsSprite[0].pos.x;
            *x1 += g_GameManager.arcadeRegionTopLeftPos.x;
            f32 *y1 = &p->orbsSprite[0].pos.y;
            *y1 += g_GameManager.arcadeRegionTopLeftPos.y;
            f32 *x2 = &p->orbsSprite[1].pos.x;
            *x2 += g_GameManager.arcadeRegionTopLeftPos.x;
            f32 *y2 = &p->orbsSprite[1].pos.y;
            *y2 += g_GameManager.arcadeRegionTopLeftPos.y;
            p->orbsSprite[0].pos.z = 0.491;
            p->orbsSprite[1].pos.z = 0.491;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
            const ZunColor originalOrb0Color = p->orbsSprite[0].color;
            const ZunColor originalOrb0PrevColor = p->orbsSprite[0].prevColor;
            const ZunColor originalOrb1Color = p->orbsSprite[1].color;
            const ZunColor originalOrb1PrevColor = p->orbsSprite[1].prevColor;
            if (MultiplayerGameplay::IsMultiplayer())
            {
                const u8 proximityAlpha = GetPlayerOverlapAlpha(p);
                ClampVmAlpha(&p->orbsSprite[0], proximityAlpha);
                ClampVmAlpha(&p->orbsSprite[1], proximityAlpha);
            }
#endif
            g_AnmManager->Draw(&p->orbsSprite[0]);
            g_AnmManager->Draw(&p->orbsSprite[1]);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
            p->orbsSprite[0].color = originalOrb0Color;
            p->orbsSprite[0].prevColor = originalOrb0PrevColor;
            p->orbsSprite[1].color = originalOrb1Color;
            p->orbsSprite[1].prevColor = originalOrb1PrevColor;
#endif
            p->orbsSprite[0].pos = orb0Pos;
            p->orbsSprite[1].pos = orb1Pos;
        }
        if ((EaglerOptions::AlwaysShowHitbox() ||
             (EaglerOptions::ShowTh06FocusHitbox() && p->isFocus)) &&
            (p->playerState == PLAYER_STATE_ALIVE || p->playerState == PLAYER_STATE_INVULNERABLE))
        {
            DrawEaglerHitbox(
                p, ZunVec3(g_GameManager.arcadeRegionTopLeftPos.x + drawPlayerPosition.x,
                           g_GameManager.arcadeRegionTopLeftPos.y + drawPlayerPosition.y, 0.488f));
        }
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        if (MultiplayerGameplay::IsMultiplayer())
        {
            DrawLifeTransferPrompt(p);
            DrawPowerTransferPrompt(p);
            DrawStageIntroPlayerName(p);
        }
#endif
    }
    p->playerSprite.pos = playerSpritePos;
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

ChainCallbackResult Player::OnDrawLowPrio(Player *p)
{
    Player::DrawBulletExplosions(p);
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}

ZunResult Player::HandlePlayerInputs()
{
    float intermediateFloat;

    float *posCenterY;
    float *posCenterX;
    float horizontalOrbOffset;
    float verticalOrbOffset;

    float horizontalSpeed = 0.0;
    float verticalSpeed = 0.0;
    float touchDx = 0.0f;
    float touchDy = 0.0f;
    float joystickX = 0.0f;
    float joystickY = 0.0f;
    bool sampledReplayTouch = false;
    bool sampledNetplayTouch = false;
    bool touchUnlimited = false;
    const bool replayPlayback = g_GameManager.isInReplay != 0;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    const bool canSampleRawTouch = CanSampleRawTouchForPlayer(this);
    const bool allowRawTouchFallback =
        canSampleRawTouch && !MultiplayerGameplay::IsMultiplayer();
#else
    const bool canSampleRawTouch = true;
    const bool allowRawTouchFallback = true;
#endif
#ifdef TH_ENABLE_NETPLAY
    const bool speculative = Netplay::SideEffects::IsSpeculative();
#else
    const bool speculative = false;
#endif
    PlayerDirection playerDirection = this->playerDirection;

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
    else
    {
        if (IS_PRESSED_PLAYER(this, TH_BUTTON_DOWN))
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
    }
    if (IS_PRESSED_PLAYER(this, TH_BUTTON_FOCUS))
    {
        this->isFocus = true;
    }
    else
    {
        this->isFocus = false;
    }

    switch (this->playerDirection)
    {
    case MOVEMENT_NONE:
        break;
    case MOVEMENT_RIGHT:
        if (IS_PRESSED_PLAYER(this, TH_BUTTON_FOCUS))
        {
            horizontalSpeed = this->characterData.orthogonalMovementSpeedFocus;
        }
        else
        {
            horizontalSpeed = this->characterData.orthogonalMovementSpeed;
        }
        break;
    case MOVEMENT_LEFT:
        if (IS_PRESSED_PLAYER(this, TH_BUTTON_FOCUS))
        {
            horizontalSpeed = -this->characterData.orthogonalMovementSpeedFocus;
        }
        else
        {
            horizontalSpeed = -this->characterData.orthogonalMovementSpeed;
        }
        break;
    case MOVEMENT_UP:
        if (IS_PRESSED_PLAYER(this, TH_BUTTON_FOCUS))
        {
            verticalSpeed = -this->characterData.orthogonalMovementSpeedFocus;
        }
        else
        {
            verticalSpeed = -this->characterData.orthogonalMovementSpeed;
        }
        break;
    case MOVEMENT_DOWN:
        if (IS_PRESSED_PLAYER(this, TH_BUTTON_FOCUS))
        {
            verticalSpeed = this->characterData.orthogonalMovementSpeedFocus;
        }
        else
        {
            verticalSpeed = this->characterData.orthogonalMovementSpeed;
        }
        break;
    case MOVEMENT_UP_LEFT:
        if (IS_PRESSED_PLAYER(this, TH_BUTTON_FOCUS))
        {
            horizontalSpeed = -this->characterData.diagonalMovementSpeedFocus;
        }
        else
        {
            horizontalSpeed = -this->characterData.diagonalMovementSpeed;
        }
        verticalSpeed = horizontalSpeed;
        break;
    case MOVEMENT_DOWN_LEFT:
        if (IS_PRESSED_PLAYER(this, TH_BUTTON_FOCUS))
        {
            horizontalSpeed = -this->characterData.diagonalMovementSpeedFocus;
        }
        else
        {
            horizontalSpeed = -this->characterData.diagonalMovementSpeed;
        }
        verticalSpeed = -horizontalSpeed;
        break;
    case MOVEMENT_UP_RIGHT:
        if (IS_PRESSED_PLAYER(this, TH_BUTTON_FOCUS))
        {
            horizontalSpeed = this->characterData.diagonalMovementSpeedFocus;
        }
        else
        {
            horizontalSpeed = this->characterData.diagonalMovementSpeed;
        }
        verticalSpeed = -horizontalSpeed;
        break;
    case MOVEMENT_DOWN_RIGHT:
        if (IS_PRESSED_PLAYER(this, TH_BUTTON_FOCUS))
        {
            horizontalSpeed = this->characterData.diagonalMovementSpeedFocus;
        }
        else
        {
            horizontalSpeed = this->characterData.diagonalMovementSpeed;
        }
        verticalSpeed = horizontalSpeed;
    }

    if (!speculative && canSampleRawTouch)
        ReplayExtension::BeginInputFrame();
    if (replayPlayback && canSampleRawTouch && ReplayExtension::GetPlaybackJoystick(&joystickX, &joystickY))
    {
        const f32 maxSpeed = this->isFocus ? this->characterData.orthogonalMovementSpeedFocus
                                           : this->characterData.orthogonalMovementSpeed;
        horizontalSpeed = joystickX * maxSpeed;
        verticalSpeed = joystickY * maxSpeed;
        this->playerDirection = MOVEMENT_NONE;
    }
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    else if (replayPlayback && ReplayExtension::MultiplayerPlaybackActive() &&
             Netplay::Input::ReplayJoystick(this->initParam, &joystickX, &joystickY))
    {
        // Multiplayer Replay stores one authoritative FrameInput lane per
        // player. During playback those lanes are installed by ReplayManager
        // as Player input overrides, so consume the per-player analog lane
        // even though GameManager is in Replay mode. The ordinary Replay
        // sidecar above intentionally rejects multiplayer playback.
        const f32 maxSpeed = this->isFocus ? this->characterData.orthogonalMovementSpeedFocus
                                           : this->characterData.orthogonalMovementSpeed;
        horizontalSpeed = joystickX * maxSpeed;
        verticalSpeed = joystickY * maxSpeed;
        this->playerDirection = MOVEMENT_NONE;
    }
#endif
    else if (!replayPlayback &&
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
             (Netplay::Input::ReplayJoystick(this->initParam, &joystickX, &joystickY) ||
#endif
              (allowRawTouchFallback &&
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
               !Netplay::Input::PlayerButtonOverridesActive() &&
#endif
               Touch::GetFreeJoystickVector(&joystickX, &joystickY))
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
             )
#endif
    )
    {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        if (!Netplay::Input::ReplayOverrideActive() && canSampleRawTouch)
            Netplay::Input::CaptureJoystick(joystickX, joystickY);
#endif
        if (!speculative && canSampleRawTouch)
            ReplayExtension::CaptureJoystick(joystickX, joystickY);
        const f32 maxSpeed = this->isFocus ? this->characterData.orthogonalMovementSpeedFocus
                                           : this->characterData.orthogonalMovementSpeed;
        horizontalSpeed = joystickX * maxSpeed;
        verticalSpeed = joystickY * maxSpeed;
        this->playerDirection = MOVEMENT_NONE;
    }
    else if ((replayPlayback && canSampleRawTouch &&
              (sampledReplayTouch = ReplayExtension::GetPlaybackDirectTouch(&touchDx, &touchDy, &touchUnlimited))) ||
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
             (replayPlayback && ReplayExtension::MultiplayerPlaybackActive() &&
              (sampledNetplayTouch = Netplay::Input::ReplayDirectTouch(
                    this->initParam, &touchDx, &touchDy, &touchUnlimited))) ||
#endif
             (!replayPlayback &&
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
              ((sampledNetplayTouch = Netplay::Input::ReplayDirectTouch(
                    this->initParam, &touchDx, &touchDy, &touchUnlimited)) ||
#endif
               (allowRawTouchFallback &&
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
                !Netplay::Input::PlayerButtonOverridesActive() &&
#endif
                Touch::GetPlayerDelta(&touchDx, &touchDy))
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
              )
#endif
             ))
    {
        if (!sampledReplayTouch && !sampledNetplayTouch)
        {
            touchUnlimited = Touch::IsUnlimited();
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
            Netplay::Input::CaptureDirectTouch(touchDx, touchDy, touchUnlimited);
#endif
        }
        // Match TH07's synchronized-touch ownership rule. Once the local raw
        // touch has been copied into the deterministic netplay lane, the same
        // logical sample may be read back through ReplayDirectTouch on this
        // fixed tick. It still owns the local raw delta and must consume it;
        // otherwise Android keeps replaying the same displacement on later
        // ticks and movement feels sticky/over-amplified.
        if (!speculative && !sampledReplayTouch && canSampleRawTouch)
            ReplayExtension::CaptureDirectTouch(touchDx, touchDy, touchUnlimited);

        const bool sampledLogicalTouch = sampledReplayTouch || sampledNetplayTouch;
        const bool consumeSynchronizedLocalTouch =
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
            sampledNetplayTouch && !speculative && canSampleRawTouch &&
            this->initParam == MultiplayerGameplay::GetLocalPlayerSlot();
#else
            false;
#endif
        f32 focusRatio = 1.0f;
        if (!touchUnlimited && this->isFocus &&
            this->characterData.orthogonalMovementSpeed != 0.0f)
        {
            focusRatio = this->characterData.orthogonalMovementSpeedFocus /
                         this->characterData.orthogonalMovementSpeed;
        }

        f32 reqGameDx = touchDx * focusRatio;
        f32 reqGameDy = touchDy * focusRatio;

        const f32 minX = g_GameManager.playerMovementAreaTopLeftPos.x;
        const f32 maxX = minX + g_GameManager.playerMovementAreaSize.x;
        const f32 minY = g_GameManager.playerMovementAreaTopLeftPos.y;
        const f32 maxY = minY + g_GameManager.playerMovementAreaSize.y;

        const f32 targetX = this->positionCenter.x + reqGameDx;
        const f32 targetY = this->positionCenter.y + reqGameDy;
        if (targetX < minX)
        {
            reqGameDx = minX - this->positionCenter.x;
        }
        else if (targetX > maxX)
        {
            reqGameDx = maxX - this->positionCenter.x;
        }
        if (targetY < minY)
        {
            reqGameDy = minY - this->positionCenter.y;
        }
        else if (targetY > maxY)
        {
            reqGameDy = maxY - this->positionCenter.y;
        }

        if (focusRatio != 0.0f &&
            (!sampledLogicalTouch || consumeSynchronizedLocalTouch))
        {
            Touch::SetPlayerDelta(reqGameDx / focusRatio, reqGameDy / focusRatio);
        }

        const f32 hx = this->horizontalMovementSpeedMultiplierDuringBomb *
                       g_Supervisor.effectiveFramerateMultiplier;
        const f32 vy = this->verticalMovementSpeedMultiplierDuringBomb *
                       g_Supervisor.effectiveFramerateMultiplier;
        const f32 requestedHorizontalSpeed = hx != 0.0f ? reqGameDx / hx : 0.0f;
        const f32 requestedVerticalSpeed = vy != 0.0f ? reqGameDy / vy : 0.0f;
        const f32 currentSpeedSq = requestedHorizontalSpeed * requestedHorizontalSpeed +
                                   requestedVerticalSpeed * requestedVerticalSpeed;
        const f32 maxSpeed = this->isFocus ? this->characterData.orthogonalMovementSpeedFocus
                                           : this->characterData.orthogonalMovementSpeed;
        const f32 effectiveMaxSpeed = touchUnlimited ? std::sqrt(currentSpeedSq) : maxSpeed;

        if (!touchUnlimited && currentSpeedSq > effectiveMaxSpeed * effectiveMaxSpeed &&
            currentSpeedSq > 0.0f)
        {
            const f32 currentSpeed = std::sqrt(currentSpeedSq);
            horizontalSpeed = requestedHorizontalSpeed / currentSpeed * effectiveMaxSpeed;
            verticalSpeed = requestedVerticalSpeed / currentSpeed * effectiveMaxSpeed;
        }
        else
        {
            horizontalSpeed = requestedHorizontalSpeed;
            verticalSpeed = requestedVerticalSpeed;
        }

        const f32 consumedGameDx = hx != 0.0f ? horizontalSpeed * hx : 0.0f;
        const f32 consumedGameDy = vy != 0.0f ? verticalSpeed * vy : 0.0f;
        if (focusRatio != 0.0f &&
            (!sampledLogicalTouch || consumeSynchronizedLocalTouch))
        {
            if (!touchUnlimited &&
                currentSpeedSq > effectiveMaxSpeed * effectiveMaxSpeed && currentSpeedSq > 0.0f)
            {
                const f32 consumeX = hx != 0.0f ? consumedGameDx / focusRatio : touchDx;
                const f32 consumeY = vy != 0.0f ? consumedGameDy / focusRatio : touchDy;
                Touch::ConsumePlayerDelta(consumeX, consumeY);
            }
            else
            {
                Touch::SetPlayerDelta(0.0f, 0.0f);
            }
        }

        this->playerDirection = MOVEMENT_NONE;
    }

    if (horizontalSpeed < 0.0f && this->previousHorizontalSpeed >= 0.0f)
    {
        g_AnmManager->SetAndExecuteScriptIdx(&this->playerSprite,
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
                                             GetPlayerAnmScript(this, ANM_SCRIPT_PLAYER_MOVING_LEFT)
#else
                                             ANM_SCRIPT_PLAYER_MOVING_LEFT
#endif
        );
    }
    else if (!horizontalSpeed && this->previousHorizontalSpeed < 0.0f)
    {
        g_AnmManager->SetAndExecuteScriptIdx(&this->playerSprite,
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
                                             GetPlayerAnmScript(this, ANM_SCRIPT_PLAYER_STOPPING_LEFT)
#else
                                             ANM_SCRIPT_PLAYER_STOPPING_LEFT
#endif
        );
    }

    if (horizontalSpeed > 0.0f && this->previousHorizontalSpeed <= 0.0f)
    {
        g_AnmManager->SetAndExecuteScriptIdx(&this->playerSprite,
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
                                             GetPlayerAnmScript(this, ANM_SCRIPT_PLAYER_MOVING_RIGHT)
#else
                                             ANM_SCRIPT_PLAYER_MOVING_RIGHT
#endif
        );
    }
    else if (!horizontalSpeed && this->previousHorizontalSpeed > 0.0f)
    {
        g_AnmManager->SetAndExecuteScriptIdx(&this->playerSprite,
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
                                             GetPlayerAnmScript(this, ANM_SCRIPT_PLAYER_STOPPING_RIGHT)
#else
                                             ANM_SCRIPT_PLAYER_STOPPING_RIGHT
#endif
        );
    }

    this->previousHorizontalSpeed = horizontalSpeed;
    this->previousVerticalSpeed = verticalSpeed;

    // TODO: Match stack variables here
    posCenterX = &this->positionCenter.x;
    *posCenterX +=
        horizontalSpeed * this->horizontalMovementSpeedMultiplierDuringBomb * g_Supervisor.effectiveFramerateMultiplier;
    posCenterY = &this->positionCenter.y;
    *posCenterY +=
        verticalSpeed * this->verticalMovementSpeedMultiplierDuringBomb * g_Supervisor.effectiveFramerateMultiplier;

    if (this->positionCenter.x < g_GameManager.playerMovementAreaTopLeftPos.x)
    {
        this->positionCenter.x = g_GameManager.playerMovementAreaTopLeftPos.x;
    }
    else if (g_GameManager.playerMovementAreaTopLeftPos.x + g_GameManager.playerMovementAreaSize.x <
             this->positionCenter.x)
    {
        this->positionCenter.x = g_GameManager.playerMovementAreaTopLeftPos.x + g_GameManager.playerMovementAreaSize.x;
    }

    if (this->positionCenter.y < g_GameManager.playerMovementAreaTopLeftPos.y)
    {
        this->positionCenter.y = g_GameManager.playerMovementAreaTopLeftPos.y;
    }
    else if (g_GameManager.playerMovementAreaTopLeftPos.y + g_GameManager.playerMovementAreaSize.y <
             this->positionCenter.y)
    {
        this->positionCenter.y = g_GameManager.playerMovementAreaTopLeftPos.y + g_GameManager.playerMovementAreaSize.y;
    }

    this->hitboxTopLeft = this->positionCenter - this->hitboxSize;

    this->hitboxBottomRight = this->positionCenter + this->hitboxSize;

    this->grabItemTopLeft = this->positionCenter - this->grabItemSize;

    this->grabItemBottomRight = this->positionCenter + this->grabItemSize;

    this->orbsPosition[0] = this->positionCenter;
    this->orbsPosition[1] = this->positionCenter;

    verticalOrbOffset = 0.0;
    horizontalOrbOffset = verticalOrbOffset;

    const i32 playerPower =
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        MultiplayerGameplay::IsMultiplayer() ? GetPlayerPower(this->initParam) : g_GameManager.currentPower;
#else
        g_GameManager.currentPower;
#endif
    if (playerPower < 8)
    {
        this->orbState = ORB_HIDDEN;
    }
    else if (this->orbState == ORB_HIDDEN)
    {
        this->orbState = ORB_UNFOCUSED;
    }

    switch (this->orbState)
    {
    case ORB_HIDDEN:
        this->focusMovementTimer.InitializeForPopup();
        break;

    case ORB_UNFOCUSED:
        horizontalOrbOffset = 24.0;
        this->focusMovementTimer.InitializeForPopup();
        if (this->isFocus)
        {
            this->orbState = ORB_FOCUSING;
        }
        else
        {
            break;
        }

    CASE_ORB_FOCUSING:
    case ORB_FOCUSING:
        this->focusMovementTimer.Tick();

        intermediateFloat = this->focusMovementTimer.AsFramesFloat() / 8.0f;
        verticalOrbOffset = (1.0f - intermediateFloat) * 32.0f + -32.0f;
        intermediateFloat *= intermediateFloat;
        horizontalOrbOffset = -16.0f * intermediateFloat + 24.0f;

        if (this->focusMovementTimer.current >= 8)
        {
            this->orbState = ORB_FOCUSED;
        }
        if (!this->isFocus)
        {

            this->orbState = ORB_UNFOCUSING;
            this->focusMovementTimer.SetCurrent(8 - this->focusMovementTimer.AsFrames());

            goto CASE_ORB_UNFOCUSING;
        }
        else
        {
            break;
        }

    case ORB_FOCUSED:
        horizontalOrbOffset = 8.0;
        verticalOrbOffset = -32.0;
        this->focusMovementTimer.InitializeForPopup();
        if (!this->isFocus)
        {
            this->orbState = ORB_UNFOCUSING;
        }
        else
        {
            break;
        }

    CASE_ORB_UNFOCUSING:
    case ORB_UNFOCUSING:
        this->focusMovementTimer.Tick();

        intermediateFloat = this->focusMovementTimer.AsFramesFloat() / 8.0f;
        verticalOrbOffset = (32.0f * intermediateFloat) + -32.0f;
        intermediateFloat *= intermediateFloat;
        intermediateFloat = 1.0f - intermediateFloat;
        horizontalOrbOffset = -16.0f * intermediateFloat + 24.0f;
        if (this->focusMovementTimer.current >= 8)
        {
            this->orbState = ORB_UNFOCUSED;
        }
        if (this->isFocus)
        {
            this->orbState = ORB_FOCUSING;
            this->focusMovementTimer.SetCurrent(8 - this->focusMovementTimer.AsFrames());
            goto CASE_ORB_FOCUSING;
        }
    }

    this->orbsPosition[0].x -= horizontalOrbOffset;
    this->orbsPosition[1].x += horizontalOrbOffset;
    this->orbsPosition[0].y += verticalOrbOffset;
    this->orbsPosition[1].y += verticalOrbOffset;
    if (IS_PRESSED_PLAYER(this, TH_BUTTON_SHOOT) && !g_Gui.HasCurrentMsgIdx())
    {
        this->StartFireBulletTimer(this);
    }
    this->previousFrameInput =
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        g_CurFrameGameInputs[this->initParam];
#else
        g_CurFrameInput;
#endif
    return ZUN_SUCCESS;
}

void Player::DrawBullets(Player *p)
{
    i32 bulletIdx;
    PlayerBullet *bullets;

    bullets = p->bullets;
    for (bulletIdx = 0; bulletIdx < ARRAY_SIZE_SIGNED(p->bullets); bulletIdx++, bullets++)
    {
        if (bullets->bulletState != BULLET_STATE_FIRED)
        {
            continue;
        }
        const f32 savedRotationZ = bullets->sprite.rotation.z;
        const f32 savedPrevRotationZ = bullets->sprite.prevRotation.z;
        if (bullets->sprite.autoRotate)
        {
            bullets->sprite.prevRotation.z =
                ZUN_PI / 2 - utils::AddNormalizeAngle(bullets->unk_134.z, ZUN_PI);
            bullets->sprite.rotation.z = ZUN_PI / 2 - utils::AddNormalizeAngle(bullets->unk_134.z, ZUN_PI);
        }
        const ZunVec3 savedPos = bullets->sprite.pos;
        bullets->sprite.pos = bullets->prevPosition.Lerp(bullets->position, g_RenderAlpha);
        g_AnmManager->Draw2(&bullets->sprite);
        bullets->sprite.pos = savedPos;
        bullets->sprite.rotation.z = savedRotationZ;
        bullets->sprite.prevRotation.z = savedPrevRotationZ;
    }
}

void Player::DrawBulletExplosions(Player *p)
{
    i32 bulletIdx;
    PlayerBullet *bullets;

    bullets = p->bullets;
    for (bulletIdx = 0; bulletIdx < ARRAY_SIZE_SIGNED(p->bullets); bulletIdx++, bullets++)
    {
        if (bullets->bulletState != BULLET_STATE_COLLIDED)
        {
            continue;
        }
        const f32 savedRotationZ = bullets->sprite.rotation.z;
        const f32 savedPrevRotationZ = bullets->sprite.prevRotation.z;
        if (bullets->sprite.autoRotate)
        {
            bullets->sprite.prevRotation.z =
                ZUN_PI / 2 - utils::AddNormalizeAngle(bullets->unk_134.z, ZUN_PI);
            bullets->sprite.rotation.z = ZUN_PI / 2 - utils::AddNormalizeAngle(bullets->unk_134.z, ZUN_PI);
        }
        const ZunVec3 savedPos = bullets->sprite.pos;
        bullets->sprite.pos = bullets->prevPosition.Lerp(bullets->position, g_RenderAlpha);
        bullets->sprite.pos.z = 0.4f;
        g_AnmManager->Draw2(&bullets->sprite);
        bullets->sprite.pos = savedPos;
        bullets->sprite.rotation.z = savedRotationZ;
        bullets->sprite.prevRotation.z = savedPrevRotationZ;
    }
}

void Player::StartFireBulletTimer(Player *p)
{
    if (p->fireBulletTimer.AsFrames() < 0)
    {
        p->fireBulletTimer.InitializeForPopup();
    }
}

ZunResult Player::UpdateFireBulletsTimer(Player *p)
{
    if (p->fireBulletTimer.AsFrames() < 0)
    {
        return ZUN_SUCCESS;
    }

    const bool marisaB =
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        PlayerCharacter(p) == CHARA_MARISA && PlayerShot(p) == SHOT_TYPE_B;
#else
        g_GameManager.character == CHARA_MARISA && g_GameManager.shotType == SHOT_TYPE_B;
#endif
    if (p->fireBulletTimer.HasTicked() && (!p->bombInfo.isInUse || !marisaB))
    {
        p->SpawnBullets(p, p->fireBulletTimer.AsFrames());
    }

    p->fireBulletTimer.Tick();

    if (p->fireBulletTimer.AsFrames() >= 30 || p->playerState == PLAYER_STATE_DEAD ||
        p->playerState == PLAYER_STATE_SPAWNING
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        || p->playerState == PLAYER_STATE_ELIMINATED ||
           p->playerState == PLAYER_STATE_REVIVABLE
#endif
    )
    {
        p->fireBulletTimer.SetCurrent(-1);
    }
    return ZUN_SUCCESS;
}

f32 Player::AngleFromPlayer(const ZunVec3 *pos) const
{
    f32 relX;
    f32 relY;

    relX = pos->x - this->positionCenter.x;
    relY = pos->y - this->positionCenter.y;
    if (relY == 0.0f && relX == 0.0f)
    {
        return ZUN_PI / 2;
    }

    return ZUN_ATAN2F(relY, relX);
}

f32 Player::AngleToPlayer(const ZunVec3 *pos) const
{
    f32 relX;
    f32 relY;

    relX = this->positionCenter.x - pos->x;
    relY = this->positionCenter.y - pos->y;
    if (relY == 0.0f && relX == 0.0f)
    {
        // Shoot down. An angle of 0 means to the right, and the angle goes
        // clockwise.
        return RADIANS(90.0f);
    }

    return ZUN_ATAN2F(relY, relX);
}

void Player::SpawnBullets(Player *p, u32 timer)
{
    FireBulletResult bulletResult;
    PlayerBullet *curBullet;
    i32 curBulletIdx;
    u32 idx;

    idx = 0;
    curBullet = p->bullets;

    for (curBulletIdx = 0; curBulletIdx < ARRAY_SIZE_SIGNED(p->bullets); curBulletIdx++, curBullet++)
    {
        if (curBullet->bulletState != BULLET_STATE_UNUSED)
        {
            continue;
        }
    WHILE_LOOP:
        if (!p->isFocus)
        {
            bulletResult = (*p->fireBulletCallback)(p, curBullet, idx, timer);
        }
        else
        {
            bulletResult = (*p->fireBulletFocusCallback)(p, curBullet, idx, timer);
        }
        if (bulletResult >= 0)
        {
            curBullet->sprite.pos.x = curBullet->position.x;
            curBullet->sprite.pos.y = curBullet->position.y;
            curBullet->sprite.pos.z = 0.495;
            curBullet->bulletState = BULLET_STATE_FIRED;
        }
        if (bulletResult == FBR_STOP_SPAWNING)
        {
            return;
        }
        if (bulletResult > 0)
        {
            return;
        }
        idx++;
        if (bulletResult == FBR_SPAWN_MORE)
        {
            goto WHILE_LOOP;
        }
    }
}

FireBulletResult Player::FireSingleBullet(Player *player, PlayerBullet *bullet, i32 bulletIdx,
                                          i32 framesSinceLastBullet, const CharacterPowerData *powerData)
{
    const CharacterPowerBulletData *bulletData;
    f32 *pfVar4;
    i32 bulletFrame;
    i32 unused;
    i32 unused2;

    const i32 currentPower =
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        MultiplayerGameplay::IsMultiplayer() ? GetPlayerPower(player->initParam) : g_GameManager.currentPower;
#else
        g_GameManager.currentPower;
#endif
    while (currentPower >= powerData->power)
    {
        powerData++;
    }

    bulletData = powerData->bullets + bulletIdx;

    if (bulletData->bulletType == BULLET_TYPE_LASER)
    {
        bulletFrame = bulletData->bulletFrame;
        if (!player->laserTimer[bulletFrame].AsFrames())
        {
            player->laserTimer[bulletFrame].SetCurrent(bulletData->waitBetweenBullets);

            bullet->unk_152 = bulletFrame;
            bullet->spawnPositionIdx = bulletData->spawnPositionIdx;
            bullet->sidewaysMotion = bulletData->motion.x;
            bullet->unk_134.x = bulletData->motion.y;
            goto SHOOT_BULLET;
        }
    }
    else if (framesSinceLastBullet % bulletData->waitBetweenBullets == bulletData->bulletFrame)
    {
    SHOOT_BULLET:

        g_AnmManager->SetAndExecuteScriptIdx(
            &bullet->sprite,
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
            GetPlayerAnmScript(player, bulletData->anmFileIdx)
#else
            bulletData->anmFileIdx
#endif
        );
        if (!bulletData->spawnPositionIdx)
        {
            bullet->position = player->positionCenter;
        }
        else
        {
            bullet->position = player->orbsPosition[bulletData->spawnPositionIdx - 1];
        }
        pfVar4 = &bullet->position.x;
        *pfVar4 = *pfVar4 + bulletData->motion.x;
        pfVar4 = &bullet->position.y;
        *pfVar4 = *pfVar4 + bulletData->motion.y;

        bullet->position.z = 0.495f;
        bullet->prevPosition = bullet->position;
        bullet->sprite.UpdatePrev();

        bullet->size.x = bulletData->size.x;
        bullet->size.y = bulletData->size.y;
        bullet->size.z = 1.0f;
        bullet->unk_134.z = bulletData->direction;
        bullet->unk_134.y = bulletData->velocity;

        bullet->velocity.x = ZUN_COSF(bulletData->direction) * bulletData->velocity;

        bullet->velocity.y = ZUN_SINF(bulletData->direction) * bulletData->velocity;

        bullet->unk_140.InitializeForPopup();

        bullet->bulletType = bulletData->bulletType;
        bullet->damage = bulletData->unk_1c;
        if (bulletData->bulletSoundIdx >= 0)
        {
            g_SoundPlayer.PlaySoundByIdx((SoundIdx)bulletData->bulletSoundIdx);
        }

        return bulletIdx >= powerData->numBullets - 1;
    }

    if (bulletIdx >= powerData->numBullets - 1)
    {
        return FBR_STOP_SPAWNING;
    }
    else
    {
        return FBR_SPAWN_MORE;
    }
}

FireBulletResult Player::FireBulletReimuA(Player *player, PlayerBullet *bullet, u32 bulletIdx,
                                          u32 framesSinceLastBullet)
{
    return player->FireSingleBullet(player, bullet, bulletIdx, framesSinceLastBullet, g_CharacterPowerDataReimuA);
}

FireBulletResult Player::FireBulletReimuB(Player *player, PlayerBullet *bullet, u32 bulletIdx,
                                          u32 framesSinceLastBullet)
{
    return player->FireSingleBullet(player, bullet, bulletIdx, framesSinceLastBullet, g_CharacterPowerDataReimuB);
}

FireBulletResult Player::FireBulletMarisaA(Player *player, PlayerBullet *bullet, u32 bulletIdx,
                                           u32 framesSinceLastBullet)
{
    return player->FireSingleBullet(player, bullet, bulletIdx, framesSinceLastBullet, g_CharacterPowerDataMarisaA);
}

FireBulletResult Player::FireBulletMarisaB(Player *player, PlayerBullet *bullet, u32 bulletIdx,
                                           u32 framesSinceLastBullet)
{
    return player->FireSingleBullet(player, bullet, bulletIdx, framesSinceLastBullet, g_CharacterPowerDataMarisaB);
}

i32 Player::CheckGraze(const ZunVec3 *center, const ZunVec3 *size) const
{
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (this->playerState == PLAYER_STATE_ELIMINATED ||
        this->playerState == PLAYER_STATE_REVIVABLE)
        return 0;
#endif
    ZunVec3 bombBottomRight;
    const PlayerRect *bombProjectile;
    ZunVec3 bombTopLeft;
    ZunVec3 bulletBottomRight;
    ZunVec3 bulletTopLeft;
    i32 i;

    bulletTopLeft.x = center->x - size->x / 2.0f - 20.0f;
    bulletTopLeft.y = center->y - size->y / 2.0f - 20.0f;
    bulletBottomRight.x = center->x + size->x / 2.0f + 20.0f;
    bulletBottomRight.y = center->y + size->y / 2.0f + 20.0f;
    bombProjectile = this->bombProjectiles;

    for (i = 0; i < ARRAY_SIZE_SIGNED(this->bombProjectiles); i++, bombProjectile++)
    {
        if (bombProjectile->sizeX == 0.0f)
        {
            continue;
        }

        bombTopLeft.x = bombProjectile->posX - bombProjectile->sizeX / 2.0f;
        bombTopLeft.y = bombProjectile->posY - bombProjectile->sizeY / 2.0f;
        bombBottomRight.x = bombProjectile->sizeX / 2.0f + bombProjectile->posX;
        bombBottomRight.y = bombProjectile->sizeY / 2.0f + bombProjectile->posY;

        // Bomb clips bullet's hitbox, destroys bullet upon return
        if (!(bombTopLeft.x > bulletBottomRight.x || bombBottomRight.x < bulletTopLeft.x ||
              bombTopLeft.y > bulletBottomRight.y || bombBottomRight.y < bulletTopLeft.y))
        {
            return 2;
        }
    }

    if (this->playerState == PLAYER_STATE_DEAD || this->playerState == PLAYER_STATE_SPAWNING)
    {
        return 0;
    }
    if (this->hitboxTopLeft.x > bulletBottomRight.x || this->hitboxBottomRight.x < bulletTopLeft.x ||
        this->hitboxTopLeft.y > bulletBottomRight.y || this->hitboxBottomRight.y < bulletTopLeft.y)
    {
        return 0;
    }

    // Bullet clips player's graze hitbox, add score and check for death upon return
    this->ScoreGraze(center);
    return 1;
}

i32 Player::CalcKillBoxCollision(const ZunVec3 *bulletCenter, const ZunVec3 *bulletSize)
{
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (this->playerState == PLAYER_STATE_ELIMINATED ||
        this->playerState == PLAYER_STATE_REVIVABLE)
        return 0;
#endif
    PlayerRect *curBombProjectile;
    f32 bulletLeft, bulletTop, bulletRight, bulletBottom;
    f32 bombProjectileLeft, bombProjectileTop, bombProjectileRight, bombProjectileBottom;
    i32 curBombIdx;
    i32 padding1, padding2, padding3, padding4;

    curBombProjectile = this->bombProjectiles;
    bulletLeft = bulletCenter->x - bulletSize->x / 2.0f;
    bulletTop = bulletCenter->y - bulletSize->y / 2.0f;
    bulletRight = bulletCenter->x + bulletSize->x / 2.0f;
    bulletBottom = bulletCenter->y + bulletSize->y / 2.0f;
    for (curBombIdx = 0; curBombIdx < ARRAY_SIZE_SIGNED(this->bombProjectiles); curBombIdx++, curBombProjectile++)
    {
        if (curBombProjectile->sizeX == 0.0f)
        {
            continue;
        }
        bombProjectileLeft = curBombProjectile->posX - curBombProjectile->sizeX / 2.0f;
        bombProjectileTop = curBombProjectile->posY - curBombProjectile->sizeY / 2.0f;
        bombProjectileRight = curBombProjectile->posX + curBombProjectile->sizeX / 2.0f;
        bombProjectileBottom = curBombProjectile->posY + curBombProjectile->sizeY / 2.0f;
        if (!(bombProjectileLeft > bulletRight || bombProjectileRight < bulletLeft ||
              bombProjectileTop > bulletBottom || bombProjectileBottom < bulletTop))
        {
            return 2;
        }
    }
    if (this->hitboxTopLeft.x > bulletRight || this->hitboxTopLeft.y > bulletBottom ||
        this->hitboxBottomRight.x < bulletLeft || this->hitboxBottomRight.y < bulletTop)
    {
        return 0;
    }
    else if (this->playerState != PLAYER_STATE_ALIVE)
    {
        return 1;
    }
    else
    {
        this->Die();
        return 1;
    }
}

i32 Player::CalcLaserHitbox(const ZunVec3 *laserCenter, const ZunVec3 *laserSize, const ZunVec3 *rotation, f32 angle,
                            i32 canGraze)
{
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (this->playerState == PLAYER_STATE_ELIMINATED ||
        this->playerState == PLAYER_STATE_REVIVABLE)
        return 0;
#endif
    ZunVec3 laserTopLeft;
    ZunVec3 laserBottomRight;
    ZunVec3 playerRelativeTopLeft;
    ZunVec3 playerRelativeBottomRight;

    laserTopLeft = this->positionCenter - *rotation;
    utils::Rotate(&laserBottomRight, &laserTopLeft, angle);
    laserBottomRight.z = 0;
    laserTopLeft = laserBottomRight + *rotation;
    playerRelativeTopLeft = laserTopLeft - this->hitboxSize;
    playerRelativeBottomRight = laserTopLeft + this->hitboxSize;

    laserTopLeft = *laserCenter - *laserSize / 2.0f;
    laserBottomRight = *laserCenter + *laserSize / 2.0f;

    if (!(playerRelativeTopLeft.x > laserBottomRight.x || playerRelativeBottomRight.x < laserTopLeft.x ||
          playerRelativeTopLeft.y > laserBottomRight.y || playerRelativeBottomRight.y < laserTopLeft.y))
    {
        goto LASER_COLLISION;
    }
    if (canGraze == 0)
    {
        return 0;
    }

    laserTopLeft.x -= 48.0f;
    laserTopLeft.y -= 48.0f;
    laserBottomRight.x += 48.0f;
    laserBottomRight.y += 48.0f;

    if (playerRelativeTopLeft.x > laserBottomRight.x || playerRelativeBottomRight.x < laserTopLeft.x ||
        playerRelativeTopLeft.y > laserBottomRight.y || playerRelativeBottomRight.y < laserTopLeft.y)
    {
        return 0;
    }
    if (this->playerState == PLAYER_STATE_DEAD || this->playerState == PLAYER_STATE_SPAWNING)
    {
        return 0;
    }

    this->ScoreGraze(&this->positionCenter);
    return 2;

LASER_COLLISION:
    if (this->playerState != PLAYER_STATE_ALIVE)
    {
        return 0;
    }

    this->Die();
    return 1;
}

i32 Player::CalcItemBoxCollision(const ZunVec3 *itemCenter, const ZunVec3 *itemSize) const
{
    if (this->playerState != PLAYER_STATE_ALIVE && this->playerState != PLAYER_STATE_INVULNERABLE)
    {
        return 0;
    }
    ZunVec3 itemTopLeft = *itemCenter - *itemSize / 2.0f;
    //    std::memcpy(&itemTopLeft, &(*itemCenter - *itemSize / 2.0f), sizeof(ZunVec3));
    ZunVec3 itemBottomRight = *itemCenter + *itemSize / 2.0f;
    //    std::memcpy(&itemBottomRight, &(*itemCenter + *itemSize / 2.0f), sizeof(ZunVec3));

    if (this->grabItemTopLeft.x > itemBottomRight.x || this->grabItemBottomRight.x < itemTopLeft.x ||
        this->grabItemTopLeft.y > itemBottomRight.y || this->grabItemBottomRight.y < itemTopLeft.y)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

void Player::ScoreGraze(const ZunVec3 *center) const
{
    ZunVec3 particlePosition;

    if (this->bombInfo.isInUse == 0)
    {
        if (g_GameManager.grazeInStage < 9999)
        {
            g_GameManager.grazeInStage++;
        }
        if (g_GameManager.grazeInTotal < 999999)
        {
            g_GameManager.grazeInTotal++;
        }
    }

    particlePosition = (this->positionCenter + *center) / 2.0f;
    g_EffectManager.SpawnParticles(PARTICLE_EFFECT_UNK_8, &particlePosition, 1, COLOR_WHITE);
    g_GameManager.AddScore(500);
    g_GameManager.IncreaseSubrank(6);
    g_Gui.flags.flag3 = 2;
    g_SoundPlayer.PlaySoundByIdx(SOUND_GRAZE);
}

void Player::Die()
{
    int curLaserTimerIdx;

    g_EnemyManager.spellcardInfo.isCapturing = 0;
    // Upstream Muteki replaces the first death-particle call with stack
    // cleanup and changes the state immediate from DEAD(2) to
    // INVULNERABLE(3); the remaining hit feedback/sound/death counter stays.
    if (!PracticeRuntime::OverlayInvincible())
        g_EffectManager.SpawnParticles(PARTICLE_EFFECT_UNK_12, &this->positionCenter, 1, COLOR_NEONBLUE);
    g_EffectManager.SpawnParticles(PARTICLE_EFFECT_UNK_6, &this->positionCenter, 16, COLOR_WHITE);
    this->playerState = PracticeRuntime::OverlayInvincible() ? PLAYER_STATE_INVULNERABLE : PLAYER_STATE_DEAD;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    this->respawnTimer = 6 + (PlayerUsedTouch(this) ? Touch::DEATHBOMB_TOLERANCE : 0);
#else
    this->respawnTimer = 6 + (Touch::WasUsedThisRun() ? Touch::DEATHBOMB_TOLERANCE : 0);
#endif
    this->invulnerabilityTimer.InitializeForPopup();
    g_SoundPlayer.PlaySoundByIdx(SOUND_PICHUN);
    g_GameManager.deaths++;
    for (curLaserTimerIdx = 0; curLaserTimerIdx < ARRAY_SIZE_SIGNED(this->laserTimer); curLaserTimerIdx++)
    {
        this->laserTimer[curLaserTimerIdx].SetCurrent(2);
    }
    return;
}
