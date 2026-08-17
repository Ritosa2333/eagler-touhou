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
#include "Ending.hpp"
#include "FileSystem.hpp"
#include "GameErrorContext.hpp"
#include "GameWindow.hpp"
#include "GameManager.hpp"
#include "AsciiManager.hpp"
#include "BulletManager.hpp"
#include "Gui.hpp"
#include "ItemManager.hpp"
#include "Localization.hpp"
#include "MainMenu.hpp"
#include "Player.hpp"
#include "PracticeRuntime.hpp"
#include "ReplayManager.hpp"
#include "ResultScreen.hpp"
#include "SoundPlayer.hpp"
#include "Supervisor.hpp"
#include "TextHelper.hpp"
#include "Touch.hpp"
#include "ZunResult.hpp"
#include "i18n.hpp"

#ifdef TH_ENABLE_THPRAC
#include "ThpracImGui.hpp"
#endif

static RenderResult renderResult = RENDER_RESULT_KEEP_RUNNING;
static bool g_ToggleFullscreenRequested = false;
static bool g_AudioSuspendedByFocus = false;
static bool g_OpenMusicRoomForVisualTest = false;
static bool g_MusicRoomVisualTestDispatched = false;
#ifdef TH_DEV_TOOLS
static bool g_StartStage1ForVisualTest = false;
static bool g_ShowStage1TextForVisualTest = false;
static bool g_ShowStage1SpellForVisualTest = false;
static bool g_ShowStage1BombForVisualTest = false;
static bool g_ShowStage1DialogueForVisualTest = false;
static bool g_Stage1VisualTestDispatched = false;
static bool g_Stage1TextVisualTestDispatched = false;
static bool g_Stage1SpellVisualTestDispatched = false;
static bool g_Stage1BombVisualTestDispatched = false;
static bool g_Stage1DialogueVisualTestDispatched = false;
static bool g_TouchStateSelfTest = false;
static bool g_OpenEndingForVisualTest = false;
static bool g_EndingVisualTestDispatched = false;
static int g_EndingVisualTestFrames = 0;
static bool g_ResultSpellAudit = false;
static bool g_ResultSpellAuditDispatched = false;
static int g_ResultSpellAuditFrames = 0;
static bool g_TimeStopInterpolationSelfTest = false;
#ifdef TH_ENABLE_THCRAP
static bool g_ThcrapFontMetricsSelfTest = false;
static bool g_ThcrapAsciiSelfTest = false;
static bool g_ThcrapAsciiFormatSelfTest = false;
static bool g_ThcrapStringSelfTest = false;
static bool g_ThcrapEndingSelfTest = false;
static bool g_ThcrapResultStatsAudit = false;
static bool g_ThcrapResultStatsAuditDispatched = false;
static int g_ThcrapResultStatsAuditFrames = 0;
static bool g_ThcrapResultShotTypeAudit = false;
static bool g_ThcrapResultShotTypeAuditDispatched = false;
static int g_ThcrapResultShotTypeAuditFrames = 0;
#endif
#endif
#ifdef TH_ENABLE_THPRAC
static bool g_OpenThpracMenuForVisualTest = false;
static bool g_ThpracMenuVisualTestDispatched = false;
static bool g_AutoStartThpracVisualTest = false;
static int g_ThpracVisualTestFrames = 0;
static bool g_AutoPauseThpracVisualTest = false;
static bool g_ThpracPauseVisualTestDispatched = false;
static bool g_ThpracReplaySelfTest = false;
#ifdef TH_DEV_TOOLS
static const char *g_ThpracReplayLoadSelfTestPath = nullptr;
#endif
static bool g_ThpracRestartSelfTest = false;
static bool g_ThpracImGuiSelfTest = false;
static bool g_ThpracInputSelfTest = false;
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
#ifdef TH_DEV_TOOLS
        else if (std::strcmp(argv[index], "--stage1") == 0)
            g_StartStage1ForVisualTest = g_ShowStage1TextForVisualTest = g_ShowStage1SpellForVisualTest = true;
        else if (std::strcmp(argv[index], "--stage1-text") == 0)
            g_StartStage1ForVisualTest = g_ShowStage1TextForVisualTest = true;
        else if (std::strcmp(argv[index], "--stage1-spell") == 0)
            g_StartStage1ForVisualTest = g_ShowStage1SpellForVisualTest = true;
        else if (std::strcmp(argv[index], "--stage1-bomb") == 0)
            g_StartStage1ForVisualTest = g_ShowStage1BombForVisualTest = true;
        else if (std::strcmp(argv[index], "--stage1-dialogue") == 0)
            g_StartStage1ForVisualTest = g_ShowStage1DialogueForVisualTest = true;
        else if (std::strcmp(argv[index], "--ending-reimu-a") == 0)
            g_OpenEndingForVisualTest = true;
        else if (std::strcmp(argv[index], "--touch-selftest") == 0)
            g_TouchStateSelfTest = true;
        else if (std::strcmp(argv[index], "--result-spells-audit") == 0)
            g_ResultSpellAudit = true;
        else if (std::strcmp(argv[index], "--time-stop-interp-selftest") == 0)
            g_TimeStopInterpolationSelfTest = true;
#ifdef TH_ENABLE_THCRAP
        else if (std::strcmp(argv[index], "--thcrap-font-selftest") == 0)
            g_ThcrapFontMetricsSelfTest = true;
        else if (std::strcmp(argv[index], "--thcrap-ascii-selftest") == 0)
            g_ThcrapAsciiSelfTest = true;
        else if (std::strcmp(argv[index], "--thcrap-ascii-format-selftest") == 0)
            g_ThcrapAsciiFormatSelfTest = true;
        else if (std::strcmp(argv[index], "--thcrap-strings-selftest") == 0)
            g_ThcrapStringSelfTest = true;
        else if (std::strcmp(argv[index], "--thcrap-ending-selftest") == 0)
            g_ThcrapEndingSelfTest = true;
        else if (std::strcmp(argv[index], "--thcrap-result-stats") == 0)
            g_ThcrapResultStatsAudit = true;
        else if (std::strcmp(argv[index], "--thcrap-result-shottype") == 0)
            g_ThcrapResultShotTypeAudit = true;
        else if (std::strcmp(argv[index], "--thcrap-result-spells") == 0)
            g_ResultSpellAudit = true;
#endif
#endif
#ifdef TH_ENABLE_THPRAC
        else if (std::strcmp(argv[index], "--thprac-menu") == 0)
            g_OpenThpracMenuForVisualTest = true;
        else if (std::strcmp(argv[index], "--thprac-run") == 0)
            g_OpenThpracMenuForVisualTest = g_AutoStartThpracVisualTest = true;
        else if (std::strcmp(argv[index], "--thprac-pause") == 0)
            g_OpenThpracMenuForVisualTest = g_AutoStartThpracVisualTest = g_AutoPauseThpracVisualTest = true;
        else if (std::strcmp(argv[index], "--thprac-replay-selftest") == 0)
            g_ThpracReplaySelfTest = true;
#ifdef TH_DEV_TOOLS
        else if (std::strcmp(argv[index], "--thprac-replay-load-selftest") == 0 && index + 1 < argc)
            g_ThpracReplayLoadSelfTestPath = argv[++index];
#endif
        else if (std::strcmp(argv[index], "--thprac-restart-selftest") == 0)
            g_ThpracRestartSelfTest = true;
        else if (std::strcmp(argv[index], "--thprac-imgui-selftest") == 0)
            g_ThpracImGuiSelfTest = true;
        else if (std::strcmp(argv[index], "--thprac-input-selftest") == 0)
            g_ThpracInputSelfTest = true;
#endif
#ifdef __EMSCRIPTEN__
    g_OpenMusicRoomForVisualTest = EM_ASM_INT({ return Module.eaglerOptions?.debugHarness === 'music-room'; }) != 0;
#if defined(TH_DEV_TOOLS) && defined(TH_ENABLE_THCRAP)
    g_ThcrapFontMetricsSelfTest = EM_ASM_INT({ return Module.eaglerOptions?.debugHarness === 'font-metrics'; }) != 0;
    g_ThcrapAsciiSelfTest = EM_ASM_INT({ return Module.eaglerOptions?.debugHarness === 'ascii'; }) != 0;
#endif
#ifdef TH_ENABLE_THPRAC
    g_OpenThpracMenuForVisualTest = EM_ASM_INT({ return Module.eaglerOptions?.debugHarness === 'thprac-menu'; }) != 0;
#endif
#endif

#if defined(TH_DEV_TOOLS) && defined(TH_ENABLE_THCRAP)
    if (g_ThcrapAsciiFormatSelfTest)
    {
        const bool passed = AsciiManager::DebugLocalizedFormatSelfTest();
        SDL_Log("th06 thcrap legacy ASCII formatter self-test: %s", passed ? "PASS" : "FAIL");
        return passed ? SDL_APP_SUCCESS : SDL_APP_FAILURE;
    }
    if (g_ThcrapStringSelfTest)
    {
        bool passed = Localization::DebugStringTableSelfTest();
        if (passed)
        {
            // Exercise the actual variadic Log/Fatal production entry points,
            // not only Localization::LogString().  base_tsa installs
            // strings_lookup#cavesize_6 on both of these functions.
            static const char unableToReadFallback[] = "%sが読み込めないです。\n";
            const char *expectedLogFmt = Localization::LogString(unableToReadFallback);
            g_GameErrorContext.ResetContext();
            g_GameErrorContext.m_ShowMessageBox = false;
            const char *usedLogFmt = g_GameErrorContext.Log(unableToReadFallback, "audit.dat");
            passed = usedLogFmt == expectedLogFmt &&
                     std::strstr(g_GameErrorContext.m_Buffer, "audit.dat") != nullptr;

            static const char twoInstancesFallback[] = "二つは起動できません\n";
            const char *expectedFatalFmt = Localization::LogString(twoInstancesFallback);
            g_GameErrorContext.ResetContext();
            g_GameErrorContext.m_ShowMessageBox = false;
            const char *usedFatalFmt = g_GameErrorContext.Fatal(twoInstancesFallback);
            passed = passed && usedFatalFmt == expectedFatalFmt &&
                     g_GameErrorContext.m_Buffer[0] != '\0' &&
                     g_GameErrorContext.m_ShowMessageBox;

            static const char unknownFallback[] = "TH06 diagnostic audit %s\n";
            g_GameErrorContext.ResetContext();
            g_GameErrorContext.m_ShowMessageBox = false;
            const char *usedUnknownFmt = g_GameErrorContext.Log(unknownFallback, "fallback");
            passed = passed && usedUnknownFmt == unknownFallback &&
                     std::strstr(g_GameErrorContext.m_Buffer, "TH06 diagnostic audit fallback") != nullptr;
            g_GameErrorContext.ResetContext();
            g_GameErrorContext.m_ShowMessageBox = false;
        }
        SDL_Log("th06 thcrap EST1 loader self-test: %s", passed ? "PASS" : "FAIL");
        return passed ? SDL_APP_SUCCESS : SDL_APP_FAILURE;
    }
    if (g_ThcrapEndingSelfTest)
    {
        const bool passed = Ending::DebugTranslatedLineSelfTest();
        SDL_Log("th06 thcrap ending direct-line self-test: %s", passed ? "PASS" : "FAIL");
        return passed ? SDL_APP_SUCCESS : SDL_APP_FAILURE;
    }
    if (g_ThcrapAsciiSelfTest)
    {
        const bool passed = Localization::DebugAsciiTableSelfTest();
        SDL_Log("th06 thcrap EAS1 loader self-test: %s", passed ? "PASS" : "FAIL");
        return passed ? SDL_APP_SUCCESS : SDL_APP_FAILURE;
    }
    if (g_ThcrapFontMetricsSelfTest)
    {
        const bool passed = TextHelper::DebugLocalizedFontMetricsSelfTest();
        SDL_Log("th06 thcrap localized SDL_ttf font metrics self-test: %s", passed ? "PASS" : "FAIL");
        return passed ? SDL_APP_SUCCESS : SDL_APP_FAILURE;
    }
#endif

#ifdef TH_DEV_TOOLS
    if (g_TimeStopInterpolationSelfTest)
    {
        g_GameManager.isTimeStopped = 1;

        Bullet &bullet = g_BulletManager.bullets[0];
        bullet.state = 1;
        bullet.pos = ZunVec3(123.0f, 234.0f, 0.1f);
        bullet.prevPos = ZunVec3(-10.0f, -20.0f, 0.1f);
        bullet.angle = 2.4f;
        bullet.prevAngle = -1.1f;
        bullet.sprites.spriteBullet.rotation.z = 0.75f;
        bullet.sprites.spriteBullet.prevRotation.z = -0.25f;

        Laser &laser = g_BulletManager.lasers[0];
        laser.inUse = 1;
        laser.pos = ZunVec3(30.0f, 40.0f, 0.0f);
        laser.prevPos = ZunVec3(-30.0f, -40.0f, 0.0f);
        laser.angle = 1.6f;
        laser.prevAngle = -2.0f;
        laser.startOffset = 12.0f;
        laser.prevStartOffset = -12.0f;
        laser.endOffset = 80.0f;
        laser.prevEndOffset = 4.0f;

        Item &item = g_ItemManager.items[0];
        item.isInUse = 1;
        item.currentPosition = ZunVec3(55.0f, 66.0f, 0.0f);
        item.prevPosition = ZunVec3(-55.0f, -66.0f, 0.0f);

        g_Player.positionCenter = ZunVec3(192.0f, 384.0f, 0.0f);
        g_Player.prevPositionCenter = ZunVec3(100.0f, 300.0f, 0.0f);
        PlayerBullet &playerBullet = g_Player.bullets[0];
        playerBullet.bulletState = BULLET_STATE_FIRED;
        playerBullet.position = ZunVec3(200.0f, 250.0f, 0.495f);
        playerBullet.prevPosition = ZunVec3(10.0f, 20.0f, 0.495f);

        Player::OnUpdate(&g_Player);
        BulletManager::OnUpdate(&g_BulletManager);

        const bool playerFrozen = g_Player.prevPositionCenter.x == g_Player.positionCenter.x &&
                                  g_Player.prevPositionCenter.y == g_Player.positionCenter.y &&
                                  playerBullet.prevPosition.x == playerBullet.position.x &&
                                  playerBullet.prevPosition.y == playerBullet.position.y;
        const bool bulletFrozen = bullet.prevPos.x == bullet.pos.x && bullet.prevPos.y == bullet.pos.y &&
                                  bullet.prevAngle == bullet.angle &&
                                  bullet.sprites.spriteBullet.prevRotation.z == bullet.sprites.spriteBullet.rotation.z;
        const bool laserFrozen = laser.prevPos.x == laser.pos.x && laser.prevPos.y == laser.pos.y &&
                                 laser.prevAngle == laser.angle && laser.prevStartOffset == laser.startOffset &&
                                 laser.prevEndOffset == laser.endOffset;
        const bool itemFrozen = item.prevPosition.x == item.currentPosition.x &&
                                item.prevPosition.y == item.currentPosition.y;
        const bool passed = playerFrozen && bulletFrozen && laserFrozen && itemFrozen;
        SDL_Log("th06 Sakuya time-stop interpolation self-test: player=%s bullet=%s laser=%s item=%s overall=%s",
                playerFrozen ? "PASS" : "FAIL", bulletFrozen ? "PASS" : "FAIL",
                laserFrozen ? "PASS" : "FAIL", itemFrozen ? "PASS" : "FAIL", passed ? "PASS" : "FAIL");
        return passed ? SDL_APP_SUCCESS : SDL_APP_FAILURE;
    }
    if (g_TouchStateSelfTest)
    {
        const bool passed = Touch::DebugStateSelfTest();
        SDL_Log("th06 touch finger-state self-test: %s", passed ? "PASS" : "FAIL");
        return passed ? SDL_APP_SUCCESS : SDL_APP_FAILURE;
    }
#endif

    if (g_Supervisor.LoadConfig(TH_CONFIG_FILE) != ZUN_SUCCESS)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "th06: configuration initialization failed");
        return SDL_APP_FAILURE;
    }
#ifdef TH_ENABLE_THPRAC
#ifdef TH_DEV_TOOLS
    if (g_ThpracReplayLoadSelfTestPath != nullptr)
    {
        ReplayHeader *replay = reinterpret_cast<ReplayHeader *>(FileSystem::OpenPath(g_ThpracReplayLoadSelfTestPath, 1));
        const i32 replaySize = g_LastFileSize;
        const bool replayValid = replay != nullptr &&
            ReplayManager::ValidateReplayData(replay, replaySize) == ZUN_SUCCESS;
        std::free(replay);
        const bool loaded = PracticeRuntime::LoadReplayMetadata(g_ThpracReplayLoadSelfTestPath);
        const PracticeRuntime::Config &config = PracticeRuntime::GetConfig();
        const bool passed = replayValid && loaded && config.active && config.mode != 0;
        SDL_Log("th06 thprac upstream replay load self-test: %s path=%s replay=%s mode=%d stage=%d section=%d",
                passed ? "PASS" : "FAIL", g_ThpracReplayLoadSelfTestPath,
                replayValid ? "VALID" : "INVALID",
                config.mode, config.stage, config.section);
        return passed ? SDL_APP_SUCCESS : SDL_APP_FAILURE;
    }
#endif
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
    if (g_ThpracImGuiSelfTest)
    {
        const bool lifecyclePassed = ThpracImGui::DebugLifecycleSelfTest();
        const bool resultRoutingPassed = ResultScreen::DebugThpracResultRoutingSelfTest();
        SDL_Log("th06 thprac Result routing self-test: %s", resultRoutingPassed ? "PASS" : "FAIL");
        const bool sectionPassed = PracticeRuntime::DebugSectionCatalogSelfTest() && resultRoutingPassed;
        const bool passed = lifecyclePassed && sectionPassed;
        SDL_Log("th06 thprac ImGui/font + section-catalog self-test: lifecycle=%s sections=%s overall=%s",
                lifecyclePassed ? "PASS" : "FAIL", sectionPassed ? "PASS" : "FAIL",
                passed ? "PASS" : "FAIL");
        return passed ? SDL_APP_SUCCESS : SDL_APP_FAILURE;
    }
    if (g_ThpracInputSelfTest)
    {
        const bool passed = ThpracImGui::DebugInputSamplingSelfTest();
        SDL_Log("th06 thprac Gen1 input sampling self-test: %s", passed ? "PASS" : "FAIL");
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
#ifdef TH_DEV_TOOLS
    if (g_OpenEndingForVisualTest && !g_EndingVisualTestDispatched && g_MainMenu.chainCalc != nullptr)
    {
        // Developer audit only. Seed the same minimum fields a no-continue
        // Normal Reimu-A clear carries into the production supervisor Ending
        // transition. Registration, AddedCallback, END parsing, ANM and text
        // drawing remain the real game path.
        g_GameManager.character = CHARA_REIMU;
        g_GameManager.shotType = SHOT_TYPE_A;
        g_GameManager.difficulty = NORMAL;
        g_GameManager.numRetries = 0;
        g_GameManager.clrd[0].difficultyClearedWithRetries[NORMAL] = 99;
        g_GameManager.clrd[0].difficultyClearedWithoutRetries[NORMAL] = 99;
        ChainElem *mainMenuCalc = g_MainMenu.chainCalc;
        g_Chain.Cut(mainMenuCalc);
        g_MainMenu.chainCalc = nullptr;
        Ending::DebugSetFastForward(true);
        // This is the exact state pair consumed by Supervisor::OnUpdate after
        // gameplay sets curState=ENDING and returns to the main-menu target.
        g_Supervisor.curState = SUPERVISOR_STATE_ENDING;
        g_Supervisor.wantedState = SUPERVISOR_STATE_MAINMENU;
        g_EndingVisualTestDispatched = true;
        g_EndingVisualTestFrames = 0;
        SDL_Log("TH06 ending audit: dispatched real Reimu-A Ending transition localization=%d",
                Localization::Active() ? 1 : 0);
    }
    if (g_EndingVisualTestDispatched && ++g_EndingVisualTestFrames == 240)
    {
        SDL_Log("TH06 ending audit: PASS window completed localization=%d",
                Localization::Active() ? 1 : 0);
        return SDL_APP_SUCCESS;
    }
#ifdef TH_ENABLE_THCRAP
    if (g_ThcrapResultStatsAudit && !g_ThcrapResultStatsAuditDispatched && g_MainMenu.chainCalc != nullptr)
    {
        ChainElem *mainMenuCalc = g_MainMenu.chainCalc;
        g_Chain.Cut(mainMenuCalc);
        g_MainMenu.chainCalc = nullptr;
        g_Supervisor.framerateMultiplier = 1.0f;
        if (ResultScreen::DebugRegisterStatsAudit() != ZUN_SUCCESS)
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TH06 thcrap result stats audit: failed to register");
            return SDL_APP_FAILURE;
        }
        g_Supervisor.curState = SUPERVISOR_STATE_RESULTSCREEN_FROMGAME;
        g_Supervisor.wantedState = SUPERVISOR_STATE_RESULTSCREEN_FROMGAME;
        g_ThcrapResultStatsAuditDispatched = true;
        g_ThcrapResultStatsAuditFrames = 0;
    }
    if (g_ThcrapResultStatsAuditDispatched && ++g_ThcrapResultStatsAuditFrames == 180)
    {
        ResultScreen::DebugCloseStatsAudit();
        SDL_Log("TH06 thcrap result stats audit: PASS window completed");
        return SDL_APP_SUCCESS;
    }
    if (g_ThcrapResultShotTypeAudit && !g_ThcrapResultShotTypeAuditDispatched && g_MainMenu.chainCalc != nullptr)
    {
        ChainElem *mainMenuCalc = g_MainMenu.chainCalc;
        g_Chain.Cut(mainMenuCalc);
        g_MainMenu.chainCalc = nullptr;
        g_Supervisor.framerateMultiplier = 1.0f;
        if (ResultScreen::DebugRegisterShotTypeAudit() != ZUN_SUCCESS)
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TH06 thcrap result shot-type audit: failed to register");
            return SDL_APP_FAILURE;
        }
        g_Supervisor.curState = SUPERVISOR_STATE_RESULTSCREEN_FROMGAME;
        g_Supervisor.wantedState = SUPERVISOR_STATE_RESULTSCREEN_FROMGAME;
        g_ThcrapResultShotTypeAuditDispatched = true;
        g_ThcrapResultShotTypeAuditFrames = 0;
    }
    if (g_ThcrapResultShotTypeAuditDispatched && ++g_ThcrapResultShotTypeAuditFrames == 90)
    {
        ResultScreen::DebugCloseStatsAudit();
        SDL_Log("TH06 thcrap result shot-type audit: PASS window completed");
        return SDL_APP_SUCCESS;
    }
#endif
    if (g_ResultSpellAudit && !g_ResultSpellAuditDispatched && g_MainMenu.chainCalc != nullptr)
    {
        ChainElem *mainMenuCalc = g_MainMenu.chainCalc;
        g_Chain.Cut(mainMenuCalc);
        g_MainMenu.chainCalc = nullptr;
        g_Supervisor.framerateMultiplier = 1.0f;
        if (ResultScreen::DebugRegisterSpellAudit() != ZUN_SUCCESS)
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TH06 result spell audit: failed to register");
            return SDL_APP_FAILURE;
        }
        g_Supervisor.curState = SUPERVISOR_STATE_RESULTSCREEN_FROMGAME;
        g_Supervisor.wantedState = SUPERVISOR_STATE_RESULTSCREEN_FROMGAME;
        g_ResultSpellAuditDispatched = true;
        g_ResultSpellAuditFrames = 0;
    }
    if (g_ResultSpellAuditDispatched && ++g_ResultSpellAuditFrames == 120)
    {
        ResultScreen::DebugCloseStatsAudit();
        SDL_Log("TH06 result spell audit: PASS window completed localization=%d",
                Localization::Active() ? 1 : 0);
        return SDL_APP_SUCCESS;
    }
    if (g_StartStage1ForVisualTest && !g_Stage1VisualTestDispatched && g_MainMenu.chainCalc != nullptr)
    {
        g_MainMenu.chainCalc->callback = reinterpret_cast<ChainCallback>(MainMenu::DebugStartStage1);
        g_Stage1VisualTestDispatched = true;
    }
    if (g_StartStage1ForVisualTest && g_Stage1VisualTestDispatched && g_Gui.impl != nullptr)
    {
        if (g_ShowStage1TextForVisualTest && !g_Stage1TextVisualTestDispatched && g_GameManager.gameFrames >= 180)
        {
            g_Gui.DebugShowLocalizedStageText();
            g_Stage1TextVisualTestDispatched = true;
        }
        if (g_ShowStage1SpellForVisualTest && !g_Stage1SpellVisualTestDispatched && g_GameManager.gameFrames >= 180)
        {
            g_Gui.DebugShowLocalizedSpellcard();
            g_Stage1SpellVisualTestDispatched = true;
        }
        if (g_ShowStage1BombForVisualTest && !g_Stage1BombVisualTestDispatched && g_GameManager.gameFrames >= 180)
        {
            g_Gui.DebugShowLocalizedBomb();
            g_Stage1BombVisualTestDispatched = true;
        }
        if (g_ShowStage1DialogueForVisualTest && !g_Stage1DialogueVisualTestDispatched &&
            g_GameManager.gameFrames >= 180)
        {
            g_Gui.DebugStartStage1BossDialogue();
            g_Stage1DialogueVisualTestDispatched = true;
        }
    }
#endif
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
        ++g_ThpracVisualTestFrames == 1)
    {
#ifdef TH_DEV_TOOLS
        // Explicit developer harness only. The real desktop entry remains
        // vanilla Practice -> character/shot -> stage-selection position.
        PracticeRuntime::DebugLoadSessionFile("thprac-session.json");
#endif
    }
    if (g_AutoStartThpracVisualTest && g_ThpracMenuVisualTestDispatched &&
        g_ThpracVisualTestFrames == 30)
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
#ifdef TH_ENABLE_THPRAC
    ThpracImGui::ProcessEvent(*event);
#endif
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
#if defined(TH_DEV_TOOLS) && !defined(TH_ENABLE_THPRAC)
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
#ifdef TH_ENABLE_THPRAC
    ThpracImGui::Shutdown();
#endif
    delete g_GfxBackend;
    g_GfxBackend = nullptr;

    if (g_GameWindow.window)
    {
        SDL_DestroyWindow(g_GameWindow.window);
        g_GameWindow.window = nullptr;
    }

#ifdef TH_DEV_TOOLS
    if (!g_TouchStateSelfTest)
#endif
    {
        FileSystem::WriteDataToFile(TH_CONFIG_FILE, &g_Supervisor.cfg, sizeof(g_Supervisor.cfg));
    }
    SDL_ShowCursor();
#ifdef TH_DEV_TOOLS
    if (!g_TouchStateSelfTest)
#endif
    {
        g_GameErrorContext.Flush();
    }
#ifdef __EMSCRIPTEN__
    EM_ASM({ globalThis.EaglerTouhouGameExited?.($0); }, static_cast<int>(result));
#endif
}
