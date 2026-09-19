#include "Th07RollbackState.hpp"

#include "CompactBulletSnapshot.hpp"
#include "LiveBulletSnapshot.hpp"
#include <eagler/netplay/RollbackJournal.hpp>
#include <eagler/netplay/SparsePoolCapture.hpp>
#include <eagler/netplay/NetplayInput.hpp>

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
#include <cstdio>
#include <vector>

#if defined(__EMSCRIPTEN__) && !defined(__wasm64__)
#include <emscripten/emscripten.h>
// One transition for a gather/scatter. No temporary TypedArray views, and
// acquire the current heap AFTER any snapshot capacity allocation/growth.
static_assert(sizeof(Netplay::LiveBulletJournal::CopyRegion) == 12);
EM_JS(void, th07_live_bullet_copy_batch,
      (void *destination, const void *source, const void *regions, std::size_t count), {
    const dst = destination >>> 0, src = source >>> 0;
    const u8 = HEAPU8, u32 = HEAPU32;
    let entry = (regions >>> 0) / 4;
    for (let i = 0; i < count; ++i, entry += 3) {
        const from = src + u32[entry], to = dst + u32[entry + 1], bytes = u32[entry + 2];
        if (from + bytes > u8.length || to + bytes > u8.length)
            throw new RangeError('live Bullet copy outside WASM heap');
        u8.copyWithin(to, from, from + bytes);
    }
});
#endif

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


RollbackJournal g_Journal;
LiveBulletJournal g_LiveBullets;
struct LiveBulletAuditFrame { std::uint32_t frame; std::vector<std::uint8_t> bytes; };
std::deque<LiveBulletAuditFrame> g_LiveBulletAudit;
std::uint32_t g_LiveBulletAuditRestores = 0;
SparsePoolCapture<1024> g_BulletCapture;
Config g_Config{};
std::deque<BombFrame> g_BombFrames;
struct BulletFrame
{
    std::uint32_t startFrame = 0;
    std::uint32_t endFrame = 0;
    bool compact = false;
    std::vector<std::uint8_t> bytes;
};
std::vector<BulletFrame> g_BulletFrames;
std::size_t g_BulletFirstFrame = 0;
std::size_t g_BulletFrameCount = 0;
bool g_BulletCheckpointCompact = false;
bool g_Configured = false;
bool g_Failed = false;
int g_HistoryStage = -1;
std::size_t g_FramesInCheckpoint = 0;

std::size_t CheckpointCapacity()
{
    return (g_Config.maxFrames + g_Config.checkpointLogicalFrames - 1) /
           g_Config.checkpointLogicalFrames;
}

LiveBulletJournal::CopyFunction LiveBulletCopyFunction()
{
#if defined(__EMSCRIPTEN__) && !defined(__wasm64__)
    return g_Config.fastBulkCopy ? th07_live_bullet_copy_batch : nullptr;
#else
    return nullptr;
#endif
}

BulletFrame &BackBulletFrame()
{
    return g_BulletFrames[(g_BulletFirstFrame + g_BulletFrameCount - 1) % g_BulletFrames.size()];
}

BulletFrame *FindBulletFrame(std::uint32_t frame)
{
    for (std::size_t index = 0; index < g_BulletFrameCount; ++index)
    {
        BulletFrame &entry = g_BulletFrames[(g_BulletFirstFrame + index) % g_BulletFrames.size()];
        if (entry.startFrame <= frame && frame <= entry.endFrame)
            return &entry;
    }
    return nullptr;
}

void ResetBulletFrameHistory()
{
    g_BulletFirstFrame = 0;
    g_BulletFrameCount = 0;
    g_BulletCheckpointCompact = false;
}

bool BeginBulletFrame(std::uint32_t frame)
{
    g_BulletCheckpointCompact = false;
    if (!g_Config.compactBulletSnapshots)
        return true;
    if (g_BulletFrames.empty())
        return false;
    if (g_BulletFrameCount == g_BulletFrames.size())
        g_BulletFirstFrame = (g_BulletFirstFrame + 1) % g_BulletFrames.size();
    else
        ++g_BulletFrameCount;

    BulletFrame &snapshot = BackBulletFrame();
    snapshot.startFrame = snapshot.endFrame = frame;
    std::size_t live = 0;
    for (int i = 0; i < 1024; ++i)
        live += g_BulletManager.bullets[i].state != BULLET_INACTIVE ? 1u : 0u;
    snapshot.compact = live * sizeof(Bullet) > snapshot.bytes.size();
    g_BulletCheckpointCompact = snapshot.compact;
    if (!snapshot.compact)
        return true;

    for (int i = 0; i < 1024; ++i)
    {
        if (!PackCompactBullet(g_BulletManager.bullets[i],
                snapshot.bytes.data() + static_cast<std::size_t>(i) * COMPACT_BULLET_BYTES,
                COMPACT_BULLET_BYTES))
            return false;
    }
    return true;
}

bool RestoreBulletFrame(const BulletFrame &snapshot)
{
    if (!snapshot.compact)
        return true;
    for (int i = 0; i < 1024; ++i)
    {
        if (!UnpackCompactBullet(g_BulletManager.bullets[i],
                snapshot.bytes.data() + static_cast<std::size_t>(i) * COMPACT_BULLET_BYTES,
                COMPACT_BULLET_BYTES))
            return false;
        RebuildCompactBulletPresentation(g_BulletManager.bullets[i]);
    }
    return true;
}

void DropBulletFramesFrom(std::uint32_t frame)
{
    while (g_BulletFrameCount != 0 && BackBulletFrame().endFrame >= frame)
        --g_BulletFrameCount;
    g_BulletCheckpointCompact = false;
}

void DiscardBulletFramesBefore(std::uint32_t frame)
{
    while (g_BulletFrameCount != 0 && g_BulletFrames[g_BulletFirstFrame].endFrame < frame)
    {
        g_BulletFirstFrame = (g_BulletFirstFrame + 1) % g_BulletFrames.size();
        --g_BulletFrameCount;
    }
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
    g_LiveBulletAudit.clear();
    g_Journal.Reset(RollbackJournalConfig{CheckpointCapacity(),
                                           g_Config.maxBytesPerFrame,
                                           g_Config.maxBlocksPerFrame,
                                           g_Config.fastBulkCopy, g_Config.coalesceRestore});
    g_LiveBullets.Clear();
    if (g_Config.liveBulletSnapshots && !g_LiveBullets.Reset(
            g_BulletManager.bullets, LiveBulletParts(), CheckpointCapacity(), LiveBulletCopyFunction()))
        g_Failed = true;
    g_BombFrames.clear();
    ResetBulletFrameHistory();
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
    if (!TouchObject(&Input::GetDirectTouchStates()))
        return false;
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
    if (g_Config.liveBulletSnapshots)
    {
        // The dedicated part journal owns every Bullet byte. Dormant spawn
        // VMs remain live and are saved only before spawn/clear overwrites.
        std::array<std::uint32_t, 1024> masks{};
        for (std::size_t i = 0; i < 1024; ++i)
            if (g_BulletManager.bullets[i].state != BULLET_INACTIVE)
                masks[i] = LiveBulletMask(g_BulletManager.bullets[i], g_Config.elideDormantBulletVm);
        if (!g_LiveBullets.CaptureMasks(masks)) return false;
    }
    else if (g_BulletCheckpointCompact)
    {
        // The complete simulation-relevant Bullet payload was packed before
        // this frame opened. Matrix-only presentation bytes intentionally stay
        // outside rollback and the live pool needs no first-write journal.
    }
    else if (g_Config.coalesceBulletRuns)
    {
        if (!g_BulletCapture.Capture(g_BulletManager.bullets,
                [](const Bullet &bullet) { return bullet.state != BULLET_INACTIVE; },
                [](void *data, std::size_t bytes) { return TouchMemory(data, bytes); }))
            return false;
    }
    else
    {
        for (int i = 0; i < 1024; ++i)
            if (g_BulletManager.bullets[i].state != BULLET_INACTIVE &&
                !TouchBullet(&g_BulletManager.bullets[i]))
                return false;
    }
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
    if (config.auditLiveBulletBytes && !config.liveBulletSnapshots) return false;
    g_LiveBulletAudit.clear();
    g_LiveBulletAuditRestores = 0;
    if (config.liveBulletSnapshots && config.compactBulletSnapshots)
        return false;
    if (config.maxFrames == 0 || config.maxBytesPerFrame == 0 ||
        config.maxBlocksPerFrame == 0 || config.maxBombEffectsPerFrame == 0 ||
        config.checkpointLogicalFrames == 0 ||
        config.checkpointLogicalFrames > config.maxFrames)
        return false;
    g_Config = config;
    g_LiveBullets.Clear();
    if (config.liveBulletSnapshots && !g_LiveBullets.Reset(
            g_BulletManager.bullets, LiveBulletParts(), CheckpointCapacity(), LiveBulletCopyFunction()))
        return false;
    g_BombFrames.clear();
    g_BulletFrames.clear();
    if (g_Config.compactBulletSnapshots)
    {
        g_BulletFrames.resize(CheckpointCapacity());
        for (BulletFrame &frame : g_BulletFrames)
            frame.bytes.resize(COMPACT_BULLET_BYTES * 1024u);
    }
    ResetBulletFrameHistory();
    g_Failed = false;
    g_HistoryStage = -1;
    g_FramesInCheckpoint = 0;
    g_Configured = g_Journal.Reset(RollbackJournalConfig{
        CheckpointCapacity(), config.maxBytesPerFrame, config.maxBlocksPerFrame,
        config.fastBulkCopy, config.coalesceRestore});
    return g_Configured;
}

void Clear()
{
    g_LiveBulletAudit.clear();
    g_Journal.Clear();
    g_LiveBullets.Clear();
    g_BombFrames.clear();
    g_BulletFrames.clear();
    ResetBulletFrameHistory();
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
    if (g_Failed || (g_Config.liveBulletSnapshots && !g_LiveBullets.BeginFrame(frame, extendPrevious)))
    {
        g_Failed = true;
        return false;
    }
    if (!extendPrevious)
    {
        if (g_Config.auditLiveBulletBytes)
        {
            while (g_LiveBulletAudit.size() >= CheckpointCapacity()) g_LiveBulletAudit.pop_front();
            g_LiveBulletAudit.push_back({frame, {}});
            auto &bytes = g_LiveBulletAudit.back().bytes;
            bytes.resize(sizeof(g_BulletManager.bullets));
            std::memcpy(bytes.data(), g_BulletManager.bullets, bytes.size());
        }
        if (!CaptureBombEffects(frame) || !BeginBulletFrame(frame) || !g_Journal.BeginFrame(frame) ||
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
    if (extendPrevious && g_Config.compactBulletSnapshots && g_BulletFrameCount != 0)
    {
        BackBulletFrame().endFrame = frame;
        g_BulletCheckpointCompact = BackBulletFrame().compact;
    }
    return true;
}

bool EndFrame()
{
    if (!g_Journal.EndFrame() || (g_Config.liveBulletSnapshots && !g_LiveBullets.EndFrame()))
    {
        g_Failed = true;
        return false;
    }
    g_FramesInCheckpoint =
        (g_FramesInCheckpoint + 1) % g_Config.checkpointLogicalFrames;
    return true;
}

bool RestoreTo(std::uint32_t frame, std::uint32_t *replayFrom)
{
    if (!g_Configured || g_Failed || g_Journal.IsFrameOpen())
        return false;
    BombFrame *snapshot = FindBombFrame(frame);
    if (!snapshot)
        return false;
    BulletFrame *bulletSnapshot = g_Config.compactBulletSnapshots ? FindBulletFrame(frame) : nullptr;
    if (g_Config.compactBulletSnapshots && !bulletSnapshot)
        return false;
    const BombFrame saved = *snapshot;
    std::uint32_t restoredFrame = frame;
    std::uint32_t liveRestoredFrame = frame;

    RemoveCurrentBombEffects();
    if (!g_Journal.UndoTo(frame, &restoredFrame) ||
        restoredFrame != saved.startFrame ||
        (g_Config.liveBulletSnapshots &&
            (!g_LiveBullets.UndoTo(frame, &liveRestoredFrame) || liveRestoredFrame != restoredFrame)) ||
        (bulletSnapshot && (bulletSnapshot->startFrame != restoredFrame ||
                            !RestoreBulletFrame(*bulletSnapshot))) ||
        !RestoreBombEffects(saved))
    {
        g_Failed = true;
        return false;
    }
    if (g_Config.auditLiveBulletBytes)
    {
        const auto found = std::find_if(g_LiveBulletAudit.begin(), g_LiveBulletAudit.end(),
            [restoredFrame](const auto &entry) { return entry.frame == restoredFrame; });
        if (found == g_LiveBulletAudit.end() ||
            std::memcmp(found->bytes.data(), g_BulletManager.bullets, found->bytes.size()) != 0)
        {
            std::fprintf(stderr, "live Bullet exact-byte audit failed frame=%u\n", restoredFrame);
            g_Failed = true;
            return false;
        }
        ++g_LiveBulletAuditRestores;
#ifdef __EMSCRIPTEN__
        EM_ASM({ globalThis.__eaglerLiveBulletAuditRestores = $0; }, g_LiveBulletAuditRestores);
#endif
        while (!g_LiveBulletAudit.empty() && g_LiveBulletAudit.back().frame >= restoredFrame)
            g_LiveBulletAudit.pop_back();
    }
    while (!g_BombFrames.empty() && g_BombFrames.back().endFrame >= restoredFrame)
        g_BombFrames.pop_back();
    if (g_Config.compactBulletSnapshots)
        DropBulletFramesFrom(restoredFrame);
    g_FramesInCheckpoint = 0;
    if (replayFrom)
        *replayFrom = restoredFrame;
    return true;
}

void DiscardBefore(std::uint32_t frame)
{
    // Keep the one checkpoint whose range may straddle the discard boundary.
    while (g_LiveBulletAudit.size() > 1 && g_LiveBulletAudit[1].frame <= frame)
        g_LiveBulletAudit.pop_front();
    g_Journal.DiscardBefore(frame);
    if (g_Config.liveBulletSnapshots) g_LiveBullets.DiscardBefore(frame);
    while (!g_BombFrames.empty() && g_BombFrames.front().endFrame < frame)
        g_BombFrames.pop_front();
    if (g_Config.compactBulletSnapshots)
        DiscardBulletFramesBefore(frame);
    // Confirmed-only stretches intentionally have no journal. A later first
    // prediction must open a fresh checkpoint, not extend across that gap.
    if (g_Journal.FrameCount() == 0)
        g_FramesInCheckpoint = 0;
}

bool IsCapturing()
{
    return g_Journal.IsFrameOpen();
}

std::uint64_t RestoreCopiedBytes() { return g_Journal.RestoreCopiedBytes() + g_LiveBullets.RestoreCopiedBytes(); }
std::uint64_t RestoreSkippedBytes() { return g_Journal.RestoreSkippedBytes() + g_LiveBullets.RestoreSkippedBytes(); }
std::uint64_t ArenaGrowths() { return g_Journal.ArenaGrowths() + g_LiveBullets.ArenaGrowths(); }

bool Failed()
{
    return g_Failed || g_Journal.Failed() || g_LiveBullets.Failed();
}

std::size_t CapturedBytes(std::uint32_t frame)
{
    std::size_t bytes = g_Journal.BytesForFrame(frame);
    if (g_Config.liveBulletSnapshots) bytes += g_LiveBullets.BytesForFrame(frame);
    if (g_Config.compactBulletSnapshots)
    {
        BulletFrame *snapshot = FindBulletFrame(frame);
        if (snapshot && snapshot->compact)
            bytes += snapshot->bytes.size();
    }
    return bytes;
}

std::size_t CapturedBlocks(std::uint32_t frame)
{
    return g_Journal.BlocksForFrame(frame) + g_LiveBullets.BlocksForFrame(frame);
}

std::size_t CapturedBombEffects(std::uint32_t frame)
{
    BombFrame *snapshot = FindBombFrame(frame);
    return snapshot ? snapshot->effects.size() : 0;
}

bool TouchEnemy(Enemy *enemy) { return TouchObject(enemy); }
bool TouchBullet(Bullet *bullet)
{
    if (!g_Journal.IsFrameOpen()) return true;
    if (g_Config.liveBulletSnapshots)
    {
        const auto first = reinterpret_cast<std::uintptr_t>(g_BulletManager.bullets);
        const auto address = reinterpret_cast<std::uintptr_t>(bullet);
        if (address < first || address - first >= sizeof(Bullet) * 1024u ||
            (address - first) % sizeof(Bullet) != 0 ||
            !g_LiveBullets.Touch((address - first) / sizeof(Bullet), LiveBulletJournal::AllParts))
        {
            g_Failed = true;
            return false;
        }
        return true;
    }
    if (g_BulletCheckpointCompact) return true;
    if (!g_Config.coalesceBulletRuns) return TouchObject(bullet);
    return g_BulletCapture.TouchSlot(g_BulletManager.bullets, bullet,
        [](void *data, std::size_t bytes) { return TouchMemory(data, bytes); });
}
void BeforeBulletDespawn(Bullet *bullet)
{
    if (!g_Config.liveBulletSnapshots || !g_Config.elideDormantBulletVm || !g_Journal.IsFrameOpen())
        return;
    // Promotion activates only the despawn VM. The tail and normal VM were
    // already saved at the checkpoint; unrelated spawn VMs remain untouched.
    // Whole-slot clear/reuse continues to call TouchBullet(AllParts).
    const auto first = reinterpret_cast<std::uintptr_t>(g_BulletManager.bullets);
    const auto address = reinterpret_cast<std::uintptr_t>(bullet);
    if (address < first || address - first >= sizeof(Bullet) * 1024u ||
        (address - first) % sizeof(Bullet) != 0 ||
        !g_LiveBullets.Touch((address - first) / sizeof(Bullet), 1u << 4))
        g_Failed = true;
}
bool TouchLaser(Laser *laser) { return TouchObject(laser); }
bool TouchItem(Item *item) { return TouchObject(item); }
bool TouchEffect(Effect *effect) { return TouchObject(effect); }
bool TouchPlayerBullet(PlayerBullet *bullet) { return TouchObject(bullet); }
bool TouchPlayerBombInfo(PlayerBombInfo *bombInfo) { return TouchObject(bombInfo); }

bool PatchDirectTouchSnapshots(std::size_t player, std::uint32_t afterFrame,
                               std::uint32_t throughFrame, float dx, float dy)
{
    auto &states = Input::GetDirectTouchStates();
    if (player >= states.size())
        return false;
    return g_Journal.AddFloatPairToSnapshots(afterFrame, throughFrame,
                                             &states[player].x, dx, dy);
}

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
    for (const auto &touch : Input::GetDirectTouchStates())
    {
        HashObject(hash, touch.x);
        HashObject(hash, touch.y);
        HashObject(hash, touch.active);
    }
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
