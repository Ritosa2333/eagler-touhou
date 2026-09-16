#include "PracticeRuntime.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#ifdef TH_DEV_TOOLS
#include <SDL3/SDL_log.h>
#endif

#include "EnemyManager.hpp"
#include "AsciiManager.hpp"
#include "Controller.hpp"
#include "FileSystem.hpp"
#include "GameManager.hpp"
#include "GameWindow.hpp"
#include "Gui.hpp"
#include "ReplayExtension.hpp"
#include "ReplayManager.hpp"
#include "ScreenEffect.hpp"
#include "Touch.hpp"
#include "SoundPlayer.hpp"
#include "utils.hpp"
#ifdef TH_ENABLE_THPRAC
#include <SDL3/SDL.h>
#include "ThpracImGui.hpp"
#endif
#if defined(THPRAC_PORTABLE_ENABLED)
#include "section_catalog.hpp"
namespace THPrac::Gui
{
void ShowLicenceInfo();
}

static void ResetTouchReplayForPracticeRestart()
{
    // A thprac Restart is a new attempt, not a continuation of the previous
    // gesture stream. End any in-flight touch lifetime and clear the extension
    // recorder/finger-id map before GAMEMANAGER_REINIT reuses ReplayManager.
    // Playback fingers are separate from live touch owners; clear them too or
    // a DOWN without a matching UP at the restart boundary leaves a visible
    // replay cross in the next attempt.
    Touch::CancelTouches();
    Touch::ResetReplayRecordingState();
    Touch::ResetReplayTouch();
    ReplayExtension::ResetRecording();
}
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

namespace PracticeRuntime
{
static Config g_Config;
// Upstream THGuiPrac owns persistent mMode/mStage/... widget state separately
// from the live thPracParam used by the current run/replay.  Keep that split:
// replay metadata and current-run parameters must never overwrite what the
// next Practice menu remembers.
static Config g_MenuConfig = [] {
    Config config;
    config.mode = 1;
    // THGuiPrac constructor defaults. Keep these in the persistent widget
    // owner; Config{} itself is reserved for live THPracParam::Reset().
    config.life = 8;
    config.bomb = 8;
    config.power = 128;
    config.rank = 32;
    return config;
}();
static bool g_MenuOpen = false;
static i32 g_MenuCursor = 0;
static i32 g_MenuDifficulty = 0;
static i32 g_MenuShotType = 0;
static i32 g_MenuSectionIndex = 0;
static i32 g_MenuChapter = 1;
static bool g_ImGuiMenuFocusPending = false;
static const char *g_ImGuiMenuFocusLabel = "Stage";
enum class MenuVisualState
{
    Closed,
    Opening,
    Open,
    Closing,
};
static MenuVisualState g_MenuVisualState = MenuVisualState::Closed;
static float g_MenuAlpha = 0.0f;
static float g_MenuCloseStep = 0.1f;
static MenuResult g_MenuPendingResult = MenuResult::Waiting;
static constexpr i32 kMenuItemCount = 16;
static bool g_PauseWasOpen = false;
static bool g_PauseSettings = false;
static i32 g_PauseCursor = 0;
#ifdef TH_ENABLE_THPRAC
static MenuVisualState g_PauseVisualState = MenuVisualState::Closed;
static float g_PauseAlpha = 0.0f;
enum class PauseAction
{
    None,
    Resume,
    Exit,
    Restart,
};
static PauseAction g_PauseAction = PauseAction::None;
// THPauseMenu::mState has a persistent STATE_OPEN distinct from the window's
// fade/status.  In particular StateClose()->StateOpen() resets the logical
// counter one tick before StateOpen(counter==1) actually calls Open().
static bool g_PauseLogicalOpen = false;
static unsigned int g_PauseFrameCounter = 0;
static bool g_ImGuiPauseFocusPending = false;
#endif
static bool g_PreserveConfigOnRestart = false;
// State(3) writes thPracParam after the vanilla new-run reset hook.  Because
// our menu commits slightly earlier, preserve that freshly accepted runtime
// config through exactly the next GameManager::AddedCallback.
static bool g_PreserveConfigOnFreshStart = false;
// Upstream th06_result_screen_create is a disabled-by-default ST hook.  The
// advanced-practice Pause->Exit path enables it for exactly the next result
// screen, where the hook disables itself again.  Keep that one-shot boundary
// instead of deriving it from g_Config.active: replay playback also restores
// the same practice metadata but must not itself offer to save another replay.
static bool g_ResultReplaySaveRequested = false;
// Exact TH06 THGuiRep ownership.  State(1) resets the live thPracParam and
// these candidate flags; State(2) edits only mRepParam/mParamStatus; State(3)
// marks replay ownership active and conditionally copies the candidate into
// the live parameter object.
static bool g_ReplayPlaybackActive = false;
// Portable-only bridge for the immediately following GameManager startup.
// Do not confuse this with upstream THGuiRep::mRepStatus above: mRepStatus is
// sticky until Replay State(1), while this flag is consumed once startup has
// inherited the State(3) live parameter block.
static bool g_ReplayStartupCommitted = false;
static bool g_ReplayParamStatus = false;
static Config g_ReplayCandidate;
// THOverlay is a persistent trainer surface, separate from THGuiPrac's run
// parameters.  Upstream keeps these hotkey patches enabled across runs until
// the user toggles them again, so do not store them in Config/replay metadata.
#ifdef TH_ENABLE_THPRAC
struct OverlayState
{
    bool menuOpen = false;
    bool invincible = false;
    bool infiniteLives = false;
    bool infiniteBombs = false;
    bool infinitePower = false;
    bool timeLock = false;
    bool autoBomb = false;
    bool everlastingBgm = false;
    bool trackerOpen = false;
};
static OverlayState g_Overlay;
// THOverlay::OnPreUpdate evaluates Backspace after THPauseMenu/THGuiPrac/
// THGuiRep have created their current-frame ImGui items and rejects the toggle
// while any item is active. Capture only the 60 Hz rising edge at th06_update;
// consume it later in DrawOverlay once those earlier owners exist.
static bool g_ModMenuToggleRequested = false;
static bool g_AdvancedMenuToggleRequested = false;
static bool g_ScreenshotRequested = false;
struct AdvancedOptionsState
{
    bool menuOpen = false;
    bool showLicense = false;
};
static AdvancedOptionsState g_AdvancedOptions;
// Backspace, F1..F7, Tab, F12. SDL is sampled only at the fixed 60 Hz trainer
// update boundary, matching upstream GuiHotKey rising-edge semantics.
static bool g_OverlayKeyDown[14] = {};
static i32 g_TrackerMisses = 0;
#endif
static bool g_PreserveBgmRestart = false;
static bool g_BgmTrackingStarted = false;
static bool g_BgmChangedSinceStart = false;
static std::string g_CurrentBgmPath;
// th06_sfx_fix is a disabled-by-default one-shot hook. th06_prac_menu_enter
// enables it for the first SpawnBulletPattern call of a newly entered Practice
// run; Replay entry does not enable it.
static bool g_BossSectionSfxFixPending = false;
static MenuResult g_MenuResult = MenuResult::Waiting;
#if defined(THPRAC_PORTABLE_ENABLED)
extern "C" bool ThpracPortableTh06SetSessionJson(const char *json);
#endif

template <typename T> static T Clamp(T value, T minimum, T maximum)
{
    return std::max(minimum, std::min(maximum, value));
}

bool AdvancedActive()
{
    return g_Config.active && g_Config.mode != 0;
}

bool OverlayInvincible()
{
#ifdef TH_ENABLE_THPRAC
    return !g_GameManager.isInReplay && g_Overlay.invincible;
#else
    return false;
#endif
}

bool OverlayInfiniteLives()
{
#ifdef TH_ENABLE_THPRAC
    return !g_GameManager.isInReplay && g_Overlay.infiniteLives;
#else
    return false;
#endif
}

bool OverlayInfiniteBombs()
{
#ifdef TH_ENABLE_THPRAC
    return !g_GameManager.isInReplay && g_Overlay.infiniteBombs;
#else
    return false;
#endif
}

bool OverlayInfinitePower()
{
#ifdef TH_ENABLE_THPRAC
    return !g_GameManager.isInReplay && g_Overlay.infinitePower;
#else
    return false;
#endif
}

bool OverlayTimeLock()
{
#ifdef TH_ENABLE_THPRAC
    return !g_GameManager.isInReplay && g_Overlay.timeLock;
#else
    return false;
#endif
}

bool OverlayAutoBomb()
{
#ifdef TH_ENABLE_THPRAC
    return !g_GameManager.isInReplay && g_Overlay.autoBomb;
#else
    return false;
#endif
}

bool OverlayEverlastingBgm()
{
#ifdef TH_ENABLE_THPRAC
    return g_Overlay.everlastingBgm;
#else
    return false;
#endif
}

void ResetTracker()
{
#ifdef TH_ENABLE_THPRAC
    g_TrackerMisses = 0;
#endif
}

void RecordTrackerMiss()
{
#ifdef TH_ENABLE_THPRAC
    if (g_TrackerMisses < 0x7fffffff)
        ++g_TrackerMisses;
#endif
}

#ifdef TH_ENABLE_THPRAC
static bool EaglerOverlayKeyDown(SDL_Scancode scancode)
{
#ifdef __EMSCRIPTEN__
    i32 bit = -1;
    switch (scancode)
    {
    case SDL_SCANCODE_BACKSPACE: bit = 0; break;
    case SDL_SCANCODE_F1: bit = 1; break;
    case SDL_SCANCODE_F2: bit = 2; break;
    case SDL_SCANCODE_F3: bit = 3; break;
    case SDL_SCANCODE_F4: bit = 4; break;
    case SDL_SCANCODE_F5: bit = 5; break;
    case SDL_SCANCODE_F6: bit = 6; break;
    case SDL_SCANCODE_F7: bit = 7; break;
    case SDL_SCANCODE_TAB: bit = 8; break;
    default: return false;
    }
    return EM_ASM_INT({
        return !!(((Module.eaglerControls && Module.eaglerControls.thpracKeyboardBits) | 0) & (1 << $0));
    }, bit) != 0;
#else
    (void)scancode;
    return false;
#endif
}

static void PublishEaglerOverlayMenuState(bool open)
{
#ifdef __EMSCRIPTEN__
    static i32 last = -1;
    const i32 value = open ? 1 : 0;
    if (last == value)
        return;
    last = value;
    EM_ASM({
        Module.eaglerThpracMenuOpen = !!$0;
        window.dispatchEvent(new CustomEvent("eagler-thprac-menu", { detail: { open: !!$0 } }));
    }, value);
#else
    (void)open;
#endif
}

static bool OverlayKeyPressed(i32 slot, SDL_Scancode scancode)
{
    int count = 0;
    const bool *keyboard = SDL_GetKeyboardState(&count);
    const bool down = (keyboard != nullptr && static_cast<int>(scancode) < count && keyboard[scancode]) ||
                      EaglerOverlayKeyDown(scancode);
    const bool pressed = down && !g_OverlayKeyDown[slot];
    g_OverlayKeyDown[slot] = down;
    return pressed;
}
#endif

void UpdateOverlay()
{
#ifdef TH_ENABLE_THPRAC
    // Upstream th06_update runs at the RunCalcChain return boundary and calls
    // THPauseMenu::Update() every trainer tick, even while its window is
    // closed. OnPreUpdate owns the persistent mFrameCounter; fade progression
    // follows in the same post-calc update. Do not reset this counter merely
    // because the player is not currently paused.
    if (g_PauseFrameCounter < 0xffffffffu)
        ++g_PauseFrameCounter;
    if (g_PauseVisualState == MenuVisualState::Opening)
    {
        g_PauseAlpha = std::min(0.8f, g_PauseAlpha + 0.1f);
        if (g_PauseAlpha >= 0.8f)
            g_PauseVisualState = MenuVisualState::Open;
    }
    else if (g_PauseVisualState == MenuVisualState::Closing)
    {
        g_PauseAlpha = std::max(0.0f, g_PauseAlpha - 0.1f);
        if (g_PauseAlpha <= 0.0f)
            g_PauseVisualState = MenuVisualState::Closed;
    }

    if (OverlayKeyPressed(0, SDL_SCANCODE_BACKSPACE))
        g_ModMenuToggleRequested = true;
    // Upstream hotkeys.tracker defaults to Tab.
    if (OverlayKeyPressed(8, SDL_SCANCODE_TAB))
        g_Overlay.trackerOpen = !g_Overlay.trackerOpen;
    // Upstream hotkeys.advanced_menu defaults to F12 and owns THAdvOptWnd.
    if (OverlayKeyPressed(9, SDL_SCANCODE_F12))
        g_AdvancedMenuToggleRequested = true;
    // hotkeys.screenshot defaults to Home and is consumed by th06_render after
    // GameGuiRender. Capture the edge here at the fixed post-calc producer and
    // defer the readback to the completed render frame.
    if (OverlayKeyPressed(13, SDL_SCANCODE_HOME))
        g_ScreenshotRequested = true;
#endif
}

void DrawOverlay()
{
#ifdef TH_ENABLE_THPRAC
    if (!ThpracImGui::IsFrameOpen())
        return;

    // GameGuiEnd(draw_cursor): the Win32 backend only asks ImGui to draw its
    // software cursor in real fullscreen, where the OS cursor is hidden. TH06
    // requests it for Advanced Options, THGuiPrac and THPauseMenu.
    const bool fullscreen = g_GameWindow.window != nullptr &&
        (SDL_GetWindowFlags(g_GameWindow.window) & SDL_WINDOW_FULLSCREEN) != 0;
    const bool pauseCursor = g_PauseVisualState != MenuVisualState::Closed;
    ImGui::GetIO().MouseDrawCursor = fullscreen &&
        (g_AdvancedOptions.menuOpen || g_MenuOpen || pauseCursor);

    if (g_ModMenuToggleRequested)
    {
        if (!ImGui::IsAnyItemActive())
            g_Overlay.menuOpen = !g_Overlay.menuOpen;
        g_ModMenuToggleRequested = false;
    }
    PublishEaglerOverlayMenuState(g_Overlay.menuOpen);

    if (g_Overlay.menuOpen)
    {
        // GuiHotKey::operator() for F1..F7 exists only in
        // THOverlay::OnContentUpdate(). Do not update their key history while
        // the Mod Menu is closed: holding F1 while closed and then opening the
        // menu must be seen as the first press, exactly like upstream.
        if (OverlayKeyPressed(1, SDL_SCANCODE_F1)) g_Overlay.invincible = !g_Overlay.invincible;
        if (OverlayKeyPressed(2, SDL_SCANCODE_F2)) g_Overlay.infiniteLives = !g_Overlay.infiniteLives;
        if (OverlayKeyPressed(3, SDL_SCANCODE_F3)) g_Overlay.infiniteBombs = !g_Overlay.infiniteBombs;
        if (OverlayKeyPressed(4, SDL_SCANCODE_F4)) g_Overlay.infinitePower = !g_Overlay.infinitePower;
        if (OverlayKeyPressed(5, SDL_SCANCODE_F5)) g_Overlay.timeLock = !g_Overlay.timeLock;
        if (OverlayKeyPressed(6, SDL_SCANCODE_F6)) g_Overlay.autoBomb = !g_Overlay.autoBomb;
        if (OverlayKeyPressed(7, SDL_SCANCODE_F7)) g_Overlay.everlastingBgm = !g_Overlay.everlastingBgm;

        ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
        ImGui::SetNextWindowBgAlpha(0.5f);
        constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar |
                                           ImGuiWindowFlags_NoResize |
                                           ImGuiWindowFlags_NoMove |
                                           ImGuiWindowFlags_AlwaysAutoResize |
                                           ImGuiWindowFlags_NoSavedSettings |
                                           ImGuiWindowFlags_NoFocusOnAppearing |
                                           ImGuiWindowFlags_NoNav;
        if (ImGui::Begin("Mod Menu###thprac-overlay", nullptr, flags))
        {
            ImGui::Checkbox(ThpracImGui::Text(ThpracImGui::TextId::Invincible), &g_Overlay.invincible);
            ImGui::Checkbox(ThpracImGui::Text(ThpracImGui::TextId::InfLives), &g_Overlay.infiniteLives);
            ImGui::Checkbox(ThpracImGui::Text(ThpracImGui::TextId::InfBombs), &g_Overlay.infiniteBombs);
            ImGui::Checkbox(ThpracImGui::Text(ThpracImGui::TextId::InfPower), &g_Overlay.infinitePower);
            ImGui::Checkbox(ThpracImGui::Text(ThpracImGui::TextId::TimeLock), &g_Overlay.timeLock);
            ImGui::Checkbox(ThpracImGui::Text(ThpracImGui::TextId::AutoBomb), &g_Overlay.autoBomb);
            ImGui::Checkbox(ThpracImGui::Text(ThpracImGui::TextId::EverlastingBgm), &g_Overlay.everlastingBgm);
        }
        ImGui::End();
    }

    // THAdvOptWnd::StaticUpdate() runs after THOverlay::Update() in th06_update.
    if (g_AdvancedMenuToggleRequested)
    {
        g_AdvancedOptions.menuOpen = !g_AdvancedOptions.menuOpen;
        g_AdvancedMenuToggleRequested = false;
    }

    if (g_Overlay.trackerOpen && (g_GameManager.isInMenu || g_GameManager.isInGameMenu || g_GameManager.isInRetryMenu))
    {
        static const char *difficulty[3][5] = {
            {"Easy", "Normal", "Hard", "Lunatic", "Extra"},
            {"Easy", "Normal", "Hard", "Lunatic", "Extra"},
            {"イージー", "ノーマル", "ハード", "ルナティック", "エキストラ"},
        };
        static const char *shots[3][4] = {
            {"灵梦A", "灵梦B", "魔理沙A", "魔理沙B"},
            {"ReimuA", "ReimuB", "MarisaA", "MarisaB"},
            {"霊夢A", "霊夢B", "魔理沙A", "魔理沙B"},
        };
        const i32 locale = static_cast<i32>(ThpracImGui::GetLocale());
        const i32 diff = Clamp(static_cast<i32>(g_GameManager.difficulty), 0, 4);
        const i32 shot = Clamp(g_GameManager.CharacterShotType(), 0, 3);

        ImGui::SetNextWindowSize(ImVec2(180.0f, 0.0f), ImGuiCond_Always);
        ImGui::SetNextWindowPos(ImVec2(433.0f, 245.0f), ImGuiCond_Always);
        constexpr ImGuiWindowFlags trackerFlags = ImGuiWindowFlags_NoScrollbar |
                                                  ImGuiWindowFlags_NoScrollWithMouse |
                                                  ImGuiWindowFlags_NoTitleBar |
                                                  ImGuiWindowFlags_NoResize |
                                                  ImGuiWindowFlags_NoMove |
                                                  ImGuiWindowFlags_NoSavedSettings |
                                                  ImGuiWindowFlags_NoInputs |
                                                  ImGuiWindowFlags_NoFocusOnAppearing |
                                                  ImGuiWindowFlags_NoNav;
        if (ImGui::Begin("Tracker###thprac-tracker", nullptr, trackerFlags))
        {
            char title[48] = {};
            std::snprintf(title, sizeof(title), "%s (%s)", difficulty[locale][diff], shots[locale][shot]);
            const ImVec2 textSize = ImGui::CalcTextSize(title);
            ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.5f - textSize.x * 0.5f);
            ImGui::TextUnformatted(title);
            if (ImGui::BeginTable("Tracker table", 2))
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(ThpracImGui::Text(ThpracImGui::TextId::TrackerMiss));
                ImGui::TableNextColumn();
                ImGui::Text("%d", g_TrackerMisses);
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(ThpracImGui::Text(ThpracImGui::TextId::TrackerBomb));
                ImGui::TableNextColumn();
                ImGui::Text("%d", g_GameManager.bombsUsed);
                ImGui::EndTable();
            }
        }
        ImGui::End();
    }

    // TH06 THAdvOptWnd has only Game Speed + About. GameplayInit/GameplaySet
    // are empty in v2.3.0.3; do not import TH07's gameplay options here.
    if (g_AdvancedOptions.menuOpen)
    {
        const i32 locale = static_cast<i32>(ThpracImGui::GetLocale());
        static const char *advanced[3] = {"高级选项", "Advanced Options", "詳細設定"};
        static const char *gameSpeed[3] = {"游戏速度", "Game Speed", "ゲーム速度"};
        static const char *about[3] = {"关于 thprac", "About thprac", "thprac について"};
        static const char *fpsUnavailable[3] = {
            "当前 portable 架构没有加载 openinputlagpatch/vpatch；与原版 thprac 的 fps_status=0 相同，游戏速度选项不可用。",
            "No openinputlagpatch/vpatch backend is loaded; matching upstream fps_status=0, Game Speed is unavailable.",
            "openinputlagpatch/vpatch が読み込まれていないため、原版の fps_status=0 と同様にゲーム速度は使用できません。",
        };
        static const char *showLicense[3] = {"显示许可证", "Show license", "ライセンスを表示"};
        static const char *hideLicense[3] = {"隐藏许可证", "Hide license", "ライセンスを隠す"};

        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(640.0f, 480.0f), ImGuiCond_Always);
        ImGui::SetNextWindowBgAlpha(0.8f);
        constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;
        if (ImGui::Begin("Advanced Options###th06-thprac-advanced", nullptr, flags))
        {
            ImGui::TextUnformatted(advanced[locale]);
            ImGui::Separator();

            if (ImGui::CollapsingHeader(gameSpeed[locale], ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Indent();
                ImGui::BeginDisabled();
                i32 fixedFps = 60;
                ImGui::SliderInt("FPS", &fixedFps, 60, 6000);
                ImGui::EndDisabled();
                ImGui::TextWrapped("%s", fpsUnavailable[locale]);
                ImGui::Unindent();
            }

            if (ImGui::CollapsingHeader(about[locale], ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Indent();
                ImGui::TextUnformatted("thprac v2.3.0.3");
                ImGui::TextUnformatted("github.com/touhouworldcup/thprac");
                ImGui::TextUnformatted("Thanks: You!");
                if (ImGui::Button(g_AdvancedOptions.showLicense ? hideLicense[locale] : showLicense[locale]))
                    g_AdvancedOptions.showLicense = !g_AdvancedOptions.showLicense;
                if (g_AdvancedOptions.showLicense)
                {
                    ImGui::BeginChild("THPRAC license", ImVec2(0.0f, 100.0f), true);
                    THPrac::Gui::ShowLicenceInfo();
                    ImGui::EndChild();
                }
                ImGui::Unindent();
            }
            ImGui::SetWindowFocus();
        }
        ImGui::End();
    }

    // GameGuiEnd locale hotkey: while the configured language chord (default
    // Alt) is held and no ImGui item owns input, 1/2/3 select Japanese / Chinese
    // / English. Keep digit key history dormant outside this branch, matching
    // Gui::KeyboardInputUpdate() being called only after the chord/owner gates.
    if (!ImGui::IsAnyItemActive())
    {
        int keyCount = 0;
        const bool *keys = SDL_GetKeyboardState(&keyCount);
        const bool alt = keys &&
            ((static_cast<int>(SDL_SCANCODE_LALT) < keyCount && keys[SDL_SCANCODE_LALT]) ||
             (static_cast<int>(SDL_SCANCODE_RALT) < keyCount && keys[SDL_SCANCODE_RALT]));
        if (alt)
        {
            if (OverlayKeyPressed(10, SDL_SCANCODE_1))
                ThpracImGui::RequestLocale(ThpracImGui::Locale::JaJP);
            else if (OverlayKeyPressed(11, SDL_SCANCODE_2))
                ThpracImGui::RequestLocale(ThpracImGui::Locale::ZhCN);
            else if (OverlayKeyPressed(12, SDL_SCANCODE_3))
                ThpracImGui::RequestLocale(ThpracImGui::Locale::EnUS);
        }
    }
#endif
}

bool ConsumeScreenshotRequest()
{
#ifdef TH_ENABLE_THPRAC
    const bool requested = g_ScreenshotRequested;
    g_ScreenshotRequested = false;
    return requested;
#else
    return false;
#endif
}

bool AdvancedOptionsOpen()
{
#ifdef TH_ENABLE_THPRAC
    return g_AdvancedOptions.menuOpen;
#else
    return false;
#endif
}

void ResetBgmTracking()
{
    if (g_PreserveBgmRestart)
        return;
    g_BgmTrackingStarted = false;
    g_BgmChangedSinceStart = false;
    g_CurrentBgmPath.clear();
}

void NotifyBgmPlay(const char *path)
{
    if (path == nullptr || *path == '\0')
        return;
    if (!g_BgmTrackingStarted)
    {
        g_CurrentBgmPath = path;
        g_BgmTrackingStarted = true;
        g_BgmChangedSinceStart = false;
        return;
    }
    if (g_CurrentBgmPath != path)
    {
        g_CurrentBgmPath = path;
        g_BgmChangedSinceStart = true;
    }
}

bool PreserveBgmOnRestart()
{
    return g_PreserveBgmRestart;
}

void FinishBgmRestartPreservation()
{
    g_PreserveBgmRestart = false;
    g_BgmChangedSinceStart = false;
}

i32 EffectivePlayerShot(i32 vanillaShot)
{
    // Upstream th06_fake_shot_type writes PLAYER_SHOT (0x487e44), the value
    // read by ECL_VAR_PLAYER_SHOT. fakeType is encoded as shot+1 so zero can
    // mean "use the real shot".  This hook checks fakeType itself, not
    // thPracParam.mode; preserve that odd hidden-widget state exactly.
    if (Active() && g_Config.fakeType != 0)
        return Clamp(g_Config.fakeType - 1, 0, 3);
    return vanillaShot;
}

bool ForceFlandreFinalRage()
{
    // th06_hamon_rage skips the 7200-frame test at 0x40e1c7 and jumps to
    // 0x40e1d8 (remainingLife=0) for QED phase 1.
    return AdvancedActive() && g_Config.stage == 6 && g_Config.section == 70 && g_Config.phase == 1;
}

static i32 InitialBgmIndexForConfig(const Config &config)
{
    if (!config.active || config.mode == 0 || config.section >= 10000)
        return 0;
#if defined(THPRAC_PORTABLE_ENABLED)
    for (const auto &entry : thprac::portable::generated::th06SectionLabels)
    {
        if (entry.patchId == config.section)
            return entry.bgm ? 1 : 0;
    }
#endif
    return 0;
}

i32 InitialBgmIndex()
{
    return InitialBgmIndexForConfig(g_Config);
}

void ApplyPendingBossSectionSfxFix()
{
    // Match the upstream ST hook exactly: disable on the first call regardless
    // of whether the selected section needs an override.
    if (!g_BossSectionSfxFixPending)
        return;
    g_BossSectionSfxFixPending = false;

    SoundIdx sound = NO_SOUND;
    switch (g_Config.section)
    {
    case 34: // TH06_ST5_BOSS2
    case 36: // TH06_ST5_BOSS4
    case 37: // TH06_ST5_BOSS5
    case 38: // TH06_ST5_BOSS6
        sound = SOUND_16;
        break;
    case 42: // TH06_ST6_BOSS2
        sound = SOUND_7;
        break;
    case 46: // TH06_ST6_BOSS6
        sound = SOUND_17;
        break;
    case 49: // TH06_ST6_BOSS9
        sound = SOUND_WTF_IS_THAT_LMAO;
        break;
    default:
        break;
    }
    if (sound != NO_SOUND && g_EnemyManager.bosses[0] != nullptr)
        g_EnemyManager.bosses[0]->bulletProps.sfx = sound;
}

void FilterUnpauseInput()
{
    // th06_unpause_prevent_desyncs @ 0x40223d. Replay input is authoritative
    // and must not be altered.
    if (g_GameManager.isInReplay)
        return;
    g_CurFrameInput &= ~TH_BUTTON_BOMB;
    if (g_Gui.HasCurrentMsgIdx())
        g_CurFrameInput &= ~TH_BUTTON_SHOOT;
}

static void StoreWorkingMenuConfig()
{
    g_MenuConfig = g_Config;
    g_MenuConfig.active = false;
}

static i32 RuntimeShotType()
{
    // Upstream TH06_ST6_MID2 reads GAME_MANAGER->character/shotType at patch
    // time.  The Practice-menu snapshot is not the owner of this value and is
    // especially wrong during PRAC Replay playback.
    return g_GameManager.CharacterShotType();
}

static i32 RuntimeDifficulty()
{
    // Like shotType, difficulty is game-owned runtime state once a run/replay
    // is selected. THGuiPrac's mDifficulty is only the menu snapshot used for
    // labels/rank bounds.
    return g_GameManager.difficulty;
}

#if defined(THPRAC_PORTABLE_ENABLED)
static void PublishPortableSession()
{
    if (!g_Config.active)
    {
        ThpracPortableTh06SetSessionJson(nullptr);
        return;
    }

    char json[2048];
    std::snprintf(json, sizeof(json),
        "{\"schema\":\"eagler-touhou/thprac-session/1\",\"game\":\"th06\","
        "\"params\":{\"mode\":%d,\"stage\":%d,\"warp\":%d,\"section\":%d,"
        "\"phase\":%d,\"frame\":%d,\"dlg\":%s,\"score\":%lld,\"life\":%d,"
        "\"bomb\":%d,\"power\":%d,\"graze\":%d,\"point\":%d,\"rank\":%d,"
        "\"rankLock\":%s,\"fakeType\":%d,\"difficulty\":%d,\"shotType\":%d}}",
        g_Config.mode, g_Config.stage, g_Config.warp, g_Config.section, g_Config.phase, g_Config.frame,
        g_Config.dialogue ? "true" : "false", static_cast<long long>(g_Config.score), g_Config.life,
        g_Config.bomb, g_Config.power, g_Config.graze, g_Config.point, g_Config.rank,
        g_Config.rankLock ? "true" : "false", g_Config.fakeType, RuntimeDifficulty(), RuntimeShotType());
    ThpracPortableTh06SetSessionJson(json);
}
#endif

#ifdef __EMSCRIPTEN__
static double HostNumber(const char *key, double fallback)
{
    return EM_ASM_DOUBLE({
        const session = Module.eaglerOptions?.thpracSession;
        const value = session?.params?.[UTF8ToString($0)];
        return Number.isFinite(Number(value)) ? Number(value) : $1;
    }, key, fallback);
}

static bool HostBool(const char *key, bool fallback)
{
    return EM_ASM_INT({
        const session = Module.eaglerOptions?.thpracSession;
        const value = session?.params?.[UTF8ToString($0)];
        return typeof value === 'boolean' ? value : !!$1;
    }, key, fallback) != 0;
}

static void PublishConfigToHost()
{
    char json[2048];
    std::snprintf(json, sizeof(json),
        "{\"schema\":\"eagler-touhou/thprac-session/1\",\"game\":\"th06\",\"params\":{" 
        "\"mode\":%d,\"stage\":%d,\"warp\":%d,\"section\":%d,\"phase\":%d,\"frame\":%d,"
        "\"dlg\":%s,\"score\":%lld,\"life\":%d,\"bomb\":%d,\"power\":%d,\"graze\":%d,"
        "\"point\":%d,\"rank\":%d,\"rankLock\":%s,\"fakeType\":%d,\"difficulty\":%d,\"shotType\":%d}}",
        g_Config.mode, g_Config.stage, g_Config.warp, g_Config.section, g_Config.phase, g_Config.frame,
        g_Config.dialogue ? "true" : "false", static_cast<long long>(g_Config.score), g_Config.life,
        g_Config.bomb, g_Config.power, g_Config.graze, g_Config.point, g_Config.rank,
        g_Config.rankLock ? "true" : "false", g_Config.fakeType, RuntimeDifficulty(), RuntimeShotType());
    EM_ASM({
        Module.eaglerOptions = Module.eaglerOptions || {};
        Module.eaglerOptions.thpracSession = JSON.parse(UTF8ToString($0));
    }, json);
}
#endif

void SetConfig(const Config &config)
{
    g_Config = config;
    g_Config.mode = Clamp(g_Config.mode, 0, 1);
    g_Config.stage = Clamp(g_Config.stage, 0, 6);
    g_Config.warp = Clamp(g_Config.warp, 0, 9);
    g_Config.section = Clamp(g_Config.section, 0, 19999);
    g_Config.phase = Clamp(g_Config.phase, 0, 64);
    g_Config.frame = std::max(0, g_Config.frame);
    g_Config.score = Clamp<std::int64_t>(g_Config.score, 0, 9999999990LL);
    g_Config.life = Clamp(g_Config.life, 0, 8);
    g_Config.bomb = Clamp(g_Config.bomb, 0, 8);
    g_Config.power = Clamp(g_Config.power, 0, 128);
    g_Config.graze = Clamp(g_Config.graze, 0, 99999);
    g_Config.point = Clamp(g_Config.point, 0, 9999);
    g_Config.rank = Clamp(g_Config.rank, 0, 99);
    g_Config.fakeType = Clamp(g_Config.fakeType, 0, 4);
#if defined(THPRAC_PORTABLE_ENABLED)
    // The portable wire schema has no `active` field: deserializing any valid
    // session object makes adapter Session::active=true.  Therefore an
    // inactive C++ Config must be represented by *absence* of a session, not
    // by serializing the old values with active=false only on the C++ side.
    if (g_Config.active)
        PublishPortableSession();
    else
        ThpracPortableTh06SetSessionJson(nullptr);
#endif
}

void RefreshFromHost()
{
    if (g_PreserveConfigOnRestart)
    {
        g_PreserveConfigOnRestart = false;
        return;
    }
    if (g_PreserveConfigOnFreshStart)
    {
        g_PreserveConfigOnFreshStart = false;
        return;
    }
    // THGuiRep::State(3) has already copied mRepParam into live thPracParam
    // before the original isInReplay write. That live object must survive the
    // following GameManager initialization. It is a separate ownership path
    // from THPauseMenu Restart / fresh Practice accept above.
    if (g_ReplayStartupCommitted && g_GameManager.isInReplay)
    {
        g_BossSectionSfxFixPending = false;
        return;
    }
    g_BossSectionSfxFixPending = false;
#ifdef __EMSCRIPTEN__
    // thPracParam belongs to the current Practice/Replay run.  The Web host
    // keeps Module alive after returning to title, so an accepted Practice
    // session can otherwise survive there and be re-imported by an ordinary
    // Start.  End that live ownership at every non-Practice new-run boundary.
    // Deliberately do not touch g_MenuConfig: upstream THGuiPrac remembers its
    // widget values independently from thPracParam, and reopening Practice
    // should still show the user's previous selections.
    if (!g_GameManager.isInPracticeMode)
    {
        g_Config = {};
        EM_ASM({
            Module.eaglerOptions = Module.eaglerOptions || {};
            Module.eaglerOptions.thpracSession = null;
        });
#if defined(THPRAC_PORTABLE_ENABLED)
        ThpracPortableTh06SetSessionJson(nullptr);
#endif
        return;
    }

    const bool active = EM_ASM_INT({
        const session = Module.eaglerOptions?.thpracSession;
        return !!session && session.game === 'th06' &&
            session.schema === 'eagler-touhou/thprac-session/1';
    }) != 0;
    if (!active)
    {
        g_Config = {};
#if defined(THPRAC_PORTABLE_ENABLED)
        // Keep the three live owners atomic.  If the Web host no longer owns
        // a session, the portable adapter must not retain the previous run's
        // section/context until ECL loading happens to refresh it later.
        ThpracPortableTh06SetSessionJson(nullptr);
#endif
        return;
    }
    Config config;
    config.active = true;
    config.mode = static_cast<i32>(HostNumber("mode", 0));
    config.stage = static_cast<i32>(HostNumber("stage", 0));
    config.warp = static_cast<i32>(HostNumber("warp", 0));
    config.section = static_cast<i32>(HostNumber("section", 0));
    config.phase = static_cast<i32>(HostNumber("phase", 0));
    config.frame = static_cast<i32>(HostNumber("frame", 0));
    config.dialogue = HostBool("dlg", false);
    config.score = static_cast<std::int64_t>(HostNumber("score", 0));
    config.life = static_cast<i32>(HostNumber("life", 0));
    config.bomb = static_cast<i32>(HostNumber("bomb", 0));
    config.power = static_cast<i32>(HostNumber("power", 0));
    config.graze = static_cast<i32>(HostNumber("graze", 0));
    config.point = static_cast<i32>(HostNumber("point", 0));
    config.rank = static_cast<i32>(HostNumber("rank", 0));
    config.rankLock = HostBool("rankLock", false);
    config.fakeType = static_cast<i32>(HostNumber("fakeType", 0));
    SetConfig(config);
#else
    // Upstream th06_restart resets thPracParam on every ordinary new/reinit
    // path unless THPauseMenu explicitly requested its custom Restart.  Keep
    // the remembered THGuiPrac menu values, but deactivate the live runtime
    // parameters so an old advanced Practice cannot leak into Story/Replay or
    // a later vanilla run.
    g_Config = {};
#if defined(THPRAC_PORTABLE_ENABLED)
    ThpracPortableTh06SetSessionJson(nullptr);
#endif
#endif
}

const Config &GetConfig()
{
    return g_Config;
}

void SyncRuntimeDerivedSession()
{
    if (!g_Config.active)
        return;
#if defined(THPRAC_PORTABLE_ENABLED)
    PublishPortableSession();
#endif
#ifdef __EMSCRIPTEN__
    PublishConfigToHost();
#endif
}

bool Active()
{
    return g_Config.active;
}

bool ConsumeResultReplaySaveRequest()
{
    const bool requested = g_ResultReplaySaveRequested;
    g_ResultReplaySaveRequested = false;
    return requested;
}

bool Enabled()
{
#ifdef __EMSCRIPTEN__
    return EM_ASM_INT({ return !!Module.eaglerOptions?.thpracEnabled; }) != 0;
#else
    // The desktop feature is gated by the vanilla Practice state. A session
    // file is metadata for the Web host/replay bridge, never a second launch
    // path for the game menu. Do not additionally gate this on isInReplay:
    // TH06 keeps that flag set after leaving Replay until the next non-replay
    // run actually starts, while upstream th06_prac_menu_1 opens THGuiPrac
    // unconditionally when the Practice stage-selection hook is reached.
    // Replay-specific ownership is handled at the gameplay Pause/Result hooks.
    return g_GameManager.isInPracticeMode != 0;
#endif
}

bool SuppressStageIntroTitles()
{
    // th06_title in thprac v2.3.0.3 jumps over the complete stage/song title
    // initialization block only for a custom run with a non-zero section.
    // Whole-stage custom runs and vanilla Practice retain the original intro.
    return Active() && g_Config.mode != 0 && g_Config.section != 0;
}

void OpenPracticeMenu(i32 difficulty, i32 shotType)
{
    // Upstream TH06 does not Reset() inside THGuiPrac::State(1) because the
    // ordinary th06_restart boundary has already reset the live thPracParam
    // before this stage-selection hook is reached.  Our remembered widget
    // model is intentionally separate, but the Web host and portable adapter
    // used to keep the *previous run* alive here while g_Config was already
    // inactive.  That split ownership lets repeated Practice entries observe
    // progressively stale/empty parameters (notably power/section).  Restore
    // the upstream boundary explicitly: fresh THGuiPrac owns only persistent
    // widgets until State(3) commits a new live run.
    g_PreserveConfigOnFreshStart = false;
#ifdef __EMSCRIPTEN__
    EM_ASM({
        Module.eaglerOptions = Module.eaglerOptions || {};
        Module.eaglerOptions.thpracSession = null;
    });
#endif
#if defined(THPRAC_PORTABLE_ENABLED)
    ThpracPortableTh06SetSessionJson(nullptr);
#endif

    // THGuiPrac's widget members persist independently from thPracParam.
    // State(1) never copies live/replay thPracParam back into those widgets,
    // so the Web host session must not become a hidden menu-restore source.
    // Always edit the remembered menu model; runtime parameters are committed
    // only on State(3)/State(5).
    g_Config = g_MenuConfig;
    g_Config.active = false;
    g_MenuDifficulty = Clamp(difficulty, 0, 5);
    g_MenuShotType = shotType;
    g_MenuCursor = 0;
    // mSection/mChapter are persistent THGuiPrac widget members. State(1)
    // updates only mDiffculty/mShotType and must not reset these selectors.
    g_ImGuiMenuFocusPending = true;
    g_ImGuiMenuFocusLabel = "Stage";
    g_MenuAlpha = 0.0f;
    g_MenuCloseStep = 0.1f;
    g_MenuVisualState = MenuVisualState::Opening;
    g_MenuPendingResult = MenuResult::Waiting;
    g_MenuOpen = true;
    g_MenuResult = MenuResult::Waiting;
    g_ResultReplaySaveRequested = false;
}

struct CurrentSectionInfo
{
    const char *name = nullptr;
    bool dialogue = false;
    i32 id = 0;
};

static i32 ChapterLimit(i32 stage)
{
    static constexpr i32 chapterCounts[7][2] = {
        {4, 2}, {2, 2}, {4, 3}, {4, 5}, {3, 2}, {2, 0}, {4, 3}};
    stage = Clamp(stage, 0, 6);
    return chapterCounts[stage][0] + chapterCounts[stage][1];
}

#if defined(THPRAC_PORTABLE_ENABLED)
static i32 BuildSectionMatchesFor(i32 stage, i32 warp, i32 fakeType, i32 menuDifficulty, i32 menuShotType,
                                  const thprac::portable::generated::SectionLabel **matches, i32 capacity)
{
    using namespace thprac::portable::generated;
    if (warp < 2 || warp > 5)
        return 0;

    const std::uint32_t difficultyBit = 1u << Clamp(menuDifficulty, 0, 5);
    i32 count = 0;
    i32 sectionStage = stage;
    if (stage == 3)
    {
        const i32 shot = fakeType != 0 ? fakeType - 1 : menuShotType;
        sectionStage += 4 + Clamp(shot, 0, 3);
    }

    // Original thprac v2.3.0.3 takes section availability from the stage's
    // th_sections_cba/cbt arrays. Difficulty only selects the display string
    // for IDs that have difficulty-specific names. The old portable bridge
    // incorrectly used difficultyMask as an availability filter, so selecting
    // Extra from a Normal/Hard/Lunatic Practice entry produced an empty list.
    const SectionLabel *candidate = nullptr;
    i32 candidatePatchId = -1;
    auto flushCandidate = [&]() {
        if (candidate != nullptr && count < capacity)
            matches[count++] = candidate;
        candidate = nullptr;
        candidatePatchId = -1;
    };

    for (const SectionLabel &entry : th06SectionLabels)
    {
        if (entry.stage != sectionStage)
            continue;
        if ((warp == 2 && entry.bgm != 0) || (warp == 3 && entry.bgm != 1) ||
            (warp == 4 && entry.spell != 0) || (warp == 5 && entry.spell != 1))
            continue;

        if (candidate != nullptr && entry.patchId != candidatePatchId)
            flushCandidate();
        if (candidate == nullptr)
        {
            candidate = &entry;
            candidatePatchId = entry.patchId;
        }
        else if (!(candidate->difficultyMask & difficultyBit) && (entry.difficultyMask & difficultyBit))
        {
            // Prefer the matching difficulty's wording. If no variant matches,
            // keep the first row so the section ID remains available.
            candidate = &entry;
        }
    }
    flushCandidate();
    return count;
}

static i32 BuildSectionMatches(const thprac::portable::generated::SectionLabel **matches, i32 capacity)
{
    return BuildSectionMatchesFor(g_Config.stage, g_Config.warp, g_Config.fakeType,
                                  g_MenuDifficulty, g_MenuShotType, matches, capacity);
}
#endif

static const CurrentSectionInfo *CurrentSection(i32 offset = 0, bool syncFromConfig = true)
{
    static CurrentSectionInfo current;
    static char chapterName[64] = {};

    if (g_Config.warp == 1)
    {
        const i32 chapterLimit = ChapterLimit(g_Config.stage);
        if (offset != 0)
            g_MenuChapter = Clamp(g_MenuChapter + offset, 1, chapterLimit);
        else if (syncFromConfig && g_Config.section >= 10000 && g_Config.section / 100 - 101 == g_Config.stage)
            g_MenuChapter = Clamp(g_Config.section % 100, 1, chapterLimit);
        g_Config.section = 10000 + (g_Config.stage + 1) * 100 + g_MenuChapter;
        std::snprintf(chapterName, sizeof(chapterName), "%s %d",
                      ThpracImGui::Text(ThpracImGui::TextId::Chapter), g_MenuChapter);
        current.name = chapterName;
        current.dialogue = false;
        current.id = g_Config.section;
        return &current;
    }

#if defined(THPRAC_PORTABLE_ENABLED)
    if (g_Config.warp >= 2 && g_Config.warp <= 5)
    {
        using namespace thprac::portable::generated;
        const SectionLabel *matches[160] = {};
        const i32 count = BuildSectionMatches(matches, 160);
        if (count == 0)
            return nullptr;
        if (offset == 0 && syncFromConfig)
        {
            for (i32 index = 0; index < count; index++)
            {
                if (matches[index]->patchId == g_Config.section)
                {
                    g_MenuSectionIndex = index;
                    break;
                }
            }
        }
        g_MenuSectionIndex = (g_MenuSectionIndex + offset + count) % count;
        const SectionLabel *selected = matches[g_MenuSectionIndex];
        g_Config.section = selected->patchId;
        switch (ThpracImGui::GetLocale())
        {
        case ThpracImGui::Locale::ZhCN:
            current.name = selected->zh;
            break;
        case ThpracImGui::Locale::JaJP:
            current.name = selected->ja;
            break;
        case ThpracImGui::Locale::EnUS:
        default:
            current.name = selected->en;
            break;
        }
        current.dialogue = selected->dialogue;
        current.id = selected->patchId;
        return &current;
    }
#else
    (void)offset;
#endif
    current.name = g_Config.warp == 0 ? "Whole stage" : "Section unavailable";
    current.dialogue = false;
    current.id = g_Config.section;
    return &current;
}

static bool SectionStoresFakeType(i32 section)
{
    // TH06_ST4_BOSS1 .. TH06_ST4_BOSS7 in upstream thprac.
    return section >= 24 && section <= 30;
}

static void CommitMenuConfigToRuntime(bool preserveConditionalFields)
{
    // State(3) (fresh Practice accept) runs after thPracParam.Reset(), while
    // State(5) (THPauseMenu Restart) writes over the existing thPracParam.
    // mDlg and mFakeShot are conditional assignments in both states, so an
    // unsupported newly selected section either keeps the previous runtime
    // value on Restart or remains reset on a fresh accept.
    const Config previousRuntime = g_Config;
    g_Config = g_MenuConfig;
    g_Config.active = false;
    const CurrentSectionInfo *section = CurrentSection();
    StoreWorkingMenuConfig();

    Config committed = g_Config;
    committed.active = true;
    // TH06 THPracParam has no warp field at all. Warp is strictly persistent
    // THGuiPrac widget state; section/frame already encode the committed path.
    committed.warp = 0;
    if (!(section && section->dialogue))
        committed.dialogue = preserveConditionalFields ? previousRuntime.dialogue : false;
    if (!SectionStoresFakeType(committed.section))
        committed.fakeType = preserveConditionalFields ? previousRuntime.fakeType : 0;
    g_Config = committed;
}

static void CommitRestartWithBgmPolicy()
{
    const Config previousRuntime = g_Config;
    const i32 previousBgm = InitialBgmIndexForConfig(previousRuntime);
    CommitMenuConfigToRuntime(true);
    const bool sameBgmIdentity = previousRuntime.mode == g_Config.mode &&
                                 previousRuntime.stage == g_Config.stage &&
                                 previousBgm == InitialBgmIndexForConfig(g_Config);
    // Upstream F7 only arms el_bgm_signal when the run still represents the
    // same mode/stage/BGM and no in-run BGM transition has occurred.
    g_PreserveBgmRestart = OverlayEverlastingBgm() && !g_BgmChangedSinceStart && sameBgmIdentity;
}

static i32 VisibleMenuCount()
{
    return g_Config.mode == 0 ? 3 : kMenuItemCount;
}

static i32 VisibleMenuItem(i32 cursor)
{
    static constexpr i32 stagePracticeItems[] = {0, 1, kMenuItemCount - 1};
    return g_Config.mode == 0 ? stagePracticeItems[Clamp(cursor, 0, 2)] : cursor;
}

static void AdjustMenuValue(i32 item, i32 direction)
{
    switch (item)
    {
    case 0: g_Config.mode = Clamp(g_Config.mode + direction, 0, 1); break;
    case 1:
        g_Config.stage = Clamp(g_Config.stage + direction, 0, 6);
        g_MenuSectionIndex = 0;
        g_MenuChapter = 1;
        break;
    case 2:
        g_Config.warp = Clamp(g_Config.warp + direction, 0, 6);
        g_MenuSectionIndex = 0;
        g_MenuChapter = 1;
        break;
    case 3: CurrentSection(direction); break;
    case 4: g_Config.phase = Clamp(g_Config.phase + direction, 0, 64); break;
    case 5: g_Config.dialogue = !g_Config.dialogue; break;
    case 6: g_Config.life = Clamp(g_Config.life + direction, 0, 8); break;
    case 7: g_Config.bomb = Clamp(g_Config.bomb + direction, 0, 8); break;
    case 8: g_Config.power = Clamp(g_Config.power + direction * 8, 0, 128); break;
    case 9: g_Config.score = Clamp<std::int64_t>(g_Config.score + direction * 1000000LL, 0, 9999999990LL); break;
    case 10: g_Config.graze = Clamp(g_Config.graze + direction * 100, 0, 99999); break;
    case 11: g_Config.point = Clamp(g_Config.point + direction * 10, 0, 9999); break;
    case 12: g_Config.rank = Clamp(g_Config.rank + direction, 0, 99); break;
    case 13: g_Config.rankLock = !g_Config.rankLock; break;
    case 14: g_Config.fakeType = Clamp(g_Config.fakeType + direction, 0, 4); break;
    }
    CurrentSection();
}

static void RequestMenuClose(MenuResult result, bool accept)
{
    if (!g_MenuOpen || g_MenuVisualState == MenuVisualState::Closing)
        return;

    if (accept)
    {
        CurrentSection();
        StoreWorkingMenuConfig();
        CommitMenuConfigToRuntime(false);
        g_PreserveConfigOnFreshStart = true;
#ifdef TH_DEV_TOOLS
        SDL_Log("TH06 thprac accept: mode=%d stage=%d warp=%d section=%d dialogue=%d phase=%d frame=%d",
                g_Config.mode, g_Config.stage, g_Config.warp, g_Config.section,
                g_Config.dialogue ? 1 : 0, g_Config.phase, g_Config.frame);
#endif
#if defined(THPRAC_PORTABLE_ENABLED)
        PublishPortableSession();
#endif
#ifdef __EMSCRIPTEN__
        PublishConfigToHost();
#endif
        g_SoundPlayer.PlaySoundByIdx(SOUND_SELECT);
        // THGuiPrac::State(3) switches to SetFade(.8f, .8f).
        g_MenuCloseStep = 0.8f;
    }
    else
    {
        StoreWorkingMenuConfig();
        // State(4) closes the persistent widget window but does not commit it
        // into thPracParam. The live object was already Reset by th06_restart,
        // so restore the actual all-zero live state rather than keeping menu
        // values in an "inactive" pseudo-live object.
        g_Config = {};
        g_SoundPlayer.PlaySoundByIdx(SOUND_BACK);
        // THGuiPrac::State(4) keeps the normal .1f fade step.
        g_MenuCloseStep = 0.1f;
    }
    g_MenuPendingResult = result;
    g_MenuVisualState = MenuVisualState::Closing;
}

MenuResult PollPracticeMenu()
{
    if (g_MenuOpen)
    {
#ifdef TH_ENABLE_THPRAC
        // Upstream GameGuiWnd gives Dear ImGui only the four directions.
        // Vanilla Practice still owns X/Z, and the original hooks translate
        // those paths to THGuiPrac State(4)=cancel / State(3)=accept.
        if (WAS_PRESSED(TH_BUTTON_RETURNMENU))
            RequestMenuClose(MenuResult::Cancelled, false);
        else if (WAS_PRESSED(TH_BUTTON_SELECTMENU))
            RequestMenuClose(MenuResult::Accepted, true);
        return MenuResult::Waiting;
#else
        if (WAS_PRESSED_PERIODIC(TH_BUTTON_UP))
        {
            const i32 count = VisibleMenuCount();
            g_MenuCursor = (g_MenuCursor + count - 1) % count;
            g_SoundPlayer.PlaySoundByIdx(SOUND_MOVE_MENU);
        }
        else if (WAS_PRESSED_PERIODIC(TH_BUTTON_DOWN))
        {
            g_MenuCursor = (g_MenuCursor + 1) % VisibleMenuCount();
            g_SoundPlayer.PlaySoundByIdx(SOUND_MOVE_MENU);
        }
        else if (WAS_PRESSED_PERIODIC(TH_BUTTON_LEFT))
        {
            AdjustMenuValue(VisibleMenuItem(g_MenuCursor), -1);
            g_MenuCursor = Clamp(g_MenuCursor, 0, VisibleMenuCount() - 1);
            g_SoundPlayer.PlaySoundByIdx(SOUND_MOVE_MENU);
        }
        else if (WAS_PRESSED_PERIODIC(TH_BUTTON_RIGHT))
        {
            AdjustMenuValue(VisibleMenuItem(g_MenuCursor), 1);
            g_MenuCursor = Clamp(g_MenuCursor, 0, VisibleMenuCount() - 1);
            g_SoundPlayer.PlaySoundByIdx(SOUND_MOVE_MENU);
        }
        else if (WAS_PRESSED(TH_BUTTON_RETURNMENU))
        {
            StoreWorkingMenuConfig();
            g_Config = {};
            g_MenuOpen = false;
            g_SoundPlayer.PlaySoundByIdx(SOUND_BACK);
            return MenuResult::Cancelled;
        }
        else if (WAS_PRESSED(TH_BUTTON_SELECTMENU))
        {
            if (VisibleMenuItem(g_MenuCursor) == kMenuItemCount - 1)
            {
                CurrentSection();
                StoreWorkingMenuConfig();
                CommitMenuConfigToRuntime(false);
                g_PreserveConfigOnFreshStart = true;
#if defined(THPRAC_PORTABLE_ENABLED)
                PublishPortableSession();
#endif
#ifdef __EMSCRIPTEN__
                PublishConfigToHost();
#endif
                g_MenuOpen = false;
                g_SoundPlayer.PlaySoundByIdx(SOUND_SELECT);
                return MenuResult::Accepted;
            }
            AdjustMenuValue(VisibleMenuItem(g_MenuCursor), 1);
            g_MenuCursor = Clamp(g_MenuCursor, 0, VisibleMenuCount() - 1);
            g_SoundPlayer.PlaySoundByIdx(SOUND_MOVE_MENU);
        }
        return MenuResult::Waiting;
#endif
    }
    if (g_MenuResult != MenuResult::Waiting)
    {
        const MenuResult result = g_MenuResult;
        g_MenuResult = MenuResult::Waiting;
        return result;
    }
    return MenuResult::Waiting;
}

#ifdef TH_ENABLE_THPRAC
static bool GuiInputLeft()
{
    return ThpracImGui::InputPressed(TH_BUTTON_LEFT);
}

static bool GuiInputRight()
{
    return ThpracImGui::InputPressed(TH_BUTTON_RIGHT);
}

static bool GuiConfirmFocusedItem()
{
    // Upstream GuiButton::operator() does not depend on ImGui NavActivate.
    // It accepts a game confirm (Z) or Return separately when the button is
    // focused. TH_BUTTON_SELECTMENU is exactly TH_BUTTON_SHOOT|TH_BUTTON_ENTER
    // in TH06, and SetGameInput() has already reduced it to a 60 Hz edge.
    return ImGui::IsItemFocused() && ThpracImGui::InputPressed(TH_BUTTON_SELECTMENU);
}

static bool GuiPauseButton(const char *label, const ImVec2 &size)
{
    return ImGui::Button(label, size) || GuiConfirmFocusedItem();
}

template <typename T> struct GuiStepState
{
    T value;
    T minimum;
    T maximum;
    T multiplier;
};

template <typename T> static void CycleGuiStep(GuiStepState<T> &step)
{
    // TH06's original Gen1 input helper treats Shift as a rising-edge input;
    // GuiSlider/GuiDrag then cycles the persistent step on that edge.
    if (!ThpracImGui::InputPressed(TH_BUTTON_FOCUS))
        return;
    if (step.value == step.maximum)
    {
        step.value = step.minimum;
        return;
    }
    step.value *= step.multiplier;
    if (step.value > step.maximum)
        step.value = step.maximum;
}

static void RememberGuiFocus(const char *label)
{
    if (label == nullptr)
        return;
    if (ImGui::IsItemFocusedAlt(label) || ImGui::IsItemFocused())
    {
        g_ImGuiMenuFocusLabel = label;
#ifdef TH_DEV_TOOLS
        static std::string lastLoggedFocus;
        if (lastLoggedFocus != label)
        {
            SDL_Log("TH06 thprac focus: %s", label);
            lastLoggedFocus = label;
        }
#endif
    }
}

template <typename T>
static bool GuiCombo(const char *label, i32 *current, T *selector, const char **items)
{
    const i32 old = *current;
    bool changed = ImGui::ComboSections(label, current, selector, items, "");
    if (ImGui::IsItemFocused() && old == *current)
    {
        if (GuiInputLeft())
        {
            --*current;
            if (*current < 0)
            {
                *current = 0;
                while (selector[*current + 1] != 0)
                    ++*current;
            }
            changed = true;
        }
        else if (GuiInputRight())
        {
            ++*current;
            if (selector[*current] == 0)
                *current = 0;
            changed = true;
        }
    }
    RememberGuiFocus(label);
    return changed;
}

static bool GuiSliderInt(const char *label, i32 *value, i32 minimum, i32 maximum,
                         GuiStepState<i32> &step, const char *format = "%d")
{
    const i32 old = *value;
    bool changed = ImGui::SliderInt(label, value, minimum, maximum, format);
    if (ImGui::IsItemFocused())
    {
        CycleGuiStep(step);
        if (GuiInputLeft())
        {
            *value = Clamp(*value - step.value, minimum, maximum);
            changed = true;
        }
        else if (GuiInputRight())
        {
            *value = Clamp(*value + step.value, minimum, maximum);
            changed = true;
        }
    }
    changed |= old != *value;
    RememberGuiFocus(label);
    return changed;
}

static bool GuiDragInt(const char *label, i32 *value, i32 minimum, i32 maximum,
                       GuiStepState<i32> &step, const char *format = "%d")
{
    const i32 old = *value;
    bool changed = ImGui::DragInt(label, value, static_cast<float>(step.value * 2), minimum, maximum, format);
    if (*value < minimum)
        *value = minimum;
    else if (*value > maximum)
        *value = maximum;
    if (ImGui::IsItemFocused())
    {
        CycleGuiStep(step);
        if (GuiInputLeft())
        {
            *value = Clamp(*value - step.value, minimum, maximum);
            changed = true;
        }
        else if (GuiInputRight())
        {
            *value = Clamp(*value + step.value, minimum, maximum);
            changed = true;
        }
    }
    changed |= old != *value;
    RememberGuiFocus(label);
    return changed;
}

static bool GuiDragScore()
{
    static GuiStepState<std::int64_t> step {10, 10, 100000000, 10};
    const std::int64_t minimum = 0;
    const std::int64_t maximum = 9999999990LL;
    const std::int64_t old = g_Config.score;
    const char *label = ThpracImGui::Text(ThpracImGui::TextId::Score);
    bool changed = ImGui::DragScalar(label, ImGuiDataType_S64, &g_Config.score,
                                    static_cast<float>(step.value * 2), &minimum, &maximum, "%lld");
    if (g_Config.score < minimum)
        g_Config.score = minimum;
    else if (g_Config.score > maximum)
        g_Config.score = maximum;
    if (ImGui::IsItemFocused())
    {
        CycleGuiStep(step);
        if (GuiInputLeft())
        {
            g_Config.score = std::max(minimum, g_Config.score - step.value);
            changed = true;
        }
        else if (GuiInputRight())
        {
            g_Config.score = std::min(maximum, g_Config.score + step.value);
            changed = true;
        }
    }
    changed |= old != g_Config.score;
    g_Config.score -= g_Config.score % 10;
    RememberGuiFocus(label);
    return changed;
}

static bool GuiCheckBox(const char *label, bool *value)
{
    bool changed = ImGui::Checkbox(label, value);
    if (ImGui::IsItemFocused() && (ThpracImGui::InputPressed(TH_BUTTON_LEFT) ||
                                   ThpracImGui::InputPressed(TH_BUTTON_RIGHT)))
    {
        *value = !*value;
        changed = true;
    }
    RememberGuiFocus(label);
    return changed;
}

static const char *SectionLabel(i32 warp)
{
    static const ThpracImGui::TextId labels[] = {
        ThpracImGui::TextId::Mode,
        ThpracImGui::TextId::Chapter,
        ThpracImGui::TextId::MidBoss,
        ThpracImGui::TextId::EndBoss,
        ThpracImGui::TextId::NonSpell,
        ThpracImGui::TextId::SpellCard,
        ThpracImGui::TextId::Frame,
    };
    return ThpracImGui::Text(labels[Clamp(warp, 0, 6)]);
}

static void FinishPracticeNav()
{
    const bool focusPending = g_ImGuiMenuFocusPending;
    if (focusPending)
    {
        g_ImGuiMenuFocusLabel = ThpracImGui::Text(ThpracImGui::TextId::Stage);
        g_ImGuiMenuFocusPending = false;
    }
    if (!ImGui::IsPopupOpen("", ImGuiPopupFlags_AnyPopup))
        ImGui::SetWindowFocus();

    // The upstream nav list starts with Stage even though Mode is drawn first.
    ImGui::SetItemFocusAlt(g_ImGuiMenuFocusLabel, focusPending);
}

static void ResetWarpDependentMenuState()
{
    // THGuiPrac::PracticeMenu() resets exactly these values when mWarp()
    // changes. Mode changes only hide/show the advanced controls and must not
    // destroy their state; stage/fake-shot changes likewise have their own
    // narrower section-index behavior.
    g_MenuSectionIndex = 0;
    g_MenuChapter = 1;
    g_Config.phase = 0;
    g_Config.frame = 0;
    CurrentSection(0, false);
}

static void DrawPracticeControls(bool includeActions)
{
    (void)includeActions;
    static GuiStepState<i32> chapterStep {1, 1, 1, 10};
    static GuiStepState<i32> frameStep {1, 1, 1, 10};
    static GuiStepState<i32> lifeStep {1, 1, 1, 10};
    static GuiStepState<i32> bombStep {1, 1, 1, 10};
    static GuiStepState<i32> powerStep {1, 1, 1, 10};
    static GuiStepState<i32> grazeStep {1, 1, 10000, 10};
    static GuiStepState<i32> pointStep {1, 1, 1000, 10};
    static GuiStepState<i32> rankStep {1, 1, 10, 10};
    static int modeSelector[] = {1, 2, 0};
    // TH_STAGE_SELECT in the original locale table contains the bare stage
    // numbers; the surrounding "Stage" label is rendered by the combo row.
    static int stageSelector[] = {1, 2, 3, 4, 5, 6, 7, 0};
    static int warpSelector[] = {1, 2, 3, 4, 5, 6, 7, 0};
    static int fakeShotSelector[] = {1, 2, 3, 4, 5, 0};
    static int phaseSelector[] = {1, 2, 0};

    const char *modeItems[] = {"", ThpracImGui::Text(ThpracImGui::TextId::Original),
                               ThpracImGui::Text(ThpracImGui::TextId::Custom)};
    const char *stageItems[] = {"", "1", "2", "3", "4", "5", "6",
                                ThpracImGui::Text(ThpracImGui::TextId::Extra)};
    const char *warpItems[] = {"", ThpracImGui::Text(ThpracImGui::TextId::None),
                               ThpracImGui::Text(ThpracImGui::TextId::StagePortion),
                               ThpracImGui::Text(ThpracImGui::TextId::MidBoss),
                               ThpracImGui::Text(ThpracImGui::TextId::EndBoss),
                               ThpracImGui::Text(ThpracImGui::TextId::NonSpell),
                               ThpracImGui::Text(ThpracImGui::TextId::SpellCard),
                               ThpracImGui::Text(ThpracImGui::TextId::Frame)};
    const char *fakeShotItems[] = {"", ThpracImGui::Text(ThpracImGui::TextId::None),
                                   ThpracImGui::Text(ThpracImGui::TextId::ReimuA),
                                   ThpracImGui::Text(ThpracImGui::TextId::ReimuB),
                                   ThpracImGui::Text(ThpracImGui::TextId::MarisaA),
                                   ThpracImGui::Text(ThpracImGui::TextId::MarisaB)};
    const char *phaseItems[] = {"", ThpracImGui::Text(ThpracImGui::TextId::Normal),
                                ThpracImGui::Text(ThpracImGui::TextId::Rage)};

    const char *modeLabel = ThpracImGui::Text(ThpracImGui::TextId::Mode);
    const char *stageLabel = ThpracImGui::Text(ThpracImGui::TextId::Stage);
    GuiCombo(modeLabel, &g_Config.mode, modeSelector, modeItems);
    const bool stageChanged = GuiCombo(stageLabel, &g_Config.stage, stageSelector, stageItems);
    if (stageChanged)
    {
        g_MenuSectionIndex = 0;
        g_MenuChapter = 1;
        CurrentSection();
    }

    if (g_Config.mode == 1)
    {
        const bool warpChanged = GuiCombo(ThpracImGui::Text(ThpracImGui::TextId::Warp), &g_Config.warp, warpSelector,
                                          warpItems);
        if (warpChanged)
            ResetWarpDependentMenuState();
        if (g_Config.warp != 0)
        {
            if (g_Config.stage == 3 &&
                GuiCombo(ThpracImGui::Text(ThpracImGui::TextId::FakeShot), &g_Config.fakeType, fakeShotSelector,
                         fakeShotItems))
            {
                // Original mFakeShot does not reset mSection. Keep the same
                // ordinal and map it into the newly selected shot's table.
                CurrentSection(0, false);
            }

            const char *sectionLabel = SectionLabel(g_Config.warp);
            if (g_Config.warp == 1)
            {
                const i32 limit = ChapterLimit(g_Config.stage);
                char format[64] = {};
                const i32 firstHalf = g_Config.stage == 5 ? 0 : (g_Config.stage == 0 ? 4 :
                    g_Config.stage == 1 ? 2 : g_Config.stage == 2 ? 4 : g_Config.stage == 3 ? 4 :
                    g_Config.stage == 4 ? 3 : 2);
                if (firstHalf == 0)
                {
                    std::snprintf(format, sizeof(format), ThpracImGui::Text(ThpracImGui::TextId::StagePortionN),
                                   g_MenuChapter);
                }
                else if (g_MenuChapter <= firstHalf)
                {
                    std::snprintf(format, sizeof(format), ThpracImGui::Text(ThpracImGui::TextId::FirstHalf),
                                   g_MenuChapter);
                }
                else
                {
                    std::snprintf(format, sizeof(format), ThpracImGui::Text(ThpracImGui::TextId::SecondHalf),
                                   g_MenuChapter - firstHalf);
                }
                if (GuiSliderInt(sectionLabel, &g_MenuChapter, 1, limit, chapterStep, format))
                    CurrentSection(0, false);
            }
            else if (g_Config.warp >= 2 && g_Config.warp <= 5)
            {
                const CurrentSectionInfo *section = CurrentSection();
#if defined(THPRAC_PORTABLE_ENABLED)
                const thprac::portable::generated::SectionLabel *matches[160] = {};
                const i32 sectionCount = BuildSectionMatches(matches, 160);
#ifdef TH_DEV_TOOLS
                if (g_Config.stage == 6)
                {
                    static i32 lastLoggedWarp = -1;
                    static i32 lastLoggedDifficulty = -1;
                    if (lastLoggedWarp != g_Config.warp || lastLoggedDifficulty != g_MenuDifficulty)
                    {
                        SDL_Log("TH06 thprac Extra section UI: entryDifficulty=%d warp=%d count=%d first=%d last=%d",
                                g_MenuDifficulty, g_Config.warp, sectionCount,
                                sectionCount > 0 ? matches[0]->patchId : -1,
                                sectionCount > 0 ? matches[sectionCount - 1]->patchId : -1);
                        lastLoggedWarp = g_Config.warp;
                        lastLoggedDifficulty = g_MenuDifficulty;
                    }
                }
#endif
                if (sectionCount > 0)
                {
                    std::vector<const char *> sectionItems(static_cast<std::size_t>(sectionCount) + 1);
                    std::vector<int> sectionSelector(static_cast<std::size_t>(sectionCount) + 1);
                    sectionItems[0] = "";
                    for (i32 index = 0; index < sectionCount; index++)
                    {
                        switch (ThpracImGui::GetLocale())
                        {
                        case ThpracImGui::Locale::ZhCN:
                            sectionItems[static_cast<std::size_t>(index) + 1] = matches[index]->zh;
                            break;
                        case ThpracImGui::Locale::JaJP:
                            sectionItems[static_cast<std::size_t>(index) + 1] = matches[index]->ja;
                            break;
                        case ThpracImGui::Locale::EnUS:
                        default:
                            sectionItems[static_cast<std::size_t>(index) + 1] = matches[index]->en;
                            break;
                        }
                        sectionSelector[static_cast<std::size_t>(index)] = index + 1;
                    }
                    sectionSelector[static_cast<std::size_t>(sectionCount)] = 0;
                    i32 sectionIndex = Clamp(g_MenuSectionIndex, 0, sectionCount - 1);
                    if (GuiCombo(sectionLabel, &sectionIndex, sectionSelector.data(), sectionItems.data()))
                    {
                        g_MenuSectionIndex = sectionIndex;
                        // Original SectionWidget resets mPhase whenever the
                        // section combo itself changes.
                        g_Config.phase = 0;
                        // The combo edits the section-list index directly, as
                        // upstream THGuiPrac's mSection does. Do not immediately
                        // re-sync that index from the old g_Config.section or the
                        // user-visible Right/Left change is undone before it can
                        // update the selected patch ID.
                        section = CurrentSection(0, false);
                    }
                }
#else
                (void)section;
#endif
                if (section && section->dialogue)
                    GuiCheckBox(ThpracImGui::Text(ThpracImGui::TextId::Dialog), &g_Config.dialogue);
            }
            else if (g_Config.warp == 6)
            {
                GuiDragInt(sectionLabel, &g_Config.frame, 0, 0x7fffffff, frameStep);
            }

            const CurrentSectionInfo *section = CurrentSection();
            if (section && section->id == 70)
            {
                g_Config.phase = Clamp(g_Config.phase, 0, 1);
                GuiCombo(ThpracImGui::Text(ThpracImGui::TextId::Phase), &g_Config.phase, phaseSelector, phaseItems);
            }
        }

        GuiSliderInt(ThpracImGui::Text(ThpracImGui::TextId::Life), &g_Config.life, 0, 8, lifeStep);
        GuiSliderInt(ThpracImGui::Text(ThpracImGui::TextId::Bomb), &g_Config.bomb, 0, 8, bombStep);
        GuiDragScore();
        GuiSliderInt(ThpracImGui::Text(ThpracImGui::TextId::Power), &g_Config.power, 0, 128, powerStep);
        GuiDragInt(ThpracImGui::Text(ThpracImGui::TextId::Graze), &g_Config.graze, 0, 99999, grazeStep);
        GuiDragInt(ThpracImGui::Text(ThpracImGui::TextId::Point), &g_Config.point, 0, 9999, pointStep);
        const i32 rankMax = g_Config.rankLock ? 99 : 32;
        g_Config.rank = Clamp(g_Config.rank, 0, rankMax);
        GuiSliderInt(ThpracImGui::Text(ThpracImGui::TextId::Rank), &g_Config.rank, 0, rankMax, rankStep);
        GuiCheckBox(ThpracImGui::Text(ThpracImGui::TextId::RankLock), &g_Config.rankLock);
    }

    FinishPracticeNav();
}

void DrawPracticeMenu()
{
    if (!g_MenuOpen || !ThpracImGui::IsFrameOpen())
        return;

    if (g_MenuVisualState == MenuVisualState::Opening)
    {
        g_MenuAlpha = std::min(0.8f, g_MenuAlpha + 0.1f);
        if (g_MenuAlpha >= 0.8f)
            g_MenuVisualState = MenuVisualState::Open;
    }
    else if (g_MenuVisualState == MenuVisualState::Closing)
    {
        g_MenuAlpha = std::max(0.0f, g_MenuAlpha - g_MenuCloseStep);
        if (g_MenuAlpha <= 0.0f)
        {
            g_MenuVisualState = MenuVisualState::Closed;
            g_MenuOpen = false;
            g_MenuResult = g_MenuPendingResult;
            g_MenuPendingResult = MenuResult::Waiting;
            return;
        }
    }

    // This is the same in-game window boundary and default flags as THGuiPrac.
    // The game input bridge feeds ImGui's own navigation arrays; no launcher
    // or browser-side cursor is involved.
    const bool englishLayout = ThpracImGui::GetLocale() == ThpracImGui::Locale::EnUS;
    const bool japaneseLayout = ThpracImGui::GetLocale() == ThpracImGui::Locale::JaJP;
    ImGui::SetNextWindowSize(englishLayout ? ImVec2(370.0f, 375.0f) : ImVec2(330.0f, 390.0f),
                             ImGuiCond_Always);
    ImGui::SetNextWindowPos(englishLayout ? ImVec2(240.0f, 75.0f) : ImVec2(260.0f, 65.0f),
                            ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(g_MenuAlpha);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    constexpr ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize |
                                               ImGuiWindowFlags_NoCollapse |
                                               ImGuiWindowFlags_NoTitleBar |
                                               ImGuiWindowFlags_NoMove |
                                               ImGuiWindowFlags_NoBringToFrontOnFocus;
    if (ImGui::Begin(ThpracImGui::Text(ThpracImGui::TextId::Menu), nullptr, windowFlags))
    {
        // Upstream GameGuiWnd pushes the item width only after Begin() and
        // pops it before End(). Popping after End() operates on ImGui's
        // implicit Debug##Default window; GetCurrentWindow() then marks that
        // fallback WriteAccessed, keeping it Active on the next frame where
        // it can sit above this NoBringToFrontOnFocus window and steal mouse
        // hover despite the visible controls being geometrically hit.
        ImGui::PushItemWidth(japaneseLayout ? -65.0f : -60.0f);
        ImGui::TextUnformatted(ThpracImGui::Text(ThpracImGui::TextId::Menu));
        ImGui::Separator();

        if (g_MenuVisualState == MenuVisualState::Open)
        {
            DrawPracticeControls(true);
            StoreWorkingMenuConfig();
        }
        ImGui::PopItemWidth();
    }
    ImGui::End();
    ImGui::PopStyleVar(2);
    return;
#else
void DrawPracticeMenu()
{
    if (!g_MenuOpen)
        return;

    const auto *section = CurrentSection();
    const char *sectionName = section ? section->name : "Whole stage";
    const char *modeName = g_Config.mode ? "Advanced practice" : "Stage practice";
    const char *warpNames[] = {"Whole stage", "Chapter", "Midboss", "Boss", "Nonspell", "Spell", "Frame"};
    char lines[kMenuItemCount][64] = {};
    std::snprintf(lines[0], 64, "Mode       %s", modeName);
    std::snprintf(lines[1], 64, "Stage      %d", g_Config.stage + 1);
    std::snprintf(lines[2], 64, "Warp       %s", warpNames[Clamp(g_Config.warp, 0, 6)]);
    std::snprintf(lines[3], 64, "Section    %.46s", sectionName);
    std::snprintf(lines[4], 64, "Phase      %d", g_Config.phase);
    std::snprintf(lines[5], 64, "Dialogue   %s", g_Config.dialogue ? "On" : "Off");
    std::snprintf(lines[6], 64, "Lives      %d", g_Config.life);
    std::snprintf(lines[7], 64, "Bombs      %d", g_Config.bomb);
    std::snprintf(lines[8], 64, "Power      %d", g_Config.power);
    std::snprintf(lines[9], 64, "Score      %lld", static_cast<long long>(g_Config.score));
    std::snprintf(lines[10], 64, "Graze      %d", g_Config.graze);
    std::snprintf(lines[11], 64, "Point      %d", g_Config.point);
    std::snprintf(lines[12], 64, "Rank       %d", g_Config.rank);
    std::snprintf(lines[13], 64, "Rank lock  %s", g_Config.rankLock ? "On" : "Off");
    std::snprintf(lines[14], 64, "Fake shot  %d", g_Config.fakeType);
    std::snprintf(lines[15], 64, "Start advanced practice");

    const i32 visibleCount = VisibleMenuCount();
    const i32 shownRows = std::min(12, visibleCount);
    const i32 first = Clamp(g_MenuCursor - 6, 0, std::max(0, visibleCount - shownRows));
    ZunRect panel {232.0f, 36.0f, 624.0f, 444.0f};
    ScreenEffect::DrawSquare(&panel, 0xff181820);
    ZunVec3 titlePos(248.0f, 52.0f, 0.0f);
    g_AsciiManager.color = 0xfff0c080;
    g_AsciiManager.AddString(&titlePos, "thprac - Advanced Practice");
    static const char *difficultyNames[] = {"Easy", "Normal", "Hard", "Lunatic", "Extra", "Phantasm"};
    static const char *shotNames[] = {"Reimu A", "Reimu B", "Marisa A", "Marisa B"};
    ZunVec3 contextPos(248.0f, 76.0f, 0.0f);
    g_AsciiManager.color = 0xffc0c0c0;
    g_AsciiManager.AddFormatText(&contextPos, "%s / %s", difficultyNames[Clamp(g_MenuDifficulty, 0, 5)],
                                 shotNames[Clamp(g_MenuShotType, 0, 3)]);
    for (i32 row = 0; row < shownRows; row++)
    {
        const i32 visibleIndex = first + row;
        const i32 index = VisibleMenuItem(visibleIndex);
        ZunVec3 pos(248.0f, 108.0f + row * 24.0f, 0.0f);
        g_AsciiManager.color = visibleIndex == g_MenuCursor ? 0xfff0f0c0 : 0xffa0c0c0;
        g_AsciiManager.AddFormatText(&pos, "%c %s", visibleIndex == g_MenuCursor ? '>' : ' ', lines[index]);
    }
    g_AsciiManager.color = 0xffffffff;
#endif
}

void DebugAcceptPracticeMenu()
{
    if (!g_MenuOpen)
        return;
    CurrentSection();
    StoreWorkingMenuConfig();
    CommitMenuConfigToRuntime(false);
    g_PreserveConfigOnFreshStart = true;
#if defined(THPRAC_PORTABLE_ENABLED)
    PublishPortableSession();
#endif
    g_MenuOpen = false;
    g_MenuVisualState = MenuVisualState::Closed;
    g_MenuPendingResult = MenuResult::Waiting;
    g_MenuResult = MenuResult::Accepted;
}

bool UpdatePauseMenu()
{
    if (!Active() || !g_GameManager.isInGameMenu || g_GameManager.isInReplay)
    {
        g_PauseWasOpen = false;
#ifdef TH_ENABLE_THPRAC
        g_ImGuiPauseFocusPending = false;
#endif
        return false;
    }

    // Upstream th06_pause_menu replaces the vanilla Pause state machine only
    // when thPracParam.mode != 0. Mode=Original keeps TH06's own Pause UI and
    // adds only the escR shortcut: R, or Ctrl+Shift+Down, restarts the current
    // vanilla Practice stage. The upstream restart hook then resets
    // thPracParam, so clear our session before reinitializing as well.
    if (g_Config.mode == 0)
    {
        const bool quickRestart = IS_PRESSED(TH_BUTTON_R) ||
            (IS_PRESSED(TH_BUTTON_SKIP) && IS_PRESSED(TH_BUTTON_FOCUS) && IS_PRESSED(TH_BUTTON_DOWN));
        if (!quickRestart)
            return false;
#ifdef TH_DEV_TOOLS
        SDL_Log("TH06 thprac original pause: quick restart");
#endif
        ScreenEffect::RequestShakeCancelForRestart();
        ResetTouchReplayForPracticeRestart();
        SetConfig(Config {});
        g_GameManager.isInGameMenu = 0;
        g_Supervisor.curState = SUPERVISOR_STATE_GAMEMANAGER_REINIT;
        return true;
    }

    if (!g_PauseWasOpen)
    {
        g_PauseWasOpen = true;
        g_PauseSettings = false;
        g_PauseCursor = 0;
#ifdef TH_ENABLE_THPRAC
        g_ImGuiPauseFocusPending = true;
#ifdef TH_DEV_TOOLS
        SDL_Log("TH06 thprac pause: open");
#endif
#endif
    }

#ifdef TH_ENABLE_THPRAC
    // THPauseMenu::mFrameCounter is incremented by UpdateOverlay() at the
    // original post-calc th06_update boundary, not here in PMState().
    if (g_PauseAction == PauseAction::None && !g_PauseLogicalOpen)
    {
        if (g_PauseFrameCounter > 5)
        {
            // StateClose() tail-calls StateOpen(). The first call only changes
            // mState and resets the counter; Open() itself happens when the
            // new STATE_OPEN is observed with counter==1 on a later calc tick.
            g_PauseLogicalOpen = true;
            g_PauseFrameCounter = 0;
        }
        return true;
    }

    if (g_PauseAction == PauseAction::None && g_PauseLogicalOpen)
    {
        // THPauseMenu::StateOpen counter==1 calls Open(). Fade progression is
        // then owned by the post-calc GameGuiWnd::Update equivalent.
        if (g_PauseFrameCounter == 1 && g_PauseVisualState == MenuVisualState::Closed)
            g_PauseVisualState = MenuVisualState::Opening;

        // StateOpen only checks Escape/Q/R after counter > 10. Button-driven
        // state changes happen later during the post-calc ImGui update/draw.
        if (g_PauseFrameCounter > 10 && WAS_PRESSED(TH_BUTTON_MENU))
        {
            g_PauseAction = PauseAction::Resume;
            g_PauseLogicalOpen = false;
            g_PauseFrameCounter = 0;
        }
        else if (g_PauseFrameCounter > 10 && IS_PRESSED(TH_BUTTON_Q))
        {
            g_PauseAction = PauseAction::Exit;
            g_PauseLogicalOpen = false;
            g_PauseFrameCounter = 0;
        }
        else if (g_PauseFrameCounter > 10 && IS_PRESSED(TH_BUTTON_R))
        {
            ScreenEffect::RequestShakeCancelForRestart();
            g_PauseAction = PauseAction::Restart;
            g_PauseLogicalOpen = false;
            g_PauseFrameCounter = 0;
        }
        return true;
    }

    // THPauseMenu keeps the game-menu state alive for ten frames while its
    // window fades out. Preserve that state boundary instead of changing the
    // game state directly from an ImGui draw callback.
    if (g_PauseAction != PauseAction::None)
    {
        if (g_PauseFrameCounter == 1)
        {
            g_PauseVisualState = MenuVisualState::Closing;
            g_PauseSettings = false;
            if (g_PauseAction == PauseAction::Restart)
            {
                // THPauseMenu::StateRestart frame 1: set thRestartFlag,
                // commit THGuiPrac::State(5), and decide everlasting-BGM
                // preservation. The actual restart signal is frame 10.
                CommitRestartWithBgmPolicy();
#if defined(THPRAC_PORTABLE_ENABLED)
                PublishPortableSession();
#endif
#ifdef __EMSCRIPTEN__
                PublishConfigToHost();
#endif
                g_PreserveConfigOnRestart = true;
            }
        }

        if (g_PauseFrameCounter != 10)
            return true;

        switch (g_PauseAction)
        {
        case PauseAction::Resume:
#ifdef TH_DEV_TOOLS
            SDL_Log("TH06 thprac pause action: execute resume at frame %u", g_PauseFrameCounter);
#endif
            FilterUnpauseInput();
            g_GameManager.isInGameMenu = 0;
            break;
        case PauseAction::Exit:
#ifdef TH_DEV_TOOLS
            SDL_Log("TH06 thprac pause action: execute exit at frame %u", g_PauseFrameCounter);
#endif
            g_GameManager.isInGameMenu = 0;
            g_GameManager.guiScore = g_GameManager.score;
            g_ResultReplaySaveRequested = true;
            g_Supervisor.curState = SUPERVISOR_STATE_RESULTSCREEN_FROMGAME;
            break;
        case PauseAction::Restart:
#ifdef TH_DEV_TOOLS
            SDL_Log("TH06 thprac pause action: execute restart at frame %u", g_PauseFrameCounter);
#endif
            ResetTouchReplayForPracticeRestart();
            g_GameManager.isInGameMenu = 0;
            g_Supervisor.curState = SUPERVISOR_STATE_GAMEMANAGER_REINIT;
            break;
        case PauseAction::None:
            break;
        }

        g_PauseAction = PauseAction::None;
        // StateRestart/Exit/Resume counter==10 calls StateClose(), which
        // switches mState to CLOSE and resets mFrameCounter to zero.
        g_PauseLogicalOpen = false;
        g_PauseFrameCounter = 0;
        g_PauseWasOpen = false;
        return true;
    }
    return true;
#else
    if (g_PauseSettings)
    {
        const Config runtimeConfig = g_Config;
        g_Config = g_MenuConfig;
        g_Config.active = false;
        if (WAS_PRESSED_PERIODIC(TH_BUTTON_UP))
            g_MenuCursor = (g_MenuCursor + 15) % 15;
        else if (WAS_PRESSED_PERIODIC(TH_BUTTON_DOWN))
            g_MenuCursor = (g_MenuCursor + 1) % 15;
        else if (WAS_PRESSED_PERIODIC(TH_BUTTON_LEFT))
            AdjustMenuValue(g_MenuCursor, -1);
        else if (WAS_PRESSED_PERIODIC(TH_BUTTON_RIGHT) || WAS_PRESSED(TH_BUTTON_SELECTMENU))
            AdjustMenuValue(g_MenuCursor, 1);
        else if (WAS_PRESSED(TH_BUTTON_RETURNMENU))
            g_PauseSettings = false;
        StoreWorkingMenuConfig();
        g_Config = runtimeConfig;
    }
    else
    {
        if (WAS_PRESSED_PERIODIC(TH_BUTTON_UP))
            g_PauseCursor = (g_PauseCursor + 3) % 4;
        else if (WAS_PRESSED_PERIODIC(TH_BUTTON_DOWN))
            g_PauseCursor = (g_PauseCursor + 1) % 4;
        else if (WAS_PRESSED(TH_BUTTON_RETURNMENU))
            g_PauseCursor = 0;
        else if (WAS_PRESSED(TH_BUTTON_SELECTMENU))
        {
            switch (g_PauseCursor)
            {
            case 0: // Resume
                FilterUnpauseInput();
                g_GameManager.isInGameMenu = 0;
                g_PauseWasOpen = false;
                break;
            case 1: // Exit through Result Screen so the run can be saved.
                g_GameManager.isInGameMenu = 0;
                g_GameManager.guiScore = g_GameManager.score;
                g_ResultReplaySaveRequested = true;
                g_Supervisor.curState = SUPERVISOR_STATE_RESULTSCREEN_FROMGAME;
                g_PauseWasOpen = false;
                break;
            case 2: // Restart with the current advanced-practice parameters.
                ScreenEffect::RequestShakeCancelForRestart();
                CommitRestartWithBgmPolicy();
#if defined(THPRAC_PORTABLE_ENABLED)
                PublishPortableSession();
#endif
                g_GameManager.isInGameMenu = 0;
#ifdef __EMSCRIPTEN__
                PublishConfigToHost();
#endif
                g_PreserveConfigOnRestart = true;
                ResetTouchReplayForPracticeRestart();
                g_Supervisor.curState = SUPERVISOR_STATE_GAMEMANAGER_REINIT;
                g_PauseWasOpen = false;
                break;
            case 3:
                g_PauseSettings = true;
                g_MenuCursor = 0;
                break;
            }
        }
    }

    ZunVec3 title(96.0f, 112.0f, 0.0f);
    g_AsciiManager.color = 0xfff0c080;
    g_AsciiManager.AddString(&title, g_PauseSettings ? "thprac - Settings" : "thprac - Pause Menu");
    if (g_PauseSettings)
    {
        const char *labels[] = {"Mode", "Stage", "Warp", "Section", "Phase", "Dialogue", "Lives", "Bombs",
                                "Power", "Score", "Graze", "Point", "Rank", "Rank lock", "Fake shot"};
        const i32 first = Clamp(g_MenuCursor - 5, 0, 7);
        for (i32 row = 0; row < 8; row++)
        {
            const i32 index = first + row;
            ZunVec3 pos(96.0f, 144.0f + row * 28.0f, 0.0f);
            g_AsciiManager.color = index == g_MenuCursor ? 0xfff0f0c0 : 0xffa0c0c0;
            g_AsciiManager.AddFormatText(&pos, "%c %s", index == g_MenuCursor ? '>' : ' ', labels[index]);
        }
    }
    else
    {
        const char *items[] = {"Resume", "Exit", "Restart", "Settings"};
        for (i32 index = 0; index < 4; index++)
        {
            ZunVec3 pos(128.0f, 168.0f + index * 44.0f, 0.0f);
            g_AsciiManager.color = index == g_PauseCursor ? 0xfff0f0c0 : 0xffa0c0c0;
            g_AsciiManager.AddFormatText(&pos, "%c %s", index == g_PauseCursor ? '>' : ' ', items[index]);
        }
    }
    g_AsciiManager.color = 0xffffffff;
    return true;
#endif
}

void DrawPauseMenuPanel()
{
    if (!Active() || g_Config.mode == 0 || !g_GameManager.isInGameMenu || g_GameManager.isInReplay)
        return;
#ifdef TH_ENABLE_THPRAC
    if (g_PauseVisualState == MenuVisualState::Closed)
        return;
    if (!ThpracImGui::IsFrameOpen())
        return;
    // These dimensions, position, spacing, and style values are copied from
    // TH06 THPauseMenu. The settings page deliberately reuses the same
    // practice controls so it cannot drift into a second, launcher-only UI.
    ImGui::SetNextWindowSize(ImVec2(384.0f, 448.0f), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(32.0f, 16.0f), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(g_PauseAlpha);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    constexpr ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize |
                                               ImGuiWindowFlags_NoCollapse |
                                               ImGuiWindowFlags_NoTitleBar |
                                               ImGuiWindowFlags_NoMove |
                                               ImGuiWindowFlags_NoBringToFrontOnFocus;
    if (ImGui::Begin("Pause Menu###thprac-pause", nullptr, windowFlags))
    {
        ImGui::PushItemWidth(-60.0f);
        // Upstream GameGuiWnd calls THPauseMenu::OnContentUpdate() only once
        // the fade has reached status 2 (fully open). Opening/closing frames
        // therefore contain the fading window but no active menu controls.
        if (g_PauseVisualState == MenuVisualState::Open)
        {
            if (!g_PauseSettings)
                ImGui::Dummy(ImVec2(10.0f, 140.0f));
            else
                ImGui::Dummy(ImVec2(10.0f, 10.0f));

            ImGui::Indent(119.0f);
            if (GuiPauseButton("Resume", ImVec2(130.0f, 25.0f)))
            {
                g_PauseAction = PauseAction::Resume;
                g_PauseLogicalOpen = false;
                g_PauseFrameCounter = 0;
                g_ImGuiPauseFocusPending = false;
                g_SoundPlayer.PlaySoundByIdx(SOUND_SELECT);
#ifdef TH_DEV_TOOLS
                SDL_Log("TH06 thprac pause action: queue resume");
#endif
            }
            if (g_ImGuiPauseFocusPending)
            {
                ImGui::SetItemDefaultFocus();
                g_ImGuiPauseFocusPending = false;
            }
            ImGui::Spacing();
            if (GuiPauseButton("Exit", ImVec2(130.0f, 25.0f)))
            {
                g_PauseAction = PauseAction::Exit;
                g_PauseLogicalOpen = false;
                g_PauseFrameCounter = 0;
                g_ImGuiPauseFocusPending = false;
                g_SoundPlayer.PlaySoundByIdx(SOUND_SELECT);
#ifdef TH_DEV_TOOLS
                SDL_Log("TH06 thprac pause action: queue exit");
#endif
            }
            ImGui::Spacing();
            if (GuiPauseButton("Restart", ImVec2(130.0f, 25.0f)))
            {
                // StateRestart() calls DisableShakeScreenEffect immediately
                // when Restart is selected, before its ten-frame close phase.
                ScreenEffect::RequestShakeCancelForRestart();
                g_PauseAction = PauseAction::Restart;
                g_PauseLogicalOpen = false;
                g_PauseFrameCounter = 0;
                g_ImGuiPauseFocusPending = false;
                g_SoundPlayer.PlaySoundByIdx(SOUND_SELECT);
#ifdef TH_DEV_TOOLS
                SDL_Log("TH06 thprac pause action: queue restart");
#endif
            }
            ImGui::Spacing();
            if (GuiPauseButton("Settings", ImVec2(130.0f, 25.0f)))
            {
                g_PauseSettings = !g_PauseSettings;
                // GuiNavFocus keeps the activated Settings/Tweak item focused
                // across the page switch; do not force the cursor back to
                // Resume/Stage merely because the content below changed.
                g_SoundPlayer.PlaySoundByIdx(SOUND_SELECT);
            }
            ImGui::Spacing();
            if (!g_PauseSettings)
                ImGui::Unindent(119.0f);
            else
            {
                // This is the same net indent used by upstream THPauseMenu before
                // it calls THGuiPrac::PracticeMenu().
                ImGui::Unindent(67.0f);
                const Config runtimeConfig = g_Config;
                g_Config = g_MenuConfig;
                g_Config.active = false;
                DrawPracticeControls(false);
                StoreWorkingMenuConfig();
                g_Config = runtimeConfig;
            }
        }
        ImGui::PopItemWidth();
    }
    ImGui::End();
    ImGui::PopStyleVar(2);
#else
    ZunRect panel {56.0f, 80.0f, 392.0f, 400.0f};
    ScreenEffect::DrawSquare(&panel, 0xff181820);
#endif
}

void PrepareStart(GameManager &gameManager)
{
    if (!Active())
        return;
    // th06_prac_menu_enter enables th06_sfx_fix only for a live Practice
    // entry. THGuiRep does not arm that one-shot during Replay playback.
    if (!gameManager.isInReplay)
        g_BossSectionSfxFixPending = true;
    if (gameManager.isInReplay)
        return;
    gameManager.currentStage = g_Config.stage;
    // The original thprac enter hook clears this flag for Extra and relies on
    // its result-screen hook to keep advanced Practice's Replay path active.
    gameManager.isInPracticeMode = g_Config.stage != 6;
    if (g_Config.stage == 6)
        gameManager.difficulty = EXTRA;
}

i32 ResolveWarpFrame(i32 stage, i32 portion)
{
    static constexpr i32 frames[][9] = {
        {68, 580, 1160, 1540, 2348, 4438, 0, 0, 0},
        {270, 924, 3528, 4563, 0, 0, 0, 0, 0},
        {340, 1050, 1670, 2762, 3807, 4118, 5274, 0, 0},
        {380, 1454, 2328, 0x0d40, 4872, 5712, 7434, 8354, 9784},
        {350, 1352, 2292, 3814, 6774, 0, 0, 0, 0},
        {380, 1484, 0, 0, 0, 0, 0, 0, 0},
        {380, 1300, 2600, 3680, 4803, 5933, 7733, 0, 0}
    };
    if (stage < 0 || stage >= 7 || portion < 1 || portion > 9)
        return 0;
    return frames[stage][portion - 1];
}

void ApplyInitialState(GameManager &gameManager, bool applyStats)
{
    // Upstream mode 0 is an unmodified whole-stage Practice run. Only mode 1
    // applies thprac parameters and ECL/time warps.
    if (!Active() || g_Config.mode != 1)
        return;
    if (applyStats)
    {
        gameManager.livesRemaining = static_cast<i8>(g_Config.life);
        gameManager.bombsRemaining = static_cast<i8>(g_Config.bomb);
        gameManager.currentPower = static_cast<u16>(g_Config.power);
        gameManager.guiScore = gameManager.score = static_cast<u32>(g_Config.score);
        gameManager.grazeInStage = gameManager.grazeInTotal = g_Config.graze;
        gameManager.pointItemsCollectedInStage = gameManager.pointItemsCollected = static_cast<u16>(g_Config.point);
        gameManager.rank = g_Config.rank;
        if (g_Config.rankLock)
            gameManager.minRank = gameManager.maxRank = g_Config.rank;
        if (gameManager.difficulty != EXTRA)
        {
            gameManager.extraLives = g_Config.score >= 60000000 ? 4 : g_Config.score >= 40000000 ? 3 :
                                     g_Config.score >= 20000000 ? 2 : g_Config.score >= 10000000 ? 1 : 0;
        }
    }
    i32 frame = g_Config.frame;
    if (frame == 0 && g_Config.section >= 10000)
    {
        const i32 encoded = g_Config.section - 10000;
        frame = ResolveWarpFrame(encoded / 100 - 1, encoded % 100);
    }
    if (frame > 0)
        g_EnemyManager.timelineTime.SetCurrent(frame);
}

static double JsonNumber(const std::string &json, const char *key, double fallback)
{
    const std::string needle = std::string("\"") + key + "\"";
    size_t position = json.find(needle);
    if (position == std::string::npos || (position = json.find(':', position + needle.size())) == std::string::npos)
        return fallback;
    const char *begin = json.c_str() + position + 1;
    char *end = nullptr;
    const double value = std::strtod(begin, &end);
    return end == begin ? fallback : value;
}

static bool JsonBool(const std::string &json, const char *key, bool fallback)
{
    const std::string needle = std::string("\"") + key + "\"";
    size_t position = json.find(needle);
    if (position == std::string::npos || (position = json.find(':', position + needle.size())) == std::string::npos)
        return fallback;
    position = json.find_first_not_of(" \t\r\n", position + 1);
    if (position == std::string::npos)
        return fallback;
    if (json.compare(position, 4, "true") == 0)
        return true;
    if (json.compare(position, 5, "false") == 0)
        return false;
    return fallback;
}

static bool ParseConfigJson(const std::string &json, Config &config, bool preserveMissing)
{
    const bool portableSchema =
        json.find("\"schema\":\"eagler-touhou/thprac-session/1\"") != std::string::npos;
    // thprac v2.3.0.3 stores THPracParam::GetJson() directly in the replay.
    // It has no portable "schema" field; its discriminator is game="th06"
    // plus the thprac parameter version string.
    const bool upstreamSchema = json.find("\"version\":") != std::string::npos;
    if (!portableSchema && !upstreamSchema)
        return false;
    if (json.find("\"game\":\"th06\"") == std::string::npos)
        return false;

    if (!preserveMissing)
        config = {};
    config.active = true;
    config.mode = static_cast<i32>(JsonNumber(json, "mode", config.mode));
    config.stage = static_cast<i32>(JsonNumber(json, "stage", config.stage));
    // warp is a portable live-session-only field. Upstream TH06 Replay JSON
    // never serializes it, so a real THGuiRep candidate must leave the
    // candidate's prior value untouched just like every other absent field.
    config.warp = static_cast<i32>(JsonNumber(json, "warp", config.warp));
    config.section = static_cast<i32>(JsonNumber(json, "section", config.section));
    config.phase = static_cast<i32>(JsonNumber(json, "phase", config.phase));
    config.frame = static_cast<i32>(JsonNumber(json, "frame", config.frame));
    config.dialogue = JsonBool(json, "dlg", config.dialogue);
    config.score = static_cast<std::int64_t>(JsonNumber(json, "score", config.score));
    config.life = static_cast<i32>(JsonNumber(json, "life", config.life));
    config.bomb = static_cast<i32>(JsonNumber(json, "bomb", config.bomb));
    config.power = static_cast<i32>(JsonNumber(json, "power", config.power));
    config.graze = static_cast<i32>(JsonNumber(json, "graze", config.graze));
    config.point = static_cast<i32>(JsonNumber(json, "point", config.point));
    config.rank = static_cast<i32>(JsonNumber(json, "rank", config.rank));
    config.rankLock = JsonBool(json, "rankLock", config.rankLock);
    config.fakeType = static_cast<i32>(JsonNumber(json, "fakeType", config.fakeType));
    return true;
}

static bool LoadConfigJson(const std::string &json)
{
    Config config;
    if (!ParseConfigJson(json, config, false))
        return false;
    SetConfig(config);
    return true;
}

static constexpr size_t kReplayMetadataMaxSize = 512;

static bool FindReplayMetadataTrailer(const u8 *bytes, size_t size, size_t &payloadOffset, size_t &payloadSize)
{
    size = ReplayExtension::BaseFileSize(bytes, size);
    if (bytes == nullptr || size < sizeof(ReplayHeader) + 8 || std::memcmp(bytes, "T6RP", 4) != 0 ||
        std::memcmp(bytes + size - 4, "PRAC", 4) != 0)
        return false;

    const u32 encodedSize = static_cast<u32>(bytes[size - 8]) |
                            (static_cast<u32>(bytes[size - 7]) << 8) |
                            (static_cast<u32>(bytes[size - 6]) << 16) |
                            (static_cast<u32>(bytes[size - 5]) << 24);
    if (encodedSize == 0 || encodedSize >= kReplayMetadataMaxSize || encodedSize > size - 8 - sizeof(ReplayHeader))
        return false;

    payloadSize = encodedSize;
    payloadOffset = size - 8 - payloadSize;
    return payloadOffset >= sizeof(ReplayHeader);
}

static bool ExtractReplayMetadataJson(const u8 *bytes, size_t size, std::string &json)
{
    size_t payloadOffset = 0;
    size_t payloadSize = 0;
    if (!FindReplayMetadataTrailer(bytes, size, payloadOffset, payloadSize))
        return false;

    size_t jsonSize = 0;
    while (jsonSize < payloadSize && bytes[payloadOffset + jsonSize] != 0)
        ++jsonSize;
    if (jsonSize == 0 || jsonSize == payloadSize)
        return false;
    for (size_t index = jsonSize; index < payloadSize; ++index)
    {
        if (bytes[payloadOffset + index] != 0)
            return false;
    }
    json.assign(reinterpret_cast<const char *>(bytes + payloadOffset), jsonSize);
    return true;
}

static u32 ReplayChecksumForRawBytes(const std::vector<u8> &bytes)
{
    if (bytes.size() <= offsetof(ReplayHeader, rngValue3))
        return 0;

    const u8 key = bytes[offsetof(ReplayHeader, key)];
    u8 rollingKey = key;
    u32 checksum = 0x3f000318u + key;
    for (size_t index = offsetof(ReplayHeader, rngValue3); index < bytes.size(); ++index)
    {
        // ReplaySaveParam in thprac intentionally appends the metadata block
        // unencrypted, then computes the checksum over a temporary decrypted
        // view of the *entire* file. The on-disk trailer therefore remains
        // readable as JSON/PRAC while TH06's checksum still covers it. The
        // upstream C++ source spells the little-endian multi-character
        // constant as 'CARP', whose actual on-disk byte order is "PRAC".
        checksum += static_cast<u8>(bytes[index] - rollingKey);
        rollingKey = static_cast<u8>(rollingKey + 7);
    }
    return checksum;
}

static std::string ReplayMetadataJson()
{
    // Match TH06 THPracParam::GetJson() field presence/order exactly. "warp"
    // is UI-only and never serialized; section/phase/frame/dlg are omitted at
    // their default values rather than emitted as explicit zero/false fields.
    char tail[384];
    std::string json = "{\"version\":\"2.3.0.3\",\"game\":\"th06\",\"mode\":" +
        std::to_string(g_Config.mode) + ",\"stage\":" + std::to_string(g_Config.stage);
    if (g_Config.section != 0)
        json += ",\"section\":" + std::to_string(g_Config.section);
    if (g_Config.phase != 0)
        json += ",\"phase\":" + std::to_string(g_Config.phase);
    if (g_Config.frame != 0)
        json += ",\"frame\":" + std::to_string(g_Config.frame);
    if (g_Config.dialogue)
        json += ",\"dlg\":true";

    const int length = std::snprintf(
        tail, sizeof(tail),
        ",\"score\":%lld,\"life\":%d,\"bomb\":%d,\"power\":%d,\"graze\":%d,\"point\":%d,"
        "\"rank\":%d,\"rankLock\":%s,\"fakeType\":%d}",
        static_cast<long long>(g_Config.score), g_Config.life, g_Config.bomb, g_Config.power,
        g_Config.graze, g_Config.point, g_Config.rank, g_Config.rankLock ? "true" : "false", g_Config.fakeType);
    if (length <= 0 || static_cast<size_t>(length) >= sizeof(tail))
        return {};
    json.append(tail, static_cast<size_t>(length));
    return json;
}

static bool EmbedReplayMetadata(std::vector<u8> &bytes, const std::string &json)
{
    if (bytes.size() < sizeof(ReplayHeader) || std::memcmp(bytes.data(), "T6RP", 4) != 0 || json.empty())
        return false;

    // If this helper is ever called on a replay that already carries thprac
    // metadata, replace the one trailer rather than stacking another block.
    size_t oldPayloadOffset = 0;
    size_t oldPayloadSize = 0;
    if (FindReplayMetadataTrailer(bytes.data(), bytes.size(), oldPayloadOffset, oldPayloadSize))
        bytes.resize(oldPayloadOffset);

    size_t payloadSize = json.size() + 1;
    while ((payloadSize & 3u) != 0)
        ++payloadSize;
    if (payloadSize >= kReplayMetadataMaxSize)
        return false;

    const size_t payloadOffset = bytes.size();
    bytes.resize(bytes.size() + payloadSize + 8, 0);
    std::memcpy(bytes.data() + payloadOffset, json.data(), json.size());
    const size_t sizeOffset = payloadOffset + payloadSize;
    const u32 encodedSize = static_cast<u32>(payloadSize);
    bytes[sizeOffset + 0] = static_cast<u8>(encodedSize);
    bytes[sizeOffset + 1] = static_cast<u8>(encodedSize >> 8);
    bytes[sizeOffset + 2] = static_cast<u8>(encodedSize >> 16);
    bytes[sizeOffset + 3] = static_cast<u8>(encodedSize >> 24);
    std::memcpy(bytes.data() + sizeOffset + 4, "PRAC", 4);

    const u32 checksum = ReplayChecksumForRawBytes(bytes);
    bytes[offsetof(ReplayHeader, checksum) + 0] = static_cast<u8>(checksum);
    bytes[offsetof(ReplayHeader, checksum) + 1] = static_cast<u8>(checksum >> 8);
    bytes[offsetof(ReplayHeader, checksum) + 2] = static_cast<u8>(checksum >> 16);
    bytes[offsetof(ReplayHeader, checksum) + 3] = static_cast<u8>(checksum >> 24);
    return true;
}

#ifdef TH_DEV_TOOLS
bool DebugLoadSessionFile(const char *path)
{
    if (path == nullptr || *path == '\0')
        return false;
    u8 *bytes = FileSystem::OpenPath(path, 1);
    if (bytes == nullptr)
        return false;
    const std::string json(reinterpret_cast<char *>(bytes), g_LastFileSize);
    std::free(bytes);
    return LoadConfigJson(json);
}
#endif

void ReplayMenuReset()
{
    // THGuiRep::State(1): mRepStatus=false, mParamStatus=false,
    // thPracParam.Reset().  mRepParam is a distinct candidate object; reset it
    // here as well so the first State(2) starts from the same zero-initialized
    // object that upstream owns at process startup.
    g_ReplayPlaybackActive = false;
    g_ReplayStartupCommitted = false;
    g_ReplayParamStatus = false;
    // State(1) does NOT reset mRepParam. In TH06 this matters because
    // THPracParam::ReadJson() itself also does not Reset(), so omitted fields
    // on the first inspected Replay of a later menu visit inherit the previous
    // mRepParam value. Preserve the candidate object across State(1).
    g_Config = {};
#ifdef __EMSCRIPTEN__
    EM_ASM({
        Module.eaglerOptions = Module.eaglerOptions || {};
        Module.eaglerOptions.thpracSession = null;
    });
#endif
#if defined(THPRAC_PORTABLE_ENABLED)
    ThpracPortableTh06SetSessionJson(nullptr);
#endif
}

bool ReplayMenuCheck(const char *replayPath)
{
    // THGuiRep::State(2) / CheckReplay().  TH06 has two important quirks that
    // must not be "fixed": ReadJson() does NOT call Reset(), so omitted JSON
    // fields retain the previous mRepParam value; and a failed metadata load
    // Reset()s mRepParam but does NOT clear mParamStatus.  Preserve both.
    if (!replayPath || !*replayPath)
    {
        g_ReplayCandidate = {};
        return false;
    }
    u8 *bytes = FileSystem::OpenPath(replayPath, 1);
    if (bytes == nullptr)
    {
        g_ReplayCandidate = {};
        return false;
    }
    const size_t size = static_cast<size_t>(g_LastFileSize);
    std::string json;
    const bool extracted = ExtractReplayMetadataJson(bytes, size, json);
    std::free(bytes);
    if (extracted)
    {
        Config candidate = g_ReplayCandidate;
        if (ParseConfigJson(json, candidate, true))
        {
            g_ReplayCandidate = candidate;
            g_ReplayParamStatus = true;
            return true;
        }
    }

    // Exact upstream failure branch: mRepParam.Reset(); mParamStatus is left
    // untouched. If a previous candidate succeeded, State(3) will therefore
    // still copy this all-zero candidate.
    g_ReplayCandidate = {};
    return false;
}

void ReplayMenuActivate()
{
    // THGuiRep::State(3): mRepStatus=true for every accepted replay. Only a
    // sticky-true mParamStatus copies mRepParam into live thPracParam.
    g_ReplayPlaybackActive = true;
    g_ReplayStartupCommitted = true;
    if (!g_ReplayParamStatus)
        return;

    g_Config = g_ReplayCandidate;
#if defined(THPRAC_PORTABLE_ENABLED)
    // Publish the fully materialized live thPracParam, not the raw candidate
    // JSON. TH06 ReadJson() preserves omitted fields from the previous
    // mRepParam, so re-parsing the raw JSON in the adapter would split owners.
    if (g_Config.active)
        PublishPortableSession();
    else
        ThpracPortableTh06SetSessionJson(nullptr);
#endif
#ifdef __EMSCRIPTEN__
    if (g_Config.active)
        PublishConfigToHost();
    else
    {
        EM_ASM({
            Module.eaglerOptions = Module.eaglerOptions || {};
            Module.eaglerOptions.thpracSession = null;
        });
    }
#endif
}

bool ReplayPlaybackActive()
{
    return g_ReplayPlaybackActive;
}

bool ReplayStartupCommitted()
{
    return g_ReplayStartupCommitted;
}

void FinishReplayStartup()
{
    g_ReplayStartupCommitted = false;
}

bool LoadReplayMetadata(const char *replayPath)
{
    g_Config = {};
#if !defined(__EMSCRIPTEN__) && defined(THPRAC_PORTABLE_ENABLED)
    ThpracPortableTh06SetSessionJson(nullptr);
#endif
    if (!replayPath || !*replayPath)
        return false;
    u8 *bytes = FileSystem::OpenPath(replayPath, 1);
    std::string json;
    if (bytes != nullptr)
    {
        const size_t replaySize = g_LastFileSize > 0 ? static_cast<size_t>(g_LastFileSize) : 0;
        const bool extracted = ExtractReplayMetadataJson(bytes, replaySize, json);
        std::free(bytes);
        if (!extracted)
            json.clear();
    }

    if (json.empty())
        return false;
    if (!LoadConfigJson(json))
        return false;
#ifdef TH_DEV_TOOLS
    SDL_Log("TH06 thprac replay metadata loaded: path=%s mode=%d stage=%d section=%d frame=%d",
            replayPath, g_Config.mode, g_Config.stage, g_Config.section, g_Config.frame);
#endif
#ifdef __EMSCRIPTEN__
    PublishConfigToHost();
#endif
    // LoadConfigJson() calls SetConfig(), which is the single authoritative
    // conversion boundary from thprac v2.3.0.3's native flat replay JSON to
    // the portable adapter schema. Do not feed the native JSON to the adapter
    // a second time here: DeserializeReplaySession intentionally accepts only
    // portable schemas, so that duplicate write used to fail and clear the
    // already-correct g_Config while leaving the adapter's converted session
    // intact. The result was a dangerous half-loaded replay (ECL warp active,
    // PracticeRuntime GUI/title state inactive).
    return true;
}

bool SaveReplayMetadata(const char *replayPath)
{
    // Upstream th06_save_replay gates THSaveReplay on thPracParam.mode, not
    // merely on being inside a Practice run. Mode=Original must remain a
    // vanilla replay without a PRAC trailer.
    if (!AdvancedActive() || !replayPath || !*replayPath)
        return false;
    const std::string json = ReplayMetadataJson();
    if (json.empty())
        return false;

    u8 *raw = FileSystem::OpenPath(replayPath, 1);
    if (!raw || g_LastFileSize <= 0)
    {
        std::free(raw);
        return false;
    }
    std::vector<u8> bytes(raw, raw + g_LastFileSize);
    std::free(raw);
    if (!EmbedReplayMetadata(bytes, json))
        return false;

    SDL_IOStream *file = FileSystem::OpenFileStream(replayPath, "wb");
    if (!file)
        return false;
    const bool written = SDL_WriteIO(file, bytes.data(), bytes.size()) == bytes.size();
    SDL_CloseIO(file);
#ifdef TH_DEV_TOOLS
    if (written)
        SDL_Log("TH06 thprac replay metadata embedded: path=%s bytes=%zu", replayPath, bytes.size());
#endif
    return written;
}

bool DebugReplayMetadataRoundTrip(const char *replayPath)
{
#if !defined(__EMSCRIPTEN__)
    if (replayPath == nullptr || *replayPath == '\0')
        return false;
    Config expected;
    expected.active = true;
    expected.mode = 1;
    expected.stage = 4;
    // Upstream THPracParam::GetJson() does not serialize the menu-only warp
    // selector; replay identity is carried by section/frame instead.
    expected.warp = 0;
    expected.section = 41;
    expected.phase = 3;
    expected.frame = 12345;
    expected.dialogue = true;
    expected.score = 76543210;
    expected.life = 5;
    expected.bomb = 3;
    expected.power = 96;
    expected.graze = 1200;
    expected.point = 340;
    expected.rank = 47;
    expected.rankLock = true;
    expected.fakeType = 2;
    SetConfig(expected);
    const std::string json = ReplayMetadataJson();
    std::vector<u8> fakeReplay(sizeof(ReplayHeader) + 32, 0);
    std::memcpy(fakeReplay.data(), "T6RP", 4);
    fakeReplay[offsetof(ReplayHeader, key)] = 0x40;
    if (!EmbedReplayMetadata(fakeReplay, json))
        return false;

    SDL_IOStream *file = FileSystem::OpenFileStream(replayPath, "wb");
    if (file == nullptr)
        return false;
    const bool wroteReplay = SDL_WriteIO(file, fakeReplay.data(), fakeReplay.size()) == fakeReplay.size();
    SDL_CloseIO(file);
    if (!wroteReplay)
    {
        std::remove(replayPath);
        return false;
    }

    Config stale;
    stale.active = true;
    stale.stage = 1;
    stale.section = 999;
    SetConfig(stale);
    const bool loaded = LoadReplayMetadata(replayPath);
    std::remove(replayPath);
    if (!loaded)
        return false;
    const Config &actual = GetConfig();
    return actual.active == expected.active && actual.mode == expected.mode && actual.stage == expected.stage &&
        actual.warp == expected.warp && actual.section == expected.section && actual.phase == expected.phase &&
        actual.frame == expected.frame && actual.dialogue == expected.dialogue && actual.score == expected.score &&
        actual.life == expected.life && actual.bomb == expected.bomb && actual.power == expected.power &&
        actual.graze == expected.graze && actual.point == expected.point && actual.rank == expected.rank &&
        actual.rankLock == expected.rankLock && actual.fakeType == expected.fakeType;
#else
    (void)replayPath;
    return false;
#endif
}

bool DebugRestartPreservesConfig()
{
#if !defined(__EMSCRIPTEN__)
    const Config savedConfig = g_Config;
    const Config savedMenuConfig = g_MenuConfig;
    const bool savedPreserveRestart = g_PreserveConfigOnRestart;
    const bool savedPreserveFresh = g_PreserveConfigOnFreshStart;
    const bool savedMenuOpen = g_MenuOpen;
    const u8 savedPracticeMode = g_GameManager.isInPracticeMode;
    const u32 savedReplayMode = g_GameManager.isInReplay;
    const u8 savedInGameMenu = g_GameManager.isInGameMenu;

    // TH06 leaves isInReplay set after returning from Replay until the next
    // non-replay run actually starts. The Practice stage-selection hook must
    // still open THGuiPrac on that very first post-Replay attempt.
    g_GameManager.isInPracticeMode = 1;
    g_GameManager.isInReplay = 1;
    const bool staleReplayDoesNotGatePractice = Enabled();

    // Conversely, while gameplay really is a Replay, the advanced-practice
    // metadata must not steal TH06's vanilla Replay ESC/Pause state machine.
    g_Config = {};
    g_Config.active = true;
    g_Config.mode = 1;
    g_GameManager.isInGameMenu = 1;
    const bool replayPauseOwnedByVanilla = !UpdatePauseMenu();

    Config expected;
    expected.active = true;
    expected.mode = 1;
    expected.stage = 4;
    expected.section = 41;
    expected.life = 3;
    expected.power = 104;
    expected.fakeType = 2;
    SetConfig(expected);
    g_PreserveConfigOnRestart = true;
    RefreshFromHost();
    const Config afterRestart = GetConfig();
    const bool restartPreserved = afterRestart.active && afterRestart.mode == expected.mode &&
        afterRestart.stage == expected.stage && afterRestart.section == expected.section &&
        afterRestart.life == expected.life && afterRestart.power == expected.power &&
        afterRestart.fakeType == expected.fakeType && !g_PreserveConfigOnRestart;

    SetConfig(expected);
    g_PreserveConfigOnFreshStart = true;
    RefreshFromHost();
    const Config afterFreshAccept = GetConfig();
    const bool freshAcceptPreserved = afterFreshAccept.active && afterFreshAccept.stage == expected.stage &&
        afterFreshAccept.section == expected.section && !g_PreserveConfigOnFreshStart;

    SetConfig(expected);
    RefreshFromHost();
    const bool staleRuntimeReset = !g_Config.active;

    Config rememberedMenu;
    rememberedMenu.mode = 1;
    rememberedMenu.stage = 2;
    rememberedMenu.warp = 5;
    rememberedMenu.section = 18;
    rememberedMenu.life = 6;
    g_MenuConfig = rememberedMenu;
    SetConfig(expected); // stand in for unrelated replay/current-run metadata
    OpenPracticeMenu(NORMAL, 0);
    const bool replayDidNotPolluteMenu = !g_Config.active && g_Config.stage == rememberedMenu.stage &&
        g_Config.warp == rememberedMenu.warp && g_Config.life == rememberedMenu.life;
    g_MenuOpen = false;

    // State(5) leaves dlg/fakeType untouched when the newly selected section
    // does not own those fields; State(3) starts from Reset(), so they remain 0.
    Config oldRuntime;
    oldRuntime.active = true;
    oldRuntime.mode = 1;
    oldRuntime.stage = 0;
    oldRuntime.warp = 5;
    oldRuntime.section = 4;
    oldRuntime.dialogue = true;
    oldRuntime.fakeType = 4;
    g_Config = oldRuntime;
    g_MenuConfig = rememberedMenu; // section 18 has neither dialogue nor Stage4 fakeType storage
    CommitMenuConfigToRuntime(true);
    const bool restartConditionalWrites = g_Config.dialogue && g_Config.fakeType == 4;
    g_Config = oldRuntime;
    CommitMenuConfigToRuntime(false);
    const bool freshConditionalWrites = !g_Config.dialogue && g_Config.fakeType == 0;

    // Regression for the real-world "first Practice works, second loses
    // power/section, third becomes ordinary Start" failure.  Run the same
    // persistent THGuiPrac selection through three complete fresh-accept / new
    // run-reset cycles.  Each cycle must publish the same live parameters,
    // consume the one-shot fresh-start preserve exactly once, then end the
    // live run without changing the remembered widget values.
    Config repeatedMenu;
    repeatedMenu.mode = 1;
    repeatedMenu.stage = 0;
    repeatedMenu.warp = 5; // Spell
    repeatedMenu.section = 4;
    repeatedMenu.life = 5;
    repeatedMenu.bomb = 4;
    repeatedMenu.power = 96;
    repeatedMenu.rank = 28;
    g_MenuConfig = repeatedMenu;
    bool repeatedPracticeStable = true;
    for (i32 round = 0; round < 3 && repeatedPracticeStable; round++)
    {
        OpenPracticeMenu(NORMAL, 0);
        repeatedPracticeStable = !g_Config.active && g_Config.mode == repeatedMenu.mode &&
            g_Config.stage == repeatedMenu.stage && g_Config.warp == repeatedMenu.warp &&
            g_Config.power == repeatedMenu.power;
        if (!repeatedPracticeStable)
            break;

        DebugAcceptPracticeMenu();
        repeatedPracticeStable = g_Config.active && g_Config.mode == 1 && g_Config.stage == 0 &&
            g_Config.section == 4 && g_Config.life == 5 && g_Config.bomb == 4 &&
            g_Config.power == 96 && g_PreserveConfigOnFreshStart;
        if (!repeatedPracticeStable)
            break;

        RefreshFromHost(); // GameManager fresh-start boundary consumes preserve.
        repeatedPracticeStable = g_Config.active && g_Config.section == 4 && g_Config.power == 96 &&
            !g_PreserveConfigOnFreshStart;
        if (!repeatedPracticeStable)
            break;

        RefreshFromHost(); // Ordinary later boundary ends this run's live owner.
        repeatedPracticeStable = !g_Config.active && g_MenuConfig.section == 4 &&
            g_MenuConfig.power == 96;
    }

    SetConfig(savedConfig);
    g_MenuConfig = savedMenuConfig;
    g_PreserveConfigOnRestart = savedPreserveRestart;
    g_PreserveConfigOnFreshStart = savedPreserveFresh;
    g_MenuOpen = savedMenuOpen;
    g_GameManager.isInPracticeMode = savedPracticeMode;
    g_GameManager.isInReplay = savedReplayMode;
    g_GameManager.isInGameMenu = savedInGameMenu;
    return staleReplayDoesNotGatePractice && replayPauseOwnedByVanilla && restartPreserved && freshAcceptPreserved &&
        staleRuntimeReset && replayDidNotPolluteMenu && restartConditionalWrites && freshConditionalWrites &&
        repeatedPracticeStable;
#else
    return false;
#endif
}

bool DebugSectionCatalogSelfTest()
{
#if defined(THPRAC_PORTABLE_ENABLED)
    const thprac::portable::generated::SectionLabel *matches[160] = {};

    // Chapter is encoded into g_Config.section, but the menu slider itself is
    // the authoritative value while the user is editing it.  Reproduce the
    // production edit path and ensure CurrentSection(0, false) never restores
    // the previous encoded section over a freshly edited chapter.
    g_Config = {};
    g_Config.mode = 1;
    g_Config.stage = 0;
    g_Config.warp = 1;
    g_Config.section = 10101;
    g_MenuChapter = 1;
    CurrentSection();
    if (g_MenuChapter != 1 || g_Config.section != 10101)
        return false;
    g_MenuChapter = 3;
    CurrentSection(0, false);
    if (g_MenuChapter != 3 || g_Config.section != 10103)
        return false;
    g_Config.section = 10104;
    g_MenuChapter = 2;
    CurrentSection(0, false);
    if (g_MenuChapter != 2 || g_Config.section != 10102)
        return false;

    static constexpr struct
    {
        i32 warp;
        i32 expectedCount;
        i32 firstPatch;
        i32 lastPatch;
    } extraCases[] = {
        {2, 3, 50, 52},
        {3, 18, 53, 70},
        {4, 8, 53, 67},
        {5, 13, 50, 70},
    };
    for (const auto &test : extraCases)
    {
        const i32 count = BuildSectionMatchesFor(6, test.warp, 0, NORMAL, 0, matches, 160);
        if (count != test.expectedCount || matches[0] == nullptr || matches[count - 1] == nullptr ||
            matches[0]->patchId != test.firstPatch || matches[count - 1]->patchId != test.lastPatch)
            return false;
    }

    const i32 normalCount = BuildSectionMatchesFor(1, 5, 0, NORMAL, 0, matches, 160);
    const thprac::portable::generated::SectionLabel *normalNine = nullptr;
    for (i32 index = 0; index < normalCount; ++index)
        if (matches[index]->patchId == 9)
            normalNine = matches[index];
    if (normalNine == nullptr || std::strcmp(normalNine->en, "Ice Sign \"Icicle Fall\"") != 0)
        return false;

    const i32 hardCount = BuildSectionMatchesFor(1, 5, 0, HARD, 0, matches, 160);
    const thprac::portable::generated::SectionLabel *hardNine = nullptr;
    for (i32 index = 0; index < hardCount; ++index)
        if (matches[index]->patchId == 9)
            hardNine = matches[index];
    if (hardCount != normalCount || hardNine == nullptr ||
        std::strcmp(hardNine->en, "Hail Sign \"Hailstorm\"") != 0)
        return false;

    // THGuiPrac keeps mDlg alive while its checkbox is hidden. Exercise one
    // dialogue-capable boss section followed by a normal section and ensure
    // merely changing the section cannot erase the user's checkbox state.
    g_Config = {};
    g_Config.mode = 1;
    g_Config.stage = 0;
    g_Config.warp = 3;
    g_Config.dialogue = true;
    g_MenuDifficulty = NORMAL;
    g_MenuShotType = 0;
    g_MenuSectionIndex = 0;
    const CurrentSectionInfo *dialogueSection = CurrentSection(0, false);
    if (dialogueSection == nullptr || !dialogueSection->dialogue || !g_Config.dialogue)
        return false;
    g_MenuSectionIndex = 1;
    const CurrentSectionInfo *nonDialogueSection = CurrentSection(0, false);
    if (nonDialogueSection == nullptr || nonDialogueSection->dialogue || !g_Config.dialogue)
        return false;

    // Warp changes reset phase/frame in upstream PracticeMenu.  This matters
    // because the portable runtime consumes frame directly during startup.
    g_Config.warp = 5;
    g_Config.phase = 7;
    g_Config.frame = 12345;
    ResetWarpDependentMenuState();
    if (g_Config.phase != 0 || g_Config.frame != 0 || g_MenuSectionIndex != 0 || g_MenuChapter != 1)
        return false;

    // Stage 4 shot variants use parallel section tables. Original mFakeShot
    // changes the table but deliberately preserves the current mSection index.
    g_Config.stage = 3;
    g_Config.warp = 3;
    g_Config.fakeType = 1;
    g_MenuSectionIndex = 1;
    if (CurrentSection(0, false) == nullptr || g_MenuSectionIndex != 1)
        return false;
    g_Config.fakeType = 2;
    if (CurrentSection(0, false) == nullptr || g_MenuSectionIndex != 1)
        return false;

    // Deterministically exercise the actual production ImGui menu instead of
    // depending on the vanilla title/practice state machine reaching it in a
    // bounded offscreen run. Only the outer vanilla menu wait is bypassed;
    // OpenPracticeMenu(), DrawPracticeMenu(), CurrentSection() and GuiCombo()
    // are the same code used by a player.
    g_Config = {};
    OpenPracticeMenu(NORMAL, 0);
    g_Config.mode = 1;
    g_Config.stage = 6;
    g_Config.warp = 5;
    g_Config.section = 70;
    if (!ThpracImGui::Initialize())
        return false;
    bool rendered = false;
    for (i32 frame = 0; frame < 10; ++frame)
    {
        ThpracImGui::BeginFrame(1.0f / 60.0f);
        DrawPracticeMenu();
        ThpracImGui::EndFrame();
        const ImDrawData *drawData = ThpracImGui::GetDrawData();
        rendered = rendered || (drawData != nullptr && drawData->CmdListsCount > 0 &&
                                drawData->TotalVtxCount > 0 && drawData->TotalIdxCount > 0);
    }
    const bool selectedExtraSpell = g_Config.section >= 50 && g_Config.section <= 70;
    ThpracImGui::Shutdown();
    g_MenuOpen = false;
    if (!rendered || !selectedExtraSpell)
        return false;

#ifdef TH_ENABLE_THPRAC
    // GuiButton in upstream accepts Z/Return separately from ImGui's own
    // NavActivate path.  Exercise that exact portable helper with a focused
    // button so the Practice-menu ownership fix cannot silently break Pause.
    if (!ThpracImGui::Initialize())
        return false;
    ThpracImGui::SetGameInput(0, true);
    ThpracImGui::BeginFrame(1.0f / 60.0f);
    ImGui::Begin("PauseConfirmSelfTest");
    ImGui::Button("Resume", ImVec2(130.0f, 25.0f));
    ImGui::SetItemFocusAlt("Resume", true);
    ImGui::End();
    ThpracImGui::EndFrame();

    ThpracImGui::SetGameInput(TH_BUTTON_SELECTMENU, true);
    ThpracImGui::BeginFrame(1.0f / 60.0f);
    ImGui::Begin("PauseConfirmSelfTest");
    const bool pauseConfirm = GuiPauseButton("Resume", ImVec2(130.0f, 25.0f));
    ImGui::End();
    ThpracImGui::EndFrame();
    ThpracImGui::SetGameInput(0, true);
    ThpracImGui::Shutdown();
    if (!pauseConfirm)
        return false;
#endif

    // Mode=Original must not replace TH06's vanilla Pause state machine, but
    // upstream thprac still handles R (and Ctrl+Shift+Down) as an immediate
    // current-stage restart. Exercise UpdatePauseMenu() directly so this
    // contract has deterministic runtime coverage independent of window focus.
    const Config savedConfig = g_Config;
    const u8 savedInGameMenu = g_GameManager.isInGameMenu;
    const u32 savedInReplay = g_GameManager.isInReplay;
    const auto savedSupervisorState = g_Supervisor.curState;
    const u16 savedCurrentInput = g_CurFrameInput;
    const u16 savedLastInput = g_LastFrameInput;

    g_Config = {};
    g_Config.active = true;
    g_Config.mode = 0;
    g_GameManager.isInGameMenu = 1;
    g_GameManager.isInReplay = 0;
    g_CurFrameInput = 0;
    g_LastFrameInput = 0;
    const bool vanillaPauseOwned = !UpdatePauseMenu() && g_Config.active && g_GameManager.isInGameMenu != 0;

    g_CurFrameInput = TH_BUTTON_R;
    const bool quickRestartHandled = UpdatePauseMenu() && !g_Config.active && g_GameManager.isInGameMenu == 0 &&
        g_Supervisor.curState == SUPERVISOR_STATE_GAMEMANAGER_REINIT;

    g_Config = savedConfig;
    g_GameManager.isInGameMenu = savedInGameMenu;
    g_GameManager.isInReplay = savedInReplay;
    g_Supervisor.curState = savedSupervisorState;
    g_CurFrameInput = savedCurrentInput;
    g_LastFrameInput = savedLastInput;
    if (!vanillaPauseOwned || !quickRestartHandled)
        return false;
#endif
    return true;
}
} // namespace PracticeRuntime

#ifdef __EMSCRIPTEN__
extern "C" EMSCRIPTEN_KEEPALIVE int EaglerThpracSaveReplaySlot(i32 slot)
{
    if (!PracticeRuntime::Active() || g_GameManager.isInReplay || !g_ReplayManager || slot < 1 || slot > 99)
        return 0;
    char path[40];
    std::snprintf(path, sizeof(path), "./replay/th6_%02d.rpy", slot);
    char name[] = "THPRAC";
    ReplayManager::SaveReplay(path, name);
    const std::string savedPath = ReplayExtension::ResolveSavePath(path);
    SDL_IOStream *file = FileSystem::OpenFileStream(savedPath.c_str(), "rb");
    if (!file)
        return 0;
    SDL_CloseIO(file);
    return 1;
}
#endif
