#include "Th07RollbackState.hpp"

#include <eagler/netplay/RollbackJournal.hpp>

#include "AnmManager.hpp"
#include "AsciiManager.hpp"
#include "BulletManager.hpp"
#include "Chain.hpp"
#include "EclManager.hpp"
#include "EffectManager.hpp"
#include "EnemyManager.hpp"
#include "GameManager.hpp"
#include "Gui.hpp"
#include "ItemManager.hpp"
#include "Player.hpp"
#include "Rng.hpp"
#include "ReplayManager.hpp"
#include "ScreenEffect.hpp"
#include "Stage.hpp"
#include "Supervisor.hpp"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <deque>
#include <vector>

namespace Netplay::Th07Rollback
{
namespace
{
struct BombFrame
{
    std::uint32_t startFrame = 0;
    std::uint32_t endFrame = 0;
    std::vector<ScreenEffect> effects;
};

constexpr std::size_t CHECKPOINT_LOGICAL_FRAMES = 2;

RollbackJournal g_Journal;
Config g_Config{};
std::deque<BombFrame> g_BombFrames;
bool g_Configured = false;
bool g_Failed = false;
int g_HistoryStage = -1;
std::size_t g_FramesInCheckpoint = 0;

std::size_t CheckpointCapacity()
{
    return (g_Config.maxFrames + CHECKPOINT_LOGICAL_FRAMES - 1) /
           CHECKPOINT_LOGICAL_FRAMES;
}

bool TouchMemory(void *address, std::size_t size)
{
    if (!g_Journal.IsFrameOpen())
    {
        return true;
    }
    if (!g_Journal.Touch(address, size))
    {
        g_Failed = true;
        return false;
    }
    return true;
}

template <typename T>
bool TouchObject(T *object)
{
    return object ? TouchMemory(object, sizeof(*object)) : true;
}

bool TouchRange(void *begin, void *end)
{
    const auto first = reinterpret_cast<std::uintptr_t>(begin);
    const auto last = reinterpret_cast<std::uintptr_t>(end);
    if (last < first)
    {
        g_Failed = true;
        return false;
    }
    return last == first || TouchMemory(begin, last - first);
}

bool IsBombEffectCallback(ChainCallback callback)
{
    return callback == reinterpret_cast<ChainCallback>(ScreenEffect::OnUpdateFadeIn) ||
           callback == reinterpret_cast<ChainCallback>(ScreenEffect::OnUpdateFadeOut) ||
           callback == reinterpret_cast<ChainCallback>(ScreenEffect::OnUpdatePulse) ||
           callback == reinterpret_cast<ChainCallback>(ScreenEffect::OnUpdateScreenShake);
}

bool CaptureBombEffects(std::uint32_t frame)
{
    if (g_BombFrames.size() == CheckpointCapacity())
    {
        g_BombFrames.pop_front();
    }
    g_BombFrames.emplace_back();
    BombFrame &snapshot = g_BombFrames.back();
    snapshot.startFrame = frame;
    snapshot.endFrame = frame;

    for (ChainElem *element = g_Chain.calcChain.next; element; element = element->next)
    {
        if (!IsBombEffectCallback(element->callback))
        {
            continue;
        }
        if (!element->arg || snapshot.effects.size() >= g_Config.maxBombEffectsPerFrame)
        {
            g_Failed = true;
            return false;
        }
        snapshot.effects.push_back(*static_cast<ScreenEffect *>(element->arg));
    }
    return true;
}

BombFrame *FindBombFrame(std::uint32_t frame)
{
    const auto it = std::find_if(g_BombFrames.begin(), g_BombFrames.end(),
                                 [frame](const BombFrame &entry) {
                                     return entry.startFrame <= frame && frame <= entry.endFrame;
                                 });
    return it == g_BombFrames.end() ? nullptr : &*it;
}

void RemoveCurrentBombEffects()
{
    ChainElem *element = g_Chain.calcChain.next;
    while (element)
    {
        ChainElem *next = element->next;
        if (IsBombEffectCallback(element->callback))
        {
            g_Chain.Cut(element);
        }
        element = next;
    }
}

bool RestoreBombEffects(const BombFrame &snapshot)
{
    for (const ScreenEffect &saved : snapshot.effects)
    {
        ScreenEffect *restored = ScreenEffect::RegisterChain(
            saved.type, saved.duration, saved.args[0], saved.args[1], saved.args[2]);
        if (!restored)
        {
            return false;
        }
        ChainElem *calcChain = restored->calcChain;
        ChainElem *drawChain = restored->drawChain;
        *restored = saved;
        restored->calcChain = calcChain;
        restored->drawChain = drawChain;
    }
    return true;
}

void ResetHistoryForStage(int stage)
{
    g_Journal.Reset(RollbackJournalConfig{CheckpointCapacity(),
                                           g_Config.maxBytesPerFrame,
                                           g_Config.maxBlocksPerFrame});
    g_BombFrames.clear();
    g_HistoryStage = stage;
    g_FramesInCheckpoint = 0;
}

bool CapturePlayer(Player *player)
{
    if (!player)
    {
        return true;
    }

    auto *playerBegin = reinterpret_cast<std::uint8_t *>(player);
    auto *bulletsBegin = reinterpret_cast<std::uint8_t *>(&player->bullets[0]);
    auto *bulletsEnd = bulletsBegin + sizeof(player->bullets);
    auto *bombBegin = reinterpret_cast<std::uint8_t *>(&player->bombInfo);
    auto *bombEnd = bombBegin + sizeof(player->bombInfo);
    auto *playerEnd = playerBegin + sizeof(*player);

    if (!TouchRange(playerBegin, bulletsBegin))
        return false;
    for (int i = 0; i < 96; ++i)
    {
        if (player->bullets[i].bulletState != 0 && !TouchPlayerBullet(&player->bullets[i]))
            return false;
    }
    if (!TouchRange(bulletsEnd, bombBegin))
        return false;
    if (player->bombInfo.isInUse && !TouchPlayerBombInfo(&player->bombInfo))
        return false;
    return TouchRange(bombEnd, playerEnd);
}

bool CaptureFixedAndSparseState()
{
    if (!TouchObject(&g_GameManager))
        return false;
    if (g_GameManager.globals && !TouchObject(g_GameManager.globals))
        return false;
    if (g_GameManager.defaultCfg && !TouchObject(g_GameManager.defaultCfg))
        return false;
    if (!TouchObject(&g_Rng) || !TouchObject(&g_GlobalEclVars) ||
        !TouchObject(&g_Stage) || !TouchObject(&g_Gui) || !TouchObject(&g_AsciiManager))
        return false;
    if (g_ReplayManager)
    {
        // Replay output is a committed side effect.  In particular frameId,
        // replayInputs/fpsCursor and the destination pointers must keep the
        // position reached by the original forward pass while rollback
        // re-simulates gameplay with side effects suppressed.  Rewinding the
        // whole ReplayManager here makes those cursors jump backwards even
        // though the pointed-to replay bytes are intentionally not journaled,
        // corrupting the next forward write after every real rollback.
        //
        // These two small fields are frame-local deterministic metadata that
        // gameplay callbacks OR into before the recording callback consumes
        // them, so keep only them rewindable.
        if (!TouchObject(&g_ReplayManager->rngSeed) ||
            !TouchObject(&g_ReplayManager->replayEventFlags))
            return false;
    }
    if (g_Gui.impl && !TouchObject(g_Gui.impl))
        return false;

    // Only Supervisor fields that can alter 60 Hz simulation semantics are
    // rewindable. Platform/render timing and device pointers stay local.
    if (!TouchObject(&g_Supervisor.wantedState) ||
        !TouchObject(&g_Supervisor.curState) || !TouchObject(&g_Supervisor.prevState) ||
        !TouchObject(&g_Supervisor.isInEnding) ||
        !TouchObject(&g_Supervisor.effectiveFramerateMultiplier) ||
        !TouchObject(&g_Supervisor.flags))
        return false;

    if (!TouchObject(&g_CurFrameRawInput) ||
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        !TouchObject(&g_CurFrameGameInputs) ||
#else
        !TouchObject(&g_CurFrameGameInput) ||
#endif
        !TouchObject(&g_LastFrameRawInput) ||
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        !TouchObject(&g_LastFrameGameInputs) ||
#else
        !TouchObject(&g_LastFrameGameInput) ||
#endif
        !TouchObject(&g_IsEighthFrameOfHeldInput) ||
        !TouchObject(&g_NumOfFramesInputsWereHeld))
        return false;

    if (g_AnmManager)
    {
        if (!TouchObject(&g_AnmManager->offset) || !TouchObject(&g_AnmManager->shakeOffset) ||
            !TouchObject(&g_AnmManager->prevShakeOffset))
            return false;
    }

#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    // Multiplayer keeps P1 in the original layout and adds P2/P3 plus small
    // deterministic sidecars.  Every active slot must rewind together; if a
    // predicted guest is left outside the journal, resimulation advances that
    // ship twice even though the shared world was restored correctly.
    if (!TouchObject(&g_PlayerActive) ||
        !TouchObject(&g_MultiplayerPlayerResources) ||
        !TouchObject(&g_MultiplayerContributionStats) ||
        !TouchObject(&g_cherryMaxGrazeGrowth) ||
        !TouchObject(&g_cherryMaxBreakGrowth) ||
        !TouchObject(&g_powerGiveTaps) || !TouchObject(&g_powerGiveWindow) ||
        !TouchObject(&g_teamWipeRetryFrames) ||
        !TouchObject(&g_stage4ChainQueue) || !TouchObject(&g_stage4ChainCount) ||
        !TouchObject(&g_stage4ChainPos) || !TouchObject(&g_stage4ChainBossId) ||
        !TouchObject(&g_stage4ChainCardActive) ||
        !TouchObject(&g_stage4ChainPhaseLife) || !TouchObject(&g_stage4ChainSpellIdx))
        return false;
    for (u8 playerId = 0; playerId < TH07_MULTI_MAX_PLAYERS; ++playerId)
    {
        if (g_PlayerActive[playerId] && !CapturePlayer(&g_Players[playerId]))
            return false;
    }
#else
    if (!CapturePlayer(&g_Player))
        return false;
#endif

    auto *enemyBegin = reinterpret_cast<std::uint8_t *>(&g_EnemyManager);
    auto *enemiesBegin = reinterpret_cast<std::uint8_t *>(&g_EnemyManager.enemies[0]);
    auto *enemiesEnd = enemiesBegin + sizeof(g_EnemyManager.enemies);
    auto *enemyManagerEnd = enemyBegin + sizeof(g_EnemyManager);
    if (!TouchRange(enemyBegin, enemiesBegin))
        return false;
    for (int i = 0; i < 480; ++i)
        if (g_EnemyManager.enemies[i].active && !TouchEnemy(&g_EnemyManager.enemies[i]))
            return false;
    if (!TouchRange(enemiesEnd, enemyManagerEnd))
        return false;

    auto *bulletManagerBegin = reinterpret_cast<std::uint8_t *>(&g_BulletManager);
    auto *bulletsBegin = reinterpret_cast<std::uint8_t *>(&g_BulletManager.bullets[0]);
    auto *bulletsEnd = bulletsBegin + sizeof(g_BulletManager.bullets);
    auto *lasersBegin = reinterpret_cast<std::uint8_t *>(&g_BulletManager.lasers[0]);
    auto *lasersEnd = lasersBegin + sizeof(g_BulletManager.lasers);
    auto *bulletManagerEnd = bulletManagerBegin + sizeof(g_BulletManager);
    if (!TouchRange(bulletManagerBegin, bulletsBegin))
        return false;
    for (int i = 0; i < 1024; ++i)
        if (g_BulletManager.bullets[i].state != BULLET_INACTIVE &&
            !TouchBullet(&g_BulletManager.bullets[i]))
            return false;
    if (!TouchRange(bulletsEnd, lasersBegin))
        return false;
    for (int i = 0; i < 64; ++i)
        if (g_BulletManager.lasers[i].isInUse && !TouchLaser(&g_BulletManager.lasers[i]))
            return false;
    if (!TouchRange(lasersEnd, bulletManagerEnd))
        return false;

    auto *itemsBegin = reinterpret_cast<std::uint8_t *>(&g_ItemManager.items[0]);
    auto *itemsEnd = itemsBegin + sizeof(g_ItemManager.items);
    auto *itemManagerEnd = reinterpret_cast<std::uint8_t *>(&g_ItemManager) + sizeof(g_ItemManager);
    for (int i = 0; i < 1100; ++i)
        if (g_ItemManager.items[i].isInUse && !TouchItem(&g_ItemManager.items[i]))
            return false;
    if (!TouchRange(itemsEnd, itemManagerEnd))
        return false;

    auto *effectManagerBegin = reinterpret_cast<std::uint8_t *>(&g_EffectManager);
    auto *effectsBegin = reinterpret_cast<std::uint8_t *>(&g_EffectManager.effects[0]);
    auto *effectsEnd = effectsBegin + sizeof(g_EffectManager.effects);
    auto *effectManagerEnd = effectManagerBegin + sizeof(g_EffectManager);
    if (!TouchRange(effectManagerBegin, effectsBegin))
        return false;
    for (int i = 0; i < 413; ++i)
        if (g_EffectManager.effects[i].inUseFlag && !TouchEffect(&g_EffectManager.effects[i]))
            return false;
    return TouchRange(effectsEnd, effectManagerEnd);
}
} // namespace

bool Reset(const Config &config)
{
    if (config.maxFrames == 0 || config.maxBytesPerFrame == 0 ||
        config.maxBlocksPerFrame == 0 || config.maxBombEffectsPerFrame == 0)
        return false;
    g_Config = config;
    g_BombFrames.clear();
    g_Failed = false;
    g_HistoryStage = -1;
    g_FramesInCheckpoint = 0;
    g_Configured = g_Journal.Reset(RollbackJournalConfig{
        CheckpointCapacity(), config.maxBytesPerFrame, config.maxBlocksPerFrame});
    return g_Configured;
}

void Clear()
{
    g_Journal.Clear();
    g_BombFrames.clear();
    g_Configured = false;
    g_Failed = false;
    g_HistoryStage = -1;
    g_FramesInCheckpoint = 0;
}

bool BeginFrame(std::uint32_t frame)
{
    if (!g_Configured || g_Failed || g_Journal.IsFrameOpen())
        return false;

    const int stage = static_cast<int>(g_GameManager.currentStage);
    if (g_HistoryStage != stage)
        ResetHistoryForStage(stage);

    const bool extendPrevious = g_FramesInCheckpoint != 0;
    if (!extendPrevious)
    {
        if (!CaptureBombEffects(frame) || !g_Journal.BeginFrame(frame) ||
            !CaptureFixedAndSparseState())
        {
            g_Failed = true;
            return false;
        }
    }
    else if (!g_Journal.BeginFrame(frame, true))
    {
        g_Failed = true;
        return false;
    }
    if (extendPrevious && !g_BombFrames.empty())
        g_BombFrames.back().endFrame = frame;
    return true;
}

bool EndFrame()
{
    if (!g_Journal.EndFrame())
    {
        g_Failed = true;
        return false;
    }
    g_FramesInCheckpoint = (g_FramesInCheckpoint + 1) % CHECKPOINT_LOGICAL_FRAMES;
    return true;
}

bool RestoreTo(std::uint32_t frame, std::uint32_t *replayFrom)
{
    if (!g_Configured || g_Failed || g_Journal.IsFrameOpen())
        return false;
    BombFrame *snapshot = FindBombFrame(frame);
    if (!snapshot)
        return false;
    const BombFrame saved = *snapshot;
    std::uint32_t restoredFrame = frame;

    RemoveCurrentBombEffects();
    if (!g_Journal.UndoTo(frame, &restoredFrame) ||
        restoredFrame != saved.startFrame || !RestoreBombEffects(saved))
    {
        g_Failed = true;
        return false;
    }
    while (!g_BombFrames.empty() && g_BombFrames.back().endFrame >= restoredFrame)
        g_BombFrames.pop_back();
    g_FramesInCheckpoint = 0;
    if (replayFrom)
        *replayFrom = restoredFrame;
    return true;
}

void DiscardBefore(std::uint32_t frame)
{
    g_Journal.DiscardBefore(frame);
    while (!g_BombFrames.empty() && g_BombFrames.front().endFrame < frame)
        g_BombFrames.pop_front();
}

bool IsCapturing()
{
    return g_Journal.IsFrameOpen();
}

bool Failed()
{
    return g_Failed || g_Journal.Failed();
}

std::size_t CapturedBytes(std::uint32_t frame)
{
    return g_Journal.BytesForFrame(frame);
}

std::size_t CapturedBlocks(std::uint32_t frame)
{
    return g_Journal.BlocksForFrame(frame);
}

std::size_t CapturedBombEffects(std::uint32_t frame)
{
    BombFrame *snapshot = FindBombFrame(frame);
    return snapshot ? snapshot->effects.size() : 0;
}

bool TouchEnemy(Enemy *enemy) { return TouchObject(enemy); }
bool TouchBullet(Bullet *bullet) { return TouchObject(bullet); }
bool TouchLaser(Laser *laser) { return TouchObject(laser); }
bool TouchItem(Item *item) { return TouchObject(item); }
bool TouchEffect(Effect *effect) { return TouchObject(effect); }
bool TouchPlayerBullet(PlayerBullet *bullet) { return TouchObject(bullet); }
bool TouchPlayerBombInfo(PlayerBombInfo *bombInfo) { return TouchObject(bombInfo); }

namespace
{
void HashBytes(std::uint64_t &hash, const void *data, std::size_t size)
{
    const auto *bytes = static_cast<const std::uint8_t *>(data);
    for (std::size_t i = 0; i < size; ++i)
    {
        hash ^= bytes[i];
        hash *= 1099511628211ull;
    }
}

template <typename T>
void HashObject(std::uint64_t &hash, const T &object)
{
    HashBytes(hash, &object, sizeof(object));
}

void HashBombEffects(std::uint64_t &hash)
{
    std::uint32_t count = 0;
    for (ChainElem *element = g_Chain.calcChain.next; element; element = element->next)
    {
        if (!IsBombEffectCallback(element->callback) || !element->arg)
            continue;
        const ScreenEffect &effect = *static_cast<ScreenEffect *>(element->arg);
        HashObject(hash, effect.type);
        HashObject(hash, effect.field3_0xc);
        HashObject(hash, effect.alpha);
        HashObject(hash, effect.prevAlpha);
        HashObject(hash, effect.duration);
        HashBytes(hash, effect.args, sizeof(effect.args));
        HashObject(hash, effect.timer);
        ++count;
    }
    HashObject(hash, count);
}
} // namespace

std::uint64_t DebugStateHash()
{
    // This first hash is intentionally same-runtime and pointer-tolerant. It
    // proves restore/replay locally. A later dual-Runtime probe will use a
    // pointer-neutral canonical hash for cross-instance determinism.
    std::uint64_t hash = 1469598103934665603ull;
    HashObject(hash, g_GameManager);
    if (g_GameManager.globals)
        HashObject(hash, *g_GameManager.globals);
    if (g_GameManager.defaultCfg)
        HashObject(hash, *g_GameManager.defaultCfg);
    HashObject(hash, g_Rng);
    HashObject(hash, g_GlobalEclVars);
    HashObject(hash, g_Stage);
    HashObject(hash, g_Gui);
    if (g_Gui.impl)
        HashObject(hash, *g_Gui.impl);
    HashObject(hash, g_AsciiManager);
    if (g_ReplayManager)
    {
        // Replay stream cursors and destination pointers are committed-output
        // bookkeeping, not simulation state. The speculative pass deliberately
        // does not advance them, while the committed replay pass does. Hash only
        // the small replay fields that can feed gameplay semantics this tick.
        HashObject(hash, g_ReplayManager->isDemo);
        HashObject(hash, g_ReplayManager->rngSeed);
        HashObject(hash, g_ReplayManager->replayEventFlags);
    }

    HashObject(hash, g_Supervisor.wantedState);
    HashObject(hash, g_Supervisor.curState);
    HashObject(hash, g_Supervisor.prevState);
    HashObject(hash, g_Supervisor.isInEnding);
    HashObject(hash, g_Supervisor.effectiveFramerateMultiplier);
    HashObject(hash, g_Supervisor.flags);
    HashObject(hash, g_CurFrameRawInput);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    HashObject(hash, g_CurFrameGameInputs);
    HashObject(hash, g_teamWipeRetryFrames);
#else
    HashObject(hash, g_CurFrameGameInput);
#endif
    HashObject(hash, g_LastFrameRawInput);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    HashObject(hash, g_LastFrameGameInputs);
#else
    HashObject(hash, g_LastFrameGameInput);
#endif
    HashObject(hash, g_IsEighthFrameOfHeldInput);
    HashObject(hash, g_NumOfFramesInputsWereHeld);
    if (g_AnmManager)
    {
        HashObject(hash, g_AnmManager->offset);
        HashObject(hash, g_AnmManager->shakeOffset);
        HashObject(hash, g_AnmManager->prevShakeOffset);
    }

    const auto *playerBegin = reinterpret_cast<const std::uint8_t *>(&g_Player);
    const auto *playerBullets = reinterpret_cast<const std::uint8_t *>(&g_Player.bullets[0]);
    const auto *playerBulletsEnd = playerBullets + sizeof(g_Player.bullets);
    const auto *playerBomb = reinterpret_cast<const std::uint8_t *>(&g_Player.bombInfo);
    const auto *playerBombEnd = playerBomb + sizeof(g_Player.bombInfo);
    const auto *playerEnd = playerBegin + sizeof(g_Player);
    HashBytes(hash, playerBegin, playerBullets - playerBegin);
    for (int i = 0; i < 96; ++i)
        if (g_Player.bullets[i].bulletState != 0)
            HashObject(hash, g_Player.bullets[i]);
    HashBytes(hash, playerBulletsEnd, playerBomb - playerBulletsEnd);
    if (g_Player.bombInfo.isInUse)
        HashObject(hash, g_Player.bombInfo);
    HashBytes(hash, playerBombEnd, playerEnd - playerBombEnd);

    HashBytes(hash, &g_EnemyManager,
              reinterpret_cast<const std::uint8_t *>(&g_EnemyManager.enemies[0]) -
                  reinterpret_cast<const std::uint8_t *>(&g_EnemyManager));
    for (int i = 0; i < 480; ++i)
        if (g_EnemyManager.enemies[i].active)
            HashObject(hash, g_EnemyManager.enemies[i]);
    HashBytes(hash,
              reinterpret_cast<const std::uint8_t *>(&g_EnemyManager.enemies[0]) +
                  sizeof(g_EnemyManager.enemies),
              reinterpret_cast<const std::uint8_t *>(&g_EnemyManager) + sizeof(g_EnemyManager) -
                  (reinterpret_cast<const std::uint8_t *>(&g_EnemyManager.enemies[0]) +
                   sizeof(g_EnemyManager.enemies)));

    HashBytes(hash, &g_BulletManager,
              reinterpret_cast<const std::uint8_t *>(&g_BulletManager.bullets[0]) -
                  reinterpret_cast<const std::uint8_t *>(&g_BulletManager));
    for (int i = 0; i < 1024; ++i)
        if (g_BulletManager.bullets[i].state != BULLET_INACTIVE)
            HashObject(hash, g_BulletManager.bullets[i]);
    HashBytes(hash,
              reinterpret_cast<const std::uint8_t *>(&g_BulletManager.bullets[0]) +
                  sizeof(g_BulletManager.bullets),
              reinterpret_cast<const std::uint8_t *>(&g_BulletManager.lasers[0]) -
                  (reinterpret_cast<const std::uint8_t *>(&g_BulletManager.bullets[0]) +
                   sizeof(g_BulletManager.bullets)));
    for (int i = 0; i < 64; ++i)
        if (g_BulletManager.lasers[i].isInUse)
            HashObject(hash, g_BulletManager.lasers[i]);
    HashBytes(hash,
              reinterpret_cast<const std::uint8_t *>(&g_BulletManager.lasers[0]) +
                  sizeof(g_BulletManager.lasers),
              reinterpret_cast<const std::uint8_t *>(&g_BulletManager) + sizeof(g_BulletManager) -
                  (reinterpret_cast<const std::uint8_t *>(&g_BulletManager.lasers[0]) +
                   sizeof(g_BulletManager.lasers)));

    for (int i = 0; i < 1100; ++i)
        if (g_ItemManager.items[i].isInUse)
            HashObject(hash, g_ItemManager.items[i]);
    HashBytes(hash,
              reinterpret_cast<const std::uint8_t *>(&g_ItemManager.items[0]) +
                  sizeof(g_ItemManager.items),
              reinterpret_cast<const std::uint8_t *>(&g_ItemManager) + sizeof(g_ItemManager) -
                  (reinterpret_cast<const std::uint8_t *>(&g_ItemManager.items[0]) +
                   sizeof(g_ItemManager.items)));

    HashBytes(hash, &g_EffectManager,
              reinterpret_cast<const std::uint8_t *>(&g_EffectManager.effects[0]) -
                  reinterpret_cast<const std::uint8_t *>(&g_EffectManager));
    for (int i = 0; i < 413; ++i)
        if (g_EffectManager.effects[i].inUseFlag)
            HashObject(hash, g_EffectManager.effects[i]);
    HashBytes(hash,
              reinterpret_cast<const std::uint8_t *>(&g_EffectManager.effects[0]) +
                  sizeof(g_EffectManager.effects),
              reinterpret_cast<const std::uint8_t *>(&g_EffectManager) + sizeof(g_EffectManager) -
                  (reinterpret_cast<const std::uint8_t *>(&g_EffectManager.effects[0]) +
                   sizeof(g_EffectManager.effects)));
    HashBombEffects(hash);
    return hash;
}
} // namespace Netplay::Th07Rollback
