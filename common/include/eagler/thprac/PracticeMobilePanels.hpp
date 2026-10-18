// Mobile presentation adapter. Gameplay/cheat ownership stays in each title.
#pragma once
#include <cstdint>
#include <imgui.h>
#include <imgui_internal.h>
#include <cstdio>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace eagler::thprac::mobile {
enum Panel { Cheats, Advanced, Information, Keyboard, Indicators, Result, PanelCount };
inline bool enabled = false, visible = false;
inline bool expanded[PanelCount]{};

inline void reset() {
    enabled = visible = false;
    for (auto& value : expanded) value = false;
}
inline void update(bool mode, bool open) {
    if (mode != enabled || open != visible)
        for (auto& value : expanded) value = false;
    enabled = mode;
    visible = mode && open;
}
inline void poll() {
#ifdef __EMSCRIPTEN__
    const int bits = EM_ASM_INT({
        const c = Module.eaglerControls;
        return (c?.thpracMobilePanelsEnabled === true ? 1 : 0) |
               (c?.thpracMobilePanelsVisible === true ? 2 : 0);
    });
    update((bits & 1) != 0, (bits & 2) != 0);
#endif
}
inline bool show(Panel, bool desktopVisible) {
    poll();
    return enabled ? visible : desktopVisible;
}
inline bool captures_advanced(bool desktopOpen) {
    poll();
    return enabled ? visible && expanded[Advanced] : desktopOpen;
}
inline bool begin(Panel panel, const char* original, ImGuiWindowFlags flags, int locale) {
    if (!enabled) return ImGui::Begin(original, nullptr, flags);
    static const char* names[PanelCount][3] = {
        {"作弊菜单", "Cheat menu", "チートメニュー"},
        {"高级设置", "Advanced settings", "詳細設定"},
        {"高级状态信息", "Advanced status", "詳細情報"},
        {"按键显示", "Key display", "キー表示"},
        {"辅助状态", "Tool indicators", "補助状態"},
        {"测试结果", "Test result", "テスト結果"},
    };
    if (locale < 0 || locale > 2) locale = 0;
    // Retain the original ### ID: desktop and mobile share the same widgets.
    const char* id = original;
    for (const char* p = original; p[0]; ++p)
        if (p[0] == '#' && p[1] == '#' && p[2] == '#') { id = p; break; }
    char title[160];
    std::snprintf(title, sizeof title, "%s%s", names[panel][locale], id);
    const bool open = expanded[panel];
    // Use the base font, not a previous window's content scale, so both
    // top-left headers have identical dimensions in every language.
    const float font_size = ImGui::GetFont()->FontSize * ImGui::GetIO().FontGlobalScale;
    float pair_width = 120.f;
    for (int i = Cheats; i <= Advanced; ++i) {
        char text[128];
        std::snprintf(text, sizeof text, "+ %s", names[i][locale]);
        const float width = ImGui::GetFont()->CalcTextSizeA(font_size, 10000.f, 0.f, text).x + 20.f;
        if (width > pair_width) pair_width = width;
    }
    if (!open) {
        ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_Always);
        if (panel == Cheats) ImGui::SetNextWindowPos(ImVec2(8, 8), ImGuiCond_Always);
        if (panel == Advanced) ImGui::SetNextWindowPos(ImVec2(8 + pair_width + 4, 8), ImGuiCond_Always);
        flags |= ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoFocusOnAppearing;
        flags &= ~ImGuiWindowFlags_HorizontalScrollbar;
    }
    // Enable touch without undoing the caller's keyboard-navigation policy.
    flags &= ~(ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoScrollWithMouse);
    flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
    // A collapsed panel is just its button: no outer background, border or
    // padding. Push only around Begin so expanded/native widget styles stay
    // untouched, including callers that already set their own window style.
    if (!open) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
    }
    const bool content = ImGui::Begin(title, nullptr, flags);
    if (!open) ImGui::PopStyleVar(3);
    if (!content) return false;
    char heading[128];
    std::snprintf(heading, sizeof heading, "%s %s", open ? "-" : "+", names[panel][locale]);
    const float previous_scale = ImGui::GetFontSize() / font_size;
    ImGui::SetWindowFontScale(1.f);
    const bool framed = panel == Cheats || panel == Advanced || panel == Information;
    if (framed) {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 1));
        ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(0, 0, 0, 0));
    }
    // Fold/unfold is pointer-only, even when the native expanded body supports
    // navigation. Game-menu swipes/confirm must never activate this header.
    ImGui::PushItemFlag(ImGuiItemFlags_NoNav, true);
    if (ImGui::Button(heading, ImVec2(panel == Cheats || panel == Advanced ? pair_width : 0.f, 28))) expanded[panel] = !open;
    ImGui::PopItemFlag();
    if (framed) { ImGui::PopStyleColor(2); ImGui::PopStyleVar(); }
    ImGui::SetWindowFontScale(previous_scale);
    // Layout changes on the next frame, never draw full content into the old
    // collapsed rectangle or let an opening tap activate a cheat underneath.
    if (!open || !expanded[panel]) return false;
    ImGui::Separator();
    return true;
}
inline bool checkbox(const char* key, const char* text, bool& value) {
    char name[256];
    std::snprintf(name, sizeof name, "%s: %s", key, text);
    return ImGui::Checkbox(name, &value);
}
// Draw-list tools retain their original geometry and simulation cadence. Only
// their presentation is gated; the small header itself is a native touch target.
inline bool draw_tool(Panel panel, const char* id, ImVec2 pos, int locale) {
    poll();
    if (!enabled) return true;
    if (!visible) return false;
    ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_Always);
    const bool body = begin(panel, id, ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize, locale);
    ImGui::End();
    return body;
}
} // namespace eagler::thprac::mobile
