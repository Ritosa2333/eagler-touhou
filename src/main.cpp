#include <SDL3/SDL.h>
#include <cstdio>
#include <cstring>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

// pull in gameerrorcontext::flush before anmmanager::releasesurfaces
#include "AnmManager.hpp"
#include "AsciiManager.hpp"
#include "Chain.hpp"
#include "Controller.hpp"
#include "EaglerOptions.hpp"
#include "Ending.hpp"
#include "FileSystem.hpp"
#include "GameErrorContext.hpp"
#include "GameManager.hpp"
#include "GameWindow.hpp"
#include "Localization.hpp"
#include "MainMenu.hpp"
#include "PracticeRuntime.hpp"
#include "ReplayExtension.hpp"
#include "ResultScreen.hpp"
#include "Rng.hpp"
#include "SoundPlayer.hpp"
#include "Supervisor.hpp"
#include "TextHelper.hpp"
#include "Touch.hpp"
#include "ZunResult.hpp"
#include "dxutil.hpp"
#ifdef TH_ENABLE_NETPLAY
#include "netplay/Th07LanStageProbe.hpp"
#endif
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#include "multiplayer/GameplaySession.hpp"
#endif
#ifdef TH_ENABLE_THPRAC
#include "ThpracImGui.hpp"
#endif

static i32 renderRes = RENDER_RESULT_KEEP_RUNNING;
static bool g_AudioSuspendedByFocus = false;
static bool g_ToggleFullscreenRequested = false;
#ifdef TH_ENABLE_THPRAC
static bool g_OpenThpracMenuForVisualTest = false;
static bool g_ThpracMenuVisualTestDispatched = false;
static bool g_AutoStartThpracVisualTest = false;
static bool g_AutoPauseThpracVisualTest = false;
static bool g_ThpracPauseVisualTestDispatched = false;
static int g_ThpracVisualTestFrames = 0;
#endif
static bool g_OpenMusicRoomForVisualTest = false;
static bool g_MusicRoomVisualTestDispatched = false;
#ifdef TH_ENABLE_NETPLAY
static bool g_NetplayStage1Harness = false;
static bool g_NetplayDualStage1Harness = false;
static bool g_NetplayLanStage1Harness = false;
static bool g_NetplayLanProduction = false;
static bool g_NetplayStage1HarnessDispatched = false;
static bool g_NetplayStage1HarnessPrepared = false;
static bool g_DemoDeterminismHarness = false;
static bool g_DemoDeterminismHarnessDispatched = false;
static i32 g_DemoDeterminismIndex = 0;
#endif
#ifdef TH_DEV_TOOLS
static i32 g_StageVisualTestIndex = -1;
static bool g_Stage1VisualTestDispatched = false;
static bool g_Stage1VisualTestPrepared = false;
static bool g_OpenResultSpellsForVisualTest = false;
static bool g_ResultSpellsVisualTestDispatched = false;
static bool g_OpenResultStatsForVisualTest = false;
static bool g_ResultStatsVisualTestDispatched = false;
static bool g_OpenResultStatsPhantasmForVisualTest = false;
static bool g_OpenEndingForVisualTest = false;
static i32 g_EndingViewerSelection = -1;
static bool g_EndingVisualTestDispatched = false;
static i32 g_CharacterSelectVisualTestIndex = -1;
static bool g_CharacterSelectVisualTestDispatched = false;
static i32 g_MenuStringVisualTestState = -1;
static bool g_MenuStringVisualTestDispatched = false;
static bool g_TouchStateSelfTest = false;
static bool g_ReplayExtensionSelfTest = false;
#ifdef TH_ENABLE_THCRAP
static bool g_ThcrapFontMetricsSelfTest = false;
static bool g_ThcrapTextImageSelfTest = false;
static bool g_ThcrapAsciiSelfTest = false;
static bool g_ThcrapStringSelfTest = false;
static bool g_ThcrapAsciiFormatSelfTest = false;
static bool g_ThcrapLayoutSelfTest = false;
static bool g_ThcrapEndingSelfTest = false;
#endif
#endif
#ifdef TH_ENABLE_THPRAC
static bool g_ThpracReplaySelfTest = false;
#endif

static void SuspendAudioForInactiveWindow()
{
    if (g_AudioSuspendedByFocus)
    {
        return;
    }
#ifdef __EMSCRIPTEN__
    g_SoundPlayer.SetWebAudioWindowActive(false);
#else
    if (g_SoundPlayer.engine != nullptr)
    {
        ma_engine_stop(g_SoundPlayer.engine);
    }
#endif
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
#ifdef __EMSCRIPTEN__
    g_SoundPlayer.SetWebAudioWindowActive(true);
#else
    if (g_SoundPlayer.engine != nullptr)
    {
        ma_engine_start(g_SoundPlayer.engine);
    }
#endif
    if (g_Supervisor.midiOutput != nullptr)
    {
        g_Supervisor.midiOutput->SetPaused(false);
    }
    g_AudioSuspendedByFocus = false;
}

#ifdef __EMSCRIPTEN__
extern "C" EMSCRIPTEN_KEEPALIVE void TouhouWebSetAudioActive(i32 active)
{
    if (active)
        ResumeAudioForActiveWindow();
    else
        SuspendAudioForInactiveWindow();
}
#endif

void AnmManager::TakeScreenshotIfRequested()
{
    if (this->screenshotTextureId >= 0)
    {
        Flush();

        TakeScreenshot(this->screenshotTextureId, this->screenshotSrcLeft, this->screenshotSrcTop,
                       this->screenshotSrcWidth, this->screenshotSrcHeight, this->screenshotDstLeft,
                       this->screenshotDstTop, this->screenshotDstWidth, this->screenshotDstHeight);
        this->screenshotTextureId = -1;
    }
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)
{
    for (int index = 1; index < argc; index++)
        if (std::strcmp(argv[index], "--music-room") == 0)
            g_OpenMusicRoomForVisualTest = true;
#ifdef TH_DEV_TOOLS
        else if (std::strcmp(argv[index], "--stage1") == 0)
            g_StageVisualTestIndex = 0;
        else if (std::strcmp(argv[index], "--stage4") == 0)
            g_StageVisualTestIndex = 3;
        else if (std::strcmp(argv[index], "--result-spells") == 0)
            g_OpenResultSpellsForVisualTest = true;
        else if (std::strcmp(argv[index], "--result-stats") == 0)
            g_OpenResultStatsForVisualTest = true;
        else if (std::strcmp(argv[index], "--result-stats-phantasm") == 0)
            g_OpenResultStatsPhantasmForVisualTest = true;
        else if (std::strcmp(argv[index], "--ending-reimu-a") == 0)
            g_OpenEndingForVisualTest = true;
        else if (std::strcmp(argv[index], "--character-select-reimu") == 0)
            g_CharacterSelectVisualTestIndex = CHAR_REIMU;
        else if (std::strcmp(argv[index], "--character-select-marisa") == 0)
            g_CharacterSelectVisualTestIndex = CHAR_MARISA;
        else if (std::strcmp(argv[index], "--character-select-sakuya") == 0)
            g_CharacterSelectVisualTestIndex = CHAR_SAKUYA;
        else if (std::strcmp(argv[index], "--options-menu-audit") == 0)
            g_MenuStringVisualTestState = MENU_STATE_OPTIONS;
        else if (std::strcmp(argv[index], "--key-config-menu-audit") == 0)
            g_MenuStringVisualTestState = MENU_STATE_KEY_CONFIG;
        else if (std::strcmp(argv[index], "--touch-selftest") == 0)
            g_TouchStateSelfTest = true;
        else if (std::strcmp(argv[index], "--replay-extension-selftest") == 0)
            g_ReplayExtensionSelfTest = true;
#ifdef TH_ENABLE_THCRAP
        else if (std::strcmp(argv[index], "--thcrap-font-selftest") == 0)
            g_ThcrapFontMetricsSelfTest = true;
        else if (std::strcmp(argv[index], "--thcrap-textimage-selftest") == 0)
            g_ThcrapTextImageSelfTest = true;
        else if (std::strcmp(argv[index], "--thcrap-ascii-selftest") == 0)
            g_ThcrapAsciiSelfTest = true;
        else if (std::strcmp(argv[index], "--thcrap-string-selftest") == 0)
            g_ThcrapStringSelfTest = true;
        else if (std::strcmp(argv[index], "--thcrap-ascii-format-selftest") == 0)
            g_ThcrapAsciiFormatSelfTest = true;
        else if (std::strcmp(argv[index], "--thcrap-layout-selftest") == 0)
            g_ThcrapLayoutSelfTest = true;
        else if (std::strcmp(argv[index], "--thcrap-ending-selftest") == 0)
            g_ThcrapEndingSelfTest = true;
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
#endif

#if defined(TH_DEV_TOOLS) && defined(__EMSCRIPTEN__)
    // Hosted/browser audit paths already communicate through eaglerOptions.
    // SDL3's callback-main wrapper does not preserve Module.callMain argv in
    // the same shape as the native test executable, so bind this Web-only
    // visual audit directly to the hidden debugHarness instead of changing the
    // production launch argument contract.
    if (EM_ASM_INT({
            return Module.eaglerOptions?.debugHarness === 'result-stats-phantasm' ? 1 : 0;
        }))
        g_OpenResultStatsPhantasmForVisualTest = true;
#endif
#ifdef __EMSCRIPTEN__
    g_OpenMusicRoomForVisualTest = EM_ASM_INT({ return Module.eaglerOptions?.debugHarness === 'music-room'; }) != 0;
#ifdef TH_ENABLE_NETPLAY
    g_NetplayDualStage1Harness = EM_ASM_INT({
        return Module.eaglerOptions?.debugHarness === 'netplay-dual-stage1' ? 1 : 0;
    }) != 0;
    g_NetplayLanStage1Harness = EM_ASM_INT({
        return Module.eaglerOptions?.debugHarness === 'netplay-lan-stage1' ? 1 : 0;
    }) != 0;
    g_NetplayLanProduction = EM_ASM_INT({
        return Module.eaglerOptions?.netplayMode === 'lan' ? 1 : 0;
    }) != 0;
    g_NetplayStage1Harness = g_NetplayDualStage1Harness || g_NetplayLanStage1Harness ||
        g_NetplayLanProduction || EM_ASM_INT({
        return Module.eaglerOptions?.debugHarness === 'netplay-stage1' ? 1 : 0;
    }) != 0;
    g_DemoDeterminismHarness = EM_ASM_INT({
        return Module.eaglerOptions?.debugHarness === 'demo-determinism' ? 1 : 0;
    }) != 0;
    if (g_DemoDeterminismHarness)
    {
        g_DemoDeterminismIndex = EM_ASM_INT({
            const index = Number(Module.eaglerOptions?.demoDeterminismIndex ?? 0);
            return Number.isInteger(index) && index >= 0 && index <= 2 ? index : 0;
        });
    }
    if (g_NetplayLanProduction)
        std::printf("th07 netplay: LAN session requested\n");
    else if (g_NetplayStage1Harness)
        std::printf("th07 netplay audit: app init harness requested\n");
#endif
#ifdef TH_DEV_TOOLS
    g_ReplayExtensionSelfTest = g_ReplayExtensionSelfTest || EM_ASM_INT({
        return Module.eaglerOptions?.debugHarness === 'replay-extension' ? 1 : 0;
    }) != 0;
    const i32 webStageVisual = EM_ASM_INT({
        const id = Module.eaglerOptions?.debugHarness;
        return id === 'stage1-visual' ? 0 : id === 'stage4-visual' ? 3 : -1;
    });
    if (webStageVisual >= 0) g_StageVisualTestIndex = webStageVisual;
    g_EndingViewerSelection = EM_ASM_INT({
        const id = Module.eaglerOptions?.debugHarness;
        return id === 'ending-reimu-a' ? 0 :
               id === 'ending-reimu-b' ? 1 :
               id === 'ending-marisa-a' ? 2 :
               id === 'ending-marisa-b' ? 3 :
               id === 'ending-sakuya-a' ? 4 :
               id === 'ending-sakuya-b' ? 5 :
               id === 'ending-reimu-bad' ? 6 :
               id === 'ending-marisa-bad' ? 7 :
               id === 'ending-sakuya-bad' ? 8 : -1;
    });
    if (g_EndingViewerSelection >= 0)
        g_OpenEndingForVisualTest = true;
#endif
#ifdef TH_ENABLE_THPRAC
    g_OpenThpracMenuForVisualTest = EM_ASM_INT({ return Module.eaglerOptions?.debugHarness === 'thprac-menu'; }) != 0;
#endif
#endif

#if defined(TH_DEV_TOOLS) && defined(TH_ENABLE_THCRAP)
    if (g_ThcrapAsciiFormatSelfTest)
    {
        const bool passed = AsciiManager::DebugLocalizedFormatSelfTest();
        SDL_Log("th07 thcrap legacy ASCII formatter self-test: %s", passed ? "PASS" : "FAIL");
        return passed ? SDL_APP_SUCCESS : SDL_APP_FAILURE;
    }
    if (g_ThcrapEndingSelfTest)
    {
        const bool passed = Ending::DebugTranslatedLineSelfTest();
        SDL_Log("th07 thcrap ending direct-line self-test: %s", passed ? "PASS" : "FAIL");
        return passed ? SDL_APP_SUCCESS : SDL_APP_FAILURE;
    }
    if (g_ThcrapLayoutSelfTest)
    {
        const bool passed = TextHelper::DebugLayoutSelfTest();
        SDL_Log("th07 thcrap persistent layout self-test: %s", passed ? "PASS" : "FAIL");
        return passed ? SDL_APP_SUCCESS : SDL_APP_FAILURE;
    }
    if (g_ThcrapAsciiSelfTest)
    {
        const bool passed = Localization::DebugAsciiTableSelfTest();
        SDL_Log("th07 thcrap EAS1 loader self-test: %s", passed ? "PASS" : "FAIL");
        return passed ? SDL_APP_SUCCESS : SDL_APP_FAILURE;
    }
    if (g_ThcrapStringSelfTest)
    {
        const bool passed = Localization::DebugStringTableSelfTest();
        SDL_Log("th07 thcrap EST1 loader self-test: %s", passed ? "PASS" : "FAIL");
        return passed ? SDL_APP_SUCCESS : SDL_APP_FAILURE;
    }
    if (g_ThcrapTextImageSelfTest)
    {
        const bool passed = Localization::DebugBossImageRowContractSelfTest();
        SDL_Log("th07 thcrap boss textimage row contract: %s", passed ? "PASS" : "FAIL");
        return passed ? SDL_APP_SUCCESS : SDL_APP_FAILURE;
    }
    if (g_ThcrapFontMetricsSelfTest)
    {
        const bool passed = TextHelper::DebugLocalizedFontMetricsSelfTest();
        SDL_Log("th07 thcrap localized SDL_ttf font metrics self-test: %s", passed ? "PASS" : "FAIL");
        return passed ? SDL_APP_SUCCESS : SDL_APP_FAILURE;
    }
#endif

#ifdef TH_DEV_TOOLS
    if (g_TouchStateSelfTest)
    {
        const bool passed = Touch::DebugStateSelfTest();
        SDL_Log("th07 touch finger-state self-test: %s", passed ? "PASS" : "FAIL");
        return passed ? SDL_APP_SUCCESS : SDL_APP_FAILURE;
    }
    if (g_ReplayExtensionSelfTest)
    {
        const bool passed = ReplayExtension::DebugRoundTrip("replay-extension-selftest.rpy");
        SDL_Log("th07 ReplayExtension round-trip self-test: %s", passed ? "PASS" : "FAIL");
        return passed ? SDL_APP_SUCCESS : SDL_APP_FAILURE;
    }
#endif

#ifdef TH_ENABLE_THPRAC
    if (g_ThpracReplaySelfTest)
    {
        const bool passed = PracticeRuntime::DebugReplayMetadataRoundTrip("thprac-replay-selftest.rpy");
        SDL_Log("th07 thprac replay metadata round-trip: %s", passed ? "PASS" : "FAIL");
        return passed ? SDL_APP_SUCCESS : SDL_APP_FAILURE;
    }
#endif

    if (g_Supervisor.LoadConfig("th07.cfg") != ZUN_SUCCESS)
    {
        return SDL_APP_FAILURE;
    }
#ifdef TH_ENABLE_NETPLAY
    if (g_NetplayStage1Harness)
        std::printf("th07 netplay audit: config ready\n");
#endif

    GameWindow::ChecksumExecutable();
    g_GameWindow.frequency = SDL_GetPerformanceFrequency();

start:
    if (GameWindow::CreateGameWindow())
    {
        return SDL_APP_FAILURE;
    }
#ifdef TH_ENABLE_NETPLAY
    if (g_NetplayStage1Harness)
        std::printf("th07 netplay audit: window ready\n");
#endif

    if (GameWindow::InitInterface())
    {
        return SDL_APP_FAILURE;
    }
#ifdef TH_ENABLE_NETPLAY
    if (g_NetplayStage1Harness)
        std::printf("th07 netplay audit: interface ready\n");
#endif

    if (GameWindow::InitRendering())
    {
        return SDL_APP_FAILURE;
    }
#ifdef TH_ENABLE_NETPLAY
    if (g_NetplayStage1Harness)
        std::printf("th07 netplay audit: rendering ready\n");
#endif

    g_SoundPlayer.InitializeSound();
    Controller::ResetKeyboard();
    g_AnmManager = new AnmManager();
    if (!g_Supervisor.cfg.windowed)
    {
        SDL_HideCursor();
    }
    renderRes = g_Supervisor.RegisterChain();
#ifdef TH_ENABLE_NETPLAY
    if (g_NetplayStage1Harness)
        std::printf("th07 netplay audit: supervisor chain result=%d\n", renderRes);
#endif
    if (renderRes != ZUN_SUCCESS)
    {
        return SDL_APP_FAILURE;
    }
    renderRes = RENDER_RESULT_KEEP_RUNNING;
    g_GameWindow.curFrame = -30;

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    if (g_OpenMusicRoomForVisualTest && !g_MusicRoomVisualTestDispatched &&
        g_MainMenuForDebug && g_MainMenuForDebug->calcChain)
    {
        g_Supervisor.curState = 8;
        g_MusicRoomVisualTestDispatched = true;
#ifdef TH_DEV_TOOLS
        SDL_Log("th07 music room audit: dispatch requested");
#endif
    }
#ifdef TH_ENABLE_NETPLAY
    if (g_DemoDeterminismHarness && !g_GameManager.demo &&
        g_MainMenuForDebug && g_MainMenuForDebug->calcChain)
    {
        // Enter the stock title-demo path on its next update.  Keeping the
        // menu as the owner preserves the retail replay validation and setup.
        // Keep it armed until the menu consumes it so an intro-skip key-up
        // cannot reset the idle counter on the same logical tick.
        g_GameManager.demoIdx = (g_DemoDeterminismIndex + 2) % 3;
        g_MainMenuForDebug->demoFramesCount = 901;
        if (!g_DemoDeterminismHarnessDispatched)
        {
            g_DemoDeterminismHarnessDispatched = true;
            SDL_Log("th07 demo determinism: armed built-in demo %d", g_DemoDeterminismIndex);
        }
    }
    if (g_NetplayStage1Harness && !g_NetplayStage1HarnessDispatched &&
        g_MainMenuForDebug && g_MainMenuForDebug->calcChain &&
        (!g_NetplayLanProduction ||
         Netplay::Th07LanStageProbe::TransportReady()))
    {
        // Netplay determinism harness only. Enter the same real gameplay
        // initialization path as the existing Stage visual audit, but keep
        // ordinary resources, lives and the canonical 1x 60 Hz simulation.
        i32 requestedDifficulty = 1;
#ifdef __EMSCRIPTEN__
        if (g_NetplayLanProduction)
        {
            requestedDifficulty = EM_ASM_INT({
                const difficulty = Number(Module.eaglerOptions?.netplayDifficulty ?? 1);
                return Number.isInteger(difficulty) && difficulty >= 0 && difficulty <= 5
                    ? difficulty : 1;
            });
        }
#endif
        g_Supervisor.cfg.defaultDifficulty = (u8)requestedDifficulty;
        g_GameManager.difficulty = requestedDifficulty;
#ifdef __EMSCRIPTEN__
        if (g_NetplayLanProduction)
        {
            EM_ASM({
                globalThis.__eaglerNetplayRequestedDifficulty = $0;
            }, requestedDifficulty);
        }
#endif
        g_GameManager.character = CHAR_REIMU;
        g_GameManager.shotType = 0;
        g_GameManager.practice = 0;
        g_GameManager.demo = 0;
        g_GameManager.SetIsReplay(0);
        if (g_NetplayLanProduction)
        {
#ifdef __EMSCRIPTEN__
            const int requestedSeed = EM_ASM_INT({
                const seed = Number(Module.eaglerOptions?.netplaySeed);
                return Number.isInteger(seed) && seed >= 0 && seed <= 65535 ? seed : 0x4a3d;
            });
            g_Rng.seed = (u16)requestedSeed;
            g_Rng.seedBackup = (u16)requestedSeed;
            g_Rng.generationCount = 0;
#endif
        }
        else if (g_NetplayDualStage1Harness || g_NetplayLanStage1Harness)
        {
            // Supervisor seeds from wall-clock during process startup. Online
            // play instead needs a session-owned seed. Reset all RNG ownership
            // immediately before GameManager consumes it for Stage 1 setup.
            g_Rng.seed = 0x4a3d;
            g_Rng.seedBackup = 0x4a3d;
            g_Rng.generationCount = 0;
        }
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        if (g_NetplayLanStage1Harness || g_NetplayLanProduction)
        {
            MultiplayerGameplay::SessionState gameplaySession;
            i32 requestedPlayerCount = 2;
#ifdef __EMSCRIPTEN__
            requestedPlayerCount = EM_ASM_INT({
                const count = Number(Module.eaglerOptions?.netplayPlayerCount ?? 2);
                return count === 3 ? 3 : 2;
            });
            const int localSlot = EM_ASM_INT({
                return Module.eaglerOptions?.netplayPlayer ?? 0;
            });
            gameplaySession.localPlayer =
                localSlot >= 0 && localSlot < requestedPlayerCount ? localSlot : 0;
#else
            gameplaySession.localPlayer = 0;
#endif
            gameplaySession.playerCount = (u8)requestedPlayerCount;
            gameplaySession.showStagePlayerNames = true;
            gameplaySession.stage4BossChain = EaglerOptions::NetplayStage4BossChain();
            for (i32 playerId = 0; playerId < requestedPlayerCount; ++playerId)
            {
                i32 character = playerId == 0 ? CHAR_REIMU :
                                playerId == 1 ? CHAR_MARISA : CHAR_SAKUYA;
                i32 shot = 0;
#ifdef __EMSCRIPTEN__
                if (g_NetplayLanProduction)
                {
                    character = EM_ASM_INT({
                        const entry = Module.eaglerOptions?.netplayLoadouts?.[$0];
                        const value = Number(entry?.character);
                        return Number.isInteger(value) && value >= 0 && value <= 2 ? value : $1;
                    }, playerId, character);
                    shot = EM_ASM_INT({
                        const entry = Module.eaglerOptions?.netplayLoadouts?.[$0];
                        const value = Number(entry?.shot);
                        return Number.isInteger(value) && value >= 0 && value <= 1 ? value : 0;
                    }, playerId);
                }
#endif
                gameplaySession.players[playerId].active = true;
                gameplaySession.players[playerId].character = (u8)character;
                gameplaySession.players[playerId].shot = (u8)shot;
            }
            // TH07's original GameManager globals remain P1's compatibility
            // owner. Match them to the negotiated P1 loadout before
            // GameManager::RegisterChain consumes the values.
            g_GameManager.character = gameplaySession.players[0].character;
            g_GameManager.shotType = gameplaySession.players[0].shot;
            if (!MultiplayerGameplay::Configure(gameplaySession))
            {
                SDL_Log("th07 netplay audit: multiplayer gameplay session rejected");
                return SDL_APP_FAILURE;
            }
        }
#endif
        // MainMenu::StartSelectedGame uses 0 for the normal route, 6 for
        // Extra and 7 for Phantasm; GameManager increments that value while
        // registering the first stage. Preserve that original TH07 mapping.
        g_GameManager.currentStage = requestedDifficulty < DIFF_EXTRA
            ? 0 : requestedDifficulty + DIFF_HARD;
#ifdef __EMSCRIPTEN__
        // Test fixture only: exercise a real late-stage ECL/STD workload.
        const int probeStage = EM_ASM_INT({
            const o = Module.eaglerOptions;
            return o?.debugHarness === 'netplay-lan-stage1' &&
                Number.isInteger(o.netplayTestStage) && o.netplayTestStage >= 1 &&
                o.netplayTestStage <= 6 ? o.netplayTestStage : 0;
        });
        if (probeStage) g_GameManager.currentStage = probeStage - 1;
#endif
        g_GameManager.finished = 0;
        MainMenu *menu = g_MainMenuForDebug;
        g_Chain.Cut(menu->calcChain);
        g_Supervisor.curState = 2;
        g_NetplayStage1HarnessDispatched = true;
        SDL_Log(g_NetplayLanProduction
                    ? "th07 netplay: dispatched real LAN Stage 1 at 1x"
                    : "th07 netplay audit: dispatched real Stage 1 at 1x");
    }
    if (g_NetplayStage1HarnessDispatched && !g_NetplayStage1HarnessPrepared &&
        g_Supervisor.curState == 2 && g_GameManager.notInMenu && g_GameManager.globals)
    {
        g_NetplayStage1HarnessPrepared = true;
#ifdef __EMSCRIPTEN__
        if (g_NetplayLanProduction)
        {
            EM_ASM({
                globalThis.__eaglerNetplayGameplayDifficulty = $0;
                globalThis.__eaglerNetplayGameplayStage = $1;
            }, g_GameManager.difficulty, g_GameManager.currentStage);
        }
#endif
        SDL_Log(g_NetplayLanProduction
                    ? "th07 netplay: Stage 1 gameplay ready at canonical 60 Hz"
                    : "th07 netplay audit: Stage 1 gameplay ready at canonical 60 Hz");
    }
#endif
#ifdef TH_DEV_TOOLS
    if (g_MenuStringVisualTestState >= 0 && !g_MenuStringVisualTestDispatched &&
        g_MainMenuForDebug && g_MainMenuForDebug->calcChain)
    {
        g_MainMenuForDebug->SetMenuState(static_cast<MenuState>(g_MenuStringVisualTestState));
        g_MenuStringVisualTestDispatched = true;
        SDL_Log("th07 strings_lookup menu audit: dispatch state=%d", g_MenuStringVisualTestState);
    }
    if (g_CharacterSelectVisualTestIndex >= 0 && !g_CharacterSelectVisualTestDispatched &&
        g_MainMenuForDebug && g_MainMenuForDebug->calcChain)
    {
        // Enter the real normal character-select state through MainMenu's own
        // transition helper. Only the requested starting character is seeded;
        // the production state machine still owns all VM activation/scripts.
        g_Supervisor.cfg.defaultDifficulty = 1;
        g_GameManager.difficulty = 1;
        g_GameManager.character = g_CharacterSelectVisualTestIndex;
        g_GameManager.practice = 0;
        g_MainMenuForDebug->SetMenuState(MENU_STATE_NORMAL_SELECT_CHARACTER);
        g_CharacterSelectVisualTestDispatched = true;
        SDL_Log("th07 character-select audit: dispatch character=%d",
                g_CharacterSelectVisualTestIndex);
    }
    if (g_OpenResultSpellsForVisualTest && !g_ResultSpellsVisualTestDispatched &&
        g_MainMenuForDebug && g_MainMenuForDebug->calcChain)
    {
        MainMenu *menu = g_MainMenuForDebug;
        g_Chain.Cut(menu->calcChain);
        g_Supervisor.curState = 9;
        g_Supervisor.wantedState = 9;
        if (ResultScreen::RegisterChain(3) != ZUN_SUCCESS)
            return SDL_APP_FAILURE;
        g_ResultSpellsVisualTestDispatched = true;
        SDL_Log("th07 dev: dispatched real Result spell-history visual test");
    }
    if ((g_OpenResultStatsForVisualTest || g_OpenResultStatsPhantasmForVisualTest) &&
        !g_ResultStatsVisualTestDispatched &&
        g_MainMenuForDebug && g_MainMenuForDebug->calcChain)
    {
        MainMenu *menu = g_MainMenuForDebug;
        g_Chain.Cut(menu->calcChain);
        g_Supervisor.curState = 9;
        g_Supervisor.wantedState = 9;
        if (ResultScreen::RegisterChain(g_OpenResultStatsPhantasmForVisualTest ? 5 : 4) != ZUN_SUCCESS)
            return SDL_APP_FAILURE;
        g_ResultStatsVisualTestDispatched = true;
        SDL_Log("th07 dev: dispatched real Result Stats visual test phantasm=%d",
                g_OpenResultStatsPhantasmForVisualTest ? 1 : 0);
    }
    if (g_OpenEndingForVisualTest && !g_EndingVisualTestDispatched &&
        g_MainMenuForDebug && g_MainMenuForDebug->calcChain && g_GameManager.globals)
    {
        // Developer viewer/audit only. Seed the production ending selector
        // fields; the Ending parser/ANM/background/text renderer remain real.
        const i32 selection = g_EndingViewerSelection >= 0 ? g_EndingViewerSelection : 0;
        const bool badEnding = selection >= 6;
        const i32 character = badEnding ? selection - 6 : selection / 2;
        const i32 shotType = badEnding ? 0 : selection % 2;
        const i32 shotTypeAndCharacter = character * 2 + shotType;
        g_GameManager.character = character;
        g_GameManager.shotType = shotType;
        g_GameManager.shotTypeAndCharacter = shotTypeAndCharacter;
        g_GameManager.difficulty = 1;
        g_GameManager.globals->numRetries = badEnding ? 1 : 0;
        g_GameManager.clrd[shotTypeAndCharacter].difficultyClearedWithRetries[1] = 99;
        g_GameManager.clrd[shotTypeAndCharacter].difficultyClearedWithoutRetries[1] = 99;
        MainMenu *menu = g_MainMenuForDebug;
        g_Chain.Cut(menu->calcChain);
        // Once the production Supervisor has registered an Ending from the
        // gameplay path, both state fields settle on 9 for the lifetime of
        // that Ending. Ending::DeletedCallback then changes only curState to
        // 6, producing the real wanted=9/cur=6 transition that registers the
        // post-ending ResultScreen. Keep that ownership pair here as well;
        // using the main-menu pair (1/1) leaves no consumer for curState=6
        // and strands the final ending frame under the FPS overlay.
        g_Supervisor.curState = 9;
        g_Supervisor.wantedState = 9;
        if (Ending::RegisterChain() != ZUN_SUCCESS)
            return SDL_APP_FAILURE;
        Ending::DebugSetFastForward(g_EndingViewerSelection < 0);
        g_EndingVisualTestDispatched = true;
        SDL_Log("th07 dev: dispatched real Ending viewer selection=%d", selection);
    }
    if (g_StageVisualTestIndex >= 0 && !g_Stage1VisualTestDispatched &&
        g_MainMenuForDebug && g_MainMenuForDebug->calcChain)
    {
        // This keeps the production menu untouched while providing a repeatable
        // desktop-only path through the real GameManager/Stage initialization.
        g_Supervisor.cfg.defaultDifficulty = 1;
        g_GameManager.difficulty = 1;
        g_GameManager.character = CHAR_REIMU;
        g_GameManager.shotType = 0;
        g_GameManager.practice = 0;
        g_GameManager.demo = 0;
        g_GameManager.SetIsReplay(0);
        g_GameManager.currentStage = g_StageVisualTestIndex;
        g_GameManager.finished = 0;
        MainMenu *menu = g_MainMenuForDebug;
        g_Chain.Cut(menu->calcChain);
        g_Supervisor.curState = 2;
        g_Stage1VisualTestDispatched = true;
        SDL_Log("th07 dev: dispatched real Stage %d visual test", g_StageVisualTestIndex + 1);
    }
    if (g_Stage1VisualTestDispatched && !g_Stage1VisualTestPrepared &&
        g_Supervisor.curState == 2 && g_GameManager.notInMenu && g_GameManager.globals)
    {
        // Keep the real ECL timeline and collision code, but make a long visual
        // inspection run practical without changing ordinary gameplay.
        g_GameManager.SetLivesRemaining(99);
        g_DevSpeedMultiplier = 8.0f;
        g_Stage1VisualTestPrepared = true;
#ifdef __EMSCRIPTEN__
        EM_ASM({
            globalThis.__eaglerOrdinaryStageVisualPrepared = true;
            globalThis.__eaglerOrdinaryStageVisualStage = $0;
        }, g_StageVisualTestIndex + 1);
#endif
        SDL_Log("th07 dev: Stage %d visual test prepared (99 lives, 8x logic)",
                g_StageVisualTestIndex + 1);
    }
#endif
#ifdef TH_ENABLE_THPRAC
    if (g_OpenThpracMenuForVisualTest && !g_ThpracMenuVisualTestDispatched &&
        g_MainMenuForDebug && g_MainMenuForDebug->calcChain)
    {
        g_Supervisor.cfg.defaultDifficulty = 1;
        g_GameManager.difficulty = 1;
        g_GameManager.character = 0;
        g_GameManager.shotType = 0;
        g_GameManager.practice = 1;
        g_MainMenuForDebug->SetMenuState(MENU_STATE_SELECT_PRACTICE_STAGE);
        g_MainMenuForDebug->stateTimer = 0;
        g_ThpracMenuVisualTestDispatched = true;
    }
    if (g_AutoStartThpracVisualTest && g_ThpracMenuVisualTestDispatched &&
        ++g_ThpracVisualTestFrames == 30)
        PracticeRuntime::DebugAcceptPracticeMenu();
    if (g_AutoPauseThpracVisualTest && !g_ThpracPauseVisualTestDispatched &&
        PracticeRuntime::Active() && g_Supervisor.curState == 2 && g_ThpracVisualTestFrames > 180)
    {
        g_GameManager.isInPauseMenu = 1;
        g_ThpracPauseVisualTestDispatched = true;
    }
#endif
    if (g_ToggleFullscreenRequested)
    {
        // Do not change native window styles from inside SDL_AppEvent. SDL can
        // otherwise apply a delayed geometry restore after the callback.
        g_ToggleFullscreenRequested = false;
        GameWindow::ToggleFullscreen();
    }
    renderRes = g_GameWindow.Render();
    if (renderRes != RENDER_RESULT_KEEP_RUNNING)
    {
        return SDL_APP_SUCCESS;
    }
    g_Supervisor.flags &= ~16;

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
#ifdef TH_ENABLE_THPRAC
    ThpracImGui::ProcessEvent(*event);
#endif
    switch (event->type)
    {
    case SDL_EVENT_KEY_DOWN:
#ifdef TH_DEV_TOOLS
        if (event->key.scancode == SDL_SCANCODE_RETURN ||
            event->key.scancode == SDL_SCANCODE_KP_ENTER)
        {
            SDL_Log("th07 window input: keydown scancode=%d mod=0x%x repeat=%d",
                    static_cast<int>(event->key.scancode), static_cast<unsigned>(event->key.mod),
                    event->key.repeat ? 1 : 0);
        }
#endif
#if !defined(__ANDROID__) && !(defined(__APPLE__) && TARGET_OS_IPHONE) && !defined(__EMSCRIPTEN__)
        if (event->key.repeat == 0 &&
            (event->key.scancode == SDL_SCANCODE_RETURN ||
             event->key.scancode == SDL_SCANCODE_KP_ENTER) &&
            (event->key.mod & (SDL_KMOD_LALT | SDL_KMOD_RALT)) != 0)
        {
            g_ToggleFullscreenRequested = true;
            // TH07 polls SDL keyboard state directly, so consuming the event
            // alone would still expose Enter to the game on this frame.
            Controller::SetEnterSuppressed(true);
#ifdef TH_DEV_TOOLS
            SDL_Log("th07 window input: Alt+Enter toggle requested");
#endif
        }
#ifdef TH_DEV_TOOLS
        else
#endif
#endif
#if defined(TH_DEV_TOOLS) && !defined(TH_ENABLE_THPRAC)
        if (event->key.repeat == 0 && event->key.scancode == SDL_SCANCODE_F5)
        {
            g_DevSpeedMultiplier = g_DevSpeedMultiplier == 1.0f ? 4.0f :
                                   g_DevSpeedMultiplier == 4.0f ? 8.0f : 1.0f;
            SDL_Log("th07 dev: logic speed = %gx", g_DevSpeedMultiplier);
        }
#endif
        break;
    case SDL_EVENT_KEY_UP:
        if (event->key.scancode == SDL_SCANCODE_RETURN ||
            event->key.scancode == SDL_SCANCODE_KP_ENTER)
        {
            Controller::SetEnterSuppressed(false);
        }
        break;
    case SDL_EVENT_WINDOW_FOCUS_GAINED:
        ResumeAudioForActiveWindow();
        g_GameWindow.isAppActive = 1;
        GameWindow::RememberWindowedState();
        if (GameWindow::IsFullscreen())
        {
            SDL_HideCursor();
        }
        break;
    case SDL_EVENT_WINDOW_FOCUS_LOST:
#ifdef __EMSCRIPTEN__
        // Canvas/iframe focus on Web, especially iOS WebKit, is not an app
        // lifecycle signal. A touch ending can transiently drop DOM focus while
        // the page remains visible; treating that as background freezes Render()
        // until another finger is held down. Real backgrounding is handled by
        // the SDL background events and the shell visibility/pagehide owner.
        Touch::CancelTouches();
        break;
#endif
    case SDL_EVENT_WILL_ENTER_BACKGROUND:
    case SDL_EVENT_DID_ENTER_BACKGROUND:
        Touch::CancelTouches();
        SuspendAudioForInactiveWindow();
        g_GameWindow.isAppActive = 0;
        SDL_ShowCursor();
        break;
    case SDL_EVENT_WILL_ENTER_FOREGROUND:
    case SDL_EVENT_DID_ENTER_FOREGROUND:
        ResumeAudioForActiveWindow();
        g_GameWindow.isAppActive = 1;
        break;
    case SDL_EVENT_GAMEPAD_ADDED:
        if (!g_Supervisor.controller)
        {
            g_Supervisor.controller = SDL_OpenGamepad(event->gdevice.which);
        }
        break;
    case SDL_EVENT_GAMEPAD_REMOVED:
        if (g_Supervisor.controller)
        {
            SDL_Joystick *joy = SDL_GetGamepadJoystick(g_Supervisor.controller);

            if (SDL_GetJoystickID(joy) == event->gdevice.which)
            {
                SDL_CloseGamepad(g_Supervisor.controller);
                g_Supervisor.controller = nullptr;
            }
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
    case SDL_EVENT_QUIT:
        return SDL_APP_SUCCESS;
    }

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    if (g_GameManager.plst.base.magic != 0)
    {
        ResultScreen::RegisterChain(2);
    }
    g_Chain.Release();
    while (g_SoundPlayer.ProcessQueues())
        ;
    g_SoundPlayer.Release();

    SAFE_DELETE(g_AnmManager);
    SAFE_DELETE(g_Supervisor.gfxDevice);
    if (g_GameWindow.window)
    {
        SDL_DestroyWindow(g_GameWindow.window);
        g_GameWindow.window = NULL;
    }
    SDL_ShowCursor();
    if (renderRes == RENDER_RESULT_EXIT_ERROR)
    {
        g_GameErrorContext.m_BufferEnd = g_GameErrorContext.m_Buffer;
        *g_GameErrorContext.m_BufferEnd = '\0';
        g_GameErrorContext.Log("再起動を要するオプションが変更されたので再起動します\n");
        // we normally should restart the application but thats not really possible in this
        // model
        // however, this is really only ever used when the application is restarting after enabling
        // vsync. afaik, it should be pretty safe to just not have the application restart after
        // enabling vsync since theres nothing before the checkvsync call that needs vsyncDisabled
        // to be there beforehand
    }
#ifdef TH_DEV_TOOLS
    if (!g_TouchStateSelfTest && !g_ReplayExtensionSelfTest)
#endif
    {
        FileSystem::WriteDataToFile("th07.cfg", &g_Supervisor.cfg, sizeof(GameConfiguration));
    }
#ifdef TH_DEV_TOOLS
    if (!g_TouchStateSelfTest && !g_ReplayExtensionSelfTest)
#endif
    {
        g_GameErrorContext.Flush();
    }
#ifdef __EMSCRIPTEN__
    EM_ASM({ globalThis.EaglerTouhouGameExited?.($0); }, static_cast<int>(result));
#endif
}
