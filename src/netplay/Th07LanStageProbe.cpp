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
#include "ReplayExtension.hpp"
#include "ReplayManager.hpp"
#include "Supervisor.hpp"
#include "Touch.hpp"
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#include "multiplayer/GameplaySession.hpp"
#endif

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <deque>
#include <filesystem>
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
constexpr std::uint32_t GAMEPLAY_ABI = TH07_MULTI_NETPLAY_ABI;
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
std::deque<SpectatorFramePacket> g_SpectatorFrames;
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
        return g_BrowserPeerTransport.Connect(url, g_LocalPlayer, g_PlayerCount);
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
    const bool hasSpectators = g_BrowserPeerTransport.HasSpectators();
#ifdef __EMSCRIPTEN__
    if (ProductionLanMode())
        EM_ASM({
            const state = globalThis.__eaglerNetplaySpectatorPublish ||= {};
            state.hasSpectators = !!$0;
            state.cursor = $1 >>> 0;
            state.simFrame = $2 >>> 0;
            state.confirmed = $3 >>> 0;
        }, hasSpectators ? 1 : 0, g_NextSpectatorPublishFrame, g_SimFrame,
            g_Core.ConfirmedThroughAllRemotes());
#endif
    if (!hasSpectators || g_SpectatorMode || g_LocalPlayer != 0 || g_SimFrame == 0)
        return;
    const std::uint32_t lastAvailable =
        std::min(g_Core.ConfirmedThroughAllRemotes(), g_SimFrame - 1);
    while (g_NextSpectatorPublishFrame <= lastAvailable)
    {
        const FrameDecision decision = g_Core.PrepareFrame(g_NextSpectatorPublishFrame);
        if (!decision.canAdvance || decision.predictedMask != 0)
            return;
        SpectatorFramePacket packet;
        packet.sessionId = CurrentSessionId();
        packet.frame = g_NextSpectatorPublishFrame;
        packet.gameplayAbi = GAMEPLAY_ABI;
        packet.playerCount = g_PlayerCount;
        packet.inputs = decision.inputs;
        std::vector<std::uint8_t> wire;
        if (!EncodeSpectatorFramePacket(packet, &wire) ||
            !g_BrowserPeerTransport.SendSpectator(wire.data(), wire.size()))
        {
#ifdef __EMSCRIPTEN__
            EM_ASM({ globalThis.__eaglerNetplaySpectatorPublishFailed = $0 >>> 0; },
                   g_NextSpectatorPublishFrame);
#endif
            return;
        }
        ++g_NextSpectatorPublishFrame;
    }
}

bool DrainSpectatorFrames()
{
    std::vector<std::uint8_t> wire;
    while (TransportPoll(&wire))
    {
        SpectatorFramePacket packet;
        if (!DecodeSpectatorFramePacket(wire.data(), wire.size(), &packet) ||
            packet.sessionId != CurrentSessionId() || packet.gameplayAbi != GAMEPLAY_ABI ||
            packet.playerCount != g_PlayerCount ||
            packet.frame != g_NextSpectatorReceiveFrame)
            return false;
        g_SpectatorFrames.push_back(packet);
        ++g_NextSpectatorReceiveFrame;
    }
    return true;
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
    return g_GameManager.isPaused || g_GameManager.isInPauseMenu ||
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
    char url[512] = {};
    char spectatorId[65] = {};
    if (g_LocalPlayer >= g_PlayerCount || !ReadUrl(url, sizeof(url)))
        return false;
    const bool connected = g_SpectatorMode
        ? ReadSpectatorId(spectatorId, sizeof(spectatorId)) &&
          g_BrowserPeerTransport.ConnectSpectator(url, spectatorId, g_PlayerCount)
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
            g_Players[player].playerState = PLAYER_STATE_INVULNERABLE;
            g_Players[player].invulnerabilityTimer = 0x3fffffff;
        }
    }
#endif

    SessionConfig sessionConfig;
    sessionConfig.sessionId = CurrentSessionId();
    sessionConfig.seed = static_cast<std::uint32_t>(g_Rng.seed);
    sessionConfig.gameplayAbi = GAMEPLAY_ABI;
    sessionConfig.gameId = GAME_ID_TH07;
    sessionConfig.playerCount = g_PlayerCount;
    sessionConfig.localPlayer = g_LocalPlayer;
    if (g_SpectatorMode)
    {
        char spectatorId[65] = {};
        if (!g_ProductionTransportStarted &&
            (!ReadSpectatorId(spectatorId, sizeof(spectatorId)) ||
             !g_BrowserPeerTransport.ConnectSpectator(url, spectatorId, g_PlayerCount)))
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
    g_RecommendedLead = 0.0;
    g_SimulationIntervalScale = 1.0;
    g_EndingSkipHistoryNormalized = false;
    g_ReplayPlaybackCycleDispatched = false;
    g_NextReplayAuditFrame = 0;
    g_NextSpectatorPublishFrame = 0;
    g_NextSpectatorReceiveFrame = 0;
    g_SpectatorFrames.clear();
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
            globalThis.__eaglerNetplayLanHash = "";
            globalThis.__eaglerNetplayLanHashes = Object.create(null);
            globalThis.__eaglerNetplayLanPeerAdvantages = [];
            globalThis.__eaglerNetplayLanPeers = [];
            globalThis.__eaglerNetplayPerf = {};
        }, g_SessionGeneration);
    }
#endif
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
        ? kMaxIncrementalCaptureLead : startupLead;
    const std::uint32_t maxCaptureFrame =
        g_SimFrame < INVALID_FRAME - captureLead ? g_SimFrame + captureLead : g_SimFrame;
    if (g_NextInputCaptureNs == 0)
        g_NextInputCaptureNs = now;

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
    if (ProductionLanMode() && ((frame + 1u) % 300u) == 0u)
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
        if (g_SpectatorFrames.empty())
            return CHAIN_CALLBACK_RESULT_CONTINUE;
        const std::size_t backlog = g_SpectatorFrames.size();
        const std::size_t framesThisTick = backlog > 8 ? 4 : backlog > 4 ? 2 : 1;
        int result = CHAIN_CALLBACK_RESULT_CONTINUE;
        for (std::size_t index = 0; index < framesThisTick && !g_SpectatorFrames.empty(); ++index)
        {
            const SpectatorFramePacket packet = g_SpectatorFrames.front();
            if (packet.frame != g_SimFrame)
            {
                Fail("spectator frame gap");
                return -1;
            }
            FrameDecision decision;
            decision.canAdvance = true;
            decision.inputs = packet.inputs;
            const i32 stageBefore = g_GameManager.currentStage;
            result = SimulateFrame(g_SimFrame, decision, false);
            if (result == 0 || result == -1)
                return result;
            g_SpectatorFrames.pop_front();
            ++g_SimFrame;
            if (g_GameManager.currentStage != stageBefore)
                break;
        }
#ifdef __EMSCRIPTEN__
        EM_ASM({
            globalThis.__eaglerNetplayLanFrame = $0;
            globalThis.__eaglerNetplaySpectator = true;
        }, g_SimFrame);
#endif
        g_LastTickAdvanced = true;
        return result;
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
    const bool independentInputClock = g_IncrementalReconcile || g_InitialFrameLag != 0;
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

        // Pause/retry UI is rewindable (GameManager + AsciiManager + relevant
        // Supervisor state are in the snapshot), but predicting menu input is
        // needlessly risky: a late Escape can change which UI frame consumes
        // subsequent keys.  Keep exchanging frames, but wait one round trip
        // for every remote player's real input while shared UI is active.
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
