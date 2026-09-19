#include "PracticeRuntime.hpp"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#include <SDL3/SDL_iostream.h>

#include "AnmManager.hpp"
#include "AsciiManager.hpp"
#include "Controller.hpp"
#include "EnemyManager.hpp"
#include "FileSystem.hpp"
#include "GameManager.hpp"
#include "GameWindow.hpp"
#include "Gui.hpp"
#include "ReplayExtension.hpp"
#include "Player.hpp"
#include "ReplayManager.hpp"
#include "ScreenEffect.hpp"
#include "SoundPlayer.hpp"
#include "Supervisor.hpp"
#include "utils.hpp"
#ifdef TH_ENABLE_THPRAC
#include <SDL3/SDL.h>
#include "ThpracImGui.hpp"
#endif
#if defined(THPRAC_PORTABLE_ENABLED)
#include "section_catalog.hpp"
extern "C" std::int32_t ThpracPortableTh07FakeShot();
extern "C" bool ThpracPortableTh07SetSessionJson(const char *json);
namespace THPrac::Gui
{
void ShowLicenceInfo();
}
#else
namespace thprac::portable::generated
{
struct SectionLabel;
}
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

namespace PracticeRuntime
{
static Config g_Config;
static Config g_MenuConfig = [] {
    Config config;
    config.mode = 1;
    // THGuiPrac constructor defaults. Config{} itself must stay equivalent to
    // THPracParam::Reset()/memset zero so live/replay resets cannot silently
    // turn into menu defaults.
    config.life = 8;
    config.bomb = 8;
    config.power = 128;
    config.cherryMax = 200000;
    config.rank = 16;
    return config;
}();
static MenuResult g_MenuResult = MenuResult::Waiting;
static bool g_MenuOpen = false;
static i32 g_MenuCursor = 0;
static i32 g_MenuDifficulty = 0;
static i32 g_MenuSectionIndex = 0;
static i32 g_MenuChapter = 1;
static constexpr i32 kMenuItemCount = 20;
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
static bool g_ImGuiMenuFocusPending = false;
static const char *g_ImGuiMenuFocusLabel = "Stage";
static bool g_ImGuiMenuFocusRemembered = false;
// THGuiRep keeps a candidate replay parameter block separate from live
// thPracParam. State(2) only inspects the highlighted replay; State(3) marks
// replay ownership active and copies the candidate if it was valid.
static Config g_ReplayCandidate;
static bool g_ReplayCandidateValid = false;
static bool g_ReplayPlaybackActive = false;
// Portable-only one-shot handoff from THGuiRep::State(3) to the immediately
// following Replay GameManager startup.  Upstream mRepStatus above is sticky
// until State(1); this bridge is deliberately not.
static bool g_ReplayStartupCommitted = false;
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
// THOverlay::OnPreUpdate runs after THGuiPrac/THGuiRep in the same ImGui frame
// and suppresses Backspace while any earlier item is active. Capture only the
// 60 Hz chord edge at th07_update; consume it later in DrawOverlay.
static bool g_ModMenuToggleRequested = false;
static bool g_AdvancedMenuToggleRequested = false;
struct AdvancedOptionsState
{
    bool menuOpen = false;
    bool allClearBonus = false;
    bool fixSpellBonusDisplay = false;
    bool showLicense = false;
};
static AdvancedOptionsState g_AdvancedOptions;
static bool g_OverlayKeyDown[13] = {};
#endif
static i32 g_TrackerBorderBreaks = 0;
static i32 g_EverlastingCurrentSong = -1;
static bool g_SupervisorFadeOutScope = false;
static bool g_ResurrectionButterflySpawnSkipPending = false;
template <typename T> static T Clamp(T value, T minimum, T maximum)
{
    return std::max(minimum, std::min(maximum, value));
}

static void StoreWorkingMenuConfig()
{
    g_MenuConfig = g_Config;
    g_MenuConfig.active = false;
}

static int FormatPortableSessionJson(char *json, std::size_t size)
{
    return std::snprintf(json, size,
        "{\"schema\":\"eagler-touhou/thprac-session/1\",\"game\":\"th07\",\"params\":{"
        "\"mode\":%d,\"stage\":%d,\"warp\":%d,\"section\":%d,\"phase\":%d,\"frame\":%d,"
        "\"dlg\":%s,\"score\":%lld,\"life\":%d,\"bomb\":%d,\"power\":%d,\"graze\":%d,"
        "\"point\":%d,\"point_total\":%d,\"point_stage\":%d,\"cherry\":%d,\"cherryMax\":%d,"
        "\"cherryPlus\":%d,\"spellBonus\":%d,\"rank\":%d,\"rankLock\":%s}}",
        g_Config.mode, g_Config.stage, g_Config.warp, g_Config.section, g_Config.phase, g_Config.frame,
        g_Config.dialogue ? "true" : "false", static_cast<long long>(g_Config.score), g_Config.life,
        g_Config.bomb, g_Config.power, g_Config.graze, g_Config.point, g_Config.pointTotal,
        g_Config.pointStage, g_Config.cherry, g_Config.cherryMax, g_Config.cherryPlus,
        g_Config.spellBonus, g_Config.rank, g_Config.rankLock ? "true" : "false");
}

#if defined(THPRAC_PORTABLE_ENABLED)
static bool PublishPortableSession()
{
    char json[2304];
    const int length = FormatPortableSessionJson(json, sizeof(json));
    if (length <= 0 || static_cast<std::size_t>(length) >= sizeof(json))
        return false;
    return ThpracPortableTh07SetSessionJson(json);
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
    char json[2304];
    const int length = FormatPortableSessionJson(json, sizeof(json));
    if (length <= 0 || static_cast<std::size_t>(length) >= sizeof(json))
        return;
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
    g_Config.stage = Clamp(g_Config.stage, 0, 7);
    g_Config.warp = Clamp(g_Config.warp, 0, 8);
    g_Config.section = Clamp(g_Config.section, 0, 19999);
    g_Config.phase = Clamp(g_Config.phase, 0, 64);
    g_Config.frame = std::max(0, g_Config.frame);
    g_Config.score = Clamp<i64>(g_Config.score, 0, 9999999990LL);
    g_Config.life = Clamp(g_Config.life, 0, 8);
    g_Config.bomb = Clamp(g_Config.bomb, 0, 8);
    g_Config.power = Clamp(g_Config.power, 0, 128);
    g_Config.graze = Clamp(g_Config.graze, 0, 99999);
    g_Config.point = Clamp(g_Config.point, 0, 9999);
    g_Config.pointTotal = Clamp(g_Config.pointTotal, 0, 9999);
    g_Config.pointStage = Clamp(g_Config.pointStage, 0, 9999);
    g_Config.cherry = Clamp(g_Config.cherry, 0, 9999990);
    g_Config.cherryMax = Clamp(g_Config.cherryMax, 0, 9999990);
    g_Config.cherryPlus = Clamp(g_Config.cherryPlus, 0, 50000);
    g_Config.spellBonus = Clamp(g_Config.spellBonus, 0, 30);
    // THPracParam::Reset()/ReadJson() use rank==0 as a real sentinel: the
    // th07_patch_main hook does not overwrite the game's current rank in that
    // case.  Keep zero representable for Replay/compatibility paths.  The GUI
    // widget itself enforces its normal difficulty-specific lower bound.
    g_Config.rank = Clamp(g_Config.rank, 0, 99);
}

void RefreshFromHost()
{
#ifdef __EMSCRIPTEN__
    // thPracParam is live-run state, not a launcher preference.  A previous
    // Practice session may remain in Module while the player returns to title;
    // ordinary Start must not resurrect that session and warp Story mode to
    // the old trainer section.  Keep g_MenuConfig untouched so THGuiPrac still
    // remembers its widget values the next time Practice is deliberately
    // opened. Replay owns its metadata separately and is loaded immediately
    // after this boundary by GameManager::AddedCallback.
    if (!g_GameManager.practice)
    {
        // THGuiRep::State(3) has already accepted this Replay and, for a PRAC
        // replay, copied mRepParam into live thPracParam. Replay startup must
        // preserve both the ownership flag and that parameter payload.
        if (g_ReplayStartupCommitted && g_GameManager.replay)
            return;

        g_Config = {};
        // Do NOT clear g_ReplayPlaybackActive here. Upstream THGuiRep::mRepStatus
        // is sticky and is cleared only by THGuiRep::State(1) when the Replay
        // menu is entered again. Ordinary Start / Practice entry reset the live
        // thPracParam but not that separate Replay-owner flag.
        EM_ASM({
            Module.eaglerOptions = Module.eaglerOptions || {};
            Module.eaglerOptions.thpracSession = null;
        });
#if defined(THPRAC_PORTABLE_ENABLED)
        ThpracPortableTh07SetSessionJson(nullptr);
#endif
        return;
    }

    const bool active = EM_ASM_INT({
        const session = Module.eaglerOptions?.thpracSession;
        return !!session && session.game === 'th07' &&
            session.schema === 'eagler-touhou/thprac-session/1';
    }) != 0;
    if (!active)
    {
        g_Config = {};
#if defined(THPRAC_PORTABLE_ENABLED)
        // Host/session absence is itself an ownership boundary.  Do not leave
        // the previous adapter Session/PatchContext alive until a later ECL
        // callback notices that the host is empty.
        ThpracPortableTh07SetSessionJson(nullptr);
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
    config.score = static_cast<i64>(HostNumber("score", 0));
    config.life = static_cast<i32>(HostNumber("life", 0));
    config.bomb = static_cast<i32>(HostNumber("bomb", 0));
    config.power = static_cast<i32>(HostNumber("power", 0));
    config.graze = static_cast<i32>(HostNumber("graze", 0));
    config.point = static_cast<i32>(HostNumber("point", 0));
    config.pointTotal = static_cast<i32>(HostNumber("point_total", 0));
    config.pointStage = static_cast<i32>(HostNumber("point_stage", 0));
    config.cherry = static_cast<i32>(HostNumber("cherry", 0));
    config.cherryMax = static_cast<i32>(HostNumber("cherryMax", 0));
    config.cherryPlus = static_cast<i32>(HostNumber("cherryPlus", 0));
    config.spellBonus = static_cast<i32>(HostNumber("spellBonus", 0));
    config.rank = static_cast<i32>(HostNumber("rank", 0));
    config.rankLock = HostBool("rankLock", false);
    SetConfig(config);
#else
    // State(3)'s THGuiRep replay ownership is a distinct sticky flag. Upstream
    // clears it only in THGuiRep::State(1), not at arbitrary GameManager starts.
    if (g_ReplayStartupCommitted && g_GameManager.replay)
        return;
    // Once THGuiPrac::State(3) has committed live parameters, they remain the
    // current thPracParam for the lifetime of this Practice run, including a
    // GameManager reinitialization/restart. State(1) explicitly clears them
    // before a new Practice selection flow.
    if (g_GameManager.practice && g_Config.active)
        return;
    g_Config = {};
#if defined(THPRAC_PORTABLE_ENABLED)
    ThpracPortableTh07SetSessionJson(nullptr);
#endif
    // TH07's live thPracParam is not a launcher/session-file setting. It is
    // reset when the real Practice UI is entered (THGuiPrac::State(1)) and
    // replay parameters are loaded only by THGuiRep. Ordinary desktop starts
    // therefore must not resurrect an old thprac-session.json payload.
#endif
}

const Config &GetConfig()
{
    return g_Config;
}

bool Active()
{
    return g_Config.active;
}

bool AdvancedActive()
{
    return g_Config.active && g_Config.mode == 1;
}

bool AdvancedSectionActive()
{
    return AdvancedActive() && g_Config.section != 0;
}

bool SuppressStageIntroTitles()
{
    // th07_disable_title: only custom section/frame starts skip the original
    // stage/title intro. Whole-stage advanced practice remains untouched.
    return AdvancedActive() && (g_Config.section != 0 || g_Config.frame != 0);
}

i32 EffectivePlayerShot(i32 vanillaShot)
{
#if defined(THPRAC_PORTABLE_ENABLED)
    if (AdvancedActive())
    {
        const i32 fakeShot = ThpracPortableTh07FakeShot();
        if (fakeShot >= 0)
            return fakeShot;
    }
#endif
    return vanillaShot;
}

i32 InitialBgmIndex()
{
    if (!AdvancedSectionActive() || g_Config.dialogue || g_Config.section >= 10000)
        return 0;
#if defined(THPRAC_PORTABLE_ENABLED)
    for (const auto &entry : thprac::portable::generated::th07SectionLabels)
    {
        if (entry.patchId == g_Config.section)
        {
            // Stage 6 Boss 10 is the unique Resurrection Butterfly start and
            // upstream th07_bgm selects preloaded slot 2 (th07_13b.mid).
            if (g_Config.stage == 5 && g_Config.section == 60)
                return 2;
            return entry.bgm ? 1 : 0;
        }
    }
#endif
    return 0;
}

void FilterUnpauseInput()
{
    // th07_unpause_prevent_desync: THGuiRep::mRepStatus is true for every
    // accepted replay, not only for PRAC-tagged replays. Do not substitute
    // GameManager::replay for this owner: State(1)/State(3) define a separate
    // lifetime in upstream THGuiRep.
    if (g_ReplayPlaybackActive)
        return;
    if (g_Gui.IsDialogueSkippable() == 1)
    {
        g_CurFrameRawInput &= ~TH_BUTTON_SHOOT;
        g_CurFrameGameInput &= ~TH_BUTTON_SHOOT;
    }
}

bool Enabled()
{
#ifdef __EMSCRIPTEN__
    return EM_ASM_INT({ return !!Module.eaglerOptions?.thpracEnabled; }) != 0;
#else
    // Upstream th07_prac_menu_1 opens THGuiPrac unconditionally when the real
    // Practice-stage hook is reached. Replay ownership belongs to THGuiRep;
    // a stale replay flag after returning to title must not suppress the next
    // Practice entry (same lifecycle bug already found/fixed in TH06).
    return g_GameManager.practice != 0;
#endif
}

void OpenPracticeMenu(i32 difficulty, i32 shotType)
{
    // THGuiPrac::State(1) calls THReset(), but it does NOT modify THGuiRep's
    // mRepStatus/mParamStatus/mRepParam members. Keep Replay ownership and the
    // candidate object separate instead of "cleaning them up" on Practice
    // entry; Replay State(1) is the only upstream reset boundary for them.
#ifdef __EMSCRIPTEN__
    // THGuiPrac::State(1) calls THReset(): live thPracParam is cleared, while
    // the window's widget members keep their own persistent values.  The host
    // session mirrors live run state, so it must not be copied back into the
    // persistent menu model on a fresh Practice entry.
    EM_ASM({
        Module.eaglerOptions = Module.eaglerOptions || {};
        Module.eaglerOptions.thpracSession = null;
    });
#endif
#if defined(THPRAC_PORTABLE_ENABLED)
    // THReset() also sets thFakeShot=-1 upstream. Clearing the adapter session
    // here owns the portable equivalent of that live patch-context reset.
    ThpracPortableTh07SetSessionJson(nullptr);
#endif
    // THGuiPrac::State(1) calls THReset() on the live thPracParam while its
    // widget members keep their previous values. Edit that persistent widget
    // model and leave the live runtime inactive until State(3)/accept.
    g_Config = g_MenuConfig;
    g_Config.active = false;

    const i32 oldDifficulty = g_MenuDifficulty;
    g_MenuDifficulty = Clamp(difficulty, 0, 5);
    // Exact State(1): oldRank is the previous persistent difficulty. When the
    // difficulty crosses Easy <-> Normal+, SetBound changes and mRank is reset
    // to 20/32 respectively. Same-band changes preserve the existing value.
    if (g_MenuDifficulty != oldDifficulty)
    {
        if (oldDifficulty == 0 && g_MenuDifficulty > 0)
            g_Config.rank = 32;
        else if (oldDifficulty > 0 && g_MenuDifficulty == 0)
            g_Config.rank = 20;
    }
    const i32 rankMin = g_MenuDifficulty == 0 ? 12 : 10;
    const i32 rankMax = g_Config.rankLock ? 99 : (g_MenuDifficulty == 0 ? 20 : 32);
    g_Config.rank = Clamp(g_Config.rank, rankMin, rankMax);
    StoreWorkingMenuConfig();

    (void)shotType; // TH07 THGuiPrac has no shot-type state/widget owner.
    // mStage/mWarp/mSection/mChapter are persistent THGuiPrac widget members.
    // Do not reset their cursor state merely because State(1) reopened the
    // window; CurrentSection() re-synchronizes from g_MenuConfig on first use.
    g_MenuCursor = 0;
    g_MenuOpen = true;
    g_MenuResult = MenuResult::Waiting;
    g_MenuPendingResult = MenuResult::Waiting;
    g_MenuVisualState = MenuVisualState::Opening;
    g_MenuAlpha = 0.0f;
    g_MenuCloseStep = 0.1f;
    g_ImGuiMenuFocusPending = true;
}

static i32 ChapterLimit(i32 stage)
{
    // THGuiPrac::mChapterSetup in thprac v2.3.0.3.
    static constexpr i32 chapterCounts[8][2] = {
        {2, 1}, {1, 1}, {2, 1}, {4, 4}, {3, 1}, {2, 0}, {5, 3}, {5, 3},
    };
    stage = Clamp(stage, 0, 7);
    return chapterCounts[stage][0] + chapterCounts[stage][1];
}

#if defined(THPRAC_PORTABLE_ENABLED)
static i32 BuildSectionMatchesFor(i32 stage, i32 warp, i32 menuDifficulty,
                                  const thprac::portable::generated::SectionLabel **matches, i32 capacity)
{
    using namespace thprac::portable::generated;
    if (warp < 2 || warp > 5)
        return 0;

    const std::uint32_t difficultyBit = 1u << Clamp(menuDifficulty, 0, 5);
    i32 count = 0;
    const SectionLabel *candidate = nullptr;
    i32 candidatePatchId = -1;
    auto flushCandidate = [&]() {
        if (candidate != nullptr && count < capacity)
            matches[count++] = candidate;
        candidate = nullptr;
        candidatePatchId = -1;
    };

    for (const SectionLabel &entry : th07SectionLabels)
    {
        if (entry.stage != stage)
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
            // cba/cbt decides whether the section exists. Difficulty only
            // chooses the wording for duplicate IDs; if no row matches the
            // selected difficulty, keep the first row so the section ID remains available.
            candidate = &entry;
        }
    }
    flushCandidate();
    return count;
}

static i32 BuildSectionMatches(const thprac::portable::generated::SectionLabel **matches, i32 capacity)
{
    return BuildSectionMatchesFor(g_Config.stage, g_Config.warp, g_MenuDifficulty, matches, capacity);
}
#endif

static const thprac::portable::generated::SectionLabel *CurrentSection(i32 offset = 0, bool syncFromConfig = true)
{
    if (g_Config.warp == 0)
    {
        g_Config.section = 0;
        return nullptr;
    }
    if (g_Config.warp == 1)
    {
        const i32 limit = ChapterLimit(g_Config.stage);
        if (offset != 0)
            g_MenuChapter = Clamp(g_MenuChapter + offset, 1, limit);
        else if (syncFromConfig && g_Config.section >= 10000 && g_Config.section / 100 - 101 == g_Config.stage)
            g_MenuChapter = Clamp(g_Config.section % 100, 1, limit);
        g_Config.section = 10000 + (g_Config.stage + 1) * 100 + g_MenuChapter;
        return nullptr;
    }
    if (g_Config.warp == 6)
    {
        g_Config.section = 0;
        return nullptr;
    }

#if defined(THPRAC_PORTABLE_ENABLED)
    const thprac::portable::generated::SectionLabel *matches[180] = {};
    const i32 count = BuildSectionMatches(matches, 180);
    if (count == 0)
        return nullptr;
    if (offset == 0 && syncFromConfig)
    {
        for (i32 index = 0; index < count; ++index)
        {
            if (matches[index]->patchId == g_Config.section)
            {
                g_MenuSectionIndex = index;
                break;
            }
        }
    }
    g_MenuSectionIndex = (g_MenuSectionIndex + offset + count) % count;
    const auto *selected = matches[g_MenuSectionIndex];
    g_Config.section = selected->patchId;
    return selected;
#else
    (void)offset;
    (void)syncFromConfig;
    return nullptr;
#endif
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

static i32 SectionPhaseCount(i32 section)
{
    // Upstream SpellPhase(): Extra TH07_ST7_END_S10 uses
    // TH_SPELL_PHASE_RAGEFUL = Normal/Full/Rage; Phantasm
    // TH07_ST8_END_S10 uses TH_SPELL_PHASE1 = Normal/Rage.
    if (section == 81)
        return 3;
    if (section == 102)
        return 2;
    return 0;
}

static void ClampMenuRank()
{
    const i32 minimum = g_MenuDifficulty == 0 ? 12 : 10;
    const i32 maximum = g_Config.rankLock ? 99 : (g_MenuDifficulty == 0 ? 20 : 32);
    g_Config.rank = Clamp(g_Config.rank, minimum, maximum);
}

static void ResetWarpDependentMenuState()
{
    g_MenuSectionIndex = 0;
    g_MenuChapter = 1;
    g_Config.phase = 0;
    g_Config.frame = 0;
    CurrentSection(0, false);
}

static void CommitMenuConfigToRuntime()
{
    g_Config = g_MenuConfig;
    g_Config.active = false;
    const auto *section = CurrentSection();
    StoreWorkingMenuConfig();

    g_Config.active = true;
    // THGuiPrac::State(3) never writes THPracParam::warp. State(1)'s THReset
    // leaves the live field at zero; Warp remains a persistent GUI selector.
    g_Config.warp = 0;
    // State(1) THReset() happened before State(3), and State(3) only writes
    // dlg when SectionHasDlg() and phase when SpellPhase() exposes a selector.
#if defined(THPRAC_PORTABLE_ENABLED)
    if (!(section && section->dialogue))
        g_Config.dialogue = false;
#else
    g_Config.dialogue = false;
#endif
    if (SectionPhaseCount(g_Config.section) == 0)
        g_Config.phase = 0;
    g_Config.point = 0;
}

static void AdjustMenuValue(i32 item, i32 direction)
{
    switch (item)
    {
    case 0: g_Config.mode = Clamp(g_Config.mode + direction, 0, 1); break;
    case 1:
        g_Config.stage = Clamp(g_Config.stage + direction, 0, 7);
        g_MenuSectionIndex = 0;
        g_MenuChapter = 1;
        CurrentSection(0, false);
        break;
    case 2:
        g_Config.warp = Clamp(g_Config.warp + direction, 0, 6);
        ResetWarpDependentMenuState();
        break;
    case 3:
        CurrentSection(direction);
        g_Config.phase = 0;
        break;
    case 4: g_Config.phase = Clamp(g_Config.phase + direction, 0, 64); break;
    case 5: g_Config.dialogue = !g_Config.dialogue; break;
    case 6: g_Config.life = Clamp(g_Config.life + direction, 0, 8); break;
    case 7: g_Config.bomb = Clamp(g_Config.bomb + direction, 0, 8); break;
    case 8: g_Config.power = Clamp(g_Config.power + direction * 8, 0, 128); break;
    case 9: g_Config.score = Clamp<i64>(g_Config.score + direction * 1000000LL, 0, 9999999990LL); break;
    case 10: g_Config.graze = Clamp(g_Config.graze + direction * 100, 0, 99999); break;
    case 11: g_Config.pointTotal = Clamp(g_Config.pointTotal + direction * 10, 0, 9999); break;
    case 12: g_Config.pointStage = Clamp(g_Config.pointStage + direction * 10, 0, 9999); break;
    case 13: g_Config.cherry = Clamp(g_Config.cherry + direction * 10000, 0, 9999990); break;
    case 14: g_Config.cherryMax = Clamp(g_Config.cherryMax + direction * 10000, 0, 9999990); break;
    case 15: g_Config.cherryPlus = Clamp(g_Config.cherryPlus + direction * 1000, 0, 50000); break;
    case 16: g_Config.spellBonus = Clamp(g_Config.spellBonus + direction, 0, 30); break;
    case 17: g_Config.rank = Clamp(g_Config.rank + direction, 10, 99); break;
    case 18: g_Config.rankLock = !g_Config.rankLock; break;
    }
    CurrentSection();
    ClampMenuRank();
    StoreWorkingMenuConfig();
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

template <typename T> struct GuiStepState
{
    T value;
    T minimum;
    T maximum;
    T multiplier;
};

template <typename T> static void CycleGuiStep(GuiStepState<T> &step)
{
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
    if (label && (ImGui::IsItemFocusedAlt(label) || ImGui::IsItemFocused()))
    {
        g_ImGuiMenuFocusLabel = label;
        g_ImGuiMenuFocusRemembered = true;
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
    *value = Clamp(*value, minimum, maximum);
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
    static GuiStepState<i64> step {10, 10, 100000000, 10};
    const i64 minimum = 0;
    const i64 maximum = 9999999990LL;
    const i64 old = g_Config.score;
    const char *label = ThpracImGui::Text(ThpracImGui::TextId::Score);
    bool changed = ImGui::DragScalar(label, ImGuiDataType_S64, &g_Config.score,
                                     static_cast<float>(step.value * 2), &minimum, &maximum, "%lld");
    g_Config.score = Clamp<i64>(g_Config.score, minimum, maximum);
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
    g_Config.score -= g_Config.score % 10;
    changed |= old != g_Config.score;
    RememberGuiFocus(label);
    return changed;
}

static bool GuiCheckBox(const char *label, bool *value)
{
    bool changed = ImGui::Checkbox(label, value);
    if (ImGui::IsItemFocused() && (GuiInputLeft() || GuiInputRight()))
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

static i32 ChapterFirstHalf(i32 stage)
{
    static constexpr i32 firstHalf[8] = {2, 1, 2, 4, 3, 2, 5, 5};
    return firstHalf[Clamp(stage, 0, 7)];
}

static void FinishPracticeNav()
{
    const bool focusPending = g_ImGuiMenuFocusPending;
    if (focusPending)
    {
        if (!g_ImGuiMenuFocusRemembered)
            g_ImGuiMenuFocusLabel = ThpracImGui::Text(ThpracImGui::TextId::Stage);
        g_ImGuiMenuFocusPending = false;
    }
    if (!ImGui::IsPopupOpen("", ImGuiPopupFlags_AnyPopup))
        ImGui::SetWindowFocus();
    ImGui::SetItemFocusAlt(g_ImGuiMenuFocusLabel, focusPending);
}

static void DrawPracticeControls()
{
    static GuiStepState<i32> chapterStep {1, 1, 1, 10};
    static GuiStepState<i32> frameStep {1, 1, 1, 10};
    static GuiStepState<i32> lifeStep {1, 1, 1, 10};
    static GuiStepState<i32> bombStep {1, 1, 1, 10};
    static GuiStepState<i32> powerStep {1, 1, 1, 10};
    static GuiStepState<i32> grazeStep {1, 1, 10000, 10};
    static GuiStepState<i32> pointStep {1, 1, 1000, 10};
    static GuiStepState<i32> cherryStep {10, 10, 1000000, 10};
    static GuiStepState<i32> cherryPlusStep {1, 1, 10000, 10};
    static GuiStepState<i32> spellBonusStep {1, 1, 10, 10};
    static GuiStepState<i32> rankStep {1, 1, 10, 10};
    static int modeSelector[] = {1, 2, 0};
    static int stageSelector[] = {1, 2, 3, 4, 5, 6, 7, 8, 0};
    static int warpSelector[] = {1, 2, 3, 4, 5, 6, 7, 0};
    static int phaseRagefulSelector[] = {1, 2, 3, 0};
    static int phaseSimpleSelector[] = {1, 2, 0};

    const char *modeItems[] = {"", ThpracImGui::Text(ThpracImGui::TextId::Original),
                               ThpracImGui::Text(ThpracImGui::TextId::Custom)};
    const char *stageItems[] = {"", "1", "2", "3", "4", "5", "6",
                                ThpracImGui::Text(ThpracImGui::TextId::Extra),
                                ThpracImGui::Text(ThpracImGui::TextId::Phantasm)};
    const char *warpItems[] = {"", ThpracImGui::Text(ThpracImGui::TextId::None),
                               ThpracImGui::Text(ThpracImGui::TextId::StagePortion),
                               ThpracImGui::Text(ThpracImGui::TextId::MidBoss),
                               ThpracImGui::Text(ThpracImGui::TextId::EndBoss),
                               ThpracImGui::Text(ThpracImGui::TextId::NonSpell),
                               ThpracImGui::Text(ThpracImGui::TextId::SpellCard),
                               ThpracImGui::Text(ThpracImGui::TextId::Frame)};
    const char *phaseRagefulItems[] = {"", ThpracImGui::Text(ThpracImGui::TextId::Normal),
                                       ThpracImGui::Text(ThpracImGui::TextId::Full),
                                       ThpracImGui::Text(ThpracImGui::TextId::Rage)};
    const char *phaseSimpleItems[] = {"", ThpracImGui::Text(ThpracImGui::TextId::Normal),
                                      ThpracImGui::Text(ThpracImGui::TextId::Rage)};

    GuiCombo(ThpracImGui::Text(ThpracImGui::TextId::Mode), &g_Config.mode, modeSelector, modeItems);
    if (GuiCombo(ThpracImGui::Text(ThpracImGui::TextId::Stage), &g_Config.stage, stageSelector, stageItems))
    {
        // Upstream mStage() clears section/chapter only; phase/frame remain
        // until a later warp/section change says otherwise.
        g_MenuSectionIndex = 0;
        g_MenuChapter = 1;
        g_Config.section = 0;
    }

    if (g_Config.mode == 1)
    {
        if (GuiCombo(ThpracImGui::Text(ThpracImGui::TextId::Warp), &g_Config.warp, warpSelector, warpItems))
            ResetWarpDependentMenuState();

        if (g_Config.warp != 0)
        {
            const char *sectionLabel = SectionLabel(g_Config.warp);
            if (g_Config.warp == 1)
            {
                const i32 limit = ChapterLimit(g_Config.stage);
                const i32 firstHalf = ChapterFirstHalf(g_Config.stage);
                const i32 secondHalf = limit - firstHalf;
                char format[64] = {};
                if (secondHalf == 0)
                    std::snprintf(format, sizeof(format), ThpracImGui::Text(ThpracImGui::TextId::StagePortionN), g_MenuChapter);
                else if (g_MenuChapter <= firstHalf)
                    std::snprintf(format, sizeof(format), ThpracImGui::Text(ThpracImGui::TextId::FirstHalf), g_MenuChapter);
                else
                    std::snprintf(format, sizeof(format), ThpracImGui::Text(ThpracImGui::TextId::SecondHalf), g_MenuChapter - firstHalf);
                if (GuiSliderInt(sectionLabel, &g_MenuChapter, 1, limit, chapterStep, format))
                    CurrentSection(0, false);
            }
            else if (g_Config.warp >= 2 && g_Config.warp <= 5)
            {
#if defined(THPRAC_PORTABLE_ENABLED)
                const thprac::portable::generated::SectionLabel *matches[180] = {};
                const i32 sectionCount = BuildSectionMatches(matches, 180);
                const thprac::portable::generated::SectionLabel *section = CurrentSection();
                if (sectionCount > 0)
                {
                    std::vector<const char *> sectionItems(static_cast<std::size_t>(sectionCount) + 1);
                    std::vector<int> sectionSelector(static_cast<std::size_t>(sectionCount) + 1);
                    sectionItems[0] = "";
                    for (i32 index = 0; index < sectionCount; ++index)
                    {
                        switch (ThpracImGui::GetLocale())
                        {
                        case ThpracImGui::Locale::ZhCN: sectionItems[index + 1] = matches[index]->zh; break;
                        case ThpracImGui::Locale::JaJP: sectionItems[index + 1] = matches[index]->ja; break;
                        case ThpracImGui::Locale::EnUS:
                        default: sectionItems[index + 1] = matches[index]->en; break;
                        }
                        sectionSelector[index] = index + 1;
                    }
                    sectionSelector[sectionCount] = 0;
                    i32 sectionIndex = Clamp(g_MenuSectionIndex, 0, sectionCount - 1);
                    if (GuiCombo(sectionLabel, &sectionIndex, sectionSelector.data(), sectionItems.data()))
                    {
                        g_MenuSectionIndex = sectionIndex;
                        g_Config.phase = 0;
                        section = CurrentSection(0, false);
                    }
                }
                if (section && section->dialogue)
                    GuiCheckBox(ThpracImGui::Text(ThpracImGui::TextId::Dialog), &g_Config.dialogue);
#endif
            }
            else if (g_Config.warp == 6)
            {
                GuiDragInt(sectionLabel, &g_Config.frame, 0, 0x7fffffff, frameStep);
            }

            const i32 phaseCount = SectionPhaseCount(g_Config.section);
            if (phaseCount == 3)
            {
                g_Config.phase = Clamp(g_Config.phase, 0, 2);
                GuiCombo(ThpracImGui::Text(ThpracImGui::TextId::Phase), &g_Config.phase,
                         phaseRagefulSelector, phaseRagefulItems);
            }
            else if (phaseCount == 2)
            {
                g_Config.phase = Clamp(g_Config.phase, 0, 1);
                GuiCombo(ThpracImGui::Text(ThpracImGui::TextId::Phase), &g_Config.phase,
                         phaseSimpleSelector, phaseSimpleItems);
            }
        }

        GuiSliderInt(ThpracImGui::Text(ThpracImGui::TextId::Life), &g_Config.life, 0, 8, lifeStep);
        GuiSliderInt(ThpracImGui::Text(ThpracImGui::TextId::Bomb), &g_Config.bomb, 0, 8, bombStep);
        GuiDragScore();
        GuiSliderInt(ThpracImGui::Text(ThpracImGui::TextId::Power), &g_Config.power, 0, 128, powerStep);
        GuiDragInt(ThpracImGui::Text(ThpracImGui::TextId::Graze), &g_Config.graze, 0, 99999, grazeStep);
        GuiDragInt(ThpracImGui::Text(ThpracImGui::TextId::PointTotal), &g_Config.pointTotal, 0, 9999, pointStep);
        GuiDragInt(ThpracImGui::Text(ThpracImGui::TextId::PointStage), &g_Config.pointStage, 0, 9999, pointStep);
        if (GuiDragInt(ThpracImGui::Text(ThpracImGui::TextId::Cherry), &g_Config.cherry, 0, 9999990, cherryStep))
            g_Config.cherry -= g_Config.cherry % 10;
        if (GuiDragInt(ThpracImGui::Text(ThpracImGui::TextId::CherryMax), &g_Config.cherryMax, 0, 9999990, cherryStep))
            g_Config.cherryMax -= g_Config.cherryMax % 10;
        GuiSliderInt(ThpracImGui::Text(ThpracImGui::TextId::CherryPlus), &g_Config.cherryPlus, 0, 50000, cherryPlusStep);
        GuiSliderInt(ThpracImGui::Text(ThpracImGui::TextId::SpellBonus), &g_Config.spellBonus, 0, 30, spellBonusStep);
        ClampMenuRank();
        const i32 rankMin = g_MenuDifficulty == 0 ? 12 : 10;
        const i32 rankMax = g_Config.rankLock ? 99 : (g_MenuDifficulty == 0 ? 20 : 32);
        GuiSliderInt(ThpracImGui::Text(ThpracImGui::TextId::Rank), &g_Config.rank, rankMin, rankMax, rankStep);
        if (GuiCheckBox(ThpracImGui::Text(ThpracImGui::TextId::RankLock), &g_Config.rankLock))
            ClampMenuRank();
    }

    FinishPracticeNav();
}
#endif

static void RequestMenuClose(MenuResult result, bool accept)
{
    if (!g_MenuOpen || g_MenuVisualState == MenuVisualState::Closing)
        return;
    if (accept)
    {
        CurrentSection();
        StoreWorkingMenuConfig();
        CommitMenuConfigToRuntime();
#if defined(THPRAC_PORTABLE_ENABLED)
        if (!PublishPortableSession())
            g_Config.active = false;
#endif
#ifdef __EMSCRIPTEN__
        if (g_Config.active)
            PublishConfigToHost();
#endif
        // THGuiPrac::State(3): SetFade(.8,.8), Close().
        g_MenuCloseStep = 0.8f;
    }
    else
    {
        StoreWorkingMenuConfig();
        // State(4) closes without committing widget members. State(1) already
        // THReset() the live THPracParam, so Cancel must leave the live owner
        // all-zero rather than retaining an inactive copy of menu defaults.
        g_Config = {};
        // THGuiPrac::State(4) alone clears mNavFocus. State(3) deliberately
        // leaves it untouched, so accepted runs remember the last focused
        // widget when Practice is opened again.
#ifdef TH_ENABLE_THPRAC
        g_ImGuiMenuFocusLabel = ThpracImGui::Text(ThpracImGui::TextId::Stage);
#endif
        g_ImGuiMenuFocusRemembered = false;
        // State(4) uses the normal .1 fade configured by State(1).
        g_MenuCloseStep = 0.1f;
    }
    g_MenuPendingResult = result;
    g_MenuVisualState = MenuVisualState::Closing;
}

MenuResult PollPracticeMenu()
{
    if (g_MenuOpen)
    {
        // Same ownership as upstream GameGuiWnd: ImGui receives directions;
        // the original Practice state machine still owns X/Z and invokes
        // State(4)/State(3) respectively.
        if (WAS_PRESSED_RAW(TH_BUTTON_RETURNMENU))
            RequestMenuClose(MenuResult::Cancelled, false);
        else if (WAS_PRESSED_RAW(TH_BUTTON_SELECTMENU))
            RequestMenuClose(MenuResult::Accepted, true);
        return MenuResult::Waiting;
    }
    if (g_MenuResult != MenuResult::Waiting)
    {
        const MenuResult result = g_MenuResult;
        g_MenuResult = MenuResult::Waiting;
        return result;
    }
    return MenuResult::Waiting;
}

void DrawPracticeMenu()
{
#ifdef TH_ENABLE_THPRAC
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

    const auto locale = ThpracImGui::GetLocale();
    const bool chinese = locale == ThpracImGui::Locale::ZhCN;
    ImGui::SetNextWindowSize(chinese ? ImVec2(330.0f, 390.0f) : ImVec2(400.0f, 390.0f), ImGuiCond_Always);
    ImGui::SetNextWindowPos(chinese ? ImVec2(260.0f, 70.0f) : ImVec2(230.0f, 70.0f), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(g_MenuAlpha);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                                       ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
                                       ImGuiWindowFlags_NoBringToFrontOnFocus;
    if (ImGui::Begin(ThpracImGui::Text(ThpracImGui::TextId::Menu), nullptr, flags))
    {
        const float itemWidth = locale == ThpracImGui::Locale::EnUS ? -80.0f :
                                locale == ThpracImGui::Locale::JaJP ? -65.0f : -60.0f;
        ImGui::PushItemWidth(itemWidth);
        ImGui::TextUnformatted(ThpracImGui::Text(ThpracImGui::TextId::Menu));
        ImGui::Separator();
        if (g_MenuVisualState == MenuVisualState::Open)
        {
            DrawPracticeControls();
            StoreWorkingMenuConfig();
        }
        ImGui::PopItemWidth();
    }
    ImGui::End();
    ImGui::PopStyleVar(2);
#endif
}

void DebugAcceptPracticeMenu()
{
    if (!g_MenuOpen)
        return;
    CurrentSection();
    StoreWorkingMenuConfig();
    CommitMenuConfigToRuntime();
#if defined(THPRAC_PORTABLE_ENABLED)
    if (!PublishPortableSession())
        g_Config.active = false;
#endif
    g_MenuOpen = false;
    g_MenuResult = MenuResult::Accepted;
}

bool UpdatePauseMenu()
{
    return false;
}

void DrawPauseMenuPanel()
{
}

void PrepareStart(GameManager &gameManager)
{
    if (!Active() || gameManager.replay)
        return;
    gameManager.currentStage = g_Config.stage;
    gameManager.practice = 1;
    if (g_Config.stage == 6)
        gameManager.difficulty = 4;
    else if (g_Config.stage == 7)
        gameManager.difficulty = 5;
}

i32 ResolveWarpFrame(i32 stage, i32 portion)
{
    static constexpr i32 frames[][8] = {
        {540, 1341, 3107, 0, 0, 0, 0, 0},
        {390, 3366, 0, 0, 0, 0, 0, 0},
        {390, 854, 1805, 0, 0, 0, 0, 0},
        {80, 1948, 3028, 4288, 7964, 10136, 11396, 13166},
        {440, 840, 2550, 4883, 0, 0, 0, 0},
        {660, 1180, 0, 0, 0, 0, 0, 0},
        {485, 1035, 1955, 2615, 3305, 4028, 5808, 7088},
        {485, 965, 1595, 2485, 3191, 3928, 5708, 6988}
    };
    if (stage < 0 || stage >= 8 || portion < 1 || portion > 8)
        return 0;
    return frames[stage][portion - 1];
}

static void SetPointItems(GameManager &gameManager)
{
    ZunGlobals &globals = *gameManager.globals;
    if (g_Config.point)
        globals.pointItemsCollectedThisStage = globals.pointItemsCollectedForExtend = g_Config.point;
    else
    {
        globals.pointItemsCollectedThisStage = g_Config.pointStage;
        globals.pointItemsCollectedForExtend = g_Config.pointTotal;
    }
    // THSetPoint() explicitly recreates the original extend-threshold loop
    // after overwriting point_stage/point_total. Preserve point_extends as the
    // loop's starting owner; only increment it while the selected total has
    // already crossed each threshold.
    while (true)
    {
        if (gameManager.difficulty >= 4)
            globals.nextNeededPointItemsForExtend = globals.extendsFromPointItems == 0 ? 200 :
                globals.extendsFromPointItems == 1 ? 500 : 500 * (globals.extendsFromPointItems - 2) + 800;
        else if (globals.extendsFromPointItems >= 3)
            globals.nextNeededPointItemsForExtend = globals.extendsFromPointItems >= 5 ?
                200 * (globals.extendsFromPointItems - 5) + 800 : 150 * (globals.extendsFromPointItems - 3) + 300;
        else
            globals.nextNeededPointItemsForExtend = 75 * globals.extendsFromPointItems + 50;
        if (globals.pointItemsCollectedForExtend < globals.nextNeededPointItemsForExtend)
            break;
        ++globals.extendsFromPointItems;
    }
}

void ApplyInitialState(GameManager &gameManager, bool applyStats)
{
    // Upstream mode 0 is an unmodified whole-stage Practice run. Only mode 1
    // applies thprac parameters and ECL/time warps.
    if (!Active() || g_Config.mode != 1)
    {
        g_ResurrectionButterflySpawnSkipPending = false;
        return;
    }
    // Upstream THSectionPatch() forces PLAYER->playerState back to ALIVE for
    // every non-zero direct section/chapter patch before the patched ECL is
    // allowed to run.  Player::RegisterChain has already initialized the
    // vanilla spawn state by this point, so omitting this write leaves direct
    // practice warps in the normal spawning/invulnerability lifecycle instead
    // of the trainer's immediate-live state.
    if (g_Config.section != 0)
        g_Player.playerState = PLAYER_STATE_ALIVE;

    // Upstream THSectionPatch enables th07_rb only for TH07_ST6_BOSS10,
    // consumes it once, then self-disables. Portable section IDs are identical
    // to the generated patch IDs; Stage 6 is zero-based stage index 5 here.
    g_ResurrectionButterflySpawnSkipPending = (g_Config.stage == 5 && g_Config.section == 60);
    if (applyStats && gameManager.globals)
    {
        gameManager.SetLivesRemaining(g_Config.life);
        gameManager.SetBombsRemainingAndComputeCsum(g_Config.bomb);
        gameManager.SetCurrentPower(g_Config.power);
        const u32 score = static_cast<u32>(g_Config.score / 10);
        gameManager.globals->guiScore = gameManager.globals->score = score;
        gameManager.globals->grazeInStage = gameManager.globals->grazeInTotal = g_Config.graze;
        gameManager.globals->spellCardsCaptured = g_Config.spellBonus;
        SetPointItems(gameManager);
        gameManager.cherry = gameManager.globals->cherryStart + g_Config.cherry;
        gameManager.cherryMax = gameManager.globals->cherryStart + g_Config.cherryMax;
        gameManager.cherryPlus = gameManager.globals->cherryStart + g_Config.cherryPlus;
        // Upstream th07_patch_main treats rank==0 as "leave vanilla rank
        // alone".  Do not turn a missing/zero Replay field into the GUI's
        // minimum rank.  Non-zero values are written exactly as the hook does;
        // normal menu input has already been bounded by the widget.
        if (g_Config.rank != 0)
        {
            gameManager.rank.rank = g_Config.rank;
            if (g_Config.rankLock)
                gameManager.rank.minRank = gameManager.rank.maxRank = g_Config.rank;
        }
        gameManager.RegenerateGameIntegrityCsum();
    }
    auto applyStage3BackgroundPatch = []() {
        // ECLST3BG() writes the currently loaded primary stage-background ANM
        // slot (ANM_FILE_STAGE_BG1 == 5).  The helper name is historical: the
        // direct-Frame path calls it unconditionally, while THStageWarp calls
        // it only for Stage 3 chapters.
        if (g_AnmManager && g_AnmManager->anmFiles[5].raw)
        {
            u16 *raw = reinterpret_cast<u16 *>(g_AnmManager->anmFiles[5].raw);
            raw[0xa0] = raw[0xd2] = raw[0x104] = raw[0x136] = 2;
        }
    };

    if (g_Config.frame > 0)
    {
        // th07_patch_main: direct Frame mode is exactly
        //   ECLST3BG(); ECLTimeWarp(2, thPracParam.frame);
        // It is independent from THStageWarp's Stage-2 three-timeline chapter
        // special case.
        applyStage3BackgroundPatch();
        for (i32 index = 0; index < 2; index++)
            g_EnemyManager.timelines[index].timelineTime.current = g_Config.frame;
    }
    else if (g_Config.section >= 10000 && g_Config.section < 20000)
    {
        // Chapter mode belongs to THStageWarp, whose timeline cardinality is
        // different from direct Frame: only Stage 2 uses three timelines, and
        // only Stage 3 calls ECLST3BG().  The chapter-only ECL byte mutations
        // are applied by the portable adapter while the file is loaded.
        const i32 encoded = g_Config.section - 10000;
        const i32 chapterStage = encoded / 100 - 1;
        const i32 portion = encoded % 100;
        const i32 chapterFrame = ResolveWarpFrame(chapterStage, portion);
        if (chapterFrame > 0)
        {
            if (chapterStage == 2)
                applyStage3BackgroundPatch();
            const i32 count = chapterStage == 1 ? 3 : 2;
            for (i32 index = 0; index < count; index++)
                g_EnemyManager.timelines[index].timelineTime.current = chapterFrame;
        }
    }
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

static bool LoadConfigJson(const std::string &json)
{
    const bool portableSchema =
        json.find("\"schema\":\"eagler-touhou/thprac-session/1\"") != std::string::npos;
    // Upstream v2.3.0.3 stores THPracParam::GetJson() directly: no schema
    // field, only the thprac version plus game="th07".
    const bool upstreamSchema = json.find("\"version\":") != std::string::npos;
    if ((!portableSchema && !upstreamSchema) || json.find("\"game\":\"th07\"") == std::string::npos)
        return false;
    Config config;
    config.active = true;
    config.mode = static_cast<i32>(JsonNumber(json, "mode", 0));
    config.stage = static_cast<i32>(JsonNumber(json, "stage", 0));
    config.warp = static_cast<i32>(JsonNumber(json, "warp", 0));
    config.section = static_cast<i32>(JsonNumber(json, "section", 0));
    config.phase = static_cast<i32>(JsonNumber(json, "phase", 0));
    config.frame = static_cast<i32>(JsonNumber(json, "frame", 0));
    config.dialogue = JsonBool(json, "dlg", false);
    config.score = static_cast<i64>(JsonNumber(json, "score", 0));
    // THPracParam::ReadJson() memsets the parameter object to zero before
    // parsing. Missing keys keep zero; they do not inherit Practice-menu
    // defaults.
    config.life = static_cast<i32>(JsonNumber(json, "life", 0));
    config.bomb = static_cast<i32>(JsonNumber(json, "bomb", 0));
    config.power = static_cast<i32>(JsonNumber(json, "power", 0));
    config.graze = static_cast<i32>(JsonNumber(json, "graze", 0));
    config.point = static_cast<i32>(JsonNumber(json, "point", 0));
    config.pointTotal = static_cast<i32>(JsonNumber(json, "point_total", 0));
    config.pointStage = static_cast<i32>(JsonNumber(json, "point_stage", 0));
    config.cherry = static_cast<i32>(JsonNumber(json, "cherry", 0));
    config.cherryMax = static_cast<i32>(JsonNumber(json, "cherryMax", 0));
    config.cherryPlus = static_cast<i32>(JsonNumber(json, "cherryPlus", 0));
    config.spellBonus = static_cast<i32>(JsonNumber(json, "spellBonus", 0));
    config.rank = static_cast<i32>(JsonNumber(json, "rank", 0));
    config.rankLock = JsonBool(json, "rankLock", false);
    SetConfig(config);
    return true;
}

static u32 ReadLe32(const u8 *bytes)
{
    return static_cast<u32>(bytes[0]) |
           (static_cast<u32>(bytes[1]) << 8) |
           (static_cast<u32>(bytes[2]) << 16) |
           (static_cast<u32>(bytes[3]) << 24);
}

static void WriteLe32(u8 *bytes, u32 value)
{
    bytes[0] = static_cast<u8>(value);
    bytes[1] = static_cast<u8>(value >> 8);
    bytes[2] = static_cast<u8>(value >> 16);
    bytes[3] = static_cast<u8>(value >> 24);
}

static bool ReadWholeFile(const char *path, std::vector<u8> &bytes)
{
    SDL_IOStream *file = SDL_IOFromFile(path, "rb");
    if (!file)
        return false;
    const Sint64 size = SDL_GetIOSize(file);
    if (size <= 0 || size > 0x7fffffff || SDL_SeekIO(file, 0, SDL_IO_SEEK_SET) < 0)
    {
        SDL_CloseIO(file);
        return false;
    }
    bytes.resize(static_cast<std::size_t>(size));
    const bool ok = SDL_ReadIO(file, bytes.data(), bytes.size()) == bytes.size();
    SDL_CloseIO(file);
    if (!ok)
        bytes.clear();
    return ok;
}

static bool WriteWholeFile(const char *path, const std::vector<u8> &bytes)
{
    SDL_IOStream *file = SDL_IOFromFile(path, "wb");
    if (!file)
        return false;
    const bool ok = SDL_WriteIO(file, bytes.data(), bytes.size()) == bytes.size();
    SDL_CloseIO(file);
    return ok;
}

static bool ExtractReplayMetadataJson(const std::vector<u8> &bytes, std::string &json)
{
    const std::size_t baseSize = ReplayExtension::BaseFileSize(bytes.data(), bytes.size());
    if (baseSize < 24 || std::memcmp(bytes.data(), "T7RP", 4) != 0 ||
        std::memcmp(bytes.data() + baseSize - 4, "PRAC", 4) != 0)
        return false;

    const u32 payloadSize = ReadLe32(bytes.data() + baseSize - 8);
    if (payloadSize == 0 || payloadSize >= 512 || (payloadSize & 3u) != 0 ||
        static_cast<std::size_t>(payloadSize) > baseSize - 8 - 16)
        return false;
    const std::size_t payloadOffset = baseSize - 8 - payloadSize;
    std::size_t jsonSize = 0;
    while (jsonSize < payloadSize && bytes[payloadOffset + jsonSize] != 0)
        ++jsonSize;
    if (jsonSize == 0 || jsonSize == payloadSize)
        return false;
    for (std::size_t i = jsonSize; i < payloadSize; ++i)
    {
        if (bytes[payloadOffset + i] != 0)
            return false;
    }
    json.assign(reinterpret_cast<const char *>(bytes.data() + payloadOffset), jsonSize);
    return true;
}

void ReplayMenuReset()
{
    // THGuiRep::State(1): THReset(), mRepStatus=false, mParamStatus=false.
    g_ReplayPlaybackActive = false;
    g_ReplayStartupCommitted = false;
    g_ReplayCandidateValid = false;
    // State(1) does not reset mRepParam. TH07 ReadJson() itself Reset()s before
    // every successful parse, and CheckReplay() Reset()s it on failure, so the
    // retained candidate bytes are normally unobservable while status=false;
    // still preserve the exact ownership boundary rather than inventing an
    // extra reset here.
    g_Config = {};
#ifdef __EMSCRIPTEN__
    EM_ASM({
        Module.eaglerOptions = Module.eaglerOptions || {};
        Module.eaglerOptions.thpracSession = null;
    });
#endif
#if defined(THPRAC_PORTABLE_ENABLED)
    ThpracPortableTh07SetSessionJson(nullptr);
#endif
}

bool ReplayMenuCheck(const char *replayPath)
{
    // THGuiRep::State(2): inspect the selected replay into mRepParam without
    // mutating the live thPracParam / active ECL patch context.
    if (!replayPath || !*replayPath)
    {
        g_ReplayCandidate = {};
        return false;
    }
    std::vector<u8> bytes;
    std::string json;
    if (!ReadWholeFile(replayPath, bytes) || !ExtractReplayMetadataJson(bytes, json))
    {
        // CheckReplay() failure resets mRepParam but leaves mParamStatus
        // untouched. This sticky flag is intentional source behavior.
        g_ReplayCandidate = {};
        return false;
    }

    const Config saved = g_Config;
    const bool parsed = LoadConfigJson(json);
    if (parsed)
    {
        g_ReplayCandidate = g_Config;
        g_ReplayCandidateValid = true;
    }
    else
    {
        g_ReplayCandidate = {};
        // Do not clear g_ReplayCandidateValid: upstream leaves mParamStatus
        // unchanged when mRepParam.ReadJson() fails.
    }
    g_Config = saved;
    return parsed;
}

void ReplayMenuActivate()
{
    // THGuiRep::State(3): mRepStatus becomes true for every accepted replay;
    // only a valid PRAC candidate is copied into live thPracParam.
    g_ReplayPlaybackActive = true;
    g_ReplayStartupCommitted = true;
    if (g_ReplayCandidateValid)
    {
        g_Config = g_ReplayCandidate;
#if defined(THPRAC_PORTABLE_ENABLED)
        // Publish the already-resolved live THPracParam. The adapter is a
        // consumer of State(3), not a second independent Replay parser.
        if (g_Config.active)
        {
            if (!PublishPortableSession())
                g_Config = {};
        }
        else
        {
            // Sticky mParamStatus + a later failed CheckReplay means State(3)
            // copies a Reset() mRepParam. Represent that as no live adapter
            // session, not an invented empty JSON payload.
            ThpracPortableTh07SetSessionJson(nullptr);
        }
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
    else
    {
        g_Config = {};
#ifdef __EMSCRIPTEN__
        EM_ASM({
            Module.eaglerOptions = Module.eaglerOptions || {};
            Module.eaglerOptions.thpracSession = null;
        });
#endif
#if defined(THPRAC_PORTABLE_ENABLED)
        ThpracPortableTh07SetSessionJson(nullptr);
#endif
    }
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

bool OverlayInvincible()
{
#ifdef TH_ENABLE_THPRAC
    return !g_GameManager.replay && g_Overlay.invincible;
#else
    return false;
#endif
}
bool OverlayInfiniteLives()
{
#ifdef TH_ENABLE_THPRAC
    return !g_GameManager.replay && g_Overlay.infiniteLives;
#else
    return false;
#endif
}
bool OverlayInfiniteBombs()
{
#ifdef TH_ENABLE_THPRAC
    return !g_GameManager.replay && g_Overlay.infiniteBombs;
#else
    return false;
#endif
}
bool OverlayInfinitePower()
{
#ifdef TH_ENABLE_THPRAC
    return !g_GameManager.replay && g_Overlay.infinitePower;
#else
    return false;
#endif
}
bool OverlayTimeLock()
{
#ifdef TH_ENABLE_THPRAC
    return !g_GameManager.replay && g_Overlay.timeLock;
#else
    return false;
#endif
}
bool OverlayAutoBomb()
{
#ifdef TH_ENABLE_THPRAC
    return !g_GameManager.replay && g_Overlay.autoBomb;
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

bool AdvancedAllClearBonus()
{
#ifdef TH_ENABLE_THPRAC
    return !g_GameManager.replay && g_AdvancedOptions.allClearBonus;
#else
    return false;
#endif
}

bool AdvancedFixSpellBonusDisplay()
{
#ifdef TH_ENABLE_THPRAC
    return g_AdvancedOptions.fixSpellBonusDisplay;
#else
    return false;
#endif
}

bool ConsumeResurrectionButterflySpawnSkip()
{
    if (!g_ResurrectionButterflySpawnSkipPending)
        return false;
    g_ResurrectionButterflySpawnSkipPending = false;
    return true;
}

void ResetTracker()
{
    g_TrackerBorderBreaks = 0;
}

void RecordBorderBreak()
{
    if (g_TrackerBorderBreaks < 0x7fffffff)
        ++g_TrackerBorderBreaks;
}

void SetSupervisorFadeOutScope(bool active)
{
    g_SupervisorFadeOutScope = active;
}

bool FilterAudioCommand(i32 opcode, i32 arg)
{
    // th07_soundplayer_queue_command runs at SoundPlayer::PushCommand entry.
    // With F7 disabled (or outside advanced Practice), AUDIO_START only tracks
    // the current slot. With F7 enabled, duplicate START and STOP/SHUTDOWN/
    // PAUSE/UNPAUSE/FADEOUT are suppressed. The one upstream exception is a
    // FadeOutMusic-originated FADEOUT outside Resurrection Butterfly.
    if (!OverlayEverlastingBgm() || !AdvancedActive())
    {
        if (opcode == AUDIO_START)
            g_EverlastingCurrentSong = arg;
        return false;
    }
    if (opcode == AUDIO_FADEOUT && g_SupervisorFadeOutScope && g_Config.section != 60)
        return false;
    if (opcode >= AUDIO_STOP)
        return true;
    if (opcode == AUDIO_START)
    {
        if (arg == g_EverlastingCurrentSong)
            return true;
        g_EverlastingCurrentSong = arg;
    }
    return false;
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
    const bool *keys = SDL_GetKeyboardState(&count);
    const bool down = (keys && static_cast<int>(scancode) < count && keys[scancode]) ||
                      EaglerOverlayKeyDown(scancode);
    const bool pressed = down && !g_OverlayKeyDown[slot];
    g_OverlayKeyDown[slot] = down;
    return pressed;
}
#endif

void UpdateOverlay()
{
#ifdef TH_ENABLE_THPRAC
    if (OverlayKeyPressed(0, SDL_SCANCODE_BACKSPACE)) g_ModMenuToggleRequested = true;
    if (OverlayKeyPressed(8, SDL_SCANCODE_TAB)) g_Overlay.trackerOpen = !g_Overlay.trackerOpen;
    if (OverlayKeyPressed(9, SDL_SCANCODE_F12)) g_AdvancedMenuToggleRequested = true;
#endif
}

void DrawOverlay()
{
#ifdef TH_ENABLE_THPRAC
    if (!ThpracImGui::IsFrameOpen())
        return;

    // TH07 GameGuiEnd(drawCursor) requests the software cursor only for
    // THAdvOptWnd or THGuiPrac, and the Win32 backend only draws it in real
    // fullscreen where the OS cursor is hidden. THGuiRep is deliberately not
    // part of drawCursor upstream.
    const bool fullscreen = g_GameWindow.window != nullptr &&
        (SDL_GetWindowFlags(g_GameWindow.window) & SDL_WINDOW_FULLSCREEN) != 0;
    ImGui::GetIO().MouseDrawCursor = fullscreen &&
        (g_AdvancedOptions.menuOpen || g_MenuOpen);
    if (g_ModMenuToggleRequested)
    {
        if (!ImGui::IsAnyItemActive())
            g_Overlay.menuOpen = !g_Overlay.menuOpen;
        g_ModMenuToggleRequested = false;
    }
    PublishEaglerOverlayMenuState(g_Overlay.menuOpen);
    if (g_Overlay.menuOpen)
    {
        // GuiHotKey F1..F7 are only evaluated from THOverlay::OnContentUpdate
        // while the Mod Menu is open. Their key history intentionally does not
        // advance while the window is closed.
        if (OverlayKeyPressed(1, SDL_SCANCODE_F1)) g_Overlay.invincible = !g_Overlay.invincible;
        if (OverlayKeyPressed(2, SDL_SCANCODE_F2)) g_Overlay.infiniteLives = !g_Overlay.infiniteLives;
        if (OverlayKeyPressed(3, SDL_SCANCODE_F3)) g_Overlay.infiniteBombs = !g_Overlay.infiniteBombs;
        if (OverlayKeyPressed(4, SDL_SCANCODE_F4)) g_Overlay.infinitePower = !g_Overlay.infinitePower;
        if (OverlayKeyPressed(5, SDL_SCANCODE_F5)) g_Overlay.timeLock = !g_Overlay.timeLock;
        if (OverlayKeyPressed(6, SDL_SCANCODE_F6)) g_Overlay.autoBomb = !g_Overlay.autoBomb;
        if (OverlayKeyPressed(7, SDL_SCANCODE_F7)) g_Overlay.everlastingBgm = !g_Overlay.everlastingBgm;

        ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f), ImGuiCond_Always);
        ImGui::SetNextWindowBgAlpha(0.5f);
        constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
        if (ImGui::Begin("Mod Menu###th07-thprac-overlay", nullptr, flags))
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
    // THAdvOptWnd::StaticUpdate() runs after THOverlay::Update() upstream.
    if (g_AdvancedMenuToggleRequested)
    {
        g_AdvancedOptions.menuOpen = !g_AdvancedOptions.menuOpen;
        g_AdvancedMenuToggleRequested = false;
    }
    if (g_Overlay.trackerOpen && g_Supervisor.curState == 2 && g_GameManager.globals)
    {
        static const char *diff[3][6] = {
            {"Easy", "Normal", "Hard", "Lunatic", "Extra", "Phantasm"},
            {"Easy", "Normal", "Hard", "Lunatic", "Extra", "Phantasm"},
            {"イージー", "ノーマル", "ハード", "ルナティック", "エキストラ", "ファンタズム"},
        };
        static const char *shots[3][6] = {
            {"灵梦A", "灵梦B", "魔理沙A", "魔理沙B", "咲夜A", "咲夜B"},
            {"ReimuA", "ReimuB", "MarisaA", "MarisaB", "SakuyaA", "SakuyaB"},
            {"霊夢A", "霊夢B", "魔理沙A", "魔理沙B", "咲夜A", "咲夜B"},
        };
        const i32 locale = static_cast<i32>(ThpracImGui::GetLocale());
        ImGui::SetNextWindowSize(ImVec2(170.0f, 0.0f), ImGuiCond_Always);
        ImGui::SetNextWindowPos(ImVec2(450.0f, 193.0f), ImGuiCond_Always);
        constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_NoNav;
        if (ImGui::Begin("Tracker###th07-thprac-tracker", nullptr, flags))
        {
            char title[48] = {};
            std::snprintf(title, sizeof(title), "%s (%s)", diff[locale][Clamp(g_GameManager.difficulty, 0, 5)],
                          shots[locale][Clamp<u8>(g_GameManager.shotTypeAndCharacter, 0, 5)]);
            const ImVec2 size = ImGui::CalcTextSize(title);
            ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.5f - size.x * 0.5f);
            ImGui::TextUnformatted(title);
            if (ImGui::BeginTable("Tracker table", 2))
            {
                ImGui::TableNextRow(); ImGui::TableNextColumn();
                ImGui::TextUnformatted(ThpracImGui::Text(ThpracImGui::TextId::TrackerMiss));
                ImGui::TableNextColumn(); ImGui::Text("%d", static_cast<i32>(g_GameManager.globals->deaths));
                ImGui::TableNextRow(); ImGui::TableNextColumn();
                ImGui::TextUnformatted(ThpracImGui::Text(ThpracImGui::TextId::TrackerBomb));
                ImGui::TableNextColumn(); ImGui::Text("%d", static_cast<i32>(g_GameManager.globals->bombsUsed));
                ImGui::TableNextRow(); ImGui::TableNextColumn();
                ImGui::TextUnformatted(ThpracImGui::Text(ThpracImGui::TextId::TrackerBorderBreak));
                ImGui::TableNextColumn(); ImGui::Text("%d", g_TrackerBorderBreaks);
                ImGui::EndTable();
            }
        }
        ImGui::End();
    }
    if (g_AdvancedOptions.menuOpen)
    {
        const i32 locale = static_cast<i32>(ThpracImGui::GetLocale());
        static const char *gameplay[3] = {"游戏玩法", "Gameplay", "ゲームプレイ"};
        static const char *allClearBonus[3] = {"全通奖励", "All Clear Bonus", "オールクリアボーナス"};
        static const char *about[3] = {"关于 thprac", "About thprac", "thprac について"};
        static const char *fpsUnavailable[3] = {
            "当前 portable 构建未加载 openinputlagpatch/vpatch；与原版 thprac 相同，此时游戏速度选项不可用。",
            "No openinputlagpatch/vpatch backend is loaded; like upstream thprac, Game Speed is unavailable in this state.",
            "openinputlagpatch/vpatch が読み込まれていないため、原版 thprac と同様にゲーム速度は使用できません。",
        };
        static const char *showLicense[3] = {"显示许可证", "Show license", "ライセンスを表示"};
        static const char *hideLicense[3] = {"隐藏许可证", "Hide license", "ライセンスを隠す"};

        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(640.0f, 480.0f), ImGuiCond_Always);
        ImGui::SetNextWindowBgAlpha(0.8f);
        constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;
        if (ImGui::Begin("Advanced Options###th07-thprac-advanced", nullptr, flags))
        {
            ImGui::TextUnformatted(ThpracImGui::Text(ThpracImGui::TextId::AdvancedOptions));
            ImGui::Separator();

            if (ImGui::CollapsingHeader(ThpracImGui::Text(ThpracImGui::TextId::GameSpeed),
                                        ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Indent();
                ImGui::BeginDisabled();
                i32 fixedFps = 60;
                ImGui::SliderInt("FPS", &fixedFps, 60, 6000);
                ImGui::EndDisabled();
                ImGui::TextWrapped("%s", fpsUnavailable[locale]);
                ImGui::Unindent();
            }

            if (ImGui::CollapsingHeader(gameplay[locale], ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Indent();
                ImGui::Checkbox(allClearBonus[locale], &g_AdvancedOptions.allClearBonus);
                ImGui::Checkbox(ThpracImGui::Text(ThpracImGui::TextId::FixSpellBonusDisplay),
                                &g_AdvancedOptions.fixSpellBonusDisplay);
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

    // Common GameGuiEnd language chord. As upstream, digit state is updated
    // only while Alt is held and no current ImGui item owns input.
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

bool AdvancedOptionsOpen()
{
#ifdef TH_ENABLE_THPRAC
    return g_AdvancedOptions.menuOpen;
#else
    return false;
#endif
}

static bool BuildUpstreamReplayJson(std::string &json)
{
    char buffer[2048] = {};
    int length = std::snprintf(buffer, sizeof(buffer),
        "{\"version\":\"2.3.0.3\",\"game\":\"th07\",\"mode\":%d,\"stage\":%d",
        g_Config.mode, g_Config.stage);
    if (length <= 0 || static_cast<std::size_t>(length) >= sizeof(buffer))
        return false;
    auto append = [&](const char *fmt, auto... args) {
        const int written = std::snprintf(buffer + length, sizeof(buffer) - static_cast<std::size_t>(length), fmt, args...);
        if (written < 0 || static_cast<std::size_t>(written) >= sizeof(buffer) - static_cast<std::size_t>(length))
            return false;
        length += written;
        return true;
    };
    if (g_Config.section && !append(",\"section\":%d", g_Config.section)) return false;
    if (g_Config.phase && !append(",\"phase\":%d", g_Config.phase)) return false;
    if (g_Config.frame && !append(",\"frame\":%d", g_Config.frame)) return false;
    if (g_Config.dialogue && !append(",\"dlg\":true")) return false;
    if (!append(",\"score\":%lld,\"life\":%d,\"bomb\":%d,\"power\":%d,\"graze\":%d,"
                "\"point_total\":%d,\"point_stage\":%d,\"cherry\":%d,\"cherryMax\":%d,"
                "\"cherryPlus\":%d,\"spellBonus\":%d,\"rank\":%d,\"rankLock\":%s}",
                static_cast<long long>(g_Config.score), g_Config.life, g_Config.bomb, g_Config.power,
                g_Config.graze, g_Config.pointTotal, g_Config.pointStage, g_Config.cherry,
                g_Config.cherryMax, g_Config.cherryPlus, g_Config.spellBonus, g_Config.rank,
                g_Config.rankLock ? "true" : "false"))
        return false;
    json.assign(buffer, static_cast<std::size_t>(length));
    return true;
}

bool LoadReplayMetadata(const char *replayPath)
{
    g_Config = {};
#if !defined(__EMSCRIPTEN__) && defined(THPRAC_PORTABLE_ENABLED)
    ThpracPortableTh07SetSessionJson(nullptr);
#endif
    if (!replayPath || !*replayPath)
        return false;
    std::vector<u8> bytes;
    if (!ReadWholeFile(replayPath, bytes))
        return false;
    std::string json;
    if (!ExtractReplayMetadataJson(bytes, json))
        return false;
    if (!LoadConfigJson(json))
        return false;
    g_ReplayPlaybackActive = true;
#ifdef __EMSCRIPTEN__
    PublishConfigToHost();
#endif
#if !defined(__EMSCRIPTEN__) && defined(THPRAC_PORTABLE_ENABLED)
    if (!ThpracPortableTh07SetSessionJson(json.c_str()))
    {
        g_Config = {};
        return false;
    }
#endif
    return true;
}

bool SaveReplayMetadata(const char *replayPath)
{
    // Upstream th07_save_replay uses `if (thPracParam.mode) THSaveReplay`.
    // Mode=Original is still an active Practice run, but its replay must stay
    // vanilla and carry no PRAC metadata.
    if (!AdvancedActive() || !replayPath || !*replayPath)
        return false;
    std::vector<u8> bytes;
    if (!ReadWholeFile(replayPath, bytes) || bytes.size() < 16 || std::memcmp(bytes.data(), "T7RP", 4) != 0)
        return false;

    std::string json;
    if (!BuildUpstreamReplayJson(json))
        return false;

    std::size_t payloadSize = json.size() + 1;
    while ((payloadSize & 3u) != 0)
        ++payloadSize;
    if (payloadSize >= 512)
        return false;
    const std::size_t oldSize = bytes.size();
    bytes.resize(oldSize + payloadSize + 8, 0);
    std::memcpy(bytes.data() + oldSize, json.data(), json.size());
    WriteLe32(bytes.data() + oldSize + payloadSize, static_cast<u32>(payloadSize));
    std::memcpy(bytes.data() + oldSize + payloadSize + 4, "PRAC", 4);

    // ReplaySaveParam's TH07 checksum path: copy bytes from file offset 13,
    // decrypt from +3 (absolute offset 16) using the replay key, then sum the
    // complete copied range.  The actual file stays obfuscated and its PRAC
    // trailer stays plain; only this checksum scratch view is decrypted.
    std::vector<u8> checksumBytes(bytes.begin() + 13, bytes.end());
    u8 key = checksumBytes[0];
    for (std::size_t i = 0; i < bytes.size() - 16; ++i)
    {
        checksumBytes[3 + i] = static_cast<u8>(checksumBytes[3 + i] - key);
        key = static_cast<u8>(key + 7);
    }
    u32 checksum = 0x3f000318;
    for (u8 value : checksumBytes)
        checksum += value;
    WriteLe32(bytes.data() + 8, checksum);
    return WriteWholeFile(replayPath, bytes);
}

bool DebugReplayMetadataRoundTrip(const char *replayPath)
{
#if !defined(__EMSCRIPTEN__)
    if (!replayPath || !*replayPath)
        return false;
    // Prefer a caller-provided real replay when present. This lets the same
    // self-test prove that appending PRAC metadata still passes TH07's real
    // replay checksum/deobfuscation validator, while retaining the tiny T7RP
    // fallback for normal automated runs that do not ship copyrighted replay
    // fixtures.
    std::vector<u8> seed;
    const bool realReplaySeed = ReadWholeFile(replayPath, seed) && seed.size() >= 16 &&
                                std::memcmp(seed.data(), "T7RP", 4) == 0 &&
                                (static_cast<u16>(seed[4]) | (static_cast<u16>(seed[5]) << 8)) == 0x1100;
    if (!realReplaySeed)
    {
        seed.assign(16, 0);
        std::memcpy(seed.data(), "T7RP", 4);
        if (!WriteWholeFile(replayPath, seed))
            return false;
    }

    struct Cleanup
    {
        const char *path;
        ~Cleanup() { std::remove(path); }
    } cleanup {replayPath};

    Config expected;
    expected.active = true;
    expected.mode = 1;
    expected.stage = 5;
    // warp is a persistent menu-only selector and is not serialized by
    // upstream THPracParam::GetJson().
    expected.warp = 0;
    // Use a catalog-backed section so the portable adapter validates the same
    // payload that replay playback will consume.
    expected.section = 6;
    expected.phase = 2;
    expected.frame = 23456;
    expected.dialogue = true;
    expected.score = 87654320;
    expected.life = 4;
    expected.bomb = 2;
    expected.power = 112;
    expected.graze = 2300;
    expected.point = 0;
    expected.pointTotal = 460;
    expected.pointStage = 47;
    expected.cherry = 12000;
    expected.cherryMax = 321000;
    expected.cherryPlus = 8000;
    expected.spellBonus = 6;
    expected.rank = 28;
    expected.rankLock = true;
    SetConfig(expected);
    if (!SaveReplayMetadata(replayPath))
        return false;

    if (realReplaySeed)
    {
        ReplayFile *rawReplay = reinterpret_cast<ReplayFile *>(FileSystem::OpenFile(replayPath, 1));
        const i32 replaySize = static_cast<i32>(g_LastFileSize);
        ReplayFile *validatedReplay = ReplayManager::ValidateReplayData(rawReplay, replaySize);
        if (!validatedReplay)
            return false;
        ReplayManager::FreeReplay(validatedReplay);
    }

    // THGuiRep ownership contract: State(2) may inspect the selected replay,
    // but must not mutate live thPracParam or claim playback ownership. Only
    // State(3) copies the candidate into the live runtime.
    Config sentinel;
    sentinel.active = true;
    sentinel.stage = 1;
    sentinel.section = 999;
    SetConfig(sentinel);
    g_ReplayPlaybackActive = false;
    if (!ReplayMenuCheck(replayPath))
        return false;
    if (g_ReplayPlaybackActive || g_Config.stage != sentinel.stage || g_Config.section != sentinel.section)
        return false;
    ReplayMenuActivate();
    if (!g_ReplayPlaybackActive)
        return false;
    const Config &menuActivated = GetConfig();
    if (!menuActivated.active || menuActivated.stage != expected.stage || menuActivated.section != expected.section ||
        menuActivated.phase != expected.phase || menuActivated.frame != expected.frame ||
        menuActivated.dialogue != expected.dialogue || menuActivated.rank != expected.rank ||
        menuActivated.rankLock != expected.rankLock)
        return false;

    ReplayMenuReset();
    Config stale;
    stale.active = true;
    stale.stage = 1;
    stale.section = 999;
    SetConfig(stale);
    if (!LoadReplayMetadata(replayPath))
        return false;
    const Config &actual = GetConfig();
    const bool replayRoundTripStable = actual.active == expected.active && actual.mode == expected.mode && actual.stage == expected.stage &&
        actual.warp == expected.warp && actual.section == expected.section && actual.phase == expected.phase &&
        actual.frame == expected.frame && actual.dialogue == expected.dialogue && actual.score == expected.score &&
        actual.life == expected.life && actual.bomb == expected.bomb && actual.power == expected.power &&
        actual.graze == expected.graze && actual.point == expected.point && actual.pointTotal == expected.pointTotal &&
        actual.pointStage == expected.pointStage && actual.cherry == expected.cherry &&
        actual.cherryMax == expected.cherryMax && actual.cherryPlus == expected.cherryPlus &&
        actual.spellBonus == expected.spellBonus && actual.rank == expected.rank &&
        actual.rankLock == expected.rankLock;
    if (!replayRoundTripStable)
        return false;

    // Regression for the reported cross-run degeneration: run the exact same
    // persistent THGuiPrac selection through three fresh Practice lifetimes.
    // State(1) must reset only live ownership, State(3) must republish the full
    // parameter block, and leaving the run must clear live C++/adapter state
    // without erasing the widget values used by the next round.
    const i32 savedPractice = g_GameManager.practice;
    const i32 savedReplay = g_GameManager.replay;
    Config repeatedMenu;
    repeatedMenu.mode = 1;
    repeatedMenu.stage = 0;
    repeatedMenu.warp = 5; // Spell
    repeatedMenu.section = 4;
    repeatedMenu.life = 5;
    repeatedMenu.bomb = 4;
    repeatedMenu.power = 96;
    repeatedMenu.rank = 18;
    g_MenuConfig = repeatedMenu;
    g_ReplayPlaybackActive = false;
    g_GameManager.replay = 0;
    g_GameManager.practice = 1;
    bool repeatedPracticeStable = true;
    for (i32 round = 0; round < 3 && repeatedPracticeStable; round++)
    {
        OpenPracticeMenu(1, 0);
        repeatedPracticeStable = !g_Config.active && g_Config.mode == repeatedMenu.mode &&
            g_Config.stage == repeatedMenu.stage && g_Config.warp == repeatedMenu.warp &&
            g_Config.power == repeatedMenu.power;
        if (!repeatedPracticeStable)
            break;

        DebugAcceptPracticeMenu();
        repeatedPracticeStable = g_Config.active && g_Config.mode == 1 && g_Config.stage == 0 &&
            g_Config.section == 4 && g_Config.life == 5 && g_Config.bomb == 4 && g_Config.power == 96;
        if (!repeatedPracticeStable)
            break;

        RefreshFromHost();
        repeatedPracticeStable = g_Config.active && g_Config.section == 4 && g_Config.power == 96;
        if (!repeatedPracticeStable)
            break;

        g_GameManager.practice = 0;
        RefreshFromHost();
        repeatedPracticeStable = !g_Config.active && g_MenuConfig.section == 4 && g_MenuConfig.power == 96;
        g_GameManager.practice = 1;
    }
    g_GameManager.practice = savedPractice;
    g_GameManager.replay = savedReplay;
    return repeatedPracticeStable;
#else
    (void)replayPath;
    return false;
#endif
}

} // namespace PracticeRuntime

#ifdef __EMSCRIPTEN__
#endif
