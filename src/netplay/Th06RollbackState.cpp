#include "Th06RollbackState.hpp"

#include "RollbackJournal.hpp"

#include "AsciiManager.hpp"
#include "BulletManager.hpp"
#include "Chain.hpp"
#include "EffectManager.hpp"
#include "EnemyEclInstr.hpp"
#include "EnemyManager.hpp"
#include "GameManager.hpp"
#include "Gui.hpp"
#include "ItemManager.hpp"
#include "Player.hpp"
#include "ReplayManager.hpp"
#include "Rng.hpp"
#include "ScreenEffect.hpp"
#include "Stage.hpp"
#include "Supervisor.hpp"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <deque>
#include <vector>

namespace Netplay::Th06Rollback
{
namespace
{
struct ScreenEffectFrame
{
    std::uint32_t startFrame = 0;
    std::uint32_t endFrame = 0;
    std::vector<ScreenEffect> effects;
};

// Keep the current TH07 production policy: two adjacent logical frames share
// one first-write set. TH06 has materially different object capacities, so the
// owner inventory below is intentionally TH06-specific rather than copied.
constexpr std::size_t CHECKPOINT_LOGICAL_FRAMES = 2;

RollbackJournal g_Journal;
Config g_Config{};
std::deque<ScreenEffectFrame> g_ScreenEffectFrames;
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
        return true;
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

bool IsScreenEffectCallback(ChainCallback callback)
{
    return callback == reinterpret_cast<ChainCallback>(ScreenEffect::CalcFadeIn) ||
           callback == reinterpret_cast<ChainCallback>(ScreenEffect::ShakeScreen) ||
           callback == reinterpret_cast<ChainCallback>(ScreenEffect::CalcFadeOut);
}

bool CaptureScreenEffects(std::uint32_t frame)
{
    if (g_ScreenEffectFrames.size() == CheckpointCapacity())
        g_ScreenEffectFrames.pop_front();
    g_ScreenEffectFrames.emplace_back();
    ScreenEffectFrame &snapshot = g_ScreenEffectFrames.back();
    snapshot.startFrame = frame;
    snapshot.endFrame = frame;

    for (ChainElem *element = g_Chain.NetplayFirstCalcElem(); element; element = element->next)
    {
        if (!IsScreenEffectCallback(element->callback))
            continue;
        if (!element->arg || snapshot.effects.size() >= g_Config.maxScreenEffectsPerFrame)
        {
            g_Failed = true;
            return false;
        }
        snapshot.effects.push_back(*static_cast<ScreenEffect *>(element->arg));
    }
    return true;
}

ScreenEffectFrame *FindScreenEffectFrame(std::uint32_t frame)
{
    const auto it = std::find_if(
        g_ScreenEffectFrames.begin(), g_ScreenEffectFrames.end(),
        [frame](const ScreenEffectFrame &entry) {
            return entry.startFrame <= frame && frame <= entry.endFrame;
        });
    return it == g_ScreenEffectFrames.end() ? nullptr : &*it;
}

void RemoveCurrentScreenEffects()
{
    ChainElem *element = g_Chain.NetplayFirstCalcElem();
    while (element)
    {
        ChainElem *next = element->next;
        if (IsScreenEffectCallback(element->callback))
            g_Chain.Cut(element);
        element = next;
    }
}

bool RestoreScreenEffects(const ScreenEffectFrame &snapshot)
{
    for (const ScreenEffect &saved : snapshot.effects)
    {
        ScreenEffect *restored = ScreenEffect::RegisterChain(
            saved.usedEffect, saved.effectLength, saved.genericParam,
            saved.shakinessParam, saved.unusedParam);
        if (!restored)
            return false;
        ChainElem *calc = restored->calcChainElement;
        ChainElem *draw = restored->drawChainElement;
        *restored = saved;
        restored->calcChainElement = calc;
        restored->drawChainElement = draw;
    }
    return true;
}

void ResetHistoryForStage(int stage)
{
    g_Journal.Reset(RollbackJournalConfig{CheckpointCapacity(),
                                           g_Config.maxBytesPerFrame,
                                           g_Config.maxBlocksPerFrame});
    g_ScreenEffectFrames.clear();
    g_HistoryStage = stage;
    g_FramesInCheckpoint = 0;
}

bool CaptureStageMutableState()
{
    if (!TouchObject(&g_Stage))
        return false;

    // TH06 Stage stores the active background ANM VMs out-of-line. They are
    // advanced by Stage::UpdateObjects and may execute random-sprite ANM
    // opcodes, so treating them as presentation-only would shift g_Rng after a
    // resimulation. Capture the concrete VM array plus the per-object active
    // flag which decides whether those scripts continue to execute.
    if (g_Stage.quadVms && g_Stage.quadCount > 0 &&
        !TouchMemory(g_Stage.quadVms,
                     sizeof(*g_Stage.quadVms) * static_cast<std::size_t>(g_Stage.quadCount)))
        return false;
    if (g_Stage.objects && g_Stage.objectsCount > 0)
    {
        for (int i = 0; i < g_Stage.objectsCount; ++i)
        {
            RawStageObject *object = g_Stage.objects[i];
            if (object && !TouchObject(&object->flags))
                return false;
        }
    }
    return true;
}

bool CaptureFixedAndSparseState()
{
    if (!TouchObject(&g_GameManager) || !TouchObject(&g_Rng) ||
        !TouchObject(&g_Gui) || !TouchObject(&g_AsciiManager) ||
        !TouchObject(EnemyEclInstr::GetRollbackState()))
        return false;
    if (g_Gui.impl && !TouchObject(g_Gui.impl))
        return false;
    // ReplayManager is committed-output bookkeeping in TH06.  Its frameId and
    // replay stream pointers advance only on the original forward pass while
    // rollback resimulation suppresses replay writes.  Rewinding the manager
    // without also rewinding the pointed-to replay bytes makes the next
    // forward frame resume from an old cursor and corrupts the recording.
    // None of the TH06 ReplayManager fields feed gameplay simulation, so keep
    // the entire object outside the rollback journal.
    if (!CaptureStageMutableState())
        return false;

    // Only Supervisor/input fields that can change fixed-60-Hz simulation are
    // rewindable. Renderer/device pointers and wall-clock timing remain local.
    if (!TouchObject(&g_Supervisor.wantedState) ||
        !TouchObject(&g_Supervisor.curState) ||
        !TouchObject(&g_Supervisor.wantedState2) ||
        !TouchObject(&g_Supervisor.isInEnding) ||
        !TouchObject(&g_Supervisor.effectiveFramerateMultiplier) ||
        !TouchObject(&g_Supervisor.framerateMultiplier))
        return false;

    if (!TouchObject(&g_CurFrameInput) || !TouchObject(&g_LastFrameInput) ||
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        !TouchObject(&g_CurFrameGameInputs) ||
        !TouchObject(&g_LastFrameGameInputs) ||
#endif
        !TouchObject(&g_IsEigthFrameOfHeldInput) ||
        !TouchObject(&g_NumOfFramesInputsWereHeld))
        return false;

    // TH06 Player is small enough to capture whole. In MP each stable player
    // owns an independent 80-shot pool/Bomb state, while the guest resource
    // sidecar and team-wipe countdown are equally simulation-authoritative.
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (!TouchObject(&g_PlayerActive) ||
        !TouchObject(&g_MultiplayerPlayerResources) ||
        !TouchObject(&g_MultiplayerContributionStats) ||
        !TouchObject(&g_teamWipeRetryFrames))
        return false;
    for (u8 playerId = 0; playerId < TH06_MULTI_MAX_PLAYERS; ++playerId)
    {
        if (g_PlayerActive[playerId] && !TouchObject(&g_Players[playerId]))
            return false;
    }
#else
    if (!TouchObject(&g_Player))
        return false;
#endif

    auto *enemyBegin = reinterpret_cast<std::uint8_t *>(&g_EnemyManager);
    auto *enemiesBegin = reinterpret_cast<std::uint8_t *>(&g_EnemyManager.enemies[0]);
    auto *enemiesEnd = enemiesBegin + sizeof(g_EnemyManager.enemies);
    auto *enemyManagerEnd = enemyBegin + sizeof(g_EnemyManager);
    if (!TouchRange(enemyBegin, enemiesBegin))
        return false;
    for (Enemy &enemy : g_EnemyManager.enemies)
        if (enemy.flags.isSlotOccupied && !TouchEnemy(&enemy))
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
    for (Bullet &bullet : g_BulletManager.bullets)
        if (bullet.state != 0 && !TouchBullet(&bullet))
            return false;
    if (!TouchRange(bulletsEnd, lasersBegin))
        return false;
    for (Laser &laser : g_BulletManager.lasers)
        if (laser.inUse && !TouchLaser(&laser))
            return false;
    if (!TouchRange(lasersEnd, bulletManagerEnd))
        return false;

    auto *itemsBegin = reinterpret_cast<std::uint8_t *>(&g_ItemManager.items[0]);
    auto *itemsEnd = itemsBegin + sizeof(g_ItemManager.items);
    auto *itemManagerEnd = reinterpret_cast<std::uint8_t *>(&g_ItemManager) + sizeof(g_ItemManager);
    for (Item &item : g_ItemManager.items)
        if (item.isInUse && !TouchItem(&item))
            return false;
    if (!TouchRange(itemsEnd, itemManagerEnd))
        return false;

    auto *effectManagerBegin = reinterpret_cast<std::uint8_t *>(&g_EffectManager);
    auto *effectsBegin = reinterpret_cast<std::uint8_t *>(&g_EffectManager.effects[0]);
    if (!TouchRange(effectManagerBegin, effectsBegin))
        return false;
    for (Effect &effect : g_EffectManager.effects)
        if (effect.inUseFlag && !TouchEffect(&effect))
            return false;

    return true;
}

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
} // namespace

bool Reset(const Config &config)
{
    if (config.maxFrames == 0 || config.maxBytesPerFrame == 0 ||
        config.maxBlocksPerFrame == 0 || config.maxScreenEffectsPerFrame == 0)
        return false;
    g_Config = config;
    g_ScreenEffectFrames.clear();
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
    g_ScreenEffectFrames.clear();
    g_Configured = false;
    g_Failed = false;
    g_HistoryStage = -1;
    g_FramesInCheckpoint = 0;
}

bool BeginFrame(std::uint32_t frame)
{
    if (!g_Configured || g_Failed || g_Journal.IsFrameOpen())
        return false;

    const int stage = g_GameManager.currentStage;
    if (g_HistoryStage != stage)
        ResetHistoryForStage(stage);

    const bool extendPrevious = g_FramesInCheckpoint != 0;
    if (!extendPrevious)
    {
        if (!CaptureScreenEffects(frame) || !g_Journal.BeginFrame(frame) ||
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
    if (extendPrevious && !g_ScreenEffectFrames.empty())
        g_ScreenEffectFrames.back().endFrame = frame;
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
    ScreenEffectFrame *snapshot = FindScreenEffectFrame(frame);
    if (!snapshot)
        return false;
    const ScreenEffectFrame saved = *snapshot;
    std::uint32_t restoredFrame = frame;

    RemoveCurrentScreenEffects();
    if (!g_Journal.UndoTo(frame, &restoredFrame) ||
        restoredFrame != saved.startFrame || !RestoreScreenEffects(saved))
    {
        g_Failed = true;
        return false;
    }
    while (!g_ScreenEffectFrames.empty() &&
           g_ScreenEffectFrames.back().endFrame >= restoredFrame)
        g_ScreenEffectFrames.pop_back();
    g_FramesInCheckpoint = 0;
    if (replayFrom)
        *replayFrom = restoredFrame;
    return true;
}

void DiscardBefore(std::uint32_t frame)
{
    g_Journal.DiscardBefore(frame);
    while (!g_ScreenEffectFrames.empty() &&
           g_ScreenEffectFrames.front().endFrame < frame)
        g_ScreenEffectFrames.pop_front();
}

bool IsCapturing() { return g_Journal.IsFrameOpen(); }
bool Failed() { return g_Failed || g_Journal.Failed(); }
std::size_t CapturedBytes(std::uint32_t frame) { return g_Journal.BytesForFrame(frame); }
std::size_t CapturedBlocks(std::uint32_t frame) { return g_Journal.BlocksForFrame(frame); }
std::size_t CapturedScreenEffects(std::uint32_t frame)
{
    ScreenEffectFrame *snapshot = FindScreenEffectFrame(frame);
    return snapshot ? snapshot->effects.size() : 0;
}

bool TouchEnemy(Enemy *enemy) { return TouchObject(enemy); }
bool TouchBullet(Bullet *bullet) { return TouchObject(bullet); }
bool TouchLaser(Laser *laser) { return TouchObject(laser); }
bool TouchItem(Item *item) { return TouchObject(item); }
bool TouchEffect(Effect *effect) { return TouchObject(effect); }

std::uint64_t DebugStateHash()
{
    // Same-runtime debug hash only. Pointer-bearing owners are intentionally
    // included here because this is for local restore/resim equivalence, not a
    // cross-browser canonical determinism proof.
    std::uint64_t hash = 1469598103934665603ull;
    HashObject(hash, g_GameManager);
    HashObject(hash, g_Rng);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    HashObject(hash, g_PlayerActive);
    HashObject(hash, g_MultiplayerPlayerResources);
    HashObject(hash, g_MultiplayerContributionStats);
    HashObject(hash, g_teamWipeRetryFrames);
    for (u8 playerId = 0; playerId < TH06_MULTI_MAX_PLAYERS; ++playerId)
        if (g_PlayerActive[playerId])
            HashObject(hash, g_Players[playerId]);
#else
    HashObject(hash, g_Player);
#endif
    HashObject(hash, g_EnemyManager);
    HashObject(hash, g_BulletManager);
    HashObject(hash, g_ItemManager);
    HashObject(hash, g_EffectManager);
    HashObject(hash, g_Gui);
    if (g_Gui.impl)
        HashObject(hash, *g_Gui.impl);
    HashObject(hash, g_AsciiManager);
    HashObject(hash, g_Stage);
    if (g_Stage.quadVms && g_Stage.quadCount > 0)
        HashBytes(hash, g_Stage.quadVms,
                  sizeof(*g_Stage.quadVms) * static_cast<std::size_t>(g_Stage.quadCount));
    HashObject(hash, g_CurFrameInput);
    HashObject(hash, g_LastFrameInput);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    HashObject(hash, g_CurFrameGameInputs);
    HashObject(hash, g_LastFrameGameInputs);
#endif
    HashObject(hash, g_IsEigthFrameOfHeldInput);
    HashObject(hash, g_NumOfFramesInputsWereHeld);
    return hash;
}
} // namespace Netplay::Th06Rollback
