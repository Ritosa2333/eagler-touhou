#include "Th07LanStageProbe.hpp"

#include <eagler/netplay/DirectTouchEquivalence.hpp>
#include <eagler/netplay/RollbackReplayBudget.hpp>
#include <eagler/netplay/InputRepairBudget.hpp>
#include <eagler/netplay/SnapshotPolicy.hpp>
#include <eagler/netplay/NetplayCore.hpp>
#include <eagler/netplay/NetplayInput.hpp>
#include <eagler/netplay/NetplayProtocol.hpp>
#include <eagler/netplay/NetplaySession.hpp>
#include <eagler/netplay/InputRepairBudget.hpp>
#include <eagler/netplay/ConfirmedInputWatchdog.hpp>
#include <eagler/netplay/FrameAdvantageWindow.hpp>
#include <eagler/netplay/FramePacingPolicy.hpp>
#include "NetplaySideEffects.hpp"
#include "Th07CanonicalHash.hpp"
#include "Th07RollbackState.hpp"
#include <eagler/netplay/BrowserPeerTransport.hpp>
#include <eagler/netplay/WebSocketTransport.hpp>
#include <eagler/netplay/SpectatorStagePlayback.hpp>
#include <eagler/netplay/SpectatorStagePublisher.hpp>
#include <eagler/netplay/SpectatorStageScalars.hpp>
#include <eagler/netplay/SpectatorStageView.hpp>

#include "BulletManager.hpp"
#include "AsciiManager.hpp"
#include "Chain.hpp"
#include "Controller.hpp"
#include "EnemyManager.hpp"
#include "FileSystem.hpp"
#include "GameManager.hpp"
#include "Gui.hpp"
#include "ItemManager.hpp"
#include "Player.hpp"
#include "EclManager.hpp"
#include "Stage.hpp"
#include "ReplayExtension.hpp"
#include "ReplayManager.hpp"
#include "Supervisor.hpp"
#include "Touch.hpp"
#include "pbg4/Lzss.hpp"
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#include "multiplayer/GameplaySession.hpp"
#endif

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <utility>
#include <vector>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

namespace Netplay::Th07LanStageProbe
{
namespace
{
constexpr std::uint64_t SESSION_ID = 0x5448374c414e5331ull; // TH7LANS1
constexpr std::uint32_t GAME_ID_TH07 = 7;
// Gameplay ABI also guards the supported input semantics. ABI 5 accepts
// once-only touch deltas and keeps their remainder in rewindable state.
// Refuse older live peers before they could misinterpret those new modes.
static std::uint32_t GameplayAbi() { return TH07_MULTI_NETPLAY_ABI | (MultiplayerGameplay::IsChallengeMode() ? 0x10000u : 0u); }
constexpr std::uint32_t DEFAULT_TEST_FRAMES = 300;

WebSocketTransport g_Transport;
BrowserPeerTransport g_BrowserPeerTransport;
RollbackCore g_Core;
SessionGate g_Session;
bool g_Initialized = false;
bool g_Active = false;
bool g_Done = false;
bool g_ProductionTransportStarted = false;
bool g_LastTickAdvanced = false;
std::uint32_t g_SessionGeneration = 0;
std::uint8_t g_LocalPlayer = 0;
std::uint8_t g_PlayerCount = 2;
std::uint32_t g_SimFrame = 0;
std::uint32_t g_DriverTicks = 0;
std::uint32_t g_TestFrames = DEFAULT_TEST_FRAMES;
std::uint64_t g_ProbeStartedMs = 0;
std::uint32_t g_Sequence = 1;
std::uint32_t g_LastReceivedSequence = 0;
std::uint32_t g_SentPackets = 0;
std::uint32_t g_ReceivedPackets = 0;
std::uint32_t g_SessionPacketsSent = 0;
std::uint32_t g_SessionPacketsReceived = 0;
std::uint32_t g_LastHelloSendTick = 0;
std::uint32_t g_LastReadySendTick = 0;
std::uint32_t g_PredictedFrames = 0;
std::uint32_t g_RollbackCount = 0;
std::uint32_t g_ResimulatedFrames = 0;
std::uint32_t g_MaxRollbackSpan = 0;
std::size_t g_MaxSnapshotBytes = 0;
bool g_PerfTelemetry = false;
int g_TouchWorkload = 0;
bool g_TestIncrementalTouch = false;
// Diagnostic-only no-rollback endpoint used by the performance harness.
// Production multiplayer is always zero-added-delay + full rollback.
bool g_NoRollbackEndpoint = false;
bool g_DemandSnapshots = false;
bool g_FrontierSnapshots = false;
std::uint64_t g_SnapshotCostNs = 0;
std::uint64_t g_CanonicalCostNs = 0;
std::uint32_t g_SnapshotTicks = 0;
std::uint32_t g_ConfirmedOnlyTicks = 0;
bool g_TestKeepRollbackSnapshots = false;
std::uint32_t g_TestDenseBullets = 0;
bool g_TestDenseCurved = false;
std::uint64_t g_ForwardCostNs = 0;
std::uint64_t g_ForwardCalcCostNs = 0;
std::uint64_t g_ResimulationCalcCostNs = 0;
std::uint32_t g_DenseObservedFrames = 0;
std::uint32_t g_DenseMinBullets = 0xffffffffu;
std::uint64_t g_DenseBulletSum = 0;
std::uint64_t g_ResimulationCostNs = 0;
std::uint64_t g_ReconcileCostNs = 0;
std::uint64_t g_MaxReconcileCostNs = 0;
bool g_IncrementalReconcile = false;
bool g_ReconcileActive = false;
bool g_ReconcileShouldYield = false;
bool g_ReconcileTimerMode = false;
std::uint32_t g_ReconcileNextFrame = INVALID_FRAME;
std::uint32_t g_ReconcileLastFrame = INVALID_FRAME;
std::uint32_t g_ReconcileYieldCount = 0;
std::uint32_t g_ReconcileSliceCount = 0;
std::uint32_t g_MaxReconcileSliceFrames = 0;
std::uint32_t g_DrainYieldCount = 0;
std::uint32_t g_MaxPacketsDrainedPerTick = 0;
std::uint32_t g_MismatchTouchDelta = 0;
std::uint32_t g_MismatchButtons = 0;
std::uint32_t g_MismatchOtherAnalog = 0;
constexpr std::size_t TOUCH_EQUIVALENCE_HISTORY = 64;
struct TouchTraceSlot
{
    std::uint32_t frame = INVALID_FRAME;
    std::array<DirectTouchFrameTrace, MAX_PLAYERS> players{};
};
std::array<TouchTraceSlot, TOUCH_EQUIVALENCE_HISTORY> g_TouchTraces{};
bool g_TouchEquivalentAbsorb = false;
std::uint32_t g_TouchEquivalentAccepted = 0;
std::uint32_t g_TouchEquivalentRejected = 0;
std::array<std::uint32_t, 7> g_TouchEquivalentRejectReasons{};
std::uint32_t g_LastTouchEquivalentSimFrame = INVALID_FRAME;
std::uint32_t g_LocalCaptures = 0;
bool g_ReliableInputRepair = false;
std::array<InputRepairBudget, MAX_PLAYERS> g_InputRepairBudgets{};
double g_CapturedTouchX = 0.0;
double g_CapturedTouchY = 0.0;
std::uint32_t g_InputCaptureFrame = 0;
std::uint64_t g_NextInputCaptureNs = 0;
FrameInput g_LastPhysicalCapture{};
bool g_HaveLastPhysicalCapture = false;
float g_TouchQuantizationStep = 0.0f;
float g_TouchQuantResidualX = 0.0f;
float g_TouchQuantResidualY = 0.0f;

void SetReconcileTimerMode(bool enabled)
{
#ifdef __EMSCRIPTEN__
    if (!g_IncrementalReconcile || g_ReconcileTimerMode == enabled)
        return;
    // Normal SDL web iteration is requestAnimationFrame-driven. While a
    // historical replay is intentionally split across callbacks, switch the
    // Emscripten loop to a short timer so progress does not depend on a visual
    // swap. Restore RAF as soon as the old live frontier is reached.
    emscripten_set_main_loop_timing(enabled ? EM_TIMING_SETTIMEOUT : EM_TIMING_RAF,
                                    enabled ? 1 : 1);
    g_ReconcileTimerMode = enabled;
#else
    (void)enabled;
#endif
}

struct ReplayFrameBinding
{
    std::uint32_t simFrame = INVALID_FRAME;
    i32 stage = -1;
    i32 replayFrame = -1;
};
std::array<ReplayFrameBinding, INPUT_HISTORY_SIZE> g_ReplayFrameBindings{};
std::array<ConfirmedInputWatchdog, MAX_PLAYERS> g_RemoteInputWatchdogs{};
bool g_IndependentInputSlotsObserved = false;
std::uint8_t g_InputSlotObservedMask = 0;
bool g_PhysicalInputObserved = false;
bool g_PhysicalLoggedFrame0Sample = false;
bool g_PhysicalLoggedFrame0Wait = false;
bool g_PhysicalLoggedFrame0Sim = false;
bool g_PauseObserved = false;
bool g_PauseResumeObserved = false;
std::uint8_t g_HighestStageObserved = 0;
bool g_StageTransitionObserved = false;
bool g_StageTransitionInputResetObserved = false;
bool g_PostTransitionInputObserved = false;
bool g_EndingSkipHistoryNormalized = false;
bool g_ReplayPlaybackCycleDispatched = false;
std::uint32_t g_NextReplayAuditFrame = 0;
bool g_SpectatorMode = false;
bool g_SpectatorRunRetired = false;
std::uint32_t g_NextSpectatorPublishFrame = 0;
std::uint32_t g_NextSpectatorReceiveFrame = 0;
Netplay::SpectatorStagePublisher g_SpectatorPublisher;
Netplay::SpectatorStagePlayback g_SpectatorPlayback;
Netplay::SpectatorStageView g_SpectatorView;
Netplay::SpectatorStageEvent g_PendingSpectatorBootstrap{};
bool g_HavePendingSpectatorBootstrap = false;
bool g_SpectatorNativeStageReady = false;
bool g_SpectatorPublisherReady = false;
bool g_SpectatorPublisherEligible = false;
bool g_SpectatorLaneStopped = false;
// Audit-only stage marker for all source seats. Publisher checkpoint decisions
// use the rollback-aware recorded predecessor in SpectatorStagePublisher.
std::uint8_t g_SpectatorAuditCheckpointStage = 0;
bool g_SpectatorSizeProbeDone = false;
int g_SpectatorSizeProbeTarget = -1;
std::uint8_t g_SpectatorBossSlotsThisFrame = 0;
std::uint32_t g_PeakEnemies = 0;
std::uint32_t g_PeakBullets = 0;
std::uint32_t g_PeakLasers = 0;
std::uint32_t g_PeakItems = 0;
std::array<std::uint32_t, MAX_PLAYERS> g_LastRemoteSenderFrame{};
std::array<FrameAdvantageWindow, MAX_PLAYERS> g_PeerTimeSync{};
std::array<std::uint32_t, MAX_PLAYERS> g_PredictionDepth{};
std::array<std::uint32_t, MAX_PLAYERS> g_RollbackByPlayer{};
double g_RecommendedLead = 0.0;
double g_SimulationIntervalScale = 1.0;
std::uint32_t g_InitialFrameLag = 0;
std::uint32_t g_InitialFrameLagRemaining = 0;
bool g_DisableTimeSyncPacing = false;

bool UsePhysicalInput();
bool UseReplayPlaybackCycle();
bool UsePauseCycle();
bool UseRestartCycle();
bool UseStageTransitionTest();
void ReportSpectatorCheckpointHash(std::uint32_t frame, std::uint64_t hash);
void DisableSpectatorPublisher(std::uint32_t frame, const char *reason);

bool AllRemoteInputsReady(std::uint32_t frame)
{
    for (std::uint8_t player = 0; player < g_PlayerCount; ++player)
    {
        if (player == g_LocalPlayer)
            continue;
        const std::uint32_t confirmed = g_Core.ConfirmedThrough(player);
        if (confirmed == INVALID_FRAME || confirmed < frame)
            return false;
    }
    return true;
}

void InjectDenseBulletLoad(std::uint32_t frame)
{
    if (g_TestDenseBullets == 0 || frame != 240)
        return;

    // Test-only moving BulletManager load AFTER the initial respawn grace.
    // The former frame-60 injection was immediately cleared by Player and
    // therefore a peak of 900 did not prove sustained high-density gameplay.
    // Normal movement, ANM, collision, snapshot and drawing remain active.
    // Tests must assert the observed minimum, not just the creation count.
    EnemyBulletShooter shooter;
    shooter.sprite = 0;
    shooter.spriteOffset = 0;
    shooter.count1 = 1;
    shooter.count2 = 1;
    shooter.aimMode = BULLET_AIM_RING_ABSOLUTE;
    shooter.speed1 = 0.0625f;
    shooter.speed2 = 0.0625f;
    shooter.flags = 0;
    if (g_TestDenseCurved)
    {
        // Sustained nontrivial engine command workload. The radius stays in
        // the upper playfield; do not turn off normal ANM/collision processing.
        shooter.speed1 = shooter.speed2 = 1.5f;
        shooter.AddAngleAccelCommand(0, 0, 1000000, 0.06f, 0.0f);
    }
    for (std::uint32_t index = 0; index < g_TestDenseBullets; ++index)
    {
        if (g_TestDenseCurved) shooter.sprite = static_cast<i16>(index % 8u);
        shooter.pos.x = 96.0f + static_cast<float>(index % 24u) * 8.0f;
        shooter.pos.y = 96.0f + static_cast<float>((index / 24u) % 20u) * 8.0f;
        shooter.angle1 = static_cast<float>(index % 4u) * 1.5707963267948966f;
        shooter.pos.z = 0.0f;
        if (g_BulletManager.SpawnBulletPattern(&shooter) != 0)
            break;
    }
}
bool ProbeMode();
bool ProductionLanMode();
void ClearTransientModes();
std::uint16_t LocalScript(std::uint8_t player, std::uint32_t frame);

std::uint64_t CurrentSessionId()
{
    // A retry/new run is a new rollback session even if the browser keeps the
    // same RTC/DataChannel transport alive. Generation 0 preserves the wire id
    // used by existing rooms; later generations derive a distinct id so late
    // packets from the previous run are ignored instead of contaminating it.
    return SESSION_ID ^
           (static_cast<std::uint64_t>(g_SessionGeneration) *
            0x9e3779b97f4a7c15ull);
}

constexpr std::uint32_t SPECTATOR_CHECKPOINT_SCHEMA = 5;
constexpr std::uint32_t SPECTATOR_CHECKPOINT_MAGIC = 0x37564353; // SCV7
constexpr std::uint32_t SPECTATOR_CHECKPOINT_RAW_LIMIT = 2u * 1024u * 1024u;
constexpr std::int32_t SPECTATOR_SIZE_PROBE_MAX_POWER = 128;

void WriteCheckpointWord(std::vector<std::uint8_t> *out, std::uint32_t value)
{
    for (unsigned n = 0; n < 4; ++n) out->push_back(static_cast<std::uint8_t>(value >> (n * 8)));
}

std::uint32_t ReadCheckpointWord(const std::uint8_t *data)
{
    std::uint32_t value = 0;
    for (unsigned n = 0; n < 4; ++n) value |= std::uint32_t(data[n]) << (n * 8);
    return value;
}

std::vector<std::uint8_t> PackCheckpointSparse(const std::vector<std::uint8_t> &raw)
{
    std::vector<std::uint8_t> packed;
    packed.reserve(raw.size() + ((raw.size() + 63u) / 64u) * 8u);
    for (std::size_t at = 0; at < raw.size(); at += 64)
    {
        const std::size_t maskOffset = packed.size();
        const std::size_t count = std::min<std::size_t>(64, raw.size() - at);
        packed.resize(maskOffset + 8, 0);
        for (std::size_t n = 0; n < count; ++n)
        {
            if (raw[at + n] == 0) continue;
            packed[maskOffset + n / 8] |= static_cast<std::uint8_t>(1u << (n % 8));
            packed.push_back(raw[at + n]);
        }
    }
    return packed;
}

bool UnpackCheckpointSparse(const std::vector<std::uint8_t> &packed, std::uint32_t rawSize,
                            std::vector<std::uint8_t> *raw)
{
    if (!raw || rawSize == 0 || rawSize > SPECTATOR_CHECKPOINT_RAW_LIMIT) return false;
    std::vector<std::uint8_t> next(rawSize, 0);
    std::size_t read = 0;
    for (std::size_t at = 0; at < rawSize; at += 64)
    {
        if (packed.size() - read < 8) return false;
        const std::size_t maskOffset = read;
        const std::size_t count = std::min<std::size_t>(64, rawSize - at);
        read += 8;
        for (std::size_t n = 0; n < 64; ++n)
        {
            if ((packed[maskOffset + n / 8] & (1u << (n % 8))) == 0) continue;
            if (n >= count || read >= packed.size() || packed[read] == 0) return false;
            next[at + n] = packed[read++];
        }
    }
    if (read != packed.size()) return false;
    *raw = std::move(next);
    return true;
}

bool CompressSpectatorCheckpoint(const std::vector<std::uint8_t> &raw,
                                 std::vector<std::uint8_t> *capsule)
{
    if (!capsule || raw.empty() || raw.size() > SPECTATOR_CHECKPOINT_RAW_LIMIT) return false;
    const auto sparse = PackCheckpointSparse(raw);
    if (sparse.empty() || sparse.size() > 0x7fffffffu) return false;
    i32 compressedSize = 0;
    u8 *compressed = Lzss::Compress(const_cast<u8 *>(sparse.data()), static_cast<i32>(sparse.size()),
                                    &compressedSize);
    if (!compressed || compressedSize <= 0)
    {
        std::free(compressed);
        return false;
    }
    const std::size_t capsuleSize = 12u + static_cast<std::size_t>(compressedSize);
    if (capsuleSize > SpectatorStageMaxStateBytes)
    {
        std::free(compressed);
        return false;
    }
    capsule->clear();
    capsule->reserve(capsuleSize);
    WriteCheckpointWord(capsule, SPECTATOR_CHECKPOINT_MAGIC);
    WriteCheckpointWord(capsule, static_cast<std::uint32_t>(raw.size()));
    WriteCheckpointWord(capsule, static_cast<std::uint32_t>(sparse.size()));
    capsule->insert(capsule->end(), compressed, compressed + compressedSize);
    std::free(compressed);
    return true;
}

bool DecompressSpectatorCheckpoint(const std::vector<std::uint8_t> &capsule,
                                   std::vector<std::uint8_t> *raw)
{
    if (!raw || capsule.size() < 13 || capsule.size() > SpectatorStageMaxStateBytes ||
        ReadCheckpointWord(capsule.data()) != SPECTATOR_CHECKPOINT_MAGIC)
        return false;
    const std::uint32_t rawSize = ReadCheckpointWord(capsule.data() + 4);
    const std::uint32_t sparseSize = ReadCheckpointWord(capsule.data() + 8);
    if (rawSize == 0 || rawSize > SPECTATOR_CHECKPOINT_RAW_LIMIT) return false;
    const std::uint32_t masks = ((rawSize + 63u) / 64u) * 8u;
    if (sparseSize < masks || sparseSize > rawSize + masks || sparseSize > 0x7fffffffu) return false;
    std::vector<std::uint8_t> sparse(sparseSize, 0);
    auto compressed = capsule;
    auto *expanded = Lzss::Decompress(compressed.data() + 12,
                                     static_cast<i32>(compressed.size() - 12),
                                     sparse.data(), sparseSize);
    if (!expanded) return false;
    // Lzss::Decompress bounds its output but tolerates zero-padded truncated
    // streams. Re-encode to require a canonical, complete compressed payload.
    i32 canonicalSize = 0;
    u8 *canonical = Lzss::Compress(sparse.data(), static_cast<i32>(sparse.size()), &canonicalSize);
    if (!canonical || canonicalSize != static_cast<i32>(capsule.size() - 12) ||
        std::memcmp(canonical, capsule.data() + 12, capsule.size() - 12) != 0)
    {
        std::free(canonical);
        return false;
    }
    std::free(canonical);
    return UnpackCheckpointSparse(sparse, rawSize, raw);
}

struct SpectatorPlayerState;
struct SpectatorPlayerBulletState;
struct SpectatorAnmState;
struct SpectatorCheckpoint
{
    std::uint32_t magic = 0x37504353; // SCP7
    std::uint32_t stage = 0;
    std::uint64_t expectedHash = 0;
    std::uint64_t expectedMeta = 0, expectedStageHash = 0;
    std::uint64_t expectedPlayer0 = 0, expectedPlayer1 = 0, expectedPlayer2 = 0;
    std::uint64_t expectedMetaRng = 0, expectedMetaGame = 0;
    std::uint64_t expectedMetaInput = 0, expectedMetaSupervisor = 0;
    std::uint16_t stageRngSeed = 0, rngSeed = 0, rngSeedBackup = 0;
    std::uint32_t rngGeneration = 0;
    std::int32_t difficulty = 0, difficultyMask = 0, cherryMax = 0, cherry = 0;
    std::int32_t cherryPlus = 0, subrank = 0, rank = 0, maxRank = 0, minRank = 0;
    std::int32_t currentStage = 0, framesThisStage = 0;
    std::uint32_t flags = 0, bulletLagTime = 0;
    std::uint8_t isTimeStopped = 0, slowModeSlowActive = 0, isPaused = 0;
    std::uint8_t powerItemCountForScore = 0, character = 0, shotType = 0;
    std::uint8_t shotTypeAndCharacter = 0, isInPauseMenu = 0, isInRetryMenu = 0;
    std::uint8_t demoIdx = 0, replayStage = 0, phantasmUnlocked = 0;
    std::int32_t demoFrames = 0;
    float arcadeTopX = 0, arcadeTopY = 0, arcadeSizeX = 0, arcadeSizeY = 0;
    float movementTopX = 0, movementTopY = 0, movementSizeX = 0, movementSizeY = 0;
    float csumFloat = 0;
    std::uint32_t guiScore = 0, score = 0, guiScoreDifference = 0, highScore = 0;
    std::int32_t grazeInStage = 0, grazeInTotal = 0, spellCardsCaptured = 0;
    std::uint8_t numRetries = 0;
    std::int32_t pointItemsThisStage = 0, pointItemsForExtend = 0;
    std::int32_t extendsFromPointItems = 0, nextNeededPointItems = 0;
    std::array<std::int32_t, 7> globalsRng1{};
    float deaths = 0;
    std::array<float, 2> globalsRngFloat1{};
    float livesRemaining = 0;
    std::array<float, 2> globalsRngFloat2{};
    float bombsRemaining = 0, bombsUsed = 0;
    std::array<float, 3> globalsRngFloat3{};
    float currentPower = 0;
    std::array<float, 2> globalsRngFloat4{};
    std::int32_t cherryStart = 0;
    std::array<std::int32_t, 8> globalsRng2{};
    std::uint32_t curCsum = 0;
    std::int32_t csumAsSum = 0;
    std::array<std::int32_t, 5> csumData{};
    MultiplayerGameplay::SessionState session{};
    std::array<std::uint8_t, TH07_MULTI_MAX_PLAYERS> playerActive{};
    std::array<std::int32_t, TH07_MULTI_MAX_PLAYERS> lives{}, bombs{}, powers{};
    std::array<std::uint32_t, TH07_MULTI_MAX_PLAYERS> kills{}, damage{}, challengeDeaths{};
    std::array<std::int32_t, TH07_MULTI_MAX_PLAYERS> playerState{};
    std::array<float, TH07_MULTI_MAX_PLAYERS * 6> playerPositions{};
    std::array<std::int32_t, TH07_MULTI_MAX_PLAYERS> playerInvulnerabilityCurrent{};
    std::array<std::int32_t, TH07_MULTI_MAX_PLAYERS> playerInvulnerabilityPrevious{};
    std::array<float, TH07_MULTI_MAX_PLAYERS> playerInvulnerabilitySubFrame{};
    std::array<std::uint16_t, TH07_MULTI_MAX_PLAYERS> currentInputs{}, lastInputs{};
    std::array<float, TH07_MULTI_MAX_PLAYERS * 2> directTouchXY{};
    std::array<std::uint8_t, TH07_MULTI_MAX_PLAYERS> directTouchActive{};
    std::uint16_t currentRawInput = 0, lastRawInput = 0;
    std::uint16_t eighthHeld = 0, heldFrames = 0;
    std::array<std::int32_t, 4> globalEclInts{};
    std::array<float, 4> globalEclFloats{};
    std::array<std::int32_t, TH07_MULTI_MAX_PLAYERS> cherryMaxGrazeGrowth{};
    std::array<std::int32_t, TH07_MULTI_MAX_PLAYERS> cherryMaxBreakGrowth{};
    std::array<std::int32_t, TH07_MULTI_MAX_PLAYERS> powerGiveTaps{};
    std::array<std::int32_t, TH07_MULTI_MAX_PLAYERS> powerGiveWindow{};
    std::int32_t teamWipeRetryFrames = 0;
    std::int32_t bulletManagerCount = 0, bulletManagerScreenClearTime = 0;
    std::int32_t bulletManagerTimePrevious = 0, bulletManagerTimeCurrent = 0;
    float bulletManagerTimeSubFrame = 0;
    std::int32_t bulletManagerUpdateCount = 0, bulletManagerItemType = 0;
    std::int32_t stageScriptTimePrevious = 0, stageScriptTimeCurrent = 0;
    float stageScriptTimeSubFrame = 0;
    std::int32_t stageInstructionIndex = 0, stageFrameCounter = 0;
    std::uint32_t stageId = 0;
    std::array<float, 6> stagePositions{};
    std::uint32_t stageColor = 0;
    std::array<float, 6> stageFogPlanes{};
    std::array<std::uint32_t, 3> stageFogColors{};
    std::int32_t stageSkyFogInterpDuration = 0, stageSkyFogInterpPrevious = 0;
    std::int32_t stageSkyFogInterpCurrent = 0;
    float stageSkyFogInterpSubFrame = 0;
    std::int32_t stageSpellCardState = 0, stageTicksSinceSpellcardStarted = 0;
    std::uint8_t stageClearBackground = 0;
    std::int32_t stageScriptWaitTime = 0;
    std::array<std::int32_t, 4> stageTimersMax{}, stageEaseModes{};
    std::array<std::int32_t, 4> stageTimerPrevious{}, stageTimerCurrent{};
    std::array<float, 4> stageTimerSubFrame{};
    std::array<float, 3> stagePositionStart{}, stagePositionInterpInitial{};
    std::int32_t stagePositionInterpEndTime = 0, stagePositionInterpStartTime = 0;
    std::uint8_t stageCameraTeleported = 0, stageIsDarkening = 0;
    std::uint32_t stageColor2 = 0;
    std::array<float, 96> stageCameras{};
    std::array<std::uint8_t, 2> stageVmActive{};
    std::vector<SpectatorAnmState> stageVms;
    std::array<std::int32_t, 16> enemyTimelinePrevious{}, enemyTimelineCurrent{};
    std::array<float, 16> enemyTimelineSubFrame{};
    std::array<std::int32_t, 16> enemyTimelineFileOffsets{};
    std::int32_t enemyTimelineTimePrevious = 0, enemyTimelineTimeCurrent = 0;
    float enemyTimelineTimeSubFrame = 0;
    std::int32_t supervisorWanted = 0, supervisorCurrent = 0, supervisorPrevious = 0;
    std::int32_t supervisorFlags = 0;
    bool supervisorEnding = false;
    float supervisorFramerate = 1.0f;
    std::vector<SpectatorPlayerState> detailedPlayers;
    std::vector<SpectatorPlayerBulletState> playerBullets;
};

struct SpectatorVec3State { float x = 0, y = 0, z = 0; };
struct SpectatorFloat2State { float x = 0, y = 0; };
struct SpectatorTimerState { i32 previous = 0, current = 0; float subFrame = 0; };
struct SpectatorShtReference
{
    bool valid = false, focusTable = false;
    std::uint16_t level = 0, entry = 0;
};
struct SpectatorAnmState
{
    i16 anmFileIndex = -1, activeSpriteIndex = -1, baseSpriteIndex = -1;
    i32 instructionOffset = -1, flags = 0, autoRotate = 0, pendingInterrupt = 0;
    i32 timeOfLastSpriteSet = 0;
    std::array<u8, 5> easeModes{};
    std::array<i32, 4> intVars1{};
    std::array<i32, 2> intVars2{};
    std::array<float, 4> floatVars{};
    SpectatorVec3State rotation{}, previousRotation{}, angleVelocity{}, position{}, previousPosition{};
    SpectatorVec3State positionInterpInitial{}, positionInterpFinal{}, rotationInterpInitial{}, rotationInterpFinal{}, offset{};
    SpectatorFloat2State scale{}, previousScale{}, scaleGrowth{}, uvScrollPosition{}, previousUvScrollPosition{};
    SpectatorFloat2State uvScrollVelocity{}, scaleInterpInitial{}, scaleInterpFinal{};
    SpectatorTimerState scriptTime{}, waitTime{};
    std::array<SpectatorTimerState, 5> interpStartTimes{}, interpEndTimes{};
    u32 color = 0, color2 = 0, previousColor = 0, previousColor2 = 0;
    u32 colorInterpInitial = 0, colorInterpFinal = 0;
    u8 fontWidth = 0, fontHeight = 0;
};
struct SpectatorPlayerBulletState
{
    u8 player = 0, slot = 0;
    SpectatorShtReference shtEntry{};
    SpectatorVec3State position{}, previousPosition{}, hitboxSize{};
    std::array<SpectatorVec3State, 16> positionHistory{};
    SpectatorFloat2State velocity{}, offset{};
    float speed = 0, angle = 0, previousAngle = 0;
    SpectatorTimerState timer{};
    i16 damage = 0, bulletState = 0, bulletState2 = 0, timerIndex = 0, optionId = 0, trailLength = 0;
    bool hasVm = false;
    SpectatorAnmState vm{};
};
struct SpectatorPlayerTimerState
{
    SpectatorTimerState timer{};
    i16 bulletSlot = -1;
};
struct SpectatorBombProjectileState
{
    SpectatorVec3State position{}, size{};
    i32 lifetime = 0, damage = 0;
};
struct SpectatorBombClearBoxState
{
    SpectatorFloat2State position{}, size{};
    float radius = 0, radiusGrowth = 0;
    i32 lifetime = 0, itemType = 0;
};
struct SpectatorBombSubState
{
    i32 state = 0, counter = 0;
    float custom = 0, previousCustom = 0, speed = 0, angle = 0;
    SpectatorVec3State position{}, previousPosition{};
    std::array<SpectatorVec3State, 32> positionHistory{};
    SpectatorVec3State velocity{}, acceleration{};
    SpectatorTimerState timer{};
    std::array<bool, 8> hasVm{};
    std::array<SpectatorAnmState, 8> vms{};
};
struct SpectatorPlayerState
{
    SpectatorVec3State position{}, previousPosition{}, hitboxTopLeft{}, hitboxBottomRight{};
    SpectatorVec3State grazeTopLeft{}, grazeBottomRight{}, grabTopLeft{}, grabBottomRight{};
    SpectatorVec3State hitboxSize{}, grazeSize{}, grabSize{};
    std::array<SpectatorVec3State, 2> optionsPosition{}, previousOptionsPosition{};
    SpectatorFloat2State velocity{};
    i32 unused9d4 = 0;
    std::array<SpectatorBombProjectileState, 112> bombDamageBoxes{};
    std::array<SpectatorBombClearBoxState, 96> bombClearBoxes{};
    i32 numActiveBombClearBoxes = 0;
    bool dirtyBombBoxes = false, isBombing = false;
    float horizontalSpeedDuringBomb = 0, verticalSpeedDuringBomb = 0;
    i32 respawnTimer = 0, borderInvulnerabilityTime = 0, bulletGracePeriod = 0, itemType = 0;
    i8 playerState = 0, optionState = 0, isFocus = 0, hasBorder = 0;
    u8 initParam = 0, bombParticleTime = 0;
    SpectatorTimerState focusMovementTimer{};
    i32 playerDirection = 0;
    float previousHorizontalSpeed = 0, previousVerticalSpeed = 0;
    SpectatorVec3State positionOfLastEnemyHit{}, sakuyaTargetPosition{};
    bool targetingEnemy = false;
    std::array<SpectatorShtReference, 4> shtEntries{};
    std::array<SpectatorPlayerTimerState, 3> timers{};
    SpectatorTimerState fireBulletTimer{}, invulnerabilityTimer{}, borderTimer{}, teamBombProtectionTimer{};
    i32 lifeGiveTimer = 0, lifeGiveTargetToken = 0, unused16a18 = 0, unused16a1c = 0;
    bool bombIsInUse = false, bombIsFocus = false;
    i32 bombDuration = 0, bombCherryDrain = 0;
    SpectatorTimerState bombTimer{};
    std::array<SpectatorBombSubState, 128> bombSubInfo{};
    SpectatorVec3State bombStartPosition{};
    float optionAngle = 0;
};

SpectatorVec3State CaptureSpectatorVec3(const ZunVec3 &source)
{
    return {source.x, source.y, source.z};
}

void ApplySpectatorVec3(const SpectatorVec3State &source, ZunVec3 *out)
{
    out->x = source.x; out->y = source.y; out->z = source.z;
}

SpectatorFloat2State CaptureSpectatorFloat2(const Float2 &source)
{
    return {source.x, source.y};
}

void ApplySpectatorFloat2(const SpectatorFloat2State &source, Float2 *out)
{
    out->x = source.x; out->y = source.y;
}

SpectatorTimerState CaptureSpectatorTimer(const ZunTimer &source)
{
    return {source.previous, source.current, source.subFrame};
}

void ApplySpectatorTimer(const SpectatorTimerState &source, ZunTimer *out)
{
    out->previous = source.previous; out->current = source.current; out->subFrame = source.subFrame;
}

bool WriteSpectatorVec3(SpectatorScalars::Writer &writer, const SpectatorVec3State &value)
{
    return writer.Values(value.x, value.y, value.z);
}

bool ReadSpectatorVec3(SpectatorScalars::Reader &reader, SpectatorVec3State *value)
{
    return reader.Values(value->x, value->y, value->z);
}

bool WriteSpectatorFloat2(SpectatorScalars::Writer &writer, const SpectatorFloat2State &value)
{
    return writer.Values(value.x, value.y);
}

bool ReadSpectatorFloat2(SpectatorScalars::Reader &reader, SpectatorFloat2State *value)
{
    return reader.Values(value->x, value->y);
}

bool WriteSpectatorTimer(SpectatorScalars::Writer &writer, const SpectatorTimerState &value)
{
    return writer.Values(value.previous, value.subFrame, value.current);
}

bool ReadSpectatorTimer(SpectatorScalars::Reader &reader, SpectatorTimerState *value)
{
    return reader.Values(value->previous, value->subFrame, value->current);
}

template<std::size_t N>
bool WriteSpectatorVec3Array(SpectatorScalars::Writer &writer,
                             const std::array<SpectatorVec3State, N> &values)
{
    for (const auto &value : values) if (!WriteSpectatorVec3(writer, value)) return false;
    return true;
}

template<std::size_t N>
bool ReadSpectatorVec3Array(SpectatorScalars::Reader &reader,
                            std::array<SpectatorVec3State, N> *values)
{
    for (auto &value : *values) if (!ReadSpectatorVec3(reader, &value)) return false;
    return true;
}

template<std::size_t N>
bool WriteSpectatorTimerArray(SpectatorScalars::Writer &writer,
                              const std::array<SpectatorTimerState, N> &values)
{
    for (const auto &value : values) if (!WriteSpectatorTimer(writer, value)) return false;
    return true;
}

template<std::size_t N>
bool ReadSpectatorTimerArray(SpectatorScalars::Reader &reader,
                             std::array<SpectatorTimerState, N> *values)
{
    for (auto &value : *values) if (!ReadSpectatorTimer(reader, &value)) return false;
    return true;
}

bool CaptureSpectatorShtReference(Player &player, const ShtEntry *entry,
                                  SpectatorShtReference *out)
{
    *out = {};
    if (!entry) return true;
    for (bool focus : {false, true})
    {
        ShtData *data = focus ? player.shooterDataFocus : player.shooterData;
        if (!data || data->numLevels > 32) continue;
        for (std::uint16_t level = 0; level < data->numLevels; ++level)
        {
            ShtEntry *candidate = data->levels[level].entry;
            if (!candidate) continue;
            for (std::uint16_t index = 0; index < 256; ++index, ++candidate)
            {
                if (candidate == entry)
                {
                    out->valid = true; out->focusTable = focus;
                    out->level = level; out->entry = index;
                    return true;
                }
                if (candidate->fireInterval < 0) break;
            }
        }
    }
    return false;
}

bool ResolveSpectatorShtReference(Player &player, const SpectatorShtReference &source,
                                  ShtEntry **out)
{
    *out = nullptr;
    if (!source.valid) return true;
    ShtData *data = source.focusTable ? player.shooterDataFocus : player.shooterData;
    if (!data || source.level >= data->numLevels || data->numLevels > 32) return false;
    ShtEntry *entry = data->levels[source.level].entry;
    if (!entry) return false;
    for (std::uint16_t index = 0; index <= source.entry; ++index, ++entry)
        if (entry->fireInterval < 0) return false;
    *out = entry;
    return true;
}

bool CaptureSpectatorAnm(const AnmVm &source, SpectatorAnmState *out)
{
    out->anmFileIndex = source.anmFileIdx;
    out->activeSpriteIndex = source.activeSpriteIdx;
    out->baseSpriteIndex = source.baseSpriteIdx;
    out->flags = static_cast<i32>(source.flags);
    out->autoRotate = source.autoRotate; out->pendingInterrupt = source.pendingInterrupt;
    out->timeOfLastSpriteSet = source.timeOfLastSpriteSet;
    if (source.currentInstruction && source.beginningOfScript)
    {
        const auto current = reinterpret_cast<std::uintptr_t>(source.currentInstruction);
        const auto begin = reinterpret_cast<std::uintptr_t>(source.beginningOfScript);
        if (current < begin || current - begin > 1024u * 1024u) return false;
        out->instructionOffset = static_cast<i32>(current - begin);
    }
    std::copy(std::begin(source.easeModes), std::end(source.easeModes), out->easeModes.begin());
    std::copy(std::begin(source.intVars1), std::end(source.intVars1), out->intVars1.begin());
    std::copy(std::begin(source.floatVars), std::end(source.floatVars), out->floatVars.begin());
    std::copy(std::begin(source.intVars2), std::end(source.intVars2), out->intVars2.begin());
    out->rotation = CaptureSpectatorVec3(source.rotation);
    out->previousRotation = CaptureSpectatorVec3(source.prevRotation);
    out->angleVelocity = CaptureSpectatorVec3(source.angleVel);
    out->position = CaptureSpectatorVec3(source.pos);
    out->previousPosition = CaptureSpectatorVec3(source.prevPos);
    out->positionInterpInitial = CaptureSpectatorVec3(source.posInterpInitial);
    out->positionInterpFinal = CaptureSpectatorVec3(source.posInterpFinal);
    out->rotationInterpInitial = CaptureSpectatorVec3(source.rotateInterpInitial);
    out->rotationInterpFinal = CaptureSpectatorVec3(source.rotateInterpFinal);
    out->offset = CaptureSpectatorVec3(source.offset);
    out->scale = CaptureSpectatorFloat2(source.scale);
    out->previousScale = CaptureSpectatorFloat2(source.prevScale);
    out->scaleGrowth = CaptureSpectatorFloat2(source.scaleGrowth);
    out->uvScrollPosition = CaptureSpectatorFloat2(source.uvScrollPos);
    out->previousUvScrollPosition = CaptureSpectatorFloat2(source.prevUvScrollPos);
    out->uvScrollVelocity = CaptureSpectatorFloat2(source.uvScrollVel);
    out->scaleInterpInitial = CaptureSpectatorFloat2(source.scaleInterpInitial);
    out->scaleInterpFinal = CaptureSpectatorFloat2(source.scaleInterpFinal);
    out->scriptTime = CaptureSpectatorTimer(source.currentTimeInScript);
    out->waitTime = CaptureSpectatorTimer(source.waitTimer);
    for (std::size_t i = 0; i < 5; ++i)
    {
        out->interpStartTimes[i] = CaptureSpectatorTimer(source.interpStartTimes[i]);
        out->interpEndTimes[i] = CaptureSpectatorTimer(source.interpEndTimes[i]);
    }
    out->color = source.color.color; out->color2 = source.color2.color;
    out->previousColor = source.prevColor.color; out->previousColor2 = source.prevColor2.color;
    out->colorInterpInitial = source.colorInterpInitialColor.color;
    out->colorInterpFinal = source.colorInterpFinalColor.color;
    out->fontWidth = source.fontWidth; out->fontHeight = source.fontHeight;
    return true;
}

bool ApplySpectatorAnm(const SpectatorAnmState &source, AnmVm *out)
{
    if (!out || source.anmFileIndex < 0 || source.anmFileIndex >= 4096) return false;
    g_AnmManager->SetAnmIdxAndExecuteScript(out, source.anmFileIndex);
    if (source.activeSpriteIndex >= 0 &&
        g_AnmManager->SetActiveSprite(out, source.activeSpriteIndex) != ZUN_SUCCESS)
        return false;
    out->anmFileIdx = source.anmFileIndex;
    out->activeSpriteIdx = source.activeSpriteIndex;
    out->baseSpriteIdx = source.baseSpriteIndex;
    out->flags = static_cast<u32>(source.flags);
    out->autoRotate = static_cast<i16>(source.autoRotate);
    out->pendingInterrupt = static_cast<i16>(source.pendingInterrupt);
    out->timeOfLastSpriteSet = source.timeOfLastSpriteSet;
    std::copy(source.easeModes.begin(), source.easeModes.end(), std::begin(out->easeModes));
    std::copy(source.intVars1.begin(), source.intVars1.end(), std::begin(out->intVars1));
    std::copy(source.floatVars.begin(), source.floatVars.end(), std::begin(out->floatVars));
    std::copy(source.intVars2.begin(), source.intVars2.end(), std::begin(out->intVars2));
    ApplySpectatorVec3(source.rotation, &out->rotation);
    ApplySpectatorVec3(source.previousRotation, &out->prevRotation);
    ApplySpectatorVec3(source.angleVelocity, &out->angleVel);
    ApplySpectatorVec3(source.position, &out->pos);
    ApplySpectatorVec3(source.previousPosition, &out->prevPos);
    ApplySpectatorVec3(source.positionInterpInitial, &out->posInterpInitial);
    ApplySpectatorVec3(source.positionInterpFinal, &out->posInterpFinal);
    ApplySpectatorVec3(source.rotationInterpInitial, &out->rotateInterpInitial);
    ApplySpectatorVec3(source.rotationInterpFinal, &out->rotateInterpFinal);
    ApplySpectatorVec3(source.offset, &out->offset);
    ApplySpectatorFloat2(source.scale, &out->scale);
    ApplySpectatorFloat2(source.previousScale, &out->prevScale);
    ApplySpectatorFloat2(source.scaleGrowth, &out->scaleGrowth);
    ApplySpectatorFloat2(source.uvScrollPosition, &out->uvScrollPos);
    ApplySpectatorFloat2(source.previousUvScrollPosition, &out->prevUvScrollPos);
    ApplySpectatorFloat2(source.uvScrollVelocity, &out->uvScrollVel);
    ApplySpectatorFloat2(source.scaleInterpInitial, &out->scaleInterpInitial);
    ApplySpectatorFloat2(source.scaleInterpFinal, &out->scaleInterpFinal);
    ApplySpectatorTimer(source.scriptTime, &out->currentTimeInScript);
    ApplySpectatorTimer(source.waitTime, &out->waitTimer);
    for (std::size_t i = 0; i < 5; ++i)
    {
        ApplySpectatorTimer(source.interpStartTimes[i], &out->interpStartTimes[i]);
        ApplySpectatorTimer(source.interpEndTimes[i], &out->interpEndTimes[i]);
    }
    out->color.color = source.color; out->color2.color = source.color2;
    out->prevColor.color = source.previousColor; out->prevColor2.color = source.previousColor2;
    out->colorInterpInitialColor.color = source.colorInterpInitial;
    out->colorInterpFinalColor.color = source.colorInterpFinal;
    out->fontWidth = source.fontWidth; out->fontHeight = source.fontHeight;
    if (source.instructionOffset < -1 || source.instructionOffset > 1024 * 1024) return false;
    out->currentInstruction = source.instructionOffset < 0 ? nullptr :
        reinterpret_cast<AnmRawInstr *>(reinterpret_cast<u8 *>(out->beginningOfScript) + source.instructionOffset);
    return true;
}

bool WriteSpectatorShtReference(SpectatorScalars::Writer &writer,
                                const SpectatorShtReference &value)
{
    return writer.Values(value.valid, value.focusTable, value.level, value.entry);
}

bool ReadSpectatorShtReference(SpectatorScalars::Reader &reader,
                               SpectatorShtReference *value)
{
    return reader.Values(value->valid, value->focusTable, value->level, value->entry);
}

bool WriteSpectatorAnm(SpectatorScalars::Writer &writer, const SpectatorAnmState &value)
{
    if (!writer.Values(value.anmFileIndex, value.activeSpriteIndex, value.baseSpriteIndex,
                       value.instructionOffset, value.flags, value.autoRotate,
                       value.pendingInterrupt, value.timeOfLastSpriteSet)) return false;
    if (!writer.Array(value.easeModes) || !writer.Array(value.intVars1) ||
        !writer.Array(value.floatVars) || !writer.Array(value.intVars2)) return false;
    for (const auto *v : {&value.rotation, &value.previousRotation, &value.angleVelocity,
                          &value.position, &value.previousPosition, &value.positionInterpInitial,
                          &value.positionInterpFinal, &value.rotationInterpInitial,
                          &value.rotationInterpFinal, &value.offset})
        if (!WriteSpectatorVec3(writer, *v)) return false;
    for (const auto *v : {&value.scale, &value.previousScale, &value.scaleGrowth,
                          &value.uvScrollPosition, &value.previousUvScrollPosition,
                          &value.uvScrollVelocity, &value.scaleInterpInitial, &value.scaleInterpFinal})
        if (!WriteSpectatorFloat2(writer, *v)) return false;
    if (!WriteSpectatorTimer(writer, value.scriptTime) || !WriteSpectatorTimer(writer, value.waitTime) ||
        !WriteSpectatorTimerArray(writer, value.interpStartTimes) ||
        !WriteSpectatorTimerArray(writer, value.interpEndTimes)) return false;
    return writer.Values(value.color, value.color2, value.previousColor, value.previousColor2,
                         value.colorInterpInitial, value.colorInterpFinal,
                         value.fontWidth, value.fontHeight);
}

bool ReadSpectatorAnm(SpectatorScalars::Reader &reader, SpectatorAnmState *value)
{
    if (!reader.Values(value->anmFileIndex, value->activeSpriteIndex, value->baseSpriteIndex,
                       value->instructionOffset, value->flags, value->autoRotate,
                       value->pendingInterrupt, value->timeOfLastSpriteSet)) return false;
    if (!reader.Array(value->easeModes) || !reader.Array(value->intVars1) ||
        !reader.Array(value->floatVars) || !reader.Array(value->intVars2)) return false;
    for (auto *v : {&value->rotation, &value->previousRotation, &value->angleVelocity,
                    &value->position, &value->previousPosition, &value->positionInterpInitial,
                    &value->positionInterpFinal, &value->rotationInterpInitial,
                    &value->rotationInterpFinal, &value->offset})
        if (!ReadSpectatorVec3(reader, v)) return false;
    for (auto *v : {&value->scale, &value->previousScale, &value->scaleGrowth,
                    &value->uvScrollPosition, &value->previousUvScrollPosition,
                    &value->uvScrollVelocity, &value->scaleInterpInitial, &value->scaleInterpFinal})
        if (!ReadSpectatorFloat2(reader, v)) return false;
    if (!ReadSpectatorTimer(reader, &value->scriptTime) || !ReadSpectatorTimer(reader, &value->waitTime) ||
        !ReadSpectatorTimerArray(reader, &value->interpStartTimes) ||
        !ReadSpectatorTimerArray(reader, &value->interpEndTimes)) return false;
    return reader.Values(value->color, value->color2, value->previousColor, value->previousColor2,
                         value->colorInterpInitial, value->colorInterpFinal,
                         value->fontWidth, value->fontHeight);
}

bool WriteSpectatorPlayerBullet(SpectatorScalars::Writer &writer,
                                const SpectatorPlayerBulletState &value)
{
    if (!writer.Values(value.player, value.slot) || !WriteSpectatorShtReference(writer, value.shtEntry) ||
        !WriteSpectatorVec3(writer, value.position) || !WriteSpectatorVec3(writer, value.previousPosition) ||
        !WriteSpectatorVec3Array(writer, value.positionHistory) ||
        !WriteSpectatorVec3(writer, value.hitboxSize) ||
        !WriteSpectatorFloat2(writer, value.velocity) || !WriteSpectatorFloat2(writer, value.offset) ||
        !writer.Values(value.speed, value.angle, value.previousAngle) ||
        !WriteSpectatorTimer(writer, value.timer) ||
        !writer.Values(value.damage, value.bulletState, value.bulletState2,
                       value.timerIndex, value.optionId, value.trailLength, value.hasVm)) return false;
    return !value.hasVm || WriteSpectatorAnm(writer, value.vm);
}

bool ReadSpectatorPlayerBullet(SpectatorScalars::Reader &reader, SpectatorPlayerBulletState *value)
{
    if (!reader.Values(value->player, value->slot) || !ReadSpectatorShtReference(reader, &value->shtEntry) ||
        !ReadSpectatorVec3(reader, &value->position) || !ReadSpectatorVec3(reader, &value->previousPosition) ||
        !ReadSpectatorVec3Array(reader, &value->positionHistory) ||
        !ReadSpectatorVec3(reader, &value->hitboxSize) ||
        !ReadSpectatorFloat2(reader, &value->velocity) || !ReadSpectatorFloat2(reader, &value->offset) ||
        !reader.Values(value->speed, value->angle, value->previousAngle) ||
        !ReadSpectatorTimer(reader, &value->timer) ||
        !reader.Values(value->damage, value->bulletState, value->bulletState2,
                       value->timerIndex, value->optionId, value->trailLength, value->hasVm)) return false;
    return !value->hasVm || ReadSpectatorAnm(reader, &value->vm);
}

bool WriteSpectatorBombSub(SpectatorScalars::Writer &writer, const SpectatorBombSubState &value)
{
    if (!writer.Values(value.state, value.counter, value.custom, value.previousCustom,
                       value.speed, value.angle) ||
        !WriteSpectatorVec3(writer, value.position) || !WriteSpectatorVec3(writer, value.previousPosition) ||
        !WriteSpectatorVec3Array(writer, value.positionHistory) ||
        !WriteSpectatorVec3(writer, value.velocity) || !WriteSpectatorVec3(writer, value.acceleration) ||
        !WriteSpectatorTimer(writer, value.timer)) return false;
    for (std::size_t i = 0; i < value.vms.size(); ++i)
    {
        if (!writer.Value(value.hasVm[i]) || (value.hasVm[i] && !WriteSpectatorAnm(writer, value.vms[i])))
            return false;
    }
    return true;
}

bool ReadSpectatorBombSub(SpectatorScalars::Reader &reader, SpectatorBombSubState *value)
{
    if (!reader.Values(value->state, value->counter, value->custom, value->previousCustom,
                       value->speed, value->angle) ||
        !ReadSpectatorVec3(reader, &value->position) || !ReadSpectatorVec3(reader, &value->previousPosition) ||
        !ReadSpectatorVec3Array(reader, &value->positionHistory) ||
        !ReadSpectatorVec3(reader, &value->velocity) || !ReadSpectatorVec3(reader, &value->acceleration) ||
        !ReadSpectatorTimer(reader, &value->timer)) return false;
    for (std::size_t i = 0; i < value->vms.size(); ++i)
        if (!reader.Value(value->hasVm[i]) || (value->hasVm[i] && !ReadSpectatorAnm(reader, &value->vms[i])))
            return false;
    return true;
}

bool WriteSpectatorPlayer(SpectatorScalars::Writer &writer, const SpectatorPlayerState &p)
{
    for (const auto *v : {&p.position, &p.previousPosition, &p.hitboxTopLeft, &p.hitboxBottomRight,
                          &p.grazeTopLeft, &p.grazeBottomRight, &p.grabTopLeft, &p.grabBottomRight,
                          &p.hitboxSize, &p.grazeSize, &p.grabSize})
        if (!WriteSpectatorVec3(writer, *v)) return false;
    if (!WriteSpectatorVec3Array(writer, p.optionsPosition) ||
        !WriteSpectatorVec3Array(writer, p.previousOptionsPosition) ||
        !WriteSpectatorFloat2(writer, p.velocity) || !writer.Value(p.unused9d4)) return false;
    for (const auto &box : p.bombDamageBoxes)
        if (!WriteSpectatorVec3(writer, box.position) || !WriteSpectatorVec3(writer, box.size) ||
            !writer.Values(box.lifetime, box.damage)) return false;
    for (const auto &box : p.bombClearBoxes)
        if (!WriteSpectatorFloat2(writer, box.position) || !WriteSpectatorFloat2(writer, box.size) ||
            !writer.Values(box.radius, box.radiusGrowth, box.lifetime, box.itemType)) return false;
    if (!writer.Values(p.numActiveBombClearBoxes, p.dirtyBombBoxes, p.isBombing,
                       p.horizontalSpeedDuringBomb, p.verticalSpeedDuringBomb, p.respawnTimer,
                       p.borderInvulnerabilityTime, p.bulletGracePeriod, p.itemType, p.playerState,
                       p.initParam, p.optionState, p.isFocus, p.bombParticleTime, p.hasBorder) ||
        !WriteSpectatorTimer(writer, p.focusMovementTimer) ||
        !writer.Values(p.playerDirection, p.previousHorizontalSpeed, p.previousVerticalSpeed) ||
        !WriteSpectatorVec3(writer, p.positionOfLastEnemyHit) ||
        !WriteSpectatorVec3(writer, p.sakuyaTargetPosition) || !writer.Value(p.targetingEnemy)) return false;
    for (const auto &entry : p.shtEntries) if (!WriteSpectatorShtReference(writer, entry)) return false;
    for (const auto &timer : p.timers)
        if (!WriteSpectatorTimer(writer, timer.timer) || !writer.Value(timer.bulletSlot)) return false;
    if (!WriteSpectatorTimer(writer, p.fireBulletTimer) || !WriteSpectatorTimer(writer, p.invulnerabilityTimer) ||
        !WriteSpectatorTimer(writer, p.borderTimer) || !WriteSpectatorTimer(writer, p.teamBombProtectionTimer) ||
        !writer.Values(p.lifeGiveTimer, p.lifeGiveTargetToken, p.unused16a18, p.unused16a1c,
                       p.bombIsInUse, p.bombIsFocus,
                       p.bombDuration, p.bombCherryDrain) || !WriteSpectatorTimer(writer, p.bombTimer)) return false;
    if (p.bombIsInUse)
        for (const auto &sub : p.bombSubInfo) if (!WriteSpectatorBombSub(writer, sub)) return false;
    return WriteSpectatorVec3(writer, p.bombStartPosition) && writer.Value(p.optionAngle);
}

bool ReadSpectatorPlayer(SpectatorScalars::Reader &reader, SpectatorPlayerState *p)
{
    for (auto *v : {&p->position, &p->previousPosition, &p->hitboxTopLeft, &p->hitboxBottomRight,
                    &p->grazeTopLeft, &p->grazeBottomRight, &p->grabTopLeft, &p->grabBottomRight,
                    &p->hitboxSize, &p->grazeSize, &p->grabSize})
        if (!ReadSpectatorVec3(reader, v)) return false;
    if (!ReadSpectatorVec3Array(reader, &p->optionsPosition) ||
        !ReadSpectatorVec3Array(reader, &p->previousOptionsPosition) ||
        !ReadSpectatorFloat2(reader, &p->velocity) || !reader.Value(p->unused9d4)) return false;
    for (auto &box : p->bombDamageBoxes)
        if (!ReadSpectatorVec3(reader, &box.position) || !ReadSpectatorVec3(reader, &box.size) ||
            !reader.Values(box.lifetime, box.damage)) return false;
    for (auto &box : p->bombClearBoxes)
        if (!ReadSpectatorFloat2(reader, &box.position) || !ReadSpectatorFloat2(reader, &box.size) ||
            !reader.Values(box.radius, box.radiusGrowth, box.lifetime, box.itemType)) return false;
    if (!reader.Values(p->numActiveBombClearBoxes, p->dirtyBombBoxes, p->isBombing,
                       p->horizontalSpeedDuringBomb, p->verticalSpeedDuringBomb, p->respawnTimer,
                       p->borderInvulnerabilityTime, p->bulletGracePeriod, p->itemType, p->playerState,
                       p->initParam, p->optionState, p->isFocus, p->bombParticleTime, p->hasBorder) ||
        !ReadSpectatorTimer(reader, &p->focusMovementTimer) ||
        !reader.Values(p->playerDirection, p->previousHorizontalSpeed, p->previousVerticalSpeed) ||
        !ReadSpectatorVec3(reader, &p->positionOfLastEnemyHit) ||
        !ReadSpectatorVec3(reader, &p->sakuyaTargetPosition) || !reader.Value(p->targetingEnemy)) return false;
    for (auto &entry : p->shtEntries) if (!ReadSpectatorShtReference(reader, &entry)) return false;
    for (auto &timer : p->timers)
        if (!ReadSpectatorTimer(reader, &timer.timer) || !reader.Value(timer.bulletSlot)) return false;
    if (!ReadSpectatorTimer(reader, &p->fireBulletTimer) || !ReadSpectatorTimer(reader, &p->invulnerabilityTimer) ||
        !ReadSpectatorTimer(reader, &p->borderTimer) || !ReadSpectatorTimer(reader, &p->teamBombProtectionTimer) ||
        !reader.Values(p->lifeGiveTimer, p->lifeGiveTargetToken, p->unused16a18, p->unused16a1c,
                       p->bombIsInUse, p->bombIsFocus,
                       p->bombDuration, p->bombCherryDrain) || !ReadSpectatorTimer(reader, &p->bombTimer)) return false;
    if (p->bombIsInUse)
        for (auto &sub : p->bombSubInfo) if (!ReadSpectatorBombSub(reader, &sub)) return false;
    return ReadSpectatorVec3(reader, &p->bombStartPosition) && reader.Value(p->optionAngle);
}

bool CaptureSpectatorPlayerState(Player &source, std::uint8_t playerId,
                                 SpectatorPlayerState *out,
                                 std::vector<SpectatorPlayerBulletState> *bullets)
{
    out->position = CaptureSpectatorVec3(source.pos);
    out->previousPosition = CaptureSpectatorVec3(source.prevPos);
    out->hitboxTopLeft = CaptureSpectatorVec3(source.hitboxTopLeft);
    out->hitboxBottomRight = CaptureSpectatorVec3(source.hitboxBottomRight);
    out->grazeTopLeft = CaptureSpectatorVec3(source.grazeTopLeft);
    out->grazeBottomRight = CaptureSpectatorVec3(source.grazeBottomRight);
    out->grabTopLeft = CaptureSpectatorVec3(source.grabItemTopLeft);
    out->grabBottomRight = CaptureSpectatorVec3(source.grabItemBottomRight);
    out->hitboxSize = CaptureSpectatorVec3(source.hitboxSize);
    out->grazeSize = CaptureSpectatorVec3(source.grazeSize);
    out->grabSize = CaptureSpectatorVec3(source.grabItemSize);
    for (std::size_t i = 0; i < 2; ++i)
    {
        out->optionsPosition[i] = CaptureSpectatorVec3(source.optionsPosition[i]);
        out->previousOptionsPosition[i] = CaptureSpectatorVec3(source.prevOptionsPosition[i]);
    }
    out->velocity = CaptureSpectatorFloat2(source.velocity);
    out->unused9d4 = source.unused_9d4;
    for (std::size_t i = 0; i < out->bombDamageBoxes.size(); ++i)
    {
        const auto &box = source.bombDamageBoxes[i];
        out->bombDamageBoxes[i].position = CaptureSpectatorVec3(box.pos);
        out->bombDamageBoxes[i].size = CaptureSpectatorVec3(box.size);
        out->bombDamageBoxes[i].lifetime = box.lifetime;
        out->bombDamageBoxes[i].damage = box.damage;
    }
    for (std::size_t i = 0; i < out->bombClearBoxes.size(); ++i)
    {
        const auto &box = source.bombClearBoxes[i];
        out->bombClearBoxes[i].position = CaptureSpectatorFloat2(box.pos);
        out->bombClearBoxes[i].size = CaptureSpectatorFloat2(box.size);
        out->bombClearBoxes[i].radius = box.radius;
        out->bombClearBoxes[i].radiusGrowth = box.radiusGrowth;
        out->bombClearBoxes[i].lifetime = box.lifetime;
        out->bombClearBoxes[i].itemType = box.itemType;
    }
    out->numActiveBombClearBoxes = source.numActiveBombClearBoxes;
    out->dirtyBombBoxes = source.dirtyBombBoxes;
    out->isBombing = source.isBombing;
    out->horizontalSpeedDuringBomb = source.horizontalMovementSpeedMultiplierDuringBomb;
    out->verticalSpeedDuringBomb = source.verticalMovementSpeedMultiplierDuringBomb;
    out->respawnTimer = source.respawnTimer; out->borderInvulnerabilityTime = source.borderInvulnerabilityTime;
    out->bulletGracePeriod = source.bulletGracePeriod; out->itemType = source.itemType;
    out->playerState = source.playerState; out->initParam = source.initParam;
    out->optionState = source.optionState; out->isFocus = source.isFocus;
    out->bombParticleTime = source.bombParticleTime; out->hasBorder = source.hasBorder;
    out->focusMovementTimer = CaptureSpectatorTimer(source.focusMovementTimer);
    out->playerDirection = source.playerDirection;
    out->previousHorizontalSpeed = source.previousHorizontalSpeed;
    out->previousVerticalSpeed = source.previousVerticalSpeed;
    out->positionOfLastEnemyHit = CaptureSpectatorVec3(source.positionOfLastEnemyHit);
    out->sakuyaTargetPosition = CaptureSpectatorVec3(source.sakuyaTargetPosition);
    out->targetingEnemy = source.targetingEnemy;
    for (std::size_t i = 0; i < out->shtEntries.size(); ++i)
        if (!CaptureSpectatorShtReference(source, source.shtEntries[i], &out->shtEntries[i])) return false;
    for (std::size_t i = 0; i < out->timers.size(); ++i)
    {
        out->timers[i].timer = CaptureSpectatorTimer(source.timers[i].timer);
        out->timers[i].bulletSlot = -1;
        if (source.timers[i].bullet)
        {
            const auto base = reinterpret_cast<std::uintptr_t>(&source.bullets[0]);
            const auto pointer = reinterpret_cast<std::uintptr_t>(source.timers[i].bullet);
            const auto end = base + sizeof(source.bullets);
            if (pointer < base || pointer >= end || (pointer - base) % sizeof(PlayerBullet) != 0) return false;
            out->timers[i].bulletSlot = static_cast<i16>((pointer - base) / sizeof(PlayerBullet));
        }
    }
    out->fireBulletTimer = CaptureSpectatorTimer(source.fireBulletTimer);
    out->invulnerabilityTimer = CaptureSpectatorTimer(source.invulnerabilityTimer);
    out->borderTimer = CaptureSpectatorTimer(source.borderTimer);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    out->teamBombProtectionTimer = CaptureSpectatorTimer(source.teamBombProtectionTimer);
#endif
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    out->lifeGiveTimer = source.lifeGiveTimer; out->lifeGiveTargetToken = source.lifeGiveTargetToken;
#else
    out->unused16a18 = source.unused_16a18; out->unused16a1c = source.unused_16a1c;
#endif
    out->bombIsInUse = source.bombInfo.isInUse; out->bombIsFocus = source.bombInfo.isFocus;
    out->bombDuration = source.bombInfo.bombDuration; out->bombCherryDrain = source.bombInfo.cherryDrain;
    out->bombTimer = CaptureSpectatorTimer(source.bombInfo.bombTimer);
    if (out->bombIsInUse)
    {
        for (std::size_t i = 0; i < out->bombSubInfo.size(); ++i)
        {
            const auto &sub = source.bombInfo.subInfo[i];
            auto &saved = out->bombSubInfo[i];
            saved.state = sub.state; saved.counter = sub.counter;
            saved.custom = sub.custom; saved.previousCustom = sub.prevCustom;
            saved.speed = sub.speed; saved.angle = sub.angle;
            saved.position = CaptureSpectatorVec3(sub.pos);
            saved.previousPosition = CaptureSpectatorVec3(sub.prevPos);
            for (std::size_t j = 0; j < saved.positionHistory.size(); ++j)
                saved.positionHistory[j] = CaptureSpectatorVec3(sub.posHistory[j]);
            saved.velocity = CaptureSpectatorVec3(sub.velocity);
            saved.acceleration = CaptureSpectatorVec3(sub.accel);
            saved.timer = CaptureSpectatorTimer(sub.timer);
            for (std::size_t vm = 0; vm < saved.vms.size(); ++vm)
            {
                if (sub.vms[vm].beginningOfScript && sub.vms[vm].anmFileIdx >= 0)
                {
                    saved.hasVm[vm] = true;
                    if (!CaptureSpectatorAnm(sub.vms[vm], &saved.vms[vm])) return false;
                }
            }
        }
    }
    out->bombStartPosition = CaptureSpectatorVec3(source.bombStartPos);
    out->optionAngle = source.optionAngle;

    if (!bullets) return true;
    for (std::uint8_t slot = 0; slot < std::size(source.bullets); ++slot)
    {
        const auto &bullet = source.bullets[slot];
        SpectatorPlayerBulletState saved{};
        saved.player = playerId; saved.slot = slot;
        saved.position = CaptureSpectatorVec3(bullet.pos);
        saved.previousPosition = CaptureSpectatorVec3(bullet.prevPos);
        saved.hitboxSize = CaptureSpectatorVec3(bullet.hitboxSize);
        for (std::size_t i = 0; i < saved.positionHistory.size(); ++i)
            saved.positionHistory[i] = CaptureSpectatorVec3(bullet.posHistory[i]);
        saved.velocity = CaptureSpectatorFloat2(bullet.velocity);
        saved.offset = CaptureSpectatorFloat2(bullet.offset);
        saved.speed = bullet.speed; saved.angle = bullet.angle; saved.previousAngle = bullet.prevAngle;
        saved.timer = CaptureSpectatorTimer(bullet.timer);
        saved.damage = bullet.damage; saved.bulletState = bullet.bulletState;
        saved.bulletState2 = bullet.bulletState2; saved.timerIndex = bullet.timerIdx;
        saved.optionId = bullet.optionId; saved.trailLength = bullet.trailLength;
        if (!CaptureSpectatorShtReference(source, bullet.shtEntry, &saved.shtEntry)) return false;
        saved.hasVm = bullet.bulletState != 0;
        if (saved.hasVm && !CaptureSpectatorAnm(bullet.vm, &saved.vm)) return false;
        bool used = bullet.bulletState != 0 || saved.shtEntry.valid || bullet.damage != 0 ||
                    bullet.bulletState2 != 0 || bullet.timerIdx != 0 || bullet.optionId != 0 ||
                    bullet.trailLength != 0 || bullet.speed != 0 || bullet.angle != 0 || bullet.prevAngle != 0 ||
                    bullet.timer.previous != 0 || bullet.timer.current != 0 || bullet.timer.subFrame != 0 ||
                    saved.position.x != 0 || saved.position.y != 0 || saved.position.z != 0 ||
                    saved.previousPosition.x != 0 || saved.previousPosition.y != 0 || saved.previousPosition.z != 0 ||
                    saved.hitboxSize.x != 0 || saved.hitboxSize.y != 0 || saved.hitboxSize.z != 0 ||
                    saved.velocity.x != 0 || saved.velocity.y != 0 || saved.offset.x != 0 || saved.offset.y != 0;
        for (const auto &position : saved.positionHistory)
            used = used || position.x != 0 || position.y != 0 || position.z != 0;
        if (used) bullets->push_back(std::move(saved));
    }
    return true;
}

bool ApplySpectatorPlayerState(Player &target, const SpectatorPlayerState &source,
                               const std::vector<SpectatorPlayerBulletState> &bullets,
                               std::uint8_t playerId)
{
    ApplySpectatorVec3(source.position, &target.pos);
    ApplySpectatorVec3(source.previousPosition, &target.prevPos);
    ApplySpectatorVec3(source.hitboxTopLeft, &target.hitboxTopLeft);
    ApplySpectatorVec3(source.hitboxBottomRight, &target.hitboxBottomRight);
    ApplySpectatorVec3(source.grazeTopLeft, &target.grazeTopLeft);
    ApplySpectatorVec3(source.grazeBottomRight, &target.grazeBottomRight);
    ApplySpectatorVec3(source.grabTopLeft, &target.grabItemTopLeft);
    ApplySpectatorVec3(source.grabBottomRight, &target.grabItemBottomRight);
    ApplySpectatorVec3(source.hitboxSize, &target.hitboxSize);
    ApplySpectatorVec3(source.grazeSize, &target.grazeSize);
    ApplySpectatorVec3(source.grabSize, &target.grabItemSize);
    for (std::size_t i = 0; i < 2; ++i)
    {
        ApplySpectatorVec3(source.optionsPosition[i], &target.optionsPosition[i]);
        ApplySpectatorVec3(source.previousOptionsPosition[i], &target.prevOptionsPosition[i]);
    }
    ApplySpectatorFloat2(source.velocity, &target.velocity);
    target.unused_9d4 = source.unused9d4;
    for (std::size_t i = 0; i < source.bombDamageBoxes.size(); ++i)
    {
        ApplySpectatorVec3(source.bombDamageBoxes[i].position, &target.bombDamageBoxes[i].pos);
        ApplySpectatorVec3(source.bombDamageBoxes[i].size, &target.bombDamageBoxes[i].size);
        target.bombDamageBoxes[i].lifetime = source.bombDamageBoxes[i].lifetime;
        target.bombDamageBoxes[i].damage = source.bombDamageBoxes[i].damage;
    }
    for (std::size_t i = 0; i < source.bombClearBoxes.size(); ++i)
    {
        ApplySpectatorFloat2(source.bombClearBoxes[i].position, &target.bombClearBoxes[i].pos);
        ApplySpectatorFloat2(source.bombClearBoxes[i].size, &target.bombClearBoxes[i].size);
        target.bombClearBoxes[i].radius = source.bombClearBoxes[i].radius;
        target.bombClearBoxes[i].radiusGrowth = source.bombClearBoxes[i].radiusGrowth;
        target.bombClearBoxes[i].lifetime = source.bombClearBoxes[i].lifetime;
        target.bombClearBoxes[i].itemType = source.bombClearBoxes[i].itemType;
    }
    target.numActiveBombClearBoxes = source.numActiveBombClearBoxes;
    target.dirtyBombBoxes = source.dirtyBombBoxes;
    target.isBombing = source.isBombing;
    target.horizontalMovementSpeedMultiplierDuringBomb = source.horizontalSpeedDuringBomb;
    target.verticalMovementSpeedMultiplierDuringBomb = source.verticalSpeedDuringBomb;
    target.respawnTimer = source.respawnTimer; target.borderInvulnerabilityTime = source.borderInvulnerabilityTime;
    target.bulletGracePeriod = source.bulletGracePeriod; target.itemType = source.itemType;
    target.playerState = source.playerState; target.initParam = source.initParam;
    target.optionState = source.optionState; target.isFocus = source.isFocus;
    target.bombParticleTime = source.bombParticleTime; target.hasBorder = source.hasBorder;
    ApplySpectatorTimer(source.focusMovementTimer, &target.focusMovementTimer);
    target.playerDirection = static_cast<PlayerDirection>(source.playerDirection);
    target.previousHorizontalSpeed = source.previousHorizontalSpeed;
    target.previousVerticalSpeed = source.previousVerticalSpeed;
    ApplySpectatorVec3(source.positionOfLastEnemyHit, &target.positionOfLastEnemyHit);
    ApplySpectatorVec3(source.sakuyaTargetPosition, &target.sakuyaTargetPosition);
    target.targetingEnemy = source.targetingEnemy;
    for (std::size_t i = 0; i < source.shtEntries.size(); ++i)
        if (!ResolveSpectatorShtReference(target, source.shtEntries[i], &target.shtEntries[i])) return false;
    for (std::size_t i = 0; i < source.timers.size(); ++i)
    {
        ApplySpectatorTimer(source.timers[i].timer, &target.timers[i].timer);
        if (source.timers[i].bulletSlot < -1 || source.timers[i].bulletSlot >= 96) return false;
        target.timers[i].bullet = source.timers[i].bulletSlot < 0 ? nullptr :
            &target.bullets[source.timers[i].bulletSlot];
    }
    ApplySpectatorTimer(source.fireBulletTimer, &target.fireBulletTimer);
    ApplySpectatorTimer(source.invulnerabilityTimer, &target.invulnerabilityTimer);
    ApplySpectatorTimer(source.borderTimer, &target.borderTimer);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    ApplySpectatorTimer(source.teamBombProtectionTimer, &target.teamBombProtectionTimer);
#endif
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    target.lifeGiveTimer = source.lifeGiveTimer; target.lifeGiveTargetToken = source.lifeGiveTargetToken;
#else
    target.unused_16a18 = source.unused16a18; target.unused_16a1c = source.unused16a1c;
#endif
    target.bombInfo.isInUse = source.bombIsInUse; target.bombInfo.isFocus = source.bombIsFocus;
    target.bombInfo.bombDuration = source.bombDuration; target.bombInfo.cherryDrain = source.bombCherryDrain;
    ApplySpectatorTimer(source.bombTimer, &target.bombInfo.bombTimer);
    if (source.bombIsInUse)
    {
        for (std::size_t i = 0; i < source.bombSubInfo.size(); ++i)
        {
            auto &dest = target.bombInfo.subInfo[i];
            const auto &saved = source.bombSubInfo[i];
            dest.state = saved.state; dest.counter = saved.counter;
            dest.custom = saved.custom; dest.prevCustom = saved.previousCustom;
            dest.speed = saved.speed; dest.angle = saved.angle;
            ApplySpectatorVec3(saved.position, &dest.pos);
            ApplySpectatorVec3(saved.previousPosition, &dest.prevPos);
            for (std::size_t j = 0; j < saved.positionHistory.size(); ++j)
                ApplySpectatorVec3(saved.positionHistory[j], &dest.posHistory[j]);
            ApplySpectatorVec3(saved.velocity, &dest.velocity);
            ApplySpectatorVec3(saved.acceleration, &dest.accel);
            ApplySpectatorTimer(saved.timer, &dest.timer);
            for (std::size_t vm = 0; vm < saved.vms.size(); ++vm)
                if (saved.hasVm[vm] && !ApplySpectatorAnm(saved.vms[vm], &dest.vms[vm])) return false;
        }
    }
    ApplySpectatorVec3(source.bombStartPosition, &target.bombStartPos);
    target.optionAngle = source.optionAngle;
    target.RebuildBombBoxCache();
    target.numActiveBombClearBoxes = source.numActiveBombClearBoxes;
    target.dirtyBombBoxes = source.dirtyBombBoxes;

    for (const auto &saved : bullets)
    {
        if (saved.player != playerId) continue;
        if (saved.slot >= std::size(target.bullets)) return false;
        PlayerBullet &bullet = target.bullets[saved.slot];
        ApplySpectatorVec3(saved.position, &bullet.pos);
        ApplySpectatorVec3(saved.previousPosition, &bullet.prevPos);
        ApplySpectatorVec3(saved.hitboxSize, &bullet.hitboxSize);
        for (std::size_t i = 0; i < saved.positionHistory.size(); ++i)
            ApplySpectatorVec3(saved.positionHistory[i], &bullet.posHistory[i]);
        ApplySpectatorFloat2(saved.velocity, &bullet.velocity);
        ApplySpectatorFloat2(saved.offset, &bullet.offset);
        bullet.speed = saved.speed; bullet.angle = saved.angle; bullet.prevAngle = saved.previousAngle;
        ApplySpectatorTimer(saved.timer, &bullet.timer);
        bullet.damage = saved.damage; bullet.bulletState = saved.bulletState;
        bullet.bulletState2 = saved.bulletState2; bullet.timerIdx = saved.timerIndex;
        bullet.optionId = saved.optionId; bullet.trailLength = saved.trailLength;
        ShtEntry *entry = nullptr;
        if (!ResolveSpectatorShtReference(target, saved.shtEntry, &entry)) return false;
        bullet.shtEntry = entry;
        bullet.updateCallback = entry ? entry->updateCallback : nullptr;
        bullet.drawCallback = entry ? entry->drawCallback : nullptr;
        bullet.hitCallback = entry ? entry->hitCallback : nullptr;
        if (bullet.bulletState != 0 && (!entry || !saved.hasVm || !ApplySpectatorAnm(saved.vm, &bullet.vm)))
            return false;
    }
    return true;
}

void LogSpectatorCheckpointCanonical(const char *phase, std::uint32_t frame,
                                    const Th07CanonicalHash::Sample &sample)
{
    std::printf(
        "netplay spectator TH07: canonical %s frame=%u composite=%016llx meta=%016llx rng=%016llx game=%016llx input=%016llx supervisor=%016llx multiplayer=%016llx stage=%016llx players=%016llx/%016llx/%016llx enemies=%016llx bullets=%016llx items=%016llx counts=%u/%u/%u/%u\n",
        phase, frame, static_cast<unsigned long long>(sample.composite),
        static_cast<unsigned long long>(sample.meta),
        static_cast<unsigned long long>(sample.metaRng),
        static_cast<unsigned long long>(sample.metaGame),
        static_cast<unsigned long long>(sample.metaInput),
        static_cast<unsigned long long>(sample.metaSupervisor),
        static_cast<unsigned long long>(sample.multiplayer),
        static_cast<unsigned long long>(sample.stage),
        static_cast<unsigned long long>(sample.player0),
        static_cast<unsigned long long>(sample.player1),
        static_cast<unsigned long long>(sample.player2),
        static_cast<unsigned long long>(sample.enemies),
        static_cast<unsigned long long>(sample.bullets),
        static_cast<unsigned long long>(sample.items), sample.enemyCount,
        sample.bulletCount, sample.laserCount, sample.itemCount);
    std::printf(
        "netplay spectator TH07: stage fields %s frame=%u scriptTime=%d/%d/%.6f instruction=%d stageFrame=%d stage=%u pos=%.3f,%.3f,%.3f prev=%.3f,%.3f,%.3f color=%08x skyFog=%.3f/%.3f fogEnd=%.3f/%.3f fogStart=%.3f/%.3f skyInterp=%d/%d/%d/%.6f spell=%d/%d clear=%d wait=%d\n",
        phase, frame, g_Stage.scriptTime.previous, g_Stage.scriptTime.current,
        g_Stage.scriptTime.subFrame, g_Stage.instructionIndex, g_Stage.stageFrameCounter,
        static_cast<unsigned>(g_Stage.stage), g_Stage.pos.x, g_Stage.pos.y, g_Stage.pos.z,
        g_Stage.prevPos.x, g_Stage.prevPos.y, g_Stage.prevPos.z,
        static_cast<unsigned>(g_Stage.color), g_Stage.skyFog.nearPlane,
        g_Stage.skyFog.farPlane, g_Stage.fogEnd.nearPlane, g_Stage.fogEnd.farPlane,
        g_Stage.fogStart.nearPlane, g_Stage.fogStart.farPlane,
        g_Stage.skyFogInterpDuration, g_Stage.skyFogInterpTimer.previous,
        g_Stage.skyFogInterpTimer.current, g_Stage.skyFogInterpTimer.subFrame,
        g_Stage.spellCardState, g_Stage.ticksSinceSpellcardStarted,
        static_cast<int>(g_Stage.clearBackground), g_Stage.scriptWaitTime);
    std::printf("netplay spectator TH07: stage arrays %s frame=%u timersMax=", phase, frame);
    for (i32 i = 0; i < ARRAY_SIZE_SIGNED(g_Stage.timersMax); ++i)
        std::printf("%s%d", i ? "," : "", g_Stage.timersMax[i]);
    std::printf(" timers=");
    for (i32 i = 0; i < ARRAY_SIZE_SIGNED(g_Stage.timers); ++i)
        std::printf("%s%d/%d/%.6f", i ? "," : "", g_Stage.timers[i].previous,
                    g_Stage.timers[i].current, g_Stage.timers[i].subFrame);
    std::printf(" ease=");
    for (i32 i = 0; i < ARRAY_SIZE_SIGNED(g_Stage.easeModes); ++i)
        std::printf("%s%d", i ? "," : "", g_Stage.easeModes[i]);
    std::printf(" interp=");
    std::printf("%.3f,%.3f,%.3f/%d/%.3f,%.3f,%.3f/%d/%d/%u/%d\n",
                g_Stage.positionStart.x, g_Stage.positionStart.y, g_Stage.positionStart.z,
                g_Stage.positionInterpEndTime, g_Stage.positionInterpInitial.x,
                g_Stage.positionInterpInitial.y, g_Stage.positionInterpInitial.z,
                g_Stage.positionInterpStartTime, static_cast<int>(g_Stage.cameraTeleported),
                static_cast<unsigned>(g_Stage.isDarkening),
                static_cast<int>(g_Stage.color2.color));
    for (i32 i = 0; i < ARRAY_SIZE_SIGNED(g_EnemyManager.timelines); ++i)
    {
        const EclTimeline &timeline = g_EnemyManager.timelines[i];
        const EclTimelineInstr *instr = timeline.timelineInstr;
        std::printf(
            "netplay spectator TH07: timeline %s frame=%u index=%d timer=%d/%d/%.6f instr=%d/%d/%d/%d args=%08x,%08x,%08x,%08x,%08x,%08x\n",
            phase, frame, i, timeline.timelineTime.previous, timeline.timelineTime.current,
            timeline.timelineTime.subFrame, instr ? instr->time : -1,
            instr ? instr->arg0 : -1, instr ? instr->opcode : -1,
            instr ? instr->size : -1, instr ? instr->args.args[0].u : 0,
            instr ? instr->args.args[1].u : 0, instr ? instr->args.args[2].u : 0,
            instr ? instr->args.args[3].u : 0, instr ? instr->args.args[4].u : 0,
            instr ? instr->args.args[5].u : 0);
    }
    std::printf(
        "netplay spectator TH07: multiplayer fields %s frame=%u active=%u/%u/%u cherryGraze=%d/%d/%d cherryBreak=%d/%d/%d powerTaps=%d/%d/%d powerWindow=%d/%d/%d teamWipeRetry=%d\n",
        phase, frame, g_PlayerActive[0] ? 1u : 0u, g_PlayerActive[1] ? 1u : 0u,
        g_PlayerActive[2] ? 1u : 0u,
        g_cherryMaxGrazeGrowth[0], g_cherryMaxGrazeGrowth[1], g_cherryMaxGrazeGrowth[2],
        g_cherryMaxBreakGrowth[0], g_cherryMaxBreakGrowth[1], g_cherryMaxBreakGrowth[2],
        g_powerGiveTaps[0], g_powerGiveTaps[1], g_powerGiveTaps[2],
        g_powerGiveWindow[0], g_powerGiveWindow[1], g_powerGiveWindow[2],
        g_teamWipeRetryFrames);
    std::printf(
        "netplay spectator TH07: bullet manager %s frame=%u count=%d clear=%d time=%d/%d/%.6f updates=%d itemType=%d\n",
        phase, frame, g_BulletManager.bulletCount, g_BulletManager.screenClearTime,
        g_BulletManager.time.previous, g_BulletManager.time.current,
        g_BulletManager.time.subFrame, g_BulletManager.updateCount,
        static_cast<int>(g_BulletManager.itemType));
    const ZunTimer &timelineTime = g_EnemyManager.timelineTime;
    std::printf("netplay spectator TH07: enemy timeline %s frame=%u timer=%d/%d/%.6f\n",
                phase, frame, timelineTime.previous, timelineTime.current,
                timelineTime.subFrame);
}

bool CaptureSpectatorCheckpointValues(SpectatorCheckpoint &cp, std::uint32_t frame)
{
    cp.stage = static_cast<std::uint32_t>(g_GameManager.currentStage);
    const auto canonical = Th07CanonicalHash::Capture();
    LogSpectatorCheckpointCanonical("source-cp-preinput", frame, canonical);
    cp.expectedHash = canonical.composite; cp.expectedMeta = canonical.meta;
    cp.expectedStageHash = canonical.stage; cp.expectedPlayer0 = canonical.player0;
    cp.expectedPlayer1 = canonical.player1; cp.expectedPlayer2 = canonical.player2;
    cp.expectedMetaRng = canonical.metaRng; cp.expectedMetaGame = canonical.metaGame;
    cp.expectedMetaInput = canonical.metaInput; cp.expectedMetaSupervisor = canonical.metaSupervisor;
    cp.stageRngSeed = g_GameManager.stageRngSeed;
    cp.rngSeed = g_Rng.seed; cp.rngSeedBackup = g_Rng.seedBackup;
    cp.rngGeneration = g_Rng.generationCount;
    cp.difficulty = g_GameManager.difficulty; cp.difficultyMask = static_cast<i32>(g_GameManager.difficultyMask);
    cp.cherryMax = g_GameManager.cherryMax; cp.cherry = g_GameManager.cherry;
    cp.cherryPlus = g_GameManager.cherryPlus; cp.subrank = g_GameManager.subrank;
    cp.rank = g_GameManager.rank.rank; cp.maxRank = g_GameManager.rank.maxRank;
    cp.minRank = g_GameManager.rank.minRank; cp.currentStage = g_GameManager.currentStage;
    cp.framesThisStage = g_GameManager.framesThisStage; cp.flags = g_GameManager.flags;
    cp.bulletLagTime = g_GameManager.bulletLagTime; cp.isTimeStopped = g_GameManager.isTimeStopped;
    cp.slowModeSlowActive = g_GameManager.slowModeSlowActive; cp.isPaused = g_GameManager.isPaused;
    cp.powerItemCountForScore = g_GameManager.powerItemCountForScore;
    cp.character = g_GameManager.character; cp.shotType = g_GameManager.shotType;
    cp.shotTypeAndCharacter = g_GameManager.shotTypeAndCharacter;
    cp.isInPauseMenu = g_GameManager.isInPauseMenu; cp.isInRetryMenu = g_GameManager.isInRetryMenu;
    cp.demoIdx = g_GameManager.demoIdx; cp.replayStage = g_GameManager.replayStage;
    cp.demoFrames = g_GameManager.demoFrames; cp.phantasmUnlocked = g_GameManager.phantasmUnlocked;
    cp.arcadeTopX = g_GameManager.arcadeRegionTopLeftPos.x;
    cp.arcadeTopY = g_GameManager.arcadeRegionTopLeftPos.y;
    cp.arcadeSizeX = g_GameManager.arcadeRegionSize.x; cp.arcadeSizeY = g_GameManager.arcadeRegionSize.y;
    cp.movementTopX = g_GameManager.playerMovementAreaTopLeftPos.x;
    cp.movementTopY = g_GameManager.playerMovementAreaTopLeftPos.y;
    cp.movementSizeX = g_GameManager.playerMovementAreaSize.x;
    cp.movementSizeY = g_GameManager.playerMovementAreaSize.y;
    cp.csumFloat = g_GameManager.csumFloat;
    const ZunGlobals &globals = *g_GameManager.globals;
    cp.guiScore = globals.guiScore; cp.score = globals.score;
    cp.guiScoreDifference = globals.guiScoreDifference; cp.highScore = globals.highScore;
    cp.grazeInStage = globals.grazeInStage; cp.grazeInTotal = globals.grazeInTotal;
    cp.spellCardsCaptured = globals.spellCardsCaptured; cp.numRetries = globals.numRetries;
    cp.pointItemsThisStage = globals.pointItemsCollectedThisStage;
    cp.pointItemsForExtend = globals.pointItemsCollectedForExtend;
    cp.extendsFromPointItems = globals.extendsFromPointItems;
    cp.nextNeededPointItems = globals.nextNeededPointItemsForExtend;
    for (std::size_t i = 0; i < cp.globalsRng1.size(); ++i) cp.globalsRng1[i] = globals.rng1[i];
    cp.deaths = globals.deaths;
    for (std::size_t i = 0; i < cp.globalsRngFloat1.size(); ++i) cp.globalsRngFloat1[i] = globals.rngFloat1[i];
    cp.livesRemaining = globals.livesRemaining;
    for (std::size_t i = 0; i < cp.globalsRngFloat2.size(); ++i) cp.globalsRngFloat2[i] = globals.rngFloat2[i];
    cp.bombsRemaining = globals.bombsRemaining; cp.bombsUsed = globals.bombsUsed;
    for (std::size_t i = 0; i < cp.globalsRngFloat3.size(); ++i) cp.globalsRngFloat3[i] = globals.rngFloat3[i];
    cp.currentPower = globals.currentPower;
    for (std::size_t i = 0; i < cp.globalsRngFloat4.size(); ++i) cp.globalsRngFloat4[i] = globals.rngFloat4[i];
    cp.cherryStart = globals.cherryStart;
    for (std::size_t i = 0; i < cp.globalsRng2.size(); ++i) cp.globalsRng2[i] = globals.rng2[i];
    cp.curCsum = globals.curCsum; cp.csumAsSum = globals.csumAsSum;
    for (std::size_t i = 0; i < cp.csumData.size(); ++i) cp.csumData[i] = globals.csumData[i];
    cp.session = MultiplayerGameplay::GetState();
    for (std::uint8_t player = 0; player < TH07_MULTI_MAX_PLAYERS; ++player)
    {
        cp.playerActive[player] = g_PlayerActive[player] ? 1u : 0u;
        cp.lives[player] = GetPlayerLives(player); cp.bombs[player] = GetPlayerBombs(player);
        cp.powers[player] = GetPlayerPower(player);
        cp.kills[player] = g_MultiplayerContributionStats[player].enemiesDefeated;
        cp.damage[player] = g_MultiplayerContributionStats[player].damageDealt;
        cp.challengeDeaths[player] = g_MultiplayerContributionStats[player].challengeDeaths;
        cp.playerState[player] = g_Players[player].playerState;
        const auto &p = g_Players[player];
        cp.playerPositions[player * 6 + 0] = p.pos.x; cp.playerPositions[player * 6 + 1] = p.pos.y;
        cp.playerPositions[player * 6 + 2] = p.pos.z; cp.playerPositions[player * 6 + 3] = p.prevPos.x;
        cp.playerPositions[player * 6 + 4] = p.prevPos.y; cp.playerPositions[player * 6 + 5] = p.prevPos.z;
        cp.playerInvulnerabilityCurrent[player] = p.invulnerabilityTimer.current;
        cp.playerInvulnerabilityPrevious[player] = p.invulnerabilityTimer.previous;
        cp.playerInvulnerabilitySubFrame[player] = p.invulnerabilityTimer.subFrame;
        cp.currentInputs[player] = g_CurFrameGameInputs[player];
        cp.lastInputs[player] = g_LastFrameGameInputs[player];
        const auto &touch = Input::GetDirectTouchStates()[player];
        cp.directTouchXY[player * 2] = touch.x;
        cp.directTouchXY[player * 2 + 1] = touch.y;
        cp.directTouchActive[player] = touch.active ? 1u : 0u;
        cp.detailedPlayers.emplace_back();
        if (!CaptureSpectatorPlayerState(g_Players[player], player, &cp.detailedPlayers.back(),
                                         &cp.playerBullets))
            return false;
    }
    cp.currentRawInput = g_CurFrameRawInput; cp.lastRawInput = g_LastFrameRawInput;
    cp.eighthHeld = g_IsEighthFrameOfHeldInput; cp.heldFrames = g_NumOfFramesInputsWereHeld;
    for (std::size_t i = 0; i < cp.globalEclInts.size(); ++i)
    {
        cp.globalEclInts[i] = g_GlobalEclVars.intVars[i];
        cp.globalEclFloats[i] = g_GlobalEclVars.floatVars[i];
    }
    std::copy(std::begin(g_cherryMaxGrazeGrowth), std::end(g_cherryMaxGrazeGrowth),
              cp.cherryMaxGrazeGrowth.begin());
    std::copy(std::begin(g_cherryMaxBreakGrowth), std::end(g_cherryMaxBreakGrowth),
              cp.cherryMaxBreakGrowth.begin());
    std::copy(std::begin(g_powerGiveTaps), std::end(g_powerGiveTaps), cp.powerGiveTaps.begin());
    std::copy(std::begin(g_powerGiveWindow), std::end(g_powerGiveWindow),
              cp.powerGiveWindow.begin());
    cp.teamWipeRetryFrames = g_teamWipeRetryFrames;
    cp.bulletManagerCount = g_BulletManager.bulletCount;
    cp.bulletManagerScreenClearTime = g_BulletManager.screenClearTime;
    cp.bulletManagerTimePrevious = g_BulletManager.time.previous;
    cp.bulletManagerTimeCurrent = g_BulletManager.time.current;
    cp.bulletManagerTimeSubFrame = g_BulletManager.time.subFrame;
    cp.bulletManagerUpdateCount = g_BulletManager.updateCount;
    cp.bulletManagerItemType = static_cast<std::int32_t>(g_BulletManager.itemType);

    if (!g_Stage.stdData || g_Stage.stdDataSize <= g_Stage.stdData->scriptOffset)
        return false;
    const std::uint32_t stageScriptCapacity =
        (g_Stage.stdDataSize - g_Stage.stdData->scriptOffset) / sizeof(StdRawInstr);
    if (g_Stage.instructionIndex < 0 ||
        static_cast<std::uint32_t>(g_Stage.instructionIndex) >= stageScriptCapacity)
        return false;
    cp.stageScriptTimePrevious = g_Stage.scriptTime.previous;
    cp.stageScriptTimeCurrent = g_Stage.scriptTime.current;
    cp.stageScriptTimeSubFrame = g_Stage.scriptTime.subFrame;
    cp.stageInstructionIndex = g_Stage.instructionIndex;
    cp.stageFrameCounter = g_Stage.stageFrameCounter;
    cp.stageId = g_Stage.stage;
    cp.stagePositions = {g_Stage.pos.x, g_Stage.pos.y, g_Stage.pos.z,
                         g_Stage.prevPos.x, g_Stage.prevPos.y, g_Stage.prevPos.z};
    cp.stageColor = g_Stage.color;
    cp.stageFogPlanes = {g_Stage.skyFog.nearPlane, g_Stage.skyFog.farPlane,
                         g_Stage.fogEnd.nearPlane, g_Stage.fogEnd.farPlane,
                         g_Stage.fogStart.nearPlane, g_Stage.fogStart.farPlane};
    cp.stageFogColors = {g_Stage.skyFog.color.color, g_Stage.fogEnd.color.color,
                         g_Stage.fogStart.color.color};
    cp.stageSkyFogInterpDuration = g_Stage.skyFogInterpDuration;
    cp.stageSkyFogInterpPrevious = g_Stage.skyFogInterpTimer.previous;
    cp.stageSkyFogInterpCurrent = g_Stage.skyFogInterpTimer.current;
    cp.stageSkyFogInterpSubFrame = g_Stage.skyFogInterpTimer.subFrame;
    cp.stageSpellCardState = g_Stage.spellCardState;
    cp.stageTicksSinceSpellcardStarted = g_Stage.ticksSinceSpellcardStarted;
    cp.stageClearBackground = g_Stage.clearBackground ? 1u : 0u;
    cp.stageScriptWaitTime = g_Stage.scriptWaitTime;
    cp.stagePositionStart = {g_Stage.positionStart.x, g_Stage.positionStart.y,
                             g_Stage.positionStart.z};
    cp.stagePositionInterpInitial = {g_Stage.positionInterpInitial.x,
                                     g_Stage.positionInterpInitial.y,
                                     g_Stage.positionInterpInitial.z};
    cp.stagePositionInterpEndTime = g_Stage.positionInterpEndTime;
    cp.stagePositionInterpStartTime = g_Stage.positionInterpStartTime;
    cp.stageCameraTeleported = g_Stage.cameraTeleported;
    cp.stageIsDarkening = g_Stage.isDarkening ? 1u : 0u;
    cp.stageColor2 = g_Stage.color2.color;
    const StageCamera *stageCameras[] = {&g_Stage.camEnd, &g_Stage.camStart,
                                         &g_Stage.camTangentEnd, &g_Stage.camTangentStart,
                                         &g_Stage.cam, &g_Stage.prevCam};
    std::size_t stageCameraOffset = 0;
    for (const StageCamera *camera : stageCameras)
    {
        for (const ZunVec3 *value : {&camera->pos, &camera->lookAt, &camera->up,
                                     &camera->lookAtDir, &camera->right})
        {
            cp.stageCameras[stageCameraOffset++] = value->x;
            cp.stageCameras[stageCameraOffset++] = value->y;
            cp.stageCameras[stageCameraOffset++] = value->z;
        }
        cp.stageCameras[stageCameraOffset++] = camera->fov;
    }
    cp.stageVms.resize(2);
    const AnmVm *stageVms[] = {&g_Stage.vm1, &g_Stage.vm2};
    for (std::size_t i = 0; i < cp.stageVms.size(); ++i)
    {
        if (!stageVms[i]->beginningOfScript || stageVms[i]->anmFileIdx < 0) continue;
        cp.stageVmActive[i] = 1;
        if (!CaptureSpectatorAnm(*stageVms[i], &cp.stageVms[i])) return false;
    }
    for (std::size_t i = 0; i < cp.stageTimersMax.size(); ++i)
    {
        cp.stageTimersMax[i] = g_Stage.timersMax[i];
        cp.stageTimerPrevious[i] = g_Stage.timers[i].previous;
        cp.stageTimerCurrent[i] = g_Stage.timers[i].current;
        cp.stageTimerSubFrame[i] = g_Stage.timers[i].subFrame;
        cp.stageEaseModes[i] = g_Stage.easeModes[i];
    }

    if (!g_EclManager.eclFile || g_EclManager.eclFileSize < sizeof(EclRawHeader) ||
        g_EclManager.eclFile->timelineCount < 0 || g_EclManager.eclFile->timelineCount > 16)
        return false;
    cp.enemyTimelineFileOffsets.fill(-1);
    for (std::size_t i = 0; i < cp.enemyTimelineFileOffsets.size(); ++i)
    {
        const auto &timeline = g_EnemyManager.timelines[i];
        cp.enemyTimelinePrevious[i] = timeline.timelineTime.previous;
        cp.enemyTimelineCurrent[i] = timeline.timelineTime.current;
        cp.enemyTimelineSubFrame[i] = timeline.timelineTime.subFrame;
        if (!timeline.timelineInstr)
            continue;
        if (i >= static_cast<std::size_t>(g_EclManager.eclFile->timelineCount))
            return false;
        const auto fileBegin = reinterpret_cast<std::uintptr_t>(g_EclManager.eclFile);
        const auto current = reinterpret_cast<std::uintptr_t>(timeline.timelineInstr);
        if (current < fileBegin || current - fileBegin >
                g_EclManager.eclFileSize - sizeof(EclTimelineInstr))
            return false;
        const std::uint32_t fileOffset = static_cast<std::uint32_t>(current - fileBegin);
        const std::uint32_t timelineStart = g_EclManager.eclFile->timelineOffsets[i];
        if (fileOffset < timelineStart ||
            (fileOffset - timelineStart) % sizeof(EclTimelineInstr) != 0)
            return false;
        cp.enemyTimelineFileOffsets[i] = static_cast<std::int32_t>(fileOffset);
    }
    cp.enemyTimelineTimePrevious = g_EnemyManager.timelineTime.previous;
    cp.enemyTimelineTimeCurrent = g_EnemyManager.timelineTime.current;
    cp.enemyTimelineTimeSubFrame = g_EnemyManager.timelineTime.subFrame;
    cp.supervisorWanted = g_Supervisor.wantedState; cp.supervisorCurrent = g_Supervisor.curState;
    cp.supervisorPrevious = g_Supervisor.prevState; cp.supervisorEnding = g_Supervisor.isInEnding;
    cp.supervisorFramerate = g_Supervisor.effectiveFramerateMultiplier;
    cp.supervisorFlags = static_cast<i32>(g_Supervisor.flags);
    return true;
}

struct SpectatorCheckpointMetrics
{
    std::uint32_t rawBytes = 0, sparseBytes = 0, compressedBytes = 0, stateBytes = 0, eventBytes = 0;
    std::uint32_t activePlayerBullets = 0, activeBombs = 0;
    std::array<std::int32_t, TH07_MULTI_MAX_PLAYERS> playerPower{};
    std::array<std::uint32_t, TH07_MULTI_MAX_PLAYERS> activePlayerBulletsByPlayer{};
    std::array<std::uint32_t, TH07_MULTI_MAX_PLAYERS> activeBombSubrecordsByPlayer{};
};

bool EncodeSpectatorCheckpoint(std::vector<std::uint8_t> *bytes,
                               SpectatorCheckpointMetrics *metrics = nullptr,
                               std::uint32_t frame = 0)
{
    if (!bytes || !g_GameManager.globals || !MultiplayerGameplay::IsMultiplayer() ||
        g_GameManager.currentStage < 1 || g_GameManager.currentStage > 6)
        return false;
    SpectatorCheckpoint cp;
    if (!CaptureSpectatorCheckpointValues(cp, frame) ||
        cp.detailedPlayers.size() != TH07_MULTI_MAX_PLAYERS ||
        cp.playerBullets.size() > TH07_MULTI_MAX_PLAYERS * 96u)
        return false;
    SpectatorScalars::Writer writer(2u * 1024u * 1024u);
    writer.Values(cp.magic, cp.stage, cp.expectedHash, cp.expectedMeta, cp.expectedStageHash,
                  cp.expectedPlayer0, cp.expectedPlayer1, cp.expectedPlayer2,
                  cp.expectedMetaRng, cp.expectedMetaGame, cp.expectedMetaInput,
                  cp.expectedMetaSupervisor,
                  cp.stageRngSeed, cp.rngSeed,
                  cp.rngSeedBackup, cp.rngGeneration, cp.difficulty, cp.difficultyMask,
                  cp.cherryMax, cp.cherry, cp.cherryPlus, cp.subrank, cp.rank, cp.maxRank,
                  cp.minRank, cp.currentStage, cp.framesThisStage, cp.flags, cp.bulletLagTime,
                  cp.isTimeStopped, cp.slowModeSlowActive, cp.isPaused, cp.powerItemCountForScore,
                  cp.character, cp.shotType, cp.shotTypeAndCharacter, cp.isInPauseMenu,
                  cp.isInRetryMenu, cp.demoIdx, cp.replayStage, cp.phantasmUnlocked, cp.demoFrames,
                  cp.arcadeTopX, cp.arcadeTopY, cp.arcadeSizeX, cp.arcadeSizeY,
                  cp.movementTopX, cp.movementTopY, cp.movementSizeX, cp.movementSizeY, cp.csumFloat,
                  cp.guiScore, cp.score, cp.guiScoreDifference, cp.highScore, cp.grazeInStage,
                  cp.grazeInTotal, cp.spellCardsCaptured, cp.numRetries, cp.pointItemsThisStage,
                  cp.pointItemsForExtend, cp.extendsFromPointItems, cp.nextNeededPointItems,
                  cp.deaths, cp.livesRemaining, cp.bombsRemaining, cp.bombsUsed, cp.currentPower,
                  cp.cherryStart, cp.curCsum, cp.csumAsSum,
                  cp.session.playerCount, cp.session.localPlayer, cp.session.showStagePlayerNames,
                  cp.session.challengeMode, cp.session.stage4BossChain, cp.session.showContributionStats,
                  cp.currentRawInput, cp.lastRawInput, cp.eighthHeld, cp.heldFrames,
                  cp.supervisorWanted, cp.supervisorCurrent, cp.supervisorPrevious,
                  cp.supervisorEnding, cp.supervisorFramerate, cp.supervisorFlags);
    for (const auto &slot : cp.session.players)
        writer.Values(slot.active, slot.temporarilyAbsent, slot.permanentlyDeparted,
                      slot.character, slot.shot);
    writer.Array(cp.globalsRng1); writer.Array(cp.globalsRngFloat1);
    writer.Array(cp.globalsRngFloat2); writer.Array(cp.globalsRngFloat3);
    writer.Array(cp.globalsRngFloat4); writer.Array(cp.globalsRng2); writer.Array(cp.csumData);
    writer.Array(cp.playerActive); writer.Array(cp.lives); writer.Array(cp.bombs); writer.Array(cp.powers);
    writer.Array(cp.kills); writer.Array(cp.damage); writer.Array(cp.challengeDeaths);
    writer.Array(cp.playerState); writer.Array(cp.playerPositions);
    writer.Array(cp.playerInvulnerabilityCurrent); writer.Array(cp.playerInvulnerabilityPrevious);
    writer.Array(cp.playerInvulnerabilitySubFrame); writer.Array(cp.currentInputs); writer.Array(cp.lastInputs);
    writer.Array(cp.directTouchXY); writer.Array(cp.directTouchActive);
    writer.Array(cp.globalEclInts); writer.Array(cp.globalEclFloats);
    writer.Values(cp.teamWipeRetryFrames, cp.bulletManagerCount,
                  cp.bulletManagerScreenClearTime, cp.bulletManagerTimePrevious,
                  cp.bulletManagerTimeCurrent, cp.bulletManagerTimeSubFrame,
                  cp.bulletManagerUpdateCount, cp.bulletManagerItemType,
                  cp.stageScriptTimePrevious, cp.stageScriptTimeCurrent,
                  cp.stageScriptTimeSubFrame, cp.stageInstructionIndex,
                  cp.stageFrameCounter, cp.stageId, cp.stageColor,
                  cp.stageSkyFogInterpDuration, cp.stageSkyFogInterpPrevious,
                  cp.stageSkyFogInterpCurrent, cp.stageSkyFogInterpSubFrame,
                  cp.stageSpellCardState, cp.stageTicksSinceSpellcardStarted,
                  cp.stageClearBackground, cp.stageScriptWaitTime,
                  cp.stagePositionInterpEndTime, cp.stagePositionInterpStartTime,
                  cp.stageCameraTeleported, cp.stageIsDarkening, cp.stageColor2,
                  cp.enemyTimelineTimePrevious, cp.enemyTimelineTimeCurrent,
                  cp.enemyTimelineTimeSubFrame);
    writer.Array(cp.cherryMaxGrazeGrowth); writer.Array(cp.cherryMaxBreakGrowth);
    writer.Array(cp.powerGiveTaps); writer.Array(cp.powerGiveWindow);
    writer.Array(cp.stagePositions); writer.Array(cp.stageFogPlanes); writer.Array(cp.stageFogColors);
    writer.Array(cp.stageTimersMax); writer.Array(cp.stageEaseModes);
    writer.Array(cp.stageTimerPrevious); writer.Array(cp.stageTimerCurrent);
    writer.Array(cp.stageTimerSubFrame); writer.Array(cp.stagePositionStart);
    writer.Array(cp.stagePositionInterpInitial); writer.Array(cp.enemyTimelinePrevious);
    writer.Array(cp.enemyTimelineCurrent); writer.Array(cp.enemyTimelineSubFrame);
    writer.Array(cp.enemyTimelineFileOffsets); writer.Array(cp.stageCameras);
    writer.Array(cp.stageVmActive);
    for (std::size_t i = 0; i < cp.stageVmActive.size(); ++i)
        if (cp.stageVmActive[i] &&
            (cp.stageVms.size() != cp.stageVmActive.size() ||
             !WriteSpectatorAnm(writer, cp.stageVms[i])))
            return false;
    const auto playerCount = static_cast<std::uint16_t>(cp.detailedPlayers.size());
    const auto bulletCount = static_cast<std::uint16_t>(cp.playerBullets.size());
    if (!writer.Value(playerCount)) return false;
    for (const auto &player : cp.detailedPlayers)
        if (!WriteSpectatorPlayer(writer, player)) return false;
    if (!writer.Value(bulletCount)) return false;
    for (const auto &bullet : cp.playerBullets)
        if (!WriteSpectatorPlayerBullet(writer, bullet)) return false;
    if (!writer.Good()) return false;
    if (metrics)
    {
        metrics->rawBytes = static_cast<std::uint32_t>(writer.Bytes().size());
        for (std::size_t player = 0; player < cp.playerActive.size(); ++player)
        {
            metrics->playerPower[player] = cp.powers[player];
            if (player < cp.detailedPlayers.size() && cp.detailedPlayers[player].bombIsInUse)
            {
                const auto &bomb = cp.detailedPlayers[player];
                ++metrics->activeBombs;
                metrics->activeBombSubrecordsByPlayer[player] = static_cast<std::uint32_t>(
                    std::count_if(bomb.bombSubInfo.begin(), bomb.bombSubInfo.end(),
                                  [](const SpectatorBombSubState &sub) { return sub.state != 0; }));
            }
        }
        for (const auto &bullet : cp.playerBullets)
        {
            if (bullet.bulletState == 0 || bullet.player >= TH07_MULTI_MAX_PLAYERS) continue;
            ++metrics->activePlayerBullets;
            ++metrics->activePlayerBulletsByPlayer[bullet.player];
        }
    }
    if (!CompressSpectatorCheckpoint(writer.Bytes(), bytes)) return false;
    if (metrics)
    {
        metrics->stateBytes = static_cast<std::uint32_t>(bytes->size());
        metrics->sparseBytes = ReadCheckpointWord(bytes->data() + 8);
        metrics->compressedBytes = static_cast<std::uint32_t>(bytes->size() - 12);
    }
    return true;
}

bool DecodeSpectatorCheckpoint(const std::vector<std::uint8_t> &bytes, SpectatorCheckpoint *cp)
{
    if (!cp) return false;
    std::vector<std::uint8_t> raw;
    if (!DecompressSpectatorCheckpoint(bytes, &raw)) return false;
    SpectatorScalars::Reader reader(raw);
    reader.Values(cp->magic, cp->stage, cp->expectedHash, cp->expectedMeta, cp->expectedStageHash,
                  cp->expectedPlayer0, cp->expectedPlayer1, cp->expectedPlayer2,
                  cp->expectedMetaRng, cp->expectedMetaGame, cp->expectedMetaInput,
                  cp->expectedMetaSupervisor,
                  cp->stageRngSeed, cp->rngSeed,
                  cp->rngSeedBackup, cp->rngGeneration, cp->difficulty, cp->difficultyMask,
                  cp->cherryMax, cp->cherry, cp->cherryPlus, cp->subrank, cp->rank, cp->maxRank,
                  cp->minRank, cp->currentStage, cp->framesThisStage, cp->flags, cp->bulletLagTime,
                  cp->isTimeStopped, cp->slowModeSlowActive, cp->isPaused, cp->powerItemCountForScore,
                  cp->character, cp->shotType, cp->shotTypeAndCharacter, cp->isInPauseMenu,
                  cp->isInRetryMenu, cp->demoIdx, cp->replayStage, cp->phantasmUnlocked, cp->demoFrames,
                  cp->arcadeTopX, cp->arcadeTopY, cp->arcadeSizeX, cp->arcadeSizeY,
                  cp->movementTopX, cp->movementTopY, cp->movementSizeX, cp->movementSizeY, cp->csumFloat,
                  cp->guiScore, cp->score, cp->guiScoreDifference, cp->highScore, cp->grazeInStage,
                  cp->grazeInTotal, cp->spellCardsCaptured, cp->numRetries, cp->pointItemsThisStage,
                  cp->pointItemsForExtend, cp->extendsFromPointItems, cp->nextNeededPointItems,
                  cp->deaths, cp->livesRemaining, cp->bombsRemaining, cp->bombsUsed, cp->currentPower,
                  cp->cherryStart, cp->curCsum, cp->csumAsSum,
                  cp->session.playerCount, cp->session.localPlayer, cp->session.showStagePlayerNames,
                  cp->session.challengeMode, cp->session.stage4BossChain, cp->session.showContributionStats,
                  cp->currentRawInput, cp->lastRawInput, cp->eighthHeld, cp->heldFrames,
                  cp->supervisorWanted, cp->supervisorCurrent, cp->supervisorPrevious,
                  cp->supervisorEnding, cp->supervisorFramerate, cp->supervisorFlags);
    for (auto &slot : cp->session.players)
        reader.Values(slot.active, slot.temporarilyAbsent, slot.permanentlyDeparted,
                      slot.character, slot.shot);
    reader.Array(cp->globalsRng1); reader.Array(cp->globalsRngFloat1);
    reader.Array(cp->globalsRngFloat2); reader.Array(cp->globalsRngFloat3);
    reader.Array(cp->globalsRngFloat4); reader.Array(cp->globalsRng2); reader.Array(cp->csumData);
    reader.Array(cp->playerActive); reader.Array(cp->lives); reader.Array(cp->bombs); reader.Array(cp->powers);
    reader.Array(cp->kills); reader.Array(cp->damage); reader.Array(cp->challengeDeaths);
    reader.Array(cp->playerState); reader.Array(cp->playerPositions);
    reader.Array(cp->playerInvulnerabilityCurrent); reader.Array(cp->playerInvulnerabilityPrevious);
    reader.Array(cp->playerInvulnerabilitySubFrame); reader.Array(cp->currentInputs); reader.Array(cp->lastInputs);
    reader.Array(cp->directTouchXY); reader.Array(cp->directTouchActive);
    reader.Array(cp->globalEclInts); reader.Array(cp->globalEclFloats);
    reader.Values(cp->teamWipeRetryFrames, cp->bulletManagerCount,
                  cp->bulletManagerScreenClearTime, cp->bulletManagerTimePrevious,
                  cp->bulletManagerTimeCurrent, cp->bulletManagerTimeSubFrame,
                  cp->bulletManagerUpdateCount, cp->bulletManagerItemType,
                  cp->stageScriptTimePrevious, cp->stageScriptTimeCurrent,
                  cp->stageScriptTimeSubFrame, cp->stageInstructionIndex,
                  cp->stageFrameCounter, cp->stageId, cp->stageColor,
                  cp->stageSkyFogInterpDuration, cp->stageSkyFogInterpPrevious,
                  cp->stageSkyFogInterpCurrent, cp->stageSkyFogInterpSubFrame,
                  cp->stageSpellCardState, cp->stageTicksSinceSpellcardStarted,
                  cp->stageClearBackground, cp->stageScriptWaitTime,
                  cp->stagePositionInterpEndTime, cp->stagePositionInterpStartTime,
                  cp->stageCameraTeleported, cp->stageIsDarkening, cp->stageColor2,
                  cp->enemyTimelineTimePrevious, cp->enemyTimelineTimeCurrent,
                  cp->enemyTimelineTimeSubFrame);
    reader.Array(cp->cherryMaxGrazeGrowth); reader.Array(cp->cherryMaxBreakGrowth);
    reader.Array(cp->powerGiveTaps); reader.Array(cp->powerGiveWindow);
    reader.Array(cp->stagePositions); reader.Array(cp->stageFogPlanes); reader.Array(cp->stageFogColors);
    reader.Array(cp->stageTimersMax); reader.Array(cp->stageEaseModes);
    reader.Array(cp->stageTimerPrevious); reader.Array(cp->stageTimerCurrent);
    reader.Array(cp->stageTimerSubFrame); reader.Array(cp->stagePositionStart);
    reader.Array(cp->stagePositionInterpInitial); reader.Array(cp->enemyTimelinePrevious);
    reader.Array(cp->enemyTimelineCurrent); reader.Array(cp->enemyTimelineSubFrame);
    reader.Array(cp->enemyTimelineFileOffsets); reader.Array(cp->stageCameras);
    reader.Array(cp->stageVmActive);
    cp->stageVms.resize(cp->stageVmActive.size());
    for (std::size_t i = 0; i < cp->stageVmActive.size(); ++i)
        if (cp->stageVmActive[i] > 1 ||
            (cp->stageVmActive[i] && !ReadSpectatorAnm(reader, &cp->stageVms[i])))
            return false;
    std::uint16_t playerCount = 0, bulletCount = 0;
    if (!reader.Value(playerCount) || playerCount != TH07_MULTI_MAX_PLAYERS) return false;
    cp->detailedPlayers.resize(playerCount);
    for (auto &player : cp->detailedPlayers)
        if (!ReadSpectatorPlayer(reader, &player)) return false;
    if (!reader.Value(bulletCount) || bulletCount > TH07_MULTI_MAX_PLAYERS * 96u) return false;
    cp->playerBullets.resize(bulletCount);
    std::array<std::array<bool, 96>, TH07_MULTI_MAX_PLAYERS> seenBullets{};
    for (auto &bullet : cp->playerBullets)
    {
        if (!ReadSpectatorPlayerBullet(reader, &bullet) || bullet.player >= TH07_MULTI_MAX_PLAYERS ||
            bullet.slot >= 96 || seenBullets[bullet.player][bullet.slot] ||
            (bullet.bulletState != 0 && (!bullet.hasVm || !bullet.shtEntry.valid)) ||
            (bullet.hasVm && bullet.bulletState == 0))
            return false;
        seenBullets[bullet.player][bullet.slot] = true;
    }
    for (std::size_t playerIndex = 0; playerIndex < cp->detailedPlayers.size(); ++playerIndex)
        for (const auto &timer : cp->detailedPlayers[playerIndex].timers)
            if (timer.bulletSlot < -1 || timer.bulletSlot >= 96 ||
                (timer.bulletSlot >= 0 && !seenBullets[playerIndex][timer.bulletSlot]))
                return false;
    return reader.Done() && cp->magic == 0x37504353 && cp->stage >= 1 && cp->stage <= 6 &&
           cp->currentStage == static_cast<i32>(cp->stage) &&
           cp->session.playerCount >= 2 && cp->session.playerCount <= TH07_MULTI_MAX_PLAYERS &&
           cp->session.players[0].active && cp->character <= 2 && cp->shotType <= 1;
}

bool ApplySpectatorCheckpointState(const SpectatorCheckpoint &cp)
{
    if (cp.detailedPlayers.size() != TH07_MULTI_MAX_PLAYERS ||
        cp.stageVms.size() != cp.stageVmActive.size() || cp.stageVmActive[0] > 1 ||
        cp.stageVmActive[1] > 1 ||
        cp.stageId != cp.stage || cp.stageInstructionIndex < 0 || cp.stageFrameCounter < 0 ||
        cp.stageSpellCardState < SPELLCARD_STATE_INACTIVE ||
        cp.stageSpellCardState > SPELLCARD_STATE_ACTIVE || cp.stageClearBackground > 1 ||
        cp.stageCameraTeleported > 1 || cp.stageIsDarkening > 1 ||
        cp.bulletManagerCount < 0 || cp.bulletManagerCount > MAX_BULLETS ||
        cp.bulletManagerItemType < ITEM_POWER_SMALL ||
        (cp.bulletManagerItemType > ITEM_STAR && cp.bulletManagerItemType != ITEM_NO_ITEM))
        return false;
    if (!g_Stage.stdData || g_Stage.stdDataSize <= g_Stage.stdData->scriptOffset ||
        !g_EclManager.eclFile || g_EclManager.eclFileSize < sizeof(EclRawHeader) ||
        g_EclManager.eclFile->timelineCount < 0 || g_EclManager.eclFile->timelineCount > 16)
        return false;
    const std::uint32_t stageScriptCapacity =
        (g_Stage.stdDataSize - g_Stage.stdData->scriptOffset) / sizeof(StdRawInstr);
    if (static_cast<std::uint32_t>(cp.stageInstructionIndex) >= stageScriptCapacity)
        return false;
    for (std::size_t i = 0; i < cp.enemyTimelineFileOffsets.size(); ++i)
    {
        const std::int32_t savedOffset = cp.enemyTimelineFileOffsets[i];
        if (savedOffset == -1) continue;
        if (savedOffset < 0 || i >= static_cast<std::size_t>(g_EclManager.eclFile->timelineCount))
            return false;
        const std::uint32_t offset = static_cast<std::uint32_t>(savedOffset);
        const std::uint32_t timelineStart = g_EclManager.eclFile->timelineOffsets[i];
        std::uint32_t timelineEnd = g_EclManager.eclFileSize;
        for (std::size_t other = 0;
             other < static_cast<std::size_t>(g_EclManager.eclFile->timelineCount); ++other)
        {
            const std::uint32_t otherStart = g_EclManager.eclFile->timelineOffsets[other];
            if (otherStart > timelineStart && otherStart < timelineEnd)
                timelineEnd = otherStart;
        }
        if (timelineEnd < sizeof(EclTimelineInstr) || offset < timelineStart ||
            offset > timelineEnd - sizeof(EclTimelineInstr) ||
            (offset - timelineStart) % sizeof(EclTimelineInstr) != 0)
            return false;
        const auto *instr = reinterpret_cast<const EclTimelineInstr *>(
            reinterpret_cast<const std::uint8_t *>(g_EclManager.eclFile) + offset);
        if (instr->time < -1 || (instr->time >= 0 && instr->size != sizeof(EclTimelineInstr)))
            return false;
    }
    MultiplayerGameplay::Configure(cp.session);
    g_GameManager.difficulty = cp.difficulty; g_GameManager.difficultyMask = static_cast<u32>(cp.difficultyMask);
    g_GameManager.cherryMax = cp.cherryMax; g_GameManager.cherry = cp.cherry;
    g_GameManager.cherryPlus = cp.cherryPlus; g_GameManager.subrank = cp.subrank;
    g_GameManager.rank.rank = cp.rank; g_GameManager.rank.maxRank = cp.maxRank; g_GameManager.rank.minRank = cp.minRank;
    g_GameManager.currentStage = cp.currentStage; g_GameManager.framesThisStage = cp.framesThisStage;
    g_GameManager.flags = cp.flags; g_GameManager.bulletLagTime = cp.bulletLagTime;
    g_GameManager.isTimeStopped = cp.isTimeStopped; g_GameManager.slowModeSlowActive = cp.slowModeSlowActive;
    g_GameManager.isPaused = cp.isPaused; g_GameManager.powerItemCountForScore = cp.powerItemCountForScore;
    g_GameManager.character = cp.character; g_GameManager.shotType = cp.shotType;
    g_GameManager.shotTypeAndCharacter = cp.shotTypeAndCharacter;
    g_GameManager.isInPauseMenu = cp.isInPauseMenu; g_GameManager.isInRetryMenu = cp.isInRetryMenu;
    g_GameManager.demoIdx = cp.demoIdx; g_GameManager.replayStage = cp.replayStage;
    g_GameManager.demoFrames = cp.demoFrames; g_GameManager.phantasmUnlocked = cp.phantasmUnlocked;
    g_GameManager.stageRngSeed = cp.stageRngSeed;
    g_GameManager.arcadeRegionTopLeftPos = {cp.arcadeTopX, cp.arcadeTopY};
    g_GameManager.arcadeRegionSize = {cp.arcadeSizeX, cp.arcadeSizeY};
    g_GameManager.playerMovementAreaTopLeftPos = {cp.movementTopX, cp.movementTopY};
    g_GameManager.playerMovementAreaSize = {cp.movementSizeX, cp.movementSizeY};
    g_GameManager.csumFloat = cp.csumFloat;
    ZunGlobals &globals = *g_GameManager.globals;
    globals.guiScore = cp.guiScore; globals.score = cp.score;
    globals.guiScoreDifference = cp.guiScoreDifference; globals.highScore = cp.highScore;
    globals.grazeInStage = cp.grazeInStage; globals.grazeInTotal = cp.grazeInTotal;
    globals.spellCardsCaptured = cp.spellCardsCaptured; globals.numRetries = cp.numRetries;
    globals.pointItemsCollectedThisStage = cp.pointItemsThisStage;
    globals.pointItemsCollectedForExtend = cp.pointItemsForExtend;
    globals.extendsFromPointItems = cp.extendsFromPointItems;
    globals.nextNeededPointItemsForExtend = cp.nextNeededPointItems;
    for (std::size_t i = 0; i < cp.globalsRng1.size(); ++i) globals.rng1[i] = cp.globalsRng1[i];
    globals.deaths = cp.deaths;
    for (std::size_t i = 0; i < cp.globalsRngFloat1.size(); ++i) globals.rngFloat1[i] = cp.globalsRngFloat1[i];
    globals.livesRemaining = cp.livesRemaining;
    for (std::size_t i = 0; i < cp.globalsRngFloat2.size(); ++i) globals.rngFloat2[i] = cp.globalsRngFloat2[i];
    globals.bombsRemaining = cp.bombsRemaining; globals.bombsUsed = cp.bombsUsed;
    for (std::size_t i = 0; i < cp.globalsRngFloat3.size(); ++i) globals.rngFloat3[i] = cp.globalsRngFloat3[i];
    globals.currentPower = cp.currentPower;
    for (std::size_t i = 0; i < cp.globalsRngFloat4.size(); ++i) globals.rngFloat4[i] = cp.globalsRngFloat4[i];
    globals.cherryStart = cp.cherryStart;
    for (std::size_t i = 0; i < cp.globalsRng2.size(); ++i) globals.rng2[i] = cp.globalsRng2[i];
    globals.curCsum = cp.curCsum; globals.csumAsSum = cp.csumAsSum;
    for (std::size_t i = 0; i < cp.csumData.size(); ++i) globals.csumData[i] = cp.csumData[i];
    for (std::uint8_t player = 0; player < TH07_MULTI_MAX_PLAYERS; ++player)
    {
        g_PlayerActive[player] = cp.playerActive[player] != 0;
        SetPlayerLives(player, cp.lives[player]); SetPlayerBombs(player, cp.bombs[player]);
        SetPlayerPower(player, cp.powers[player]);
        g_MultiplayerContributionStats[player].enemiesDefeated = cp.kills[player];
        g_MultiplayerContributionStats[player].damageDealt = cp.damage[player];
        g_MultiplayerContributionStats[player].challengeDeaths = cp.challengeDeaths[player];
        Player &p = g_Players[player];
        p.playerState = static_cast<i8>(cp.playerState[player]);
        p.pos = {cp.playerPositions[player * 6], cp.playerPositions[player * 6 + 1], cp.playerPositions[player * 6 + 2]};
        p.prevPos = {cp.playerPositions[player * 6 + 3], cp.playerPositions[player * 6 + 4], cp.playerPositions[player * 6 + 5]};
        p.invulnerabilityTimer.current = cp.playerInvulnerabilityCurrent[player];
        p.invulnerabilityTimer.previous = cp.playerInvulnerabilityPrevious[player];
        p.invulnerabilityTimer.subFrame = cp.playerInvulnerabilitySubFrame[player];
        g_CurFrameGameInputs[player] = cp.currentInputs[player];
        g_LastFrameGameInputs[player] = cp.lastInputs[player];
        auto &touch = Input::GetDirectTouchStates()[player];
        touch.x = cp.directTouchXY[player * 2];
        touch.y = cp.directTouchXY[player * 2 + 1];
        touch.active = cp.directTouchActive[player] != 0;
        if (!ApplySpectatorPlayerState(g_Players[player], cp.detailedPlayers[player],
                                       cp.playerBullets, player))
            return false;
    }
    // Seat-0 resource setters above update TH07's original integrity checksum.
    // Checkpoint restore must finish by reinstating the source checksum values
    // after those setters, or the otherwise-correct initializer hashes differ.
    g_GameManager.csumFloat = cp.csumFloat;
    for (std::size_t i = 0; i < cp.globalsRng1.size(); ++i) globals.rng1[i] = cp.globalsRng1[i];
    for (std::size_t i = 0; i < cp.globalsRng2.size(); ++i) globals.rng2[i] = cp.globalsRng2[i];
    globals.curCsum = cp.curCsum;
    globals.csumAsSum = cp.csumAsSum;
    g_CurFrameRawInput = cp.currentRawInput; g_LastFrameRawInput = cp.lastRawInput;
    g_IsEighthFrameOfHeldInput = cp.eighthHeld; g_NumOfFramesInputsWereHeld = cp.heldFrames;
    for (std::size_t i = 0; i < cp.globalEclInts.size(); ++i)
    {
        g_GlobalEclVars.intVars[i] = cp.globalEclInts[i];
        g_GlobalEclVars.floatVars[i] = cp.globalEclFloats[i];
    }
    std::copy(cp.cherryMaxGrazeGrowth.begin(), cp.cherryMaxGrazeGrowth.end(),
              std::begin(g_cherryMaxGrazeGrowth));
    std::copy(cp.cherryMaxBreakGrowth.begin(), cp.cherryMaxBreakGrowth.end(),
              std::begin(g_cherryMaxBreakGrowth));
    std::copy(cp.powerGiveTaps.begin(), cp.powerGiveTaps.end(), std::begin(g_powerGiveTaps));
    std::copy(cp.powerGiveWindow.begin(), cp.powerGiveWindow.end(), std::begin(g_powerGiveWindow));
    g_teamWipeRetryFrames = cp.teamWipeRetryFrames;

    g_BulletManager.bulletCount = cp.bulletManagerCount;
    g_BulletManager.screenClearTime = cp.bulletManagerScreenClearTime;
    g_BulletManager.time.previous = cp.bulletManagerTimePrevious;
    g_BulletManager.time.current = cp.bulletManagerTimeCurrent;
    g_BulletManager.time.subFrame = cp.bulletManagerTimeSubFrame;
    g_BulletManager.updateCount = cp.bulletManagerUpdateCount;
    g_BulletManager.itemType = static_cast<ItemType>(cp.bulletManagerItemType);

    g_Stage.scriptTime.previous = cp.stageScriptTimePrevious;
    g_Stage.scriptTime.current = cp.stageScriptTimeCurrent;
    g_Stage.scriptTime.subFrame = cp.stageScriptTimeSubFrame;
    g_Stage.instructionIndex = cp.stageInstructionIndex;
    g_Stage.stageFrameCounter = cp.stageFrameCounter;
    g_Stage.stage = cp.stageId;
    g_Stage.pos = {cp.stagePositions[0], cp.stagePositions[1], cp.stagePositions[2]};
    g_Stage.prevPos = {cp.stagePositions[3], cp.stagePositions[4], cp.stagePositions[5]};
    g_Stage.color = cp.stageColor;
    g_Stage.skyFog.nearPlane = cp.stageFogPlanes[0];
    g_Stage.skyFog.farPlane = cp.stageFogPlanes[1];
    g_Stage.fogEnd.nearPlane = cp.stageFogPlanes[2];
    g_Stage.fogEnd.farPlane = cp.stageFogPlanes[3];
    g_Stage.fogStart.nearPlane = cp.stageFogPlanes[4];
    g_Stage.fogStart.farPlane = cp.stageFogPlanes[5];
    g_Stage.skyFog.color.color = cp.stageFogColors[0];
    g_Stage.fogEnd.color.color = cp.stageFogColors[1];
    g_Stage.fogStart.color.color = cp.stageFogColors[2];
    g_Stage.skyFogInterpDuration = cp.stageSkyFogInterpDuration;
    g_Stage.skyFogInterpTimer.previous = cp.stageSkyFogInterpPrevious;
    g_Stage.skyFogInterpTimer.current = cp.stageSkyFogInterpCurrent;
    g_Stage.skyFogInterpTimer.subFrame = cp.stageSkyFogInterpSubFrame;
    g_Stage.spellCardState = cp.stageSpellCardState;
    g_Stage.ticksSinceSpellcardStarted = cp.stageTicksSinceSpellcardStarted;
    g_Stage.clearBackground = cp.stageClearBackground != 0;
    g_Stage.scriptWaitTime = cp.stageScriptWaitTime;
    g_Stage.positionStart = {cp.stagePositionStart[0], cp.stagePositionStart[1],
                             cp.stagePositionStart[2]};
    g_Stage.positionInterpInitial = {cp.stagePositionInterpInitial[0],
                                     cp.stagePositionInterpInitial[1],
                                     cp.stagePositionInterpInitial[2]};
    g_Stage.positionInterpEndTime = cp.stagePositionInterpEndTime;
    g_Stage.positionInterpStartTime = cp.stagePositionInterpStartTime;
    g_Stage.cameraTeleported = cp.stageCameraTeleported;
    g_Stage.isDarkening = cp.stageIsDarkening != 0;
    g_Stage.color2.color = cp.stageColor2;
    StageCamera *stageCameras[] = {&g_Stage.camEnd, &g_Stage.camStart,
                                   &g_Stage.camTangentEnd, &g_Stage.camTangentStart,
                                   &g_Stage.cam, &g_Stage.prevCam};
    std::size_t stageCameraOffset = 0;
    for (StageCamera *camera : stageCameras)
    {
        for (ZunVec3 *value : {&camera->pos, &camera->lookAt, &camera->up,
                               &camera->lookAtDir, &camera->right})
        {
            value->x = cp.stageCameras[stageCameraOffset++];
            value->y = cp.stageCameras[stageCameraOffset++];
            value->z = cp.stageCameras[stageCameraOffset++];
        }
        camera->fov = cp.stageCameras[stageCameraOffset++];
    }
    AnmVm *stageVms[] = {&g_Stage.vm1, &g_Stage.vm2};
    for (std::size_t i = 0; i < cp.stageVmActive.size(); ++i)
        if (cp.stageVmActive[i] && !ApplySpectatorAnm(cp.stageVms[i], stageVms[i]))
            return false;
    for (std::size_t i = 0; i < cp.stageTimersMax.size(); ++i)
    {
        g_Stage.timersMax[i] = cp.stageTimersMax[i];
        g_Stage.timers[i].previous = cp.stageTimerPrevious[i];
        g_Stage.timers[i].current = cp.stageTimerCurrent[i];
        g_Stage.timers[i].subFrame = cp.stageTimerSubFrame[i];
        g_Stage.easeModes[i] = cp.stageEaseModes[i];
    }
    g_EnemyManager.timelineTime.previous = cp.enemyTimelineTimePrevious;
    g_EnemyManager.timelineTime.current = cp.enemyTimelineTimeCurrent;
    g_EnemyManager.timelineTime.subFrame = cp.enemyTimelineTimeSubFrame;
    for (std::size_t i = 0; i < cp.enemyTimelineFileOffsets.size(); ++i)
    {
        auto &timeline = g_EnemyManager.timelines[i];
        timeline.timelineTime.previous = cp.enemyTimelinePrevious[i];
        timeline.timelineTime.current = cp.enemyTimelineCurrent[i];
        timeline.timelineTime.subFrame = cp.enemyTimelineSubFrame[i];
        if (cp.enemyTimelineFileOffsets[i] < 0)
            timeline.timelineInstr = nullptr;
        else
            timeline.timelineInstr = reinterpret_cast<EclTimelineInstr *>(
                reinterpret_cast<std::uint8_t *>(g_EclManager.eclFile) +
                static_cast<std::uint32_t>(cp.enemyTimelineFileOffsets[i]));
    }
    g_Supervisor.wantedState = cp.supervisorWanted; g_Supervisor.curState = cp.supervisorCurrent;
    g_Supervisor.prevState = cp.supervisorPrevious; g_Supervisor.isInEnding = cp.supervisorEnding;
    g_Supervisor.effectiveFramerateMultiplier = cp.supervisorFramerate;
    g_Supervisor.flags = static_cast<u32>(cp.supervisorFlags);
    g_Rng.seed = cp.rngSeed; g_Rng.seedBackup = cp.rngSeedBackup;
    g_Rng.generationCount = cp.rngGeneration;
    return true;
}

void LogSpectatorCheckpointMetaDifferences(const SpectatorCheckpoint &cp)
{
#define CHECK_SPECTATOR_FIELD(actual, saved, label) \
    do { if ((actual) != (saved)) std::printf(" %s", label); } while (false)
    std::printf("netplay spectator TH07: metaGame differing fields:");
    CHECK_SPECTATOR_FIELD(g_GameManager.isTimeStopped, cp.isTimeStopped, "isTimeStopped");
    CHECK_SPECTATOR_FIELD(g_GameManager.slowModeSlowActive, cp.slowModeSlowActive, "slowModeSlowActive");
    CHECK_SPECTATOR_FIELD(g_GameManager.difficulty, cp.difficulty, "difficulty");
    CHECK_SPECTATOR_FIELD(g_GameManager.difficultyMask, static_cast<u32>(cp.difficultyMask), "difficultyMask");
    CHECK_SPECTATOR_FIELD(g_GameManager.isPaused, cp.isPaused, "isPaused");
    CHECK_SPECTATOR_FIELD(g_GameManager.powerItemCountForScore, cp.powerItemCountForScore, "powerItemCountForScore");
    CHECK_SPECTATOR_FIELD(g_GameManager.character, cp.character, "character");
    CHECK_SPECTATOR_FIELD(g_GameManager.shotType, cp.shotType, "shotType");
    CHECK_SPECTATOR_FIELD(g_GameManager.shotTypeAndCharacter, cp.shotTypeAndCharacter, "shotTypeAndCharacter");
    CHECK_SPECTATOR_FIELD(g_GameManager.flags, cp.flags, "flags");
    CHECK_SPECTATOR_FIELD(g_GameManager.isInPauseMenu, cp.isInPauseMenu, "isInPauseMenu");
    CHECK_SPECTATOR_FIELD(g_GameManager.isInRetryMenu, cp.isInRetryMenu, "isInRetryMenu");
    CHECK_SPECTATOR_FIELD(g_GameManager.demoIdx, cp.demoIdx, "demoIdx");
    CHECK_SPECTATOR_FIELD(g_GameManager.replayStage, cp.replayStage, "replayStage");
    CHECK_SPECTATOR_FIELD(g_GameManager.demoFrames, cp.demoFrames, "demoFrames");
    CHECK_SPECTATOR_FIELD(g_GameManager.stageRngSeed, cp.stageRngSeed, "stageRngSeed");
    CHECK_SPECTATOR_FIELD(g_GameManager.framesThisStage, cp.framesThisStage, "framesThisStage");
    CHECK_SPECTATOR_FIELD(g_GameManager.currentStage, cp.currentStage, "currentStage");
    CHECK_SPECTATOR_FIELD(g_GameManager.arcadeRegionTopLeftPos.x, cp.arcadeTopX, "arcadeTopX");
    CHECK_SPECTATOR_FIELD(g_GameManager.arcadeRegionTopLeftPos.y, cp.arcadeTopY, "arcadeTopY");
    CHECK_SPECTATOR_FIELD(g_GameManager.arcadeRegionSize.x, cp.arcadeSizeX, "arcadeSizeX");
    CHECK_SPECTATOR_FIELD(g_GameManager.arcadeRegionSize.y, cp.arcadeSizeY, "arcadeSizeY");
    CHECK_SPECTATOR_FIELD(g_GameManager.playerMovementAreaTopLeftPos.x, cp.movementTopX, "movementTopX");
    CHECK_SPECTATOR_FIELD(g_GameManager.playerMovementAreaTopLeftPos.y, cp.movementTopY, "movementTopY");
    CHECK_SPECTATOR_FIELD(g_GameManager.playerMovementAreaSize.x, cp.movementSizeX, "movementSizeX");
    CHECK_SPECTATOR_FIELD(g_GameManager.playerMovementAreaSize.y, cp.movementSizeY, "movementSizeY");
    CHECK_SPECTATOR_FIELD(g_GameManager.csumFloat, cp.csumFloat, "csumFloat");
    CHECK_SPECTATOR_FIELD(g_GameManager.cherryMax, cp.cherryMax, "cherryMax");
    CHECK_SPECTATOR_FIELD(g_GameManager.cherry, cp.cherry, "cherry");
    CHECK_SPECTATOR_FIELD(g_GameManager.cherryPlus, cp.cherryPlus, "cherryPlus");
    CHECK_SPECTATOR_FIELD(g_GameManager.bulletLagTime, cp.bulletLagTime, "bulletLagTime");
    CHECK_SPECTATOR_FIELD(g_GameManager.rank.rank, cp.rank, "rank");
    CHECK_SPECTATOR_FIELD(g_GameManager.rank.maxRank, cp.maxRank, "maxRank");
    CHECK_SPECTATOR_FIELD(g_GameManager.rank.minRank, cp.minRank, "minRank");
    CHECK_SPECTATOR_FIELD(g_GameManager.subrank, cp.subrank, "subrank");
    const ZunGlobals &globals = *g_GameManager.globals;
    CHECK_SPECTATOR_FIELD(globals.guiScore, cp.guiScore, "guiScore");
    CHECK_SPECTATOR_FIELD(globals.score, cp.score, "score");
    CHECK_SPECTATOR_FIELD(globals.guiScoreDifference, cp.guiScoreDifference, "guiScoreDifference");
    CHECK_SPECTATOR_FIELD(globals.highScore, cp.highScore, "highScore");
    CHECK_SPECTATOR_FIELD(globals.grazeInStage, cp.grazeInStage, "grazeInStage");
    CHECK_SPECTATOR_FIELD(globals.grazeInTotal, cp.grazeInTotal, "grazeInTotal");
    CHECK_SPECTATOR_FIELD(globals.spellCardsCaptured, cp.spellCardsCaptured, "spellCardsCaptured");
    CHECK_SPECTATOR_FIELD(globals.numRetries, cp.numRetries, "numRetries");
    CHECK_SPECTATOR_FIELD(globals.pointItemsCollectedThisStage, cp.pointItemsThisStage, "pointItemsThisStage");
    CHECK_SPECTATOR_FIELD(globals.pointItemsCollectedForExtend, cp.pointItemsForExtend, "pointItemsForExtend");
    CHECK_SPECTATOR_FIELD(globals.extendsFromPointItems, cp.extendsFromPointItems, "extendsFromPointItems");
    CHECK_SPECTATOR_FIELD(globals.nextNeededPointItemsForExtend, cp.nextNeededPointItems, "nextNeededPointItems");
    for (std::size_t i = 0; i < cp.globalsRng1.size(); ++i)
        CHECK_SPECTATOR_FIELD(globals.rng1[i], cp.globalsRng1[i], "globals.rng1");
    CHECK_SPECTATOR_FIELD(globals.deaths, cp.deaths, "deaths");
    for (std::size_t i = 0; i < cp.globalsRngFloat1.size(); ++i)
        CHECK_SPECTATOR_FIELD(globals.rngFloat1[i], cp.globalsRngFloat1[i], "globals.rngFloat1");
    CHECK_SPECTATOR_FIELD(globals.livesRemaining, cp.livesRemaining, "globals.livesRemaining");
    for (std::size_t i = 0; i < cp.globalsRngFloat2.size(); ++i)
        CHECK_SPECTATOR_FIELD(globals.rngFloat2[i], cp.globalsRngFloat2[i], "globals.rngFloat2");
    CHECK_SPECTATOR_FIELD(globals.bombsRemaining, cp.bombsRemaining, "globals.bombsRemaining");
    CHECK_SPECTATOR_FIELD(globals.bombsUsed, cp.bombsUsed, "globals.bombsUsed");
    for (std::size_t i = 0; i < cp.globalsRngFloat3.size(); ++i)
        CHECK_SPECTATOR_FIELD(globals.rngFloat3[i], cp.globalsRngFloat3[i], "globals.rngFloat3");
    CHECK_SPECTATOR_FIELD(globals.currentPower, cp.currentPower, "globals.currentPower");
    for (std::size_t i = 0; i < cp.globalsRngFloat4.size(); ++i)
        CHECK_SPECTATOR_FIELD(globals.rngFloat4[i], cp.globalsRngFloat4[i], "globals.rngFloat4");
    CHECK_SPECTATOR_FIELD(globals.cherryStart, cp.cherryStart, "globals.cherryStart");
    for (std::size_t i = 0; i < cp.globalsRng2.size(); ++i)
        CHECK_SPECTATOR_FIELD(globals.rng2[i], cp.globalsRng2[i], "globals.rng2");
    CHECK_SPECTATOR_FIELD(globals.curCsum, cp.curCsum, "globals.curCsum");
    CHECK_SPECTATOR_FIELD(globals.csumAsSum, cp.csumAsSum, "globals.csumAsSum");
    for (std::size_t i = 0; i < cp.csumData.size(); ++i)
        CHECK_SPECTATOR_FIELD(globals.csumData[i], cp.csumData[i], "globals.csumData");
    for (std::size_t i = 0; i < cp.globalEclInts.size(); ++i)
    {
        CHECK_SPECTATOR_FIELD(g_GlobalEclVars.intVars[i], cp.globalEclInts[i], "globalEcl.intVars");
        CHECK_SPECTATOR_FIELD(g_GlobalEclVars.floatVars[i], cp.globalEclFloats[i], "globalEcl.floatVars");
    }
    std::printf("\n");
#undef CHECK_SPECTATOR_FIELD
}

bool RestoreSpectatorCheckpoint(const SpectatorStageEvent &event)
{
    std::printf("netplay spectator TH07: restore begin frame=%u bytes=%u nativeReady=%d\n",
                event.frame, static_cast<unsigned>(event.state.size()),
                g_SpectatorNativeStageReady ? 1 : 0);
    SpectatorCheckpoint cp;
    if (event.game != '7' || event.schema != SPECTATOR_CHECKPOINT_SCHEMA ||
        !DecodeSpectatorCheckpoint(event.state, &cp) || event.stage != cp.stage)
        return false;
    if (!g_GameManager.globals)
        return false;
    std::printf("netplay spectator TH07: source/rebuild target stage=%u sourcePos=(%.3f,%.3f,%.3f)/(%.3f,%.3f,%.3f) sourceBirth=%d/%d/%.3f state=%d/%d stageClock=%d/%d/%d enemyClock=%d\n",
                cp.stage, cp.playerPositions[0], cp.playerPositions[1], cp.playerPositions[2],
                cp.playerPositions[6], cp.playerPositions[7], cp.playerPositions[8],
                cp.playerInvulnerabilityCurrent[0], cp.playerInvulnerabilityPrevious[0],
                cp.playerInvulnerabilitySubFrame[0], cp.playerState[0], cp.playerState[1],
                g_Stage.scriptTime.current, g_Stage.stageFrameCounter, g_Stage.instructionIndex,
                g_EnemyManager.timelineTime.current);
    if (!g_SpectatorNativeStageReady && cp.stage == 1 &&
        g_GameManager.currentStage == static_cast<i32>(cp.stage) &&
        g_GameManager.notInMenu)
    {
        // The independent viewer enters the ordinary native Stage 1 path
        // before it can receive a relay bootstrap. Reuse that freshly
        // constructed stage for the frame-zero initializer; later-stage
        // checkpoints below always rebuild the native chain.
        if (!ApplySpectatorCheckpointState(cp))
            return false;
        const auto actual = Th07CanonicalHash::Capture();
        if (actual.composite != cp.expectedHash)
        {
            LogSpectatorCheckpointMetaDifferences(cp);
            std::printf("netplay spectator TH07: initial checkpoint mismatch frame=%u expected=%016llx actual=%016llx expectedMeta=%016llx actualMeta=%016llx submeta=%016llx/%016llx/%016llx/%016llx actual=%016llx/%016llx/%016llx/%016llx expectedStage=%016llx actualStage=%016llx expectedPlayers=%016llx/%016llx/%016llx actualPlayers=%016llx/%016llx/%016llx\n",
                        event.frame, static_cast<unsigned long long>(cp.expectedHash),
                        static_cast<unsigned long long>(actual.composite),
                        static_cast<unsigned long long>(cp.expectedMeta),
                        static_cast<unsigned long long>(actual.meta),
                        static_cast<unsigned long long>(cp.expectedMetaRng),
                        static_cast<unsigned long long>(cp.expectedMetaGame),
                        static_cast<unsigned long long>(cp.expectedMetaInput),
                        static_cast<unsigned long long>(cp.expectedMetaSupervisor),
                        static_cast<unsigned long long>(actual.metaRng),
                        static_cast<unsigned long long>(actual.metaGame),
                        static_cast<unsigned long long>(actual.metaInput),
                        static_cast<unsigned long long>(actual.metaSupervisor),
                        static_cast<unsigned long long>(cp.expectedStageHash),
                        static_cast<unsigned long long>(actual.stage),
                        static_cast<unsigned long long>(cp.expectedPlayer0),
                        static_cast<unsigned long long>(cp.expectedPlayer1),
                        static_cast<unsigned long long>(cp.expectedPlayer2),
                        static_cast<unsigned long long>(actual.player0),
                        static_cast<unsigned long long>(actual.player1),
                        static_cast<unsigned long long>(actual.player2));
            return false;
        }
        g_SpectatorNativeStageReady = true;
        g_SimFrame = event.frame;
        ReportSpectatorCheckpointHash(event.frame, actual.composite);
        std::printf("netplay spectator TH07: checkpoint restored frame=%u stage=%u hash=%016llx\n",
                    event.frame, cp.stage, static_cast<unsigned long long>(actual.composite));
        return true;
    }
    SideEffects::SetSpeculative(true);
    std::printf("netplay spectator TH07: restore cut begin stage=%u current=%d\n",
                cp.stage, g_GameManager.currentStage);
    // The chain's synchronous DeletedCallbacks use IsInitialStageLoad() to
    // decide whether stage-lifetime GUI/ANM state is destroyed. Match the
    // normal Supervisor transition path before cutting the current stage so
    // GuiImpl and its transition resources survive into the recreated stage.
    const i32 previousSupervisorState = g_Supervisor.curState;
    g_Supervisor.curState = SUPERVISOR_STATE_NEXT_STAGE;
    std::printf("netplay spectator TH07: restore cut lifecycle state %d->%d\n",
                previousSupervisorState, g_Supervisor.curState);
    GameManager::CutChain();
    std::printf("netplay spectator TH07: restore cut done\n");
    if (!MultiplayerGameplay::Configure(cp.session))
    {
        SideEffects::SetSpeculative(false);
        return false;
    }
    for (std::uint8_t player = 0; player < TH07_MULTI_MAX_PLAYERS; ++player)
    {
        SetPlayerLives(player, cp.lives[player]); SetPlayerBombs(player, cp.bombs[player]);
        SetPlayerPower(player, cp.powers[player]);
        g_MultiplayerContributionStats[player].enemiesDefeated = cp.kills[player];
        g_MultiplayerContributionStats[player].damageDealt = cp.damage[player];
        g_MultiplayerContributionStats[player].challengeDeaths = cp.challengeDeaths[player];
        g_Players[player].playerState = static_cast<i8>(cp.playerState[player]);
    }
    g_GameManager.difficulty = cp.difficulty; g_GameManager.character = cp.character;
    g_GameManager.shotType = cp.shotType; g_GameManager.practice = 0; g_GameManager.demo = 0;
    g_GameManager.SetIsReplay(0); g_GameManager.currentStage = static_cast<i32>(cp.stage) - 1;
    g_GameManager.stageRngSeed = cp.stageRngSeed;
    if (g_GameManager.defaultCfg)
    {
        g_GameManager.defaultCfg->lifeCount = 2;
        g_GameManager.defaultCfg->slowMode = 0;
    }
    g_Rng.seed = cp.stageRngSeed;
    g_Supervisor.curState = SUPERVISOR_STATE_NEXT_STAGE;
    g_Supervisor.wantedState = SUPERVISOR_STATE_GAMEMANAGER;
    std::printf("netplay spectator TH07: restore register begin target=%d rng=%u\n",
                g_GameManager.currentStage, static_cast<unsigned>(g_Rng.seed));
    const bool constructed = GameManager::RegisterChain() == ZUN_SUCCESS;
    std::printf("netplay spectator TH07: restore register done result=%d stage=%d\n",
                constructed ? 1 : 0, g_GameManager.currentStage);
    SideEffects::SetSpeculative(false);
    if (!constructed)
        return false;
    LogSpectatorCheckpointCanonical("restore-register-preapply", event.frame,
                                    Th07CanonicalHash::Capture());
    g_SpectatorNativeStageReady = true;
    if (!ApplySpectatorCheckpointState(cp))
        return false;
    const auto actual = Th07CanonicalHash::Capture();
    LogSpectatorCheckpointCanonical("restore-cp-preinput", event.frame, actual);
    if (actual.composite != cp.expectedHash)
    {
        LogSpectatorCheckpointMetaDifferences(cp);
        std::printf("netplay spectator TH07: checkpoint hash mismatch frame=%u stage=%u expected=%016llx actual=%016llx\n",
                    event.frame, cp.stage, static_cast<unsigned long long>(cp.expectedHash),
                    static_cast<unsigned long long>(actual.composite));
        std::printf("netplay spectator TH07: checkpoint components expected meta=%016llx rng=%016llx game=%016llx input=%016llx supervisor=%016llx stage=%016llx players=%016llx/%016llx/%016llx; actual meta=%016llx rng=%016llx game=%016llx input=%016llx supervisor=%016llx multiplayer=%016llx stage=%016llx players=%016llx/%016llx/%016llx enemies=%016llx bullets=%016llx items=%016llx\n",
                    static_cast<unsigned long long>(cp.expectedMeta),
                    static_cast<unsigned long long>(cp.expectedMetaRng),
                    static_cast<unsigned long long>(cp.expectedMetaGame),
                    static_cast<unsigned long long>(cp.expectedMetaInput),
                    static_cast<unsigned long long>(cp.expectedMetaSupervisor),
                    static_cast<unsigned long long>(cp.expectedStageHash),
                    static_cast<unsigned long long>(cp.expectedPlayer0),
                    static_cast<unsigned long long>(cp.expectedPlayer1),
                    static_cast<unsigned long long>(cp.expectedPlayer2),
                    static_cast<unsigned long long>(actual.meta),
                    static_cast<unsigned long long>(actual.metaRng),
                    static_cast<unsigned long long>(actual.metaGame),
                    static_cast<unsigned long long>(actual.metaInput),
                    static_cast<unsigned long long>(actual.metaSupervisor),
                    static_cast<unsigned long long>(actual.multiplayer),
                    static_cast<unsigned long long>(actual.stage),
                    static_cast<unsigned long long>(actual.player0),
                    static_cast<unsigned long long>(actual.player1),
                    static_cast<unsigned long long>(actual.player2),
                    static_cast<unsigned long long>(actual.enemies),
                    static_cast<unsigned long long>(actual.bullets),
                    static_cast<unsigned long long>(actual.items));
        return false;
    }
    g_SimFrame = event.frame;
    ReportSpectatorCheckpointHash(event.frame, actual.composite);
    std::printf("netplay spectator TH07: checkpoint restored frame=%u stage=%u hash=%016llx\n",
                    event.frame, cp.stage,
                    static_cast<unsigned long long>(actual.composite));
    return true;
}

bool TryPendingSpectatorBootstrap()
{
    if (!g_HavePendingSpectatorBootstrap) return true;
    const auto received = g_SpectatorPlayback.History().Received();
    if (received > g_PendingSpectatorBootstrap.frame) return false;
    if (!g_SpectatorPlayback.History().BootstrapMatches(g_PendingSpectatorBootstrap)) return true;
    const bool accepted = g_SpectatorPlayback.Receive(g_PendingSpectatorBootstrap);
    g_HavePendingSpectatorBootstrap = false;
    return accepted;
}

bool UseRestartCycle()
{
#ifdef __EMSCRIPTEN__
    return EM_ASM_INT({ return Module.eaglerOptions?.netplayRestartCycle ? 1 : 0; }) != 0;
#else
    return false;
#endif
}

bool UseReplayPlaybackCycle()
{
#ifdef __EMSCRIPTEN__
    return EM_ASM_INT({
        return Module.eaglerOptions?.netplayReplayPlaybackCycle ? 1 : 0;
    }) != 0;
#else
    return false;
#endif
}

bool DetailedCanonicalTelemetry()
{
#ifdef __EMSCRIPTEN__
    if (ProbeMode())
        return true;
    return EM_ASM_INT({ return Module.eaglerOptions?.netplayScriptedInput ? 1 : 0; }) != 0;
#else
    return false;
#endif
}

void RetireGameplaySession()
{
    SetReconcileTimerMode(false);
    ClearTransientModes();
    Th07Rollback::Clear();
    if (g_SpectatorMode)
    {
        // Spectator admission is scoped to the current relay run. A later
        // room start gets a new startSerial and a fresh Runtime from the
        // Launcher, so never reuse this old read-only socket for generation 1.
        g_SpectatorRunRetired = true;
        if (ProductionLanMode() && g_ProductionTransportStarted)
        {
            g_BrowserPeerTransport.Close();
            g_ProductionTransportStarted = false;
        }
    }
    g_Initialized = false;
    g_Active = false;
    g_Done = false;
    g_ReconcileActive = g_ReconcileShouldYield = false;
    g_ReconcileNextFrame = g_ReconcileLastFrame = INVALID_FRAME;
    g_SimFrame = 0;
    g_DriverTicks = 0;
    g_Sequence = 1;
    g_LastReceivedSequence = 0;
    g_LastHelloSendTick = 0;
    g_LastReadySendTick = 0;
    g_ReliableInputRepair = false;
    g_InputRepairBudgets = {};
    ++g_SessionGeneration;
#ifdef __EMSCRIPTEN__
    if (ProductionLanMode())
    {
        EM_ASM({
            globalThis.__eaglerNetplayLanActive = false;
            globalThis.__eaglerNetplayLanFrame = 0;
            globalThis.__eaglerNetplayLanGeneration = $0;
        }, g_SessionGeneration);
    }
#endif
}

bool TransportConnect(const char *url)
{
    if (ProductionLanMode())
        return g_BrowserPeerTransport.Connect(url, g_LocalPlayer, g_PlayerCount, true);
    return g_Transport.Connect(url);
}

bool ReadSpectatorId(char *out, std::size_t capacity)
{
#ifdef __EMSCRIPTEN__
    if (!out || capacity == 0)
        return false;
    EM_ASM({ stringToUTF8(Module.eaglerOptions?.netplaySpectatorId || "", $0, $1); }, out, capacity);
    return out[0] != '\0';
#else
    (void)out; (void)capacity;
    return false;
#endif
}

bool SpectatorModeRequested()
{
#ifdef __EMSCRIPTEN__
    return ProductionLanMode() &&
           EM_ASM_INT({ return Module.eaglerOptions?.netplaySpectator ? 1 : 0; }) != 0;
#else
    return false;
#endif
}

bool TransportIsOpen()
{
    return ProductionLanMode() ? g_BrowserPeerTransport.IsOpen() : g_Transport.IsOpen();
}

bool TransportFailed()
{
    return ProductionLanMode() ? g_BrowserPeerTransport.Failed() : g_Transport.Failed();
}

bool TransportSend(const std::uint8_t *data, std::size_t size, bool control = false)
{
    if (!ProductionLanMode())
        return g_Transport.Send(data, size);
    return control ? g_BrowserPeerTransport.SendControl(data, size)
                   : g_BrowserPeerTransport.Send(data, size);
}

bool TransportSendTo(std::uint8_t peer, const std::uint8_t *data, std::size_t size)
{
    return ProductionLanMode() && g_BrowserPeerTransport.SendTo(peer, data, size);
}

bool TransportPoll(std::vector<std::uint8_t> *packet)
{
    return ProductionLanMode() ? g_BrowserPeerTransport.Poll(packet) : g_Transport.Poll(packet);
}

std::size_t TransportBufferedAmount()
{
    return ProductionLanMode() ? g_BrowserPeerTransport.BufferedAmount() : g_Transport.BufferedAmount();
}

const std::string &TransportLastError()
{
    return ProductionLanMode() ? g_BrowserPeerTransport.LastError() : g_Transport.LastError();
}

bool ProbeMode()
{
#ifdef __EMSCRIPTEN__
    return EM_ASM_INT({
        return Module.eaglerOptions?.debugHarness === 'netplay-lan-stage1' ? 1 : 0;
    }) != 0;
#else
    return false;
#endif
}

void RecordTimeSyncSample(const InputPacket &packet)
{
    // GGPO/GGRS maintain time-sync state per endpoint.  A 3P room must not
    // average two unrelated links into one sample stream: update the sender's
    // window, then use the largest local lead as the room wait recommendation.
    if (!ProductionLanMode() || packet.senderFrame == INVALID_FRAME ||
        packet.senderPlayer >= g_PlayerCount || packet.senderPlayer == g_LocalPlayer)
        return;

    if (g_DisableTimeSyncPacing)
    {
        g_SimulationIntervalScale = 1.0;
        return;
    }

    std::uint32_t &lastRemoteFrame = g_LastRemoteSenderFrame[packet.senderPlayer];
    if (lastRemoteFrame != INVALID_FRAME && packet.senderFrame <= lastRemoteFrame)
        return;
    lastRemoteFrame = packet.senderFrame;

    const std::int32_t inferredLead = FramePacingPolicy::InferLead(
        g_SimFrame, packet.senderFrame, packet.frameAdvantage);
    if (!FramePacingPolicy::AcceptLead(inferredLead))
        return;

    FrameAdvantageWindow &state = g_PeerTimeSync[packet.senderPlayer];
    if (!state.AddSample(inferredLead))
        return;

    bool haveRecommendation = false;
    double recommendedLead = 0.0;
    for (std::uint8_t player = 0; player < g_PlayerCount; ++player)
    {
        if (player == g_LocalPlayer || !g_PeerTimeSync[player].ready)
            continue;
        if (!haveRecommendation || g_PeerTimeSync[player].averageLead > recommendedLead)
            recommendedLead = g_PeerTimeSync[player].averageLead;
        haveRecommendation = true;
    }
    if (!haveRecommendation)
        return;
    g_RecommendedLead = recommendedLead;

    g_SimulationIntervalScale =
        FramePacingPolicy::UpdateScale(g_SimulationIntervalScale, recommendedLead);
#ifdef __EMSCRIPTEN__
    EM_ASM({
        globalThis.__eaglerNetplayLanFrameAdvantage = $0;
        globalThis.__eaglerNetplayLanPacingScale = $1;
        globalThis.__eaglerNetplayLanPeerAdvantages ||= [];
        globalThis.__eaglerNetplayLanPeerAdvantages[$2] = $3;
    }, recommendedLead, g_SimulationIntervalScale,
       static_cast<int>(packet.senderPlayer), state.averageLead);
#endif
}

bool ProductionLanMode()
{
#ifdef __EMSCRIPTEN__
    return EM_ASM_INT({
        return Module.eaglerOptions?.netplayMode === 'lan' ? 1 : 0;
    }) != 0;
#else
    return false;
#endif
}

bool RequestedInternal()
{
    return ProbeMode() || ProductionLanMode();
}

bool UseStageTransitionTest()
{
#ifdef __EMSCRIPTEN__
    return EM_ASM_INT({ return Module.eaglerOptions?.netplayStageTransition ? 1 : 0; }) != 0;
#else
    return false;
#endif
}

bool RollbackAuditEnabled()
{
#ifdef __EMSCRIPTEN__
    return EM_ASM_INT({ return Module.eaglerOptions?.netplayRollbackAudit ? 1 : 0; }) != 0;
#else
    return false;
#endif
}

int SpectatorSizeProbeFrame()
{
#ifdef __EMSCRIPTEN__
    return EM_ASM_INT({
        const value = Number(Module.eaglerOptions?.netplaySpectatorSizeProbeFrame);
        return Number.isInteger(value) && value >= 0 && value < 0x7fffffff ? value : -1;
    });
#else
    return -1;
#endif
}

bool CaptureConfirmedReplayAuditFrames()
{
#if defined(TH_DEV_TOOLS) && defined(TH_ENABLE_MULTIPLAYER_GAMEPLAY)
    if (!UseReplayPlaybackCycle() || g_SimFrame == 0)
        return true;
    const std::uint32_t confirmed = g_Core.ConfirmedThroughAllRemotes();
    const std::uint32_t lastSimulated = g_SimFrame - 1;
    const std::uint32_t lastAvailable = std::min(confirmed, lastSimulated);
    while (g_NextReplayAuditFrame <= lastAvailable)
    {
        const ReplayFrameBinding &binding =
            g_ReplayFrameBindings[g_NextReplayAuditFrame % g_ReplayFrameBindings.size()];
        const FrameDecision authoritative = g_Core.PrepareFrame(g_NextReplayAuditFrame);
        if (binding.simFrame != g_NextReplayAuditFrame || binding.stage < 0 ||
            binding.replayFrame < 0 || !authoritative.canAdvance ||
            authoritative.predictedMask != 0)
            return false;
        ReplayExtension::DebugExpectMultiplayerPlaybackFrame(
            binding.stage, binding.replayFrame,
            authoritative.inputs.data(), g_PlayerCount);
        ++g_NextReplayAuditFrame;
    }
#endif
    return true;
}

void PublishConfirmedSpectatorFrames()
{
#ifdef __EMSCRIPTEN__
    if (ProductionLanMode())
        EM_ASM({
            const state = globalThis.__eaglerNetplaySpectatorPublish ||= {};
            state.cursor = $0 >>> 0;
            state.simFrame = $1 >>> 0;
            state.confirmed = $2 >>> 0;
            state.lastAttemptFrame = $0 >>> 0;
        }, g_NextSpectatorPublishFrame, g_SimFrame,
            g_Core.ConfirmedThroughAllRemotes());
#endif
    if (!g_SpectatorPublisherReady || g_SpectatorMode || g_LocalPlayer != 0 || g_SimFrame == 0)
        return;
    SpectatorStagePublisher::PublishFailure failure =
        SpectatorStagePublisher::PublishFailure::None;
    const bool sent = g_SpectatorPublisher.Publish(
        g_Core.ConfirmedThroughAllRemotes(), g_SimFrame - 1,
        [](std::uint32_t frame, std::array<FrameInput, MAX_PLAYERS> &inputs) {
            const auto decision = g_Core.PrepareFrame(frame);
            if (!decision.canAdvance || decision.predictedMask != 0)
                return false;
            inputs = decision.inputs;
            return true;
        },
        [](const std::vector<std::uint8_t> &wire) {
#ifdef __EMSCRIPTEN__
            EM_ASM({
                const state = globalThis.__eaglerNetplaySpectatorPublish ||= {};
                state.lastWireBytes = $0 >>> 0;
                state.maxWireBytes = Math.max(state.maxWireBytes || 0, $0 >>> 0);
                state.lastSendSucceeded = false;
            }, static_cast<unsigned>(wire.size()));
#endif
            const bool result = g_BrowserPeerTransport.SendSpectator(wire.data(), wire.size());
#ifdef __EMSCRIPTEN__
            EM_ASM({
                const state = globalThis.__eaglerNetplaySpectatorPublish ||= {};
                state.lastSendSucceeded = !!$0;
                state.sendAttempts = (state.sendAttempts || 0) + 1;
                if ($0) state.sendSuccesses = (state.sendSuccesses || 0) + 1;
            }, result ? 1 : 0);
#endif
            return result;
        }, &failure);
    g_NextSpectatorPublishFrame = g_SpectatorPublisher.Published();
#ifdef __EMSCRIPTEN__
    EM_ASM({
        const state = globalThis.__eaglerNetplaySpectatorPublish ||= {};
        state.publisherReady = !!$0;
        state.lastPublishOk = !!$1;
        state.lastPublishCursor = $2 >>> 0;
        state.lastPublishFailure = UTF8ToString($3);
        state.lastPublishFailureFrame = state.lastPublishFailure === "none" ? -1 : ($2 >>> 0);
        state.lastPublishPermanent = !!$4;
    }, g_SpectatorPublisherReady ? 1 : 0, sent ? 1 : 0,
       g_NextSpectatorPublishFrame, SpectatorStagePublisher::FailureName(failure), 0);
#endif
    if (!sent)
    {
        const char *reason = SpectatorStagePublisher::FailureName(failure);
        const bool permanent = failure == SpectatorStagePublisher::PublishFailure::InvalidPublisher ||
            failure == SpectatorStagePublisher::PublishFailure::MissingRecord ||
            failure == SpectatorStagePublisher::PublishFailure::StageCheckpointMissing ||
            failure == SpectatorStagePublisher::PublishFailure::CheckpointEncode ||
            failure == SpectatorStagePublisher::PublishFailure::InputUnavailable ||
            failure == SpectatorStagePublisher::PublishFailure::InputEncode ||
            failure == SpectatorStagePublisher::PublishFailure::BossNodeEncode;
        std::printf("netplay spectator TH07: publish %s frame=%u permanent=%d cursor=%u confirmed=%u simFrame=%u\n",
                    reason, g_NextSpectatorPublishFrame, permanent ? 1 : 0,
                    g_NextSpectatorPublishFrame, g_Core.ConfirmedThroughAllRemotes(), g_SimFrame);
#ifdef __EMSCRIPTEN__
        EM_ASM({
            const state = globalThis.__eaglerNetplaySpectatorPublish ||= {};
            state.lastPublishFailure = UTF8ToString($1);
            state.lastPublishFailureFrame = $0 >>> 0;
            state.lastPublishPermanent = !!$2;
        }, g_NextSpectatorPublishFrame, reason, permanent ? 1 : 0);
#endif
        if (permanent)
        {
            DisableSpectatorPublisher(g_NextSpectatorPublishFrame, reason);
        }
    }
}

void DisableSpectatorPublisher(std::uint32_t frame, const char *reason)
{
    // This is an optional P0 publisher lane. Structural state/capture failures
    // retire only spectator delivery; the player rollback transport continues.
    if (!g_SpectatorPublisherEligible || g_SpectatorLaneStopped || g_SpectatorMode ||
        g_LocalPlayer != 0 || !ProductionLanMode())
        return;
    g_SpectatorPublisherReady = false;
    g_SpectatorLaneStopped = true;
    if (!reason) reason = "unknown";
    g_BrowserPeerTransport.StopSpectators(false);
    std::printf("netplay spectator TH07: publisher stopped frame=%u reason=%s\n",
                frame, reason);
#ifdef __EMSCRIPTEN__
    EM_ASM({
        const state = globalThis.__eaglerNetplaySpectatorPublish ||= {};
        state.publisherReady = false;
        state.failureFrame = $0 >>> 0;
        state.failureReason = UTF8ToString($1);
        globalThis.__eaglerNetplaySpectatorPublishFailed = $0 >>> 0;
    }, frame, reason);
#endif
}

bool DrainSpectatorFrames()
{
    std::vector<std::uint8_t> wire;
    while (TransportPoll(&wire))
    {
        if (IsSpectatorStageEvent(wire.data(), wire.size()))
        {
            SpectatorStageEvent event;
            if (!DecodeSpectatorStageEvent(wire.data(), wire.size(), &event))
                return false;
            if (event.kind == SpectatorEventKind::Bootstrap)
            {
                if (g_HavePendingSpectatorBootstrap)
                    return false;
                g_PendingSpectatorBootstrap = std::move(event);
                g_HavePendingSpectatorBootstrap = true;
            }
            else if (!g_SpectatorPlayback.Receive(event))
                return false;
        }
        else
        {
            SpectatorFramePacket packet;
            if (!DecodeSpectatorFramePacket(wire.data(), wire.size(), &packet) ||
                !g_SpectatorPlayback.Receive(packet))
                return false;
            ++g_NextSpectatorReceiveFrame;
        }
        if (!TryPendingSpectatorBootstrap())
            return false;
    }
    return true;
}

const char *SpectatorStageLabel(std::uint32_t stage)
{
    switch (stage)
    {
    case 1: return "Stage 1";
    case 2: return "Stage 2";
    case 3: return "Stage 3";
    case 4: return "Stage 4";
    case 5: return "Stage 5";
    case 6: return "Stage 6";
    case 7: return "Extra";
    case 8: return "Phantasm";
    default: return nullptr;
    }
}

const char *SpectatorState()
{
    return g_SpectatorView.State(g_SpectatorPlayback, SpectatorStageLabel);
}

int SpectatorControl(const char *run, const char *action, std::uint32_t frame)
{
    return g_SpectatorView.Control(g_SpectatorPlayback, run, action, frame) ? 1 : 0;
}

void PublishPeerDiagnostics(std::uint8_t predictedMask)
{
#ifdef __EMSCRIPTEN__
    if (!ProductionLanMode())
        return;
    for (std::uint8_t player = 0; player < g_PlayerCount; ++player)
    {
        if (player == g_LocalPlayer)
            continue;
        const std::uint32_t confirmed = g_Core.ConfirmedThrough(player);
        const std::uint32_t gap = confirmed == INVALID_FRAME
                                      ? g_SimFrame + 1
                                      : confirmed < g_SimFrame ? g_SimFrame - confirmed : 0;
        g_PredictionDepth[player] =
            (predictedMask & static_cast<std::uint8_t>(1u << player)) != 0 ? gap : 0;
        EM_ASM({
            globalThis.__eaglerNetplayLanPeers ||= [];
            const entry = globalThis.__eaglerNetplayLanPeers[$0] ||= {};
            entry.player = $0;
            entry.confirmed = $1 >>> 0;
            entry.gap = $2 >>> 0;
            entry.predicted = $3 >>> 0;
            entry.rollbacks = $4 >>> 0;
        }, static_cast<int>(player), confirmed, gap,
           g_PredictionDepth[player], g_RollbackByPlayer[player]);
    }
#else
    (void)predictedMask;
#endif
}

std::uint8_t FirstRemotePlayer()
{
    for (std::uint8_t player = 0; player < g_PlayerCount; ++player)
        if (player != g_LocalPlayer)
            return player;
    return 0xff;
}

std::uint8_t ReadPlayerCount()
{
#ifdef __EMSCRIPTEN__
    const int value = EM_ASM_INT({
        const count = Number(Module.eaglerOptions?.netplayPlayerCount ?? 2);
        return count === 3 ? 3 : 2;
    });
    return static_cast<std::uint8_t>(value);
#else
    return 2;
#endif
}

std::uint32_t ReadTestFrames()
{
#ifdef __EMSCRIPTEN__
    const int value = EM_ASM_INT({
        const value = Number(Module.eaglerOptions?.netplayTestFrames ?? 300);
        const maxFrames = Module.eaglerOptions?.netplayStageTransition ? 24000 : 12000;
        return Number.isInteger(value) && value >= 60 && value <= maxFrames ? value : 300;
    });
    return static_cast<std::uint32_t>(value);
#else
    return DEFAULT_TEST_FRAMES;
#endif
}

FrameInput CaptureLocalInput(std::uint32_t frame)
{
    // Deterministic restart smoke uses the real production BrowserPeerTransport
    // but scripted gameplay/menu edges so both browser instances exercise the
    // same Pause -> Reset lifecycle without host-window focus races.
    if (UseRestartCycle())
        return FrameInput(LocalScript(g_LocalPlayer, frame));
    if (!UsePhysicalInput())
    {
        FrameInput input(LocalScript(g_LocalPlayer, frame));
        // Controlled test-only traces. Production touch samples are unmodified.
        if (g_TouchWorkload != 0)
        {
            input.analogMode = AnalogMode::DirectTouch;
            input.touchUsed = true;
            const auto phase = (frame + g_LocalPlayer * 13u) % 300u;
            input.x = g_LocalPlayer == 0 ? 0.5f : -0.5f;
            input.y = 0.125f;
            if (g_TouchWorkload == 2)
            {
                input.x += (static_cast<int>(phase % 17) - 8) * 0.015625f;
                input.y = (static_cast<int>(phase % 11) - 5) * 0.03125f;
            }
            if (g_TouchWorkload == 3 && phase >= 120)
                input.x = input.y = 0.0f;
            if (g_TouchWorkload == 4 && phase >= 150)
            {
                input.x = -input.x;
                input.y = -input.y;
            }
            if (g_TouchWorkload == 5)
            {
                input.x = phase % 3 == 0 ? input.x * 3 : 0.0f;
                input.y = phase % 3 == 0 ? input.y * 3 : 0.0f;
            }
            if (g_TestIncrementalTouch)
            {
                input.analogMode = frame % 300 == 0 ? AnalogMode::DirectTouchBegin : AnalogMode::DirectTouchDelta;
                if (g_TouchWorkload == 3 && phase >= 120) input.analogMode = AnalogMode::None;
            }
            return input;
        }
        if (UseReplayPlaybackCycle())
        {
            // Match TH06's hidden Replay profile and cover the complete input
            // record while keeping Stage 1 movement deliberately small.
            const std::uint32_t phase = (frame / 20u + g_LocalPlayer) % 3u;
            if (phase == 1u)
            {
                input.analogMode = AnalogMode::Joystick;
                input.x = g_LocalPlayer == 0 ? 0.25f : -0.25f;
                input.y = (frame & 1u) != 0 ? 0.125f : -0.125f;
                input.touchUsed = true;
            }
            else if (phase == 2u)
            {
                input.analogMode = AnalogMode::DirectTouch;
                if (g_TestIncrementalTouch)
                    input.analogMode = frame % 20 == 0 ? AnalogMode::DirectTouchBegin : AnalogMode::DirectTouchDelta;
                input.x = g_LocalPlayer == 0 ? 0.5f : -0.5f;
                input.y = (frame & 1u) != 0 ? 0.25f : -0.25f;
                input.unlimited = ((frame / 20u) & 1u) != 0;
                input.touchUsed = true;
            }
            input.touchBomb = frame == 173u + g_LocalPlayer;
        }
        return input;
    }

    // Sample the local browser/controller exactly once when this logical
    // frame is first scheduled. The simulation pass below replays a
    // deterministic aggregate raw word, so Supervisor never consumes the
    // device a second time for prediction or rollback resimulation.
    Input::ClearReplayOverride();
    Input::BeginCapture();
    (void)Controller::GetInput();
    float x = 0.0f;
    float y = 0.0f;
    bool beginGesture = false;
    if (Touch::GetFreeJoystickVector(&x, &y))
        Input::CaptureJoystick(x, y);
    else if (Touch::TakePlayerDelta(&x, &y, &beginGesture))
    {
        if (beginGesture)
            g_TouchQuantResidualX = g_TouchQuantResidualY = 0.0f;
        if (g_TouchQuantizationStep > 0.0f)
        {
            const auto quantize = [](float value, float step, float &residual) {
                const float total = value + residual;
                const float quantized = std::round(total / step) * step;
                residual = total - quantized;
                return quantized;
            };
            x = quantize(x, g_TouchQuantizationStep, g_TouchQuantResidualX);
            y = quantize(y, g_TouchQuantizationStep, g_TouchQuantResidualY);
        }
        Input::CaptureDirectTouchDelta(x, y, Touch::IsUnlimited(), beginGesture);
        if (g_PerfTelemetry) { g_CapturedTouchX += x; g_CapturedTouchY += y; }
    }
    FrameInput input = Input::EndCapture();
    input.touchUsed = Touch::WasUsedThisRun();
    input.touchBomb = Touch::UsedTouchToBomb();
    if (input.buttons != 0 || input.analogMode != AnalogMode::None)
        g_PhysicalInputObserved = true;
    return input;
}

std::uint16_t CombinedButtons(const FrameDecision &decision)
{
    std::uint16_t bits = 0;
    for (std::uint8_t player = 0; player < g_PlayerCount; ++player)
        bits = static_cast<std::uint16_t>(bits | decision.inputs[player].buttons);
    return bits;
}

bool UsePhysicalInput()
{
#ifdef __EMSCRIPTEN__
    if (EM_ASM_INT({ return Module.eaglerOptions?.netplayScriptedInput ? 1 : 0; }) != 0)
        return false;
    return ProductionLanMode() ||
           EM_ASM_INT({ return Module.eaglerOptions?.netplayPhysicalInput ? 1 : 0; }) != 0;
#else
    return false;
#endif
}

bool UsePauseCycle()
{
#ifdef __EMSCRIPTEN__
    return EM_ASM_INT({ return Module.eaglerOptions?.netplayPauseCycle ? 1 : 0; }) != 0;
#else
    return false;
#endif
}

bool RequestedStageFixture(int stage)
{
#ifdef __EMSCRIPTEN__
    return ProbeMode() && EM_ASM_INT({
        return Module.eaglerOptions?.netplayTestStage === $0 ? 1 : 0;
    }, stage) != 0;
#else
    (void)stage;
    return false;
#endif
}

bool EligibleForInitialNetplayStart()
{
    const int stage = g_GameManager.currentStage;
    const bool validInitialStage = stage == 1 || stage == 7 || stage == 8 || RequestedStageFixture(stage);
    return g_GameManager.notInMenu && !g_GameManager.replay &&
           !g_GameManager.isPaused && !g_GameManager.isInPauseMenu &&
           !g_GameManager.isInRetryMenu && !g_GameManager.isTimeStopped &&
           validInitialStage && g_GameManager.globals != nullptr &&
           g_GameManager.defaultCfg != nullptr && g_ReplayManager != nullptr &&
           g_Supervisor.curState == 2 && g_Supervisor.wantedState == g_Supervisor.curState;
}

bool InitialNetplayBootstrapInProgress()
{
    const int stage = g_GameManager.currentStage;
    const bool validInitialStage = stage == 1 || stage == 7 || stage == 8 || RequestedStageFixture(stage);
    const bool gameplayScene =
        (g_Supervisor.curState == 2 || g_Supervisor.curState == 3) &&
        (g_Supervisor.wantedState == 2 || g_Supervisor.wantedState == 3);
    return !g_GameManager.replay && validInitialStage && gameplayScene;
}

bool SessionStillOwnsStageState()
{
    // Once the session gate has taken ownership, pause/retry/message time-stop
    // are part of the synchronized stage state as well.  Dropping back to a
    // local g_Chain.RunCalcChain() here would re-sample each machine's hardware
    // and make the first pause the first desync.
    const bool gameplayScene =
        (g_Supervisor.curState == 2 || g_Supervisor.curState == 3) &&
        (g_Supervisor.wantedState == 2 || g_Supervisor.wantedState == 3);
    // TH07 writes the requested scene into curState while wantedState still
    // names the scene being left. Keep ownership for the exact confirmed
    // transition from gameplay into Ending, then for the complete Ending.
    const bool endingScene = g_Supervisor.curState == 9 &&
        (g_Supervisor.wantedState == 2 || g_Supervisor.wantedState == 3 ||
         g_Supervisor.wantedState == 9);
    return !g_GameManager.replay && g_GameManager.currentStage >= 1 &&
           g_GameManager.currentStage <= 8 &&
           g_GameManager.globals != nullptr && g_GameManager.defaultCfg != nullptr &&
           g_ReplayManager != nullptr &&
           (gameplayScene || endingScene);
}

bool SharedUiNeedsConfirmedInputs()
{
    return (g_GameManager.isInPauseMenu && g_AsciiManager.pauseMenu.NeedsConfirmedInput()) ||
           (g_GameManager.isPaused && !g_GameManager.isInPauseMenu) ||
           g_GameManager.isInRetryMenu ||
           (g_Gui.impl && (g_Gui.impl->finishedStage ||
                           g_Gui.impl->transitionToScoreScreen)) ||
           g_Supervisor.curState != 2 || g_Supervisor.wantedState != 2;
}

void NormalizeMultiplayerEndingSkipHistory()
{
    if (g_EndingSkipHistoryNormalized || g_Supervisor.curState != 9 ||
        !g_GameManager.globals)
        return;

    const int shotType = g_GameManager.shotTypeAndCharacter;
    const int difficulty = g_GameManager.difficulty;
    if (shotType < 0 || shotType >= 6 || difficulty < 0 || difficulty >= 6)
        return;

    // Port final sbrik's Ending contract. Ending::AddedCallback derives Ctrl
    // fast-forward from each machine's local score.dat history. Mark the clear
    // one callback earlier on every peer, which is the same eventual save
    // result but makes the parser speed deterministic for this shared scene.
    if (g_GameManager.globals->numRetries == 0)
        g_GameManager.clrd[shotType]
            .difficultyClearedWithRetries[difficulty] = 99;
    else
        g_GameManager.clrd[shotType]
            .difficultyClearedWithoutRetries[difficulty] = 99;
    g_EndingSkipHistoryNormalized = true;
    std::printf("netplay lan ending: synchronized skip history\n");
}

bool RemoteInputsTimedOut()
{
    // Native sbrik can contract a 3P UDP session around an absent guest by
    // announcing host-authored lifecycle controls and relaying synthesized
    // inputs. The browser relay has no equivalent authority protocol. Do not
    // guess that simulation behavior locally: end the room visibly if any
    // required WebSocket lane stops advancing instead of leaving every canvas
    // in an unbounded silent stall.
    constexpr std::uint64_t timeoutMs = 15000;
    const std::uint64_t now = SDL_GetTicks();
    for (std::uint8_t player = 0; player < g_PlayerCount; ++player)
    {
        if (player == g_LocalPlayer)
            continue;
        const std::uint32_t confirmed = g_Core.ConfirmedThrough(player);
        if (!g_Session.CanStart())
        {
            g_RemoteInputWatchdogs[player].Disarm();
            continue;
        }
        // A buffered lockstep peer may intentionally send far ahead of the
        // current simulation frame and then go quiet while we consume already
        // received input. That is healthy progress, not a dead connection.
        if (confirmed != INVALID_FRAME && confirmed >= g_SimFrame)
        {
            g_RemoteInputWatchdogs[player].Disarm();
            (void)g_RemoteInputWatchdogs[player].Observe(confirmed, now, timeoutMs);
            continue;
        }
        if (g_RemoteInputWatchdogs[player].Observe(confirmed, now, timeoutMs))
        {
            std::printf(
                "netplay lan stage: ERROR remote input timeout player=%u confirmed=%u sim=%u\n",
                static_cast<unsigned>(player), confirmed, g_SimFrame);
            return true;
        }
    }
    return false;
}

std::uint8_t ReadPlayer()
{
#ifdef __EMSCRIPTEN__
    const int value = EM_ASM_INT({ return Module.eaglerOptions?.netplayPlayer ?? -1; });
    return value >= 0 && value < g_PlayerCount ? static_cast<std::uint8_t>(value) : 0xff;
#else
    return 0xff;
#endif
}

bool ReadUrl(char *out, std::size_t capacity)
{
#ifdef __EMSCRIPTEN__
    if (!out || capacity == 0)
        return false;
    EM_ASM({ stringToUTF8(Module.eaglerOptions?.netplayUrl || "", $0, $1); }, out, capacity);
    return out[0] != '\0';
#else
    (void)out;
    (void)capacity;
    return false;
#endif
}

bool StartProductionTransportEarly()
{
    if (!ProductionLanMode() || g_ProductionTransportStarted)
        return true;

    // ICE/DataChannel setup is asynchronous and can take several seconds on a
    // real public route. Start it while the vanilla title/loading chain is
    // still presenting frames; waiting until the first Stage 1 deterministic
    // tick can freeze the frame-zero gate before HELLO has a transport.
    g_PlayerCount = ReadPlayerCount();
    g_SpectatorMode = SpectatorModeRequested();
    g_LocalPlayer = g_SpectatorMode ? 0 : ReadPlayer();
    g_SpectatorSizeProbeTarget = SpectatorSizeProbeFrame();
    g_SpectatorSizeProbeDone = false;
    char url[512] = {};
    char spectatorId[65] = {};
    if (g_LocalPlayer >= g_PlayerCount || !ReadUrl(url, sizeof(url)))
        return false;
    const bool connected = g_SpectatorMode
        ? ReadSpectatorId(spectatorId, sizeof(spectatorId)) &&
          g_BrowserPeerTransport.ConnectSpectator(url, spectatorId, g_PlayerCount, true)
        : TransportConnect(url);
    if (!connected)
        return false;
    g_ProductionTransportStarted = true;
    std::printf("netplay lan stage: PRECONNECT player=%u players=%u url=%s\n",
                static_cast<unsigned>(g_LocalPlayer),
                static_cast<unsigned>(g_PlayerCount), url);
    return true;
}

std::uint16_t LocalScript(std::uint8_t player, std::uint32_t frame)
{
    if (UseRestartCycle() && g_SessionGeneration == 0 && frame >= 600 && frame <= 660)
    {
        // Exact vanilla Pause -> Reset path.  P1 opens Pause at frame 600 and
        // hits the dedicated Reset edge after the menu is live.  The resulting
        // Supervisor state 10 must retire this rollback session, rebuild the
        // GameManager, then synchronize a fresh generation from frame zero.
        if (player != 0)
            return 0;
        if (frame == 600)
            return TH_BUTTON_MENU;
        if (frame == 620)
            return TH_BUTTON_RESET;
        return 0;
    }
    if (UsePauseCycle() && frame >= 600 && frame <= 700)
    {
        // Exercise the exact shared pause path without gameplay buttons
        // accidentally navigating the menu.  P1 opens at 600 and closes at
        // 680; every peer receives the same synchronized edge through normal
        // rollback/network input.
        return player == 0 && (frame == 600 || frame == 680)
                   ? TH_BUTTON_MENU
                   : 0;
    }

    // Give every logical player a distinct deterministic pattern so a lane
    // alias or dropped P3 input is observable in both movement and the hash.
    if (player == 0)
    {
        std::uint16_t bits = TH_BUTTON_SHOOT;
        if (UseStageTransitionTest())
            bits |= TH_BUTTON_SKIP;
        const std::uint32_t phase = (frame / 40) & 3u;
        bits |= phase == 0 ? TH_BUTTON_LEFT :
                phase == 1 ? TH_BUTTON_RIGHT :
                phase == 2 ? TH_BUTTON_LEFT : TH_BUTTON_RIGHT;
        return bits;
    }
    if (player == 2)
    {
        std::uint16_t bits = TH_BUTTON_SHOOT;
        const std::uint32_t phase = (frame / 30) & 3u;
        bits |= phase == 0 ? TH_BUTTON_RIGHT :
                phase == 1 ? TH_BUTTON_UP :
                phase == 2 ? TH_BUTTON_LEFT : TH_BUTTON_DOWN;
        if ((frame / 45) & 1u)
            bits |= TH_BUTTON_FOCUS;
        return bits;
    }
    std::uint16_t bits = 0;
    if (UseStageTransitionTest())
        bits |= TH_BUTTON_SHOOT;
    const std::uint32_t phase = (frame / 35) & 3u;
    bits |= phase == 0 ? TH_BUTTON_UP :
            phase == 1 ? TH_BUTTON_DOWN :
            phase == 2 ? TH_BUTTON_UP : TH_BUTTON_DOWN;
    if ((frame / 50) & 1u)
        bits |= TH_BUTTON_FOCUS;
    return bits;
}

void HashToHex(std::uint64_t value, char out[17])
{
    std::snprintf(out, 17, "%016llx", static_cast<unsigned long long>(value));
}

void ReportSpectatorCheckpointHash(std::uint32_t frame, std::uint64_t hash)
{
#ifdef __EMSCRIPTEN__
    char hashHex[17] = {};
    HashToHex(hash, hashHex);
    EM_ASM({
        globalThis.__eaglerNetplayLanCheckpointFrame = $0 >>> 0;
        globalThis.__eaglerNetplayLanCheckpointHash = UTF8ToString($1);
    }, frame, hashHex);
#else
    (void)frame;
    (void)hash;
#endif
}

void PrintHash(const char *label, const Th07CanonicalHash::Sample &sample)
{
    char total[17], meta[17], stage[17], player[17], enemies[17], bullets[17], items[17];
    HashToHex(sample.composite, total);
    HashToHex(sample.meta, meta);
    HashToHex(sample.stage, stage);
    HashToHex(sample.player, player);
    HashToHex(sample.enemies, enemies);
    HashToHex(sample.bullets, bullets);
    HashToHex(sample.items, items);
    std::printf(
        "netplay lan stage: %s player=%u hash=%s meta=%s stage=%s playerHash=%s enemies=%s bullets=%s items=%s counts=%u/%u/%u/%u\n",
        label, static_cast<unsigned>(g_LocalPlayer), total, meta, stage, player,
        enemies, bullets, items, sample.enemyCount, sample.bulletCount,
        sample.laserCount, sample.itemCount);
}

void ClearTransientModes()
{
    Input::ClearReplayOverride();
    Input::ClearPlayerButtonOverrides();
    if (Input::CaptureActive())
        (void)Input::EndCapture();
    SideEffects::SetSpeculative(false);
}

void Fail(const char *reason)
{
    if (g_Done)
        return;
    ClearTransientModes();
    g_Done = true;
#ifdef __EMSCRIPTEN__
    if (ProductionLanMode())
    {
        EM_ASM({
            globalThis.__eaglerNetplayFailed = true;
            globalThis.__eaglerNetplayError = UTF8ToString($0);
        }, reason);
    }
#endif
    std::printf(
        "netplay lan stage: FAIL player=%u reason=%s sim=%u sent=%u recv=%u rollback=%u resim=%u predicted=%u confirmed=%u buffered=%llu error=%s\n",
        static_cast<unsigned>(g_LocalPlayer), reason, g_SimFrame, g_SentPackets,
        g_ReceivedPackets, g_RollbackCount, g_ResimulatedFrames, g_PredictedFrames,
        static_cast<unsigned>(g_Core.ConfirmedThroughAllRemotes()),
        static_cast<unsigned long long>(TransportBufferedAmount()),
        TransportLastError().c_str());
}

bool Initialize()
{
    SideEffects::ResetBgmHistory();
    g_PlayerCount = ReadPlayerCount();
    g_SpectatorMode = SpectatorModeRequested();
    g_LocalPlayer = g_SpectatorMode ? 0 : ReadPlayer();
    g_TestFrames = ProbeMode() ? ReadTestFrames() : 0xffffffffu;
    g_InputRepairBudgets = {};
    g_ReliableInputRepair = false;
#ifdef __EMSCRIPTEN__
    g_ReliableInputRepair = !g_SpectatorMode &&
        EM_ASM_INT({ return Module.eaglerOptions?.netplayReliableInputRepair ? 1 : 0; }) != 0;
#endif
    char url[512] = {};
    if (g_LocalPlayer >= g_PlayerCount || !ReadUrl(url, sizeof(url)))
        return false;

    CoreConfig coreConfig;
    coreConfig.sessionId = CurrentSessionId();
    coreConfig.playerCount = g_PlayerCount;
    coreConfig.localPlayer = g_LocalPlayer;
    coreConfig.inputDelay = 0;
#ifdef __EMSCRIPTEN__
    coreConfig.inputDelay = static_cast<std::uint8_t>(EM_ASM_INT({
        const value = Module.eaglerOptions?.netplayInputDelayFrames;
        return Number.isInteger(value) && value >= 0 && value <= 12 ? value : $0;
    }, coreConfig.inputDelay));
    coreConfig.maxDirectTouchDeltaPredictionFrames = static_cast<std::uint8_t>(EM_ASM_INT({
        const value = Module.eaglerOptions?.netplayTouchDeltaPredictionFrames;
        return Number.isInteger(value) && value >= 0 && value <= 6 ? value : 0;
    }));
    g_TouchQuantizationStep = static_cast<float>(EM_ASM_DOUBLE({
        const value = Number(Module.eaglerOptions?.netplayTouchQuantization || 0);
        return Number.isFinite(value) && value >= 0 && value <= 2 ? value : 0;
    }));
    g_TouchEquivalentAbsorb = EM_ASM_INT({
        return Module.eaglerOptions?.netplayTouchEquivalentAbsorb ? 1 : 0;
    }) != 0;
    g_IncrementalReconcile = EM_ASM_INT({
        return Module.eaglerOptions?.netplayIncrementalReconcile ? 1 : 0;
    }) != 0;
    g_InitialFrameLag = static_cast<std::uint32_t>(EM_ASM_INT({
        const value = Module.eaglerOptions?.netplayInitialFrameLag;
        return Number.isInteger(value) && value >= 0 && value <= 8 ? value : 0;
    }));
    g_InitialFrameLagRemaining = g_InitialFrameLag;
    g_DisableTimeSyncPacing = EM_ASM_INT({
        return Module.eaglerOptions?.netplayDisableTimeSyncPacing ? 1 : 0;
    }) != 0;
    EM_ASM({
        globalThis.__eaglerNetplayInitialFrameLag = $0;
        globalThis.__eaglerNetplayTimeSyncDisabled = !!$1;
    }, g_InitialFrameLag, g_DisableTimeSyncPacing ? 1 : 0);
    EM_ASM({
        globalThis.__eaglerNetplayTouchDeltaPredictionFrames = $0;
        globalThis.__eaglerNetplayTouchQuantization = $1;
        globalThis.__eaglerNetplayTouchEquivalentAbsorb = !!$2;
    }, coreConfig.maxDirectTouchDeltaPredictionFrames, g_TouchQuantizationStep,
       g_TouchEquivalentAbsorb ? 1 : 0);
    EM_ASM({ globalThis.__eaglerNetplayInputDelayFrames = $0; }, coreConfig.inputDelay);
    g_PerfTelemetry = EM_ASM_INT({ return Module.eaglerOptions?.netplayPerfTelemetry ? 1 : 0; }) != 0;
    g_TestIncrementalTouch = ProbeMode() && EM_ASM_INT({
        return Module.eaglerOptions?.netplayTestIncrementalTouch ? 1 : 0;
    }) != 0;
    g_NoRollbackEndpoint = ProbeMode() && EM_ASM_INT({
        return Module.eaglerOptions?.netplayTestNoRollback ? 1 : 0;
    }) != 0;
    g_DemandSnapshots = EM_ASM_INT({
        const policy = Module.eaglerOptions?.netplaySnapshotPolicy;
        return policy === 'demand' || policy === 'frontier' ? 1 : 0;
    }) != 0;
    g_FrontierSnapshots = EM_ASM_INT({
        return Module.eaglerOptions?.netplaySnapshotPolicy === 'frontier' ? 1 : 0;
    }) != 0;
    EM_ASM({ globalThis.__eaglerNetplaySnapshotPolicy = $1 ? 'frontier' : $0 ? 'demand' : 'always'; },
           g_DemandSnapshots ? 1 : 0, g_FrontierSnapshots ? 1 : 0);
    g_TestKeepRollbackSnapshots = ProbeMode() && g_NoRollbackEndpoint && EM_ASM_INT({
        return Module.eaglerOptions?.netplayTestKeepRollbackSnapshots ? 1 : 0;
    }) != 0;
    g_TestDenseBullets = ProbeMode() ? static_cast<std::uint32_t>(EM_ASM_INT({
        const value = Module.eaglerOptions?.netplayTestDenseBullets;
        return Number.isInteger(value) && value >= 0 && value <= 960 ? value : 0;
    })) : 0u;
    g_TestDenseCurved = ProbeMode() && g_TestDenseBullets && EM_ASM_INT({
        return Module.eaglerOptions?.netplayTestDenseCurved ? 1 : 0;
    }) != 0;
    EM_ASM({ globalThis.__eaglerNetplayDenseCurved = !!$0; }, g_TestDenseCurved);
    EM_ASM({
        globalThis.__eaglerNetplayNoRollback = !!$0;
        globalThis.__eaglerNetplayKeepRollbackSnapshots = !!$1;
        globalThis.__eaglerNetplayDenseBullets = $2 >>> 0;
    }, g_NoRollbackEndpoint ? 1 : 0, g_TestKeepRollbackSnapshots ? 1 : 0,
       g_TestDenseBullets);
    g_TouchWorkload = ProbeMode() ? EM_ASM_INT({
        return Math.max(0, ['default', 'steady', 'variable', 'stop', 'reverse', 'burst']
            .indexOf(Module.eaglerOptions?.netplayTouchWorkload));
    }) : 0;
#endif
    SetReconcileTimerMode(false);
    g_ForwardCostNs = g_ResimulationCostNs = g_ReconcileCostNs = g_MaxReconcileCostNs = 0;
    g_ReconcileActive = g_ReconcileShouldYield = false;
    g_ReconcileTimerMode = false;
    g_ReconcileNextFrame = g_ReconcileLastFrame = INVALID_FRAME;
    g_ReconcileYieldCount = g_ReconcileSliceCount = g_MaxReconcileSliceFrames = 0;
    g_DrainYieldCount = g_MaxPacketsDrainedPerTick = 0;
    g_MismatchTouchDelta = g_MismatchButtons = g_MismatchOtherAnalog = 0;
    g_ProbeStartedMs = SDL_GetTicks();
    g_ForwardCalcCostNs = g_ResimulationCalcCostNs = 0;
    g_DenseObservedFrames = 0;
    g_DenseMinBullets = 0xffffffffu;
    g_DenseBulletSum = 0;
    g_SnapshotCostNs = 0;
    g_CanonicalCostNs = 0;
    g_SnapshotTicks = g_ConfirmedOnlyTicks = 0;
    g_LocalCaptures = 0;
    g_InputRepairBudgets = {};
    g_ReliableInputRepair = false;
#ifdef __EMSCRIPTEN__
    g_ReliableInputRepair = EM_ASM_INT({ return Module.eaglerOptions?.netplayReliableInputRepair ? 1 : 0; }) != 0;
#endif
    g_CapturedTouchX = g_CapturedTouchY = 0.0;
    g_InputCaptureFrame = 0;
    g_NextInputCaptureNs = 0;
    g_LastPhysicalCapture = {};
    g_HaveLastPhysicalCapture = false;
    g_TouchQuantResidualX = g_TouchQuantResidualY = 0.0f;
    for (TouchTraceSlot &trace : g_TouchTraces) trace = {};
    g_TouchEquivalentAccepted = g_TouchEquivalentRejected = 0;
    g_TouchEquivalentRejectReasons.fill(0);
    g_LastTouchEquivalentSimFrame = INVALID_FRAME;
    Input::ResetDirectTouchStates();
    coreConfig.maxRollbackFrames = 12;
#ifdef __EMSCRIPTEN__
    coreConfig.maxRollbackFrames = static_cast<std::uint8_t>(EM_ASM_INT({
        const value = Module.eaglerOptions?.netplayMaxPredictionFrames;
        return Number.isInteger(value) && value >= 1 && value <= 12 ? value : 12;
    }));
    EM_ASM({ globalThis.__eaglerNetplayMaxPredictionFrames = $0; }, coreConfig.maxRollbackFrames);
#endif
    coreConfig.predictableButtons =
        TH_BUTTON_DIRECTION | TH_BUTTON_FOCUS | TH_BUTTON_SHOOT | TH_BUTTON_SKIP;
    coreConfig.directionButtons = TH_BUTTON_DIRECTION;
    coreConfig.maxDirectionPredictionFrames = 3;
    if (!g_SpectatorMode && !g_Core.Reset(coreConfig))
        return false;

    // Title/stage bootstrap is allowed to run before the rollback driver owns
    // frame zero, and different browsers can spend a different number of
    // outer ticks there. None of that machine-local input-repeat history is
    // part of the synchronized game timeline. Establish one common baseline
    // before scheduling the first real FrameInput.
    g_CurFrameRawInput = 0;
    g_LastFrameRawInput = 0;
    g_IsEighthFrameOfHeldInput = 0;
    g_NumOfFramesInputsWereHeld = 0;
    g_Supervisor.calcCount = 0;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    for (u8 playerId = 0; playerId < TH07_MULTI_MAX_PLAYERS; ++playerId)
    {
        g_CurFrameGameInputs[playerId] = 0;
        g_LastFrameGameInputs[playerId] = 0;
    }
#endif

    Th07Rollback::Config stateConfig;
    stateConfig.maxFrames = 16;
    stateConfig.checkpointLogicalFrames = 2;
    stateConfig.maxBytesPerFrame = 8 * 1024 * 1024;
    stateConfig.maxBlocksPerFrame = 4096;
    // Final sbrik raised the 3P rollback ceiling to 1024 after simultaneous
    // Bombs routinely exceeded the earlier two-player-sized budget. The
    // journal stores only live ScreenEffect, so retain that gameplay-derived
    // headroom without importing the native snapshot/network shell.
    stateConfig.maxBombEffectsPerFrame = 1024;
#ifdef __EMSCRIPTEN__
    stateConfig.coalesceBulletRuns = EM_ASM_INT({
        return Module.eaglerOptions?.netplaySnapshotLayout === 'runs' ? 1 : 0;
    }) != 0;
    stateConfig.compactBulletSnapshots = EM_ASM_INT({
        return Module.eaglerOptions?.netplayBulletSnapshot === 'compact' ? 1 : 0;
    }) != 0;
    stateConfig.liveBulletSnapshots = EM_ASM_INT({
        return Module.eaglerOptions?.netplayBulletSnapshot === 'live' ? 1 : 0;
    }) != 0;
    stateConfig.auditLiveBulletBytes = ProbeMode() && stateConfig.liveBulletSnapshots && EM_ASM_INT({
        return Module.eaglerOptions?.netplayLiveBulletAudit ? 1 : 0;
    }) != 0;
    stateConfig.elideDormantBulletVm = stateConfig.liveBulletSnapshots && EM_ASM_INT({
        return Module.eaglerOptions?.netplayDormantBulletElision ? 1 : 0;
    }) != 0;
    EM_ASM({ globalThis.__eaglerDormantBulletElision = !!$0; }, stateConfig.elideDormantBulletVm ? 1 : 0);
    EM_ASM({ globalThis.__eaglerNetplayBulletSnapshot = $1 ? 'live' : $0 ? 'compact' : 'journal'; },
           stateConfig.compactBulletSnapshots ? 1 : 0, stateConfig.liveBulletSnapshots ? 1 : 0);
    stateConfig.fastBulkCopy = EM_ASM_INT({
        return Module.eaglerOptions?.netplaySnapshotCopy === 'bulk' ? 1 : 0;
    }) != 0;
    stateConfig.coalesceRestore = EM_ASM_INT({
        return Module.eaglerOptions?.netplaySnapshotRestore === 'coalesced' ? 1 : 0;
    }) != 0;
    stateConfig.checkpointLogicalFrames = static_cast<std::size_t>(EM_ASM_INT({
        const value = Module.eaglerOptions?.netplaySnapshotCheckpointFrames;
        return Number.isInteger(value) && value >= 1 && value <= 8 ? value : 2;
    }));
    EM_ASM({ globalThis.__eaglerNetplaySnapshotCheckpointFrames = $0; },
           static_cast<int>(stateConfig.checkpointLogicalFrames));
    EM_ASM({ globalThis.__eaglerNetplaySnapshotRestore = $0 ? 'coalesced' : 'sequential'; },
           stateConfig.coalesceRestore ? 1 : 0);
    EM_ASM({ globalThis.__eaglerNetplaySnapshotCopy = $0 ? 'bulk' : 'wasm'; },
           stateConfig.fastBulkCopy ? 1 : 0);
    EM_ASM({ globalThis.__eaglerNetplaySnapshotLayout = $0 ? 'runs' : 'objects'; },
           stateConfig.coalesceBulletRuns ? 1 : 0);
#endif
    if (!Th07Rollback::Reset(stateConfig))
        return false;

#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (UseStageTransitionTest() || RequestedStageFixture(g_GameManager.currentStage))
    {
        // Upstream's full-run preset is invincible as well: the purpose of
        // this harness is to exercise the real normal-route stage lifecycle,
        // not measure how long a deterministic bot survives. Keep that test
        // policy local to the probe so production Player/GameManager semantics
        // stay untouched.
        for (std::uint8_t player = 0; player < g_PlayerCount; ++player)
        {
            SetPlayerLives(player, 8);
            if (g_SpectatorSizeProbeTarget >= 0)
                SetPlayerPower(player, SPECTATOR_SIZE_PROBE_MAX_POWER);
            g_Players[player].playerState = PLAYER_STATE_INVULNERABLE;
            g_Players[player].invulnerabilityTimer = 0x3fffffff;
        }
    }
#endif

    SessionConfig sessionConfig;
    sessionConfig.sessionId = CurrentSessionId();
    sessionConfig.seed = static_cast<std::uint32_t>(g_Rng.seed);
    sessionConfig.gameplayAbi = GameplayAbi();
    sessionConfig.gameId = GAME_ID_TH07;
    sessionConfig.playerCount = g_PlayerCount;
    sessionConfig.localPlayer = g_LocalPlayer;
    if (g_SpectatorMode)
    {
        char spectatorId[65] = {};
        if (!g_ProductionTransportStarted &&
            (!ReadSpectatorId(spectatorId, sizeof(spectatorId)) ||
             !g_BrowserPeerTransport.ConnectSpectator(url, spectatorId, g_PlayerCount, true)))
            return false;
    }
    else if (!g_Session.Reset(sessionConfig) ||
             (!g_ProductionTransportStarted && !TransportConnect(url)))
        return false;
    g_ProductionTransportStarted = true;

    g_LastRemoteSenderFrame.fill(INVALID_FRAME);
    g_RemoteInputWatchdogs = {};
    for (FrameAdvantageWindow &state : g_PeerTimeSync)
        state = FrameAdvantageWindow{};
    g_PredictionDepth.fill(0);
    g_RollbackByPlayer.fill(0);
    g_HighestStageObserved = 0;
    g_StageTransitionObserved = false;
    g_RecommendedLead = 0.0;
    g_SimulationIntervalScale = 1.0;
    g_EndingSkipHistoryNormalized = false;
    g_ReplayPlaybackCycleDispatched = false;
    g_NextReplayAuditFrame = 0;
    g_NextSpectatorPublishFrame = 0;
    g_NextSpectatorReceiveFrame = 0;
    g_SpectatorPlayback.Clear();
    g_SpectatorView.Clear();
    g_HavePendingSpectatorBootstrap = false;
    g_PendingSpectatorBootstrap = {};
    g_SpectatorNativeStageReady = false;
    g_SpectatorAuditCheckpointStage = 0;
    g_SpectatorSizeProbeDone = false;
    g_SpectatorBossSlotsThisFrame = 0;
    g_SpectatorPublisherEligible = !g_SpectatorMode && g_LocalPlayer == 0 &&
        ProductionLanMode();
    g_SpectatorLaneStopped = false;
    g_SpectatorPublisherReady = g_SpectatorPublisherEligible &&
        g_SpectatorPublisher.Reset('7', g_PlayerCount, CurrentSessionId(), GameplayAbi());
#ifdef TH_DEV_TOOLS
    if (UseReplayPlaybackCycle())
        ReplayExtension::DebugResetMultiplayerPlaybackAudit();
#endif
    g_Active = true;
#ifdef __EMSCRIPTEN__
    if (ProductionLanMode())
    {
        EM_ASM({
            globalThis.__eaglerNetplayRuntimeBuild = "th07mp-20260917-zero-delay-responsive";
            globalThis.__eaglerNetplayFailed = false;
            globalThis.__eaglerNetplayError = "";
            globalThis.__eaglerNetplayLanActive = true;
            globalThis.__eaglerNetplayLanFrame = 0;
            globalThis.__eaglerNetplayLanGeneration = $0;
            globalThis.__eaglerNetplayLanHighestStage = 0;
            globalThis.__eaglerNetplayLanStageTransitionObserved = false;
            globalThis.__eaglerNetplayLanCheckpointFrame = -1;
            globalThis.__eaglerNetplayLanCheckpointHash = "";
            globalThis.__eaglerNetplayLanHash = "";
            globalThis.__eaglerNetplayLanHashes = Object.create(null);
            globalThis.__eaglerNetplayLanPeerAdvantages = [];
            globalThis.__eaglerNetplayLanPeers = [];
            globalThis.__eaglerNetplayPerf = {};
            const spectatorPublish = globalThis.__eaglerNetplaySpectatorPublish = {};
            spectatorPublish.cursor = 0;
            spectatorPublish.simFrame = 0;
            spectatorPublish.confirmed = -1;
            spectatorPublish.publisherReady = !!$1;
            spectatorPublish.failureFrame = -1;
            spectatorPublish.failureReason = "";
            spectatorPublish.lastPublishFailure = "";
            spectatorPublish.lastPublishFailureFrame = -1;
            spectatorPublish.lastPublishPermanent = false;
            spectatorPublish.sendAttempts = 0;
            spectatorPublish.sendSuccesses = 0;
            spectatorPublish.lastSendSucceeded = false;
            globalThis.__eaglerNetplaySpectatorPublishFailed = -1;
        }, g_SessionGeneration, g_SpectatorPublisherReady ? 1 : 0);
    }
#endif
    if (g_SpectatorPublisherEligible && !g_SpectatorPublisherReady)
        DisableSpectatorPublisher(0, "publisher-reset-failed");
    std::printf("netplay lan stage: CONNECT player=%u players=%u url=%s seed=%u rngGeneration=%u sessionGeneration=%u\n",
                static_cast<unsigned>(g_LocalPlayer), static_cast<unsigned>(g_PlayerCount), url,
                static_cast<unsigned>(g_Rng.seed),
                static_cast<unsigned>(g_Rng.generationCount),
                static_cast<unsigned>(g_SessionGeneration));
    return true;
}

bool TryAcceptEquivalentDirectTouch(std::uint8_t player, std::uint32_t frame,
                                    const FrameInput &actual)
{
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (!g_TouchEquivalentAbsorb || player >= g_PlayerCount || player == g_LocalPlayer ||
        g_Core.HasRollbackRequest() || g_LastTouchEquivalentSimFrame == g_SimFrame ||
        g_Core.InputPresent(player, frame))
        return false;

    const std::uint32_t last = g_Core.LastSimulatedFrame();
    if (last == INVALID_FRAME || frame > last || last - frame >= TOUCH_EQUIVALENCE_HISTORY)
        return false;

    FrameInput used{};
    bool predicted = false;
    if (!g_Core.UsedInput(player, frame, &used, &predicted) || !predicted || used == actual)
        return false;

    // Keep deterministic diagnostic checkpoints authoritative without having
    // to reconstruct a historical world merely to refresh their sampled hash.
    if (ProbeMode())
    {
        for (std::uint32_t value = frame; value <= last; ++value)
            if (((value + 1u) % 300u) == 0u)
                return false;
    }

    std::array<DirectTouchFrameTrace, TOUCH_EQUIVALENCE_HISTORY> sequence{};
    const std::size_t count = static_cast<std::size_t>(last - frame + 1u);
    for (std::size_t index = 0; index < count; ++index)
    {
        const std::uint32_t traceFrame = frame + static_cast<std::uint32_t>(index);
        const TouchTraceSlot &slot = g_TouchTraces[traceFrame % g_TouchTraces.size()];
        if (slot.frame != traceFrame)
            return false;
        sequence[index] = slot.players[player];
    }
    if (sequence[0].used != used)
        return false;

    const DirectTouchEquivalenceResult proof =
        ProveDirectTouchEquivalent(sequence.data(), count, actual);
    if (!proof.equivalent)
    {
        const auto reason = static_cast<std::size_t>(proof.reject);
        if (reason < g_TouchEquivalentRejectReasons.size())
            ++g_TouchEquivalentRejectReasons[reason];
        if (g_PerfTelemetry && g_TouchEquivalentRejected < 8)
        {
            const DirectTouchFrameTrace &trace = sequence[std::min(
                proof.rejectIndex, count - 1)];
            std::printf(
                "netplay touch equivalent reject player=%u frame=%u reason=%u index=%u "
                "used=%.3f/%.3f actual=%.3f/%.3f applied=%.3f/%.3f remaining=%.3f/%.3f "
                "margin=%.1f/%.1f/%.1f/%.1f\n",
                static_cast<unsigned>(player), frame,
                static_cast<unsigned>(proof.reject),
                static_cast<unsigned>(proof.rejectIndex),
                used.x, used.y, actual.x, actual.y,
                trace.applied.x, trace.applied.y,
                trace.remaining.x, trace.remaining.y,
                trace.leftMargin, trace.rightMargin,
                trace.topMargin, trace.bottomMargin);
        }
        ++g_TouchEquivalentRejected;
        return false;
    }

    const bool carrySurvives = proof.carryX != 0.0f || proof.carryY != 0.0f;
    auto &live = Input::GetDirectTouchStates()[player];
    if (carrySurvives)
    {
        const DirectTouchState &expected = sequence[count - 1].remaining;
        if (live.x != expected.x || live.y != expected.y || live.active != expected.active)
            return false;
    }

    const std::uint32_t patchThrough = carrySurvives
        ? last
        : frame + static_cast<std::uint32_t>(proof.framesChecked);
    if (!Th07Rollback::PatchDirectTouchSnapshots(
            player, frame, patchThrough, proof.carryX, proof.carryY))
        return false;

    const EquivalentRemoteInputResult accepted =
        g_Core.SubmitEquivalentRemoteInput(player, frame, actual);
    if (accepted != EquivalentRemoteInputResult::Confirmed &&
        accepted != EquivalentRemoteInputResult::Duplicate)
        return false;

    if (carrySurvives)
    {
        live.x += proof.carryX;
        live.y += proof.carryY;
    }

    ReplayFrameBinding &binding = g_ReplayFrameBindings[frame % g_ReplayFrameBindings.size()];
    const FrameDecision authoritative = g_Core.PrepareFrame(frame);
    if (binding.simFrame == frame && binding.stage >= 0 && binding.replayFrame >= 0 &&
        authoritative.canAdvance)
    {
        ReplayExtension::RecordMultiplayerFrame(
            binding.stage, binding.replayFrame, authoritative.inputs.data(), g_PlayerCount);
    }

    ++g_TouchEquivalentAccepted;
    g_LastTouchEquivalentSimFrame = g_SimFrame;
    return true;
#else
    (void)player; (void)frame; (void)actual;
    return false;
#endif
}

bool DrainPackets()
{
    std::vector<std::uint8_t> wire;
    constexpr std::uint32_t kIncrementalDrainPacketBudget = 64;
    std::uint32_t drainedThisTick = 0;
    while (TransportPoll(&wire))
    {
        ++drainedThisTick;
        const bool drainBudgetReached = g_IncrementalReconcile &&
            drainedThisTick >= kIncrementalDrainPacketBudget;
        ++g_ReceivedPackets;
        PacketType type;
        if (!PeekPacketType(wire.data(), wire.size(), &type))
            return false;
        if (type == PacketType::Session)
        {
            SessionPacket packet;
            if (!DecodeSessionPacket(wire.data(), wire.size(), &packet))
                return false;
            if (packet.sessionId != CurrentSessionId())
                continue;
            ++g_SessionPacketsReceived;
            const SessionPacketResult result = g_Session.Apply(packet);
            if (result == SessionPacketResult::InvalidPeer ||
                result == SessionPacketResult::ContractMismatch)
                return false;
            // Browser production uses a separate reliable/ordered RTC control
            // channel. The periodic session packets remain useful for the
            // legacy WebSocket/probe path and reconnect-safe startup logic.
            if (drainBudgetReached)
                break;
            continue;
        }

        InputPacket packet;
        if (!DecodeInputPacket(wire.data(), wire.size(), &packet))
            return false;
        if (packet.sessionId != CurrentSessionId())
            continue;
        if (packet.senderPlayer == g_LocalPlayer)
            return false;
        RecordTimeSyncSample(packet);
        g_LastReceivedSequence = std::max(g_LastReceivedSequence, packet.sequence);
        if (g_TouchEquivalentAbsorb)
        {
            for (std::uint8_t i = 0; i < packet.inputCount; ++i)
            {
                const std::uint32_t frame = packet.firstInputFrame + i;
                if (TryAcceptEquivalentDirectTouch(packet.senderPlayer, frame, packet.inputs[i]))
                    break;
            }
        }
        if (g_PerfTelemetry)
        {
            for (std::uint8_t i = 0; i < packet.inputCount; ++i)
            {
                const std::uint32_t frame = packet.firstInputFrame + i;
                if (g_Core.InputPresent(packet.senderPlayer, frame))
                    continue;
                FrameInput used{};
                bool predicted = false;
                if (!g_Core.UsedInput(packet.senderPlayer, frame, &used, &predicted) || !predicted ||
                    used == packet.inputs[i])
                    continue;
                const FrameInput &actual = packet.inputs[i];
                if (used.buttons != actual.buttons || used.touchBomb != actual.touchBomb)
                    ++g_MismatchButtons;
                const bool usedTouch = used.analogMode == AnalogMode::DirectTouchDelta ||
                                       used.analogMode == AnalogMode::DirectTouchBegin;
                const bool actualTouch = actual.analogMode == AnalogMode::DirectTouchDelta ||
                                         actual.analogMode == AnalogMode::DirectTouchBegin;
                if (usedTouch && actualTouch && used.buttons == actual.buttons &&
                    used.touchBomb == actual.touchBomb)
                    ++g_MismatchTouchDelta;
                else if (used.analogMode != actual.analogMode || used.x != actual.x ||
                         used.y != actual.y || used.unlimited != actual.unlimited)
                    ++g_MismatchOtherAnalog;
            }
        }
        RemoteInputResult result = RemoteInputResult::Duplicate;
        if (!g_Core.ApplyInputPacket(packet, &result))
            return false;
        if (result == RemoteInputResult::RollbackRequired)
            ++g_RollbackByPlayer[packet.senderPlayer];
        if (drainBudgetReached)
            break;
    }
    g_MaxPacketsDrainedPerTick = std::max(g_MaxPacketsDrainedPerTick, drainedThisTick);
    if (g_IncrementalReconcile && drainedThisTick >= kIncrementalDrainPacketBudget)
        ++g_DrainYieldCount;
    return true;
}

bool SendSessionControl(bool forceReady = false)
{
    if (!TransportIsOpen() || (g_Session.CanStart() && !forceReady))
        return true;

    // The RTC control DataChannel is reliable and ordered. Re-sending the
    // same HELLO/READY on every driver tick only creates a large backlog when
    // one browser is temporarily busy rebuilding GameManager during Restart.
    // Keep a conservative 250 ms retry cadence for loss/reconnect tolerance;
    // phase changes still send immediately because HELLO and READY track
    // separate clocks.
    constexpr std::uint32_t SESSION_RETRY_TICKS = 15;
    const auto due = [](std::uint32_t lastTick) {
        return lastTick == 0 || g_DriverTicks - lastTick >= SESSION_RETRY_TICKS;
    };

    const auto sendPhase = [&](SessionPhase phase, std::uint32_t *lastTick) {
        if (!due(*lastTick))
            return true;
        const SessionPacket packet = g_Session.BuildPacket(phase);
        std::vector<std::uint8_t> wire;
        if (!EncodeSessionPacket(packet, &wire) ||
            !TransportSend(wire.data(), wire.size(), true))
            return false;
        *lastTick = g_DriverTicks;
        ++g_SessionPacketsSent;
        return true;
    };

    if (forceReady)
        return sendPhase(SessionPhase::Ready, &g_LastReadySendTick);
    if (!g_Session.CanSendReady())
        return sendPhase(SessionPhase::Hello, &g_LastHelloSendTick);

    // A slower peer can receive our HELLO before its own Stage 1 gate starts.
    // Its first packet would then be READY, which the earlier peer must reject
    // until it has seen that slower peer's HELLO. Keep HELLO retransmission in
    // the READY phase so asymmetric loading can always complete the ordered
    // session contract, then send READY on the reliable control channel.
    if (!sendPhase(SessionPhase::Hello, &g_LastHelloSendTick))
        return false;
    g_Session.MarkLocalReady();
    return sendPhase(SessionPhase::Ready, &g_LastReadySendTick);
}

bool SendScheduledLocalFrame(std::uint32_t frame);
bool SendLocalFrame(std::uint32_t frame);
bool SendLocalSample(std::uint32_t captureFrame, const FrameInput &input);

FrameInput ContinuePhysicalInput(const FrameInput &input)
{
    FrameInput continued = input;
    // Only state that is safe to hold across an input slot we could not sample
    // individually. One-shot/menu/debug edges must never be manufactured.
    continued.buttons &= static_cast<std::uint16_t>(
        TH_BUTTON_DIRECTION | TH_BUTTON_FOCUS | TH_BUTTON_SHOOT | TH_BUTTON_SKIP);
    continued.touchBomb = false;
    if (continued.analogMode == AnalogMode::DirectTouchBegin ||
        continued.analogMode == AnalogMode::DirectTouchDelta)
    {
        continued.analogMode = AnalogMode::DirectTouchDelta;
        continued.x = 0.0f;
        continued.y = 0.0f;
    }
    return continued;
}

bool PumpBufferedLockstepInput()
{
    constexpr std::uint64_t captureIntervalNs = 1000000000ull / 60ull;
    const std::uint64_t now = SDL_GetTicksNS();
    // Usually the input timeline stays at most one capture slot ahead of
    // simulation. Incremental rollback deliberately pauses g_SimFrame while
    // historical state is replayed across several browser callbacks; keep
    // sampling the physical 60 Hz timeline during that window so rollback
    // work cannot also freeze local touch/keyboard transmission. The bounded
    // lead remains well below INPUT_HISTORY_SIZE and collapses back to one as
    // soon as reconciliation is no longer pending/active.
    constexpr std::uint32_t kMaxIncrementalCaptureLead = 64;
    const bool reconciliationNeedsInputLead = g_IncrementalReconcile &&
        (g_ReconcileActive || g_Core.HasRollbackRequest());
    const std::uint32_t startupLead = g_InitialFrameLagRemaining != 0
        ? g_InitialFrameLag + 1u : 1u;
    const std::uint32_t captureLead = reconciliationNeedsInputLead
        ? kMaxIncrementalCaptureLead : SharedUiNeedsConfirmedInputs() ? 8u : startupLead;
    const std::uint32_t maxCaptureFrame =
        g_SimFrame < INVALID_FRAME - captureLead ? g_SimFrame + captureLead : g_SimFrame;
    if (g_NextInputCaptureNs == 0)
        g_NextInputCaptureNs = now;
    // A rollback may enter shared UI after this callback already captured the
    // current gameplay slot. Preserve that exact sample rather than recapture.
    while (g_InputCaptureFrame <= maxCaptureFrame && g_Core.HasLocalCapture(g_InputCaptureFrame))
        ++g_InputCaptureFrame;
    // A bounded future tail can outgrow packet redundancy during a stall.
    // Keep the unadvanced frontier recoverable even without reliable repair.
    if (g_InputCaptureFrame > g_SimFrame &&
        g_InputCaptureFrame - g_SimFrame > MAX_REDUNDANT_INPUTS &&
        (g_DriverTicks % 3u) == 0u && g_Core.HasLocalCapture(g_SimFrame) &&
        !SendScheduledLocalFrame(g_Core.LocalFrameForCapture(g_SimFrame)))
        return false;

    if (UsePhysicalInput() && now >= g_NextInputCaptureNs &&
        g_InputCaptureFrame <= maxCaptureFrame)
    {
        // Preserve the fixed 60 Hz input timeline even if one browser callback
        // arrives late. Missed historical slots receive only the previous
        // holdable state; the newest slot samples hardware exactly once and is
        // the only slot allowed to consume accumulated DirectTouch movement or
        // one-shot Bomb/Menu edges.
        constexpr unsigned maxCatchupCaptures = 8;
        const std::uint64_t overdue = now - g_NextInputCaptureNs;
        const std::uint32_t delay = g_Core.LocalFrameForCapture(0);
        const std::uint32_t captureEnd = ProbeMode() ? g_TestFrames - delay : INVALID_FRAME;
        const unsigned due = BoundedCaptureBatch(overdue / captureIntervalNs + 1u,
            g_InputCaptureFrame, maxCaptureFrame, captureEnd, maxCatchupCaptures);
        unsigned captured = 0;
        for (unsigned index = 0; index < due; ++index)
        {
            if (g_InputCaptureFrame > maxCaptureFrame)
                break;
            const std::uint32_t scheduled = g_Core.LocalFrameForCapture(g_InputCaptureFrame);
            if (scheduled == INVALID_FRAME || (ProbeMode() && scheduled >= g_TestFrames))
                break;

            const bool newest = index + 1u == due;
            FrameInput input;
            if (newest || !g_HaveLastPhysicalCapture)
            {
                input = CaptureLocalInput(g_InputCaptureFrame);
                g_LastPhysicalCapture = input;
                g_HaveLastPhysicalCapture = true;
            }
            else
            {
                input = ContinuePhysicalInput(g_LastPhysicalCapture);
            }
            if (!SendLocalSample(g_InputCaptureFrame, input))
                return false;
            ++g_InputCaptureFrame;
            ++captured;
            g_NextInputCaptureNs += captureIntervalNs;
        }
        if (captured != 0)
            return true;
    }

    // Scripted probe input is a pure function of capture-frame number, so the
    // harness can reconstruct capture slots missed by a late browser callback.
    // This removes RAF cadence as a confounder when measuring the network
    // buffer itself. Physical input deliberately does NOT take this path: a
    // direct-touch delta is sampled/consumed once and must never be duplicated.
    if (ProbeMode() && !UsePhysicalInput() && now >= g_NextInputCaptureNs &&
        g_InputCaptureFrame <= maxCaptureFrame)
    {
        constexpr unsigned maxCatchupCaptures = 8;
        unsigned captured = 0;
        while (now >= g_NextInputCaptureNs && captured < maxCatchupCaptures &&
               g_InputCaptureFrame <= maxCaptureFrame)
        {
            const std::uint32_t scheduled = g_Core.LocalFrameForCapture(g_InputCaptureFrame);
            if (scheduled == INVALID_FRAME || scheduled >= g_TestFrames)
                break;
            if (!SendLocalFrame(g_InputCaptureFrame))
                return false;
            ++g_InputCaptureFrame;
            ++captured;
            g_NextInputCaptureNs += captureIntervalNs;
        }
        if (captured != 0)
            return true;
    }

    if (now >= g_NextInputCaptureNs && g_InputCaptureFrame <= maxCaptureFrame)
    {
        const std::uint32_t scheduled = g_Core.LocalFrameForCapture(g_InputCaptureFrame);
        if (scheduled != INVALID_FRAME && (!ProbeMode() || scheduled < g_TestFrames))
        {
            if (!SendLocalFrame(g_InputCaptureFrame))
                return false;
            ++g_InputCaptureFrame;
            g_NextInputCaptureNs = now + captureIntervalNs;
            return true;
        }
        // Probe input is finite. Park on the first out-of-range capture so the
        // retry path below keeps retransmitting the last valid scheduled frame
        // until every peer has ACKed the tail. Advancing forever here used to
        // strand a slower peer at the first 32-frame redundancy window.
        if (!ProbeMode())
            ++g_InputCaptureFrame;
        g_NextInputCaptureNs = now + captureIntervalNs;
    }

    // Keep the most recent future slot redundant on the wire without sampling
    // input again. This preserves the ordinary packet-loss recovery contract.
    if (g_InputCaptureFrame != 0 && (g_DriverTicks % 3u) == 0u)
    {
        const std::uint32_t latest = g_Core.LocalFrameForCapture(g_InputCaptureFrame - 1u);
        if (latest != INVALID_FRAME && (!ProbeMode() || latest < g_TestFrames) &&
            !SendScheduledLocalFrame(latest))
            return false;
    }
    return true;
}

bool SendLocalFrame(std::uint32_t frame)
{
    const FrameInput input = CaptureLocalInput(frame);
    return SendLocalSample(frame, input);
}

bool SendLocalSample(std::uint32_t captureFrame, const FrameInput &input)
{
    if (UsePhysicalInput() && captureFrame == 0 && !g_PhysicalLoggedFrame0Sample)
    {
        g_PhysicalLoggedFrame0Sample = true;
        std::printf("netplay lan physical: FRAME0 SAMPLE player=%u bits=0x%04x\n",
                    static_cast<unsigned>(g_LocalPlayer),
                    static_cast<unsigned>(input.buttons));
    }
    if (!g_Core.ScheduleLocalInput(captureFrame, input))
        return false;
#ifdef __EMSCRIPTEN__
    if (g_PerfTelemetry)
        EM_ASM({ globalThis.__th07InputLatencyProbe?.capture($0, $1, !!$2); },
               captureFrame, g_Core.LocalFrameForCapture(captureFrame),
               (input.x != 0.0f || input.y != 0.0f) ? 1 : 0);
#endif
    ++g_LocalCaptures;
    return SendScheduledLocalFrame(g_Core.LocalFrameForCapture(captureFrame));
}

bool SendScheduledLocalFrame(std::uint32_t frame)
{
    // Rebuild only the redundant wire packet. The logical FrameInput was
    // captured and stored exactly once by SendLocalFrame, so a startup/stall
    // retry never samples keyboard, controller or touch displacement again.
    if (ProductionLanMode())
    {
        for (std::uint8_t peer = 0; peer < g_PlayerCount; ++peer)
        {
            if (peer == g_LocalPlayer)
                continue;
            InputPacket packet = g_Core.BuildInputPacket(
                peer, frame, g_Sequence++, g_LastReceivedSequence);
            packet.senderFrame = g_SimFrame;
            if (g_LastRemoteSenderFrame[peer] != INVALID_FRAME)
            {
                packet.frameAdvantage = static_cast<std::int16_t>(
                    FramePacingPolicy::SignedFrameDelta(
                        g_SimFrame, g_LastRemoteSenderFrame[peer]));
            }
            std::vector<std::uint8_t> wire;
            if (!EncodeInputPacket(packet, &wire) ||
                !TransportSendTo(peer, wire.data(), wire.size()))
                return false;
            if (g_ReliableInputRepair && g_InputRepairBudgets[peer].ShouldRepair(
                    packet.firstInputFrame, packet.inputCount != 0, SDL_GetTicks()))
                (void)g_BrowserPeerTransport.SendRepairTo(peer, wire.data(), wire.size());
            ++g_SentPackets;
        }
        return true;
    }

    const std::uint8_t peer = FirstRemotePlayer();
    InputPacket packet = g_Core.BuildInputPacket(
        peer, frame, g_Sequence++, g_LastReceivedSequence);
    packet.senderFrame = g_SimFrame;
    if (g_PlayerCount > 2)
    {
        // The minimal LAN relay broadcasts one wire packet to every other
        // peer. ACK state is peer-specific, so a 3P broadcast deliberately
        // carries no ACK and keeps the normal 32-frame redundant input tail.
        // This is a test-transport choice, not the public transport contract.
        packet.ackFrame = INVALID_FRAME;
    }
    std::vector<std::uint8_t> wire;
    if (!EncodeInputPacket(packet, &wire) || !TransportSend(wire.data(), wire.size()))
        return false;
    ++g_SentPackets;
    return true;
}

bool SendTailKeepalive()
{
    if (!TransportIsOpen() || g_SimFrame < g_TestFrames)
        return true;

    return SendScheduledLocalFrame(g_TestFrames - 1);
}

int SimulateFrame(std::uint32_t frame, const FrameDecision &decision, bool resimulation)
{
    const std::uint32_t spectatorStage = static_cast<std::uint32_t>(g_GameManager.currentStage);
    const std::uint32_t sizeProbeConfirmed = g_Core.ConfirmedThroughAllRemotes();
    const bool sizeProbePrefixConfirmed = frame == 0 ||
        (sizeProbeConfirmed != INVALID_FRAME && sizeProbeConfirmed >= frame - 1u);
    if (!g_SpectatorSizeProbeDone && decision.predictedMask == 0 &&
        g_SpectatorSizeProbeTarget >= 0 &&
        frame == static_cast<std::uint32_t>(g_SpectatorSizeProbeTarget) &&
        sizeProbePrefixConfirmed && ProductionLanMode() &&
        !g_SpectatorMode)
    {
        g_SpectatorSizeProbeDone = true;
        std::vector<std::uint8_t> stateBytes;
        SpectatorCheckpointMetrics metrics{};
        std::vector<std::uint8_t> wire;
        bool encoded = EncodeSpectatorCheckpoint(&stateBytes, &metrics, frame);
        if (encoded)
        {
            SpectatorStageEvent event;
            event.game = '7'; event.playerCount = g_PlayerCount;
            event.sessionId = CurrentSessionId(); event.gameplayAbi = GameplayAbi();
            event.frame = frame; event.stage = spectatorStage; event.serial = 1;
            event.schema = SPECTATOR_CHECKPOINT_SCHEMA; event.state = stateBytes;
            encoded = EncodeSpectatorStageEvent(event, &wire);
            if (encoded) metrics.eventBytes = static_cast<std::uint32_t>(wire.size());
        }
        char probeJson[768] = {};
        const int probeJsonLength = std::snprintf(
            probeJson, sizeof(probeJson),
            "{\"frame\":%u,\"stage\":%u,\"ok\":%s,\"predictedMask\":%u,\"confirmedThrough\":%u,"
            "\"rawBytes\":%u,\"sparseBytes\":%u,"
            "\"compressedBytes\":%u,\"stateBytes\":%u,\"eventBytes\":%u,\"activePlayerBullets\":%u,"
            "\"activeBombs\":%u,\"players\":["
            "{\"power\":%d,\"activeShots\":%u,\"activeBombSubrecords\":%u},"
            "{\"power\":%d,\"activeShots\":%u,\"activeBombSubrecords\":%u},"
            "{\"power\":%d,\"activeShots\":%u,\"activeBombSubrecords\":%u}]}",
            frame, spectatorStage, encoded ? "true" : "false",
            static_cast<unsigned>(decision.predictedMask), sizeProbeConfirmed, metrics.rawBytes,
            metrics.sparseBytes, metrics.compressedBytes, metrics.stateBytes,
            static_cast<unsigned>(wire.size()), metrics.activePlayerBullets, metrics.activeBombs,
            metrics.playerPower[0], metrics.activePlayerBulletsByPlayer[0], metrics.activeBombSubrecordsByPlayer[0],
            metrics.playerPower[1], metrics.activePlayerBulletsByPlayer[1], metrics.activeBombSubrecordsByPlayer[1],
            metrics.playerPower[2], metrics.activePlayerBulletsByPlayer[2], metrics.activeBombSubrecordsByPlayer[2]);
        if (probeJsonLength <= 0 || static_cast<std::size_t>(probeJsonLength) >= sizeof(probeJson))
            probeJson[0] = '\0';
        std::printf("netplay spectator TH07: high-fire size probe %s\n", probeJson);
#ifdef __EMSCRIPTEN__
        EM_ASM({
            const state = globalThis.__eaglerNetplaySpectatorPublish ||= {};
            state.highFireProbe = JSON.parse(UTF8ToString($0));
        }, probeJson);
#endif
    }
    std::vector<std::uint8_t> spectatorInitializer;
    std::uint32_t spectatorSchema = 0;
    g_SpectatorBossSlotsThisFrame = 0;
    // Keep the source hash audit for every player seat. This monotonic marker
    // is diagnostic only; the publishing seat derives a checkpoint boundary
    // from its recorded logical predecessor so rollback can recapture a stage
    // initializer at the same frame.
    const bool auditCheckpointDue = frame == 0 ||
        spectatorStage != g_SpectatorAuditCheckpointStage;
    const auto checkpointRequirement = g_SpectatorPublisherReady
        ? g_SpectatorPublisher.NeedsCheckpoint(frame, spectatorStage)
        : Netplay::SpectatorStagePublisher::CheckpointRequirement::NotRequired;
    const bool publisherCheckpointDue = checkpointRequirement ==
        Netplay::SpectatorStagePublisher::CheckpointRequirement::Required;
    if (g_SpectatorPublisherReady && checkpointRequirement ==
            Netplay::SpectatorStagePublisher::CheckpointRequirement::MissingPredecessor)
    {
        std::printf("netplay spectator TH07: publisher checkpoint predecessor unavailable frame=%u stage=%u\n",
                    frame, spectatorStage);
        DisableSpectatorPublisher(frame, "checkpoint-predecessor-missing");
    }
    if (auditCheckpointDue && !g_SpectatorMode && ProductionLanMode())
        ReportSpectatorCheckpointHash(frame, Th07CanonicalHash::Capture().composite);
    if (g_SpectatorPublisherReady && publisherCheckpointDue)
    {
        SpectatorCheckpointMetrics checkpointMetrics{};
        if (EncodeSpectatorCheckpoint(&spectatorInitializer, &checkpointMetrics, frame))
        {
            spectatorSchema = SPECTATOR_CHECKPOINT_SCHEMA;
            SpectatorStageEvent checkpointEvent;
            checkpointEvent.game = '7'; checkpointEvent.playerCount = g_PlayerCount;
            checkpointEvent.sessionId = CurrentSessionId(); checkpointEvent.gameplayAbi = GameplayAbi();
            checkpointEvent.frame = frame; checkpointEvent.stage = spectatorStage;
            checkpointEvent.serial = g_SpectatorPublisher.Serial() + 1;
            checkpointEvent.kind = SpectatorEventKind::Checkpoint;
            checkpointEvent.schema = SPECTATOR_CHECKPOINT_SCHEMA;
            checkpointEvent.state = spectatorInitializer;
            std::vector<std::uint8_t> checkpointEventWire;
            if (EncodeSpectatorStageEvent(checkpointEvent, &checkpointEventWire))
                checkpointMetrics.eventBytes = static_cast<std::uint32_t>(checkpointEventWire.size());
            std::printf("netplay spectator TH07: checkpoint encoded frame=%u stage=%u rawBytes=%u sparseBytes=%u compressedBytes=%u stateBytes=%u eventBytes=%u activePlayerBullets=%u activeBombs=%u\n",
                        frame, spectatorStage, checkpointMetrics.rawBytes, checkpointMetrics.sparseBytes,
                        checkpointMetrics.compressedBytes, checkpointMetrics.stateBytes,
                        checkpointMetrics.eventBytes, checkpointMetrics.activePlayerBullets,
                        checkpointMetrics.activeBombs);
#ifdef __EMSCRIPTEN__
            char checkpointMetricsJson[768] = {};
            const int checkpointMetricsLength = std::snprintf(
                checkpointMetricsJson, sizeof(checkpointMetricsJson),
                "{\"rawBytes\":%u,\"sparseBytes\":%u,\"compressedBytes\":%u,\"stateBytes\":%u,"
                "\"eventBytes\":%u,\"activePlayerBullets\":%u,\"activeBombs\":%u,\"players\":["
                "{\"power\":%d,\"activeShots\":%u,\"activeBombSubrecords\":%u},"
                "{\"power\":%d,\"activeShots\":%u,\"activeBombSubrecords\":%u},"
                "{\"power\":%d,\"activeShots\":%u,\"activeBombSubrecords\":%u}]}",
                checkpointMetrics.rawBytes, checkpointMetrics.sparseBytes,
                checkpointMetrics.compressedBytes, checkpointMetrics.stateBytes,
                checkpointMetrics.eventBytes, checkpointMetrics.activePlayerBullets,
                checkpointMetrics.activeBombs,
                checkpointMetrics.playerPower[0], checkpointMetrics.activePlayerBulletsByPlayer[0], checkpointMetrics.activeBombSubrecordsByPlayer[0],
                checkpointMetrics.playerPower[1], checkpointMetrics.activePlayerBulletsByPlayer[1], checkpointMetrics.activeBombSubrecordsByPlayer[1],
                checkpointMetrics.playerPower[2], checkpointMetrics.activePlayerBulletsByPlayer[2], checkpointMetrics.activeBombSubrecordsByPlayer[2]);
            if (checkpointMetricsLength <= 0 || static_cast<std::size_t>(checkpointMetricsLength) >= sizeof(checkpointMetricsJson))
                checkpointMetricsJson[0] = '\0';
            EM_ASM({
                const state = globalThis.__eaglerNetplaySpectatorPublish ||= {};
                state.checkpointFrame = $0 >>> 0;
                state.checkpointStage = $1 >>> 0;
                state.checkpointSchema = $3 >>> 0;
                state.checkpointMetrics = JSON.parse(UTF8ToString($2));
                state.checkpointRawBytes = state.checkpointMetrics.rawBytes;
                state.checkpointStateBytes = state.checkpointMetrics.stateBytes;
                state.checkpointActivePlayerBullets = state.checkpointMetrics.activePlayerBullets;
                state.checkpointActiveBombs = state.checkpointMetrics.activeBombs;
            }, frame, spectatorStage, checkpointMetricsJson, SPECTATOR_CHECKPOINT_SCHEMA);
#endif
        }
        else
        {
            // Stage history is optional. If a native initializer cannot be
            // represented, stop advertising a broken stream and keep the
            // active players running.
            DisableSpectatorPublisher(frame, "checkpoint-capture-failed");
        }
    }
    if (auditCheckpointDue)
        g_SpectatorAuditCheckpointStage = static_cast<std::uint8_t>(spectatorStage);
    const std::uint64_t costStartNs = g_PerfTelemetry ? SDL_GetTicksNS() : 0;
    if (UsePhysicalInput() && frame == 0 && !g_PhysicalLoggedFrame0Sim)
    {
        g_PhysicalLoggedFrame0Sim = true;
        std::printf(
            "netplay lan physical: FRAME0 SIM player=%u p1=0x%04x p2=0x%04x p3=0x%04x resim=%d\n",
            static_cast<unsigned>(g_LocalPlayer),
            static_cast<unsigned>(decision.inputs[0].buttons),
            static_cast<unsigned>(decision.inputs[1].buttons),
            static_cast<unsigned>(decision.inputs[2].buttons), resimulation ? 1 : 0);
    }
    ReplayFrameBinding replayBinding{};
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (MultiplayerGameplay::IsMultiplayer() && !g_GameManager.replay && g_ReplayManager)
    {
        ReplayFrameBinding &slot = g_ReplayFrameBindings[frame % g_ReplayFrameBindings.size()];
        if (!resimulation)
        {
            // Netplay frame numbers span the whole rollback session while the
            // vanilla ReplayManager frameId restarts at zero for each stage.
            // Remember that mapping on the original forward pass so a later
            // rollback can overwrite the exact EAGX frame with corrected
            // remote input without rewinding committed replay cursors.
            slot.simFrame = frame;
            slot.stage = std::clamp(g_GameManager.currentStage - 1, 0, 6);
            slot.replayFrame = g_ReplayManager->frameId;
        }
        if (slot.simFrame == frame)
            replayBinding = slot;
    }
#endif
    NormalizeMultiplayerEndingSkipHistory();
    // DrainPackets/ReconcileRollback must run before every forward pass. Never
    // discard a required restore point merely because its input just arrived.
    if (!resimulation && !g_SpectatorMode && g_Core.HasRollbackRequest())
        return -1;
    const bool snapshotAllowed = !g_SpectatorMode &&
        (!g_NoRollbackEndpoint || g_TestKeepRollbackSnapshots);
    const bool captureRollback = snapshotAllowed &&
        (!g_DemandSnapshots || g_TestKeepRollbackSnapshots || NeedsRollbackSnapshot(frame,
            g_Core.ConfirmedThroughAllRemotes(), decision.predictedMask, resimulation, g_FrontierSnapshots));
    if (snapshotAllowed && !captureRollback)
    {
        Th07Rollback::DiscardBefore(frame);
        ++g_ConfirmedOnlyTicks;
    }
    const auto snapshotStartNs = g_PerfTelemetry ? SDL_GetTicksNS() : 0;
    if (captureRollback && !Th07Rollback::BeginFrame(frame))
        return -1;
    if (captureRollback)
    {
        ++g_SnapshotTicks;
        if (g_PerfTelemetry) g_SnapshotCostNs += SDL_GetTicksNS() - snapshotStartNs;
    }
    InjectDenseBulletLoad(frame);
    SideEffects::SetSpeculative(resimulation);
    // Legacy/global raw-input owners still need one deterministic word. Use
    // the synchronized union on every peer while Player consumes its own
    // slot below. This also prevents Supervisor from re-sampling hardware
    // during rollback resimulation.
    Input::SetReplayOverride(CombinedButtons(decision));
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    TouchTraceSlot *touchTrace = nullptr;
    if (g_TouchEquivalentAbsorb)
    {
        touchTrace = &g_TouchTraces[frame % g_TouchTraces.size()];
        *touchTrace = {};
        touchTrace->frame = frame;
        const float minX = g_GameManager.playerMovementAreaTopLeftPos.x;
        const float maxX = minX + g_GameManager.playerMovementAreaSize.x;
        const float minY = g_GameManager.playerMovementAreaTopLeftPos.y;
        const float maxY = minY + g_GameManager.playerMovementAreaSize.y;
        for (std::uint8_t player = 0; player < g_PlayerCount; ++player)
        {
            DirectTouchFrameTrace &trace = touchTrace->players[player];
            trace.frame = frame;
            trace.used = decision.inputs[player];
            const ZunVec3 &position = g_Players[player].pos;
            trace.leftMargin = position.x - minX;
            trace.rightMargin = maxX - position.x;
            trace.topMargin = position.y - minY;
            trace.bottomMargin = maxY - position.y;
        }
    }
#endif
    Input::SetPlayerInputOverrides(decision.inputs.data(), g_PlayerCount);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (touchTrace)
    {
        const auto &touchStates = Input::GetDirectTouchStates();
        for (std::uint8_t player = 0; player < g_PlayerCount; ++player)
            touchTrace->players[player].applied = touchStates[player];
    }
#endif
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (UseStageTransitionTest() && frame >= 3000u && g_GameManager.currentStage == 1)
    {
        // Stage-transition smoke fixture: once the real Stage 1 boss can take
        // damage, deplete its remaining life and let the normal ECL death,
        // results, Supervisor, and GameManager::RegisterChain flow advance.
        // This never writes currentStage or the GUI completion flags.
        for (Enemy *boss : g_EnemyManager.bosses)
        {
            if (!boss || !boss->active || !boss->isBoss || !boss->canBeDamaged ||
                boss->life <= 0)
                continue;
            boss->life = 0;
            std::printf(
                "netplay lan stage: FIXTURE stage-transition frame=%u confirmed=%u predictedMask=0x%x\n",
                frame,
                static_cast<unsigned>(g_Core.ConfirmedThroughAllRemotes()),
                static_cast<unsigned>(decision.predictedMask));
        }
    }
#endif
    const std::uint8_t stageBeforeCalc =
        static_cast<std::uint8_t>(g_GameManager.currentStage);
    const auto calcStartNs = g_PerfTelemetry ? SDL_GetTicksNS() : 0;
    const int result = g_Chain.RunCalcChain();
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (touchTrace)
    {
        const auto &touchStates = Input::GetDirectTouchStates();
        for (std::uint8_t player = 0; player < g_PlayerCount; ++player)
            touchTrace->players[player].remaining = touchStates[player];
    }
#endif
    UpdateTeamWipeRetryCountdown();
    if (g_PerfTelemetry)
        (resimulation ? g_ResimulationCalcCostNs : g_ForwardCalcCostNs) += SDL_GetTicksNS() - calcStartNs;
    if (g_TestDenseBullets != 0 && !resimulation && frame >= 300u)
    {
        const auto bullets = static_cast<std::uint32_t>(std::max(0, g_BulletManager.bulletCount));
        ++g_DenseObservedFrames;
        g_DenseMinBullets = std::min(g_DenseMinBullets, bullets);
        g_DenseBulletSum += bullets;
    }
#ifdef __EMSCRIPTEN__
    if (ProductionLanMode() && !resimulation &&
        (frame == 0u || ((frame + 1u) % 15u) == 0u))
    {
        EM_ASM({
            globalThis.__eaglerNetplayTeamWipeTimer = $0;
            const playerStates = globalThis.__eaglerNetplayPlayerStates ||= new Array(3);
            playerStates[0] = $1;
            playerStates[1] = $2;
            playerStates[2] = $3;
            const pauseState = globalThis.__eaglerNetplayPauseState ||= new Array(3);
            pauseState[0] = $4;
            pauseState[1] = $5;
            pauseState[2] = $6;
        }, g_teamWipeRetryFrames,
           static_cast<int>(g_Players[0].playerState),
           static_cast<int>(g_Players[1].playerState),
           static_cast<int>(g_Players[2].playerState),
           g_GameManager.isPaused ? 1 : 0,
           g_GameManager.isInPauseMenu ? 1 : 0,
           g_GameManager.isTimeStopped ? 1 : 0);
    }
#endif
    const bool stageChangedDuringCalc =
        static_cast<std::uint8_t>(g_GameManager.currentStage) != stageBeforeCalc;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (g_GameManager.currentStage > g_HighestStageObserved)
    {
        g_HighestStageObserved = static_cast<std::uint8_t>(g_GameManager.currentStage);
        std::printf("netplay lan stage: STAGE player=%u stage=%u frame=%u resim=%d\n",
                    static_cast<unsigned>(g_LocalPlayer),
                    static_cast<unsigned>(g_HighestStageObserved), frame,
                    resimulation ? 1 : 0);
        if (g_HighestStageObserved >= 2)
            g_StageTransitionObserved = true;
#ifdef __EMSCRIPTEN__
        EM_ASM({
            globalThis.__eaglerNetplayLanHighestStage = $0;
            globalThis.__eaglerNetplayLanStageTransitionObserved = $1 !== 0;
        }, static_cast<int>(g_HighestStageObserved), g_StageTransitionObserved ? 1 : 0);
#endif
    }
#endif
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (UsePauseCycle())
    {
        if (g_GameManager.isInPauseMenu || g_GameManager.isPaused)
            g_PauseObserved = true;
        else if (g_PauseObserved && frame > 680)
            g_PauseResumeObserved = true;
    }
#endif
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    bool allInputSlotsMatch = true;
    for (std::uint8_t player = 0; player < g_PlayerCount; ++player)
    {
        if (g_CurFrameGameInputs[player] != decision.inputs[player].buttons)
        {
            allInputSlotsMatch = false;
            // The original ReplayManager::RegisterChain contract clears both
            // input words while the next stage is installed. Accept only that
            // exact all-zero lifecycle boundary; the following frame must
            // prove that synchronized per-player input resumed.
            bool stageTransitionReset = stageChangedDuringCalc;
            for (std::uint8_t slot = 0; slot < g_PlayerCount; ++slot)
            {
                if (g_CurFrameGameInputs[slot] != 0 ||
                    g_LastFrameGameInputs[slot] != 0)
                    stageTransitionReset = false;
            }
            if (stageTransitionReset)
            {
                g_StageTransitionInputResetObserved = true;
                continue;
            }
            std::printf(
                "netplay lan stage: INPUT_MISMATCH frame=%u local=%u slot=%u expected=0x%04x actual=0x%04x raw=0x%04x lastRaw=0x%04x notInMenu=%d paused=%d pauseMenu=%d retryMenu=%d pauseState=%d resim=%d\n",
                frame, static_cast<unsigned>(g_LocalPlayer),
                static_cast<unsigned>(player),
                static_cast<unsigned>(decision.inputs[player].buttons),
                static_cast<unsigned>(g_CurFrameGameInputs[player]),
                static_cast<unsigned>(g_CurFrameRawInput),
                static_cast<unsigned>(g_LastFrameRawInput),
                g_GameManager.notInMenu ? 1 : 0,
                g_GameManager.isPaused ? 1 : 0,
                g_GameManager.isInPauseMenu ? 1 : 0,
                g_GameManager.isInRetryMenu ? 1 : 0,
                g_AsciiManager.pauseMenu.curState,
                resimulation ? 1 : 0);
            Fail("per-player input slot mismatch");
            return -1;
        }
        if (decision.inputs[player].buttons != 0 ||
            decision.inputs[player].analogMode != AnalogMode::None)
            g_InputSlotObservedMask |= static_cast<std::uint8_t>(1u << player);
    }
    if (g_StageTransitionObserved && !stageChangedDuringCalc && allInputSlotsMatch)
        g_PostTransitionInputObserved = true;
    if (decision.inputs[0] != decision.inputs[1] ||
        (g_PlayerCount >= 3 && decision.inputs[2] != decision.inputs[0]))
        g_IndependentInputSlotsObserved = true;
#endif
    Input::ClearPlayerButtonOverrides();
    Input::ClearReplayOverride();
    SideEffects::SetSpeculative(false);
    g_PeakEnemies = std::max(
        g_PeakEnemies, static_cast<std::uint32_t>(g_EnemyManager.enemyCountReal));
    g_PeakBullets = std::max(
        g_PeakBullets, static_cast<std::uint32_t>(g_BulletManager.bulletCount));
#ifdef __EMSCRIPTEN__
    if (g_PerfTelemetry && !resimulation)
    {
        std::uint32_t activeBombMask = 0;
        for (std::uint8_t player = 0; player < g_PlayerCount; ++player)
            if (g_Players[player].bombInfo.isInUse) activeBombMask |= (1u << player);
        if (activeBombMask)
            EM_ASM({ globalThis.__eaglerNetplayBombObservedMask = (globalThis.__eaglerNetplayBombObservedMask || 0) | $0; }, activeBombMask);
    }
#endif
    g_PeakItems = std::max(
        g_PeakItems, static_cast<std::uint32_t>(g_ItemManager.activeItemCount));
    std::uint32_t activeLasers = 0;
    for (const auto &laser : g_BulletManager.lasers)
        activeLasers += laser.isInUse ? 1u : 0u;
    g_PeakLasers = std::max(g_PeakLasers, activeLasers);
    if (UseStageTransitionTest() && !resimulation && (frame % 1800u) == 0u)
    {
        const i32 msg = g_Gui.impl ? g_Gui.impl->msg.currentMsgIdx : -999;
        const i32 msgTime = g_Gui.impl ? g_Gui.impl->msg.timer.GetCurrent() : -1;
        const i32 finishedStage = g_Gui.impl ? g_Gui.impl->finishedStage : -1;
        const i32 scoreTransition = g_Gui.impl ? g_Gui.impl->transitionToScoreScreen : -1;
        EclTimeline &timeline = g_EnemyManager.timelines[0];
        const i32 timelineTime = timeline.timelineTime.GetCurrent();
        const i32 timelineInstrTime = timeline.timelineInstr ? timeline.timelineInstr->time : -999;
        const i32 timelineOpcode = timeline.timelineInstr ? timeline.timelineInstr->opcode : -999;
        Enemy *boss = g_EnemyManager.bosses[0];
        std::printf(
            "netplay lan stage: PROGRESS player=%u frame=%u stage=%d stageFrames=%d msg=%d msgTime=%d finished=%d scoreTransition=%d enemies=%d timeline=%d/%d/%d boss=%d bossLife=%d/%d bossTimer=%d bossDamage=%d bossPos=%.1f/%.1f playerX=%.1f/%.1f/%.1f playerState=%d/%d/%d lives=%d/%d/%d\n",
            static_cast<unsigned>(g_LocalPlayer), frame,
            g_GameManager.currentStage, g_GameManager.framesThisStage,
            msg, msgTime, finishedStage, scoreTransition,
            g_EnemyManager.enemyCountReal, timelineTime, timelineInstrTime, timelineOpcode,
            boss ? 1 : 0, boss ? boss->life : -1, boss ? boss->maxLife : -1,
            boss ? boss->timer.GetCurrent() : -1, boss ? boss->canBeDamaged : 0,
            boss ? boss->pos.x : -999.0f, boss ? boss->pos.y : -999.0f,
            g_Players[0].pos.x, g_Players[1].pos.x,
            g_Players[2].pos.x,
            g_Players[0].playerState, g_Players[1].playerState, g_Players[2].playerState,
            GetPlayerLives(0), GetPlayerLives(1), GetPlayerLives(2));
    }
    if (captureRollback && !Th07Rollback::EndFrame())
        return -1;
    if (!g_SpectatorMode && !g_Core.MarkSimulated(frame, decision))
        return -1;
    if (g_SpectatorPublisherReady)
    {
        SpectatorStagePublisher::Frame record;
        record.frame = frame;
        record.stage = spectatorStage;
        record.schema = spectatorSchema;
        record.initializer = std::move(spectatorInitializer);
        record.inputs = decision.inputs;
        record.hasInputs = true;
        record.bossSlots = g_SpectatorBossSlotsThisFrame;
        if (!g_SpectatorPublisher.Record(std::move(record)))
        {
            DisableSpectatorPublisher(frame, "record-rejected");
        }
    }
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (replayBinding.simFrame == frame && replayBinding.stage >= 0 &&
        replayBinding.replayFrame >= 0)
    {
        // The first forward pass may contain predicted remote input.  EAGX is
        // indexed by replay frame, so rewriting the same slot here makes every
        // rollback resimulation replace that prediction with the final
        // decision that actually produced the surviving game state.
        ReplayExtension::RecordMultiplayerFrame(
            replayBinding.stage, replayBinding.replayFrame,
            decision.inputs.data(), g_PlayerCount);
    }
#endif
    if (captureRollback)
        g_MaxSnapshotBytes = std::max(g_MaxSnapshotBytes, Th07Rollback::CapturedBytes(frame));
#ifdef __EMSCRIPTEN__
    // Rollback-audit sampling is independent of the private native transition
    // fixture, so viewers can compare exact future frames without applying
    // fixture-only gameplay mutations.
    const std::uint32_t canonicalInterval =
        (UseStageTransitionTest() || RollbackAuditEnabled()) ? 20u : 300u;
    if (ProductionLanMode() && ((frame + 1u) % canonicalInterval) == 0u)
    {
        const auto canonicalStartNs = g_PerfTelemetry ? SDL_GetTicksNS() : 0;
        // This lives inside SimulateFrame rather than the outer driver so a
        // rollback resimulation overwrites the sample for the same logical
        // frame. Consumers only trust a sample once that frame is confirmed.
        const auto sample = Th07CanonicalHash::Capture();
        char total[17];
        HashToHex(sample.composite, total);
        EM_ASM({
            const hash = UTF8ToString($0);
            globalThis.__eaglerNetplayLanHash = hash;
            globalThis.__eaglerNetplayLanHashes[String($1)] = hash;
        }, total, frame + 1u);
        if (DetailedCanonicalTelemetry())
        {
            char meta[17], multiplayer[17], stage[17], player[17];
            char metaRng[17], metaGame[17], metaInput[17], metaSupervisor[17];
            char enemies[17], bullets[17], items[17];
            char player0[17], player1[17], player2[17];
            HashToHex(sample.meta, meta);
            HashToHex(sample.metaRng, metaRng);
            HashToHex(sample.metaGame, metaGame);
            HashToHex(sample.metaInput, metaInput);
            HashToHex(sample.metaSupervisor, metaSupervisor);
            HashToHex(sample.multiplayer, multiplayer);
            HashToHex(sample.stage, stage);
            HashToHex(sample.player, player);
            HashToHex(sample.enemies, enemies);
            HashToHex(sample.bullets, bullets);
            HashToHex(sample.items, items);
            HashToHex(sample.player0, player0);
            HashToHex(sample.player1, player1);
            HashToHex(sample.player2, player2);
            EM_ASM({
                globalThis.__eaglerNetplayLanCanonical ||= Object.create(null);
                const entry = Object.create(null);
                entry.meta = UTF8ToString($0);
                entry.multiplayer = UTF8ToString($1);
                entry.stage = UTF8ToString($2);
                entry.player = UTF8ToString($3);
                entry.enemies = UTF8ToString($4);
                entry.bullets = UTF8ToString($5);
                entry.items = UTF8ToString($6);
                entry.counts = [];
                entry.counts[0] = $7;
                entry.counts[1] = $8;
                entry.counts[2] = $9;
                entry.counts[3] = $10;
                globalThis.__eaglerNetplayLanCanonical[String($11)] = entry;
            }, meta, multiplayer, stage, player, enemies, bullets, items,
               sample.enemyCount, sample.bulletCount, sample.laserCount, sample.itemCount,
               frame + 1u);
            EM_ASM({
                globalThis.__eaglerNetplayLanCanonicalPlayers ||= Object.create(null);
                const players = [];
                players[0] = UTF8ToString($0);
                players[1] = UTF8ToString($1);
                players[2] = UTF8ToString($2);
                globalThis.__eaglerNetplayLanCanonicalPlayers[String($3)] = players;
            }, player0, player1, player2, frame + 1u);
            EM_ASM({
                globalThis.__eaglerNetplayLanCanonicalMeta ||= Object.create(null);
                const detail = [];
                detail[0] = UTF8ToString($0);
                detail[1] = UTF8ToString($1);
                detail[2] = UTF8ToString($2);
                detail[3] = UTF8ToString($3);
                globalThis.__eaglerNetplayLanCanonicalMeta[String($4)] = detail;
            }, metaRng, metaGame, metaInput, metaSupervisor, frame + 1u);
        }
        if (g_PerfTelemetry) g_CanonicalCostNs += SDL_GetTicksNS() - canonicalStartNs;
    }
#endif
    if (resimulation)
        ++g_ResimulatedFrames;
#ifdef __EMSCRIPTEN__
    if (g_PerfTelemetry && !resimulation && result != 0 && result != -1)
        EM_ASM({ globalThis.__th07InputLatencyProbe?.simulate($0); }, frame);
#endif
    if (g_PerfTelemetry)
        (resimulation ? g_ResimulationCostNs : g_ForwardCostNs) += SDL_GetTicksNS() - costStartNs;
    return result;
}

bool ReconcileRollback()
{
    g_ReconcileShouldYield = false;
    if (g_NoRollbackEndpoint)
        return !g_Core.HasRollbackRequest();

    if (!g_IncrementalReconcile)
    {
        if (!g_Core.HasRollbackRequest())
            return true;
        const std::uint64_t costStartNs = g_PerfTelemetry ? SDL_GetTicksNS() : 0;
        const std::uint32_t rollback = g_Core.RollbackFrame();
        const std::uint32_t last = g_Core.LastSimulatedFrame();
        if (last == INVALID_FRAME || rollback > last)
        {
            g_Core.ClearRollbackRequest();
            return true;
        }
        std::uint32_t replayFrom = rollback;
        if (!Th07Rollback::RestoreTo(rollback, &replayFrom))
        {
            if (Th07Rollback::Failed())
                return false;
            std::printf(
                "netplay lan stage: WARN rollback correction abandoned pending=%u last=%u\n",
                rollback, last);
            g_Core.ClearRollbackRequest();
            return true;
        }
        ++g_RollbackCount;
        g_MaxRollbackSpan = std::max(g_MaxRollbackSpan, last - rollback + 1);
        g_Core.ClearRollbackRequest();
        for (std::uint32_t frame = replayFrom; frame <= last; ++frame)
        {
            const FrameDecision decision = g_Core.PrepareFrame(frame);
            if (!decision.canAdvance)
                return false;
            const int result = SimulateFrame(frame, decision, true);
            if (result == 0 || result == -1)
                return false;
        }
        if (g_PerfTelemetry)
        {
            const auto elapsed = SDL_GetTicksNS() - costStartNs;
            g_ReconcileCostNs += elapsed;
            g_MaxReconcileCostNs = std::max(g_MaxReconcileCostNs, elapsed);
        }
        char correctedBgmPath[256];
        if (SideEffects::ConsumeCorrectedBgmPlay(correctedBgmPath, sizeof(correctedBgmPath)))
            g_Supervisor.PlayAudio(correctedBgmPath);
        return true;
    }

    const std::uint64_t sliceStartNs = SDL_GetTicksNS();
    if (!g_ReconcileActive)
    {
        if (!g_Core.HasRollbackRequest())
            return true;
        const std::uint32_t rollback = g_Core.RollbackFrame();
        const std::uint32_t last = g_Core.LastSimulatedFrame();
        if (last == INVALID_FRAME || rollback > last)
        {
            g_Core.ClearRollbackRequest();
            return true;
        }
        std::uint32_t replayFrom = rollback;
        if (!Th07Rollback::RestoreTo(rollback, &replayFrom))
        {
            if (Th07Rollback::Failed())
                return false;
            std::printf(
                "netplay lan stage: WARN rollback correction abandoned pending=%u last=%u\n",
                rollback, last);
            g_Core.ClearRollbackRequest();
            return true;
        }
        ++g_RollbackCount;
        g_MaxRollbackSpan = std::max(g_MaxRollbackSpan, last - rollback + 1);
        g_Core.ClearRollbackRequest();
        g_ReconcileActive = true;
        SetReconcileTimerMode(true);
        g_ReconcileNextFrame = replayFrom;
        g_ReconcileLastFrame = last;
    }

    std::uint32_t sliceFrames = 0;
    while (g_ReconcileNextFrame <= g_ReconcileLastFrame &&
           RollbackReplayBudget::CanContinue(
               sliceFrames, SDL_GetTicksNS() - sliceStartNs))
    {
        const std::uint32_t frame = g_ReconcileNextFrame;
        const FrameDecision decision = g_Core.PrepareFrame(frame);
        if (!decision.canAdvance)
        {
            SetReconcileTimerMode(false);
            return false;
        }
        const int result = SimulateFrame(frame, decision, true);
        if (result == 0 || result == -1)
        {
            SetReconcileTimerMode(false);
            return false;
        }
        ++g_ReconcileNextFrame;
        ++sliceFrames;
    }

    const std::uint64_t elapsed = SDL_GetTicksNS() - sliceStartNs;
    ++g_ReconcileSliceCount;
    g_MaxReconcileSliceFrames = std::max(g_MaxReconcileSliceFrames, sliceFrames);
    if (g_ReconcileNextFrame > g_ReconcileLastFrame)
    {
        g_ReconcileActive = false;
        SetReconcileTimerMode(false);
        g_ReconcileNextFrame = g_ReconcileLastFrame = INVALID_FRAME;
        char correctedBgmPath[256];
        if (SideEffects::ConsumeCorrectedBgmPlay(correctedBgmPath, sizeof(correctedBgmPath)))
            g_Supervisor.PlayAudio(correctedBgmPath);
    }
    g_ReconcileShouldYield = g_ReconcileActive ||
        elapsed >= RollbackReplayBudget::SliceBudgetNs;
    if (g_ReconcileShouldYield)
        ++g_ReconcileYieldCount;
    if (g_PerfTelemetry)
    {
        g_ReconcileCostNs += elapsed;
        g_MaxReconcileCostNs = std::max(g_MaxReconcileCostNs, elapsed);
    }
    return true;
}
} // namespace

bool Active()
{
    return RequestedInternal() && g_Active;
}

bool TransportReady()
{
    return !ProductionLanMode() ||
           (g_ProductionTransportStarted && TransportIsOpen());
}

bool LastTickAdvanced()
{
    return !RequestedInternal() || g_LastTickAdvanced;
}

bool ReconciliationInProgress()
{
    return RequestedInternal() && g_ReconcileActive;
}

bool Requested()
{
    return RequestedInternal();
}

double SimulationIntervalScale()
{
    return ProductionLanMode() && !g_DisableTimeSyncPacing
        ? g_SimulationIntervalScale : 1.0;
}

bool PerformanceTelemetryEnabled()
{
    return g_PerfTelemetry && g_Active && g_SimFrame >= (g_TestDenseBullets ? 300u : 120u) &&
           (!ProbeMode() || g_SimFrame < g_TestFrames);
}

void RecordPresentationCost(double driverMs, double drawMs)
{
#ifdef __EMSCRIPTEN__
    // Per-presentation deltas separate real work from task/transport waiting.
    // Bounded diagnostic records only; no GL readback and no pacing decisions.
    EM_ASM({
        const p = globalThis.__eaglerNetplayPerf;
        if (!p) return;
        const now = performance.now();
        const previous = p.tracePrevious;
        const state = ({frame: $0, snapshot: $1, canonical: $2, reconcile: $3,
            calc: $4, arena: $5, heap: HEAPU8.byteLength, time: now, bullets: $8});
        if (previous) {
            const gap = now - previous.time;
            // Report the requested dense-bullet phase independently, without
            // deleting ANY stalls from whole-run measurements. Both endpoints
            // must be dense so entering/leaving the workload is not mislabeled.
            if ($8 >= 500 && previous.bullets >= 500 && $0 >= 120) {
                const samples = p.densePresentMs ||= [];
                if (samples.length < 12000) samples.push(gap);
                p.denseElapsedMs = (p.denseElapsedMs || 0) + gap;
                p.denseAdvanced = (p.denseAdvanced || 0) + $0 - previous.frame;
                p.denseBulletSamples = (p.denseBulletSamples || 0) + 1;
                p.denseBulletSum = (p.denseBulletSum || 0) + $8;
            }
            if (gap >= 40 || $6 + $7 >= 25 || state.heap !== previous.heap) {
                const events = p.longFrames ||= [];
                if (events.length < 384) events.push({frame: $0, advanced: $0 - previous.frame,
                    gap, driver: $6, draw: $7, bullets: $8, items: $9,
                    snapshot: $1 - previous.snapshot, canonical: $2 - previous.canonical,
                    reconcile: $3 - previous.reconcile, calc: $4 - previous.calc,
                    arenaGrowths: $5 - previous.arena, heapBytes: state.heap,
                    heapGrowth: state.heap - previous.heap});
            }
        }
        p.tracePrevious = state;
        p.canonicalMs = $2;
        p.arenaGrowths = $5;
        globalThis.__th07InputLatencyProbe?.present($0, $8);
    }, g_SimFrame, g_SnapshotCostNs / 1000000.0, g_CanonicalCostNs / 1000000.0,
       g_ReconcileCostNs / 1000000.0,
       (g_ForwardCalcCostNs + g_ResimulationCalcCostNs) / 1000000.0,
       static_cast<double>(Th07Rollback::ArenaGrowths()), driverMs, drawMs,
       g_BulletManager.bulletCount, g_ItemManager.activeItemCount);
    // Bounded observations of actual Present calls, distinct from an unrelated
    // RAF counter. Draw timing includes submission/swap, not GPU completion.
    EM_ASM({
        const perf = globalThis.__eaglerNetplayPerf;
        if (!perf) return;
        const draw = perf.drawMs ||= [];
        const driver = perf.driverMs ||= [];
        const gaps = perf.presentGapMs ||= [];
        const now = performance.now();
        if (draw.length < 4096) { draw.push($1); driver.push($0); }
        if (perf.lastPresentMs && gaps.length < 4096) gaps.push(now - perf.lastPresentMs);
        perf.lastPresentMs = now;
        perf.presented = (perf.presented || 0) + 1;
        const positions = perf.positions ||= new Array(6);
        positions[0] = $2; positions[1] = $3;
        positions[2] = $4; positions[3] = $5;
        positions[4] = $6; positions[5] = $7;
        perf.capturedTouchX = $8; perf.capturedTouchY = $9;
    }, driverMs, drawMs,
       g_Players[0].pos.x, g_Players[0].pos.y,
       g_Players[1].pos.x, g_Players[1].pos.y,
       g_Players[2].pos.x, g_Players[2].pos.y,
       g_CapturedTouchX, g_CapturedTouchY);
#else
    (void)driverMs;
    (void)drawMs;
#endif
}

int RunCalcChain()
{
    g_LastTickAdvanced = false;
    if (!RequestedInternal())
    {
        g_LastTickAdvanced = true;
        return g_Chain.RunCalcChain();
    }
    if (!g_Initialized && g_SpectatorRunRetired)
    {
        // Local Result/MainMenu may keep running, but a one-shot spectator
        // Runtime cannot silently become another netplay generation.
        g_LastTickAdvanced = true;
        return g_Chain.RunCalcChain();
    }
    if (!g_Initialized && ProductionLanMode() &&
        !StartProductionTransportEarly())
    {
        Fail("transport preconnect");
        return -1;
    }
    if (ProbeMode() && g_Done)
    {
        // A finished test endpoint still owes the slower endpoint its final
        // input/ACK. Do not strand it behind the last redundant packet window.
        if (g_Initialized && !g_SpectatorMode && g_SimFrame >= g_TestFrames && TransportIsOpen())
        {
            ++g_DriverTicks;
            if ((g_DriverTicks % 3u) == 0u)
            {
                (void)DrainPackets();
                (void)SendTailKeepalive();
            }
        }
        return CHAIN_CALLBACK_RESULT_CONTINUE;
    }
    if (g_Initialized && !SessionStillOwnsStageState())
    {
        // Gameplay/retry/Ending ownership is one deterministic rollback
        // session. Once vanilla scene management takes over, retire that
        // session. A retry or later new run starts again from frame zero while
        // reusing only the already-established browser transport.
        RetireGameplaySession();
    }
    if (!g_Initialized && !EligibleForInitialNetplayStart())
    {
        if (!InitialNetplayBootstrapInProgress())
        {
            // The room transport intentionally survives the gameplay session,
            // but Result/Ending/MainMenu are local post-game scenes and must
            // receive ordinary UI input. Neutral lanes belong only to the
            // actual pre-frame-zero GameManager construction window.
            g_LastTickAdvanced = true;
            return g_Chain.RunCalcChain();
        }
        // The room has already committed this run to netplay, but TH07 still
        // needs a few normal calc ticks to construct Stage/Player/ReplayManager
        // before the deterministic gate can take ownership. Keep those
        // initialization callbacks running, while preventing local keyboard,
        // controller or touch state from moving only this machine's player
        // before HELLO/READY. Otherwise peers can enter synchronized frame zero
        // from different world states and rollback cannot repair the offset.
        std::array<FrameInput, TH07_MULTI_MAX_PLAYERS> neutralInputs{};
        Input::SetReplayOverride(0);
        Input::SetPlayerInputOverrides(neutralInputs.data(), g_PlayerCount);
        const int result = g_Chain.RunCalcChain();
        Input::ClearPlayerButtonOverrides();
        Input::ClearReplayOverride();
        g_LastTickAdvanced = true;
        return result;
    }
    if (!g_Initialized)
    {
        g_Initialized = true;
        if (!Initialize())
        {
            Fail("initialize");
            return -1;
        }
    }

    ++g_DriverTicks;
#ifdef __EMSCRIPTEN__
    if (g_PerfTelemetry)
    {
        const std::uint32_t pendingRollback = g_Core.HasRollbackRequest()
            ? g_Core.RollbackFrame() : INVALID_FRAME;
        EM_ASM({
            globalThis.__eaglerNetplayDebugCaptureFrame = $0;
            globalThis.__eaglerNetplayDebugReconcileActive = !!$1;
            globalThis.__eaglerNetplayDebugReconcileNext = $2;
            globalThis.__eaglerNetplayDebugReconcileLast = $3;
            globalThis.__eaglerNetplayDebugCoreLast = $4;
            globalThis.__eaglerNetplayDebugRollback = $5;
            globalThis.__eaglerNetplayDebugLocalConfirmed = $6;
            globalThis.__eaglerNetplayDebugDriverTick = $7;
            globalThis.__eaglerNetplayDebugReconcileYield = !!$8;
        }, g_InputCaptureFrame, g_ReconcileActive ? 1 : 0,
           g_ReconcileNextFrame, g_ReconcileLastFrame,
           g_Core.LastSimulatedFrame(), pendingRollback,
           g_Core.ConfirmedThrough(g_LocalPlayer), g_DriverTicks,
           g_ReconcileShouldYield ? 1 : 0);
    }
#endif
    if (g_SpectatorMode)
    {
        if (!DrainSpectatorFrames())
        {
            Fail("invalid spectator stream");
            return -1;
        }
        if (TransportFailed())
        {
            Fail("spectator transport");
            return -1;
        }
        const bool fastForwarding = g_SpectatorPlayback.FastForwarding();
        const std::uint64_t catchUpStartedNs = fastForwarding ? SDL_GetTicksNS() : 0;
        const bool stepped = g_SpectatorPlayback.Step(
            [](const SpectatorStageEvent &checkpoint) {
                return RestoreSpectatorCheckpoint(checkpoint);
            },
            [&](const SpectatorFramePacket &packet, bool seeking) {
                if (packet.frame != g_SimFrame)
                    return false;
                FrameDecision decision;
                decision.canAdvance = true;
                decision.inputs = packet.inputs;
                const int result = SimulateFrame(packet.frame, decision, seeking || fastForwarding);
                if (result == 0 || result == -1)
                    return false;
                ++g_SimFrame;
                return true;
            },
            [&] {
                return fastForwarding && SDL_GetTicksNS() - catchUpStartedNs >= 4000000u;
            }, fastForwarding ? 32u : 1u);
        if (!stepped)
        {
            Fail("spectator native restore or tick failed");
            return -1;
        }
#ifdef __EMSCRIPTEN__
        EM_ASM({
            globalThis.__eaglerNetplayLanFrame = $0;
            globalThis.__eaglerNetplaySpectator = true;
        }, g_SimFrame);
#endif
        g_LastTickAdvanced = g_SpectatorPlayback.Ready() && !g_SpectatorPlayback.Paused();
        return CHAIN_CALLBACK_RESULT_CONTINUE;
    }
    if (!DrainPackets())
    {
        Fail("invalid packet");
        return -1;
    }
    if (TransportFailed())
    {
        Fail("transport");
        return -1;
    }
    if (RemoteInputsTimedOut())
    {
        Fail("remote input timeout");
        return -1;
    }

    if (!g_Session.CanStart())
    {
        if (g_DriverTicks == 120)
        {
            std::printf(
                "netplay lan stage: GATE player=%u sent=%u recv=%u localReady=%d peer1=%d/%d peer2=%d/%d\n",
                static_cast<unsigned>(g_LocalPlayer), g_SessionPacketsSent,
                g_SessionPacketsReceived, g_Session.LocalReady() ? 1 : 0,
                g_Session.PeerHello(1) ? 1 : 0,
                g_Session.PeerReady(1) ? 1 : 0,
                g_Session.PeerHello(2) ? 1 : 0,
                g_Session.PeerReady(2) ? 1 : 0);
        }
        if (UsePhysicalInput() && g_DriverTicks <= 5)
        {
            std::printf(
                "netplay lan physical: GATE player=%u tick=%u sent=%u recv=%u canReady=%d\n",
                static_cast<unsigned>(g_LocalPlayer), g_DriverTicks,
                g_SessionPacketsSent, g_SessionPacketsReceived,
                g_Session.CanSendReady() ? 1 : 0);
        }
        if (!SendSessionControl())
        {
            Fail("session control");
            return -1;
        }
        // Control packets are deliberately separated from game inputs. Frame
        // zero is not scheduled until both peers have verified the complete
        // deterministic session contract and exchanged READY.
        return CHAIN_CALLBACK_RESULT_CONTINUE;
    }
    // Put the next once-only physical sample on the network before expensive
    // reconciliation. RTC/WebSocket network threads can transmit while this
    // endpoint recomputes; waiting until afterwards adds our correction cost
    // to the OTHER endpoint's input age and can amplify a rollback feedback loop.
    // Frame numbers and the neutral lead-in are unchanged. A retry or a replay
    // never samples hardware twice for this logical capture slot.
    bool freshInputSent = false;
    // Confirmed menus need a wall-clock input pipeline: sending only the next
    // simulated frame would make each menu tick wait an entire round trip.
    // Once gameplay resumes, catch-up consumes the already captured tail; no
    // extra scheduling delay is added to normal gameplay.
    const bool sharedUi = SharedUiNeedsConfirmedInputs();
    if (sharedUi && !g_IncrementalReconcile && g_InitialFrameLag == 0 &&
        g_InputCaptureFrame <= g_SimFrame)
    {
        g_InputCaptureFrame = g_SimFrame;
        g_NextInputCaptureNs = 0;
        g_HaveLastPhysicalCapture = false;
    }
    const bool independentInputClock = g_IncrementalReconcile || g_InitialFrameLag != 0 ||
        sharedUi || g_InputCaptureFrame > g_SimFrame;
    if (!g_NoRollbackEndpoint && independentInputClock &&
        (!ProbeMode() || g_SimFrame < g_TestFrames) && TransportIsOpen())
    {
        if (!PumpBufferedLockstepInput())
        {
            Fail("incremental input pump");
            return -1;
        }
        freshInputSent = true;
    }
    else if (!g_NoRollbackEndpoint && (!ProbeMode() || g_SimFrame < g_TestFrames) &&
        TransportIsOpen() && !g_Core.HasLocalCapture(g_SimFrame))
    {
        if (!SendLocalFrame(g_SimFrame))
        {
            Fail("send local input");
            return -1;
        }
        freshInputSent = true;
    }
    // Test/negotiated frame skew: the weak endpoint intentionally begins its
    // simulation a few wall-clock ticks later while the independent capture
    // clock keeps publishing future local inputs. Once established, both
    // endpoints continue at fixed 60 Hz, preserving the offset without adding
    // ongoing local input delay.
    if (g_InitialFrameLagRemaining != 0)
    {
        --g_InitialFrameLagRemaining;
        return CHAIN_CALLBACK_RESULT_CONTINUE;
    }
    if (!ReconcileRollback())
    {
        Fail("rollback reconcile");
        return -1;
    }
    if (g_ReconcileShouldYield)
    {
        // A historical replay slice either still owns the world state or used
        // the callback budget. Do not advance the live logical frame in this
        // rAF; the outer scheduler keeps accumulator debt for the next one.
        return CHAIN_CALLBACK_RESULT_CONTINUE;
    }
    if (!CaptureConfirmedReplayAuditFrames())
    {
        Fail("Replay confirmed input audit unavailable");
        return -1;
    }
    PublishConfirmedSpectatorFrames();
#ifdef __EMSCRIPTEN__
    if (ProductionLanMode())
    {
        const std::uint32_t confirmed = g_Core.ConfirmedThroughAllRemotes();
        EM_ASM({
            globalThis.__eaglerNetplayLanConfirmed = $0;
            globalThis.__eaglerNetplayLanRollback = $1;
            globalThis.__eaglerNetplayLanResimulated = $2;
        }, confirmed, g_RollbackCount, g_ResimulatedFrames);
        if (g_PerfTelemetry && ((g_DriverTicks % 15u) == 0u || g_SimFrame >= g_TestFrames))
        {
            EM_ASM({
                const perf = globalThis.__eaglerNetplayPerf;
                perf.forwardMs = $0; perf.resimulationMs = $1; perf.reconcileMs = $2;
                perf.captures = $3; perf.driverTicks = $4; perf.maxReconcileMs = $5;
                perf.maxSnapshotBytes = $6; perf.peakBullets = $7;
                perf.snapshotBeginMs = $8; perf.snapshotTicks = $9;
                perf.confirmedOnlyTicks = $10;
                perf.forwardCalcMs = $11; perf.resimulationCalcMs = $12;
                perf.denseObservedFrames = $13; perf.denseMinBullets = $14;
                perf.denseMeanBullets = $15;
            }, g_ForwardCostNs / 1000000.0, g_ResimulationCostNs / 1000000.0,
               g_ReconcileCostNs / 1000000.0, g_LocalCaptures, g_DriverTicks,
               g_MaxReconcileCostNs / 1000000.0, g_MaxSnapshotBytes, g_PeakBullets,
               g_SnapshotCostNs / 1000000.0, g_SnapshotTicks, g_ConfirmedOnlyTicks,
               g_ForwardCalcCostNs / 1000000.0, g_ResimulationCalcCostNs / 1000000.0,
               g_DenseObservedFrames, g_DenseObservedFrames ? g_DenseMinBullets : 0u,
               g_DenseObservedFrames ? static_cast<double>(g_DenseBulletSum) / g_DenseObservedFrames : 0.0);
            EM_ASM({
                const perf = globalThis.__eaglerNetplayPerf;
                perf.mismatchTouchDelta = $0; perf.mismatchButtons = $1;
                perf.mismatchOtherAnalog = $2;
            }, g_MismatchTouchDelta, g_MismatchButtons, g_MismatchOtherAnalog);
            EM_ASM({
                const perf = globalThis.__eaglerNetplayPerf;
                perf.touchEquivalentAccepted = $0;
                perf.touchEquivalentRejected = $1;
            }, g_TouchEquivalentAccepted, g_TouchEquivalentRejected);
            EM_ASM({
                const reasons = globalThis.__eaglerNetplayPerf.touchEquivalentRejectReasons ||= [];
                reasons[0] = $0; reasons[1] = $1; reasons[2] = $2;
            }, g_TouchEquivalentRejectReasons[0], g_TouchEquivalentRejectReasons[1],
               g_TouchEquivalentRejectReasons[2]);
            EM_ASM({
                const reasons = globalThis.__eaglerNetplayPerf.touchEquivalentRejectReasons;
                reasons[3] = $0; reasons[4] = $1; reasons[5] = $2; reasons[6] = $3;
            }, g_TouchEquivalentRejectReasons[3], g_TouchEquivalentRejectReasons[4],
               g_TouchEquivalentRejectReasons[5], g_TouchEquivalentRejectReasons[6]);
            EM_ASM({
                globalThis.__eaglerNetplayPerf.restoreCopiedBytes = $0;
                globalThis.__eaglerNetplayPerf.restoreSkippedBytes = $1;
            }, static_cast<double>(Th07Rollback::RestoreCopiedBytes()),
               static_cast<double>(Th07Rollback::RestoreSkippedBytes()));
            EM_ASM({
                const perf = globalThis.__eaglerNetplayPerf;
                perf.reconcileSlices = $0;
                perf.reconcileYields = $1;
                perf.maxReconcileSliceFrames = $2;
                perf.reconcileActive = !!$3;
            }, g_ReconcileSliceCount, g_ReconcileYieldCount,
               g_MaxReconcileSliceFrames, g_ReconcileActive ? 1 : 0);
            EM_ASM({
                const perf = globalThis.__eaglerNetplayPerf;
                perf.packetDrainYields = $0;
                perf.maxPacketsDrainedPerTick = $1;
            }, g_DrainYieldCount, g_MaxPacketsDrainedPerTick);
        }
    }
#endif

    if ((!ProbeMode() || g_SimFrame < g_TestFrames) && TransportIsOpen())
    {
        if (g_NoRollbackEndpoint)
        {
            if (!PumpBufferedLockstepInput())
            {
                Fail("buffered input pump");
                return -1;
            }
        }
        else if (independentInputClock)
        {
            // Fresh capture and redundancy are owned by the independent 60 Hz
            // input clock above. Never sample the same physical slot again from
            // the simulation-frame path while live simulation is catching up.
        }
        else
        {
            // During the neutral lead-in the due input exists but the future
            // capture does not. Only the future slot proves we sampled this tick.
            const bool localPresent = g_Core.HasLocalCapture(g_SimFrame);
            if (!localPresent && !SendLocalFrame(g_SimFrame))
            {
                Fail("send local input");
                return -1;
            }
            if (!freshInputSent && localPresent && (g_DriverTicks % 3u) == 0u &&
                !SendScheduledLocalFrame(g_Core.LocalFrameForCapture(g_SimFrame)))
            {
                Fail("input retry");
                return -1;
            }
        }

        // Frame zero is the session barrier. Receive every peer's real first
        // input before gameplay advances. Later frames may use prediction.
        if (g_SimFrame == 0 && g_Core.ConfirmedThroughAllRemotes() == INVALID_FRAME)
        {
            if (UsePhysicalInput() && !g_PhysicalLoggedFrame0Wait)
            {
                g_PhysicalLoggedFrame0Wait = true;
                std::printf(
                    "netplay lan physical: FRAME0 WAIT player=%u recv=%u session=%u/%u\n",
                    static_cast<unsigned>(g_LocalPlayer), g_ReceivedPackets,
                    g_SessionPacketsSent, g_SessionPacketsReceived);
            }
            // Even after our local gate opens, the peer may still be missing
            // our READY. Keep retransmitting READY until receiving peer frame
            // zero, which is an implicit acknowledgement that it crossed the
            // same session gate.
            if ((g_DriverTicks % 3u) == 0 && !SendSessionControl(true))
            {
                Fail("session ready keepalive");
                return -1;
            }
            return CHAIN_CALLBACK_RESULT_CONTINUE;
        }

        // Rewind ordinary Pause navigation with the existing menu snapshot.
        // Keep screenshot initialization, final clock/lifetime steps and
        // unsupported Retry/Ending phases on the confirmed input pipeline.
        if (SharedUiNeedsConfirmedInputs() &&
            g_Core.ConfirmedThroughAllRemotes() < g_SimFrame)
            return CHAIN_CALLBACK_RESULT_CONTINUE;

        // A buffered endpoint never creates predicted logical state. If the
        // stronger peer's input did not arrive inside its scheduling lead,
        // yield this callback and wait for the exact sample rather than making
        // the constrained endpoint pay snapshot/rollback cost.
        if (g_NoRollbackEndpoint && !AllRemoteInputsReady(g_SimFrame))
            return CHAIN_CALLBACK_RESULT_CONTINUE;

        const FrameDecision decision = g_Core.PrepareFrame(g_SimFrame);
        if (!decision.canAdvance)
            return CHAIN_CALLBACK_RESULT_CONTINUE;
        PublishPeerDiagnostics(decision.predictedMask);
        if (decision.predictedMask)
            ++g_PredictedFrames;
        const int result = SimulateFrame(g_SimFrame, decision, false);
        if (result == 0 || result == -1)
            return result;
        ++g_SimFrame;
        PublishConfirmedSpectatorFrames();
#ifdef __EMSCRIPTEN__
        if (ProductionLanMode())
            EM_ASM({ globalThis.__eaglerNetplayLanFrame = $0; }, g_SimFrame);
#endif
        g_LastTickAdvanced = true;
        return result;
    }

    if (!DrainPackets() || !ReconcileRollback())
    {
        Fail("final reconcile");
        return -1;
    }
    if (g_ReconcileShouldYield)
        return CHAIN_CALLBACK_RESULT_CONTINUE;
    if (!CaptureConfirmedReplayAuditFrames())
    {
        Fail("Replay confirmed input audit unavailable");
        return -1;
    }
    // A low-rate tail keepalive is part of the protocol behavior, not a
    // transport retransmission. It closes the otherwise unavoidable hole where
    // the very last input or its ACK is the packet that gets lost.
    if ((g_DriverTicks % 3u) == 0 && !SendTailKeepalive())
    {
        Fail("tail keepalive");
        return -1;
    }

#if defined(__EMSCRIPTEN__) && defined(TH_ENABLE_MULTIPLAYER_GAMEPLAY) && defined(TH_DEV_TOOLS)
    if (UseReplayPlaybackCycle() && !g_ReplayPlaybackCycleDispatched &&
        g_SimFrame >= g_TestFrames &&
        g_Core.ConfirmedThroughAllRemotes() >= g_TestFrames - 1 &&
        !g_Core.HasRollbackRequest())
    {
        // Test-only short end-to-end gate. Save only after every input in the
        // recording window is authoritative, then use TH07's established
        // gameplay-quit Supervisor transition to reach the real Replay menu.
        if (ReplayExtension::DebugExpectedMultiplayerPlaybackFrames() != g_TestFrames)
        {
            Fail("Replay confirmed input audit incomplete");
            return -1;
        }
        EM_ASM({
            globalThis.__eaglerNetplayReplayExpectedFrames = $0;
            globalThis.__eaglerNetplayReplayInputCoverage = $1;
            globalThis.__eaglerNetplayReplayComparedFrames = 0;
            globalThis.__eaglerNetplayReplayInputMismatch = false;
        }, ReplayExtension::DebugExpectedMultiplayerPlaybackFrames(),
           ReplayExtension::DebugExpectedMultiplayerPlaybackCoverage());
        char replayName[] = "SMOKE";
        std::filesystem::create_directory(
            std::filesystem::u8path(FileSystem::GetPrefPath("replay")));
        const std::string replayPath =
            FileSystem::GetPrefPath("replay/th7_01.rpy");
        ReplayManager::SaveReplay(replayPath.c_str(), replayName);
        EM_ASM({ Module.eaglerOptions.replayViewer = true; });
        g_ReplayPlaybackCycleDispatched = true;
        g_Supervisor.curState = 7;
        return CHAIN_CALLBACK_RESULT_CONTINUE;
    }
#endif

    if (ProbeMode() && g_SimFrame >= g_TestFrames &&
        g_Core.ConfirmedThroughAllRemotes() >= g_TestFrames - 1 &&
        !g_Core.HasRollbackRequest())
    {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        const std::uint8_t expectedInputMask =
            static_cast<std::uint8_t>((1u << g_PlayerCount) - 1u);
        if (!g_IndependentInputSlotsObserved ||
            (g_InputSlotObservedMask & expectedInputMask) != expectedInputMask)
        {
            Fail("per-player input slots not exercised");
            return -1;
        }
        if (UsePhysicalInput() && !g_PhysicalInputObserved)
        {
            Fail("physical input not observed");
            return -1;
        }
        if (UsePauseCycle() && (!g_PauseObserved || !g_PauseResumeObserved))
        {
            Fail("pause cycle not completed");
            return -1;
        }
        if (UseRestartCycle() && g_SessionGeneration == 0)
        {
            Fail("restart cycle did not create a new session generation");
            return -1;
        }
        if (UseStageTransitionTest() &&
            (!g_StageTransitionObserved || !g_StageTransitionInputResetObserved ||
             !g_PostTransitionInputObserved))
        {
            Fail("stage transition input lifecycle not completed");
            return -1;
        }
#endif
        const auto sample = Th07CanonicalHash::Capture();
        PrintHash("FINAL", sample);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        if (g_PlayerCount >= 3)
        {
            std::printf(
                "netplay lan stage: PLAYERS p1=(%.2f,%.2f) p2=(%.2f,%.2f) p3=(%.2f,%.2f) independentInputs=1 physicalInput=%d\n",
                g_Players[0].pos.x, g_Players[0].pos.y,
                g_Players[1].pos.x, g_Players[1].pos.y,
                g_Players[2].pos.x, g_Players[2].pos.y,
                UsePhysicalInput() ? 1 : 0);
        }
        else
        {
            std::printf(
                "netplay lan stage: PLAYERS p1=(%.2f,%.2f) p2=(%.2f,%.2f) independentInputs=1 physicalInput=%d\n",
                g_Players[0].pos.x, g_Players[0].pos.y,
                g_Players[1].pos.x, g_Players[1].pos.y,
                UsePhysicalInput() ? 1 : 0);
        }
#endif
        std::printf(
            "netplay lan stage: PASS player=%u players=%u frames=%u sent=%u recv=%u session=%u/%u rollback=%u resim=%u maxRollback=%u predicted=%u confirmed=%u maxSnapshot=%llu buffered=%llu peak=%u/%u/%u/%u pause=%d/%d restart=%u stageMax=%u\n",
            static_cast<unsigned>(g_LocalPlayer), static_cast<unsigned>(g_PlayerCount),
            g_TestFrames, g_SentPackets,
            g_ReceivedPackets, g_SessionPacketsSent, g_SessionPacketsReceived,
            g_RollbackCount, g_ResimulatedFrames, g_MaxRollbackSpan, g_PredictedFrames,
            static_cast<unsigned>(g_Core.ConfirmedThroughAllRemotes()),
            static_cast<unsigned long long>(g_MaxSnapshotBytes),
            static_cast<unsigned long long>(TransportBufferedAmount()),
            g_PeakEnemies, g_PeakBullets, g_PeakLasers, g_PeakItems,
            g_PauseObserved ? 1 : 0, g_PauseResumeObserved ? 1 : 0,
            static_cast<unsigned>(g_SessionGeneration),
            static_cast<unsigned>(g_HighestStageObserved));
        g_Done = true;
#ifdef __EMSCRIPTEN__
        EM_ASM({ globalThis.__eaglerNetplayLanStageDone = true; });
#endif
        return CHAIN_CALLBACK_RESULT_CONTINUE;
    }
    // Driver callbacks are display-paced and can retry without advancing a
    // frame. Counting them as 60 Hz seconds prematurely failed 120/165 Hz
    // peers while the slower endpoint was still delivering the final inputs.
    // Keep a bounded six-times-real-time test deadline; the independent
    // 15-second missing-input watchdog remains unchanged.
    if (ProbeMode() && SDL_GetTicks() - g_ProbeStartedMs >=
        std::max<std::uint64_t>(30000, static_cast<std::uint64_t>(g_TestFrames) * 100u))
    {
        Fail("timeout");
        return -1;
    }
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}
} // namespace Netplay::Th07LanStageProbe

#ifdef __EMSCRIPTEN__
extern "C" EMSCRIPTEN_KEEPALIVE const char *th07_mp_spectator_state()
{
    return Netplay::Th07LanStageProbe::SpectatorState();
}

extern "C" EMSCRIPTEN_KEEPALIVE int th07_mp_spectator_control(
    const char *run, const char *action, std::uint32_t frame)
{
    return Netplay::Th07LanStageProbe::SpectatorControl(run, action, frame);
}
#else
extern "C" const char *th07_mp_spectator_state()
{
    return Netplay::Th07LanStageProbe::SpectatorState();
}

extern "C" int th07_mp_spectator_control(
    const char *run, const char *action, std::uint32_t frame)
{
    return Netplay::Th07LanStageProbe::SpectatorControl(run, action, frame);
}
#endif
