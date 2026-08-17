#include "ThpracImGui.hpp"

#ifdef TH_ENABLE_THPRAC

#include "Controller.hpp"
#include "FileSystem.hpp"
#include "GameWindow.hpp"
#include "Supervisor.hpp"
#include "imgui_freetype.h"
#include "utils.hpp"
#if defined(THPRAC_PORTABLE_ENABLED)
#include "section_catalog.hpp"
#endif

#include <SDL3/SDL.h>

#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <cfloat>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#if defined(_WIN32) && !defined(__EMSCRIPTEN__)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#endif

namespace ThpracImGui
{
namespace
{
bool g_Initialized = false;
bool g_FrameOpen = false;
std::uint16_t g_GameButtons = 0;
Locale g_Locale = Locale::EnUS;
float g_MouseWheel = 0.0f;
float g_MouseWheelH = 0.0f;
bool g_MouseButtons[5] = {};
float g_MouseClientX = 0.0f;
float g_MouseClientY = 0.0f;
bool g_MousePositionValid = false;

constexpr const char *g_Text[][3] = {
    {"练习选项", "Option", "オプション"},
    {"原版练习", "Original", "オリジナル"},
    {"自定义练习", "Custom", "カスタム"},
    {"模式", "Mode", "モード"},
    {"关卡", "Stage", "ステージ"},
    {"Extra", "Extra", "エクストラ"},
    {"传送", "Warp", "ワープ"},
    {"无", "None", "なし"},
    {"道中", "Stage Portion", "道中"},
    {"#%d", "#%d", "#%d"},
    {"道中Boss", "Mid Boss", "道中ボス"},
    {"关底Boss", "End Boss", "ボス"},
    {"非符", "Non Spell", "通常"},
    {"符卡", "Spell Card", "スペカ"},
    {"帧", "Frame", "フレーム"},
    {"机体伪装", "Fake Shot", "機体偽装"},
    {"梦A", "Reimu A", "霊符"},
    {"梦B", "Reimu B", "夢符"},
    {"魔A", "Marisa A", "魔符"},
    {"魔B", "Marisa B", "恋符"},
    {"章节", "Chapter", "チャプター"},
    {"前半 #%d", "First Half #%d", "前半 #%d"},
    {"后半 #%d", "Second Half #%d", "後半 #%d"},
    {"对话", "Dialog", "会話"},
    {"阶段", "Phase", "段階"},
    {"正常", "Normal", "普通"},
    {"发狂", "Rage", "発狂"},
    {"残机", "Life", "残機"},
    {"Bomb", "Bomb", "ボム"},
    {"分数", "Score", "スコア"},
    {"火力", "Power", "霊力"},
    {"擦弹", "Graze", "グレイズ"},
    {"蓝点", "Point", "得点"},
    {"Rank", "Rank", "ランク"},
    {"锁Rank", "Rank Lock", "ランクを固定する"},
    {"继续游戏", "Resume", "再開"},
    {"退出游戏", "Exit", "終了"},
    {"重新开始", "Restart", "リトライ"},
    {"调整选项", "Settings", "設定変更"},
    {"无敌", "Invincible", "無敵"},
    {"锁残", "Inf. Lives", "残機減らない"},
    {"锁Bomb", "Inf. Bombs", "ボム減らない"},
    {"锁火力", "Inf. Power", "霊力減らない"},
    {"锁时", "Time Lock", "残り時間減らない"},
    {"自动B", "Auto Bomb", "自動喰らいボム"},
    {"永续BGM", "Everlasting BGM", "永遠に続くBGM"},
    {"Miss", "Misses", "ミス"},
    {"Bomb", "Bombs", "ボム"},
};

Locale DetectLocale()
{
#ifdef __EMSCRIPTEN__
    const int locale = EM_ASM_INT({
        const value = Module.eaglerOptions?.thpracLocale;
        if (value === 'zh-CN') return 1;
        if (value === 'ja-JP') return 2;
        return 0;
    });
    if (locale == 1)
        return Locale::ZhCN;
    if (locale == 2)
        return Locale::JaJP;
    return Locale::EnUS;
#endif
#if defined(_WIN32) && !defined(__EMSCRIPTEN__)
    switch (GetUserDefaultUILanguage() & 0x03ff)
    {
    case 0x4:
        return Locale::ZhCN;
    case 0x11:
        return Locale::JaJP;
    case 0x9:
        return Locale::EnUS;
    default:
        break;
    }
#endif
    return Locale::EnUS;
}

#if defined(_WIN32) && !defined(__EMSCRIPTEN__)
struct FontSpec
{
    const wchar_t *name;
    BYTE charset;
    int fontIndex;
    float scale;
};

int CALLBACK FontPresentProc(const LOGFONTW *, const TEXTMETRICW *, DWORD, LPARAM signal)
{
    *reinterpret_cast<int *>(signal) = 1;
    return 0;
}

const FontSpec *FindSystemFont(Locale locale, HDC hdc)
{
    static const FontSpec zhFonts[] = {
        {L"Microsoft YaHei UI Light", GB2312_CHARSET, 1, 1.0f},
        {L"Microsoft YaHei Light", GB2312_CHARSET, 0, 1.0f},
        {L"Microsoft YaHei", GB2312_CHARSET, 0, 1.0f},
        {L"SimHei", GB2312_CHARSET, 0, 0.9f},
        {L"SimSun", GB2312_CHARSET, 0, 0.9f},
    };
    static const FontSpec enFonts[] = {
        {L"Segoe UI", ANSI_CHARSET, 0, 1.0f},
        {L"Tahoma", ANSI_CHARSET, 0, 0.88f},
    };
    static const FontSpec jaFonts[] = {
        {L"Yu Gothic UI", SHIFTJIS_CHARSET, 1, 1.0f},
        {L"Meiryo UI", SHIFTJIS_CHARSET, 0, 1.0f},
        {L"MS UI Gothic", SHIFTJIS_CHARSET, 0, 0.85f},
        {L"MS Mincho", SHIFTJIS_CHARSET, 0, 0.85f},
    };

    const FontSpec *fonts = enFonts;
    std::size_t count = sizeof(enFonts) / sizeof(enFonts[0]);
    if (locale == Locale::ZhCN)
    {
        fonts = zhFonts;
        count = sizeof(zhFonts) / sizeof(zhFonts[0]);
    }
    else if (locale == Locale::JaJP)
    {
        fonts = jaFonts;
        count = sizeof(jaFonts) / sizeof(jaFonts[0]);
    }

    for (std::size_t index = 0; index < count; index++)
    {
        LOGFONTW query = {};
        query.lfCharSet = fonts[index].charset;
        wcsncpy_s(query.lfFaceName, fonts[index].name, LF_FACESIZE - 1);
        int present = 0;
        EnumFontFamiliesExW(hdc, &query, FontPresentProc, reinterpret_cast<LPARAM>(&present), 0);
        if (present != 0)
            return &fonts[index];
    }
    return nullptr;
}

bool AddSystemFont(ImGuiIO &io, Locale locale)
{
    HDC hdc = CreateCompatibleDC(nullptr);
    if (hdc == nullptr)
        return false;

    const FontSpec *spec = FindSystemFont(locale, hdc);
    if (spec == nullptr)
    {
        DeleteDC(hdc);
        return false;
    }

    HFONT font = CreateFontW(0, 0, 0, 0, 0, FALSE, FALSE, FALSE, spec->charset, OUT_DEFAULT_PRECIS,
                             CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | VARIABLE_PITCH, spec->name);
    if (font == nullptr)
    {
        DeleteDC(hdc);
        return false;
    }
    HGDIOBJ previous = SelectObject(hdc, font);

    constexpr DWORD kTrueTypeCollection = 0x66637474; // 'ttcf'
    DWORD table = kTrueTypeCollection;
    DWORD fontDataSize = GetFontData(hdc, table, 0, nullptr, 0);
    if (fontDataSize == GDI_ERROR)
    {
        table = 0;
        fontDataSize = GetFontData(hdc, table, 0, nullptr, 0);
    }

    void *fontData = nullptr;
    if (fontDataSize != GDI_ERROR && fontDataSize != 0)
    {
        fontData = ImGui::MemAlloc(fontDataSize);
        if (fontData == nullptr || GetFontData(hdc, table, 0, fontData, fontDataSize) == GDI_ERROR)
        {
            if (fontData != nullptr)
                ImGui::MemFree(fontData);
            fontData = nullptr;
        }
    }
    SelectObject(hdc, previous);
    DeleteObject(font);
    DeleteDC(hdc);
    if (fontData == nullptr)
        return false;

    ImFontConfig config = {};
    config.FontNo = spec->fontIndex;
    config.RasterizerMultiply = 1.25f;
    config.OversampleH = 5;
    config.OversampleV = 5;
    config.FontDataOwnedByAtlas = true;

    const ImWchar *glyphRange = io.Fonts->GetGlyphRangesDefault();
    if (locale == Locale::ZhCN)
        // thprac v2.3.0.3 defaults render_only_used_glyphs=false, which uses
        // the complete Chinese range.  Restricting this to Dear ImGui's 2500
        // "common simplified" characters makes valid thprac labels fall back
        // to '?' even though the selected system font contains the glyph.
        glyphRange = io.Fonts->GetGlyphRangesChineseFull();
    else if (locale == Locale::JaJP)
        glyphRange = io.Fonts->GetGlyphRangesJapanese();

    ImFont *fontResult = io.Fonts->AddFontFromMemoryTTF(fontData, fontDataSize, 16.0f * spec->scale, &config,
                                                        glyphRange);
    if (fontResult == nullptr)
    {
        ImGui::MemFree(fontData);
        return false;
    }
    io.FontDefault = fontResult;
    return true;
}
#endif

bool BuildLocaleFont(ImGuiIO &io, Locale locale)
{
    io.Fonts->Clear();
    ImFontConfig config = {};
    config.RasterizerMultiply = 1.0f;
    config.OversampleH = 1;
    config.OversampleV = 1;
    const ImWchar *glyphRange = io.Fonts->GetGlyphRangesDefault();
    if (locale == Locale::ZhCN)
        glyphRange = io.Fonts->GetGlyphRangesChineseFull();
    else if (locale == Locale::JaJP)
        glyphRange = io.Fonts->GetGlyphRangesJapanese();
    const std::string fontPath = FileSystem::GetBasePath("unifont.otf");
    io.FontDefault = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), 16.0f, &config, glyphRange);
    if (io.FontDefault == nullptr)
        return false;

#if defined(_WIN32) && !defined(__EMSCRIPTEN__)
    if (!ImGuiFreeType::BuildFontAtlas(io.Fonts, 0))
        return false;
#else
    if (!io.Fonts->Build())
        return false;
#endif
    return true;
}

void ApplyGameInput()
{
    ImGuiIO &io = ImGui::GetIO();
    io.NavInputs[ImGuiNavInput_DpadUp] = (g_GameButtons & TH_BUTTON_UP) != 0 ? 1.0f : 0.0f;
    io.NavInputs[ImGuiNavInput_DpadDown] = (g_GameButtons & TH_BUTTON_DOWN) != 0 ? 1.0f : 0.0f;
    io.NavInputs[ImGuiNavInput_DpadLeft] = (g_GameButtons & TH_BUTTON_LEFT) != 0 ? 1.0f : 0.0f;
    io.NavInputs[ImGuiNavInput_DpadRight] = (g_GameButtons & TH_BUTTON_RIGHT) != 0 ? 1.0f : 0.0f;
    // Original GameGuiWnd::Update() deliberately exposes only D-pad input to
    // ImGui. Z/X still belong to TH06's vanilla Practice state, where the
    // thprac hooks translate them to State(3)=accept / State(4)=cancel. If we
    // also map them to Activate/Cancel here, one Z can both operate the focused
    // ImGui widget and start the run in PollPracticeMenu().
    io.NavInputs[ImGuiNavInput_Activate] = 0.0f;
    io.NavInputs[ImGuiNavInput_Cancel] = 0.0f;
}

void ApplyMouseInput()
{
    ImGuiIO &io = ImGui::GetIO();

    // Original thprac only requires the game to be the foreground window for
    // mouse position, while button state is latched immediately by Win32
    // DOWN/UP messages.  Do not gate either on SDL mouse focus: the click that
    // moves focus into the game is itself a valid first interaction.
    io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
    if (g_GameWindow.window != nullptr)
    {
        // Keep the cached client position fresh while SDL has mouse focus.
        // ProcessEvent() also updates it from motion/button event coordinates,
        // which is what preserves the focus-acquiring first click.
        if (SDL_GetMouseFocus() == g_GameWindow.window)
        {
            float mouseX = 0.0f;
            float mouseY = 0.0f;
            SDL_GetMouseState(&mouseX, &mouseY);
            g_MouseClientX = mouseX;
            g_MouseClientY = mouseY;
            g_MousePositionValid = true;
        }

        int windowWidth = 0;
        int windowHeight = 0;
        SDL_GetWindowSize(g_GameWindow.window, &windowWidth, &windowHeight);
        if (windowWidth > 0 && windowHeight > 0 && g_MousePositionValid &&
            SDL_GetKeyboardFocus() == g_GameWindow.window)
        {
            const float scaleX = static_cast<float>(windowWidth) / 640.0f;
            const float scaleY = static_cast<float>(windowHeight) / 480.0f;
            const float scale = std::min(scaleX, scaleY);
            const float renderWidth = 640.0f * scale;
            const float renderHeight = 480.0f * scale;
            const float renderX = (static_cast<float>(windowWidth) - renderWidth) * 0.5f;
            const float renderY = (static_cast<float>(windowHeight) - renderHeight) * 0.5f;

            if (scale > 0.0f && g_MouseClientX >= renderX && g_MouseClientX < renderX + renderWidth &&
                g_MouseClientY >= renderY && g_MouseClientY < renderY + renderHeight)
            {
                io.MousePos.x = (g_MouseClientX - renderX) / scale;
                io.MousePos.y = (g_MouseClientY - renderY) / scale;
            }
        }
    }

    for (int index = 0; index < 5; ++index)
        io.MouseDown[index] = g_MouseButtons[index];

    io.MouseWheel += g_MouseWheel;
    io.MouseWheelH += g_MouseWheelH;
    g_MouseWheel = 0.0f;
    g_MouseWheelH = 0.0f;
}

std::uint16_t BuildGameInputPulse(std::uint16_t buttons, std::uint16_t previousButtons,
                                  bool eighthFrameRepeat, bool sampled)
{
    if (!sampled)
        return 0;

    std::uint16_t pulse = 0;
    static constexpr std::uint16_t repeatedButtons[] = {
        TH_BUTTON_UP, TH_BUTTON_DOWN, TH_BUTTON_LEFT, TH_BUTTON_RIGHT,
    };
    for (const std::uint16_t button : repeatedButtons)
    {
        if ((buttons & button) != 0 && (((previousButtons & button) == 0) || eighthFrameRepeat))
            pulse |= button;
    }

    static constexpr std::uint16_t edgeButtons[] = {
        TH_BUTTON_SELECTMENU, TH_BUTTON_RETURNMENU, TH_BUTTON_FOCUS,
    };
    for (const std::uint16_t button : edgeButtons)
    {
        if ((buttons & button) != 0 && (previousButtons & button) == 0)
            pulse |= button;
    }
    return pulse;
}
}

bool Initialize()
{
    if (g_Initialized)
        return true;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    g_Locale = DetectLocale();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
    io.DisplaySize = ImVec2(640.0f, 480.0f);
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
    io.IniFilename = nullptr;
    io.BackendPlatformName = "eagler-th06-sdl3";
    io.BackendRendererName = "eagler-th06-gles3";
    ImGui::StyleColorsDark();

    // Match thprac's locale-specific 16px GDI/FreeType font contract. Dear
    // ImGui requires a built atlas before NewFrame(); the GLES backend uploads
    // the same CPU buffer on its first draw.
    if (!BuildLocaleFont(io, g_Locale))
    {
        ImGui::DestroyContext();
        return false;
    }
    unsigned char *pixels = nullptr;
    int width = 0;
    int height = 0;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
    if (pixels == nullptr || width <= 0 || height <= 0)
    {
        ImGui::DestroyContext();
        return false;
    }

    g_Initialized = true;
    return true;
}

void Shutdown()
{
    if (!g_Initialized)
        return;
    if (g_FrameOpen)
    {
        ImGui::EndFrame();
        g_FrameOpen = false;
    }
    ImGui::DestroyContext();
    g_Initialized = false;
}

bool IsInitialized()
{
    return g_Initialized;
}

Locale GetLocale()
{
    return g_Locale;
}

const char *Text(TextId id)
{
    const std::size_t index = static_cast<std::size_t>(id);
    constexpr std::size_t count = sizeof(g_Text) / sizeof(g_Text[0]);
    if (index >= count)
        return "";
    const std::size_t localeIndex = static_cast<std::size_t>(g_Locale);
    return g_Text[index][localeIndex < 3 ? localeIndex : 1];
}

void SetGameInput(std::uint16_t buttons, bool sampled)
{
    g_GameButtons = BuildGameInputPulse(buttons, g_LastFrameInput,
                                        g_IsEigthFrameOfHeldInput != 0, sampled);
    if (g_Initialized)
        ApplyGameInput();
}

bool InputPressed(std::uint16_t button)
{
    return (g_GameButtons & button) != 0;
}

void ProcessEvent(const SDL_Event &event)
{
    if (!g_Initialized)
        return;

    auto mouseButtonIndex = [](Uint8 button) -> int {
        switch (button)
        {
        case SDL_BUTTON_LEFT: return 0;
        case SDL_BUTTON_RIGHT: return 1;
        case SDL_BUTTON_MIDDLE: return 2;
        case SDL_BUTTON_X1: return 3;
        case SDL_BUTTON_X2: return 4;
        default: return -1;
        }
    };

    switch (event.type)
    {
    case SDL_EVENT_MOUSE_MOTION:
        g_MouseClientX = event.motion.x;
        g_MouseClientY = event.motion.y;
        g_MousePositionValid = true;
        break;
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP:
    {
        const int index = mouseButtonIndex(event.button.button);
        g_MouseClientX = event.button.x;
        g_MouseClientY = event.button.y;
        g_MousePositionValid = true;
        if (index >= 0)
            g_MouseButtons[index] = event.type == SDL_EVENT_MOUSE_BUTTON_DOWN;
        bool anyDown = false;
        for (const bool down : g_MouseButtons)
            anyDown = anyDown || down;
        // Match the original Win32 backend's SetCapture/ReleaseCapture. SDL's
        // auto-capture normally does this too, but requesting it explicitly
        // keeps drag/release semantics deterministic across platforms.
        SDL_CaptureMouse(anyDown);
        break;
    }
    case SDL_EVENT_MOUSE_WHEEL:
        g_MouseWheel += event.wheel.y;
        g_MouseWheelH += event.wheel.x;
        break;
    case SDL_EVENT_WINDOW_FOCUS_LOST:
    case SDL_EVENT_WILL_ENTER_BACKGROUND:
    case SDL_EVENT_DID_ENTER_BACKGROUND:
        for (bool &down : g_MouseButtons)
            down = false;
        g_MousePositionValid = false;
        SDL_CaptureMouse(false);
        break;
    default:
        break;
    }
}

void BeginFrame(float deltaSeconds)
{
    if (!Initialize() || g_FrameOpen)
        return;

    ImGuiIO &io = ImGui::GetIO();
    io.DisplaySize = ImVec2(640.0f, 480.0f);
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
    io.DeltaTime = std::max(deltaSeconds, 1.0f / 1000.0f);
    ApplyGameInput();
    ApplyMouseInput();
    ImGui::NewFrame();
    g_FrameOpen = true;
}

bool IsFrameOpen()
{
    return g_FrameOpen;
}

void EndFrame()
{
    if (!g_FrameOpen)
        return;
    ImGui::EndFrame();
    ImGui::Render();
    g_FrameOpen = false;
}

const ImDrawData *GetDrawData()
{
    return g_Initialized ? ImGui::GetDrawData() : nullptr;
}

bool DebugLifecycleSelfTest()
{
    if (!Initialize())
        return false;

    // The user-visible TH06 practice catalogue contains less-common Chinese
    // characters. Rebuild the atlas exactly as a Zh-CN thprac session would
    // and prove that every shipped Chinese label resolves to a real glyph,
    // rather than silently using ImFont::FallbackGlyph ('?').
    g_Locale = Locale::ZhCN;
    ImGuiIO &io = ImGui::GetIO();
    if (!BuildLocaleFont(io, g_Locale) || io.FontDefault == nullptr)
    {
        Shutdown();
        return false;
    }

    auto fontContainsUtf8 = [](const ImFont *font, const char *text) {
        if (font == nullptr || text == nullptr)
            return false;
        const unsigned char *cursor = reinterpret_cast<const unsigned char *>(text);
        while (*cursor != 0)
        {
            std::uint32_t codepoint = 0;
            std::size_t length = 0;
            if (cursor[0] < 0x80)
            {
                codepoint = cursor[0];
                length = 1;
            }
            else if ((cursor[0] & 0xe0) == 0xc0 && (cursor[1] & 0xc0) == 0x80)
            {
                codepoint = ((cursor[0] & 0x1f) << 6) | (cursor[1] & 0x3f);
                length = 2;
            }
            else if ((cursor[0] & 0xf0) == 0xe0 && (cursor[1] & 0xc0) == 0x80 &&
                     (cursor[2] & 0xc0) == 0x80)
            {
                codepoint = ((cursor[0] & 0x0f) << 12) | ((cursor[1] & 0x3f) << 6) | (cursor[2] & 0x3f);
                length = 3;
            }
            else if ((cursor[0] & 0xf8) == 0xf0 && (cursor[1] & 0xc0) == 0x80 &&
                     (cursor[2] & 0xc0) == 0x80 && (cursor[3] & 0xc0) == 0x80)
            {
                codepoint = ((cursor[0] & 0x07) << 18) | ((cursor[1] & 0x3f) << 12) |
                            ((cursor[2] & 0x3f) << 6) | (cursor[3] & 0x3f);
                length = 4;
            }
            else
            {
                return false;
            }
            if (codepoint >= 0x20 &&
                (codepoint > 0xffff || font->FindGlyphNoFallback(static_cast<ImWchar>(codepoint)) == nullptr))
                return false;
            cursor += length;
        }
        return true;
    };

    constexpr std::size_t textCount = sizeof(g_Text) / sizeof(g_Text[0]);
    for (std::size_t index = 0; index < textCount; ++index)
    {
        if (!fontContainsUtf8(io.FontDefault, g_Text[index][0]))
        {
            Shutdown();
            return false;
        }
    }
#if defined(THPRAC_PORTABLE_ENABLED)
    for (const auto &entry : thprac::portable::generated::th06SectionLabels)
    {
        if (!fontContainsUtf8(io.FontDefault, entry.zh))
        {
            Shutdown();
            return false;
        }
    }
#endif

    BeginFrame(1.0f / 60.0f);
    ImGui::SetNextWindowPos(ImVec2(16.0f, 16.0f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(240.0f, 96.0f), ImGuiCond_Always);
    ImGui::Begin("thprac lifecycle self-test", nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoSavedSettings);
    ImGui::TextUnformatted("same-process ImGui frame");
    ImGui::End();
    EndFrame();

    const ImDrawData *drawData = GetDrawData();
    const bool passed = drawData != nullptr && drawData->CmdListsCount > 0 &&
                        drawData->TotalVtxCount > 0 && drawData->TotalIdxCount > 0;
    Shutdown();
    return passed;
}


bool DebugInputSamplingSelfTest()
{
    constexpr std::uint16_t direction = TH_BUTTON_DOWN;
    constexpr std::uint16_t accept = TH_BUTTON_SELECTMENU;
    const bool pulseContract =
        BuildGameInputPulse(direction | accept, 0, false, true) == (direction | accept) &&
        BuildGameInputPulse(direction | accept, direction | accept, false, true) == 0 &&
        BuildGameInputPulse(direction | accept, direction | accept, true, true) == direction &&
        BuildGameInputPulse(direction | accept, 0, false, false) == 0;
    if (!pulseContract || !Initialize())
        return false;

    // Upstream GameGuiWnd sends only directional navigation into Dear ImGui;
    // menu-level Z/X must not become widget Activate/Cancel inputs.
    g_GameButtons = direction | TH_BUTTON_SELECTMENU | TH_BUTTON_RETURNMENU;
    ApplyGameInput();
    ImGuiIO &io = ImGui::GetIO();
    const bool inputOwnershipContract =
        io.NavInputs[ImGuiNavInput_DpadDown] == 1.0f &&
        io.NavInputs[ImGuiNavInput_Activate] == 0.0f &&
        io.NavInputs[ImGuiNavInput_Cancel] == 0.0f;
    if (!inputOwnershipContract)
    {
        Shutdown();
        return false;
    }

    auto drawNavigationFrame = [](std::uint16_t pulse, bool seedFocus) {
        g_GameButtons = pulse;
        ApplyGameInput();
        BeginFrame(1.0f / 60.0f);
        ImGui::SetNextWindowPos(ImVec2(16.0f, 16.0f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(240.0f, 180.0f), ImGuiCond_Always);
        ImGui::Begin("thprac navigation self-test", nullptr,
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                         ImGuiWindowFlags_NoSavedSettings);

        int focused = -1;
        for (int index = 0; index < 4; ++index)
        {
            char label[32] = {};
            std::snprintf(label, sizeof(label), "Row %d", index + 1);
            ImGui::Button(label, ImVec2(120.0f, 24.0f));
            if (index == 0 && seedFocus)
                ImGui::SetItemDefaultFocus();
            if (ImGui::IsItemFocused())
                focused = index;
        }
        ImGui::End();
        EndFrame();
        return focused;
    };

    // Dear ImGui resolves a navigation request after the current item pass,
    // so observe it on the following frame. One Gen1 Down pulse must advance
    // exactly one row, and two release frames must not continue moving.
    const int initial = drawNavigationFrame(0, true);
    const int pressFrame = drawNavigationFrame(direction, false);
    const int afterPress = drawNavigationFrame(0, false);
    const int afterRelease = drawNavigationFrame(0, false);

    g_GameButtons = 0;
    Shutdown();
    const bool navigationContract = pressFrame == 0 && afterPress == 1 && afterRelease == 1;
    if (!navigationContract)
    {
        std::fprintf(stderr,
                     "th06 thprac nav self-test focus: initial=%d press=%d after=%d release=%d\n",
                     initial, pressFrame, afterPress, afterRelease);
    }
    return navigationContract;
}
} // namespace ThpracImGui

#endif
