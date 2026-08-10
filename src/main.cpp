#include <SDL3/SDL.h>

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

#include "AnmManager.hpp"
#include "Chain.hpp"
#include "Controller.hpp"
#include "FileSystem.hpp"
#include "GameErrorContext.hpp"
#include "GameWindow.hpp"
#include "SoundPlayer.hpp"
#include "Supervisor.hpp"
#include "ZunResult.hpp"
#include "i18n.hpp"

static RenderResult renderResult = RENDER_RESULT_KEEP_RUNNING;
static bool g_ToggleFullscreenRequested = false;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)
{
    (void)appstate;
    (void)argc;
    (void)argv;

    if (g_Supervisor.LoadConfig(TH_CONFIG_FILE) != ZUN_SUCCESS)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "th06: configuration initialization failed");
        return SDL_APP_FAILURE;
    }

    GameWindow::CreateGameWindow();
    if (!g_GameWindow.window || !g_GfxBackend)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "th06: window/GLES initialization failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    g_AnmManager = new AnmManager();
    // Resource initialization can render temporary glyph/ANM data. Establish
    // the streaming VBO for that work before any DrawPrimitiveUP call.
    g_GfxBackend->BeginFrame();
    if (GameWindow::InitD3dRendering() != ZUN_SUCCESS)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "th06: renderer resource initialization failed");
        return SDL_APP_FAILURE;
    }
    g_GfxBackend->EndFrame();

    g_SoundPlayer.InitializeDSound();
    Controller::GetJoystickCaps();
    Controller::ResetKeyboard();
    if (Supervisor::RegisterChain() != ZUN_SUCCESS)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "th06: supervisor chain initialization failed");
        return SDL_APP_FAILURE;
    }

    if (!g_Supervisor.cfg.windowed)
    {
        SDL_HideCursor();
    }
    g_GameWindow.curFrame = 0;
    g_GameWindow.isAppActive = 1;
    g_GameWindow.lastActiveAppValue = 1;
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    (void)appstate;
    if (g_ToggleFullscreenRequested)
    {
        // Toggling inside SDL_AppEvent re-enters SDL's message handling and
        // the window can be reset to its windowed geometry shortly afterwards;
        // perform it here, from the regular frame loop instead.
        g_ToggleFullscreenRequested = false;
        GameWindow::ToggleFullscreen();
    }
    renderResult = g_GameWindow.Render();
    return renderResult == RENDER_RESULT_KEEP_RUNNING ? SDL_APP_CONTINUE : SDL_APP_SUCCESS;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    (void)appstate;
    switch (event->type)
    {
    case SDL_EVENT_WINDOW_FOCUS_GAINED:
    case SDL_EVENT_WILL_ENTER_FOREGROUND:
    case SDL_EVENT_DID_ENTER_FOREGROUND:
        g_GameWindow.lastActiveAppValue = 1;
        g_GameWindow.ResetTiming();
        g_GameWindow.isAppActive = 1;
        break;
    case SDL_EVENT_WINDOW_FOCUS_LOST:
    case SDL_EVENT_WILL_ENTER_BACKGROUND:
    case SDL_EVENT_DID_ENTER_BACKGROUND:
        g_GameWindow.lastActiveAppValue = 0;
        g_GameWindow.isAppActive = 0;
        SDL_ShowCursor();
        break;
    case SDL_EVENT_QUIT:
        return SDL_APP_SUCCESS;
    case SDL_EVENT_KEY_DOWN:
        // Toggle windowed <-> borderless fullscreen with Alt+Enter. Deferred to
        // SDL_AppIterate: see g_ToggleFullscreenRequested above.
        if (event->key.repeat == 0 &&
            (event->key.scancode == SDL_SCANCODE_RETURN || event->key.scancode == SDL_SCANCODE_KP_ENTER) &&
            (event->key.mod & (SDL_KMOD_LALT | SDL_KMOD_RALT)) != 0)
        {
            g_ToggleFullscreenRequested = true;
            // Don't let the Enter used for the toggle trigger in-game actions.
            Controller::SetEnterSuppressed(true);
        }
#ifdef TH_DEV_TOOLS
        else if (event->key.repeat == 0 && event->key.scancode == SDL_SCANCODE_F5)
        {
            // Developer fast-forward: cycle 1x -> 4x -> 8x -> 1x.
            g_DevSpeedMultiplier = (g_DevSpeedMultiplier == 1.0f) ? 4.0f
                                  : (g_DevSpeedMultiplier == 4.0f) ? 8.0f
                                                                   : 1.0f;
            SDL_Log("th06 dev: logic speed = %gx", g_DevSpeedMultiplier);
        }
#endif
        break;
    case SDL_EVENT_KEY_UP:
        if (event->key.scancode == SDL_SCANCODE_RETURN || event->key.scancode == SDL_SCANCODE_KP_ENTER)
        {
            Controller::SetEnterSuppressed(false);
        }
        break;
    default:
        break;
    }
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    (void)appstate;
    (void)result;

    g_Chain.Release();
    g_SoundPlayer.Release();
    delete g_AnmManager;
    g_AnmManager = nullptr;
    delete g_GfxBackend;
    g_GfxBackend = nullptr;

    if (g_GameWindow.window)
    {
        SDL_DestroyWindow(g_GameWindow.window);
        g_GameWindow.window = nullptr;
    }

    FileSystem::WriteDataToFile(TH_CONFIG_FILE, &g_Supervisor.cfg, sizeof(g_Supervisor.cfg));
    SDL_ShowCursor();
    g_GameErrorContext.Flush();
}
