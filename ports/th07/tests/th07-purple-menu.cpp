#include <cstdint>
#include "PurpleHotkey.hpp"
#include "PurplePlatform.hpp"
#include <imgui_internal.h>
#include <cassert>
#include <cstdio>
static ThpracImGui::Locale fixtureLocale = ThpracImGui::Locale::ZhCN;
ThpracImGui::Locale ThpracImGui::GetLocale() { return fixtureLocale; }

int main()
{
    ImGui::CreateContext();
    auto& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(640, 480);
    io.DeltaTime = 1.0f / 60.0f;
    io.IniFilename = nullptr;
    io.Fonts->AddFontDefault();
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
    bool status = false;
    ImVec2 target;
    for (int frame = 0; frame < 12; ++frame)
    {
        io.MousePos = frame >= 2 ? target : ImVec2(-100, -100);
        io.MouseDown[0] = frame == 2 || frame == 7;
        ImGui::NewFrame();
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
        ImGui::Begin("Mod Menu", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        const int colors = GImGui->ColorStack.Size;
        const int flags = GImGui->ItemFlagsStack.Size;
        PracticeRuntime::DrawPurpleHotkey("U", "Enemy Invincible", status);
        assert(GImGui->ColorStack.Size == colors);
        assert(GImGui->ItemFlagsStack.Size == flags);
        const auto lo = ImGui::GetItemRectMin(), hi = ImGui::GetItemRectMax();
        target = ImVec2((lo.x + hi.x) * .5f, (lo.y + hi.y) * .5f);
        assert(hi.x > lo.x && hi.y > lo.y);
        if (frame == 3) assert(status);
        if (frame == 8) assert(!status);
        ImGui::End();
        ImGui::Render();
        assert(ImGui::GetDrawData()->Valid);
    }
    auto& monitor = PracticeRuntime::key_monitor;
    PracticeRuntime::THGuiTestReactionTest reaction;
    for (int locale = 0; locale < 3; ++locale)
    {
        reaction.Reset();
        ImGui::NewFrame();
        ImGui::Begin("Reaction test");
        fixtureLocale = static_cast<ThpracImGui::Locale>(locale);
        assert(reaction.GuiUpdate(true));
        ImGui::End();
        ImGui::Render();
        assert(ImGui::GetDrawData()->Valid);
    }
    std::puts("TH07 native reaction cold-open/reset/locales draw PASS");
    monitor.g_record_key_aps = true;
    monitor.record(7, 0);
    monitor.record(7, 0x11);
    assert(monitor.g_keys_down[eagler::thprac::Key_Up]);
    assert(monitor.g_keys_down[eagler::thprac::Key_Z]);
    assert(!monitor.g_key_mask[eagler::thprac::Key_C]);
    assert(monitor.g_aps_cur == 2);
    monitor.record(7, 0x11);
    assert(monitor.g_aps_cur == 2);
    monitor.record(7, 0);
    assert(monitor.g_aps_cur == 4);
    for (int tick = 0; tick < 62; ++tick) monitor.record(7, 0);
    assert(monitor.g_aps_cur == 0 && monitor.keys_per_sec.size() == 60);
    assert(monitor.csv().find("frame,aps,up,down,left,right,Z,X,C,D,Ctrl,Shift\n") == 0);
    ImGui::NewFrame();
    PracticeRuntime::KeysHUD(7, ImVec2(1280, 0), ImVec2(833, 0), PracticeRuntime::KeyRectStyle{}, true, false);
    ImGui::Render();
    assert(ImGui::GetDrawData()->Valid && ImGui::GetDrawData()->TotalVtxCount > 0);
    monitor.clear_record();
    assert(monitor.g_recorded_keys.empty() && monitor.g_recorded_aps.empty());
    ImGui::DestroyContext();
    std::puts("TH07 native purple HUD: key mapping, 60-tick APS, CSV, actual ImGui draw PASS");
    std::puts("TH07 native purple hotkey: cold open, click, toggle, reopen stacks PASS");
}
