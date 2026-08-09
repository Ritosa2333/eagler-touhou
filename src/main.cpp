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
    if (GameWindow::InitD3dRendering() != ZUN_SUCCESS)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "th06: renderer resource initialization failed");
        return SDL_APP_FAILURE;
    }

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
