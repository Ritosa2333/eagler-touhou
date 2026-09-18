#include "Th07DeterminismProbe.hpp"

#include <eagler/netplay/NetplayInput.hpp>
#include "Th07CanonicalHash.hpp"

#include "Controller.hpp"
#include "GameManager.hpp"
#include "ReplayManager.hpp"
#include "Supervisor.hpp"

#include <cstdint>
#include <cstdio>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

namespace Netplay::Th07DeterminismProbe
{
namespace
{
constexpr std::uint32_t SAMPLE_INTERVAL = 30;
constexpr std::uint32_t TEST_FRAMES = 300;

bool g_Started = false;
bool g_Finished = false;
bool g_OverrideActive = false;
std::uint32_t g_Frame = 0;

bool Requested()
{
#ifdef __EMSCRIPTEN__
    return EM_ASM_INT({
        return Module.eaglerOptions?.debugHarness === 'netplay-dual-stage1' ? 1 : 0;
    }) != 0;
#else
    return false;
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

std::uint16_t ScriptButtons(std::uint32_t frame)
{
    // Deliberately exercises held input, direction changes and focus without
    // introducing menu/pause or bomb side effects into this first cross-Runtime
    // gate. The script is keyed only by logical simulation frame.
    std::uint16_t buttons = TH_BUTTON_SHOOT;
    if (frame >= 45 && frame < 105)
        buttons |= TH_BUTTON_LEFT;
    else if (frame >= 105 && frame < 165)
        buttons |= TH_BUTTON_RIGHT | TH_BUTTON_FOCUS;
    else if (frame >= 165 && frame < 225)
        buttons |= TH_BUTTON_UP;
    else if (frame >= 225 && frame < 285)
        buttons |= TH_BUTTON_DOWN | TH_BUTTON_FOCUS;
    return buttons;
}

void ToHex(std::uint64_t value, char out[17])
{
    std::snprintf(out, 17, "%016llx", static_cast<unsigned long long>(value));
}

void EmitSample(std::uint32_t frame, const Th07CanonicalHash::Sample &sample)
{
    char meta[17], stage[17], player[17], enemies[17], bullets[17], items[17], composite[17];
    ToHex(sample.meta, meta);
    ToHex(sample.stage, stage);
    ToHex(sample.player, player);
    ToHex(sample.enemies, enemies);
    ToHex(sample.bullets, bullets);
    ToHex(sample.items, items);
    ToHex(sample.composite, composite);

    std::printf(
        "netplay dual probe: SAMPLE frame=%u hash=%s meta=%s stage=%s player=%s enemies=%s bullets=%s items=%s counts=%u/%u/%u/%u\n",
        frame, composite, meta, stage, player, enemies, bullets, items,
        sample.enemyCount, sample.bulletCount, sample.laserCount, sample.itemCount);

}
} // namespace

void BeforeSimulationTick()
{
    if (!Requested() || g_Finished || !EligibleGameplayState())
        return;

    if (!g_Started)
    {
        g_Started = true;
        g_Frame = 0;
        std::printf("netplay dual probe: START seed=%u generation=%u stage=%d\n",
                    static_cast<unsigned>(g_Rng.seed),
                    static_cast<unsigned>(g_Rng.generationCount),
                    static_cast<int>(g_GameManager.currentStage));
    }

    Input::FrameInput input;
    input.buttons = ScriptButtons(g_Frame);
    Input::SetReplayOverride(input);
    g_OverrideActive = true;
}

void AfterSimulationTick(int chainResult)
{
    if (!g_OverrideActive)
        return;

    Input::ClearReplayOverride();
    g_OverrideActive = false;

    if (chainResult == 0 || chainResult == -1)
    {
        std::printf("netplay dual probe: FAIL frame=%u chain=%d\n", g_Frame, chainResult);
        g_Finished = true;
        return;
    }

    if ((g_Frame % SAMPLE_INTERVAL) == 0 || g_Frame + 1 == TEST_FRAMES)
        EmitSample(g_Frame, Th07CanonicalHash::Capture());

    ++g_Frame;
    if (g_Frame >= TEST_FRAMES)
    {
        g_Finished = true;
        std::printf("netplay dual probe: DONE frames=%u\n", TEST_FRAMES);
#ifdef __EMSCRIPTEN__
        EM_ASM({ globalThis.__eaglerNetplayDualDone = true; });
#endif
    }
}
} // namespace Netplay::Th07DeterminismProbe
