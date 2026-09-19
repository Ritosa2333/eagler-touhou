#include "GameWindow.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_video.h>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#endif

#include "AnmManager.hpp"
#include "Chain.hpp"
#include "Controller.hpp"
#include "EaglerOptions.hpp"
#include "FileSystem.hpp"
#include "GameErrorContext.hpp"
#include "GameManager.hpp"
#include "PracticeRuntime.hpp"
#include "ScreenEffect.hpp"
#include "SoundPlayer.hpp"
#include "Stage.hpp"
#include "Supervisor.hpp"
#include "graphics/Gles.hpp"
#include "graphics/ZunGraphics.hpp"
#ifdef TH_ENABLE_THPRAC
#include "ThpracImGui.hpp"
#endif
#ifdef TH_ENABLE_NETPLAY
#include <eagler/netplay/FrameBudget.hpp>
#include "netplay/Th07DeterminismProbe.hpp"
#include "netplay/Th07LanCoreProbe.hpp"
#include "netplay/Th07LanStageProbe.hpp"
#include "netplay/Th07RollbackProbe.hpp"
#endif

#if !defined(__EMSCRIPTEN__)
static f64 GetNativePresentationHz()
{
    SDL_DisplayID display = SDL_GetDisplayForWindow(g_GameWindow.window);
    const SDL_DisplayMode *mode = display ? SDL_GetCurrentDisplayMode(display) : nullptr;
    f64 hz = mode ? (f64)mode->refresh_rate : 0.0;
    if (hz < 30.0 || hz > 1000.0)
        hz = 60.0;
    return hz;
}

struct NativePerfTelemetryState
{
    bool initialized;
    bool enabled;
    u64 windowStartNs;
    u32 callbacks;
    u32 updates;
    u32 draws;
    u64 calcNs;
    u64 drawNs;
    u64 maxDrawNs;
};

static NativePerfTelemetryState g_NativePerfTelemetry;

static void ReportNativePerfTelemetry(u64 nowNs)
{
    auto &perf = g_NativePerfTelemetry;
    if (!perf.initialized)
    {
        perf.initialized = true;
        const char *value = std::getenv("EAGLER_NATIVE_PERF");
        perf.enabled = value && value[0] == '1';
        perf.windowStartNs = nowNs;
    }
    if (!perf.enabled || nowNs - perf.windowStartNs < 1000000000ULL)
        return;

    const GlesNativePerfCounters glPerf = GlesTakeNativePerfCounters();
    Supervisor::DebugPrint(
        "native-perf callbacks=%u updates=%u draws=%u calc_ms=%.3f draw_ms=%.3f max_draw_ms=%.3f vsync=%d display_hz=%.3f gl_draws=%llu gl_vertices=%llu subdata=%llu subdata_bytes=%llu bufferdata=%llu binds=%llu uniforms=%llu swaps=%llu\n",
        perf.callbacks, perf.updates, perf.draws,
        (double)perf.calcNs / 1000000.0, (double)perf.drawNs / 1000000.0,
        (double)perf.maxDrawNs / 1000000.0, g_Supervisor.vsyncDisabled,
        (double)GetNativePresentationHz(),
        (unsigned long long)glPerf.drawCalls, (unsigned long long)glPerf.drawVertices,
        (unsigned long long)glPerf.bufferSubDataCalls,
        (unsigned long long)glPerf.bufferSubDataBytes,
        (unsigned long long)glPerf.bufferDataCalls,
        (unsigned long long)glPerf.bindTextureCalls,
        (unsigned long long)glPerf.uniformCalls,
        (unsigned long long)glPerf.swapCalls);

    const bool enabled = perf.enabled;
    perf = {};
    perf.initialized = true;
    perf.enabled = enabled;
    perf.windowStartNs = nowNs;
}
#endif

GameWindow g_GameWindow;
i32 g_FrameCount;
f64 g_LastFrameTime;
u64 g_LastPerfCounter;
f32 g_RenderAlpha = 1.0f;
bool g_SuppressAnmAdvance;
#ifdef TH_DEV_TOOLS
f32 g_DevSpeedMultiplier = 1.0f;
#endif

#ifdef TH_ENABLE_THCRAP
static i32 g_ThcrapSnapshotRequests = 0;
#endif

#ifdef _WIN32
static RECT g_WindowedRect;
static int g_WindowedClientW;
static int g_WindowedClientH;
static LONG g_WindowedStyle;
static bool g_InBorderlessFullscreen = false;
#endif

static GfxInit g_RenderingBackends[] = {
    GlesGraphics::Init,
};

void GameWindow::Present()
{
    char snapshotPath[252];
    i32 i;

#ifdef TH_ENABLE_THCRAP
    if (g_ThcrapSnapshotRequests > 0)
    {
        --g_ThcrapSnapshotRequests;
        std::filesystem::create_directory(FileSystem::GetPrefPath("snapshot"));
        for (i = 0; i < 1000; i++)
        {
            sprintf(snapshotPath, "snapshot/th%.3d.png", i);
            if (FileSystem::CheckFileExists(snapshotPath) == 0)
                break;
        }
        if (i < 1000)
            g_Supervisor.SnapshotPng(snapshotPath);
    }
#endif

    g_Supervisor.gfxDevice->SwapBuffers();

    if (WAS_PRESSED_RAW(TH_BUTTON_HOME))
    {
        std::filesystem::create_directory(FileSystem::GetPrefPath("snapshot"));
        for (i = 0; i < 1000; i++)
        {
            sprintf(snapshotPath, "snapshot/th%.3d.bmp", i);
            if (FileSystem::CheckFileExists(snapshotPath) == 0)
            {
                break;
            }
        }
        if (i < 1000)
        {
            g_Supervisor.SnapshotScreen(snapshotPath);
        }
    }
}

RenderResult GameWindow::Render()
{
#ifdef __EMSCRIPTEN__
    static std::uint32_t s_DebugWebRenderCallbacks = 0;
    ++s_DebugWebRenderCallbacks;
    if ((s_DebugWebRenderCallbacks & 15u) == 0u)
    {
        EM_ASM({
            globalThis.__eaglerDebugRenderCallbacks = $0;
            globalThis.__eaglerDebugAppActive = !!$1;
            globalThis.__eaglerDebugAccumulator = $2;
        }, s_DebugWebRenderCallbacks, this->isAppActive ? 1 : 0, this->accumulator);
    }
#endif
#if !defined(__EMSCRIPTEN__)
    const u64 nativePerfNowNs = SDL_GetTicksNS();
    ReportNativePerfTelemetry(nativePerfNowNs);
    if (g_NativePerfTelemetry.enabled)
        g_NativePerfTelemetry.callbacks++;
#endif

    if (!this->isAppActive)
    {
#ifndef __EMSCRIPTEN__
        SDL_WaitEventTimeout(nullptr, 1000);
#endif
        return RENDER_RESULT_KEEP_RUNNING;
    }

    GameWindow::RememberWindowedState();

    const f64 baseTargetDt = 1.0 / 60.0;
#ifdef TH_ENABLE_NETPLAY
    const f64 targetDt = baseTargetDt * Netplay::Th07LanStageProbe::SimulationIntervalScale();
#else
    const f64 targetDt = baseTargetDt;
#endif
#ifdef __EMSCRIPTEN__
    if ((s_DebugWebRenderCallbacks & 15u) == 0u)
        EM_ASM({ globalThis.__eaglerDebugTargetDt = $0; }, targetDt);
#endif

    u64 currentPerfCounter = SDL_GetPerformanceCounter();
    if (g_LastPerfCounter == 0)
    {
        g_LastPerfCounter = currentPerfCounter;
    }

    f64 elapsed = (f64)(currentPerfCounter - g_LastPerfCounter) / (f64)g_GameWindow.frequency;
    g_LastPerfCounter = currentPerfCounter;

    if (elapsed < 0.0)
    {
        elapsed = 0.0;
    }
    if (elapsed > 0.1)
    {
        elapsed = 0.1;
    }

    this->accumulator += elapsed;
#ifdef __EMSCRIPTEN__
    const bool limitPresentationTo60 = EaglerOptions::LimitPresentationTo60();
#else
    constexpr bool limitPresentationTo60 = false;
#endif
    const bool preserveReplayCadence = g_GameManager.replay != 0;
#ifdef TH_DEV_TOOLS
    if (g_DevSpeedMultiplier > 1.0f)
        this->accumulator += elapsed * (g_DevSpeedMultiplier - 1.0f);
#endif

    i32 chainRes = CHAIN_CALLBACK_RESULT_CONTINUE;
    bool updated = false;
    bool lastSimulationTickAdvanced = true;

#if defined(__EMSCRIPTEN__) && defined(TH_ENABLE_NETPLAY)
    const bool netplayPerf = Netplay::Th07LanStageProbe::PerformanceTelemetryEnabled();
    const u64 netplayDriverStartNs = netplayPerf ? SDL_GetTicksNS() : 0;
#endif
#ifndef __EMSCRIPTEN__
    u64 timeToRender = SDL_GetTicksNS();
#endif

    const auto runSimulationTick = [&]() -> i32
    {
#ifndef __EMSCRIPTEN__
        const u64 calcStartNs = g_NativePerfTelemetry.enabled ? SDL_GetTicksNS() : 0;
#endif
#ifdef TH_ENABLE_NETPLAY
        Netplay::Th07DeterminismProbe::BeforeSimulationTick();
        if (Netplay::Th07LanStageProbe::Requested())
        {
            chainRes = Netplay::Th07LanStageProbe::RunCalcChain();
            lastSimulationTickAdvanced = Netplay::Th07LanStageProbe::LastTickAdvanced();
        }
        else
        {
            chainRes = Netplay::Th07RollbackProbe::RunCalcChain();
            lastSimulationTickAdvanced = true;
        }
        Netplay::Th07DeterminismProbe::AfterSimulationTick(chainRes);
        Netplay::Th07LanCoreProbe::OnSimulationTick();
#else
        chainRes = g_Chain.RunCalcChain();
#endif
#ifdef TH_ENABLE_THPRAC
        // th07_update @ 0x42fdf8 is a one-byte hook in the tail of
        // Chain::RunCalcChain (0x42fd60..0x42fe20), matching th07_render's
        // tail hook in RunDrawChain. Trainer GUI/hotkey producers therefore
        // run after this tick's game consumers, not before them.
        PracticeRuntime::UpdateOverlay();
#endif
#ifdef TH_ENABLE_THCRAP
        const bool *keyboard = SDL_GetKeyboardState(NULL);
        if (keyboard != nullptr && keyboard[SDL_SCANCODE_P] && g_ThcrapSnapshotRequests < 1000)
            ++g_ThcrapSnapshotRequests;
#endif
        if (lastSimulationTickAdvanced)
            g_SoundPlayer.ProcessQueues();

#ifndef __EMSCRIPTEN__
        if (g_NativePerfTelemetry.enabled)
        {
            g_NativePerfTelemetry.calcNs += SDL_GetTicksNS() - calcStartNs;
            g_NativePerfTelemetry.updates++;
        }
#endif
        return chainRes;
    };

#ifdef TH_ENABLE_NETPLAY
    if (Netplay::Th07LanStageProbe::Requested())
    {
        // A stalled netplay tick has not consumed simulation time. Keep a
        // bounded wall-clock backlog while presentation continues, then catch
        // up several fixed ticks before the next draw once packets resume.
        // This prevents a brief network stall from becoming lasting slow time.
        // A tick may itself replay up to the rollback window. A count-only
        // limit can therefore monopolize the browser long after its render
        // deadline. Keep the original tick bound and also yield on elapsed
        // cost; unconsumed accumulator time remains for the next callback.
        constexpr i32 maxNetplayCatchupTicks =
            static_cast<i32>(Netplay::FrameBudget::MaxCatchupTicks);
        const u64 catchupStartNs = SDL_GetTicksNS();
        i32 catchupTicks = 0;
        while (this->accumulator >= targetDt && Netplay::FrameBudget::CanStartTick(
                   static_cast<std::uint32_t>(catchupTicks), SDL_GetTicksNS() - catchupStartNs))
        {
            const i32 res = runSimulationTick();
            if (res == 0)
                return RENDER_RESULT_EXIT_SUCCESS;
            if (res == -1)
                return RENDER_RESULT_EXIT_ERROR;
            if (!lastSimulationTickAdvanced)
            {
                this->accumulator = std::min(
                    this->accumulator, targetDt * (f64)maxNetplayCatchupTicks);
                break;
            }
            this->accumulator -= targetDt;
            updated = true;
            ++catchupTicks;
        }
    }
    else
#endif
    if (limitPresentationTo60 || preserveReplayCadence)
    {
        if (this->accumulator >= targetDt)
        {
            // Original TH07 does the same thing as TH06 here: advance its
            // timing baseline past every overdue 1/60 interval, then run the
            // game chains only once. Catching up multiple simulation ticks in
            // one visible frame is a portable-only behavior and is especially
            // obvious on fast bullets.
            do
            {
                this->accumulator -= targetDt;
            } while (this->accumulator >= targetDt);

            const i32 res = runSimulationTick();
            if (res == 0)
                return RENDER_RESULT_EXIT_SUCCESS;
            if (res == -1)
                return RENDER_RESULT_EXIT_ERROR;
            updated = updated || lastSimulationTickAdvanced;
        }
    }
    else
    {
        while (this->accumulator >= targetDt)
        {
            const i32 res = runSimulationTick();
            if (res == 0)
                return RENDER_RESULT_EXIT_SUCCESS;
            if (res == -1)
                return RENDER_RESULT_EXIT_ERROR;
            this->accumulator -= targetDt;
            updated = updated || lastSimulationTickAdvanced;
        }
    }

#ifdef __EMSCRIPTEN__
    // Audio commands remain owned by the fixed 60 Hz simulation tick, but the
    // SDL/WebAudio output queue is maintained at presentation cadence. This
    // keeps streaming decode slices small without tying underrun protection to
    // the simulation rate on 120+ Hz displays.
    if (!g_SoundPlayer.PumpWebAudio())
    {
        SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "th07: SDL audio pump failed: %s", SDL_GetError());
    }

#ifdef TH_ENABLE_NETPLAY
    // Incremental rollback can yield after RestoreTo() but before historical
    // replay reaches the old live frontier. Keep the previous complete swap on
    // screen instead of drawing this temporary historical state. The netplay
    // driver temporarily switches Emscripten's main-loop timing to a short
    // timer while reconciliation is active, so no visual swap is needed just
    // to request the next historical replay slice.
    if (Netplay::Th07LanStageProbe::ReconciliationInProgress())
        return RENDER_RESULT_KEEP_RUNNING;
#endif

    // Simulation stays fixed at 60 Hz, while Web presentation normally follows
    // requestAnimationFrame at the display refresh rate. The optional 60 FPS
    // mode follows the original one-tick-per-picture behavior above and skips
    // callbacks with no new simulation tick; with it disabled, high-refresh
    // catch-up + interpolation/presentation remains available.
    if (limitPresentationTo60 && !updated)
    {
        return RENDER_RESULT_KEEP_RUNNING;
    }
#endif

    g_RenderAlpha = std::clamp((f32)(this->accumulator / targetDt), 0.0f, 1.0f);
#ifdef __EMSCRIPTEN__
    // A 60 Hz presentation cap draws only on simulation ticks, so there is no
    // intermediate frame to interpolate. Using the residual accumulator alpha
    // on those retained frames produces uneven visual step sizes whenever RAF
    // is not phase-locked to 60 Hz. Draw the current simulation state instead.
    if (limitPresentationTo60)
    {
        g_RenderAlpha = 1.0f;
    }
#endif
    if (g_GameManager.isInPauseMenu || g_GameManager.isInRetryMenu)
    {
        g_RenderAlpha = 1.0f;
    }

#ifndef __EMSCRIPTEN__
    const u64 drawStartNs = g_NativePerfTelemetry.enabled ? SDL_GetTicksNS() : 0;
#endif
#if defined(__EMSCRIPTEN__) && defined(TH_ENABLE_NETPLAY)
    const u64 netplayDrawStartNs = netplayPerf ? SDL_GetTicksNS() : 0;
#endif

    g_Supervisor.gfxDevice->BeginFrame();
    g_AnmManager->ResetVertexBuffer();
    g_Supervisor.fogEnabled = 255;
    g_Supervisor.DisableFog();

    // Pause/retry menu drawing restores the full 640x480 viewport at the end
    // of the previous draw. On high-refresh presentation-only frames there is
    // no intervening GameManager::OnUpdate() to put the playfield viewport
    // back, so stage/background rendering can alternate between two viewports.
    // Re-establish the authoritative playfield viewport for every paused draw.
    if (g_GameManager.isInPauseMenu || g_GameManager.isInRetryMenu)
    {
        g_Supervisor.viewport.x = g_GameManager.arcadeRegionTopLeftPos.x;
        g_Supervisor.viewport.y = g_GameManager.arcadeRegionTopLeftPos.y;
        g_Supervisor.viewport.width = g_GameManager.arcadeRegionSize.x;
        g_Supervisor.viewport.height = g_GameManager.arcadeRegionSize.y;
        g_Supervisor.viewport.minZ = 0.0f;
        g_Supervisor.viewport.maxZ = 1.0f;
        g_Supervisor.gfxDevice->SetViewport(g_Supervisor.viewport);
    }

    g_SuppressAnmAdvance = !updated;
#ifdef TH_ENABLE_THPRAC
    // GameGuiBegin(..., !THAdvOptWnd::IsOpen()): Advanced Options owns game
    // navigation exclusively while open.
    ThpracImGui::SetGameNavEnabled(!PracticeRuntime::AdvancedOptionsOpen());
    ThpracImGui::SetGameInput(g_CurFrameRawInput, updated);
    if (updated)
        ThpracImGui::BeginFrame(static_cast<f32>(targetDt));
#endif
    if (g_AnmManager)
    {
        g_AnmManager->offset =
            g_AnmManager->prevShakeOffset.Lerp(g_AnmManager->shakeOffset, g_RenderAlpha);
    }
    g_Chain.RunDrawChain();
    g_SuppressAnmAdvance = false;

    g_AnmManager->Flush();
#ifdef TH_ENABLE_THPRAC
    // THOverlay / Tracker / THAdvOptWnd are not owned by any vanilla draw-chain
    // element. Build them into the same ImGui frame after the game scene, then
    // submit that frame once through the GLES backend.
    PracticeRuntime::DrawOverlay();
    if (ThpracImGui::IsFrameOpen())
        ThpracImGui::EndFrame();
    if (g_Supervisor.gfxDevice && g_Supervisor.gfxDevice->GetType() == RENDERER_OPENGLES)
        static_cast<GlesGraphics *>(g_Supervisor.gfxDevice)->RenderImGui(ThpracImGui::GetDrawData());
#endif
    g_Supervisor.gfxDevice->BindTexture({0});
    g_Supervisor.gfxDevice->EndFrame();

    Present();

#if defined(__EMSCRIPTEN__) && defined(TH_ENABLE_NETPLAY)
    if (netplayPerf)
        Netplay::Th07LanStageProbe::RecordPresentationCost(
            (netplayDrawStartNs - netplayDriverStartNs) / 1000000.0,
            (SDL_GetTicksNS() - netplayDrawStartNs) / 1000000.0);
#endif
#ifdef __EMSCRIPTEN__
    // One-shot observability only: distinguish "runtime loaded" from "the
    // game actually presented a frame" without altering either cadence.
    static bool s_FirstWebFrameReported = false;
    if (!s_FirstWebFrameReported)
    {
        s_FirstWebFrameReported = true;
        EM_ASM({ globalThis.EaglerTouhouFirstFrame?.(); });
    }

    // Presentation-only diagnostics. These counters observe completed swaps
    // and never influence the fixed 60 Hz simulation or Replay determinism.
    static f64 s_WebFrameHealthStartMs = 0.0;
    static f64 s_WebLastPresentMs = 0.0;
    static f64 s_WebMaxPresentGapMs = 0.0;
    static u32 s_WebPresentedFrames = 0;
    const f64 presentNowMs = emscripten_get_now();
    if (s_WebFrameHealthStartMs == 0.0)
        s_WebFrameHealthStartMs = presentNowMs;
    if (s_WebLastPresentMs > 0.0)
        s_WebMaxPresentGapMs = std::max(s_WebMaxPresentGapMs, presentNowMs - s_WebLastPresentMs);
    s_WebLastPresentMs = presentNowMs;
    s_WebPresentedFrames++;
    const f64 frameHealthWindowMs = presentNowMs - s_WebFrameHealthStartMs;
    if (frameHealthWindowMs >= 500.0)
    {
        const f64 presentationFps = static_cast<f64>(s_WebPresentedFrames) * 1000.0 / frameHealthWindowMs;
        EM_ASM({ globalThis.EaglerTouhouFrameHealth?.($0, $1); }, presentationFps, s_WebMaxPresentGapMs);
        s_WebFrameHealthStartMs = presentNowMs;
        s_WebMaxPresentGapMs = 0.0;
        s_WebPresentedFrames = 0;
    }
#endif

#ifndef __EMSCRIPTEN__
    if (g_NativePerfTelemetry.enabled)
    {
        const u64 drawNs = SDL_GetTicksNS() - drawStartNs;
        g_NativePerfTelemetry.draws++;
        g_NativePerfTelemetry.drawNs += drawNs;
        g_NativePerfTelemetry.maxDrawNs = std::max(g_NativePerfTelemetry.maxDrawNs, drawNs);
    }
#endif

    if (updated)
    {
        g_FrameCount++;
    }

#ifndef __EMSCRIPTEN__
    timeToRender = SDL_GetTicksNS() - timeToRender;
    // Keep the original 60 Hz simulation, but pace presentation to the actual
    // display refresh rate. SDL_GL_GetSwapInterval(1) is only a request: some
    // GLES translation layers report it as enabled while returning from swap
    // hundreds of times per second. If swap already consumed the display-frame
    // budget, this adds no delay; otherwise it prevents runaway duplicate draws
    // while preserving high-refresh interpolation/presentation.
    const f64 presentationHz = GetNativePresentationHz();
    const u64 presentationFrameNs = (u64)(1000000000.0 / presentationHz);
    if (timeToRender < presentationFrameNs)
    {
        SDL_DelayPrecise(presentationFrameNs - timeToRender);
    }
#else
    // SDL's browser callback already runs from requestAnimationFrame. Return
    // immediately so network/input/audio tasks can run; the imported desktop
    // vsync flag must never add a synchronous sleep to the browser main thread.
    // LimitPresentationTo60 above caps presentation without blocking, and the
    // fixed-step accumulator remains the sole owner of logical game time.
#endif

    return RENDER_RESULT_KEEP_RUNNING;
}

ZunResult GameWindow::InitInterface()
{
    for (auto gfxInit : g_RenderingBackends)
    {
        g_Supervisor.gfxDevice = gfxInit();
        if (g_Supervisor.gfxDevice)
        {
            g_Supervisor.hasLockableBackbuffer = 1;
            g_Supervisor.lockableBackBuffer = 1;
            return ZUN_SUCCESS;
        }
    }

    g_GameErrorContext.Fatal("Direct3D オブジェクトは何故か作成出来なかった\n");
    return ZUN_ERROR;
}

ZunResult GameWindow::CreateGameWindow()
{
    SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "0");
    SDL_SetHint(SDL_HINT_MOUSE_TOUCH_EVENTS, "0");

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        g_GameErrorContext.Fatal("Direct3D オブジェクトは何故か作成出来なかった\n");
        return ZUN_ERROR;
    }

    SDL_PropertiesID props = SDL_CreateProperties();
    if (!props)
    {
        Supervisor::DebugPrint("SDL_CreateProperties failed: %s\n", SDL_GetError());
        return ZUN_ERROR;
    }

    SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING,
                          "東方妖々夢　〜 Perfect Cherry Blossom. ver 1.00b");
    SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_OPENGL_BOOLEAN, true);
#if defined(__ANDROID__) || (defined(__APPLE__) && TARGET_OS_IPHONE)
    SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_FULLSCREEN_BOOLEAN, true);
    SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_HIGH_PIXEL_DENSITY_BOOLEAN, true);
#elif defined(__EMSCRIPTEN__)
    SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_HIGH_PIXEL_DENSITY_BOOLEAN, true);
#else
    // Create a normal desktop window first. Saved fullscreen mode is applied
    // after SDL has created and synchronized the native window, using the same
    // borderless path as the runtime Alt+Enter toggle.
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, 640);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, 480);
#endif

    g_GameWindow.isAppActive = 1;
    g_LastPerfCounter = SDL_GetPerformanceCounter();

#ifdef USING_GL
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 0);

    g_GameWindow.window = SDL_CreateWindowWithProperties(props);
    SDL_DestroyProperties(props);
    if (!g_GameWindow.window)
    {
        Supervisor::DebugPrint("sdl window create failed: %s\n", SDL_GetError());
        return ZUN_ERROR;
    }

    SDL_ShowWindow(g_GameWindow.window);
    SDL_SyncWindow(g_GameWindow.window);
    GameWindow::RememberWindowedState();

#if !defined(__ANDROID__) && !(defined(__APPLE__) && TARGET_OS_IPHONE) && !defined(__EMSCRIPTEN__)
    if (!g_Supervisor.cfg.windowed)
    {
        GameWindow::ToggleFullscreen();
    }
#endif

    SDL_RaiseWindow(g_GameWindow.window);
    return ZUN_SUCCESS;
}

void GameWindow::RememberWindowedState()
{
#ifdef _WIN32
    if (!g_GameWindow.window || g_InBorderlessFullscreen)
    {
        return;
    }

    HWND hwnd = (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(g_GameWindow.window),
                                             SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
    if (!hwnd)
    {
        return;
    }

    RECT wr;
    RECT clientRect;
    const LONG style = GetWindowLong(hwnd, GWL_STYLE);
    // Mesa/DXGI can process Alt+Enter before SDL delivers the key event to the
    // application and temporarily strip the caption. Never let that transient
    // state overwrite the last authoritative captioned-window geometry.
    if ((style & WS_CAPTION) == 0 || !GetWindowRect(hwnd, &wr) || !GetClientRect(hwnd, &clientRect))
    {
        return;
    }

    g_WindowedRect = wr;
    g_WindowedClientW = clientRect.right - clientRect.left;
    g_WindowedClientH = clientRect.bottom - clientRect.top;
    g_WindowedStyle = style;
#endif
}

void GameWindow::ToggleFullscreen()
{
    if (!g_GameWindow.window)
    {
#ifdef TH_DEV_TOOLS
        SDL_Log("th07 window: toggle aborted (no SDL window)");
#endif
        return;
    }

#ifdef _WIN32
    HWND hwnd = (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(g_GameWindow.window),
                                             SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
    if (!hwnd)
    {
#ifdef TH_DEV_TOOLS
        SDL_Log("th07 window: toggle aborted (no Win32 HWND)");
#endif
        return;
    }

    if (!g_InBorderlessFullscreen)
    {
        // Refresh only if the native window is still captioned. On Mesa/DXGI,
        // Alt+Enter may already have stripped the style before our SDL event is
        // dispatched, in which case RememberWindowedState intentionally keeps
        // the pre-key cached geometry from create/focus/render time.
        GameWindow::RememberWindowedState();
#ifdef TH_DEV_TOOLS
        SDL_Log("th07 window: pre-toggle style=0x%08lx "
                "cachedOuter=(%ld,%ld)-(%ld,%ld)",
                static_cast<unsigned long>(GetWindowLong(hwnd, GWL_STYLE)), g_WindowedRect.left, g_WindowedRect.top,
                g_WindowedRect.right, g_WindowedRect.bottom);
#endif
        if (g_WindowedRect.right == 0 && g_WindowedRect.bottom == 0)
        {
#ifdef TH_DEV_TOOLS
            SDL_Log("th07 window: toggle aborted (no valid windowed geometry)");
#endif
            return;
        }

        SDL_DisplayID disp = SDL_GetDisplayForWindow(g_GameWindow.window);
        SDL_Rect dispBounds;
        if (!disp || !SDL_GetDisplayBounds(disp, &dispBounds))
        {
#ifdef TH_DEV_TOOLS
            SDL_Log("th07 window: toggle aborted (display lookup failed: id=%u error=%s)",
                    static_cast<unsigned>(disp), SDL_GetError());
#endif
            return;
        }
        const SDL_DisplayMode *dm = SDL_GetDesktopDisplayMode(disp);
        const int fsW = (dm && dm->w > 0) ? dm->w : dispBounds.w;
        const int fsH = (dm && dm->h > 0) ? dm->h : dispBounds.h;

        SetWindowLong(hwnd, GWL_STYLE,
                      (g_WindowedStyle & ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX |
                                           WS_MAXIMIZEBOX | WS_SYSMENU)) |
                          WS_POPUP);
        SDL_SetWindowSize(g_GameWindow.window, fsW, fsH);
        SDL_SetWindowPosition(g_GameWindow.window, dispBounds.x, dispBounds.y);
        g_InBorderlessFullscreen = true;
        SDL_HideCursor();
#ifdef TH_DEV_TOOLS
        SDL_Log("th07 window: entered borderless fullscreen %dx%d at %d,%d", fsW, fsH,
                dispBounds.x, dispBounds.y);
#endif
    }
    else
    {
        SetWindowLong(hwnd, GWL_STYLE, g_WindowedStyle);
        // Apply the caption/frame before asking SDL to restore the client
        // size. If style restoration and resize are combined, Windows can
        // apply the non-client frame asynchronously afterwards and preserve
        // the temporary borderless client size instead (e.g. 646x526), which
        // leaves the game larger than its authoritative 640x480 client area.
        SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
                     SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                         SWP_NOACTIVATE);
        SDL_SyncWindow(g_GameWindow.window);
        SDL_SetWindowSize(g_GameWindow.window, g_WindowedClientW, g_WindowedClientH);
        SDL_SetWindowPosition(g_GameWindow.window, g_WindowedRect.left, g_WindowedRect.top);
        SDL_SyncWindow(g_GameWindow.window);
        SetWindowPos(hwnd, HWND_TOP, g_WindowedRect.left, g_WindowedRect.top,
                     g_WindowedRect.right - g_WindowedRect.left,
                     g_WindowedRect.bottom - g_WindowedRect.top,
                     SWP_FRAMECHANGED | SWP_NOACTIVATE | SWP_SHOWWINDOW);
        SDL_SyncWindow(g_GameWindow.window);
        g_InBorderlessFullscreen = false;
        SDL_ShowCursor();
#ifdef TH_DEV_TOOLS
        SDL_Log("th07 window: restored windowed client %dx%d outer=(%ld,%ld)-(%ld,%ld)",
                g_WindowedClientW, g_WindowedClientH, g_WindowedRect.left, g_WindowedRect.top,
                g_WindowedRect.right, g_WindowedRect.bottom);
#endif
    }
#else
    const bool fullscreen = (SDL_GetWindowFlags(g_GameWindow.window) & SDL_WINDOW_FULLSCREEN) != 0;
    SDL_SetWindowFullscreen(g_GameWindow.window, !fullscreen);
    if (fullscreen)
        SDL_ShowCursor();
    else
        SDL_HideCursor();
#endif
}

bool GameWindow::IsFullscreen()
{
    if (!g_GameWindow.window)
    {
        return false;
    }
#ifdef _WIN32
    return g_InBorderlessFullscreen;
#else
    return (SDL_GetWindowFlags(g_GameWindow.window) & SDL_WINDOW_FULLSCREEN) != 0;
#endif
}

ZunResult GameWindow::InitRendering()
{
    ZunVec3 pEye;
    ZunVec3 pAt;
    ZunVec3 pUp;
    f32 fov;
    f32 aspectRatio;
    f32 halfWidth;
    f32 halfHeight;
    f32 halfCameraDistance;

    halfWidth = 320.0f;
    halfHeight = 240.0f;
    aspectRatio = 4.0f / 3.0f;
    fov = ZUN_PI / 6.0f;
    halfCameraDistance = halfHeight / tanf(fov / 2.0f);
    pUp.x = 0.0f;
    pUp.y = 1.0f;
    pUp.z = 0.0f;
    pAt.x = halfWidth;
    pAt.y = -halfHeight;
    pAt.z = 0.0f;
    pEye.x = halfWidth;
    pEye.y = -halfHeight;
    pEye.z = -halfCameraDistance;
    g_Supervisor.viewMatrix.LookAtLH(&pEye, &pAt, &pUp);
    g_Supervisor.projectionMatrix.PerspectiveFovLH(fov, aspectRatio, 100.0f, 10000.0f);
    g_Supervisor.viewProjectionMatrix = g_Supervisor.viewMatrix * g_Supervisor.projectionMatrix;

    g_Supervisor.gfxDevice->SetTransformMatrix(MATRIX_VIEW, g_Supervisor.viewMatrix);
    g_Supervisor.gfxDevice->SetTransformMatrix(MATRIX_PROJECTION, g_Supervisor.projectionMatrix);

    g_Supervisor.viewport.x = 0;
    g_Supervisor.viewport.y = 0;
    g_Supervisor.viewport.width = GAME_WINDOW_WIDTH;
    g_Supervisor.viewport.height = GAME_WINDOW_HEIGHT;
    g_Supervisor.viewport.minZ = 0.0f;
    g_Supervisor.viewport.maxZ = 1.0f;
    g_Supervisor.gfxDevice->SetViewport(g_Supervisor.viewport);

    ResetRenderState();
    ScreenEffect::SetViewport(0xff000000);
    g_Supervisor.lastFrameTime = 0;
    g_Supervisor.cfg.colorMode16bit = 0;

    return ZUN_SUCCESS;
}

void GameWindow::ResetRenderState()
{
    ZunColor fogColor;

    if (!g_Supervisor.cfg.disableZBuffer)
    {
        g_Supervisor.gfxDevice->Enable(CAPS_DEPTH_TEST);
    }
    else
    {
        g_Supervisor.gfxDevice->Disable(CAPS_DEPTH_TEST);
    }

    g_Supervisor.gfxDevice->Enable(CAPS_BLEND);
    g_Supervisor.gfxDevice->SetBlendMode(BLEND_ALPHA, BLEND_ALPHA);
    g_Supervisor.gfxDevice->SetDepthFunc(DEPTH_FUNC_ALWAYS);
    g_Supervisor.gfxDevice->Enable(CAPS_ALPHA_TEST);
    g_Supervisor.gfxDevice->SetAlphaTestRef(4);

    if (!g_Supervisor.cfg.disableFog)
    {
        g_Supervisor.gfxDevice->Enable(CAPS_FOG);
    }
    else
    {
        g_Supervisor.gfxDevice->Disable(CAPS_FOG);
    }

    fogColor.color = 0xffa0a0a0;
    g_Supervisor.gfxDevice->SetFogColor(fogColor);
    g_Supervisor.gfxDevice->SetFogRange(1000.0f, 5000.0f);

    g_Supervisor.gfxDevice->SetTextureFilter();
    if (g_AnmManager)
    {
        g_AnmManager->SetBlendMode(255);
        g_AnmManager->SetColorOp(255);
        g_AnmManager->SetVertexShader(255);
        g_AnmManager->SetTexture(0);
        g_AnmManager->SetCameraMode(255);
    }
    g_Stage.renderStateWasReset = 1;
}

i32 GameWindow::ChecksumExecutable()
{
    // the game uses exechecksum and exesize to write to replay and score files about the program
    // that produced that file, and in the original executable those are compared to values in the
    // verfile to check if they're "good" untampered files. obviously it's not gonna match, so we
    // just return these hardcoded values.
    g_Supervisor.exeSize = 650752;
    return g_Supervisor.exeChecksum = 0xaec5445c;
}
