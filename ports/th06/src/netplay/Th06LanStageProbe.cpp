#include "Th06LanStageProbe.hpp"

#include <eagler/netplay/NetplayCore.hpp>
#include <eagler/netplay/NetplayInput.hpp>
#include <eagler/netplay/NetplayProtocol.hpp>
#include <eagler/netplay/NetplaySession.hpp>
#include "NetplaySideEffects.hpp"
#include <eagler/netplay/SnapshotPolicy.hpp>
#include "Th06CanonicalHash.hpp"
#include "Th06RollbackState.hpp"
#include <eagler/netplay/BrowserPeerTransport.hpp>
#include <eagler/netplay/WebSocketTransport.hpp>
#include <eagler/netplay/InputRepairBudget.hpp>
#include <eagler/netplay/FrameAdvantageWindow.hpp>
#include <eagler/netplay/FramePacingPolicy.hpp>
#include <eagler/netplay/ConfirmedInputWatchdog.hpp>
#include <eagler/netplay/SpectatorStagePlayback.hpp>
#include <eagler/netplay/SpectatorStagePublisher.hpp>
#include <eagler/netplay/SpectatorStageScalars.hpp>
#include <eagler/netplay/SpectatorStageView.hpp>

#include "BulletManager.hpp"
#include "AsciiManager.hpp"
#include "Chain.hpp"
#include "Controller.hpp"
#include "EnemyManager.hpp"
#include "EnemyEclInstr.hpp"
#include "FileSystem.hpp"
#include "GameManager.hpp"
#include "Gui.hpp"
#include "ItemManager.hpp"
#include "MainMenu.hpp"
#include "Player.hpp"
#include "ReplayExtension.hpp"
#include "ReplayManager.hpp"
#include "Rng.hpp"
#include "SoundPlayer.hpp"
#include "Stage.hpp"
#include "Supervisor.hpp"
#include "Touch.hpp"
#include "pbg4/Lzss.hpp"
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#include "multiplayer/GameplaySession.hpp"
#endif

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <utility>
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
static std::uint32_t GameplayAbi() { return TH06_MULTI_GAMEPLAY_ABI | (MultiplayerGameplay::IsChallengeMode() ? 0x10000u : 0u); }
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
std::uint16_t g_GameplayStartSeed = 0;
std::uint8_t g_LocalPlayer = 0;
std::uint8_t g_PlayerCount = 2;
std::uint32_t g_SimFrame = 0;
std::uint32_t g_DriverTicks = 0;
std::uint32_t g_UiCaptureFrame = 0;
std::uint64_t g_NextUiCaptureNs = 0;
FrameInput g_LastUiCapture;
bool g_HaveLastUiCapture = false;
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
std::array<FrameInput, MAX_PLAYERS> g_SpectatorDebugFrame34FirstInputs{};
bool g_SpectatorDebugFrame34FirstSeen = false;
std::uint32_t g_SnapshotTicks = 0;
std::uint32_t g_ConfirmedOnlyTicks = 0;
std::size_t g_MaxSnapshotBytes = 0;
std::size_t g_MaxSnapshotBlocks = 0;
bool g_FrontierSnapshots = false;
bool g_DemandSnapshots = false;

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
std::uint32_t g_DirectTouchBeginCaptures = 0;
std::uint32_t g_DirectTouchDeltaCaptures = 0;
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
SpectatorStagePublisher g_SpectatorPublisher;
SpectatorStagePlayback g_SpectatorPlayback;
SpectatorStageView g_SpectatorView;
SpectatorStageEvent g_PendingSpectatorBootstrap{};
bool g_HavePendingSpectatorBootstrap = false;
std::uint8_t g_SpectatorBossSlotsThisFrame = 0;
bool g_SpectatorNativeStageReady = false;
bool g_SpectatorPublisherReady = false;
bool g_SpectatorPublisherEligible = false;
bool g_SpectatorLaneStopped = false;
std::array<std::uint32_t, 64> g_StageStartFrameTags{};
std::array<std::uint8_t, 64> g_StageAtFrameStart{};
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
std::array<FrameAdvantageWindow, MAX_PLAYERS> g_PeerTimeSync{};
std::array<std::uint32_t, MAX_PLAYERS> g_PredictionDepth{};
std::array<std::uint32_t, MAX_PLAYERS> g_RollbackByPlayer{};
std::array<InputRepairBudget, MAX_PLAYERS> g_InputRepairBudgets{};
bool g_ReliableInputRepair = false;
double g_RecommendedLead = 0.0;
double g_SimulationIntervalScale = 1.0;

struct RestoreAuditEntry
{
    std::uint32_t frame = INVALID_FRAME;
    Th06CanonicalHash::Sample sample{};
    std::uint64_t localDebugHash = 0;
    Th06Rollback::DebugStatePartArray localDebugParts{};
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
void DisableSpectatorPublisher(std::uint32_t frame, const char *reason);
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

// Schema 6 wraps schema-5 named scalars with bounded sparse/LZSS encoding.
// The scalar stream remains explicit and never contains native object images.
constexpr std::uint32_t SPECTATOR_CHECKPOINT_SCHEMA = 6;
constexpr std::uint32_t SPECTATOR_CHECKPOINT_MAGIC = 0x36564353; // SCV6
constexpr std::uint32_t SPECTATOR_CHECKPOINT_RAW_LIMIT = 256u * 1024u;

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
    // The legacy decoder accepts zero-padded truncated bitstreams. Re-encode
    // and compare so only canonical, complete LZSS capsules are accepted.
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
struct SpectatorVec3Checkpoint { f32 x = 0.0f, y = 0.0f, z = 0.0f; };
struct SpectatorTimerCheckpoint { i32 previous = 0, current = 0; f32 subFrame = 0.0f; };
struct SpectatorPlayerBulletCheckpoint
{
    std::uint8_t player = 0, slot = 0;
    f32 positionX = 0.0f, positionY = 0.0f, positionZ = 0.0f;
    f32 previousPositionX = 0.0f, previousPositionY = 0.0f, previousPositionZ = 0.0f;
    f32 sizeX = 0.0f, sizeY = 0.0f, sizeZ = 0.0f;
    f32 velocityX = 0.0f, velocityY = 0.0f, sidewaysMotion = 0.0f;
    f32 unk134X = 0.0f, unk134Y = 0.0f, unk134Z = 0.0f;
    i32 unk140Previous = 0, unk140Current = 0;
    f32 unk140SubFrame = 0.0f;
    i32 damage = 0, bulletState = 0, bulletType = 0, unk152 = 0, spawnPositionIdx = 0;
    i32 anmFileIndex = -1;
    bool hasVm = false;
    i32 vmInstructionOffset = -1;
    i16 vmActiveSpriteIndex = -1, vmBaseSpriteIndex = -1;
    u32 vmFlags = 0, vmColor = 0, vmPrevColor = 0, vmAlphaInitial = 0, vmAlphaFinal = 0;
    i16 vmAlphaEnd = 0, vmScaleEnd = 0, vmAutoRotate = 0, vmPendingInterrupt = 0, vmPosEnd = 0;
    i32 vmTimeOfLastSpriteSet = 0;
    u8 vmFontWidth = 0, vmFontHeight = 0;
    SpectatorVec3Checkpoint vmRotation{}, vmPrevRotation{}, vmAngleVel{}, vmPos{}, vmPrevPos{};
    SpectatorVec3Checkpoint vmPosInterpInitial{}, vmPosInterpFinal{}, vmPosOffset{};
    f32 vmScaleX = 0.0f, vmScaleY = 0.0f, vmPrevScaleX = 0.0f, vmPrevScaleY = 0.0f;
    f32 vmScaleFinalX = 0.0f, vmScaleFinalY = 0.0f, vmScaleInitialX = 0.0f, vmScaleInitialY = 0.0f;
    f32 vmUvX = 0.0f, vmUvY = 0.0f, vmPrevUvX = 0.0f, vmPrevUvY = 0.0f;
    SpectatorTimerCheckpoint vmScriptTime{}, vmScaleTime{}, vmPosTime{}, vmAlphaTime{};
};
struct SpectatorPlayerCheckpoint
{
    SpectatorVec3Checkpoint unk44c, hitboxTopLeft, hitboxBottomRight, grabItemTopLeft, grabItemBottomRight;
    SpectatorVec3Checkpoint hitboxSize, grabItemSize;
    std::array<SpectatorVec3Checkpoint, 2> orbsPosition{};
    std::array<SpectatorVec3Checkpoint, 32> bombRegionPositions{}, bombRegionSizes{};
    std::array<i32, 32> bombRegionDamages{}, unk838{};
    std::array<f32, 64> bombProjectiles{};
    std::array<SpectatorTimerCheckpoint, 2> laserTimer{};
    f32 horizontalMovementSpeedMultiplierDuringBomb = 0.0f;
    f32 verticalMovementSpeedMultiplierDuringBomb = 0.0f;
    u8 initParam = 0;
    i32 lifeGiveTimer = 0, lifeGiveTargetToken = 0, powerGiveTaps = 0, powerGiveWindow = 0;
    i32 orbState = 0, isFocus = 0, unk9e4 = 0;
    SpectatorTimerCheckpoint focusMovementTimer{};
    i32 playerDirection = 0;
    f32 previousHorizontalSpeed = 0.0f, previousVerticalSpeed = 0.0f;
    i16 previousFrameInput = 0;
    SpectatorVec3Checkpoint positionOfLastEnemyHit{};
    SpectatorTimerCheckpoint fireBulletTimer{}, teamBombProtectionTimer{}, bombTimer{};
    u32 bombIsInUse = 0;
    i32 bombDuration = 0;
    std::array<i32, 8> bombProjectileStates{};
    std::array<f32, 8> bombProjectileRelated{};
    std::array<SpectatorVec3Checkpoint, 8> bombPositions{}, bombVelocities{};
};
struct SpectatorCheckpoint
{
    std::uint32_t magic = 0x36504353; // SCP6
    std::int32_t stage = 0, difficulty = 0, rank = 0, maxRank = 0, minRank = 0, subRank = 0;
    std::uint16_t initialSeed = 0, stageSeed = 0, rngSeed = 0, currentPower = 0, pointItemsInStage = 0, pointItems = 0;
    std::uint16_t randomItemSpawnIndex = 0, randomItemTableIndex = 0;
    std::uint32_t rngGeneration = 0;
    std::uint64_t expectedHash = 0;
    std::uint64_t expectedPlayer0 = 0, expectedPlayer1 = 0, expectedPlayer2 = 0;
    std::array<f32, TH06_MULTI_MAX_PLAYERS * 3> playerPositionCenter{};
    std::array<i32, TH06_MULTI_MAX_PLAYERS> playerRespawnTimer{}, playerBulletGracePeriod{}, playerState{};
    std::array<i32, TH06_MULTI_MAX_PLAYERS * 2> playerInvulnerabilityTimer{};
    std::array<f32, TH06_MULTI_MAX_PLAYERS> playerInvulnerabilitySubFrame{};
    std::array<SpectatorPlayerCheckpoint, TH06_MULTI_MAX_PLAYERS> players{};
    std::uint32_t guiScore = 0, score = 0, nextScoreIncrement = 0, highScore = 0, gameFrames = 0;
    std::int32_t grazeInStage = 0, grazeTotal = 0, deaths = 0, bombsUsed = 0, spellcardsCaptured = 0;
    std::int32_t demoFrames = 0, menuCursorBackup = 0;
    std::uint8_t numRetries = 0, powerItemCountForScore = 0, livesRemaining = 0, bombsRemaining = 0;
    std::uint8_t extraLives = 0, character = 0, shotType = 0;
    std::uint8_t isTimeStopped = 0, isInReplay = 0, isInGameMenu = 0, isInRetryMenu = 0;
    std::uint8_t isInMenu = 0, isGameCompleted = 0, isInPracticeMode = 0, demoMode = 0;
    ZunVec2 arcadeTop{}, arcadeSize{}, prevArcadeTop{}, prevArcadeSize{};
    ZunVec2 movementTop{}, movementSize{};
    f32 cameraDistance = 0.0f;
    ZunVec3 cameraFacing{};
    std::uint16_t curInput = 0, lastInput = 0, eighthHeld = 0, heldFrames = 0;
    std::array<std::uint16_t, TH06_MULTI_MAX_PLAYERS> curPlayerInput{}, lastPlayerInput{};
    MultiplayerGameplay::SessionState session{};
    std::array<i32, TH06_MULTI_MAX_PLAYERS> lives{}, bombs{}, powers{}, kills{}, damage{}, challengeDeaths{};
    std::vector<SpectatorPlayerBulletCheckpoint> playerBullets;
    i32 teamWipeRetryFrames = 0;
    i32 supervisorWanted = 0, supervisorCurrent = 0, supervisorWanted2 = 0;
    bool supervisorEnding = false;
    f32 supervisorFramerate = 0.0f;
};

void CaptureSpectatorVec3(SpectatorVec3Checkpoint *out, const ZunVec3 &value)
{
    out->x = value.x; out->y = value.y; out->z = value.z;
}

void ApplySpectatorVec3(const SpectatorVec3Checkpoint &value, ZunVec3 *out)
{
    *out = ZunVec3(value.x, value.y, value.z);
}

void CaptureSpectatorTimer(SpectatorTimerCheckpoint *out, const ZunTimer &value)
{
    out->previous = value.previous; out->current = value.current; out->subFrame = value.subFrame;
}

void ApplySpectatorTimer(const SpectatorTimerCheckpoint &value, ZunTimer *out)
{
    out->previous = value.previous; out->current = value.current; out->subFrame = value.subFrame;
}

bool WriteSpectatorVec3(SpectatorScalars::Writer &writer, const SpectatorVec3Checkpoint &value)
{
    return writer.Values(value.x, value.y, value.z);
}

bool ReadSpectatorVec3(SpectatorScalars::Reader &reader, SpectatorVec3Checkpoint *value)
{
    return reader.Values(value->x, value->y, value->z);
}

bool WriteSpectatorTimer(SpectatorScalars::Writer &writer, const SpectatorTimerCheckpoint &value)
{
    return writer.Values(value.previous, value.subFrame, value.current);
}

bool ReadSpectatorTimer(SpectatorScalars::Reader &reader, SpectatorTimerCheckpoint *value)
{
    return reader.Values(value->previous, value->subFrame, value->current);
}

u32 CaptureSpectatorAnmFlags(const AnmVmFlags &flags)
{
    return (u32(flags.isVisible) << 0) | (u32(flags.flag1) << 1) | (u32(flags.blendMode) << 2) |
           (u32(flags.colorOp) << 3) | (u32(flags.flag4) << 4) | (u32(flags.usePosOffset) << 5) |
           (u32(flags.flip) << 6) | (u32(flags.anchor) << 8) | (u32(flags.posTime) << 10) |
           (u32(flags.zWriteDisable) << 12) | (u32(flags.isStopped) << 13);
}

void ApplySpectatorAnmFlags(u32 bits, AnmVmFlags *flags)
{
    flags->isVisible = (bits >> 0) & 1; flags->flag1 = (bits >> 1) & 1;
    flags->blendMode = (bits >> 2) & 1; flags->colorOp = (bits >> 3) & 1;
    flags->flag4 = (bits >> 4) & 1; flags->usePosOffset = (bits >> 5) & 1;
    flags->flip = (bits >> 6) & 3; flags->anchor = (bits >> 8) & 3;
    flags->posTime = (bits >> 10) & 3; flags->zWriteDisable = (bits >> 12) & 1;
    flags->isStopped = (bits >> 13) & 1;
}

bool WriteSpectatorBullet(SpectatorScalars::Writer &writer, const SpectatorPlayerBulletCheckpoint &b)
{
    if (!writer.Values(b.player, b.slot, b.positionX, b.positionY, b.positionZ,
                       b.previousPositionX, b.previousPositionY, b.previousPositionZ,
                       b.sizeX, b.sizeY, b.sizeZ, b.velocityX, b.velocityY,
                       b.sidewaysMotion, b.unk134X, b.unk134Y, b.unk134Z,
                       b.unk140Previous, b.unk140SubFrame, b.unk140Current,
                       b.damage, b.bulletState, b.bulletType, b.unk152,
                       b.spawnPositionIdx, b.anmFileIndex, b.hasVm)) return false;
    if (!b.hasVm) return true;
    if (!writer.Values(b.vmInstructionOffset, b.vmActiveSpriteIndex, b.vmBaseSpriteIndex,
                       b.vmFlags, b.vmColor, b.vmPrevColor, b.vmAlphaInitial, b.vmAlphaFinal,
                       b.vmAlphaEnd, b.vmScaleEnd, b.vmAutoRotate, b.vmPendingInterrupt, b.vmPosEnd,
                       b.vmTimeOfLastSpriteSet, b.vmFontWidth, b.vmFontHeight,
                       b.vmScaleX, b.vmScaleY, b.vmPrevScaleX, b.vmPrevScaleY,
                       b.vmScaleFinalX, b.vmScaleFinalY, b.vmScaleInitialX, b.vmScaleInitialY,
                       b.vmUvX, b.vmUvY, b.vmPrevUvX, b.vmPrevUvY)) return false;
    return WriteSpectatorVec3(writer, b.vmRotation) && WriteSpectatorVec3(writer, b.vmPrevRotation) &&
           WriteSpectatorVec3(writer, b.vmAngleVel) && WriteSpectatorVec3(writer, b.vmPos) &&
           WriteSpectatorVec3(writer, b.vmPrevPos) && WriteSpectatorVec3(writer, b.vmPosInterpInitial) &&
           WriteSpectatorVec3(writer, b.vmPosInterpFinal) && WriteSpectatorVec3(writer, b.vmPosOffset) &&
           WriteSpectatorTimer(writer, b.vmScriptTime) && WriteSpectatorTimer(writer, b.vmScaleTime) &&
           WriteSpectatorTimer(writer, b.vmPosTime) && WriteSpectatorTimer(writer, b.vmAlphaTime);
}

bool ReadSpectatorBullet(SpectatorScalars::Reader &reader, SpectatorPlayerBulletCheckpoint *b)
{
    if (!reader.Values(b->player, b->slot, b->positionX, b->positionY, b->positionZ,
                       b->previousPositionX, b->previousPositionY, b->previousPositionZ,
                       b->sizeX, b->sizeY, b->sizeZ, b->velocityX, b->velocityY,
                       b->sidewaysMotion, b->unk134X, b->unk134Y, b->unk134Z,
                       b->unk140Previous, b->unk140SubFrame, b->unk140Current,
                       b->damage, b->bulletState, b->bulletType, b->unk152,
                       b->spawnPositionIdx, b->anmFileIndex, b->hasVm)) return false;
    if (!b->hasVm) return true;
    if (!reader.Values(b->vmInstructionOffset, b->vmActiveSpriteIndex, b->vmBaseSpriteIndex,
                       b->vmFlags, b->vmColor, b->vmPrevColor, b->vmAlphaInitial, b->vmAlphaFinal,
                       b->vmAlphaEnd, b->vmScaleEnd, b->vmAutoRotate, b->vmPendingInterrupt, b->vmPosEnd,
                       b->vmTimeOfLastSpriteSet, b->vmFontWidth, b->vmFontHeight,
                       b->vmScaleX, b->vmScaleY, b->vmPrevScaleX, b->vmPrevScaleY,
                       b->vmScaleFinalX, b->vmScaleFinalY, b->vmScaleInitialX, b->vmScaleInitialY,
                       b->vmUvX, b->vmUvY, b->vmPrevUvX, b->vmPrevUvY)) return false;
    return ReadSpectatorVec3(reader, &b->vmRotation) && ReadSpectatorVec3(reader, &b->vmPrevRotation) &&
           ReadSpectatorVec3(reader, &b->vmAngleVel) && ReadSpectatorVec3(reader, &b->vmPos) &&
           ReadSpectatorVec3(reader, &b->vmPrevPos) && ReadSpectatorVec3(reader, &b->vmPosInterpInitial) &&
           ReadSpectatorVec3(reader, &b->vmPosInterpFinal) && ReadSpectatorVec3(reader, &b->vmPosOffset) &&
           ReadSpectatorTimer(reader, &b->vmScriptTime) && ReadSpectatorTimer(reader, &b->vmScaleTime) &&
           ReadSpectatorTimer(reader, &b->vmPosTime) && ReadSpectatorTimer(reader, &b->vmAlphaTime);
}

void CaptureSpectatorBullet(const PlayerBullet &bullet, std::uint8_t player, std::uint8_t slot,
                            SpectatorPlayerBulletCheckpoint *out)
{
    out->player = player; out->slot = slot;
    out->positionX = bullet.position.x; out->positionY = bullet.position.y; out->positionZ = bullet.position.z;
    out->previousPositionX = bullet.prevPosition.x; out->previousPositionY = bullet.prevPosition.y;
    out->previousPositionZ = bullet.prevPosition.z;
    out->sizeX = bullet.size.x; out->sizeY = bullet.size.y; out->sizeZ = bullet.size.z;
    out->velocityX = bullet.velocity.x; out->velocityY = bullet.velocity.y;
    out->sidewaysMotion = bullet.sidewaysMotion;
    out->unk134X = bullet.unk_134.x; out->unk134Y = bullet.unk_134.y; out->unk134Z = bullet.unk_134.z;
    out->unk140Previous = bullet.unk_140.previous; out->unk140Current = bullet.unk_140.current;
    out->unk140SubFrame = bullet.unk_140.subFrame;
    out->damage = bullet.damage; out->bulletState = bullet.bulletState; out->bulletType = bullet.bulletType;
    out->unk152 = bullet.unk_152; out->spawnPositionIdx = bullet.spawnPositionIdx;
    const AnmVm &vm = bullet.sprite;
    out->anmFileIndex = vm.anmFileIndex;
    // Used inactive slots keep their bullet scalars because a future spawn
    // does not overwrite every TH06 bullet field. Their VM is reinitialized
    // by SetAndExecuteScriptIdx on the next spawn, so only active slots need
    // the larger animation/timing checkpoint.
    out->hasVm = bullet.bulletState != PLAYER_BULLET_STATE_UNUSED;
    if (!out->hasVm) return;
    if (vm.currentInstruction && vm.beginingOfScript)
    {
        const auto current = reinterpret_cast<std::uintptr_t>(vm.currentInstruction);
        const auto begin = reinterpret_cast<std::uintptr_t>(vm.beginingOfScript);
        if (current >= begin && current - begin <= 1024 * 1024)
            out->vmInstructionOffset = static_cast<i32>(current - begin);
    }
    out->vmActiveSpriteIndex = vm.activeSpriteIndex; out->vmBaseSpriteIndex = vm.baseSpriteIndex;
    out->vmFlags = CaptureSpectatorAnmFlags(vm.flags);
    out->vmColor = vm.color; out->vmPrevColor = vm.prevColor;
    out->vmAlphaInitial = vm.alphaInterpInitial; out->vmAlphaFinal = vm.alphaInterpFinal;
    out->vmAlphaEnd = vm.alphaInterpEndTime; out->vmScaleEnd = vm.scaleInterpEndTime;
    out->vmAutoRotate = vm.autoRotate; out->vmPendingInterrupt = vm.pendingInterrupt;
    out->vmPosEnd = vm.posInterpEndTime; out->vmTimeOfLastSpriteSet = vm.timeOfLastSpriteSet;
    out->vmFontWidth = vm.fontWidth; out->vmFontHeight = vm.fontHeight;
    CaptureSpectatorVec3(&out->vmRotation, vm.rotation); CaptureSpectatorVec3(&out->vmPrevRotation, vm.prevRotation);
    CaptureSpectatorVec3(&out->vmAngleVel, vm.angleVel); CaptureSpectatorVec3(&out->vmPos, vm.pos);
    CaptureSpectatorVec3(&out->vmPrevPos, vm.prevPos);
    CaptureSpectatorVec3(&out->vmPosInterpInitial, vm.posInterpInitial);
    CaptureSpectatorVec3(&out->vmPosInterpFinal, vm.posInterpFinal); CaptureSpectatorVec3(&out->vmPosOffset, vm.posOffset);
    out->vmScaleX = vm.scaleX; out->vmScaleY = vm.scaleY; out->vmPrevScaleX = vm.prevScaleX; out->vmPrevScaleY = vm.prevScaleY;
    out->vmScaleFinalX = vm.scaleInterpFinalX; out->vmScaleFinalY = vm.scaleInterpFinalY;
    out->vmScaleInitialX = vm.scaleInterpInitialX; out->vmScaleInitialY = vm.scaleInterpInitialY;
    out->vmUvX = vm.uvScrollPos.x; out->vmUvY = vm.uvScrollPos.y;
    out->vmPrevUvX = vm.prevUvScrollPos.x; out->vmPrevUvY = vm.prevUvScrollPos.y;
    CaptureSpectatorTimer(&out->vmScriptTime, vm.currentTimeInScript);
    CaptureSpectatorTimer(&out->vmScaleTime, vm.scaleInterpTime);
    CaptureSpectatorTimer(&out->vmPosTime, vm.posInterpTime);
    CaptureSpectatorTimer(&out->vmAlphaTime, vm.alphaInterpTime);
}

bool ApplySpectatorBullet(const SpectatorPlayerBulletCheckpoint &source, PlayerBullet *out)
{
    out->position = ZunVec3(source.positionX, source.positionY, source.positionZ);
    out->prevPosition = ZunVec3(source.previousPositionX, source.previousPositionY, source.previousPositionZ);
    out->size = ZunVec3(source.sizeX, source.sizeY, source.sizeZ);
    out->velocity = ZunVec2(source.velocityX, source.velocityY);
    out->sidewaysMotion = source.sidewaysMotion;
    out->unk_134 = ZunVec3(source.unk134X, source.unk134Y, source.unk134Z);
    out->unk_140.previous = source.unk140Previous; out->unk_140.subFrame = source.unk140SubFrame;
    out->unk_140.current = source.unk140Current;
    out->damage = static_cast<i16>(source.damage); out->bulletState = static_cast<i16>(source.bulletState);
    out->bulletType = static_cast<i16>(source.bulletType); out->unk_152 = static_cast<i16>(source.unk152);
    out->spawnPositionIdx = static_cast<i16>(source.spawnPositionIdx);
    if (!source.hasVm) return true;
    AnmVm &vm = out->sprite;
    if (source.anmFileIndex < 0 || source.anmFileIndex >= 2048) return false;
    g_AnmManager->SetAndExecuteScriptIdx(&vm, source.anmFileIndex);
    if (source.vmActiveSpriteIndex >= 0)
    {
        if (source.vmActiveSpriteIndex >= 2048 ||
            g_AnmManager->SetActiveSprite(&vm, static_cast<u32>(source.vmActiveSpriteIndex)) != ZUN_SUCCESS)
            return false;
    }
    else
    {
        vm.activeSpriteIndex = -1;
        vm.sprite = nullptr;
    }
    vm.baseSpriteIndex = source.vmBaseSpriteIndex;
    vm.currentTimeInScript.previous = source.vmScriptTime.previous;
    vm.currentTimeInScript.current = source.vmScriptTime.current;
    vm.currentTimeInScript.subFrame = source.vmScriptTime.subFrame;
    vm.currentInstruction = source.vmInstructionOffset < 0 || !vm.beginingOfScript
        ? nullptr
        : reinterpret_cast<const AnmRawInstr *>(reinterpret_cast<std::uintptr_t>(vm.beginingOfScript) +
                                               static_cast<std::uintptr_t>(source.vmInstructionOffset));
    vm.rotation = ZunVec3(source.vmRotation.x, source.vmRotation.y, source.vmRotation.z);
    vm.prevRotation = ZunVec3(source.vmPrevRotation.x, source.vmPrevRotation.y, source.vmPrevRotation.z);
    vm.angleVel = ZunVec3(source.vmAngleVel.x, source.vmAngleVel.y, source.vmAngleVel.z);
    vm.pos = ZunVec3(source.vmPos.x, source.vmPos.y, source.vmPos.z);
    vm.prevPos = ZunVec3(source.vmPrevPos.x, source.vmPrevPos.y, source.vmPrevPos.z);
    vm.scaleX = source.vmScaleX; vm.scaleY = source.vmScaleY;
    vm.prevScaleX = source.vmPrevScaleX; vm.prevScaleY = source.vmPrevScaleY;
    vm.scaleInterpFinalX = source.vmScaleFinalX; vm.scaleInterpFinalY = source.vmScaleFinalY;
    vm.scaleInterpInitialX = source.vmScaleInitialX; vm.scaleInterpInitialY = source.vmScaleInitialY;
    vm.scaleInterpEndTime = source.vmScaleEnd;
    vm.scaleInterpTime.previous = source.vmScaleTime.previous; vm.scaleInterpTime.current = source.vmScaleTime.current;
    vm.scaleInterpTime.subFrame = source.vmScaleTime.subFrame;
    vm.uvScrollPos = ZunVec2(source.vmUvX, source.vmUvY);
    vm.prevUvScrollPos = ZunVec2(source.vmPrevUvX, source.vmPrevUvY);
    vm.color = source.vmColor; vm.prevColor = source.vmPrevColor;
    vm.alphaInterpInitial = source.vmAlphaInitial; vm.alphaInterpFinal = source.vmAlphaFinal;
    vm.alphaInterpEndTime = source.vmAlphaEnd;
    vm.alphaInterpTime.previous = source.vmAlphaTime.previous; vm.alphaInterpTime.current = source.vmAlphaTime.current;
    vm.alphaInterpTime.subFrame = source.vmAlphaTime.subFrame;
    vm.posInterpEndTime = source.vmPosEnd;
    vm.posInterpInitial = ZunVec3(source.vmPosInterpInitial.x, source.vmPosInterpInitial.y, source.vmPosInterpInitial.z);
    vm.posInterpFinal = ZunVec3(source.vmPosInterpFinal.x, source.vmPosInterpFinal.y, source.vmPosInterpFinal.z);
    vm.posOffset = ZunVec3(source.vmPosOffset.x, source.vmPosOffset.y, source.vmPosOffset.z);
    vm.posInterpTime.previous = source.vmPosTime.previous; vm.posInterpTime.current = source.vmPosTime.current;
    vm.posInterpTime.subFrame = source.vmPosTime.subFrame;
    vm.autoRotate = source.vmAutoRotate; vm.pendingInterrupt = source.vmPendingInterrupt;
    vm.timeOfLastSpriteSet = source.vmTimeOfLastSpriteSet; vm.fontWidth = source.vmFontWidth; vm.fontHeight = source.vmFontHeight;
    ApplySpectatorAnmFlags(source.vmFlags, &vm.flags);
    out->sprite.pos = ZunVec3(source.vmPos.x, source.vmPos.y, source.vmPos.z);
    return true;
}

bool WriteSpectatorPlayer(SpectatorScalars::Writer &writer, const SpectatorPlayerCheckpoint &p)
{
    if (!WriteSpectatorVec3(writer, p.unk44c) || !WriteSpectatorVec3(writer, p.hitboxTopLeft) ||
        !WriteSpectatorVec3(writer, p.hitboxBottomRight) || !WriteSpectatorVec3(writer, p.grabItemTopLeft) ||
        !WriteSpectatorVec3(writer, p.grabItemBottomRight) || !WriteSpectatorVec3(writer, p.hitboxSize) ||
        !WriteSpectatorVec3(writer, p.grabItemSize)) return false;
    for (const auto &v : p.orbsPosition) if (!WriteSpectatorVec3(writer, v)) return false;
    for (const auto &v : p.bombRegionPositions) if (!WriteSpectatorVec3(writer, v)) return false;
    for (const auto &v : p.bombRegionSizes) if (!WriteSpectatorVec3(writer, v)) return false;
    if (!writer.Array(p.bombRegionDamages) || !writer.Array(p.unk838) || !writer.Array(p.bombProjectiles)) return false;
    for (const auto &t : p.laserTimer) if (!WriteSpectatorTimer(writer, t)) return false;
    if (!writer.Values(p.horizontalMovementSpeedMultiplierDuringBomb, p.verticalMovementSpeedMultiplierDuringBomb,
                       p.initParam, p.lifeGiveTimer, p.lifeGiveTargetToken, p.powerGiveTaps, p.powerGiveWindow,
                       p.orbState, p.isFocus, p.unk9e4) || !WriteSpectatorTimer(writer, p.focusMovementTimer) ||
        !writer.Values(p.playerDirection, p.previousHorizontalSpeed, p.previousVerticalSpeed, p.previousFrameInput) ||
        !WriteSpectatorVec3(writer, p.positionOfLastEnemyHit) || !WriteSpectatorTimer(writer, p.fireBulletTimer) ||
        !WriteSpectatorTimer(writer, p.teamBombProtectionTimer) || !writer.Values(p.bombIsInUse, p.bombDuration) ||
        !WriteSpectatorTimer(writer, p.bombTimer) || !writer.Array(p.bombProjectileStates) ||
        !writer.Array(p.bombProjectileRelated)) return false;
    for (const auto &v : p.bombPositions) if (!WriteSpectatorVec3(writer, v)) return false;
    for (const auto &v : p.bombVelocities) if (!WriteSpectatorVec3(writer, v)) return false;
    return writer.Good();
}

bool ReadSpectatorPlayer(SpectatorScalars::Reader &reader, SpectatorPlayerCheckpoint *p)
{
    if (!ReadSpectatorVec3(reader, &p->unk44c) || !ReadSpectatorVec3(reader, &p->hitboxTopLeft) ||
        !ReadSpectatorVec3(reader, &p->hitboxBottomRight) || !ReadSpectatorVec3(reader, &p->grabItemTopLeft) ||
        !ReadSpectatorVec3(reader, &p->grabItemBottomRight) || !ReadSpectatorVec3(reader, &p->hitboxSize) ||
        !ReadSpectatorVec3(reader, &p->grabItemSize)) return false;
    for (auto &v : p->orbsPosition) if (!ReadSpectatorVec3(reader, &v)) return false;
    for (auto &v : p->bombRegionPositions) if (!ReadSpectatorVec3(reader, &v)) return false;
    for (auto &v : p->bombRegionSizes) if (!ReadSpectatorVec3(reader, &v)) return false;
    if (!reader.Array(p->bombRegionDamages) || !reader.Array(p->unk838) || !reader.Array(p->bombProjectiles)) return false;
    for (auto &t : p->laserTimer) if (!ReadSpectatorTimer(reader, &t)) return false;
    if (!reader.Values(p->horizontalMovementSpeedMultiplierDuringBomb, p->verticalMovementSpeedMultiplierDuringBomb,
                       p->initParam, p->lifeGiveTimer, p->lifeGiveTargetToken, p->powerGiveTaps, p->powerGiveWindow,
                       p->orbState, p->isFocus, p->unk9e4) || !ReadSpectatorTimer(reader, &p->focusMovementTimer) ||
        !reader.Values(p->playerDirection, p->previousHorizontalSpeed, p->previousVerticalSpeed, p->previousFrameInput) ||
        !ReadSpectatorVec3(reader, &p->positionOfLastEnemyHit) || !ReadSpectatorTimer(reader, &p->fireBulletTimer) ||
        !ReadSpectatorTimer(reader, &p->teamBombProtectionTimer) || !reader.Values(p->bombIsInUse, p->bombDuration) ||
        !ReadSpectatorTimer(reader, &p->bombTimer) || !reader.Array(p->bombProjectileStates) ||
        !reader.Array(p->bombProjectileRelated)) return false;
    for (auto &v : p->bombPositions) if (!ReadSpectatorVec3(reader, &v)) return false;
    for (auto &v : p->bombVelocities) if (!ReadSpectatorVec3(reader, &v)) return false;
    return true;
}

void CaptureSpectatorPlayer(const Player &source, SpectatorPlayerCheckpoint *out)
{
    CaptureSpectatorVec3(&out->unk44c, source.unk_44c);
    CaptureSpectatorVec3(&out->hitboxTopLeft, source.hitboxTopLeft);
    CaptureSpectatorVec3(&out->hitboxBottomRight, source.hitboxBottomRight);
    CaptureSpectatorVec3(&out->grabItemTopLeft, source.grabItemTopLeft);
    CaptureSpectatorVec3(&out->grabItemBottomRight, source.grabItemBottomRight);
    CaptureSpectatorVec3(&out->hitboxSize, source.hitboxSize);
    CaptureSpectatorVec3(&out->grabItemSize, source.grabItemSize);
    for (std::size_t i = 0; i < out->orbsPosition.size(); ++i) CaptureSpectatorVec3(&out->orbsPosition[i], source.orbsPosition[i]);
    for (std::size_t i = 0; i < out->bombRegionPositions.size(); ++i)
    {
        CaptureSpectatorVec3(&out->bombRegionPositions[i], source.bombRegionPositions[i]);
        CaptureSpectatorVec3(&out->bombRegionSizes[i], source.bombRegionSizes[i]);
        out->bombRegionDamages[i] = source.bombRegionDamages[i]; out->unk838[i] = source.unk_838[i];
    }
    for (std::size_t i = 0; i < std::size(source.bombProjectiles); ++i)
    {
        out->bombProjectiles[i * 4] = source.bombProjectiles[i].posX;
        out->bombProjectiles[i * 4 + 1] = source.bombProjectiles[i].posY;
        out->bombProjectiles[i * 4 + 2] = source.bombProjectiles[i].sizeX;
        out->bombProjectiles[i * 4 + 3] = source.bombProjectiles[i].sizeY;
    }
    for (std::size_t i = 0; i < out->laserTimer.size(); ++i) CaptureSpectatorTimer(&out->laserTimer[i], source.laserTimer[i]);
    out->horizontalMovementSpeedMultiplierDuringBomb = source.horizontalMovementSpeedMultiplierDuringBomb;
    out->verticalMovementSpeedMultiplierDuringBomb = source.verticalMovementSpeedMultiplierDuringBomb;
    out->initParam = source.initParam; out->lifeGiveTimer = source.lifeGiveTimer;
    out->lifeGiveTargetToken = source.lifeGiveTargetToken; out->powerGiveTaps = source.powerGiveTaps;
    out->powerGiveWindow = source.powerGiveWindow; out->orbState = source.orbState;
    out->isFocus = source.isFocus; out->unk9e4 = source.unk_9e4;
    CaptureSpectatorTimer(&out->focusMovementTimer, source.focusMovementTimer);
    out->playerDirection = source.playerDirection; out->previousHorizontalSpeed = source.previousHorizontalSpeed;
    out->previousVerticalSpeed = source.previousVerticalSpeed; out->previousFrameInput = source.previousFrameInput;
    CaptureSpectatorVec3(&out->positionOfLastEnemyHit, source.positionOfLastEnemyHit);
    CaptureSpectatorTimer(&out->fireBulletTimer, source.fireBulletTimer);
    CaptureSpectatorTimer(&out->teamBombProtectionTimer, source.teamBombProtectionTimer);
    out->bombIsInUse = source.bombInfo.isInUse; out->bombDuration = source.bombInfo.duration;
    CaptureSpectatorTimer(&out->bombTimer, source.bombInfo.timer);
    for (std::size_t i = 0; i < out->bombProjectileStates.size(); ++i)
    {
        out->bombProjectileStates[i] = source.bombInfo.reimuABombProjectilesState[i];
        out->bombProjectileRelated[i] = source.bombInfo.reimuABombProjectilesRelated[i];
        CaptureSpectatorVec3(&out->bombPositions[i], source.bombInfo.bombRegionPositions[i]);
        CaptureSpectatorVec3(&out->bombVelocities[i], source.bombInfo.bombRegionVelocities[i]);
    }
}

void ApplySpectatorPlayer(const SpectatorPlayerCheckpoint &source, Player *out)
{
    ApplySpectatorVec3(source.unk44c, &out->unk_44c);
    ApplySpectatorVec3(source.hitboxTopLeft, &out->hitboxTopLeft);
    ApplySpectatorVec3(source.hitboxBottomRight, &out->hitboxBottomRight);
    ApplySpectatorVec3(source.grabItemTopLeft, &out->grabItemTopLeft);
    ApplySpectatorVec3(source.grabItemBottomRight, &out->grabItemBottomRight);
    ApplySpectatorVec3(source.hitboxSize, &out->hitboxSize);
    ApplySpectatorVec3(source.grabItemSize, &out->grabItemSize);
    for (std::size_t i = 0; i < source.orbsPosition.size(); ++i) ApplySpectatorVec3(source.orbsPosition[i], &out->orbsPosition[i]);
    for (std::size_t i = 0; i < source.bombRegionPositions.size(); ++i)
    {
        ApplySpectatorVec3(source.bombRegionPositions[i], &out->bombRegionPositions[i]);
        ApplySpectatorVec3(source.bombRegionSizes[i], &out->bombRegionSizes[i]);
        out->bombRegionDamages[i] = source.bombRegionDamages[i]; out->unk_838[i] = source.unk838[i];
    }
    for (std::size_t i = 0; i < std::size(out->bombProjectiles); ++i)
    {
        out->bombProjectiles[i].posX = source.bombProjectiles[i * 4];
        out->bombProjectiles[i].posY = source.bombProjectiles[i * 4 + 1];
        out->bombProjectiles[i].sizeX = source.bombProjectiles[i * 4 + 2];
        out->bombProjectiles[i].sizeY = source.bombProjectiles[i * 4 + 3];
    }
    for (std::size_t i = 0; i < source.laserTimer.size(); ++i) ApplySpectatorTimer(source.laserTimer[i], &out->laserTimer[i]);
    out->horizontalMovementSpeedMultiplierDuringBomb = source.horizontalMovementSpeedMultiplierDuringBomb;
    out->verticalMovementSpeedMultiplierDuringBomb = source.verticalMovementSpeedMultiplierDuringBomb;
    out->initParam = source.initParam; out->lifeGiveTimer = source.lifeGiveTimer;
    out->lifeGiveTargetToken = source.lifeGiveTargetToken; out->powerGiveTaps = source.powerGiveTaps;
    out->powerGiveWindow = source.powerGiveWindow; out->orbState = source.orbState;
    out->isFocus = source.isFocus; out->unk_9e4 = source.unk9e4;
    ApplySpectatorTimer(source.focusMovementTimer, &out->focusMovementTimer);
    out->playerDirection = static_cast<PlayerDirection>(source.playerDirection);
    out->previousHorizontalSpeed = source.previousHorizontalSpeed; out->previousVerticalSpeed = source.previousVerticalSpeed;
    out->previousFrameInput = source.previousFrameInput;
    ApplySpectatorVec3(source.positionOfLastEnemyHit, &out->positionOfLastEnemyHit);
    ApplySpectatorTimer(source.fireBulletTimer, &out->fireBulletTimer);
    ApplySpectatorTimer(source.teamBombProtectionTimer, &out->teamBombProtectionTimer);
    out->bombInfo.isInUse = source.bombIsInUse; out->bombInfo.duration = source.bombDuration;
    ApplySpectatorTimer(source.bombTimer, &out->bombInfo.timer);
    for (std::size_t i = 0; i < source.bombProjectileStates.size(); ++i)
    {
        out->bombInfo.reimuABombProjectilesState[i] = source.bombProjectileStates[i];
        out->bombInfo.reimuABombProjectilesRelated[i] = source.bombProjectileRelated[i];
        ApplySpectatorVec3(source.bombPositions[i], &out->bombInfo.bombRegionPositions[i]);
        ApplySpectatorVec3(source.bombVelocities[i], &out->bombInfo.bombRegionVelocities[i]);
    }
}

struct SpectatorDiagnosticHasher
{
    std::uint64_t value = 1469598103934665603ull;

    template <typename T>
    void Scalar(const T &item)
    {
        const auto *bytes = reinterpret_cast<const std::uint8_t *>(&item);
        for (std::size_t i = 0; i < sizeof(item); ++i)
        {
            value ^= bytes[i];
            value *= 1099511628211ull;
        }
    }

    void Vec2(const ZunVec2 &item) { Scalar(item.x); Scalar(item.y); }
    void Vec3(const ZunVec3 &item) { Scalar(item.x); Scalar(item.y); Scalar(item.z); }
    void Timer(const ZunTimer &item) { Scalar(item.previous); Scalar(item.subFrame); Scalar(item.current); }
};

std::array<std::uint64_t, 5> SpectatorStageDiagnosticParts()
{
    std::array<SpectatorDiagnosticHasher, 5> h{};
    h[0].Timer(g_Stage.scriptTime); h[0].Scalar(g_Stage.instructionIndex);
    h[0].Timer(g_Stage.timer); h[0].Scalar(g_Stage.stage); h[0].Vec3(g_Stage.position);
    h[0].Scalar(g_Stage.skyFog.nearPlane); h[0].Scalar(g_Stage.skyFog.farPlane);
    h[0].Scalar(g_Stage.skyFogInterpInitial.nearPlane); h[0].Scalar(g_Stage.skyFogInterpInitial.farPlane);
    h[0].Scalar(g_Stage.skyFogInterpFinal.nearPlane); h[0].Scalar(g_Stage.skyFogInterpFinal.farPlane);
    h[0].Scalar(g_Stage.skyFogInterpDuration); h[0].Timer(g_Stage.skyFogInterpTimer);
    h[0].Scalar(g_Stage.skyFogNeedsSetup); h[0].Scalar(g_Stage.spellcardState);
    h[0].Scalar(g_Stage.ticksSinceSpellcardStarted); h[0].Scalar(g_Stage.unpauseFlag);

    h[1].Vec3(g_Stage.facingDirInterpInitial); h[1].Vec3(g_Stage.facingDirInterpFinal);
    h[1].Scalar(g_Stage.facingDirInterpDuration); h[1].Timer(g_Stage.facingDirInterpTimer);
    h[1].Vec3(g_Stage.positionInterpFinal); h[1].Scalar(g_Stage.positionInterpEndTime);
    h[1].Vec3(g_Stage.positionInterpInitial); h[1].Scalar(g_Stage.positionInterpStartTime);

    h[2].Scalar(g_Stage.quadCount); h[2].Scalar(g_Stage.objectsCount);
    for (i32 i = 0; i < g_Stage.objectsCount; ++i)
    {
        const std::uint8_t present = g_Stage.objects && g_Stage.objects[i] ? 1 : 0;
        h[2].Scalar(present);
        if (present)
            h[2].Scalar(g_Stage.objects[i]->flags);
    }

    h[3].Scalar(g_EnemyManager.randomItemSpawnIndex);
    h[3].Scalar(g_EnemyManager.randomItemTableIndex);
    const EclTimelineInstr *timeline = g_EnemyManager.timelineInstr;
    const std::uint8_t timelinePresent = timeline ? 1 : 0;
    h[3].Scalar(timelinePresent);
    if (timeline)
    {
        h[3].Scalar(timeline->time); h[3].Scalar(timeline->arg0);
        h[3].Scalar(timeline->opCode); h[3].Scalar(timeline->size);
    }
    h[3].Timer(g_EnemyManager.timelineTime);

    const EnemyEclInstr::RollbackState *ecl = EnemyEclInstr::GetRollbackState();
    if (ecl)
    {
        for (f32 value : ecl->starAngleTable) h[4].Scalar(value);
        h[4].Vec3(ecl->enemyPosVector); h[4].Vec3(ecl->playerPosVector);
    }
    std::array<std::uint64_t, 5> result{};
    for (std::size_t i = 0; i < result.size(); ++i) result[i] = h[i].value;
    return result;
}

std::array<std::uint64_t, 4> SpectatorPlayerDiagnosticParts(const Player &p)
{
    std::array<SpectatorDiagnosticHasher, 4> h{};
    h[0].Vec3(p.positionCenter); h[0].Vec3(p.unk_44c);
    h[0].Vec3(p.hitboxTopLeft); h[0].Vec3(p.hitboxBottomRight);
    h[0].Vec3(p.grabItemTopLeft); h[0].Vec3(p.grabItemBottomRight);
    h[0].Vec3(p.hitboxSize); h[0].Vec3(p.grabItemSize);
    for (const ZunVec3 &value : p.orbsPosition) h[0].Vec3(value);
    for (const ZunVec3 &value : p.bombRegionPositions) h[0].Vec3(value);
    for (const ZunVec3 &value : p.bombRegionSizes) h[0].Vec3(value);
    for (i32 value : p.bombRegionDamages) h[0].Scalar(value);
    for (i32 value : p.unk_838) h[0].Scalar(value);
    for (const PlayerRect &rect : p.bombProjectiles)
    {
        h[0].Scalar(rect.posX); h[0].Scalar(rect.posY);
        h[0].Scalar(rect.sizeX); h[0].Scalar(rect.sizeY);
    }
    for (const ZunTimer &timer : p.laserTimer) h[0].Timer(timer);
    h[0].Scalar(p.horizontalMovementSpeedMultiplierDuringBomb);
    h[0].Scalar(p.verticalMovementSpeedMultiplierDuringBomb);

    h[1].Scalar(p.respawnTimer); h[1].Scalar(p.bulletGracePeriod); h[1].Scalar(p.playerState);
    h[1].Scalar(p.initParam); h[1].Scalar(p.lifeGiveTimer); h[1].Scalar(p.lifeGiveTargetToken);
    h[1].Scalar(p.powerGiveTaps); h[1].Scalar(p.powerGiveWindow);
    h[1].Scalar(p.orbState); h[1].Scalar(p.isFocus); h[1].Scalar(p.unk_9e4);
    h[1].Timer(p.focusMovementTimer); h[1].Scalar(p.playerDirection);
    h[1].Scalar(p.previousHorizontalSpeed); h[1].Scalar(p.previousVerticalSpeed);
    h[1].Scalar(p.previousFrameInput); h[1].Vec3(p.positionOfLastEnemyHit);

    for (i32 i = 0; i < 80; ++i)
    {
        const PlayerBullet &b = p.bullets[i];
        if (b.bulletState == BULLET_STATE_UNUSED) continue;
        h[2].Scalar(i); h[2].Vec3(b.position); h[2].Vec3(b.size);
        h[2].Scalar(b.velocity.x); h[2].Scalar(b.velocity.y); h[2].Scalar(b.sidewaysMotion);
        h[2].Vec3(b.unk_134); h[2].Timer(b.unk_140); h[2].Scalar(b.damage);
        h[2].Scalar(b.bulletState); h[2].Scalar(b.bulletType); h[2].Scalar(b.unk_152);
        h[2].Scalar(b.spawnPositionIdx);
    }

    h[3].Timer(p.fireBulletTimer); h[3].Timer(p.invulnerabilityTimer);
    h[3].Timer(p.teamBombProtectionTimer); h[3].Scalar(p.bombInfo.isInUse);
    h[3].Scalar(p.bombInfo.duration); h[3].Timer(p.bombInfo.timer);
    if (p.bombInfo.isInUse)
    {
        for (i32 value : p.bombInfo.reimuABombProjectilesState) h[3].Scalar(value);
        for (f32 value : p.bombInfo.reimuABombProjectilesRelated) h[3].Scalar(value);
        for (const ZunVec3 &value : p.bombInfo.bombRegionPositions) h[3].Vec3(value);
        for (const ZunVec3 &value : p.bombInfo.bombRegionVelocities) h[3].Vec3(value);
    }
    std::array<std::uint64_t, 4> result{};
    for (std::size_t i = 0; i < result.size(); ++i) result[i] = h[i].value;
    return result;
}

void LogSpectatorNativeDiagnosticParts(const char *tag)
{
    const auto s = SpectatorStageDiagnosticParts();
    std::printf("spectator-debug native-parts tag=%s stage=%016llx/%016llx/%016llx/%016llx/%016llx clocks=%d,%d/%.3f,%d,%d/%.3f,%u script=%d objects=%d,%d timeline=%d,%d,%d,%d/%.3f/%d rnd=%u ecl=%g,%g,%g rng=%u/%u gameSeed=%u player0=",
                tag,
                static_cast<unsigned long long>(s[0]), static_cast<unsigned long long>(s[1]),
                static_cast<unsigned long long>(s[2]), static_cast<unsigned long long>(s[3]),
                static_cast<unsigned long long>(s[4]),
                g_Stage.scriptTime.previous, g_Stage.scriptTime.current, g_Stage.scriptTime.subFrame,
                g_Stage.timer.previous, g_Stage.timer.current, g_Stage.timer.subFrame,
                static_cast<unsigned>(g_Stage.stage), g_Stage.instructionIndex,
                g_Stage.quadCount, g_Stage.objectsCount,
                g_EnemyManager.timelineInstr ? g_EnemyManager.timelineInstr->time : -1,
                g_EnemyManager.timelineInstr ? g_EnemyManager.timelineInstr->arg0 : -1,
                g_EnemyManager.timelineInstr ? g_EnemyManager.timelineInstr->opCode : -1,
                g_EnemyManager.timelineInstr ? g_EnemyManager.timelineInstr->size : -1,
                g_EnemyManager.timelineTime.subFrame, g_EnemyManager.timelineTime.current,
                static_cast<unsigned>(g_EnemyManager.randomItemSpawnIndex),
                EnemyEclInstr::GetRollbackState()->starAngleTable[0],
                EnemyEclInstr::GetRollbackState()->enemyPosVector.x,
                EnemyEclInstr::GetRollbackState()->playerPosVector.x,
                static_cast<unsigned>(g_Rng.seed),
                static_cast<unsigned>(g_Rng.generationCount),
                static_cast<unsigned>(g_GameManager.randomSeed));
    for (u8 slot = 0; slot < TH06_MULTI_MAX_PLAYERS; ++slot)
    {
        const auto p = SpectatorPlayerDiagnosticParts(g_Players[slot]);
        std::printf("P%u active=%d=%016llx/%016llx/%016llx/%016llx pos=%.2f,%.2f,%.2f prevHit=%.2f,%.2f,%.2f bullets=%d bomb=%d invuln=%d/%d/%.3f team=%d/%d/%.3f ",
                    static_cast<unsigned>(slot),
                    g_PlayerActive[slot] ? 1 : 0,
                    static_cast<unsigned long long>(p[0]), static_cast<unsigned long long>(p[1]),
                    static_cast<unsigned long long>(p[2]), static_cast<unsigned long long>(p[3]),
                    g_Players[slot].positionCenter.x, g_Players[slot].positionCenter.y, g_Players[slot].positionCenter.z,
                    g_Players[slot].positionOfLastEnemyHit.x, g_Players[slot].positionOfLastEnemyHit.y,
                    g_Players[slot].positionOfLastEnemyHit.z,
                    static_cast<int>(std::count_if(std::begin(g_Players[slot].bullets), std::end(g_Players[slot].bullets),
                                                   [](const PlayerBullet &b) { return b.bulletState != BULLET_STATE_UNUSED; })),
                    g_Players[slot].bombInfo.isInUse,
                    g_Players[slot].invulnerabilityTimer.previous, g_Players[slot].invulnerabilityTimer.current,
                    g_Players[slot].invulnerabilityTimer.subFrame,
                    g_Players[slot].teamBombProtectionTimer.previous, g_Players[slot].teamBombProtectionTimer.current,
                    g_Players[slot].teamBombProtectionTimer.subFrame);
    }
    std::printf("\n");
}

void PublishCheckpointDiagnostic(std::uint32_t frame, std::uint8_t stage,
                                 std::uint64_t hash, std::uint32_t confirmed,
                                 std::uint8_t predictedMask, bool encodedCapture,
                                 bool verifiedRestore = false)
{
#ifdef __EMSCRIPTEN__
    if (!ProductionLanMode() || (!RollbackAuditEnabled() && !verifiedRestore))
        return;
    char hashText[17];
    std::snprintf(hashText, sizeof(hashText), "%016llx", static_cast<unsigned long long>(hash));
    EM_ASM({
        const hash = UTF8ToString($1);
        globalThis.__eaglerNetplayLanCheckpointFrame = $0 >>> 0;
        globalThis.__eaglerNetplayLanCheckpointHash = hash;
        globalThis.__eaglerNetplayLanCheckpointStage = $2 >>> 0;
        globalThis.__eaglerNetplayLanCheckpointConfirmed = $3 >>> 0;
        globalThis.__eaglerNetplayLanCheckpointPredictedMask = $4 >>> 0;
        if ($5) {
            globalThis.__eaglerNetplayLanCheckpointCaptureFrame = $0 >>> 0;
            globalThis.__eaglerNetplayLanCheckpointCaptureHash = hash;
            globalThis.__eaglerNetplayLanCheckpointCaptureConfirmed = $3 >>> 0;
            globalThis.__eaglerNetplayLanCheckpointCapturePredictedMask = $4 >>> 0;
        }
    }, frame, hashText, static_cast<unsigned>(stage), confirmed,
       static_cast<unsigned>(predictedMask), encodedCapture ? 1 : 0);
#else
    (void)frame;
    (void)stage;
    (void)hash;
    (void)confirmed;
    (void)predictedMask;
    (void)encodedCapture;
    (void)verifiedRestore;
#endif
}

bool EncodeSpectatorCheckpoint(std::vector<std::uint8_t> *bytes,
                               std::uint8_t predictedMask)
{
    if (!bytes || !MultiplayerGameplay::IsMultiplayer() || !g_GameManager.currentStage)
        return false;
    SpectatorCheckpoint cp;
    cp.stage = g_GameManager.currentStage;
    cp.difficulty = static_cast<i32>(g_GameManager.difficulty);
    cp.rank = g_GameManager.rank; cp.maxRank = g_GameManager.maxRank;
    cp.minRank = g_GameManager.minRank; cp.subRank = g_GameManager.subRank;
    cp.initialSeed = g_GameplayStartSeed; cp.stageSeed = g_GameManager.randomSeed; cp.rngSeed = g_Rng.seed;
    cp.randomItemSpawnIndex = g_EnemyManager.randomItemSpawnIndex;
    cp.randomItemTableIndex = g_EnemyManager.randomItemTableIndex;
    cp.rngGeneration = g_Rng.generationCount;
    const auto checkpointHash = Th06CanonicalHash::Capture();
    cp.expectedHash = checkpointHash.composite;
    cp.expectedPlayer0 = checkpointHash.player0;
    cp.expectedPlayer1 = checkpointHash.player1;
    cp.expectedPlayer2 = checkpointHash.player2;
    cp.gameFrames = g_GameManager.gameFrames;
    for (u8 slot = 0; slot < TH06_MULTI_MAX_PLAYERS; ++slot)
    {
        const Player &player = g_Players[slot];
        CaptureSpectatorPlayer(player, &cp.players[slot]);
        cp.playerPositionCenter[slot * 3] = player.positionCenter.x;
        cp.playerPositionCenter[slot * 3 + 1] = player.positionCenter.y;
        cp.playerPositionCenter[slot * 3 + 2] = player.positionCenter.z;
        cp.playerRespawnTimer[slot] = player.respawnTimer;
        cp.playerBulletGracePeriod[slot] = player.bulletGracePeriod;
        cp.playerState[slot] = player.playerState;
        cp.playerInvulnerabilityTimer[slot * 2] = player.invulnerabilityTimer.previous;
        cp.playerInvulnerabilityTimer[slot * 2 + 1] = player.invulnerabilityTimer.current;
        cp.playerInvulnerabilitySubFrame[slot] = player.invulnerabilityTimer.subFrame;
        for (u8 index = 0; index < std::size(player.bullets); ++index)
        {
            const PlayerBullet &bullet = player.bullets[index];
            SpectatorPlayerBulletCheckpoint saved;
            CaptureSpectatorBullet(bullet, slot, index, &saved);
            const bool usedSlot = saved.bulletState != PLAYER_BULLET_STATE_UNUSED ||
                saved.positionX != 0.0f || saved.positionY != 0.0f || saved.positionZ != 0.0f ||
                saved.previousPositionX != 0.0f || saved.previousPositionY != 0.0f ||
                saved.previousPositionZ != 0.0f || saved.sizeX != 0.0f || saved.sizeY != 0.0f ||
                saved.sizeZ != 0.0f || saved.velocityX != 0.0f || saved.velocityY != 0.0f ||
                saved.sidewaysMotion != 0.0f || saved.unk134X != 0.0f || saved.unk134Y != 0.0f ||
                saved.unk134Z != 0.0f || saved.unk140Previous != 0 || saved.unk140Current != 0 ||
                saved.unk140SubFrame != 0.0f || saved.damage != 0 || saved.bulletType != 0 ||
                saved.unk152 != 0 || saved.spawnPositionIdx != 0;
            if (usedSlot) cp.playerBullets.push_back(saved);
        }
    }
    std::printf("spectator-debug checkpoint frame=%u composite=%016llx meta=%016llx game=%016llx stage=%016llx player0=%016llx player1=%016llx player2=%016llx enemies=%016llx bullets=%016llx items=%016llx counts=%u,%u,%u,%u stageSeed=%u rngSeed=%u rngGen=%u gameFrames=%u initialSeed=%u cpP0=(%.2f,%.2f,%.2f)/%d/%d/%d/%d/%.3f cpP1=(%.2f,%.2f,%.2f)/%d/%d/%d/%d/%.3f\n",
                g_SimFrame, static_cast<unsigned long long>(checkpointHash.composite),
                static_cast<unsigned long long>(checkpointHash.meta), static_cast<unsigned long long>(checkpointHash.metaGame),
                static_cast<unsigned long long>(checkpointHash.stage), static_cast<unsigned long long>(checkpointHash.player0),
                static_cast<unsigned long long>(checkpointHash.player1), static_cast<unsigned long long>(checkpointHash.player2),
                static_cast<unsigned long long>(checkpointHash.enemies), static_cast<unsigned long long>(checkpointHash.bullets),
                static_cast<unsigned long long>(checkpointHash.items), checkpointHash.enemyCount, checkpointHash.bulletCount,
                checkpointHash.laserCount, checkpointHash.itemCount, static_cast<unsigned>(cp.stageSeed),
                static_cast<unsigned>(cp.rngSeed), static_cast<unsigned>(cp.rngGeneration),
                static_cast<unsigned>(cp.gameFrames), static_cast<unsigned>(cp.initialSeed),
                cp.playerPositionCenter[0], cp.playerPositionCenter[1], cp.playerPositionCenter[2],
                cp.playerRespawnTimer[0], cp.playerBulletGracePeriod[0], cp.playerState[0],
                cp.playerInvulnerabilityTimer[1], cp.playerInvulnerabilitySubFrame[0],
                cp.playerPositionCenter[3], cp.playerPositionCenter[4], cp.playerPositionCenter[5],
                cp.playerRespawnTimer[1], cp.playerBulletGracePeriod[1], cp.playerState[1],
                cp.playerInvulnerabilityTimer[3], cp.playerInvulnerabilitySubFrame[1]);
    LogSpectatorNativeDiagnosticParts("checkpoint");
    cp.guiScore = g_GameManager.guiScore; cp.score = g_GameManager.score;
    cp.nextScoreIncrement = g_GameManager.nextScoreIncrement; cp.highScore = g_GameManager.highScore;
    cp.grazeInStage = g_GameManager.grazeInStage; cp.grazeTotal = g_GameManager.grazeInTotal;
    cp.deaths = g_GameManager.deaths; cp.bombsUsed = g_GameManager.bombsUsed;
    cp.spellcardsCaptured = g_GameManager.spellcardsCaptured; cp.demoFrames = g_GameManager.demoFrames;
    cp.menuCursorBackup = static_cast<i32>(g_GameManager.menuCursorBackup);
    cp.currentPower = g_GameManager.currentPower;
    cp.pointItemsInStage = g_GameManager.pointItemsCollectedInStage;
    cp.pointItems = g_GameManager.pointItemsCollected;
    cp.numRetries = g_GameManager.numRetries; cp.powerItemCountForScore = g_GameManager.powerItemCountForScore;
    cp.livesRemaining = g_GameManager.livesRemaining; cp.bombsRemaining = g_GameManager.bombsRemaining;
    cp.extraLives = g_GameManager.extraLives; cp.character = g_GameManager.character;
    cp.shotType = g_GameManager.shotType; cp.isTimeStopped = g_GameManager.isTimeStopped;
    cp.isInReplay = g_GameManager.isInReplay; cp.isInGameMenu = g_GameManager.isInGameMenu;
    cp.isInRetryMenu = g_GameManager.isInRetryMenu; cp.isInMenu = g_GameManager.isInMenu;
    cp.isGameCompleted = g_GameManager.isGameCompleted; cp.isInPracticeMode = g_GameManager.isInPracticeMode;
    cp.demoMode = g_GameManager.demoMode;
    cp.arcadeTop = g_GameManager.arcadeRegionTopLeftPos; cp.arcadeSize = g_GameManager.arcadeRegionSize;
    cp.prevArcadeTop = g_GameManager.prevArcadeRegionTopLeftPos;
    cp.prevArcadeSize = g_GameManager.prevArcadeRegionSize;
    cp.movementTop = g_GameManager.playerMovementAreaTopLeftPos;
    cp.movementSize = g_GameManager.playerMovementAreaSize;
    cp.cameraDistance = g_GameManager.cameraDistance; cp.cameraFacing = g_GameManager.stageCameraFacingDir;
    cp.curInput = g_CurFrameInput; cp.lastInput = g_LastFrameInput;
    cp.eighthHeld = g_IsEigthFrameOfHeldInput; cp.heldFrames = g_NumOfFramesInputsWereHeld;
    for (u8 slot = 0; slot < TH06_MULTI_MAX_PLAYERS; ++slot)
    {
        cp.curPlayerInput[slot] = g_CurFrameGameInputs[slot];
        cp.lastPlayerInput[slot] = g_LastFrameGameInputs[slot];
        cp.lives[slot] = GetPlayerLives(slot); cp.bombs[slot] = GetPlayerBombs(slot);
        cp.powers[slot] = GetPlayerPower(slot);
        cp.kills[slot] = g_MultiplayerContributionStats[slot].enemiesDefeated;
        cp.damage[slot] = g_MultiplayerContributionStats[slot].damageDealt;
        cp.challengeDeaths[slot] = g_MultiplayerContributionStats[slot].challengeDeaths;
    }
    cp.session = MultiplayerGameplay::GetState();
    cp.teamWipeRetryFrames = g_teamWipeRetryFrames;
    cp.supervisorWanted = g_Supervisor.wantedState; cp.supervisorCurrent = g_Supervisor.curState;
    cp.supervisorWanted2 = g_Supervisor.wantedState2; cp.supervisorEnding = g_Supervisor.isInEnding;
    cp.supervisorFramerate = g_Supervisor.effectiveFramerateMultiplier;

    SpectatorScalars::Writer writer(256u * 1024u);
    writer.Values(cp.magic, cp.stage, cp.difficulty, cp.rank, cp.maxRank, cp.minRank, cp.subRank,
                  cp.initialSeed, cp.stageSeed, cp.rngSeed, cp.currentPower, cp.pointItemsInStage, cp.pointItems,
                  cp.randomItemSpawnIndex, cp.randomItemTableIndex,
                  cp.rngGeneration, cp.expectedHash, cp.expectedPlayer0, cp.expectedPlayer1, cp.expectedPlayer2,
                  cp.guiScore, cp.score, cp.nextScoreIncrement,
                  cp.highScore, cp.gameFrames, cp.grazeInStage, cp.grazeTotal, cp.deaths,
                  cp.bombsUsed, cp.spellcardsCaptured, cp.demoFrames, cp.menuCursorBackup,
                  cp.numRetries, cp.powerItemCountForScore, cp.livesRemaining, cp.bombsRemaining,
                  cp.extraLives, cp.character, cp.shotType, cp.isTimeStopped, cp.isInReplay,
                  cp.isInGameMenu, cp.isInRetryMenu, cp.isInMenu, cp.isGameCompleted,
                  cp.isInPracticeMode, cp.demoMode,
                  cp.arcadeTop.x, cp.arcadeTop.y, cp.arcadeSize.x, cp.arcadeSize.y,
                  cp.prevArcadeTop.x, cp.prevArcadeTop.y, cp.prevArcadeSize.x, cp.prevArcadeSize.y,
                  cp.movementTop.x, cp.movementTop.y, cp.movementSize.x, cp.movementSize.y,
                  cp.cameraDistance, cp.cameraFacing.x, cp.cameraFacing.y, cp.cameraFacing.z,
                  cp.curInput, cp.lastInput, cp.eighthHeld, cp.heldFrames,
                  cp.session.playerCount, cp.session.localPlayer, cp.session.showStagePlayerNames,
                  cp.session.challengeMode, cp.session.showContributionStats,
                  cp.teamWipeRetryFrames, cp.supervisorWanted, cp.supervisorCurrent,
                  cp.supervisorWanted2, cp.supervisorEnding, cp.supervisorFramerate);
    for (const auto &slot : cp.session.players)
        writer.Values(slot.active, slot.temporarilyAbsent, slot.permanentlyDeparted,
                      slot.character, slot.shot);
    writer.Array(cp.curPlayerInput); writer.Array(cp.lastPlayerInput);
    writer.Array(cp.playerPositionCenter); writer.Array(cp.playerRespawnTimer);
    writer.Array(cp.playerBulletGracePeriod); writer.Array(cp.playerState);
    writer.Array(cp.playerInvulnerabilityTimer); writer.Array(cp.playerInvulnerabilitySubFrame);
    writer.Array(cp.lives); writer.Array(cp.bombs); writer.Array(cp.powers);
    writer.Array(cp.kills); writer.Array(cp.damage); writer.Array(cp.challengeDeaths);
    for (const auto &player : cp.players)
        if (!WriteSpectatorPlayer(writer, player)) return false;
    if (cp.playerBullets.size() > TH06_MULTI_MAX_PLAYERS * 80u ||
        !writer.Value(static_cast<std::uint16_t>(cp.playerBullets.size())))
        return false;
    for (const auto &bullet : cp.playerBullets)
        if (!WriteSpectatorBullet(writer, bullet)) return false;
    if (!writer.Good() || !CompressSpectatorCheckpoint(writer.Bytes(), bytes))
        return false;
    PublishCheckpointDiagnostic(g_SimFrame, static_cast<std::uint8_t>(cp.stage),
                                checkpointHash.composite,
                                g_Core.ConfirmedThroughAllRemotes(), predictedMask, true);
    std::printf("spectator-debug checkpoint capture diagnostic frame=%u stage=%u hash=%016llx confirmed=%u predicted=0x%x\n",
                g_SimFrame, static_cast<unsigned>(cp.stage),
                static_cast<unsigned long long>(checkpointHash.composite),
                g_Core.ConfirmedThroughAllRemotes(), static_cast<unsigned>(predictedMask));
    return true;
}

bool DecodeSpectatorCheckpoint(const std::vector<std::uint8_t> &bytes, SpectatorCheckpoint *cp)
{
    if (!cp) return false;
    std::vector<std::uint8_t> raw;
    if (!DecompressSpectatorCheckpoint(bytes, &raw)) return false;
    SpectatorScalars::Reader reader(raw);
    reader.Values(cp->magic, cp->stage, cp->difficulty, cp->rank, cp->maxRank, cp->minRank, cp->subRank,
                  cp->initialSeed, cp->stageSeed, cp->rngSeed, cp->currentPower, cp->pointItemsInStage, cp->pointItems,
                  cp->randomItemSpawnIndex, cp->randomItemTableIndex,
                  cp->rngGeneration, cp->expectedHash, cp->expectedPlayer0, cp->expectedPlayer1, cp->expectedPlayer2,
                  cp->guiScore, cp->score, cp->nextScoreIncrement,
                  cp->highScore, cp->gameFrames, cp->grazeInStage, cp->grazeTotal, cp->deaths,
                  cp->bombsUsed, cp->spellcardsCaptured, cp->demoFrames, cp->menuCursorBackup,
                  cp->numRetries, cp->powerItemCountForScore, cp->livesRemaining, cp->bombsRemaining,
                  cp->extraLives, cp->character, cp->shotType, cp->isTimeStopped, cp->isInReplay,
                  cp->isInGameMenu, cp->isInRetryMenu, cp->isInMenu, cp->isGameCompleted,
                  cp->isInPracticeMode, cp->demoMode,
                  cp->arcadeTop.x, cp->arcadeTop.y, cp->arcadeSize.x, cp->arcadeSize.y,
                  cp->prevArcadeTop.x, cp->prevArcadeTop.y, cp->prevArcadeSize.x, cp->prevArcadeSize.y,
                  cp->movementTop.x, cp->movementTop.y, cp->movementSize.x, cp->movementSize.y,
                  cp->cameraDistance, cp->cameraFacing.x, cp->cameraFacing.y, cp->cameraFacing.z,
                  cp->curInput, cp->lastInput, cp->eighthHeld, cp->heldFrames,
                  cp->session.playerCount, cp->session.localPlayer, cp->session.showStagePlayerNames,
                  cp->session.challengeMode, cp->session.showContributionStats,
                  cp->teamWipeRetryFrames, cp->supervisorWanted, cp->supervisorCurrent,
                  cp->supervisorWanted2, cp->supervisorEnding, cp->supervisorFramerate);
    for (auto &slot : cp->session.players)
        reader.Values(slot.active, slot.temporarilyAbsent, slot.permanentlyDeparted,
                      slot.character, slot.shot);
    reader.Array(cp->curPlayerInput); reader.Array(cp->lastPlayerInput);
    reader.Array(cp->playerPositionCenter); reader.Array(cp->playerRespawnTimer);
    reader.Array(cp->playerBulletGracePeriod); reader.Array(cp->playerState);
    reader.Array(cp->playerInvulnerabilityTimer); reader.Array(cp->playerInvulnerabilitySubFrame);
    reader.Array(cp->lives); reader.Array(cp->bombs); reader.Array(cp->powers);
    reader.Array(cp->kills); reader.Array(cp->damage); reader.Array(cp->challengeDeaths);
    for (auto &player : cp->players)
        if (!ReadSpectatorPlayer(reader, &player)) return false;
    std::uint16_t bulletCount = 0;
    if (!reader.Value(bulletCount) || bulletCount > TH06_MULTI_MAX_PLAYERS * 80u)
        return false;
    cp->playerBullets.resize(bulletCount);
    for (auto &bullet : cp->playerBullets)
        if (!ReadSpectatorBullet(reader, &bullet)) return false;
    std::array<std::array<bool, 80>, TH06_MULTI_MAX_PLAYERS> seenBullets{};
    bool validBullets = true;
    for (const auto &bullet : cp->playerBullets)
    {
        if (bullet.player >= TH06_MULTI_MAX_PLAYERS || bullet.slot >= 80 ||
            seenBullets[bullet.player][bullet.slot])
        {
            validBullets = false;
            break;
        }
        seenBullets[bullet.player][bullet.slot] = true;
    }
    for (const auto &bullet : cp->playerBullets)
        if (bullet.hasVm && (bullet.anmFileIndex < 0 || bullet.anmFileIndex >= 2048 ||
                             bullet.vmInstructionOffset < -1 || bullet.vmInstructionOffset > 1024 * 1024 ||
                             bullet.vmActiveSpriteIndex < -1 || bullet.vmActiveSpriteIndex >= 2048))
            validBullets = false;
    return reader.Done() && validBullets && cp->magic == 0x36504353 && cp->stage >= 1 && cp->stage <= 7 &&
           cp->session.playerCount >= 2 && cp->session.playerCount <= TH06_MULTI_MAX_PLAYERS &&
           cp->session.players[0].active && cp->character <= 1 && cp->shotType <= 1;
}

bool ApplySpectatorCheckpointState(const SpectatorCheckpoint &cp)
{
    MultiplayerGameplay::Configure(cp.session);
    g_GameManager.difficulty = static_cast<Difficulty>(cp.difficulty);
    g_GameManager.character = cp.character; g_GameManager.shotType = cp.shotType;
    g_GameManager.guiScore = cp.guiScore; g_GameManager.score = cp.score;
    g_GameManager.nextScoreIncrement = cp.nextScoreIncrement; g_GameManager.highScore = cp.highScore;
    g_GameManager.grazeInStage = cp.grazeInStage; g_GameManager.grazeInTotal = cp.grazeTotal;
    g_GameManager.deaths = cp.deaths; g_GameManager.bombsUsed = cp.bombsUsed;
    g_GameManager.spellcardsCaptured = cp.spellcardsCaptured; g_GameManager.isTimeStopped = cp.isTimeStopped;
    g_GameManager.currentPower = cp.currentPower; g_GameManager.pointItemsCollectedInStage = cp.pointItemsInStage;
    g_GameManager.pointItemsCollected = cp.pointItems; g_GameManager.numRetries = cp.numRetries;
    g_GameManager.powerItemCountForScore = cp.powerItemCountForScore;
    g_GameManager.livesRemaining = cp.livesRemaining; g_GameManager.bombsRemaining = cp.bombsRemaining;
    g_GameManager.extraLives = cp.extraLives; g_GameManager.isInReplay = cp.isInReplay;
    g_GameManager.isInGameMenu = cp.isInGameMenu; g_GameManager.isInRetryMenu = cp.isInRetryMenu;
    g_GameManager.isInMenu = cp.isInMenu; g_GameManager.isGameCompleted = cp.isGameCompleted;
    g_GameManager.isInPracticeMode = cp.isInPracticeMode; g_GameManager.demoMode = cp.demoMode;
    g_GameManager.demoFrames = cp.demoFrames; g_GameManager.gameFrames = cp.gameFrames;
    g_GameManager.currentStage = cp.stage; g_GameManager.menuCursorBackup = static_cast<u32>(cp.menuCursorBackup);
    g_GameManager.randomSeed = cp.stageSeed; g_GameManager.arcadeRegionTopLeftPos = cp.arcadeTop;
    g_GameManager.arcadeRegionSize = cp.arcadeSize; g_GameManager.prevArcadeRegionTopLeftPos = cp.prevArcadeTop;
    g_GameManager.prevArcadeRegionSize = cp.prevArcadeSize;
    g_GameManager.playerMovementAreaTopLeftPos = cp.movementTop;
    g_GameManager.playerMovementAreaSize = cp.movementSize;
    g_GameManager.cameraDistance = cp.cameraDistance; g_GameManager.stageCameraFacingDir = cp.cameraFacing;
    g_GameManager.rank = cp.rank; g_GameManager.maxRank = cp.maxRank;
    g_GameManager.minRank = cp.minRank; g_GameManager.subRank = cp.subRank;
    for (u8 slot = 0; slot < TH06_MULTI_MAX_PLAYERS; ++slot)
    {
        SetPlayerLives(slot, cp.lives[slot]); SetPlayerBombs(slot, cp.bombs[slot]);
        SetPlayerPower(slot, cp.powers[slot]);
        g_MultiplayerContributionStats[slot].enemiesDefeated = cp.kills[slot];
        g_MultiplayerContributionStats[slot].damageDealt = cp.damage[slot];
        g_MultiplayerContributionStats[slot].challengeDeaths = cp.challengeDeaths[slot];
        Player &player = g_Players[slot];
        player.positionCenter.x = cp.playerPositionCenter[slot * 3];
        player.positionCenter.y = cp.playerPositionCenter[slot * 3 + 1];
        player.positionCenter.z = cp.playerPositionCenter[slot * 3 + 2];
        player.respawnTimer = cp.playerRespawnTimer[slot];
        player.bulletGracePeriod = cp.playerBulletGracePeriod[slot];
        player.playerState = static_cast<i8>(cp.playerState[slot]);
        player.invulnerabilityTimer.previous = cp.playerInvulnerabilityTimer[slot * 2];
        player.invulnerabilityTimer.current = cp.playerInvulnerabilityTimer[slot * 2 + 1];
        player.invulnerabilityTimer.subFrame = cp.playerInvulnerabilitySubFrame[slot];
        ApplySpectatorPlayer(cp.players[slot], &player);
        g_CurFrameGameInputs[slot] = cp.curPlayerInput[slot];
        g_LastFrameGameInputs[slot] = cp.lastPlayerInput[slot];
    }
    g_teamWipeRetryFrames = cp.teamWipeRetryFrames;
    g_CurFrameInput = cp.curInput; g_LastFrameInput = cp.lastInput;
    g_IsEigthFrameOfHeldInput = cp.eighthHeld; g_NumOfFramesInputsWereHeld = cp.heldFrames;
    g_Supervisor.wantedState = cp.supervisorWanted; g_Supervisor.curState = cp.supervisorCurrent;
    g_Supervisor.wantedState2 = cp.supervisorWanted2; g_Supervisor.isInEnding = cp.supervisorEnding;
    g_Supervisor.effectiveFramerateMultiplier = cp.supervisorFramerate;
    g_EnemyManager.randomItemSpawnIndex = cp.randomItemSpawnIndex;
    g_EnemyManager.randomItemTableIndex = cp.randomItemTableIndex;
    for (const auto &saved : cp.playerBullets)
    {
        PlayerBullet &bullet = g_Players[saved.player].bullets[saved.slot];
        if (!ApplySpectatorBullet(saved, &bullet)) return false;
    }
    // Script assignment may execute randomized ANM instructions. Restore the
    // frame's RNG only after all native VM reconstruction has completed.
    g_Rng.seed = cp.rngSeed;
    g_Rng.generationCount = cp.rngGeneration;
    return true;
}

bool RestoreSpectatorCheckpoint(const SpectatorStageEvent &event)
{
    std::printf("netplay spectator TH06: restore begin frame=%u bytes=%u nativeReady=%d\n",
                event.frame, static_cast<unsigned>(event.state.size()),
                g_SpectatorNativeStageReady ? 1 : 0);
    SpectatorCheckpoint cp;
    if (event.game != '6' || event.schema != SPECTATOR_CHECKPOINT_SCHEMA ||
        !DecodeSpectatorCheckpoint(event.state, &cp) || event.stage != static_cast<std::uint32_t>(cp.stage))
        return false;

    // Rebuild through the actual GameManager -> Stage -> Player -> ECL chain.
    // This is the native game's constructor, seeded and configured by the
    // versioned scalar initializer, rather than an object-memory restore.
    SideEffects::SetSpeculative(true);
    if (g_SpectatorNativeStageReady)
        GameManager::CutChain();
    else if (g_MainMenu.chainCalc)
    {
        g_Chain.Cut(g_MainMenu.chainCalc);
        g_MainMenu.chainCalc = nullptr;
        if (g_MainMenu.chainDraw)
        {
            g_Chain.Cut(g_MainMenu.chainDraw);
            g_MainMenu.chainDraw = nullptr;
        }
    }
    std::printf("netplay spectator TH06: restore chain cut\n");

    if (!MultiplayerGameplay::Configure(cp.session))
    {
        SideEffects::SetSpeculative(false);
        return false;
    }
    std::printf("netplay spectator TH06: restore session configured stage=%d seed=%u\n",
                cp.stage, static_cast<unsigned>(cp.stageSeed));
    // Resources are read by PrepareMultiplayerStageRevival before Player's
    // native constructor clears its prior slot. Seed all seats first.
    for (u8 slot = 0; slot < TH06_MULTI_MAX_PLAYERS; ++slot)
    {
        SetPlayerLives(slot, cp.lives[slot]);
        SetPlayerBombs(slot, cp.bombs[slot]);
        SetPlayerPower(slot, cp.powers[slot]);
        g_MultiplayerContributionStats[slot].enemiesDefeated = cp.kills[slot];
        g_MultiplayerContributionStats[slot].damageDealt = cp.damage[slot];
        g_MultiplayerContributionStats[slot].challengeDeaths = cp.challengeDeaths[slot];
    }
    g_teamWipeRetryFrames = cp.teamWipeRetryFrames;
    g_GameManager.difficulty = static_cast<Difficulty>(cp.difficulty);
    g_GameManager.character = cp.character;
    g_GameManager.shotType = cp.shotType;
    g_GameManager.isInReplay = 0;
    g_GameManager.isInPracticeMode = cp.isInPracticeMode;
    g_GameManager.isInMenu = 0;
    g_GameManager.isInGameMenu = 0;
    g_GameManager.isInRetryMenu = 0;
    g_GameManager.isGameCompleted = 0;
    g_GameManager.currentStage = cp.stage - 1;
    g_GameManager.randomSeed = cp.stageSeed;
    g_Rng.Initialize(cp.initialSeed);
    // A late viewer may have no previous native GameManager chain. REINIT
    // deliberately reuses stage-owned GUI allocations and ANM banks, so the
    // first reconstruction must use the normal fresh-stage initialization
    // path. ApplySpectatorCheckpointState restores the captured named scalars
    // after the authentic native constructors have completed.
    g_Supervisor.curState = SUPERVISOR_STATE_MAINMENU;
    g_Supervisor.wantedState = SUPERVISOR_STATE_GAMEMANAGER;
    g_Supervisor.wantedState2 = SUPERVISOR_STATE_GAMEMANAGER;
    std::printf("netplay spectator TH06: restore register begin stage=%d\n", cp.stage);
    const bool constructed = GameManager::RegisterChain() == ZUN_SUCCESS;
    std::printf("netplay spectator TH06: restore register result=%d\n", constructed ? 1 : 0);
    SideEffects::SetSpeculative(false);
    if (!constructed)
        return false;
    g_SpectatorNativeStageReady = true;
    if (cp.gameFrames != 0)
    {
        std::array<FrameInput, TH06_MULTI_MAX_PLAYERS> neutralInputs{};
        Input::SetReplayOverride(0);
        Input::SetPlayerInputOverrides(neutralInputs.data(), cp.session.playerCount);
        g_Supervisor.curState = cp.supervisorCurrent;
        g_Supervisor.wantedState = cp.supervisorWanted;
        g_Supervisor.wantedState2 = cp.supervisorWanted2;
        SideEffects::SetSpeculative(true);
        for (std::uint32_t i = 0; i < cp.gameFrames; ++i)
            if (g_Chain.RunCalcChain() == CHAIN_CALLBACK_RESULT_EXIT_GAME_ERROR)
                break;
        SideEffects::SetSpeculative(false);
        Input::ClearPlayerButtonOverrides();
        Input::ClearReplayOverride();
        std::printf("spectator-debug warmed native ticks=%u gmFrames=%u\n",
                    static_cast<unsigned>(cp.gameFrames), static_cast<unsigned>(g_GameManager.gameFrames));
    }
    std::printf("netplay spectator TH06: restore apply begin\n");
    if (!ApplySpectatorCheckpointState(cp))
    {
        SideEffects::SetSpeculative(false);
        return false;
    }

    LogSpectatorNativeDiagnosticParts("restore");
    const auto actual = Th06CanonicalHash::Capture().composite;
    if (actual != cp.expectedHash)
    {
        const auto expectedDebug = Th06CanonicalHash::Capture();
        std::printf("spectator-debug actual parts composite=%016llx meta=%016llx game=%016llx stage=%016llx player0=%016llx player1=%016llx player2=%016llx enemies=%016llx bullets=%016llx items=%016llx counts=%u,%u,%u,%u\n",
                    static_cast<unsigned long long>(expectedDebug.composite), static_cast<unsigned long long>(expectedDebug.meta),
                    static_cast<unsigned long long>(expectedDebug.metaGame), static_cast<unsigned long long>(expectedDebug.stage),
                    static_cast<unsigned long long>(expectedDebug.player0), static_cast<unsigned long long>(expectedDebug.player1),
                    static_cast<unsigned long long>(expectedDebug.player2), static_cast<unsigned long long>(expectedDebug.enemies),
                    static_cast<unsigned long long>(expectedDebug.bullets), static_cast<unsigned long long>(expectedDebug.items),
                    expectedDebug.enemyCount, expectedDebug.bulletCount, expectedDebug.laserCount, expectedDebug.itemCount);
        std::printf("netplay spectator TH06: checkpoint hash mismatch frame=%u expected=%016llx actual=%016llx expectedPlayer=%016llx/%016llx/%016llx actualPlayer=%016llx/%016llx/%016llx expectedP0=(%.2f,%.2f,%.2f)/%d/%d/%d/%d/%.3f actualP0=(%.2f,%.2f,%.2f)/%d/%d/%d/%d/%.3f expectedP1=(%.2f,%.2f,%.2f)/%d/%d/%d/%d/%.3f actualP1=(%.2f,%.2f,%.2f)/%d/%d/%d/%d/%.3f\n",
                    event.frame, static_cast<unsigned long long>(cp.expectedHash),
                    static_cast<unsigned long long>(actual),
                    static_cast<unsigned long long>(cp.expectedPlayer0),
                    static_cast<unsigned long long>(cp.expectedPlayer1),
                    static_cast<unsigned long long>(cp.expectedPlayer2),
                    static_cast<unsigned long long>(expectedDebug.player0),
                    static_cast<unsigned long long>(expectedDebug.player1),
                    static_cast<unsigned long long>(expectedDebug.player2),
                    cp.playerPositionCenter[0], cp.playerPositionCenter[1], cp.playerPositionCenter[2],
                    cp.playerRespawnTimer[0], cp.playerBulletGracePeriod[0], cp.playerState[0],
                    cp.playerInvulnerabilityTimer[1], cp.playerInvulnerabilitySubFrame[0],
                    g_Players[0].positionCenter.x, g_Players[0].positionCenter.y, g_Players[0].positionCenter.z,
                    g_Players[0].respawnTimer, g_Players[0].bulletGracePeriod, g_Players[0].playerState,
                    g_Players[0].invulnerabilityTimer.current, g_Players[0].invulnerabilityTimer.subFrame,
                    cp.playerPositionCenter[3], cp.playerPositionCenter[4], cp.playerPositionCenter[5],
                    cp.playerRespawnTimer[1], cp.playerBulletGracePeriod[1], cp.playerState[1],
                    cp.playerInvulnerabilityTimer[3], cp.playerInvulnerabilitySubFrame[1],
                    g_Players[1].positionCenter.x, g_Players[1].positionCenter.y, g_Players[1].positionCenter.z,
                    g_Players[1].respawnTimer, g_Players[1].bulletGracePeriod, g_Players[1].playerState,
                    g_Players[1].invulnerabilityTimer.current, g_Players[1].invulnerabilityTimer.subFrame);
        return false;
    }
    g_SimFrame = event.frame;
    PublishCheckpointDiagnostic(event.frame, static_cast<std::uint8_t>(cp.stage), cp.expectedHash,
                                g_Core.ConfirmedThroughAllRemotes(), 0, false, true);
    std::printf("netplay spectator TH06: checkpoint restored frame=%u stage=%d hash=%016llx\n",
                event.frame, cp.stage, static_cast<unsigned long long>(actual));
    return true;
}

bool TryPendingSpectatorBootstrap()
{
    if (!g_HavePendingSpectatorBootstrap)
        return true;
    const auto received = g_SpectatorPlayback.History().Received();
    if (received > g_PendingSpectatorBootstrap.frame)
        return false;
    if (!g_SpectatorPlayback.History().BootstrapMatches(g_PendingSpectatorBootstrap))
        return true;
    const bool accepted = g_SpectatorPlayback.Receive(g_PendingSpectatorBootstrap);
    g_HavePendingSpectatorBootstrap = false;
    return accepted;
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
        // Launcher explicitly rearms the retained Runtime for its next launch;
        // never reinterpret the old read-only socket as
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
    g_UiCaptureFrame = 0;
    g_NextUiCaptureNs = 0;
    g_HaveLastUiCapture = false;
    g_DriverTicks = 0;
    g_Sequence = 1;
    g_LastReceivedSequence = 0;
    g_LastHelloSendTick = 0;
    g_LastReadySendTick = 0;
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
    return g_TestUseStageTransition;
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
            return g_BrowserPeerTransport.SendSpectator(wire.data(), wire.size());
        }, &failure);
    g_NextSpectatorPublishFrame = g_SpectatorPublisher.Published();
    if (!sent)
    {
        const char *reason = SpectatorStagePublisher::FailureName(failure);
        const bool permanent =
            failure == SpectatorStagePublisher::PublishFailure::InvalidPublisher ||
            failure == SpectatorStagePublisher::PublishFailure::MissingRecord ||
            failure == SpectatorStagePublisher::PublishFailure::StageCheckpointMissing ||
            failure == SpectatorStagePublisher::PublishFailure::CheckpointEncode ||
            failure == SpectatorStagePublisher::PublishFailure::InputUnavailable ||
            failure == SpectatorStagePublisher::PublishFailure::InputEncode ||
            failure == SpectatorStagePublisher::PublishFailure::BossNodeEncode;
        std::printf("netplay spectator TH06: publish %s frame=%u permanent=%u cursor=%u confirmed=%u simFrame=%u\n",
                    reason, g_NextSpectatorPublishFrame, permanent ? 1u : 0u,
                    g_NextSpectatorPublishFrame, g_Core.ConfirmedThroughAllRemotes(), g_SimFrame);
#ifdef __EMSCRIPTEN__
        EM_ASM({
            const state = globalThis.__eaglerNetplaySpectatorPublish ||= {};
            state.lastPublishFailure = UTF8ToString($1);
            state.lastPublishFailureFrame = $0 >>> 0;
            state.lastPublishPermanent = !!$2;
            if ($2) globalThis.__eaglerNetplaySpectatorPublishFailed = $0 >>> 0;
        }, g_NextSpectatorPublishFrame, reason, permanent ? 1 : 0);
#endif
        if (permanent)
            DisableSpectatorPublisher(g_NextSpectatorPublishFrame, reason);
    }
}

void DisableSpectatorPublisher(std::uint32_t frame, const char *reason)
{
    // Spectator publication is an optional P0 lane; retire it without failing
    // the player transport or ending the multiplayer session.
    if (!g_SpectatorPublisherEligible || g_SpectatorLaneStopped || g_SpectatorMode ||
        g_LocalPlayer != 0 || !ProductionLanMode())
        return;
    g_SpectatorPublisherReady = false;
    g_SpectatorLaneStopped = true;
    if (!reason)
        reason = "unknown";
    g_BrowserPeerTransport.StopSpectators(false);
    std::printf("netplay spectator TH06: publisher stopped frame=%u reason=%s\n",
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
    bool beginGesture = false;
    if (Touch::GetFreeJoystickVector(&x, &y))
        Input::CaptureJoystick(x, y);
    else if (Touch::TakePlayerDelta(&x, &y, &beginGesture))
        Input::CaptureDirectTouchDelta(x, y, Touch::IsUnlimited(), beginGesture);
    FrameInput input = Input::EndCapture();
    if (input.analogMode == AnalogMode::DirectTouchBegin)
        ++g_DirectTouchBeginCaptures;
    else if (input.analogMode == AnalogMode::DirectTouchDelta)
        ++g_DirectTouchDeltaCaptures;
#ifdef __EMSCRIPTEN__
    if (ProbeMode() || UsePhysicalInput())
    {
        EM_ASM({
            globalThis.__eaglerNetplayDirectTouchBeginCaptures = $0;
            globalThis.__eaglerNetplayDirectTouchDeltaCaptures = $1;
        }, g_DirectTouchBeginCaptures, g_DirectTouchDeltaCaptures);
    }
#endif
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
    return (g_GameManager.isInGameMenu && g_AsciiManager.gameMenu.NeedsConfirmedGameMenuInput()) ||
           g_GameManager.isInRetryMenu ||
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
    // The hidden Replay save/playback gate has a finite authoritative input
    // window. Once every peer has confirmed the final recorded frame, no
    // later input is supposed to advance: the driver intentionally sends only
    // the final tail keepalive while it saves and switches to Replay playback.
    // Do not let the production liveness watchdog race that test-only
    // finalization state and misreport the expected stationary confirmation
    // frontier as a dead peer.
    if (UseReplayPlaybackCycle() && g_SimFrame >= g_TestFrames &&
        g_Core.ConfirmedThroughAllRemotes() >= g_TestFrames - 1)
        return false;

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
            g_RemoteInputWatchdogs[player].Disarm();
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
    Input::ResetDirectTouchStates();
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
#ifdef __EMSCRIPTEN__
    g_ReliableInputRepair = !g_SpectatorMode &&
        EM_ASM_INT({ return Module.eaglerOptions?.netplayReliableInputRepair ? 1 : 0; }) != 0;
    g_InputRepairBudgets = {};
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
    g_ReliableInputRepair = false;
    g_InputRepairBudgets = {};
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
#ifdef __EMSCRIPTEN__
    stateConfig.coalesceBulletRuns = EM_ASM_INT({
        const value = Module.eaglerOptions?.netplayCoalesceBulletRuns;
        return value == null ? 1 : (value ? 1 : 0);
    }) != 0;
    stateConfig.fastBulkCopy = EM_ASM_INT({
        return Module.eaglerOptions?.netplayFastBulkCopy ? 1 : 0;
    }) != 0;
    stateConfig.coalesceRestore = EM_ASM_INT({
        return Module.eaglerOptions?.netplayCoalesceRestore ? 1 : 0;
    }) != 0;
    stateConfig.liveBulletSnapshots = EM_ASM_INT({
        return Module.eaglerOptions?.netplayLiveBulletSnapshots ? 1 : 0;
    }) != 0;
    stateConfig.auditLiveBulletBytes = RollbackAuditEnabled() && stateConfig.liveBulletSnapshots && EM_ASM_INT({
        return Module.eaglerOptions?.netplayLiveBulletAudit ? 1 : 0;
    }) != 0;
    g_DemandSnapshots = EM_ASM_INT({
        const policy = Module.eaglerOptions?.netplaySnapshotPolicy;
        return policy == null || policy === 'demand' || policy === 'frontier' ? 1 : 0;
    }) != 0;
    g_FrontierSnapshots = EM_ASM_INT({
        const policy = Module.eaglerOptions?.netplaySnapshotPolicy;
        return policy == null || policy === 'frontier' ? 1 : 0;
    }) != 0;
    EM_ASM({
        globalThis.__eaglerNetplaySnapshotPolicy = $0 ? 'frontier' : ($1 ? 'demand' : 'always');
    }, g_FrontierSnapshots ? 1 : 0, g_DemandSnapshots ? 1 : 0);
#else
    g_DemandSnapshots = false;
    g_FrontierSnapshots = false;
#endif
    if (!Th06Rollback::Reset(stateConfig))
        return false;
    g_SnapshotTicks = 0;
    g_ConfirmedOnlyTicks = 0;

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
    sessionConfig.gameplayAbi = GameplayAbi();
    sessionConfig.gameId = GAME_ID_TH06;
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
    g_DirectTouchBeginCaptures = 0;
    g_DirectTouchDeltaCaptures = 0;
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
    g_SpectatorPlayback.Clear();
    g_SpectatorView.Clear();
    g_HavePendingSpectatorBootstrap = false;
    g_PendingSpectatorBootstrap = {};
    g_SpectatorNativeStageReady = false;
    g_StageStartFrameTags.fill(INVALID_FRAME);
    g_StageAtFrameStart.fill(0);
    g_SpectatorPublisherEligible = !g_SpectatorMode && g_LocalPlayer == 0 &&
                                   ProductionLanMode();
    g_SpectatorLaneStopped = false;
    g_SpectatorPublisherReady = g_SpectatorPublisherEligible &&
                                g_SpectatorPublisher.Reset('6', g_PlayerCount,
                                                           CurrentSessionId(), GameplayAbi());
    g_SpectatorBossSlotsThisFrame = 0;
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
            globalThis.__eaglerNetplayDirectTouchBeginCaptures = 0;
            globalThis.__eaglerNetplayDirectTouchDeltaCaptures = 0;
            globalThis.__eaglerNetplayLanGeneration = $0;
            globalThis.__eaglerNetplayLanHighestStage = 0;
            globalThis.__eaglerNetplayLanStageTransitionObserved = false;
            globalThis.__eaglerNetplayLanCheckpointFrame = -1;
            globalThis.__eaglerNetplayLanCheckpointHash = "";
            globalThis.__eaglerNetplayLanCheckpointStage = -1;
            globalThis.__eaglerNetplayLanCheckpointConfirmed = -1;
            globalThis.__eaglerNetplayLanCheckpointPredictedMask = -1;
            globalThis.__eaglerNetplayLanCheckpointCaptureFrame = -1;
            globalThis.__eaglerNetplayLanCheckpointCaptureHash = "";
            globalThis.__eaglerNetplayLanCheckpointCaptureConfirmed = -1;
            globalThis.__eaglerNetplayLanCheckpointCapturePredictedMask = -1;
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

// BEGIN CONFIRMED UI INPUT PUMP
bool PumpConfirmedUiInput()
{
    constexpr std::uint64_t interval = 1000000000ull / 60ull;
    constexpr std::uint32_t maxLead = 8;
    const bool sharedUi = SharedUiNeedsConfirmedInputs();
    if (!sharedUi)
    {
        g_NextUiCaptureNs = 0;
        g_HaveLastUiCapture = false;
        return g_UiCaptureFrame <= g_SimFrame || (g_DriverTicks % 3u) != 0u ||
            SendScheduledLocalFrame(g_Core.LocalFrameForCapture(g_UiCaptureFrame - 1u));
    }
    const std::uint64_t now = SDL_GetTicksNS();
    if (g_NextUiCaptureNs == 0)
    {
        g_UiCaptureFrame = std::max(g_UiCaptureFrame, g_SimFrame);
        g_NextUiCaptureNs = now;
    }
    const std::uint32_t last = g_SimFrame < INVALID_FRAME - maxLead
        ? g_SimFrame + maxLead : g_SimFrame;
    const unsigned due = now >= g_NextUiCaptureNs ? BoundedCaptureBatch(
        (now - g_NextUiCaptureNs) / interval + 1u, g_UiCaptureFrame, last,
        ProbeMode() ? g_TestFrames - g_Core.LocalFrameForCapture(0) : INVALID_FRAME, 8) : 0;
    for (unsigned i = 0; i < due; ++i)
    {
        if (!g_Core.HasLocalCapture(g_UiCaptureFrame))
        {
            FrameInput input;
            if (!UsePhysicalInput() || i + 1u == due || !g_HaveLastUiCapture)
            {
                input = CaptureLocalInput(g_UiCaptureFrame);
                g_LastUiCapture = input;
                g_HaveLastUiCapture = true;
            }
            else
            {
                // Missed physical slots may continue held buttons, but never
                // repeat a menu/Bomb edge or a consumed touch displacement.
                input = g_LastUiCapture;
                input.buttons &= TH_BUTTON_DIRECTION | TH_BUTTON_FOCUS |
                    TH_BUTTON_SHOOT | TH_BUTTON_SKIP;
                input.touchBomb = false;
                if (input.analogMode == AnalogMode::DirectTouchBegin ||
                    input.analogMode == AnalogMode::DirectTouchDelta)
                {
                    input.analogMode = AnalogMode::DirectTouchDelta;
                    input.x = input.y = 0;
                }
            }
            if (!g_Core.ScheduleLocalInput(g_UiCaptureFrame, input) ||
                !SendScheduledLocalFrame(g_Core.LocalFrameForCapture(g_UiCaptureFrame)))
                return false;
        }
        ++g_UiCaptureFrame;
        g_NextUiCaptureNs += interval;
    }
    return g_UiCaptureFrame == 0 || (g_DriverTicks % 3u) != 0u ||
        SendScheduledLocalFrame(g_Core.LocalFrameForCapture(g_UiCaptureFrame - 1u));
}
// END CONFIRMED UI INPUT PUMP

bool SendLocalFrame(std::uint32_t captureFrame)
{
    const FrameInput input = CaptureLocalInput(captureFrame);
    if (UsePhysicalInput() && captureFrame == 0 && !g_PhysicalLoggedFrame0Sample)
    {
        g_PhysicalLoggedFrame0Sample = true;
        std::printf("netplay lan physical: FRAME0 SAMPLE player=%u bits=0x%04x\n",
                    static_cast<unsigned>(g_LocalPlayer),
                    static_cast<unsigned>(input.buttons));
    }
    if (!g_Core.ScheduleLocalInput(captureFrame, input))
        return false;
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
    if (frame == 34u)
    {
        if (!resimulation)
        {
            g_SpectatorDebugFrame34FirstInputs = decision.inputs;
            g_SpectatorDebugFrame34FirstSeen = true;
        }
        std::printf("spectator-debug frame34 %s predicted=%u inputs=",
                    resimulation ? "resim" : "first",
                    static_cast<unsigned>(decision.predictedMask));
        for (std::size_t player = 0; player < MAX_PLAYERS; ++player)
        {
            const FrameInput &input = decision.inputs[player];
            const FrameInput &prior = g_SpectatorDebugFrame34FirstInputs[player];
            std::printf(" p%u={btn:%u mode:%u xy:%.9g,%.9g ul:%u used:%u bomb:%u",
                        static_cast<unsigned>(player),
                        static_cast<unsigned>(input.buttons),
                        static_cast<unsigned>(input.analogMode), input.x, input.y,
                        input.unlimited ? 1u : 0u, input.touchUsed ? 1u : 0u,
                        input.touchBomb ? 1u : 0u);
            if (resimulation && g_SpectatorDebugFrame34FirstSeen)
                std::printf(" old:%u/%u/%.9g,%.9g/%u/%u/%u",
                            static_cast<unsigned>(prior.buttons),
                            static_cast<unsigned>(prior.analogMode), prior.x, prior.y,
                            prior.unlimited ? 1u : 0u, prior.touchUsed ? 1u : 0u,
                            prior.touchBomb ? 1u : 0u);
            std::printf("}");
        }
        std::printf("\n");
    }
    g_SpectatorBossSlotsThisFrame = 0;
    std::vector<std::uint8_t> spectatorInitializer;
    std::uint32_t spectatorSchema = 0;
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
    const bool snapshotAllowed = !g_SpectatorMode;
    const bool captureRollback = snapshotAllowed &&
        (!g_DemandSnapshots || NeedsRollbackSnapshot(
            frame, g_Core.ConfirmedThroughAllRemotes(), decision.predictedMask,
            resimulation, g_FrontierSnapshots));
    if (snapshotAllowed && !captureRollback)
    {
        Th06Rollback::DiscardBefore(frame);
        ++g_ConfirmedOnlyTicks;
    }
    if (snapshotAllowed && RollbackAuditEnabled())
    {
        RestoreAuditEntry &entry = g_RestoreAudit[frame % g_RestoreAudit.size()];
        entry.frame = frame;
        entry.sample = Th06CanonicalHash::Capture();
        const auto debugState = Th06Rollback::CaptureDebugState();
        entry.localDebugHash = debugState.aggregate;
        entry.localDebugParts = debugState.parts;
    }
    if (captureRollback && !Th06Rollback::BeginFrame(frame))
        return -1;
    if (captureRollback)
        ++g_SnapshotTicks;
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
        // The generic-journal stress lane deliberately forces every Bullet
        // slot through Touch(). Specialized Bullet snapshot backends instead
        // keep their production sparse policy: live state is captured by
        // BeginFrame(), while inactive/reused slots are touched by their
        // mutation paths. Forcing TouchBullet(AllParts) here would defeat the
        // live-part backend and turn this test hook into a different workload.
        if (!Th06Rollback::SpecializedBulletSnapshotsEnabled())
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
    if (UseStageTransitionTest() && frame >= 3000u &&
        g_GameManager.currentStage == 1)
    {
        // The browser stage-transition smoke should exercise the actual native
        // boss-death, results-message, and next-stage registration path without
        // requiring a full manual clear. Wait for Stage 1's real boss to become
        // interactable, then deterministically deplete its remaining life. This
        // does not change currentStage or skip the native ECL/GUI lifecycle.
        for (Enemy *boss : g_EnemyManager.bosses)
        {
            if (boss == nullptr || !boss->flags.isSlotOccupied ||
                !boss->flags.isBoss)
                continue;
            if (boss->flags.isInteractable && boss->life > 0)
            {
                boss->life = 0;
                std::printf(
                    "netplay lan stage: FIXTURE stage-transition frame=%u confirmed=%u predictedMask=0x%x\n",
                    frame,
                    static_cast<unsigned>(g_Core.ConfirmedThroughAllRemotes()),
                    static_cast<unsigned>(decision.predictedMask));
            }
            if (boss->timerCallbackThreshold >= 0 &&
                boss->bossTimer.AsFrames() < boss->timerCallbackThreshold)
            {
                boss->bossTimer.SetCurrent(boss->timerCallbackThreshold);
            }
        }
    }
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
            if (boss == nullptr || !boss->flags.isSlotOccupied || !boss->flags.isBoss)
                continue;
            if (boss->flags.isInteractable && boss->life > 0)
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
    const std::size_t stageSlot = frame % g_StageStartFrameTags.size();
    std::uint8_t previousStage = 0;
    if (frame != 0)
    {
        const std::size_t previousSlot = (frame - 1u) % g_StageStartFrameTags.size();
        if (g_StageStartFrameTags[previousSlot] == frame - 1u)
            previousStage = g_StageAtFrameStart[previousSlot];
    }
    const bool enteredCheckpointStage = stageBeforeCalc >= 2 &&
        previousStage < stageBeforeCalc;
    g_StageStartFrameTags[stageSlot] = frame;
    g_StageAtFrameStart[stageSlot] = stageBeforeCalc;
    if (enteredCheckpointStage && !g_SpectatorMode && ProductionLanMode() &&
        RollbackAuditEnabled())
    {
        const auto source = Th06CanonicalHash::Capture();
        const std::uint32_t confirmed = g_Core.ConfirmedThroughAllRemotes();
        PublishCheckpointDiagnostic(frame, stageBeforeCalc, source.composite,
                                    confirmed, decision.predictedMask, false);
        std::printf("spectator-debug checkpoint source boundary frame=%u stage=%u hash=%016llx confirmed=%u predicted=0x%x resim=%d\n",
                    frame, static_cast<unsigned>(stageBeforeCalc),
                    static_cast<unsigned long long>(source.composite), confirmed,
                    static_cast<unsigned>(decision.predictedMask), resimulation ? 1 : 0);
    }
    if (!g_SpectatorMode && stageBeforeCalc == 2 && frame == 6680u)
    {
        const auto source = Th06CanonicalHash::Capture();
        std::printf("spectator-debug source-stage2 local=%u frame=%u predicted=0x%x composite=%016llx meta=%016llx game=%016llx stage=%016llx player0=%016llx player1=%016llx player2=%016llx enemies=%016llx bullets=%016llx items=%016llx counts=%u,%u,%u,%u\n",
                    static_cast<unsigned>(g_LocalPlayer), frame,
                    static_cast<unsigned>(decision.predictedMask),
                    static_cast<unsigned long long>(source.composite),
                    static_cast<unsigned long long>(source.meta),
                    static_cast<unsigned long long>(source.metaGame),
                    static_cast<unsigned long long>(source.stage),
                    static_cast<unsigned long long>(source.player0),
                    static_cast<unsigned long long>(source.player1),
                    static_cast<unsigned long long>(source.player2),
                    static_cast<unsigned long long>(source.enemies),
                    static_cast<unsigned long long>(source.bullets),
                    static_cast<unsigned long long>(source.items), source.enemyCount,
                    source.bulletCount, source.laserCount, source.itemCount);
    }
    const auto checkpointRequirement = g_SpectatorPublisherReady
        ? g_SpectatorPublisher.NeedsCheckpoint(frame, stageBeforeCalc)
        : SpectatorStagePublisher::CheckpointRequirement::NotRequired;
    const bool publisherCheckpointDue = checkpointRequirement ==
        SpectatorStagePublisher::CheckpointRequirement::Required;
    if (g_SpectatorPublisherReady && checkpointRequirement ==
            SpectatorStagePublisher::CheckpointRequirement::MissingPredecessor)
    {
        std::printf("netplay spectator TH06: publisher checkpoint predecessor unavailable frame=%u stage=%u\n",
                    frame, static_cast<unsigned>(stageBeforeCalc));
        DisableSpectatorPublisher(frame, "checkpoint-predecessor-missing");
    }
    if (g_SpectatorPublisherReady && publisherCheckpointDue)
    {
        if (EncodeSpectatorCheckpoint(&spectatorInitializer, decision.predictedMask))
        {
            spectatorSchema = SPECTATOR_CHECKPOINT_SCHEMA;
        }
        else
        {
            // Spectator history is an optional publication lane. If a title
            // checkpoint cannot be represented, keep the player session alive
            // and stop advertising a stream with an unrecoverable gap.
            DisableSpectatorPublisher(frame, "checkpoint-capture-failed");
        }
    }
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
    if (captureRollback && !Th06Rollback::EndFrame())
        return -1;
    if (!g_SpectatorMode && !g_Core.MarkSimulated(frame, decision))
        return -1;
    if (g_SpectatorPublisherReady)
    {
        SpectatorStagePublisher::Frame record;
        record.frame = frame;
        record.stage = stageBeforeCalc;
        record.schema = spectatorSchema;
        record.initializer = std::move(spectatorInitializer);
        record.inputs = decision.inputs;
        record.hasInputs = true;
        record.bossSlots = g_SpectatorBossSlotsThisFrame;
        if (!g_SpectatorPublisher.Record(std::move(record)))
        {
            std::printf("spectator-debug publisher record failed frame=%u cursor=%u confirmed=%u resim=%d stage=%u gameFrames=%u\n",
                        frame, g_SpectatorPublisher.Published(),
                        static_cast<unsigned>(g_Core.ConfirmedThroughAllRemotes()),
                        resimulation ? 1 : 0, stageBeforeCalc,
                        static_cast<unsigned>(g_GameManager.gameFrames));
            // Recording is intentionally not coupled to player simulation
            // success: observers can lose their optional stream independently.
            DisableSpectatorPublisher(frame, "record-rejected");
        }
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
            globalThis.__eaglerNetplayRestoreCopiedBytes = $2;
            globalThis.__eaglerNetplayRestoreSkippedBytes = $3;
            globalThis.__eaglerNetplayJournalArenaGrowths = $4;
            globalThis.__eaglerNetplaySnapshotTicks = $5;
            globalThis.__eaglerNetplayConfirmedOnlyTicks = $6;
        }, static_cast<double>(g_MaxSnapshotBytes),
           static_cast<double>(g_MaxSnapshotBlocks),
           static_cast<double>(Th06Rollback::RestoreCopiedBytes()),
           static_cast<double>(Th06Rollback::RestoreSkippedBytes()),
           static_cast<double>(Th06Rollback::ArenaGrowths()),
           g_SnapshotTicks, g_ConfirmedOnlyTicks);
    }
#endif
#ifdef __EMSCRIPTEN__
    // The rollback-audit fixture samples every 20 frames so it can compare an
    // exact +600-frame spectator restore target (including non-300 boundaries).
    // Normal production telemetry remains at its lower-cost 300-frame cadence.
    const std::uint32_t canonicalInterval = RollbackAuditEnabled() ? 20u : 300u;
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
    if (rollback <= 34u && last >= 34u)
    {
        std::printf("spectator-debug frame34 rollback requested=%u last=%u confirmed=%u p1=%u p2=%u resims=%u\n",
                    rollback, last,
                    static_cast<unsigned>(g_Core.ConfirmedThroughAllRemotes()),
                    static_cast<unsigned>(g_Core.ConfirmedThrough(1)),
                    static_cast<unsigned>(g_Core.ConfirmedThrough(2)),
                    g_ResimulatedFrames);
    }
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
        const auto actualLocalDebug = Th06Rollback::CaptureDebugState();
        if (expected.frame != replayFrom ||
            expected.sample.composite != actual.composite ||
            expected.localDebugHash != actualLocalDebug.aggregate)
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
                static_cast<unsigned long long>(actualLocalDebug.aggregate),
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
            std::printf("netplay rollback audit local-owner differences frame=%u", replayFrom);
            bool anyLocalOwnerDifference = false;
            for (std::size_t i = 0; i < actualLocalDebug.parts.size(); ++i)
            {
                const auto &before = expected.localDebugParts[i];
                const auto &after = actualLocalDebug.parts[i];
                if (before.hash == after.hash)
                    continue;
                anyLocalOwnerDifference = true;
                std::printf(" %s=%016llx/%016llx", after.owner ? after.owner : "unknown",
                            static_cast<unsigned long long>(before.hash),
                            static_cast<unsigned long long>(after.hash));
            }
            if (!anyLocalOwnerDifference)
                std::printf(" none");
            std::printf("\n");
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
    char correctedBgmPath[256];
    if (SideEffects::ConsumeCorrectedBgmPlay(correctedBgmPath, sizeof(correctedBgmPath)))
        g_Supervisor.PlayAudio(correctedBgmPath);
    return true;
}
} // namespace

bool Active()
{
    return RequestedInternal() && g_Active;
}

void RecordBossRegistration(std::uint8_t slot)
{
    if (g_SpectatorPublisherReady && slot < 8)
        g_SpectatorBossSlotsThisFrame |= static_cast<std::uint8_t>(1u << slot);
}

bool SpectatorViewing()
{
    return g_SpectatorMode;
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

bool TransportReady()
{
    return !ProductionLanMode() ||
           (g_ProductionTransportStarted && TransportIsOpen());
}

bool LastTickAdvanced()
{
    return !RequestedInternal() || g_LastTickAdvanced;
}

void SetGameplayStartSeed(std::uint16_t seed)
{
    g_GameplayStartSeed = seed;
}

bool Requested()
{
    return RequestedInternal();
}

void ReturnToRoom()
{
    g_BrowserPeerTransport.Close();
    g_ProductionTransportStarted = false;
    RetireGameplaySession();
    g_SpectatorRunRetired = true;
    g_SpectatorPlayback.Clear();
    g_SpectatorView.Clear();
    if (g_Supervisor.wantedState == SUPERVISOR_STATE_MAINMENU &&
        g_Supervisor.curState == SUPERVISOR_STATE_GAMEMANAGER)
        g_Supervisor.wantedState = SUPERVISOR_STATE_INIT;
    g_Supervisor.curState = SUPERVISOR_STATE_MAINMENU;
}

void BeginRoomRun()
{
    g_SpectatorRunRetired = false;
    g_SpectatorMode = false;
    g_Initialized = g_Active = g_Done = false;
    g_SpectatorPlayback.Clear();
    g_SpectatorView.Clear();
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
    if (g_Initialized && !g_SpectatorMode && !SessionStillOwnsStageState())
    {
        // Gameplay ownership is one deterministic rollback session.
        // TH06 retires that session as soon as the confirmed gameplay state
        // requests a post-game scene (Ending/Result/MainMenu). A later new run
        // starts again from frame zero while reusing only the established
        // browser transport. TH06 multiplayer never exposes Continue; a team
        // wipe retires this generation directly into Result after its grace.
        RetireGameplaySession();
    }
    if (!g_Initialized && !g_SpectatorMode && !EligibleForInitialNetplayStart())
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
        const std::uint32_t confirmed = g_Core.ConfirmedThroughAllRemotes();
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
        if (!PumpConfirmedUiInput())
        {
            Fail("confirmed UI input pump");
            return -1;
        }
        const bool localPresent = g_Core.HasLocalCapture(g_SimFrame);
        if (!localPresent && !SendLocalFrame(g_SimFrame))
        {
            Fail("send local input");
            return -1;
        }
        if (localPresent && (g_DriverTicks % 3u) == 0u &&
            !SendScheduledLocalFrame(g_Core.LocalFrameForCapture(g_SimFrame)))
        {
            Fail("input retry");
            return -1;
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

        // Ordinary Pause navigation/animation uses its existing snapshot.
        // Screenshot setup, final resume/exit/restart and unsupported phases
        // still require a contiguous actual prefix before external state moves.
        if (SharedUiNeedsConfirmedInputs() &&
            g_Core.ConfirmedThroughAllRemotes() < g_SimFrame)
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
        g_Core.ConfirmedThroughAllRemotes() >= g_TestFrames - 1 &&
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
    if (ProbeMode() && g_DriverTicks >= g_TestFrames * 6u)
    {
        Fail("timeout");
        return -1;
    }
    return CHAIN_CALLBACK_RESULT_CONTINUE;
}
} // namespace Netplay::Th06LanStageProbe

extern "C" const char *th06_mp_spectator_state()
{
    return Netplay::Th06LanStageProbe::SpectatorState();
}

extern "C" int th06_mp_spectator_control(const char *run, const char *action,
                                          std::uint32_t frame)
{
    return Netplay::Th06LanStageProbe::SpectatorControl(run, action, frame);
}
