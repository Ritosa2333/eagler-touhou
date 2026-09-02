#include "Th06LanStageProbe.hpp"

#include "NetplayCore.hpp"
#include "NetplayInput.hpp"
#include "NetplayProtocol.hpp"
#include "NetplaySession.hpp"
#include "NetplaySideEffects.hpp"
#include "Th06CanonicalHash.hpp"
#include "Th06RollbackState.hpp"
#include "BrowserPeerTransport.hpp"
#include "WebSocketTransport.hpp"

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
#include "Rng.hpp"
#include "SoundPlayer.hpp"
#include "Supervisor.hpp"
#include "Touch.hpp"
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#include "multiplayer/GameplaySession.hpp"
#endif

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <deque>
#include <vector>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

namespace Netplay::Th06LanStageProbe
{
namespace
{
constexpr std::uint64_t SESSION_ID = 0x5448364c414e5331ull; // TH6LANS1
constexpr std::uint32_t GAME_ID_TH06 = 6;
// TH06 starts with the first gameplay ABI: synchronized stable player lanes,
// rollback, stage transitions and shared pause/retry state.
// ABI 2 adds the synchronized TH06 gameplay lifecycle and isolates
// multiplayer Result persistence from ordinary score.dat progression.
// Confirmed gameplay retires before vanilla local Ending/Result take over.
// ABI 4 adds the cross-frame ExInsShootStarPattern ECL sidecar to rollback.
// Older peers must fail the room gate before a rollback through that pattern
// can silently reuse future player-position/random-angle state.
// ABI 5 removes multiplayer Continue: total team wipe keeps the deterministic
// 180-logical-frame grace, then requests Result directly. Retry/Continue is a
// forbidden fallback path rather than part of the synchronized run lifecycle.
constexpr std::uint32_t GAMEPLAY_ABI = TH06_MULTI_GAMEPLAY_ABI;
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
std::size_t g_MaxSnapshotBlocks = 0;

struct ReplayFrameBinding
{
    std::uint32_t simFrame = INVALID_FRAME;
    i32 stage = -1;
    i32 replayFrame = -1;
};
std::array<ReplayFrameBinding, INPUT_HISTORY_SIZE> g_ReplayFrameBindings{};
std::array<std::uint32_t, MAX_PLAYERS> g_LastConfirmedFrame{};
std::array<std::uint64_t, MAX_PLAYERS> g_LastConfirmedAdvanceMs{};
std::array<bool, MAX_PLAYERS> g_RemoteTimeoutArmed{};
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
bool g_TestUsePhysicalInput = false;
bool g_TestUseScriptedStressInput = false;
bool g_TestUseEliminationCycle = false;
bool g_TestUseCoopTransferCycle = false;
bool g_TestUseReplayPlaybackCycle = false;
std::uint32_t g_NextReplayAuditFrame = 0;
bool g_ReplayPlaybackAuditInitialized = false;
bool g_SpectatorMode = false;
bool g_SpectatorRunRetired = false;
std::uint32_t g_NextSpectatorPublishFrame = 0;
std::uint32_t g_NextSpectatorReceiveFrame = 0;
std::deque<SpectatorFramePacket> g_SpectatorFrames;
bool g_TestUsePauseCycle = false;
bool g_TestUseRetryContinueCycle = false;
bool g_TestUseDenseRollbackProfile = false;
bool g_TestUseQuitCycle = false;
bool g_TestUseEndingCycle = false;
bool g_TestUseStageTransition = false;
bool g_TestRollbackAudit = false;
bool g_SingleEliminationObserved = false;
bool g_CoopRevivableObserved = false;
bool g_CoopReviveObserved = false;
bool g_CoopPowerTransferObserved = false;
bool g_TeamWipeObserved = false;
bool g_TeamWipeGraceObserved = false;
bool g_TeamWipeRetryObserved = false;
std::uint32_t g_TeamWipeStartFrame = INVALID_FRAME;
std::uint32_t g_TeamWipeRetryFrame = INVALID_FRAME;
std::uint32_t g_PeakEnemies = 0;
std::uint32_t g_PeakBullets = 0;
std::uint32_t g_PeakLasers = 0;
std::uint32_t g_PeakItems = 0;
std::array<std::uint32_t, MAX_PLAYERS> g_LastRemoteSenderFrame{};
struct PeerTimeSyncState
{
    std::array<std::int32_t, 64> samples{};
    std::size_t sampleCount = 0;
    std::size_t sampleCursor = 0;
    double averageLead = 0.0;
    bool ready = false;
};
std::array<PeerTimeSyncState, MAX_PLAYERS> g_PeerTimeSync{};
std::array<std::uint32_t, MAX_PLAYERS> g_PredictionDepth{};
std::array<std::uint32_t, MAX_PLAYERS> g_RollbackByPlayer{};
double g_RecommendedLead = 0.0;
double g_SimulationIntervalScale = 1.0;

struct RestoreAuditEntry
{
    std::uint32_t frame = INVALID_FRAME;
    Th06CanonicalHash::Sample sample{};
    std::uint64_t localDebugHash = 0;
};
std::array<RestoreAuditEntry, 64> g_RestoreAudit{};

bool UsePhysicalInput();
bool UseScriptedStressInput();
bool UseEliminationCycle();
bool UseCoopTransferCycle();
bool UseReplayPlaybackCycle();
bool UsePauseCycle();
bool UseRestartCycle();
bool UseRetryContinueCycle();
bool UseDenseRollbackProfile();
bool UseQuitCycle();
bool UseEndingCycle();
bool UseStageTransitionTest();
bool ProbeMode();
bool ProductionLanMode();
bool RollbackAuditEnabled();
void ClearTransientModes();
void Fail(const char *reason);
std::uint16_t LocalScript(std::uint8_t player, std::uint32_t frame);

std::uint64_t CurrentSessionId()
{
    // Leaving gameplay and later starting a new run creates a new rollback
    // session even if the browser keeps the same RTC/DataChannel transport
    // alive. Generation 0 preserves the initial wire id; later generations
    // reject late packets from a retired run.
    return SESSION_ID ^
           (static_cast<std::uint64_t>(g_SessionGeneration) *
            0x9e3779b97f4a7c15ull);
}

bool UseEliminationCycle()
{
    return g_TestUseEliminationCycle;
}

bool UseCoopTransferCycle()
{
    return g_TestUseCoopTransferCycle;
}

bool UseReplayPlaybackCycle()
{
    return g_TestUseReplayPlaybackCycle;
}

bool RollbackAuditEnabled()
{
    return g_TestRollbackAudit;
}

bool UseScriptedStressInput()
{
    return g_TestUseScriptedStressInput;
}

bool UseRetryContinueCycle()
{
    // Historical test option retained as an adversarial no-Continue probe: it
    // still sends the old Retry-Yes input pattern, but ABI5 must retire to
    // Result before those inputs can ever reach a Continue menu.
    return g_TestUseRetryContinueCycle;
}

bool UseDenseRollbackProfile()
{
    return g_TestUseDenseRollbackProfile;
}

bool UseQuitCycle()
{
    return g_TestUseQuitCycle;
}

bool UseEndingCycle()
{
    return g_TestUseEndingCycle;
}

void RetireGameplaySession()
{
    ClearTransientModes();
    Th06Rollback::Clear();
    if (g_SpectatorMode)
    {
        // A spectator admission belongs to exactly one relay run. The
        // Launcher creates a fresh run/startSerial (and a fresh Runtime) for
        // the next room start; never reinterpret the old read-only socket as
        // a second gameplay generation from the local post-game MainMenu.
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
    g_SimFrame = 0;
    g_DriverTicks = 0;
    g_Sequence = 1;
    g_LastReceivedSequence = 0;
    g_LastHelloSendTick = 0;
    g_LastReadySendTick = 0;
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

std::int32_t SignedFrameDelta(std::uint32_t lhs, std::uint32_t rhs)
{
    return static_cast<std::int32_t>(lhs - rhs);
}

void RecordTimeSyncSample(const InputPacket &packet)
{
    // GGPO/GGRS maintain time-sync state per endpoint.  A 3P room must not
    // average two unrelated links into one sample stream: update the sender's
    // window, then use the largest local lead as the room wait recommendation.
    if (!ProductionLanMode() || packet.senderFrame == INVALID_FRAME ||
        packet.senderPlayer >= g_PlayerCount || packet.senderPlayer == g_LocalPlayer)
        return;

    std::uint32_t &lastRemoteFrame = g_LastRemoteSenderFrame[packet.senderPlayer];
    if (lastRemoteFrame != INVALID_FRAME && packet.senderFrame <= lastRemoteFrame)
        return;
    lastRemoteFrame = packet.senderFrame;

    const std::int32_t localAdvantage = SignedFrameDelta(g_SimFrame, packet.senderFrame);
    const std::int32_t advantageDifference =
        localAdvantage - static_cast<std::int32_t>(packet.frameAdvantage);
    const std::int32_t inferredLead = advantageDifference / 2;
    if (std::abs(inferredLead) > 30)
        return;

    PeerTimeSyncState &state = g_PeerTimeSync[packet.senderPlayer];
    state.samples[state.sampleCursor] = inferredLead;
    state.sampleCursor = (state.sampleCursor + 1) % state.samples.size();
    state.sampleCount = std::min(state.sampleCount + 1, state.samples.size());
    if (state.sampleCount < 20)
        return;

    std::vector<std::int32_t> sorted;
    sorted.reserve(state.sampleCount);
    for (std::size_t i = 0; i < state.sampleCount; ++i)
        sorted.push_back(state.samples[i]);
    std::sort(sorted.begin(), sorted.end());
    const std::size_t trim = std::min<std::size_t>(4, sorted.size() / 8);
    std::int64_t sum = 0;
    for (std::size_t i = trim; i < sorted.size() - trim; ++i)
        sum += sorted[i];
    state.averageLead = static_cast<double>(sum) /
        static_cast<double>(sorted.size() - trim * 2);
    state.ready = true;

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

    const double deadbandLead = std::abs(recommendedLead) < 0.5 ? 0.0 : recommendedLead;
    const double desiredScale = std::clamp(1.0 + deadbandLead * 0.003, 0.98, 1.02);
    g_SimulationIntervalScale += (desiredScale - g_SimulationIntervalScale) * 0.08;
    if (std::abs(g_SimulationIntervalScale - 1.0) < 0.0002)
        g_SimulationIntervalScale = 1.0;
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
    return g_TestUseStageTransition;
}

std::uint32_t ConfirmedThroughAllRemotes()
{
    std::uint32_t confirmed = INVALID_FRAME;
    bool found = false;
    for (std::uint8_t player = 0; player < g_PlayerCount; ++player)
    {
        if (player == g_LocalPlayer)
            continue;
        const std::uint32_t value = g_Core.ConfirmedThrough(player);
        if (!found || value == INVALID_FRAME ||
            (confirmed != INVALID_FRAME && value < confirmed))
            confirmed = value;
        found = true;
    }
    return found ? confirmed : INVALID_FRAME;
}

bool CaptureConfirmedReplayAuditFrames()
{
#if defined(TH_DEV_TOOLS) && defined(TH_ENABLE_MULTIPLAYER_GAMEPLAY)
    if (!UseReplayPlaybackCycle() || g_SimFrame == 0)
        return true;
    const std::uint32_t confirmed = ConfirmedThroughAllRemotes();
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
            const state = globalThis.__eaglerNetplaySpectatorPublish = {};
            state.hasSpectators = !!$0;
            state.cursor = $1 >>> 0;
            state.simFrame = $2 >>> 0;
            state.confirmed = $3 >>> 0;
        }, hasSpectators ? 1 : 0, g_NextSpectatorPublishFrame, g_SimFrame,
            ConfirmedThroughAllRemotes());
#endif
    if (g_SpectatorMode || g_LocalPlayer != 0 || g_SimFrame == 0)
        return;
    const std::uint32_t lastAvailable =
        std::min(ConfirmedThroughAllRemotes(), g_SimFrame - 1);
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
    if (!UsePhysicalInput())
    {
        FrameInput input(LocalScript(g_LocalPlayer, frame));
        if (UseReplayPlaybackCycle())
        {
            // Exercise every field carried by multiplayer Replay input while
            // keeping this hidden Stage 1 profile deliberately playable.
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
    if (Touch::GetFreeJoystickVector(&x, &y))
        Input::CaptureJoystick(x, y);
    else if (Touch::GetPlayerDelta(&x, &y))
        Input::CaptureDirectTouch(x, y, Touch::IsUnlimited());
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
    return g_TestUsePhysicalInput;
}

bool UsePauseCycle()
{
    return g_TestUsePauseCycle;
}

bool UseRestartCycle()
{
#ifdef __EMSCRIPTEN__
    // Keep the restart-cycle intent across RetireGameplaySession().  The old
    // generation's transient test flags are deliberately cleared there, while
    // Module.eaglerOptions describes the whole browser acceptance run.
    return EM_ASM_INT({ return Module.eaglerOptions?.netplayRestartCycle ? 1 : 0; }) != 0;
#else
    return false;
#endif
}

bool EligibleForInitialNetplayStart()
{
    const int stage = g_GameManager.currentStage;
    const bool validInitialStage = stage >= 1 && stage <= 7;
    return g_GameManager.isInMenu && !g_GameManager.isInReplay &&
           !g_GameManager.isInGameMenu &&
           !g_GameManager.isInRetryMenu && !g_GameManager.isTimeStopped &&
           validInitialStage && g_ReplayManager != nullptr &&
           g_Supervisor.curState == SUPERVISOR_STATE_GAMEMANAGER &&
           g_Supervisor.wantedState == g_Supervisor.curState;
}

bool InitialNetplayBootstrapInProgress()
{
    // The dangerous input window starts one tick before curState actually
    // becomes GameManager: Supervisor has already requested the gameplay
    // scene, then that same vanilla calc pass constructs Player and can let
    // P1 consume machine-local input. Gate from wantedState instead of waiting
    // for curState/currentStage to catch up. On the way out to Result/Ending,
    // wantedState is no longer GameManager, so post-game UI keeps vanilla
    // input as intended.
    const bool enteringGameplay =
        g_Supervisor.wantedState == SUPERVISOR_STATE_GAMEMANAGER ||
        g_Supervisor.wantedState == SUPERVISOR_STATE_GAMEMANAGER_REINIT;
    return !g_GameManager.isInReplay && enteringGameplay;
}

bool SessionStillOwnsStageState()
{
    // Once the session gate has taken ownership, pause/retry/message time-stop
    // are part of the synchronized stage state as well.  Dropping back to a
    // local g_Chain.RunCalcChain() here would re-sample each machine's hardware
    // and make the first pause the first desync.
    const bool gameplayScene =
        (g_Supervisor.curState == SUPERVISOR_STATE_GAMEMANAGER ||
         g_Supervisor.curState == SUPERVISOR_STATE_GAMEMANAGER_REINIT) &&
        (g_Supervisor.wantedState == SUPERVISOR_STATE_GAMEMANAGER ||
         g_Supervisor.wantedState == SUPERVISOR_STATE_GAMEMANAGER_REINIT);
    return !g_GameManager.isInReplay && g_GameManager.currentStage >= 1 &&
           g_GameManager.currentStage <= 7 && g_ReplayManager != nullptr &&
           gameplayScene;
}

bool SharedUiNeedsConfirmedInputs()
{
    return g_GameManager.isInGameMenu || g_GameManager.isInRetryMenu ||
           (g_Gui.impl && g_Gui.impl->finishedStage) ||
           g_Supervisor.curState != SUPERVISOR_STATE_GAMEMANAGER ||
           g_Supervisor.wantedState != SUPERVISOR_STATE_GAMEMANAGER;
}

void NormalizeMultiplayerEndingSkipHistory()
{
    // TH06 retires rollback ownership when gameplay leaves GameManager.
    // Ending is a local post-game scene, so unlike TH07 there is no shared
    // score.dat/skip-history branch to normalize inside the rollback session.
}

bool RemoteInputsTimedOut()
{
    // Native sbrik can contract a 3P UDP session around an absent guest by
    // announcing host-authored lifecycle controls and relaying synthesized
    // inputs. The browser relay has no equivalent authority protocol. Do not
    // guess that simulation behavior locally: end the room visibly if any
    // required WebSocket lane stops advancing instead of leaving every canvas
    // in an unbounded silent stall.
    // Long Ending/Result/Replay browser smoke runs multiple headless Chromium
    // instances on one host and can occasionally have an entire renderer
    // descheduled for longer than the production watchdog. Keep the real
    // room behavior strict, but let this hidden lifecycle harness tolerate a
    // local test-runner stall instead of misreporting it as a network bug.
    const std::uint64_t timeoutMs = UseEndingCycle() ? 60000u : 15000u;
    const std::uint64_t now = SDL_GetTicks();
    for (std::uint8_t player = 0; player < g_PlayerCount; ++player)
    {
        if (player == g_LocalPlayer)
            continue;
        const std::uint32_t confirmed = g_Core.ConfirmedThrough(player);
        if (!g_Session.CanStart())
        {
            g_RemoteTimeoutArmed[player] = false;
            continue;
        }
        if (!g_RemoteTimeoutArmed[player])
        {
            g_RemoteTimeoutArmed[player] = true;
            g_LastConfirmedFrame[player] = confirmed;
            g_LastConfirmedAdvanceMs[player] = now;
            continue;
        }
        if (confirmed != g_LastConfirmedFrame[player])
        {
            g_LastConfirmedFrame[player] = confirmed;
            g_LastConfirmedAdvanceMs[player] = now;
        }
        else if (now - g_LastConfirmedAdvanceMs[player] >= timeoutMs)
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
    if (UseEndingCycle())
    {
        // Drive the actual final stage. P1's synchronized Skip accelerates
        // skippable Stage 6 dialogue only; it never bypasses Gui/Supervisor.
        std::uint16_t bits = TH_BUTTON_SHOOT;
        if (player == 0)
        {
            bits |= TH_BUTTON_SKIP;
            bits |= ((frame / 45u) & 1u) ? TH_BUTTON_RIGHT : TH_BUTTON_LEFT;
        }
        else if (player == 1)
            bits |= ((frame / 55u) & 1u) ? TH_BUTTON_DOWN : TH_BUTTON_UP;
        else
        {
            bits |= ((frame / 65u) & 1u) ? TH_BUTTON_LEFT : TH_BUTTON_RIGHT;
            bits |= TH_BUTTON_FOCUS;
        }
        return bits;
    }
    if (UseQuitCycle() && frame >= 600u && frame <= 660u)
    {
        // Exercise TH06's real shared pause -> Q -> Quit Yes path. Q selects
        // the affirmative quit state directly once the pause menu owns input;
        // vanilla then spends twenty logical frames closing before MainMenu.
        if (player != 0)
            return 0;
        if (frame == 600u)
            return TH_BUTTON_MENU;
        if (frame == 610u)
            return TH_BUTTON_Q;
        return 0;
    }
    if (UseCoopTransferCycle())
    {
        if (player != 0)
            return 0;
        // P1 holds Focus for exactly 90 committed logical frames while the
        // nearby P2 is REVIVABLE, then later supplies eight clean Shoot edges
        // inside the 24-frame Power-transfer window.
        if (frame >= 301u && frame <= 390u)
            return TH_BUTTON_FOCUS;
        if (frame >= 501u && frame <= 515u && (frame & 1u) != 0)
            return TH_BUTTON_SHOOT;
        return 0;
    }
    if (UseRetryContinueCycle() && frame >= 780u && frame <= 860u)
    {
        // Adversarially send the old Retry-Yes interaction around the exact
        // team-wipe handoff. Multiplayer must still go directly to Result.
        if (player != 0)
            return 0;
        if (frame == 813u)
            return TH_BUTTON_UP;
        if (frame == 819u)
            return TH_BUTTON_SHOOT;
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
    if (UseRestartCycle() && g_SessionGeneration == 0 && frame >= 600 && frame <= 660)
    {
        // Match TH07's exact shared Pause -> R lifecycle. P1 opens Pause, then
        // requests quick Restart only after the menu is live. Generation 1 is
        // intentionally excluded so the acceptance probe performs one restart.
        if (player != 0)
            return 0;
        if (frame == 600)
            return TH_BUTTON_MENU;
        if (frame == 620)
            return TH_BUTTON_R;
        return 0;
    }

    if (UseScriptedStressInput())
    {
        // Logical-frame equivalent of the browser KeyboardEvent stress case:
        // direction changes happen every ~5-6 fixed ticks, but the sequence is
        // fully deterministic and therefore independent of wall-clock timer
        // scheduling or browser catch-up cadence.
        std::uint16_t bits = TH_BUTTON_SHOOT;
        if (player == 0)
            bits |= ((frame / 5u) & 1u) ? TH_BUTTON_RIGHT : TH_BUTTON_LEFT;
        else if (player == 1)
            bits |= ((frame / 6u) & 1u) ? TH_BUTTON_LEFT : TH_BUTTON_RIGHT;
        else
            bits |= ((frame / 7u) & 1u) ? TH_BUTTON_DOWN : TH_BUTTON_UP;
        return bits;
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

#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
void ForceTerminalDeathForEliminationTest(std::uint8_t playerId)
{
    if (playerId >= g_PlayerCount || !g_PlayerActive[playerId])
        return;
    Player &player = g_Players[playerId];
    if (IsPlayerTerminal(playerId))
        return;
    SetPlayerLives(playerId, 0);
    player.playerState = PLAYER_STATE_DEAD;
    player.respawnTimer = 0;
    player.invulnerabilityTimer.SetCurrent(30);
    player.bombInfo.isInUse = 0;
}

void ApplyEliminationCycleEvent(std::uint32_t frame)
{
    if (!UseEliminationCycle() && !UseRetryContinueCycle())
        return;
    if (frame == 300u)
    {
        // Remove one guest first. The room must continue with the remaining
        // participant(s) and must not enter Retry.
        ForceTerminalDeathForEliminationTest(1);
    }
    else if (frame == 600u)
    {
        // Remove every remaining participant in one deterministic logical
        // frame. Player::OnUpdate still performs the real terminal death ->
        // REVIVABLE/terminal transition; this hook only prepares its final
        // vanilla death tick.
        ForceTerminalDeathForEliminationTest(0);
        if (g_PlayerCount >= 3)
            ForceTerminalDeathForEliminationTest(2);
    }
}

void ApplyCoopTransferCycleEvent(std::uint32_t frame)
{
    if (!UseCoopTransferCycle() || g_PlayerCount < 2)
        return;

    Player &giver = g_Players[0];
    Player &receiver = g_Players[1];
    if (frame == 300u)
    {
        SetPlayerLives(0, 2);
        ForceTerminalDeathForEliminationTest(1);
        giver.positionCenter = ZunVec3(192.0f, 360.0f, 0.2f);
        receiver.positionCenter = giver.positionCenter;
        giver.prevPositionCenter = giver.positionCenter;
        receiver.prevPositionCenter = receiver.positionCenter;
    }
    else if (frame >= 301u && frame <= 390u)
    {
        // REVIVABLE players keep their synchronized drift. Follow that drift
        // in this test so all 90 Focus frames exercise the real in-range hold
        // instead of letting the fixed giver fall outside the 20px radius.
        giver.positionCenter = receiver.positionCenter;
        giver.prevPositionCenter = giver.positionCenter;
    }
    else if (frame == 500u)
    {
        giver.positionCenter = ZunVec3(192.0f, 320.0f, 0.2f);
        receiver.positionCenter = giver.positionCenter;
        giver.prevPositionCenter = giver.positionCenter;
        receiver.prevPositionCenter = receiver.positionCenter;
        SetPlayerPower(0, 100);
        SetPlayerPower(1, 0);
    }
}

bool ObserveCoopTransferCycle(std::uint32_t frame)
{
    if (!UseCoopTransferCycle())
        return true;

    if (frame >= 300u && g_Players[1].playerState == PLAYER_STATE_REVIVABLE)
        g_CoopRevivableObserved = true;
    if (frame >= 390u && g_CoopRevivableObserved &&
        g_Players[1].playerState == PLAYER_STATE_INVULNERABLE &&
        GetPlayerLives(0) == 2 && GetPlayerLives(1) == 0)
        g_CoopReviveObserved = true;

    if (frame >= 515u && GetPlayerPower(0) == 80 && GetPlayerPower(1) == 0)
    {
        i32 transferredItems = 0;
        const i32 transferState = GetItemTransferStateForPlayer(1);
        for (const Item &item : g_ItemManager.items)
        {
            if (item.isInUse && item.state == transferState &&
                (item.itemType == ITEM_POWER_BIG || item.itemType == ITEM_POWER_SMALL))
                ++transferredItems;
        }
        if (transferredItems >= 6)
            g_CoopPowerTransferObserved = true;
    }

#ifdef __EMSCRIPTEN__
    if (ProductionLanMode())
    {
        EM_ASM({
            globalThis.__eaglerNetplayCoopRevivableObserved = !!$0;
            globalThis.__eaglerNetplayCoopReviveObserved = !!$1;
            globalThis.__eaglerNetplayCoopPowerTransferObserved = !!$2;
        }, g_CoopRevivableObserved ? 1 : 0,
           g_CoopReviveObserved ? 1 : 0,
           g_CoopPowerTransferObserved ? 1 : 0);
    }
#endif
    return true;
}

bool ObserveEliminationCycle(std::uint32_t frame)
{
    if (!UseEliminationCycle() && !UseRetryContinueCycle())
        return true;

    if (frame >= 300u && frame < 600u && IsPlayerTerminal(1))
    {
        bool survivorActive = !IsPlayerTerminal(0);
        if (g_PlayerCount >= 3)
            survivorActive = survivorActive && !IsPlayerTerminal(2);
        if (survivorActive && !g_GameManager.isInRetryMenu)
            g_SingleEliminationObserved = true;
        if (g_GameManager.isInRetryMenu)
        {
            Fail("single elimination opened Retry");
            return false;
        }
    }

    bool allEliminated = true;
    for (std::uint8_t playerId = 0; playerId < g_PlayerCount; ++playerId)
        allEliminated = allEliminated && IsPlayerTerminal(playerId);
    if (allEliminated)
        g_TeamWipeObserved = true;

    if (g_teamWipeRetryFrames > 0)
    {
        if (g_TeamWipeStartFrame == INVALID_FRAME)
            g_TeamWipeStartFrame = frame;
        if (!g_GameManager.isInRetryMenu && g_teamWipeRetryFrames <= 90)
            g_TeamWipeGraceObserved = true;
    }

    if (g_GameManager.isInRetryMenu)
    {
        Fail("multiplayer exposed Retry/Continue");
        return false;
    }

    if (g_Supervisor.curState == SUPERVISOR_STATE_RESULTSCREEN_FROMGAME &&
        !g_TeamWipeRetryObserved)
    {
        if (!allEliminated || g_TeamWipeStartFrame == INVALID_FRAME)
        {
            Fail("Result requested before synchronized team wipe grace");
            return false;
        }
        g_TeamWipeRetryFrame = frame;
        if (g_TeamWipeRetryFrame - g_TeamWipeStartFrame != 180u)
        {
            Fail("team wipe grace was not 180 logical frames");
            return false;
        }
        // Historical telemetry names retain "Retry" so older harness readers
        // keep working. In ABI5 this means the post-grace direct Result handoff.
        g_TeamWipeRetryObserved = true;
    }

#ifdef __EMSCRIPTEN__
    if (ProductionLanMode())
    {
        EM_ASM({
            globalThis.__eaglerNetplaySingleEliminationObserved = !!$0;
            globalThis.__eaglerNetplayTeamWipeObserved = !!$1;
            globalThis.__eaglerNetplayTeamWipeGraceObserved = !!$2;
            globalThis.__eaglerNetplayTeamWipeRetryObserved = !!$3;
            globalThis.__eaglerNetplayTeamWipeStartFrame = $4;
            globalThis.__eaglerNetplayTeamWipeRetryFrame = $5;
            globalThis.__eaglerNetplayRetryContinueObserved = false;
            globalThis.__eaglerNetplayRetryResourcesResetObserved = false;
            globalThis.__eaglerNetplayRetryContinueFrame = -1;
            globalThis.__eaglerNetplaySecondTeamWipeGraceObserved = false;
            globalThis.__eaglerNetplaySecondTeamWipeRetryFrame = -1;
            globalThis.__eaglerNetplaySecondRetryContinueObserved = false;
            globalThis.__eaglerNetplaySecondRetryContinueFrame = -1;
            globalThis.__eaglerNetplayRetryResetCount = 0;
            globalThis.__eaglerNetplayRetryResetVerifiedCount = 0;
        }, g_SingleEliminationObserved ? 1 : 0,
           g_TeamWipeObserved ? 1 : 0,
           g_TeamWipeGraceObserved ? 1 : 0,
           g_TeamWipeRetryObserved ? 1 : 0,
           g_TeamWipeStartFrame == INVALID_FRAME ? -1 :
               static_cast<int>(g_TeamWipeStartFrame),
           g_TeamWipeRetryFrame == INVALID_FRAME ? -1 :
               static_cast<int>(g_TeamWipeRetryFrame));
    }
#endif
    return true;
}
#endif

void HashToHex(std::uint64_t value, char out[17])
{
    std::snprintf(out, 17, "%016llx", static_cast<unsigned long long>(value));
}

void PrintHash(const char *label, std::uint64_t sample)
{
    char total[17];
    HashToHex(sample, total);
    std::printf("netplay lan stage: %s player=%u localDebugHash=%s\n",
                label, static_cast<unsigned>(g_LocalPlayer), total);
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
        static_cast<unsigned>(ConfirmedThroughAllRemotes()),
        static_cast<unsigned long long>(TransportBufferedAmount()),
        TransportLastError().c_str());
}

bool Initialize()
{
    g_PlayerCount = ReadPlayerCount();
    g_SpectatorMode = SpectatorModeRequested();
    g_LocalPlayer = g_SpectatorMode ? 0 : ReadPlayer();
    g_TestFrames = ProbeMode() ? ReadTestFrames() : 0xffffffffu;
#ifdef __EMSCRIPTEN__
    const int testFlags = EM_ASM_INT({
        const o = Module.eaglerOptions || {};
        return (o.netplayScriptedInput ? 1 : 0) |
               (o.netplayScriptedStressInput ? 2 : 0) |
               (o.netplayEliminationCycle ? 4 : 0) |
               (o.netplayPauseCycle ? 8 : 0) |
               (o.netplayRetryContinueCycle ? 16 : 0) |
               (o.netplayStageTransition ? 32 : 0) |
               (o.netplayRollbackAudit ? 64 : 0) |
               (o.netplayPhysicalInput ? 128 : 0) |
               (o.netplayQuitCycle ? 256 : 0) |
               (o.netplayEndingCycle ? 512 : 0) |
               (o.netplayRestartCycle ? 1024 : 0) |
               (o.netplayDenseRollbackProfile ? 2048 : 0) |
               (o.netplayCoopTransferCycle ? 4096 : 0) |
               (o.netplayReplayPlaybackCycle ? 8192 : 0);
    });
    g_TestUseScriptedStressInput = (testFlags & 2) != 0;
    g_TestUseEliminationCycle = (testFlags & 4) != 0;
    g_TestUseCoopTransferCycle = (testFlags & 4096) != 0;
    g_TestUseReplayPlaybackCycle = (testFlags & 8192) != 0;
    if (g_TestUseReplayPlaybackCycle)
        g_TestFrames = ReadTestFrames();
    g_NextReplayAuditFrame = 0;
#ifdef TH_DEV_TOOLS
    if (g_TestUseReplayPlaybackCycle && !g_ReplayPlaybackAuditInitialized)
    {
        ReplayExtension::DebugResetMultiplayerPlaybackAudit();
        g_ReplayPlaybackAuditInitialized = true;
    }
#endif
    g_TestUsePauseCycle = (testFlags & 8) != 0;
    g_TestUseRetryContinueCycle = (testFlags & 16) != 0;
    g_TestUseDenseRollbackProfile = (testFlags & 2048) != 0;
    g_TestUseQuitCycle = (testFlags & 256) != 0;
    g_TestUseEndingCycle = (testFlags & 512) != 0;
    g_TestUseStageTransition = (testFlags & 32) != 0;
    g_TestRollbackAudit = (testFlags & 64) != 0;
    // Production rooms use physical input by default. Deterministic harness
    // modes that define logical-frame menu/gameplay edges must all route
    // through LocalScript; do not special-case individual lifecycle tests.
    constexpr int scriptedTestMask = 1 | 2 | 8 | 16 | 32 | 256 | 512 | 1024 | 4096 | 8192;
    g_TestUsePhysicalInput = (testFlags & scriptedTestMask) == 0 &&
                             (ProductionLanMode() || (testFlags & 128) != 0);
#else
    g_TestUsePhysicalInput = false;
    g_TestUseScriptedStressInput = false;
    g_TestUseEliminationCycle = false;
    g_TestUseCoopTransferCycle = false;
    g_TestUseReplayPlaybackCycle = false;
    g_TestUsePauseCycle = false;
    g_TestUseRetryContinueCycle = false;
    g_TestUseDenseRollbackProfile = false;
    g_TestUseQuitCycle = false;
    g_TestUseEndingCycle = false;
    g_TestUseStageTransition = false;
    g_TestRollbackAudit = false;
#endif

    const bool captureRollback = !g_SpectatorMode;
    if (captureRollback && RollbackAuditEnabled())
    {
        std::printf(
            "netplay rollback audit: START local=%u "
            "p0=%.1f/%.1f prev=%.1f/%.1f state=%d inv=%d "
            "p1=%.1f/%.1f prev=%.1f/%.1f state=%d inv=%d\n",
            static_cast<unsigned>(g_LocalPlayer),
            g_Players[0].positionCenter.x, g_Players[0].positionCenter.y,
            g_Players[0].prevPositionCenter.x, g_Players[0].prevPositionCenter.y,
            static_cast<int>(g_Players[0].playerState),
            g_Players[0].invulnerabilityTimer.AsFrames(),
            g_Players[1].positionCenter.x, g_Players[1].positionCenter.y,
            g_Players[1].prevPositionCenter.x, g_Players[1].prevPositionCenter.y,
            static_cast<int>(g_Players[1].playerState),
            g_Players[1].invulnerabilityTimer.AsFrames());
    }
    char url[512] = {};
    if (g_LocalPlayer >= g_PlayerCount || !ReadUrl(url, sizeof(url)))
        return false;

    CoreConfig coreConfig;
    coreConfig.sessionId = CurrentSessionId();
    coreConfig.playerCount = g_PlayerCount;
    coreConfig.localPlayer = g_LocalPlayer;
    coreConfig.inputDelay = 0;
    coreConfig.maxRollbackFrames = 12;
    coreConfig.predictableButtons =
        TH_BUTTON_DIRECTION | TH_BUTTON_FOCUS | TH_BUTTON_SHOOT | TH_BUTTON_SKIP;
    coreConfig.directionButtons = TH_BUTTON_DIRECTION;
    coreConfig.maxDirectionPredictionFrames = 3;
    if (!g_SpectatorMode && !g_Core.Reset(coreConfig))
        return false;

    // The vanilla title/stage bootstrap may have run a different number of
    // outer browser ticks on each peer before the netplay gate takes
    // ownership. None of that pre-session input-repeat phase belongs to the
    // synchronized gameplay timeline. Establish one common frame-zero
    // baseline before the first real FrameInput is scheduled.
    g_CurFrameInput = 0;
    g_LastFrameInput = 0;
    g_IsEigthFrameOfHeldInput = 0;
    g_NumOfFramesInputsWereHeld = 0;
    g_Supervisor.calcCount = 0;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    for (u8 playerId = 0; playerId < TH06_MULTI_MAX_PLAYERS; ++playerId)
    {
        g_CurFrameGameInputs[playerId] = 0;
        g_LastFrameGameInputs[playerId] = 0;
    }
#endif

    Th06Rollback::Config stateConfig;
    stateConfig.maxFrames = 16;
    stateConfig.maxBytesPerFrame = 8 * 1024 * 1024;
    stateConfig.maxBlocksPerFrame = 4096;
    if (!Th06Rollback::Reset(stateConfig))
        return false;

#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (UseStageTransitionTest() || UseEndingCycle() ||
        UseEliminationCycle() || UseRetryContinueCycle() || UseCoopTransferCycle())
    {
        // Lifecycle harnesses isolate the state transition under test from
        // incidental bullet hits. The elimination cycle explicitly replaces
        // this state at its deterministic terminal-death frames below.
        for (std::uint8_t player = 0; player < g_PlayerCount; ++player)
        {
            SetPlayerLives(player, 8);
            g_Players[player].playerState = PLAYER_STATE_INVULNERABLE;
            g_Players[player].invulnerabilityTimer.SetCurrent(0x3fffffff);
        }
    }
#endif

    SessionConfig sessionConfig;
    sessionConfig.sessionId = CurrentSessionId();
    sessionConfig.seed = static_cast<std::uint32_t>(g_Rng.seed);
    sessionConfig.gameplayAbi = GAMEPLAY_ABI;
    sessionConfig.gameId = GAME_ID_TH06;
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

    g_LastConfirmedFrame.fill(INVALID_FRAME);
    g_LastRemoteSenderFrame.fill(INVALID_FRAME);
    g_RemoteTimeoutArmed.fill(false);
    for (PeerTimeSyncState &state : g_PeerTimeSync)
        state = PeerTimeSyncState{};
    g_PredictionDepth.fill(0);
    g_RollbackByPlayer.fill(0);
    for (RestoreAuditEntry &entry : g_RestoreAudit)
        entry = RestoreAuditEntry{};
    g_RecommendedLead = 0.0;
    g_SimulationIntervalScale = 1.0;
    g_SingleEliminationObserved = false;
    g_CoopRevivableObserved = false;
    g_CoopReviveObserved = false;
    g_CoopPowerTransferObserved = false;
    g_TeamWipeObserved = false;
    g_TeamWipeGraceObserved = false;
    g_TeamWipeRetryObserved = false;
    g_MaxSnapshotBytes = 0;
    g_MaxSnapshotBlocks = 0;
    g_PeakEnemies = 0;
    g_PeakBullets = 0;
    g_PeakLasers = 0;
    g_PeakItems = 0;
    g_TeamWipeStartFrame = INVALID_FRAME;
    g_TeamWipeRetryFrame = INVALID_FRAME;
    g_NextSpectatorPublishFrame = 0;
    g_NextSpectatorReceiveFrame = 0;
    g_SpectatorFrames.clear();
    g_LastConfirmedAdvanceMs.fill(SDL_GetTicks());

    g_Active = true;
#ifdef __EMSCRIPTEN__
    if (ProductionLanMode())
    {
        EM_ASM({
            globalThis.__eaglerNetplayRuntimeBuild = "th06mp-20260831-abi6-pause-restart";
            globalThis.__eaglerNetplayFailed = false;
            globalThis.__eaglerNetplayError = "";
            globalThis.__eaglerNetplayLanActive = true;
            globalThis.__eaglerNetplayLanFrame = 0;
            globalThis.__eaglerNetplayLanGeneration = $0;
            globalThis.__eaglerNetplayLanHighestStage = 0;
            globalThis.__eaglerNetplayLanStageTransitionObserved = false;
            globalThis.__eaglerNetplayEndingEntered = false;
            globalThis.__eaglerNetplayEndingCompleted = false;
            globalThis.__eaglerNetplayResultEntered = false;
            globalThis.__eaglerNetplayResultCompleted = false;
            globalThis.__eaglerNetplayResultState = -1;
            globalThis.__eaglerNetplayReplaySaved = false;
            globalThis.__eaglerNetplayReplaySavedPath = "";
            globalThis.__eaglerNetplaySingleEliminationObserved = false;
            globalThis.__eaglerNetplayTeamWipeObserved = false;
            globalThis.__eaglerNetplayTeamWipeGraceObserved = false;
            globalThis.__eaglerNetplayTeamWipeRetryObserved = false;
            globalThis.__eaglerNetplayTeamWipeStartFrame = -1;
            globalThis.__eaglerNetplayTeamWipeRetryFrame = -1;
            globalThis.__eaglerNetplaySecondTeamWipeGraceObserved = false;
            globalThis.__eaglerNetplaySecondTeamWipeRetryFrame = -1;
            globalThis.__eaglerNetplaySecondRetryContinueObserved = false;
            globalThis.__eaglerNetplaySecondRetryContinueFrame = -1;
            globalThis.__eaglerNetplayRetryResetCount = 0;
            globalThis.__eaglerNetplayRetryResetVerifiedCount = 0;
            globalThis.__eaglerNetplayLanHash = "";
            globalThis.__eaglerNetplayLanHashes = Object.create(null);
            globalThis.__eaglerNetplayLanCanonical = Object.create(null);
            globalThis.__eaglerNetplayLanCanonicalDetail = Object.create(null);
            globalThis.__eaglerNetplayLanLocalDebugHashes = Object.create(null);
            globalThis.__eaglerNetplayLanPeerAdvantages = [];
            globalThis.__eaglerNetplayLanPeers = [];
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

bool DrainPackets()
{
    std::vector<std::uint8_t> wire;
    while (TransportPoll(&wire))
    {
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
        RemoteInputResult result = RemoteInputResult::Duplicate;
        if (!g_Core.ApplyInputPacket(packet, &result))
            return false;
        if (result == RemoteInputResult::RollbackRequired)
            ++g_RollbackByPlayer[packet.senderPlayer];
    }
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

bool SendLocalFrame(std::uint32_t frame)
{
    const FrameInput input = CaptureLocalInput(frame);
    if (UsePhysicalInput() && frame == 0 && !g_PhysicalLoggedFrame0Sample)
    {
        g_PhysicalLoggedFrame0Sample = true;
        std::printf("netplay lan physical: FRAME0 SAMPLE player=%u bits=0x%04x\n",
                    static_cast<unsigned>(g_LocalPlayer),
                    static_cast<unsigned>(input.buttons));
    }
    if (!g_Core.ScheduleLocalInput(frame, input))
        return false;
    return SendScheduledLocalFrame(frame);
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
                packet.frameAdvantage = static_cast<std::int16_t>(SignedFrameDelta(
                    g_SimFrame, g_LastRemoteSenderFrame[peer]));
            }
            std::vector<std::uint8_t> wire;
            if (!EncodeInputPacket(packet, &wire) ||
                !TransportSendTo(peer, wire.data(), wire.size()))
                return false;
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
    if (MultiplayerGameplay::IsMultiplayer() && !g_GameManager.isInReplay && g_ReplayManager)
    {
        ReplayFrameBinding &slot = g_ReplayFrameBindings[frame % g_ReplayFrameBindings.size()];
        if (!resimulation)
        {
            slot.simFrame = frame;
            slot.stage = std::clamp(g_GameManager.currentStage - 1, 0, 6);
            slot.replayFrame = g_ReplayManager->frameId;
        }
        if (slot.simFrame == frame)
            replayBinding = slot;
    }
#endif
    const bool captureRollback = !g_SpectatorMode;
    if (captureRollback && RollbackAuditEnabled())
    {
        RestoreAuditEntry &entry = g_RestoreAudit[frame % g_RestoreAudit.size()];
        entry.frame = frame;
        entry.sample = Th06CanonicalHash::Capture();
        entry.localDebugHash = Th06Rollback::DebugStateHash();
    }
    if (captureRollback && !Th06Rollback::BeginFrame(frame))
        return -1;
    if (captureRollback && UseDenseRollbackProfile())
    {
        // Hidden rollback-budget stress only. Touch every reusable enemy,
        // bullet and item slot without mutating gameplay state. This exercises
        // the exact first-write journal copy/index path (including rollback
        // resimulation) at a density well above ordinary Stage 1 while leaving
        // the game's ECL/object lifecycle untouched.
        for (Enemy &enemy : g_EnemyManager.enemies)
            if (!Th06Rollback::TouchEnemy(&enemy))
                return -1;
        for (Bullet &bullet : g_BulletManager.bullets)
            if (!Th06Rollback::TouchBullet(&bullet))
                return -1;
        for (Item &item : g_ItemManager.items)
            if (!Th06Rollback::TouchItem(&item))
                return -1;
    }
    SideEffects::SimulationFrameScope sideEffectFrame(g_SessionGeneration, frame, resimulation);
    NormalizeMultiplayerEndingSkipHistory();
    // Legacy/global raw-input owners still need one deterministic word. Use
    // the synchronized union on every peer while Player consumes its own
    // slot below. This also prevents Supervisor from re-sampling hardware
    // during rollback resimulation.
    Input::SetReplayOverride(CombinedButtons(decision));
    Input::SetPlayerInputOverrides(decision.inputs.data(), g_PlayerCount);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    ApplyEliminationCycleEvent(frame);
    ApplyCoopTransferCycleEvent(frame);
    if (UseEndingCycle() && frame >= 3000u)
    {
        // Test-only lifecycle acceleration. Do not jump scene state or set
        // Gui::finishedStage directly: keep every real ECL death callback,
        // stage-end message, Gui transition and Supervisor Ending handoff.
        // Once Stage 6 has naturally reached a killable boss phase, collapse
        // only that phase's remaining HP so the smoke does not spend several
        // minutes replaying the full final-boss fight on every run. Because
        // this executes after BeginFrame and on resimulation as well, rollback
        // sees the same deterministic mutation on every peer.
        for (Enemy *boss : g_EnemyManager.bosses)
        {
            if (boss == nullptr || !boss->flags.active || !boss->flags.isBoss)
                continue;
            if (boss->flags.unk6 && boss->life > 0)
                boss->life = 0;
            if (boss->timerCallbackThreshold >= 0 &&
                boss->bossTimer.AsFrames() < boss->timerCallbackThreshold)
            {
                boss->bossTimer.SetCurrent(boss->timerCallbackThreshold);
            }
        }
    }
#endif
    const std::uint8_t stageBeforeCalc =
        static_cast<std::uint8_t>(g_GameManager.currentStage);
    const int result = g_Chain.RunCalcChain();
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (!ObserveEliminationCycle(frame))
        return -1;
    if (!ObserveCoopTransferCycle(frame))
        return -1;
#endif
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
           g_GameManager.isInGameMenu ? 1 : 0,
           g_GameManager.isInRetryMenu ? 1 : 0,
           g_GameManager.isTimeStopped ? 1 : 0);
        EM_ASM({
            const contributionKills = globalThis.__eaglerNetplayContributionKills ||= new Array(3);
            contributionKills[0] = $0;
            contributionKills[1] = $1;
            contributionKills[2] = $2;
            const contributionDamage = globalThis.__eaglerNetplayContributionDamage ||= new Array(3);
            contributionDamage[0] = $3;
            contributionDamage[1] = $4;
            contributionDamage[2] = $5;
        }, GetPlayerEnemiesDefeated(0), GetPlayerEnemiesDefeated(1),
           GetPlayerEnemiesDefeated(2), GetPlayerDamageDealt(0),
           GetPlayerDamageDealt(1), GetPlayerDamageDealt(2));
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
        if (ProductionLanMode())
        {
            EM_ASM({
                globalThis.__eaglerNetplayLanHighestStage = $0;
                if ($0 >= 2) globalThis.__eaglerNetplayLanStageTransitionObserved = true;
            }, static_cast<unsigned>(g_HighestStageObserved));
        }
#endif
    }
#endif
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (UsePauseCycle())
    {
        if (g_GameManager.isInGameMenu)
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
                "netplay lan stage: INPUT_MISMATCH frame=%u local=%u slot=%u expected=0x%04x actual=0x%04x raw=0x%04x lastRaw=0x%04x inGameplay=%d gameMenu=%d retryMenu=%d resim=%d\n",
                frame, static_cast<unsigned>(g_LocalPlayer),
                static_cast<unsigned>(player),
                static_cast<unsigned>(decision.inputs[player].buttons),
                static_cast<unsigned>(g_CurFrameGameInputs[player]),
                static_cast<unsigned>(g_CurFrameInput),
                static_cast<unsigned>(g_LastFrameInput),
                g_GameManager.isInMenu ? 1 : 0,
                g_GameManager.isInGameMenu ? 1 : 0,
                g_GameManager.isInRetryMenu ? 1 : 0,
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
    sideEffectFrame.Finish();
    g_PeakEnemies = std::max(
        g_PeakEnemies, static_cast<std::uint32_t>(g_EnemyManager.enemyCount));
    g_PeakBullets = std::max(
        g_PeakBullets, static_cast<std::uint32_t>(g_BulletManager.bulletCount));
    g_PeakItems = std::max(
        g_PeakItems, static_cast<std::uint32_t>(g_ItemManager.itemCount));
    std::uint32_t activeLasers = 0;
    for (const auto &laser : g_BulletManager.lasers)
        activeLasers += laser.inUse ? 1u : 0u;
    g_PeakLasers = std::max(g_PeakLasers, activeLasers);
    if ((UseStageTransitionTest() || UseEndingCycle()) && !resimulation &&
        (frame % 1800u) == 0u)
    {
        const i32 msg = g_Gui.impl ? g_Gui.impl->msg.currentMsgIdx : -999;
        const i32 msgTime = g_Gui.impl ? g_Gui.impl->msg.timer.AsFrames() : -1;
        const i32 finishedStage = g_Gui.impl ? g_Gui.impl->finishedStage : -1;
        std::printf(
            "netplay lan stage: PROGRESS player=%u frame=%u stage=%d gameFrames=%u msg=%d msgTime=%d finished=%d enemies=%d bullets=%d items=%u playerX=%.1f/%.1f/%.1f playerState=%d/%d/%d lives=%d/%d/%d\n",
            static_cast<unsigned>(g_LocalPlayer), frame,
            g_GameManager.currentStage, g_GameManager.gameFrames,
            msg, msgTime, finishedStage,
            g_EnemyManager.enemyCount, g_BulletManager.bulletCount, g_ItemManager.itemCount,
            g_Players[0].positionCenter.x, g_Players[1].positionCenter.x,
            g_Players[2].positionCenter.x,
            g_Players[0].playerState, g_Players[1].playerState, g_Players[2].playerState,
            GetPlayerLives(0), GetPlayerLives(1), GetPlayerLives(2));
    }
    if (captureRollback)
    {
        if (!Th06Rollback::EndFrame() || !g_Core.MarkSimulated(frame, decision))
            return -1;
    }
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if (replayBinding.simFrame == frame && replayBinding.stage >= 0 &&
        replayBinding.replayFrame >= 0)
    {
        ReplayExtension::RecordMultiplayerFrame(
            replayBinding.stage, replayBinding.replayFrame,
            decision.inputs.data(), g_PlayerCount);
    }
#endif
    if (captureRollback)
    {
        const std::size_t snapshotBytes = Th06Rollback::CapturedBytes(frame);
        const std::size_t snapshotBlocks = Th06Rollback::CapturedBlocks(frame);
        g_MaxSnapshotBytes = std::max(g_MaxSnapshotBytes, snapshotBytes);
        g_MaxSnapshotBlocks = std::max(g_MaxSnapshotBlocks, snapshotBlocks);
    }
#ifdef __EMSCRIPTEN__
    if (ProductionLanMode() && UseDenseRollbackProfile())
    {
        EM_ASM({
            globalThis.__eaglerNetplayDenseRollback = true;
            globalThis.__eaglerNetplayMaxSnapshotBytes = $0;
            globalThis.__eaglerNetplayMaxSnapshotBlocks = $1;
        }, static_cast<double>(g_MaxSnapshotBytes),
           static_cast<double>(g_MaxSnapshotBlocks));
    }
#endif
#ifdef __EMSCRIPTEN__
    const std::uint32_t canonicalInterval = RollbackAuditEnabled() ? 60u : 300u;
    if (ProductionLanMode() && ((frame + 1u) % canonicalInterval) == 0u)
    {
        // This lives inside SimulateFrame rather than the outer driver so a
        // rollback resimulation overwrites the sample for the same logical
        // frame. Consumers only trust a sample once that frame is confirmed.
        const auto sample = Th06CanonicalHash::Capture();
        char total[17];
        HashToHex(sample.composite, total);
        EM_ASM({
            const hash = UTF8ToString($0);
            globalThis.__eaglerNetplayLanHash = hash;
            globalThis.__eaglerNetplayLanHashes[String($1)] = hash;
        }, total, frame + 1u);
        if (RollbackAuditEnabled())
        {
            const auto localDebugSample = Th06Rollback::DebugStateHash();
            char meta[17];
            char stage[17];
            char player[17];
            char enemies[17];
            char bullets[17];
            char items[17];
            char localDebug[17];
            char rng[17];
            char game[17];
            char input[17];
            char supervisor[17];
            char player0[17];
            char player1[17];
            char player2[17];
            HashToHex(sample.meta, meta);
            HashToHex(sample.stage, stage);
            HashToHex(sample.player, player);
            HashToHex(sample.enemies, enemies);
            HashToHex(sample.bullets, bullets);
            HashToHex(sample.items, items);
            HashToHex(localDebugSample, localDebug);
            HashToHex(sample.metaRng, rng);
            HashToHex(sample.metaGame, game);
            HashToHex(sample.metaInput, input);
            HashToHex(sample.metaSupervisor, supervisor);
            HashToHex(sample.player0, player0);
            HashToHex(sample.player1, player1);
            HashToHex(sample.player2, player2);
            EM_ASM({
                const detail = Object.create(null);
                detail.composite = UTF8ToString($0);
                detail.meta = UTF8ToString($1);
                detail.stage = UTF8ToString($2);
                detail.player = UTF8ToString($3);
                detail.enemies = UTF8ToString($4);
                detail.bullets = UTF8ToString($5);
                detail.items = UTF8ToString($6);
                detail.counts = [];
                detail.counts[0] = $7;
                detail.counts[1] = $8;
                detail.counts[2] = $9;
                detail.counts[3] = $10;
                globalThis.__eaglerNetplayLanCanonical[String($11)] = detail;
                globalThis.__eaglerNetplayLanLocalDebugHashes[String($11)] = UTF8ToString($12);
            }, total, meta, stage, player, enemies, bullets, items,
               sample.enemyCount, sample.bulletCount, sample.laserCount, sample.itemCount,
               frame + 1u, localDebug);
            EM_ASM({
                const detail = [];
                detail[0] = UTF8ToString($0);
                detail[1] = UTF8ToString($1);
                detail[2] = UTF8ToString($2);
                detail[3] = UTF8ToString($3);
                detail[4] = UTF8ToString($4);
                detail[5] = UTF8ToString($5);
                detail[6] = UTF8ToString($6);
                globalThis.__eaglerNetplayLanCanonicalDetail[String($7)] = detail;
            }, rng, game, input, supervisor, player0, player1, player2, frame + 1u);
            const Player &p0 = g_Players[0];
            EM_ASM({
                const detail = [];
                detail[0] = $0;
                detail[1] = $1;
                detail[2] = $2;
                detail[3] = $3;
                detail[4] = $4;
                detail[5] = $5;
                detail[6] = $6;
                detail[7] = $7;
                detail[8] = $8;
                detail[9] = $9;
                globalThis.__eaglerNetplayLanPlayer0Detail = detail;
            },
               p0.positionCenter.x, p0.positionCenter.y,
               p0.prevPositionCenter.x, p0.prevPositionCenter.y,
               static_cast<int>(p0.playerState), static_cast<int>(p0.isFocus),
               static_cast<int>(p0.playerDirection),
               p0.previousHorizontalSpeed, p0.previousVerticalSpeed,
               static_cast<int>(p0.previousFrameInput));
            EM_ASM({
                const detail = globalThis.__eaglerNetplayLanPlayer0Detail || [];
                detail[10] = $0;
                detail[11] = $1;
                detail[12] = $2;
                detail[13] = $3;
                globalThis.__eaglerNetplayLanPlayer0Detail = detail;
            }, p0.fireBulletTimer.AsFrames(), p0.invulnerabilityTimer.AsFrames(),
               static_cast<int>(p0.orbState), static_cast<int>(p0.bombInfo.isInUse));
        }
    }
#endif
    if (resimulation)
        ++g_ResimulatedFrames;
    return result;
}

bool ReconcileRollback()
{
    if (!g_Core.HasRollbackRequest())
        return true;

    const std::uint32_t rollback = g_Core.RollbackFrame();
    const std::uint32_t last = g_Core.LastSimulatedFrame();
    if (last == INVALID_FRAME || rollback > last)
    {
        // The corrected frame has not run yet. Its exact input will be used
        // normally, so there is no state to rewind.
        g_Core.ClearRollbackRequest();
        return true;
    }
    std::uint32_t replayFrom = rollback;
    if (!Th06Rollback::RestoreTo(rollback, &replayFrom))
    {
        if (Th06Rollback::Failed())
            return false;
        // Stage changes deliberately invalidate the previous stage's journal.
        // Match final sbrik's mature failure policy: do not freeze/end the
        // whole session by retrying an impossible rewind forever. Continue on
        // the current state and surface the abandoned correction in the log.
        std::printf(
            "netplay lan stage: WARN rollback correction abandoned pending=%u last=%u\n",
            rollback, last);
        g_Core.ClearRollbackRequest();
        return true;
    }
    if (RollbackAuditEnabled())
    {
        const RestoreAuditEntry &expected =
            g_RestoreAudit[replayFrom % g_RestoreAudit.size()];
        const auto actual = Th06CanonicalHash::Capture();
        const auto actualLocalDebug = Th06Rollback::DebugStateHash();
        if (expected.frame != replayFrom ||
            expected.sample.composite != actual.composite ||
            expected.localDebugHash != actualLocalDebug)
        {
            std::printf(
                "netplay rollback audit: FAIL requested=%u restored=%u last=%u tag=%u "
                "local=%016llx/%016llx "
                "composite=%016llx/%016llx meta=%016llx/%016llx "
                "stage=%016llx/%016llx player=%016llx/%016llx "
                "enemies=%016llx/%016llx bullets=%016llx/%016llx "
                "items=%016llx/%016llx counts=%u/%u/%u/%u:%u/%u/%u/%u\n",
                rollback, replayFrom, last, expected.frame,
                static_cast<unsigned long long>(expected.localDebugHash),
                static_cast<unsigned long long>(actualLocalDebug),
                static_cast<unsigned long long>(expected.sample.composite),
                static_cast<unsigned long long>(actual.composite),
                static_cast<unsigned long long>(expected.sample.meta),
                static_cast<unsigned long long>(actual.meta),
                static_cast<unsigned long long>(expected.sample.stage),
                static_cast<unsigned long long>(actual.stage),
                static_cast<unsigned long long>(expected.sample.player),
                static_cast<unsigned long long>(actual.player),
                static_cast<unsigned long long>(expected.sample.enemies),
                static_cast<unsigned long long>(actual.enemies),
                static_cast<unsigned long long>(expected.sample.bullets),
                static_cast<unsigned long long>(actual.bullets),
                static_cast<unsigned long long>(expected.sample.items),
                static_cast<unsigned long long>(actual.items),
                expected.sample.enemyCount, expected.sample.bulletCount,
                expected.sample.laserCount, expected.sample.itemCount,
                actual.enemyCount, actual.bulletCount,
                actual.laserCount, actual.itemCount);
            return false;
        }
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

    // Ordinary resimulation SFX remain suppressed. Replay only the SFX IDs
    // produced by a corrected player Bomb's initial calc that were not already
    // heard on the original forward pass.
    const std::uint32_t correctedBombStartSounds = SideEffects::ConsumeCorrectedBombStartSounds();
    for (std::uint32_t soundId = 0; soundId < 32; ++soundId)
    {
        if ((correctedBombStartSounds & (std::uint32_t{1} << soundId)) != 0)
            g_SoundPlayer.PlaySoundByIdx(static_cast<SoundIdx>(soundId));
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

bool Requested()
{
    return RequestedInternal();
}

double SimulationIntervalScale()
{
    return ProductionLanMode() ? g_SimulationIntervalScale : 1.0;
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
        // Result/Ending/MainMenu remain usable locally, but this Runtime's
        // one-shot spectator admission may not start another netplay session.
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
        return CHAIN_CALLBACK_RESULT_CONTINUE;
    if (g_Initialized && !SessionStillOwnsStageState())
    {
        // Gameplay ownership is one deterministic rollback session.
        // TH06 retires that session as soon as the confirmed gameplay state
        // requests a post-game scene (Ending/Result/MainMenu). A later new run
        // starts again from frame zero while reusing only the established
        // browser transport. TH06 multiplayer never exposes Continue; a team
        // wipe retires this generation directly into Result after its grace.
        RetireGameplaySession();
    }
    if (!g_Initialized && !EligibleForInitialNetplayStart())
    {
        if (!InitialNetplayBootstrapInProgress())
        {
            // LAN mode intentionally remains active after gameplay so the
            // Launcher room survives. ResultScreen/Ending/MainMenu are local
            // post-game scenes and must regain ordinary UI input; neutral
            // lanes are only for the real pre-frame-zero GameManager window.
            g_LastTickAdvanced = true;
            return g_Chain.RunCalcChain();
        }
        // The Launcher has already committed this run to LAN play, but TH06
        // still needs a few vanilla calc ticks to finish constructing the
        // Stage/Player/ReplayManager objects before the deterministic gate can
        // take ownership. Do not let machine-local keyboard/touch state leak
        // into those bootstrap ticks: otherwise the local player can move a
        // frame or two before HELLO/READY while the same slot stays still on
        // every remote peer, so rollback begins from different world states.
        // Keep the initialization callbacks running, but feed them neutral
        // logical lanes until synchronized frame zero is ready.
        std::array<FrameInput, TH06_MULTI_MAX_PLAYERS> neutralInputs{};
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
    if (!ReconcileRollback())
    {
        Fail("rollback reconcile");
        return -1;
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
        const std::uint32_t confirmed = ConfirmedThroughAllRemotes();
        EM_ASM({
            globalThis.__eaglerNetplayLanConfirmed = $0;
            globalThis.__eaglerNetplayLanRollback = $1;
            globalThis.__eaglerNetplayLanResimulated = $2;
        }, confirmed, g_RollbackCount, g_ResimulatedFrames);
    }
#endif

    if (((!ProbeMode() && !UseReplayPlaybackCycle()) || g_SimFrame < g_TestFrames) &&
        TransportIsOpen())
    {
        bool localPresent = false;
        (void)g_Core.LocalInput(g_SimFrame, &localPresent);
        if (!localPresent && !SendLocalFrame(g_SimFrame))
        {
            Fail("send local input");
            return -1;
        }
        if (localPresent && (g_DriverTicks % 3u) == 0u &&
            !SendScheduledLocalFrame(g_SimFrame))
        {
            Fail("input retry");
            return -1;
        }

        // Frame zero is the session barrier. Receive every peer's real first
        // input before gameplay advances. Later frames may use prediction.
        if (g_SimFrame == 0 && ConfirmedThroughAllRemotes() == INVALID_FRAME)
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
            ConfirmedThroughAllRemotes() < g_SimFrame)
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
    if (UseReplayPlaybackCycle() && g_SimFrame >= g_TestFrames &&
        ConfirmedThroughAllRemotes() >= g_TestFrames - 1 &&
        !g_Core.HasRollbackRequest())
    {
        // Hidden short end-to-end Replay gate. Save only after every input in
        // the recording window is confirmed, outside SimulateFrame and its
        // rollback journal, then return through the real Supervisor/MainMenu
        // Replay path. The default LAN runtime never enables this option.
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
        FileSystem::CreateDir("./replay");
        ReplayManager::SaveReplay("./replay/th6_01.rpy", replayName);
        EM_ASM({ Module.eaglerOptions.replayViewer = true; });
        g_Supervisor.curState = SUPERVISOR_STATE_MAINMENU;
        return CHAIN_CALLBACK_RESULT_CONTINUE;
    }
#endif

    if (ProbeMode() && g_SimFrame >= g_TestFrames &&
        ConfirmedThroughAllRemotes() >= g_TestFrames - 1 &&
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
        const auto sample = Th06Rollback::DebugStateHash();
        PrintHash("FINAL", sample);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        if (g_PlayerCount >= 3)
        {
            std::printf(
                "netplay lan stage: PLAYERS p1=(%.2f,%.2f) p2=(%.2f,%.2f) p3=(%.2f,%.2f) independentInputs=1 physicalInput=%d\n",
                g_Players[0].positionCenter.x, g_Players[0].positionCenter.y,
                g_Players[1].positionCenter.x, g_Players[1].positionCenter.y,
                g_Players[2].positionCenter.x, g_Players[2].positionCenter.y,
                UsePhysicalInput() ? 1 : 0);
        }
        else
        {
            std::printf(
                "netplay lan stage: PLAYERS p1=(%.2f,%.2f) p2=(%.2f,%.2f) independentInputs=1 physicalInput=%d\n",
                g_Players[0].positionCenter.x, g_Players[0].positionCenter.y,
                g_Players[1].positionCenter.x, g_Players[1].positionCenter.y,
                UsePhysicalInput() ? 1 : 0);
        }
#endif
        std::printf(
            "netplay lan stage: PASS player=%u players=%u frames=%u sent=%u recv=%u session=%u/%u rollback=%u resim=%u maxRollback=%u predicted=%u confirmed=%u maxSnapshot=%llu buffered=%llu peak=%u/%u/%u/%u pause=%d/%d restart=%u stageMax=%u\n",
            static_cast<unsigned>(g_LocalPlayer), static_cast<unsigned>(g_PlayerCount),
            g_TestFrames, g_SentPackets,
            g_ReceivedPackets, g_SessionPacketsSent, g_SessionPacketsReceived,
            g_RollbackCount, g_ResimulatedFrames, g_MaxRollbackSpan, g_PredictedFrames,
            static_cast<unsigned>(ConfirmedThroughAllRemotes()),
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
    if (ProbeMode() && g_DriverTicks >= g_TestFrames * 6u)
    {
        Fail("timeout");
        return -1;
    }
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}
} // namespace Netplay::Th06LanStageProbe
