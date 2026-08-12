#include "PracticeRuntime.hpp"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <string>

#include "EnemyManager.hpp"
#include "AsciiManager.hpp"
#include "Controller.hpp"
#include "FileSystem.hpp"
#include "GameManager.hpp"
#include "ReplayManager.hpp"
#include "ScreenEffect.hpp"
#include "SoundPlayer.hpp"
#include "utils.hpp"
#if defined(THPRAC_PORTABLE_ENABLED)
#include "section_catalog.hpp"
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

namespace PracticeRuntime
{
static Config g_Config;
static bool g_MenuOpen = false;
static i32 g_MenuCursor = 0;
static i32 g_MenuDifficulty = 0;
static i32 g_MenuShotType = 0;
static i32 g_MenuSectionIndex = 0;
static constexpr i32 kMenuItemCount = 16;
static bool g_PauseWasOpen = false;
static bool g_PauseSettings = false;
static i32 g_PauseCursor = 0;
static bool g_PreserveConfigOnRestart = false;
static MenuResult g_MenuResult = MenuResult::Waiting;
#if !defined(__EMSCRIPTEN__)
static bool LoadDesktopSession();
#endif

template <typename T> static T Clamp(T value, T minimum, T maximum)
{
    return std::max(minimum, std::min(maximum, value));
}

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
        "\"point\":%d,\"rank\":%d,\"rankLock\":%s,\"fakeType\":%d}}",
        g_Config.mode, g_Config.stage, g_Config.warp, g_Config.section, g_Config.phase, g_Config.frame,
        g_Config.dialogue ? "true" : "false", static_cast<long long>(g_Config.score), g_Config.life,
        g_Config.bomb, g_Config.power, g_Config.graze, g_Config.point, g_Config.rank,
        g_Config.rankLock ? "true" : "false", g_Config.fakeType);
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
    g_Config.score = Clamp<std::int64_t>(g_Config.score, 0, 999999999);
    g_Config.life = Clamp(g_Config.life, 0, 8);
    g_Config.bomb = Clamp(g_Config.bomb, 0, 8);
    g_Config.power = Clamp(g_Config.power, 0, 128);
    g_Config.graze = Clamp(g_Config.graze, 0, 99999);
    g_Config.point = Clamp(g_Config.point, 0, 9999);
    g_Config.rank = Clamp(g_Config.rank, 0, 99);
    g_Config.fakeType = Clamp(g_Config.fakeType, 0, 4);
}

void RefreshFromHost()
{
    if (g_PreserveConfigOnRestart)
    {
        g_PreserveConfigOnRestart = false;
        return;
    }
#ifdef __EMSCRIPTEN__
    const bool active = EM_ASM_INT({
        const session = Module.eaglerOptions?.thpracSession;
        return !!session && session.game === 'th06' &&
            (session.schema === 'eagler-touhou/thprac-session/1' ||
             session.schema === 'eagler-touhou/thprac-replay/1');
    }) != 0;
    if (!active)
    {
        g_Config = {};
        return;
    }
    Config config;
    config.active = true;
    config.mode = static_cast<i32>(HostNumber("mode", 1));
    config.stage = static_cast<i32>(HostNumber("stage", 0));
    config.warp = static_cast<i32>(HostNumber("warp", 0));
    config.section = static_cast<i32>(HostNumber("section", 0));
    config.phase = static_cast<i32>(HostNumber("phase", 0));
    config.frame = static_cast<i32>(HostNumber("frame", 0));
    config.dialogue = HostBool("dlg", false);
    config.score = static_cast<std::int64_t>(HostNumber("score", 0));
    config.life = static_cast<i32>(HostNumber("life", 8));
    config.bomb = static_cast<i32>(HostNumber("bomb", 8));
    config.power = static_cast<i32>(HostNumber("power", 128));
    config.graze = static_cast<i32>(HostNumber("graze", 0));
    config.point = static_cast<i32>(HostNumber("point", 0));
    config.rank = static_cast<i32>(HostNumber("rank", 32));
    config.rankLock = HostBool("rankLock", false);
    config.fakeType = static_cast<i32>(HostNumber("fakeType", 0));
    SetConfig(config);
#else
    LoadDesktopSession();
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

bool Enabled()
{
#ifdef __EMSCRIPTEN__
    return EM_ASM_INT({ return !!Module.eaglerOptions?.thpracEnabled; }) != 0;
#else
    SDL_IOStream *file = FileSystem::OpenFileStream("thprac-session.json", "rb");
    if (!file)
        return false;
    SDL_CloseIO(file);
    return true;
#endif
}

void OpenPracticeMenu(i32 difficulty, i32 shotType)
{
#ifdef __EMSCRIPTEN__
    RefreshFromHost();
#else
    LoadDesktopSession();
#endif
    if (!g_Config.active)
    {
        g_Config = {};
        g_Config.mode = 1;
        g_Config.life = 8;
        g_Config.bomb = 8;
        g_Config.power = 128;
        g_Config.rank = 32;
    }
    g_MenuDifficulty = Clamp(difficulty, 0, 5);
    g_MenuShotType = shotType;
    g_MenuCursor = 0;
    g_MenuSectionIndex = 0;
    g_MenuOpen = true;
    g_MenuResult = MenuResult::Waiting;
}

static const thprac::portable::generated::SectionLabel *CurrentSection(i32 offset = 0)
{
#if defined(THPRAC_PORTABLE_ENABLED)
    using namespace thprac::portable::generated;
    const std::uint32_t difficultyBit = 1u << Clamp(g_MenuDifficulty, 0, 5);
    const SectionLabel *matches[160] = {};
    i32 count = 0;
    i32 lastId = -1;
    for (const SectionLabel &entry : th06SectionLabels)
    {
        if (entry.stage != g_Config.stage || !(entry.difficultyMask & difficultyBit) || entry.id == lastId)
            continue;
        if ((g_Config.warp == 2 && entry.bgm != 0) || (g_Config.warp == 3 && entry.bgm != 1) ||
            (g_Config.warp == 4 && entry.spell != 0) || (g_Config.warp == 5 && entry.spell != 1))
            continue;
        matches[count++] = &entry;
        lastId = entry.id;
    }
    if (count == 0)
        return nullptr;
    g_MenuSectionIndex = (g_MenuSectionIndex + offset + count) % count;
    const SectionLabel *selected = matches[g_MenuSectionIndex];
    g_Config.section = selected->id;
    g_Config.dialogue = selected->dialogue && g_Config.dialogue;
    return selected;
#else
    (void)offset;
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

static void AdjustMenuValue(i32 item, i32 direction)
{
    switch (item)
    {
    case 0: g_Config.mode = Clamp(g_Config.mode + direction, 0, 1); break;
    case 1: g_Config.stage = Clamp(g_Config.stage + direction, 0, 6); g_MenuSectionIndex = 0; break;
    case 2: g_Config.warp = Clamp(g_Config.warp + direction, 0, 6); g_MenuSectionIndex = 0; break;
    case 3: CurrentSection(direction); break;
    case 4: g_Config.phase = Clamp(g_Config.phase + direction, 0, 64); break;
    case 5: g_Config.dialogue = !g_Config.dialogue; break;
    case 6: g_Config.life = Clamp(g_Config.life + direction, 0, 8); break;
    case 7: g_Config.bomb = Clamp(g_Config.bomb + direction, 0, 8); break;
    case 8: g_Config.power = Clamp(g_Config.power + direction * 8, 0, 128); break;
    case 9: g_Config.score = Clamp<std::int64_t>(g_Config.score + direction * 1000000LL, 0, 999999999); break;
    case 10: g_Config.graze = Clamp(g_Config.graze + direction * 100, 0, 99999); break;
    case 11: g_Config.point = Clamp(g_Config.point + direction * 10, 0, 9999); break;
    case 12: g_Config.rank = Clamp(g_Config.rank + direction, 0, 99); break;
    case 13: g_Config.rankLock = !g_Config.rankLock; break;
    case 14: g_Config.fakeType = Clamp(g_Config.fakeType + direction, 0, 4); break;
    }
    CurrentSection();
}

MenuResult PollPracticeMenu()
{
    if (g_MenuOpen)
    {
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
            g_MenuOpen = false;
            g_SoundPlayer.PlaySoundByIdx(SOUND_BACK);
            return MenuResult::Cancelled;
        }
        else if (WAS_PRESSED(TH_BUTTON_SELECTMENU))
        {
            if (VisibleMenuItem(g_MenuCursor) == kMenuItemCount - 1)
            {
                g_Config.active = true;
                CurrentSection();
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
    if (!g_MenuOpen)
        return;
    const auto *section = CurrentSection();
    const char *sectionName = section ? section->en : "Whole stage";
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
}

void DebugAcceptPracticeMenu()
{
    if (!g_MenuOpen)
        return;
    g_Config.active = true;
    CurrentSection();
    g_MenuOpen = false;
    g_MenuResult = MenuResult::Accepted;
}

bool UpdatePauseMenu()
{
    if (!Active() || !g_GameManager.isInGameMenu || g_GameManager.isInReplay)
    {
        g_PauseWasOpen = false;
        return false;
    }
    if (!g_PauseWasOpen)
    {
        g_PauseWasOpen = true;
        g_PauseSettings = false;
        g_PauseCursor = 0;
    }

    if (g_PauseSettings)
    {
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
                g_GameManager.isInGameMenu = 0;
                g_PauseWasOpen = false;
                break;
            case 1: // Exit through Result Screen so the run can be saved.
                g_GameManager.isInGameMenu = 0;
                g_GameManager.guiScore = g_GameManager.score;
                g_Supervisor.curState = SUPERVISOR_STATE_RESULTSCREEN_FROMGAME;
                g_PauseWasOpen = false;
                break;
            case 2: // Restart with the current advanced-practice parameters.
                g_GameManager.isInGameMenu = 0;
#ifdef __EMSCRIPTEN__
                PublishConfigToHost();
#endif
                g_PreserveConfigOnRestart = true;
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
}

void DrawPauseMenuPanel()
{
    if (!Active() || !g_GameManager.isInGameMenu)
        return;
    ZunRect panel {56.0f, 80.0f, 392.0f, 400.0f};
    ScreenEffect::DrawSquare(&panel, 0xff181820);
}

void PrepareStart(GameManager &gameManager)
{
    if (!Active() || gameManager.isInReplay)
        return;
    gameManager.currentStage = g_Config.stage;
    gameManager.isInPracticeMode = g_Config.stage < 6;
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
    if (frame == 0)
        frame = ResolveWarpFrame(g_Config.stage, g_Config.warp);
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

static bool LoadConfigJson(const std::string &json)
{
    if (json.find("\"schema\":\"thprac/portable-replay/1\"") == std::string::npos &&
        json.find("\"schema\":\"eagler-touhou/thprac-replay/1\"") == std::string::npos &&
        json.find("\"schema\":\"eagler-touhou/thprac-session/1\"") == std::string::npos)
        return false;
    if (json.find("\"game\":\"th06\"") == std::string::npos)
        return false;

    Config config;
    config.active = true;
    config.mode = static_cast<i32>(JsonNumber(json, "mode", 1));
    config.stage = static_cast<i32>(JsonNumber(json, "stage", 0));
    config.warp = static_cast<i32>(JsonNumber(json, "warp", 0));
    config.section = static_cast<i32>(JsonNumber(json, "section", 0));
    config.phase = static_cast<i32>(JsonNumber(json, "phase", 0));
    config.frame = static_cast<i32>(JsonNumber(json, "frame", 0));
    config.dialogue = JsonBool(json, "dlg", false);
    config.score = static_cast<std::int64_t>(JsonNumber(json, "score", 0));
    config.life = static_cast<i32>(JsonNumber(json, "life", 8));
    config.bomb = static_cast<i32>(JsonNumber(json, "bomb", 8));
    config.power = static_cast<i32>(JsonNumber(json, "power", 128));
    config.graze = static_cast<i32>(JsonNumber(json, "graze", 0));
    config.point = static_cast<i32>(JsonNumber(json, "point", 0));
    config.rank = static_cast<i32>(JsonNumber(json, "rank", 32));
    config.rankLock = JsonBool(json, "rankLock", false);
    config.fakeType = static_cast<i32>(JsonNumber(json, "fakeType", 0));
    SetConfig(config);
    return true;
}

#if !defined(__EMSCRIPTEN__)
#if defined(THPRAC_PORTABLE_ENABLED)
extern "C" bool ThpracPortableTh06SetSessionJson(const char *json);
#endif

static bool LoadDesktopSession()
{
    u8 *bytes = FileSystem::OpenPath("thprac-session.json", 1);
    if (!bytes)
    {
        g_Config = {};
        return false;
    }
    const std::string json(reinterpret_cast<char *>(bytes), g_LastFileSize);
    std::free(bytes);
    if (!LoadConfigJson(json))
    {
        g_Config = {};
        return false;
    }
#if defined(THPRAC_PORTABLE_ENABLED)
    if (!ThpracPortableTh06SetSessionJson(json.c_str()))
    {
        g_Config = {};
        return false;
    }
#endif
    return true;
}
#endif

bool LoadReplayMetadata(const char *replayPath)
{
    g_Config = {};
#if !defined(__EMSCRIPTEN__) && defined(THPRAC_PORTABLE_ENABLED)
    ThpracPortableTh06SetSessionJson(nullptr);
#endif
    if (!replayPath || !*replayPath)
        return false;
    const std::string sidecar = std::string(replayPath) + ".thprac.json";
    u8 *bytes = FileSystem::OpenPath(sidecar.c_str(), 1);
    if (!bytes)
        return false;
    const std::string json(reinterpret_cast<char *>(bytes), g_LastFileSize);
    std::free(bytes);
    if (!LoadConfigJson(json))
        return false;
#ifdef __EMSCRIPTEN__
    PublishConfigToHost();
#endif
#if !defined(__EMSCRIPTEN__) && defined(THPRAC_PORTABLE_ENABLED)
    if (!ThpracPortableTh06SetSessionJson(json.c_str()))
    {
        g_Config = {};
        return false;
    }
#endif
    return true;
}

bool SaveReplayMetadata(const char *replayPath)
{
    if (!Active() || !replayPath || !*replayPath)
        return false;
    const std::string sidecar = std::string(replayPath) + ".thprac.json";
    char json[2048];
    const int length = std::snprintf(json, sizeof(json),
        "{\"schema\":\"eagler-touhou/thprac-replay/1\",\"game\":\"th06\",\"source\":\"advanced-practice\","
        "\"params\":{\"mode\":%d,\"stage\":%d,\"warp\":%d,\"section\":%d,\"phase\":%d,\"frame\":%d,"
        "\"dlg\":%s,\"score\":%lld,\"life\":%d,\"bomb\":%d,\"power\":%d,\"graze\":%d,\"point\":%d,"
        "\"rank\":%d,\"rankLock\":%s,\"fakeType\":%d}}\n",
        g_Config.mode, g_Config.stage, g_Config.warp, g_Config.section, g_Config.phase, g_Config.frame,
        g_Config.dialogue ? "true" : "false", static_cast<long long>(g_Config.score), g_Config.life,
        g_Config.bomb, g_Config.power, g_Config.graze, g_Config.point, g_Config.rank,
        g_Config.rankLock ? "true" : "false", g_Config.fakeType);
    if (length <= 0 || static_cast<size_t>(length) >= sizeof(json))
        return false;
    SDL_IOStream *file = FileSystem::OpenFileStream(sidecar.c_str(), "wb");
    if (!file)
        return false;
    const bool written = SDL_WriteIO(file, json, static_cast<size_t>(length)) == static_cast<size_t>(length);
    SDL_CloseIO(file);
    return written;
}

bool DebugReplayMetadataRoundTrip(const char *replayPath)
{
#if !defined(__EMSCRIPTEN__)
    Config expected;
    expected.active = true;
    expected.mode = 1;
    expected.stage = 4;
    expected.warp = 5;
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
    if (!SaveReplayMetadata(replayPath))
        return false;

    Config stale;
    stale.active = true;
    stale.stage = 1;
    stale.section = 999;
    SetConfig(stale);
    if (!LoadReplayMetadata(replayPath))
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
    const Config &actual = GetConfig();
    return actual.active && actual.mode == expected.mode && actual.stage == expected.stage &&
        actual.section == expected.section && actual.life == expected.life && actual.power == expected.power &&
        actual.fakeType == expected.fakeType && !g_PreserveConfigOnRestart;
#else
    return false;
#endif
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
    SDL_IOStream *file = FileSystem::OpenFileStream(path, "rb");
    if (!file)
        return 0;
    SDL_CloseIO(file);
    return 1;
}
#endif
