#include "GameWindow.hpp"
#include "AnmManager.hpp"
#include "EaglerOptions.hpp"
#include "FileSystem.hpp"
#include "GameErrorContext.hpp"
#include "PracticeRuntime.hpp"
#include "ScreenEffect.hpp"
#include "SoundPlayer.hpp"
#include "Stage.hpp"
#include "Supervisor.hpp"
#include "ZunMath.hpp"
#include "graphics/Gles.hpp"
#include "i18n.hpp"
#include "utils.hpp"

#ifdef TH_ENABLE_THPRAC
#include "ThpracImGui.hpp"
#endif

#include <SDL3/SDL.h>
#include <algorithm>
#include <cstring>
#include <filesystem>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

GameWindow g_GameWindow;
GfxInterface *g_GfxBackend;
i32 g_TickCountToEffectiveFramerate;
f64 g_LastFrameTime;
f32 g_RenderAlpha = 1.0f;
bool g_SuppressAnmAdvance = false;
bool g_PresentationVsyncEnabled = false;

#ifndef __EMSCRIPTEN__
static f64 GetNativePresentationHz()
{
    SDL_DisplayID display = SDL_GetDisplayForWindow(g_GameWindow.window);
    const SDL_DisplayMode *mode = display ? SDL_GetCurrentDisplayMode(display) : nullptr;
    f64 hz = mode ? (f64)mode->refresh_rate : 0.0;
    if (hz < 30.0 || hz > 1000.0)
        hz = 60.0;
    return hz;
}
#endif

#ifdef TH_DEV_TOOLS
f32 g_DevSpeedMultiplier = 1.0f;
#endif

#ifdef _WIN32
static RECT g_WindowedRect;
static int g_WindowedClientW;
static int g_WindowedClientH;
static LONG g_WindowedStyle;
static bool g_InBorderlessFullscreen = false;
#endif

#define FRAME_TIME (1000. / 60.)

#ifdef TH_ENABLE_THPRAC
static void SaveThpracSnapshot()
{
    std::filesystem::create_directory(FileSystem::GetPrefPath("snapshot"));
    char relativePath[64] = {};
    i32 index = 0;
    for (; index < 1000; ++index)
    {
        std::snprintf(relativePath, sizeof(relativePath), "snapshot/th%.3d.bmp", index);
        if (!std::filesystem::exists(std::filesystem::u8path(FileSystem::GetPrefPath(relativePath))))
            break;
    }
    if (index >= 1000)
        return;

    std::vector<u8> pixels(640 * 480 * 4);
    g_GfxBackend->ReadPixels(0, 0, 640, 480, pixels.data());
    SDL_Surface *surface = SDL_CreateSurfaceFrom(640, 480, SDL_PIXELFORMAT_RGBA32,
                                                 pixels.data(), 640 * 4);
    if (surface != nullptr)
    {
        SDL_SaveBMP(surface, FileSystem::GetPrefPath(relativePath).c_str());
        SDL_DestroySurface(surface);
    }
}
#endif

RenderResult GameWindow::Render()
{
    // Refresh-rate / frameskip only controlled how often the original game drew.
    // Its simulation still advanced at 60 Hz.
    constexpr f64 targetDt = 1.0 / 60.0;
    const u64 renderStartNs = SDL_GetTicksNS();
    ZunViewport viewport;

    if (this->lastActiveAppValue == 0)
    {
        // Vanilla TH06 stops advancing the game while inactive, but its
        // DirectSound buffers keep playing.  Our SDL stream is fed by the
        // main thread, so keep that stream supplied without running a game
        // tick.  Otherwise WAV/OGG music stops as soon as the queued audio is
        // exhausted.
        g_SoundPlayer.PlaySounds();
#ifndef __EMSCRIPTEN__
        SDL_Delay(16);
#endif
        return RENDER_RESULT_KEEP_RUNNING;
    }

#ifdef _WIN32
    // Keep a persistent record of the windowed geometry while the window is
    // windowed, so the fullscreen toggle never has to trust a single
    // instantaneous GetWindowRect around a transition (which can return the
    // window's initial unpositioned state instead of its real geometry).
    {
        HWND hwnd = (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(g_GameWindow.window),
                                                 SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
        if (hwnd && !g_InBorderlessFullscreen)
        {
            RECT wr;
            RECT clientRect;
            if (GetWindowRect(hwnd, &wr) && GetClientRect(hwnd, &clientRect))
            {
                g_WindowedRect = wr;
                g_WindowedClientW = clientRect.right - clientRect.left;
                g_WindowedClientH = clientRect.bottom - clientRect.top;
                g_WindowedStyle = GetWindowLong(hwnd, GWL_STYLE);
            }
        }
    }
#endif

    const u64 currentCounter = SDL_GetPerformanceCounter();
    if (this->lastPerformanceCounter == 0)
    {
        this->lastPerformanceCounter = currentCounter;
    }
    const f64 elapsed = static_cast<f64>(currentCounter - this->lastPerformanceCounter) /
                        static_cast<f64>(SDL_GetPerformanceFrequency());
    this->lastPerformanceCounter = currentCounter;
    const f64 clampedElapsed = std::clamp(elapsed, 0.0, 0.1);
    this->accumulator += clampedElapsed;

#ifdef __EMSCRIPTEN__
    const bool limitPresentationTo60 = EaglerOptions::LimitPresentationTo60();
#else
    constexpr bool limitPresentationTo60 = false;
#endif
    const bool preserveReplayCadence = g_GameManager.isInReplay != 0;

#ifdef TH_DEV_TOOLS
    // Developer fast-forward: run extra 60 Hz simulation passes proportional
    // to the real elapsed time, like the original's frame-count manipulation,
    // while keeping the presentation rate unchanged. F5 cycles the speed.
    if (g_DevSpeedMultiplier > 1.0f)
    {
        this->accumulator += elapsed * (g_DevSpeedMultiplier - 1.0);
    }
#endif

    bool updated = false;
    const auto runSimulationTick = [&]() -> i32
    {
        g_Supervisor.framerateMultiplier = 1.0f;
        g_Supervisor.effectiveFramerateMultiplier = 1.0f;
        const i32 res = g_Chain.RunCalcChain();
#ifdef TH_ENABLE_THPRAC
        // Upstream th06_update is hooked at the RunCalcChain return boundary
        // (0x41caac). Trainer GUI/hotkey producers must therefore run after
        // this tick's game consumers, not before them.
        PracticeRuntime::UpdateOverlay();
#endif
#ifdef TH_ENABLE_THCRAP
        g_AnmManager->QueueThcrapSnapshotIfRequested();
#endif
        g_SoundPlayer.PlaySounds();
        return res;
    };

    if (limitPresentationTo60 || preserveReplayCadence)
    {
        if (this->accumulator >= targetDt)
        {
            // Original TH06's 60 Hz path consumes all overdue wall-clock
            // intervals but runs the game chains only once. Do not let a late
            // browser callback turn into two or three simulation steps before
            // the next picture; fast bullets make that catch-up visibly jump.
            do
            {
                this->accumulator -= targetDt;
            } while (this->accumulator >= targetDt);

            const i32 res = runSimulationTick();
            if (res == 0)
                return RENDER_RESULT_EXIT_SUCCESS;
            if (res == -1)
                return RENDER_RESULT_EXIT_ERROR;
            updated = true;
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
            updated = true;
        }
    }

    // Scene callbacks can request a Supervisor state change after the
    // Supervisor has already run for this 60 Hz tick. Their calc element and
    // draw element are removed immediately, while the replacement scene can
    // only be registered on the next fixed tick. At high presentation rates,
    // clearing and presenting in that interval exposes one or more black
    // frames. Vanilla TH06 leaves the last completed backbuffer visible during
    // this hand-off. Do the same until Supervisor has installed the new scene.
    if (g_Supervisor.wantedState != g_Supervisor.curState)
    {
#ifndef __EMSCRIPTEN__
        SDL_Delay(1);
#endif
        return RENDER_RESULT_KEEP_RUNNING;
    }

#ifdef __EMSCRIPTEN__
    // Simulation stays fixed at 60 Hz, while Web presentation normally follows
    // requestAnimationFrame at the display refresh rate. The optional 60 FPS
    // mode follows the original one-tick-per-picture behavior above and skips
    // callbacks with no new simulation tick. With it off, 90/120/144 Hz
    // catch-up + interpolation/presentation remains available.
    if (limitPresentationTo60 && !updated)
    {
        return RENDER_RESULT_KEEP_RUNNING;
    }
#endif

    g_RenderAlpha = std::clamp(static_cast<f32>(this->accumulator / targetDt), 0.0f, 1.0f);
#ifdef __EMSCRIPTEN__
    // When presentation itself is capped to the fixed 60 Hz simulation ticks,
    // there are no intermediate presentation frames to interpolate. Drawing a
    // residual accumulator fraction here makes the retained frames alternate
    // between different points inside the previous/current interval on 75/90/
    // 120/144+ Hz requestAnimationFrame schedules, which visibly jitters moving
    // objects. Match the original 60 Hz presentation semantics and draw the
    // authoritative current simulation state on every retained frame.
    if (limitPresentationTo60)
    {
        g_RenderAlpha = 1.0f;
    }
#endif
    if (g_GameManager.isInGameMenu || g_GameManager.isInRetryMenu)
    {
        g_RenderAlpha = 1.0f;
    }

    g_GfxBackend->BeginFrame();

    if (g_Supervisor.RedrawWholeFrame())
    {
        viewport = {0, 0, GAME_WINDOW_WIDTH, GAME_WINDOW_HEIGHT, 0.0f, 1.0f};
        viewport.Set();
        g_GfxBackend->SetClearColor(((g_Stage.skyFog.color >> 16) & 0xff) / 255.0f,
                                    ((g_Stage.skyFog.color >> 8) & 0xff) / 255.0f,
                                    (g_Stage.skyFog.color & 0xff) / 255.0f,
                                    (g_Stage.skyFog.color >> 24) / 255.0f);
        g_GfxBackend->Clear(CLEAR_COLOR_BUFFER | CLEAR_DEPTH_BUFFER);
        g_AnmManager->SetProjectionMode(PROJECTION_MODE_PERSPECTIVE);
        // Keep the full-window viewport as the presentation-frame baseline.
        // g_Supervisor.viewport is shared mutable state and, at this point,
        // still contains whichever viewport the final draw callback selected
        // on the previous presentation frame. Restoring it here made the first
        // callback of a new frame depend on the previous frame's tail. Each
        // scene callback establishes its own authoritative viewport below.
    }

    g_AnmManager->ClearVertexBuffer();
    g_AnmManager->flushesThisFrame = 0;
    // The arcade region is layout/clip state, not simulation motion. Interpolating
    // it makes render-only frames use different viewports: one frame draws into
    // the 384x448 playfield while the next can expand across the whole 640x480
    // window and cover the HUD. TH07 keeps its viewport authoritative and only
    // interpolates world/object coordinates, so do the same here.
    g_SuppressAnmAdvance = !updated;
#ifdef TH_ENABLE_THPRAC
    // GameGuiBegin(..., !THAdvOptWnd::IsOpen()): Advanced Options exclusively
    // owns keyboard navigation while open, so background trainer windows do
    // not receive D-pad input.
    ThpracImGui::SetGameNavEnabled(!PracticeRuntime::AdvancedOptionsOpen());
    ThpracImGui::SetGameInput(g_CurFrameInput, updated);
    // Upstream thprac builds its ImGui frame from TH06's 60 Hz update hook
    // and only renders the resulting draw data from the render hook.  Do not
    // advance ImGui again on presentation-only frames (180 Hz on high-refresh
    // displays), otherwise UI timing/navigation runs faster than the game.
    if (updated)
        ThpracImGui::BeginFrame(static_cast<f32>(targetDt));
#endif
    g_Chain.RunDrawChain();
    g_SuppressAnmAdvance = false;
    g_AnmManager->SetCurrentTexture(0);
    g_AnmManager->SetCurrentSprite(nullptr);
    g_AnmManager->FlushVertexBuffer();
#ifdef TH_ENABLE_THPRAC
    PracticeRuntime::DrawOverlay();
    if (ThpracImGui::IsFrameOpen())
        ThpracImGui::EndFrame();
    static_cast<GlesGraphics *>(g_GfxBackend)->RenderImGui(ThpracImGui::GetDrawData());
#endif
    g_GfxBackend->EndFrame();
#ifdef TH_ENABLE_THPRAC
    // th06_render takes the snapshot after GameGuiRender, so the thprac UI is
    // part of the captured 640x480 backbuffer. Keep the same semantic point,
    // immediately before the portable buffer swap.
    if (PracticeRuntime::ConsumeScreenshotRequest())
        SaveThpracSnapshot();
#endif
    Present();

#ifndef __EMSCRIPTEN__
    const f64 presentationHz = GetNativePresentationHz();
    const u64 presentationFrameNs = (u64)(1000000000.0 / presentationHz);
    const u64 elapsedNs = SDL_GetTicksNS() - renderStartNs;
    if (elapsedNs < presentationFrameNs)
    {
        SDL_DelayPrecise(presentationFrameNs - elapsedNs);
    }
#endif

    return RENDER_RESULT_KEEP_RUNNING;
}

void GameWindow::ResetTiming()
{
    this->lastPerformanceCounter = SDL_GetPerformanceCounter();
    this->accumulator = 0.0;
    g_RenderAlpha = 1.0f;
}

void GameWindow::Present()
{
    // In D3D, this was done after the present call, but SDL makes no guarantees
    // about the color buffer state immediately after a swap, so it has to be moved to be before it
    g_AnmManager->TakeScreenshotIfRequested();
#ifdef TH_ENABLE_THCRAP
    // The P key itself is sampled at the 60 Hz calc breakpoint above. Read
    // the completed portable backbuffer here, immediately before the swap,
    // so double-buffered GLES does not return the cleared/old draw target.
    g_AnmManager->TakeThcrapSnapshotIfRequested();
#endif
    if (g_Supervisor.unk198 != 0)
    {
        g_Supervisor.unk198--;
    }

    g_GfxBackend->SwapBuffers();

    return;
}

void GameWindow::CreateGameWindow()
{
    SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "0");
    SDL_SetHint(SDL_HINT_MOUSE_TOUCH_EVENTS, "0");

    // On Windows SDL3 may otherwise create an OpenGL ES profile through WGL.
    // The portable builds link Mesa GLES, so force SDL onto the matching EGL
    // path just as the established TH07 desktop package does.
#ifndef USING_GL
    SDL_SetHint(SDL_HINT_OPENGL_ES_DRIVER, "1");
#endif
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
    {
        return;
    }

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

    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, TH_WINDOW_TITLE);
    SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_OPENGL_BOOLEAN, true);
#if defined(__ANDROID__) || (defined(__APPLE__) && TARGET_OS_IPHONE)
    SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_FULLSCREEN_BOOLEAN, true);
    SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_HIGH_PIXEL_DENSITY_BOOLEAN, true);
#elif defined(__EMSCRIPTEN__)
    SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_HIGH_PIXEL_DENSITY_BOOLEAN, true);
#else
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, GAME_WINDOW_WIDTH);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, GAME_WINDOW_HEIGHT);
#endif
    g_GameWindow.window = SDL_CreateWindowWithProperties(props);
    SDL_DestroyProperties(props);
    if (!g_GameWindow.window)
    {
        return;
    }

    g_GfxBackend = GlesGraphics::Init();
    SDL_ShowWindow(g_GameWindow.window);

    g_GameWindow.lastActiveAppValue = 1;
}

// Toggle between windowed and borderless fullscreen.
//
// SDL_SetWindowFullscreen is deliberately not used on Windows: under SDL3 +
// Mesa EGL in RDP sessions the fullscreen transition can be followed by an
// SDL-internal restore of the windowed geometry (~250-400 ms later), leaving
// the window "fullscreen" but at the windowed size. Instead we toggle the
// Win32 style directly and let SDL_SetWindowSize/Position apply the geometry:
// those update SDL's tracked state before touching the window, so the Win32
// min/max-track clamp (derived from SDL's size) never fights the toggle, and
// SwapBuffers() already re-reads the window size every frame.
//
// The windowed geometry is captured continuously by the frame loop (see
// RememberWindowedState) rather than at toggle time, because reading the
// window rect synchronously around a transition can return the window's
// initial unpositioned state (0,0) instead of its real geometry.
void GameWindow::ToggleFullscreen()
{
    if (g_GameWindow.window == NULL)
    {
        return;
    }

#ifdef _WIN32
    HWND hwnd = (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(g_GameWindow.window),
                                             SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
    if (!hwnd)
    {
        return;
    }

    if (!g_InBorderlessFullscreen)
    {
        if (g_WindowedRect.right == 0 && g_WindowedRect.bottom == 0)
        {
            RECT windowedRect;
            RECT windowedClient;
            if (!GetWindowRect(hwnd, &windowedRect) || !GetClientRect(hwnd, &windowedClient))
            {
                return;
            }
            g_WindowedRect = windowedRect;
            g_WindowedClientW = windowedClient.right - windowedClient.left;
            g_WindowedClientH = windowedClient.bottom - windowedClient.top;
            g_WindowedStyle = GetWindowLong(hwnd, GWL_STYLE);
        }

        HMONITOR mon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
        MONITORINFO minfo;
        minfo.cbSize = sizeof(minfo);
        if (!mon || !GetMonitorInfoW(mon, &minfo))
        {
            return;
        }

        // Size the window to the display's current desktop mode rather than the
        // raw monitor rect. Virtual/remote displays can report a native
        // geometry (e.g. 2560x1600) that is taller than the visible desktop
        // mode (e.g. 2560x1440); covering the monitor rect then letterboxes
        // the image on the client's screen. The desktop mode matches what the
        // user actually sees.
        SDL_DisplayID disp = SDL_GetDisplayForWindow(g_GameWindow.window);
        SDL_Rect dispBounds;
        SDL_GetDisplayBounds(disp, &dispBounds);
        const SDL_DisplayMode *dm = SDL_GetDesktopDisplayMode(disp);
        const int fsW = (dm && dm->w > 0) ? dm->w : dispBounds.w;
        const int fsH = (dm && dm->h > 0) ? dm->h : dispBounds.h;

        SetWindowLong(hwnd, GWL_STYLE, (g_WindowedStyle & ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX |
                                                            WS_MAXIMIZEBOX | WS_SYSMENU)) |
                                           WS_POPUP);
        SDL_SetWindowSize(g_GameWindow.window, fsW, fsH);
        SDL_SetWindowPosition(g_GameWindow.window, dispBounds.x, dispBounds.y);
        g_InBorderlessFullscreen = true;
    }
    else
    {
        SetWindowLong(hwnd, GWL_STYLE, g_WindowedStyle);
        SDL_SetWindowSize(g_GameWindow.window, g_WindowedClientW, g_WindowedClientH);
        SDL_SetWindowPosition(g_GameWindow.window, g_WindowedRect.left, g_WindowedRect.top);
        // Pin the exact window rect. SDL_SetWindowPosition addresses the client
        // area, which shifts with the caption frame; SDL now tracks the windowed
        // size so the Win32 min/max-track clamp (if any) lets this through.
        SetWindowPos(hwnd, HWND_TOP, g_WindowedRect.left, g_WindowedRect.top,
                     g_WindowedRect.right - g_WindowedRect.left,
                     g_WindowedRect.bottom - g_WindowedRect.top,
                     SWP_FRAMECHANGED | SWP_NOACTIVATE | SWP_SHOWWINDOW);
        g_InBorderlessFullscreen = false;
    }
#else
    if (SDL_GetWindowFlags(g_GameWindow.window) & SDL_WINDOW_FULLSCREEN)
    {
        SDL_SetWindowFullscreen(g_GameWindow.window, false);
    }
    else
    {
        SDL_SetWindowFullscreen(g_GameWindow.window, true);
    }
#endif
}

// LRESULT __stdcall GameWindow::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
// {
//     switch (uMsg)
//     {
//     case 0x3c9:
//         if (g_Supervisor.midiOutput != NULL)
//         {
//             g_Supervisor.midiOutput->UnprepareHeader((LPMIDIHDR)lParam);
//         }
//         break;
//     case WM_ACTIVATEAPP:
//         g_GameWindow.lastActiveAppValue = wParam;
//         if (g_GameWindow.lastActiveAppValue != 0)
//         {
//             g_GameWindow.isAppActive = 0;
//         }
//         else
//         {
//             g_GameWindow.isAppActive = 1;
//         }
//         break;
//     case WM_SETCURSOR:
//         if (!g_Supervisor.cfg.windowed)
//         {
//             if (g_GameWindow.isAppActive != 0)
//             {
//                 SetCursor(LoadCursorA(NULL, IDC_ARROW));
//                 ShowCursor(1);
//             }
//             else
//             {
//                 ShowCursor(0);
//                 SetCursor((HCURSOR)0x0);
//             }
//         }
//         else
//         {
//             SetCursor(LoadCursorA(NULL, IDC_ARROW));
//             ShowCursor(1);
//         }
//
//         return 1;
//     case WM_CLOSE:
//         g_GameWindow.isAppClosing = 1;
//         return 1;
//     }
//     return DefWindowProcA(hWnd, uMsg, wParam, lParam);
// }

ZunResult GameWindow::InitD3dRendering()
{
    if (!g_GfxBackend)
    {
        g_GameErrorContext.Fatal(TH_ERR_D3D_INIT_FAILED);
        return ZUN_ERROR;
    }

    // TH06 used this legacy capability bit to choose between two materially
    // different bullet/popup draw paths.  The original Direct3D code set it
    // when D3DCREATE_HARDWARE_VERTEXPROCESSING succeeded.  Our GLES backend
    // always implements the corresponding model/view/projection transform
    // path (whether the host GL implementation is backed by a physical GPU or
    // a software rasterizer), so leaving the zero-initialized D3D bit unset
    // incorrectly forces the old software-vertex fallback forever.
    //
    // This matters visibly for bullets: the normal path reaches Draw2/Draw3,
    // which preserves sub-pixel positions for rotated sprites, while the
    // fallback Draw() path rounds their center with rintf() every frame.
    g_Supervisor.hasD3dHardwareVertexProcessing = 1;

    //    u8 using_d3d_hal;
    //    D3DPRESENT_PARAMETERS present_params;
    //    D3DDISPLAYMODE display_mode;
    ZunVec3 eye;
    ZunVec3 at;
    ZunVec3 up;
    f32 half_width;
    f32 half_height;
    f32 aspect_ratio;
    f32 field_of_view_y;
    f32 camera_distance;

    // OpenGL considers textures to be incomplete if the bound texture has no image defined
    // Incomplete textures result in texturing being turned off, but EoSD has places where it
    // uses the texturing engine to color fragments without using the texture itself. The dummy
    // texture is necessary to ensure the texture can't be considered incomplete in these cases.
    g_AnmManager->CreateTextureObject();
    g_AnmManager->dummyTextureHandle = g_AnmManager->currentTextureHandle;
    g_GfxBackend->SetTextureImage(1, 1, PIXEL_RGBA, PIXEL_UNSIGNED_BYTE, NULL);

    //    using_d3d_hal = 1;
    //    std::memset(&present_params, 0, sizeof(D3DPRESENT_PARAMETERS));
    //    g_Supervisor.d3dIface->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &display_mode);
    if (!g_Supervisor.cfg.windowed)
    {
        if ((((g_Supervisor.cfg.opts >> GCOS_FORCE_16BIT_COLOR_MODE) & 1) == 1))
        {
            //            present_params.BackBufferFormat = D3DFMT_R5G6B5;
            g_Supervisor.cfg.colorMode16bit = 1;
        }
        else if (g_Supervisor.cfg.colorMode16bit == 0xff)
        {
            //            if ((display_mode.Format == D3DFMT_X8R8G8B8) || (display_mode.Format == D3DFMT_A8R8G8B8))
            //            {
            //                present_params.BackBufferFormat = D3DFMT_X8R8G8B8;
            g_Supervisor.cfg.colorMode16bit = 0;
            g_GameErrorContext.Log(TH_ERR_SCREEN_INIT_32BITS);
            //            }
            //            else
            //            {
            //                present_params.BackBufferFormat = D3DFMT_R5G6B5;
            //                g_Supervisor.cfg.colorMode16bit = 1;
            //                GameErrorContext::Log(&g_GameErrorContext, TH_ERR_SCREEN_INIT_16BITS);
            //            }
        }
        //        else if (g_Supervisor.cfg.colorMode16bit == 0)
        //        {
        //            present_params.BackBufferFormat = D3DFMT_X8R8G8B8;
        //        }
        //        else
        //        {
        //            present_params.BackBufferFormat = D3DFMT_R5G6B5;
        //        }
        if (!((g_Supervisor.cfg.opts >> GCOS_FORCE_60FPS) & 1))
        {

            //            present_params.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE;
        }
        else
        {
            //            present_params.FullScreen_RefreshRateInHz = 60;
            //            present_params.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE;
            //            GameErrorContext::Log(&g_GameErrorContext, TH_ERR_SET_REFRESH_RATE_60HZ);
        }

        //        if (g_Supervisor.cfg.frameskipConfig == 0)
        //        {
        //            present_params.SwapEffect = D3DSWAPEFFECT_FLIP;
        //        }
        //        else
        //        {
        //            present_params.SwapEffect = D3DSWAPEFFECT_COPY_VSYNC;
        //        }
    }
    //    else
    //    {
    //        present_params.BackBufferFormat = display_mode.Format;
    //        present_params.SwapEffect = D3DSWAPEFFECT_COPY;
    //        present_params.Windowed = 1;
    //    }
    //    present_params.BackBufferWidth = GAME_WINDOW_WIDTH;
    //    present_params.BackBufferHeight = GAME_WINDOW_HEIGHT;
    //    present_params.EnableAutoDepthStencil = true;
    //    present_params.AutoDepthStencilFormat = D3DFMT_D16;
    //    present_params.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

    g_Supervisor.vsyncEnabled = 1;

    g_Supervisor.lockableBackbuffer = 1;
    //    memcpy(&g_Supervisor.presentParameters, &present_params, sizeof(D3DPRESENT_PARAMETERS));
    //    for (;;)
    //    {
    //        if (((g_Supervisor.cfg.opts >> GCOS_REFERENCE_RASTERIZER_MODE) & 1) != 0)
    //        {
    //            goto REFERENCE_RASTERIZER_MODE;
    //        }
    //        else
    //        {
    //            if (g_Supervisor.d3dIface->CreateDevice(0, D3DDEVTYPE_HAL, g_GameWindow.window,
    //                                                    D3DCREATE_HARDWARE_VERTEXPROCESSING, &present_params,
    //                                                    &g_Supervisor.d3dDevice) < 0)
    //            {
    //                GameErrorContext::Log(&g_GameErrorContext, TH_ERR_TL_HAL_UNAVAILABLE);
    //                if (g_Supervisor.d3dIface->CreateDevice(0, D3DDEVTYPE_HAL, g_GameWindow.window,
    //                                                        D3DCREATE_SOFTWARE_VERTEXPROCESSING, &present_params,
    //                                                        &g_Supervisor.d3dDevice) < 0)
    //                {
    //                    GameErrorContext::Log(&g_GameErrorContext, TH_ERR_HAL_UNAVAILABLE);
    //                REFERENCE_RASTERIZER_MODE:
    //                    if (g_Supervisor.d3dIface->CreateDevice(0, D3DDEVTYPE_REF, g_GameWindow.window,
    //                                                            D3DCREATE_SOFTWARE_VERTEXPROCESSING, &present_params,
    //                                                            &g_Supervisor.d3dDevice) < 0)
    //                    {
    //                        if (((g_Supervisor.cfg.opts >> GCOS_FORCE_60FPS) & 1) != 0 && !g_Supervisor.vsyncEnabled)
    //                        {
    //                            GameErrorContext::Log(&g_GameErrorContext,
    //                            TH_ERR_CANT_CHANGE_REFRESH_RATE_FORCE_VSYNC);
    //                            present_params.FullScreen_RefreshRateInHz = 0;
    //                            g_Supervisor.vsyncEnabled = 1;
    //                            present_params.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    //                            continue;
    //                        }
    //                        else
    //                        {
    //                            if (present_params.Flags == D3DPRESENTFLAG_LOCKABLE_BACKBUFFER)
    //                            {
    //                                GameErrorContext::Log(&g_GameErrorContext, TH_ERR_BACKBUFFER_NONLOCKED);
    //                                present_params.Flags = 0;
    //                                g_Supervisor.lockableBackbuffer = 0;
    //                                continue;
    //                            }
    //                            else
    //                            {
    //                                GameErrorContext::Fatal(&g_GameErrorContext, TH_ERR_D3D_INIT_FAILED);
    //                                if (g_Supervisor.d3dIface != NULL)
    //                                {
    //                                    g_Supervisor.d3dIface->Release();
    //                                    g_Supervisor.d3dIface = NULL;
    //                                }
    //                                return 1;
    //                            }
    //                        }
    //                    }
    //                    else
    //                    {
    //                        GameErrorContext::Log(&g_GameErrorContext, TH_USING_REF_MODE);
    //                        g_Supervisor.hasD3dHardwareVertexProcessing = 0;
    //                        using_d3d_hal = 0;
    //                    }
    //                }
    //                else
    //                {
    //                    GameErrorContext::Log(&g_GameErrorContext, TH_USING_HAL_MODE);
    //                    g_Supervisor.hasD3dHardwareVertexProcessing = 0;
    //                }
    //            }
    //            else
    //            {
    //                GameErrorContext::Log(&g_GameErrorContext, TH_USING_TL_HAL_MODE);
    //                g_Supervisor.hasD3dHardwareVertexProcessing = 1;
    //            }
    //            break;
    //        }
    //    }

    // Camera set up so that at z = 0.0, world coordinates map exactly to (quadrant 4) window coordinates

    half_width = (float)GAME_WINDOW_WIDTH / 2.0;
    half_height = (float)GAME_WINDOW_HEIGHT / 2.0;
    aspect_ratio = (float)GAME_WINDOW_WIDTH / (float)GAME_WINDOW_HEIGHT;
    field_of_view_y = 0.52359879; // PI / 6.0f
    camera_distance = half_height / ZUN_TANF(field_of_view_y / 2.0f);
    up.x = 0.0;
    up.y = 1.0;
    up.z = 0.0;
    at.x = half_width;
    at.y = -half_height;
    at.z = 0.0;
    eye.x = half_width;
    eye.y = -half_height;
    eye.z = -camera_distance;
    //    D3DXMatrixLookAtLH(&g_Supervisor.viewMatrix, &eye, &at, &up);

    ZunMatrix viewMatrix = createViewMatrix(eye, at, up);
    g_AnmManager->SetTransformMatrix(MATRIX_VIEW, viewMatrix);
    g_Supervisor.viewMatrix = viewMatrix;

    ZunMatrix perspectiveMatrix = perspectiveMatrixFromFOV(field_of_view_y, aspect_ratio, 100.0f, 10000.0f);
    g_AnmManager->SetTransformMatrix(MATRIX_PROJECTION, perspectiveMatrix);
    g_Supervisor.projectionMatrix = perspectiveMatrix;

    //    D3DXMatrixPerspectiveFovLH(&g_Supervisor.projectionMatrix, field_of_view_y, aspect_ratio, 100.0, 10000.0);
    //    g_Supervisor.d3dDevice->SetTransform(D3DTS_VIEW, &g_Supervisor.viewMatrix);
    //    g_Supervisor.d3dDevice->SetTransform(D3DTS_PROJECTION, &g_Supervisor.projectionMatrix);
    g_Supervisor.viewport.Get();

    //    g_Supervisor.d3dDevice->GetDeviceCaps(&g_Supervisor.d3dCaps);
    //    if (((((g_Supervisor.cfg.opts >> GCOS_USE_D3D_HW_TEXTURE_BLENDING) & 1) == 0) &&
    //         ((g_Supervisor.d3dCaps.TextureOpCaps & D3DTEXOPCAPS_ADD) == 0)))
    //    {
    //        GameErrorContext::Log(&g_GameErrorContext, TH_ERR_NO_SUPPORT_FOR_D3DTEXOPCAPS_ADD);
    //        g_Supervisor.cfg.opts = g_Supervisor.cfg.opts | (1 << GCOS_USE_D3D_HW_TEXTURE_BLENDING);
    //    }
    //    if (g_Supervisor.ShouldRunAt60Fps() &&
    //        ((g_Supervisor.d3dCaps.PresentationIntervals & D3DPRESENT_INTERVAL_IMMEDIATE) == 0))
    //    {
    //        GameErrorContext::Log(&g_GameErrorContext, TH_ERR_CANT_FORCE_60FPS_NO_ASYNC_FLIP);
    //        g_Supervisor.cfg.opts = g_Supervisor.cfg.opts & ~(1 << GCOS_FORCE_60FPS);
    //    }
    //    if ((((g_Supervisor.cfg.opts >> GCOS_FORCE_16BIT_COLOR_MODE) & 1) == 0) && (using_d3d_hal != 0))
    //    {
    //        if (g_Supervisor.d3dIface->CheckDeviceFormat(0, D3DDEVTYPE_HAL, present_params.BackBufferFormat, 0,
    //                                                     D3DRTYPE_TEXTURE, D3DFMT_A8R8G8B8) == 0)
    //        {
    //            g_Supervisor.colorMode16Bits = 1;
    //        }
    //        else
    //        {
    //            g_Supervisor.colorMode16Bits = 0;
    //            g_Supervisor.cfg.opts = g_Supervisor.cfg.opts | (1 << GCOS_FORCE_16BIT_COLOR_MODE);
    //            GameErrorContext::Log(&g_GameErrorContext, TH_ERR_D3DFMT_A8R8G8B8_UNSUPPORTED);
    //        }
    //    }
    InitD3dDevice();
    ScreenEffect::SetViewport(0);
    g_GameWindow.isAppClosing = 0;
    g_Supervisor.lastFrameTime = 0;
    g_Supervisor.framerateMultiplier = 0.0;
    return ZUN_SUCCESS;
}

void GameWindow::InitD3dDevice(void)
{
    AnmManager *anm1;
    AnmManager *anm2;
    AnmManager *anm3;
    AnmManager *anm4;

    g_GfxBackend->Enable(CAPS_BLEND);

    g_GfxBackend->SetBlendMode(BLEND_INV_SRC_ALPHA);

    // Match the fixed-function renderer defaults. GLES implements these in
    // the shader, so leaving them disabled silently removes cutout alpha and
    // all stage fog.
    g_GfxBackend->Enable(CAPS_ALPHA_TEST);
    g_GfxBackend->SetAlphaTestRef(4);

    if (((g_Supervisor.cfg.opts >> GCOS_DONT_USE_FOG) & 1) == 0)
    {
        g_GfxBackend->Enable(CAPS_FOG);
        g_Supervisor.fogEnabled = 1;
    }

    if (((g_Supervisor.cfg.opts >> GCOS_TURN_OFF_DEPTH_TEST) & 1) == 0)
    {
        g_GfxBackend->Enable(CAPS_DEPTH_TEST);
        g_AnmManager->SetDepthMask(true);
        g_AnmManager->SetDepthFunc(DEPTH_FUNC_LEQUAL);
    }

    g_AnmManager->SetFogColor(0xFF'A0'A0'A0);
    g_AnmManager->SetFogRange(1'000.0f, 5'000.0f);

    // All of these are set per texture object in OpenGL (and also most are defaults)
    //    g_Supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_NONE);
    //    g_Supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
    //    g_Supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
    //    g_Supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
    //    g_Supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP);
    //    g_Supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
    //    g_Supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
    if (g_AnmManager != NULL)
    {
        anm1 = g_AnmManager;
        anm1->currentBlendMode = 0xff;
        anm4 = g_AnmManager;
        anm4->currentTextureHandle = 0;
        anm4->currentSprite = nullptr;
    }
    g_Stage.skyFogNeedsSetup = 1;
    return;
}
