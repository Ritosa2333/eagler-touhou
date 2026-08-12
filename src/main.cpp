#include <SDL3/SDL.h>
#include <cstring>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

#include "AnmManager.hpp"
#include "Chain.hpp"
#include "Controller.hpp"
#include "FileSystem.hpp"
#include "GameErrorContext.hpp"
#include "GameWindow.hpp"
#include "GameManager.hpp"
#include "MainMenu.hpp"
#include "PracticeRuntime.hpp"
#include "SoundPlayer.hpp"
#include "Supervisor.hpp"
#include "Touch.hpp"
#include "ZunResult.hpp"
#include "i18n.hpp"

static RenderResult renderResult = RENDER_RESULT_KEEP_RUNNING;
static bool g_ToggleFullscreenRequested = false;
static bool g_AudioSuspendedByFocus = false;
static bool g_OpenMusicRoomForVisualTest = false;
static bool g_MusicRoomVisualTestDispatched = false;
#ifdef TH_ENABLE_THPRAC
static bool g_OpenThpracMenuForVisualTest = false;
static bool g_ThpracMenuVisualTestDispatched = false;
static bool g_AutoStartThpracVisualTest = false;
static int g_ThpracVisualTestFrames = 0;
static bool g_AutoPauseThpracVisualTest = false;
static bool g_ThpracPauseVisualTestDispatched = false;
static bool g_ThpracReplaySelfTest = false;
static bool g_ThpracRestartSelfTest = false;
#endif

static void SuspendAudioForInactiveWindow()
{
    if (g_AudioSuspendedByFocus)
    {
        return;
    }
    if (g_SoundPlayer.audioDev != 0)
    {
        SDL_PauseAudioDevice(g_SoundPlayer.audioDev);
    }
    if (g_Supervisor.midiOutput != nullptr)
    {
        g_Supervisor.midiOutput->SetPaused(true);
    }
    g_AudioSuspendedByFocus = true;
}

static void ResumeAudioForActiveWindow()
{
    if (!g_AudioSuspendedByFocus)
    {
        return;
    }
    if (g_SoundPlayer.audioDev != 0)
    {
        SDL_ResumeAudioDevice(g_SoundPlayer.audioDev);
    }
    if (g_Supervisor.midiOutput != nullptr)
    {
        g_Supervisor.midiOutput->SetPaused(false);
    }
    g_AudioSuspendedByFocus = false;
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)
{
    (void)appstate;
    for (int index = 1; index < argc; index++)
        if (std::strcmp(argv[index], "--music-room") == 0)
            g_OpenMusicRoomForVisualTest = true;
#ifdef TH_ENABLE_THPRAC
        else if (std::strcmp(argv[index], "--thprac-menu") == 0)
            g_OpenThpracMenuForVisualTest = true;
        else if (std::strcmp(argv[index], "--thprac-run") == 0)
            g_OpenThpracMenuForVisualTest = g_AutoStartThpracVisualTest = true;
        else if (std::strcmp(argv[index], "--thprac-pause") == 0)
            g_OpenThpracMenuForVisualTest = g_AutoStartThpracVisualTest = g_AutoPauseThpracVisualTest = true;
        else if (std::strcmp(argv[index], "--thprac-replay-selftest") == 0)
            g_ThpracReplaySelfTest = true;
        else if (std::strcmp(argv[index], "--thprac-restart-selftest") == 0)
            g_ThpracRestartSelfTest = true;
#endif
#ifdef __EMSCRIPTEN__
    g_OpenMusicRoomForVisualTest = EM_ASM_INT({ return Module.eaglerOptions?.debugHarness === 'music-room'; }) != 0;
#ifdef TH_ENABLE_THPRAC
    g_OpenThpracMenuForVisualTest = EM_ASM_INT({ return Module.eaglerOptions?.debugHarness === 'thprac-menu'; }) != 0;
#endif
#endif

    if (g_Supervisor.LoadConfig(TH_CONFIG_FILE) != ZUN_SUCCESS)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "th06: configuration initialization failed");
        return SDL_APP_FAILURE;
    }
#ifdef TH_ENABLE_THPRAC
    if (g_ThpracReplaySelfTest)
    {
        const bool passed = PracticeRuntime::DebugReplayMetadataRoundTrip("thprac-replay-selftest.rpy");
        SDL_Log("th06 thprac replay metadata round-trip: %s", passed ? "PASS" : "FAIL");
        return passed ? SDL_APP_SUCCESS : SDL_APP_FAILURE;
    }
    if (g_ThpracRestartSelfTest)
    {
        const bool passed = PracticeRuntime::DebugRestartPreservesConfig();
        SDL_Log("th06 thprac restart parameter preservation: %s", passed ? "PASS" : "FAIL");
        return passed ? SDL_APP_SUCCESS : SDL_APP_FAILURE;
    }
#endif

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
    if (g_OpenMusicRoomForVisualTest && !g_MusicRoomVisualTestDispatched &&
        g_MainMenu.chainCalc != nullptr)
    {
        // Drive the same transition as selecting Music Room in the real main
        // menu, after its shared title resources have finished loading.
        g_MainMenu.gameState = STATE_MUSIC_ROOM;
        g_MainMenu.stateTimer = 60;
        for (AnmVm &vm : g_MainMenu.vm)
            vm.pendingInterrupt = 4;
        g_MusicRoomVisualTestDispatched = true;
    }
#ifdef TH_ENABLE_THPRAC
    if (g_OpenThpracMenuForVisualTest && !g_ThpracMenuVisualTestDispatched &&
        g_MainMenu.chainCalc != nullptr)
    {
        // Desktop-only UI harness: provide the same character data that the
        // vanilla Practice flow has already committed before stage selection.
        g_GameManager.difficulty = NORMAL;
        g_GameManager.character = 0;
        g_GameManager.shotType = 0;
        g_GameManager.isInPracticeMode = 1;
        g_MainMenu.gameState = STATE_PRACTICE_LVL_SELECT;
        g_MainMenu.stateTimer = 0;
        g_ThpracMenuVisualTestDispatched = true;
    }
    if (g_AutoStartThpracVisualTest && g_ThpracMenuVisualTestDispatched &&
        ++g_ThpracVisualTestFrames == 30)
        PracticeRuntime::DebugAcceptPracticeMenu();
    if (g_AutoPauseThpracVisualTest && !g_ThpracPauseVisualTestDispatched &&
        PracticeRuntime::Active() && g_Supervisor.curState == SUPERVISOR_STATE_GAMEMANAGER &&
        g_ThpracVisualTestFrames > 180)
    {
        g_GameManager.isInGameMenu = 1;
        g_ThpracPauseVisualTestDispatched = true;
    }
#endif
    if (g_ToggleFullscreenRequested)
    {
        // Toggling inside SDL_AppEvent re-enters SDL's message handling and
        // the window can be reset to its windowed geometry shortly afterwards;
        // perform it here, from the regular frame loop instead.
        g_ToggleFullscreenRequested = false;
        GameWindow::ToggleFullscreen();
    }
    renderResult = g_GameWindow.Render();
    if (renderResult == RENDER_RESULT_KEEP_RUNNING)
    {
        return SDL_APP_CONTINUE;
    }
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "th06: render loop exited (%d), supervisor wanted=%d current=%d\n%s",
                 static_cast<int>(renderResult), g_Supervisor.wantedState,
                 g_Supervisor.curState, g_GameErrorContext.m_Buffer);
    return renderResult == RENDER_RESULT_EXIT_SUCCESS ? SDL_APP_SUCCESS : SDL_APP_FAILURE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    (void)appstate;
    switch (event->type)
    {
    case SDL_EVENT_WINDOW_FOCUS_GAINED:
    case SDL_EVENT_WILL_ENTER_FOREGROUND:
    case SDL_EVENT_DID_ENTER_FOREGROUND:
        ResumeAudioForActiveWindow();
        g_GameWindow.lastActiveAppValue = 1;
        g_GameWindow.ResetTiming();
        g_GameWindow.isAppActive = 1;
        break;
    case SDL_EVENT_WINDOW_FOCUS_LOST:
    case SDL_EVENT_WILL_ENTER_BACKGROUND:
    case SDL_EVENT_DID_ENTER_BACKGROUND:
        Touch::CancelTouches();
        SuspendAudioForInactiveWindow();
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
    case SDL_EVENT_GAMEPAD_ADDED:
        if (g_Supervisor.gameController == NULL)
        {
            g_Supervisor.gameController = SDL_OpenGamepad(event->gdevice.which);
        }
        break;
    case SDL_EVENT_GAMEPAD_REMOVED:
        if (g_Supervisor.gameController != NULL &&
            SDL_GetGamepadID(g_Supervisor.gameController) == event->gdevice.which)
        {
            SDL_CloseGamepad(g_Supervisor.gameController);
            g_Supervisor.gameController = NULL;
        }
        break;
    case SDL_EVENT_FINGER_DOWN:
        Touch::FingerDown(event->tfinger);
        break;
    case SDL_EVENT_FINGER_CANCELED:
    case SDL_EVENT_FINGER_UP:
        Touch::FingerUp(event->tfinger);
        break;
    case SDL_EVENT_FINGER_MOTION:
        Touch::FingerMotion(event->tfinger);
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
#ifdef __EMSCRIPTEN__
    EM_ASM({ globalThis.EaglerTouhouGameExited?.($0); }, static_cast<int>(result));
#endif
}
