#include "Th07RollbackProbe.hpp"

#include <eagler/netplay/NetplayInput.hpp>
#include "NetplaySideEffects.hpp"
#include "Th07RollbackState.hpp"

#include "Chain.hpp"
#include "GameManager.hpp"
#include "ReplayManager.hpp"
#include "Supervisor.hpp"

#include <cstdint>
#include <cstdio>
#include <cstdlib>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

namespace Netplay::Th07RollbackProbe
{
namespace
{
constexpr int STABLE_GAMEPLAY_TICKS = 120;

bool g_ProbeDone = false;
int g_LastStage = -1;
int g_StableTicks = 0;
std::uint32_t g_ProbeFrame = 1;

bool Requested()
{
#ifdef __EMSCRIPTEN__
    return EM_ASM_INT({
        return Module.eaglerOptions?.netplayRollbackProbe === true ? 1 : 0;
    }) != 0;
#else
    static const bool requested = []() {
        const char *value = std::getenv("EAGLER_NETPLAY_ROLLBACK_PROBE");
        if (value && value[0] == '1' && value[1] == '\0')
            return true;

        // The MCP/native smoke-test runner cannot inject environment variables.
        // A deliberately named cwd marker gives it an equivalent opt-in without
        // changing normal TH_ENABLE_NETPLAY behavior.
        if (std::FILE *marker = std::fopen("netplay-rollback-probe.enable", "rb"))
        {
            std::fclose(marker);
            return true;
        }
        return false;
    }();
    return requested;
#endif
}

bool EligibleGameplayState()
{
    return g_GameManager.notInMenu && !g_GameManager.replay &&
           !g_GameManager.isPaused && !g_GameManager.isInPauseMenu &&
           !g_GameManager.isInRetryMenu && !g_GameManager.isTimeStopped &&
           g_GameManager.currentStage > 0 && g_GameManager.globals != nullptr &&
           g_GameManager.defaultCfg != nullptr && g_ReplayManager != nullptr &&
           g_Supervisor.curState == 2 && g_Supervisor.wantedState == g_Supervisor.curState;
}

void ClearTransientModes()
{
    Input::ClearReplayOverride();
    if (Input::CaptureActive())
        (void)Input::EndCapture();
    SideEffects::SetSpeculative(false);
}

int FailAfterSpeculative(const char *reason, std::uint32_t frame)
{
    ClearTransientModes();
    Supervisor::DebugPrint("netplay rollback probe: FAIL frame=%u stage=%d reason=%s\n",
                           frame, static_cast<int>(g_GameManager.currentStage), reason);
    // The first pass deliberately suppressed persistent side effects. Continuing
    // from it would therefore be a state the real game can never produce.
    return CHAIN_CALLBACK_RESULT_EXIT_GAME_ERROR;
}
} // namespace

int RunCalcChain()
{
    if (!Requested() || g_ProbeDone)
        return g_Chain.RunCalcChain();

    const int stage = static_cast<int>(g_GameManager.currentStage);
    if (!EligibleGameplayState())
    {
        g_LastStage = stage;
        g_StableTicks = 0;
        return g_Chain.RunCalcChain();
    }
    if (stage != g_LastStage)
    {
        g_LastStage = stage;
        g_StableTicks = 0;
        return g_Chain.RunCalcChain();
    }
    if (++g_StableTicks < STABLE_GAMEPLAY_TICKS)
        return g_Chain.RunCalcChain();

    g_ProbeDone = true;

    Th07Rollback::Config config;
    config.maxFrames = 2;
    config.maxBytesPerFrame = 32 * 1024 * 1024;
    config.maxBlocksPerFrame = 4096;
    config.maxBombEffectsPerFrame = 1024;
    if (!Th07Rollback::Reset(config))
    {
        Supervisor::DebugPrint("netplay rollback probe: FAIL reason=state reset\n");
        return CHAIN_CALLBACK_RESULT_EXIT_GAME_ERROR;
    }

    const std::uint32_t frame = g_ProbeFrame++;
    if (!Th07Rollback::BeginFrame(frame))
    {
        Supervisor::DebugPrint("netplay rollback probe: FAIL frame=%u reason=capture begin\n",
                               frame);
        return CHAIN_CALLBACK_RESULT_EXIT_GAME_ERROR;
    }

    SideEffects::SetSpeculative(true);
    Input::BeginCapture();
    const int speculativeResult = g_Chain.RunCalcChain();
    const Input::FrameInput input = Input::EndCapture();
    SideEffects::SetSpeculative(false);

    if (!Th07Rollback::EndFrame())
        return FailAfterSpeculative("capture end", frame);

    const std::uint64_t speculativeHash = Th07Rollback::DebugStateHash();
    const std::size_t bytes = Th07Rollback::CapturedBytes(frame);
    const std::size_t blocks = Th07Rollback::CapturedBlocks(frame);
    const std::size_t bombs = Th07Rollback::CapturedBombEffects(frame);

    if (!Th07Rollback::RestoreTo(frame))
        return FailAfterSpeculative("restore", frame);

    Input::SetReplayOverride(input);
    const int committedResult = g_Chain.RunCalcChain();
    Input::ClearReplayOverride();
    const std::uint64_t committedHash = Th07Rollback::DebugStateHash();

    if (speculativeResult != committedResult || speculativeHash != committedHash)
    {
        Supervisor::DebugPrint(
            "netplay rollback probe: FAIL frame=%u stage=%d result=%d/%d hash=%016llx/%016llx bytes=%llu blocks=%llu bombs=%llu analog=%u\n",
            frame, stage, speculativeResult, committedResult,
            static_cast<unsigned long long>(speculativeHash),
            static_cast<unsigned long long>(committedHash),
            static_cast<unsigned long long>(bytes),
            static_cast<unsigned long long>(blocks),
            static_cast<unsigned long long>(bombs),
            static_cast<unsigned>(input.analogMode));
        return CHAIN_CALLBACK_RESULT_EXIT_GAME_ERROR;
    }

    Supervisor::DebugPrint(
        "netplay rollback probe: PASS frame=%u stage=%d hash=%016llx bytes=%llu blocks=%llu bombs=%llu analog=%u\n",
        frame, stage, static_cast<unsigned long long>(committedHash),
        static_cast<unsigned long long>(bytes),
        static_cast<unsigned long long>(blocks),
        static_cast<unsigned long long>(bombs),
        static_cast<unsigned>(input.analogMode));
    return committedResult;
}
} // namespace Netplay::Th07RollbackProbe
