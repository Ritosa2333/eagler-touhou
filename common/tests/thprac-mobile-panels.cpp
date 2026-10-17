#include <eagler/thprac/PracticeMobilePanels.hpp>
#include <imgui_internal.h>
#include <cassert>
#include <cstdio>
namespace m = eagler::thprac::mobile;
static ImVec2 header;
static ImVec2 cheat_box;
static bool cheat = false;
static int bodies[3]{};
static ImVec2 header_sizes[3];
static void controls(bool enabled, bool visible) {
    EM_ASM({
        Module.eaglerControls = {};
        Module.eaglerControls.thpracMobilePanelsEnabled = !!$0;
        Module.eaglerControls.thpracMobilePanelsVisible = !!$1;
    }, enabled, visible);
    m::poll();
}
static void frame(bool click = false) {
    auto& io = ImGui::GetIO();
    io.DisplaySize = {640, 480}; io.DeltaTime = 1.f / 60;
    io.MouseDown[0] = click;
    ImGui::NewFrame();
    for (int i = 0; i < 3; ++i) {
        auto panel = static_cast<m::Panel>(i);
        if (!m::show(panel, false)) continue;
        ImGui::SetNextWindowPos({float(i * 210), 10}, ImGuiCond_Always);
        ImGui::SetNextWindowSize({200, 300}, ImGuiCond_Always);
        const char* ids[]{"Cheats###c", "Advanced###a", "Info###i"};
        const bool content = m::begin(panel, ids[i], ImGuiWindowFlags_NoMouseInputs, 1);
        if (!content) header_sizes[i] = ImGui::GetItemRectSize();
        if (i == 1) {
            const auto pos = ImGui::GetWindowPos();
            const auto padding = ImGui::GetStyle().WindowPadding;
            header = {pos.x + padding.x + 20, pos.y + padding.y + 14};
        }
        if (content) {
            ++bodies[i];
            if (i == 0) {
                m::checkbox("F1", "Invincibility", cheat);
                const auto a = ImGui::GetItemRectMin(), b = ImGui::GetItemRectMax();
                cheat_box = {(a.x + b.x) * .5f, (a.y + b.y) * .5f};
            }
            ImGui::TextUnformatted("Original content");
        }
        ImGui::End();
    }
    ImGui::Render();
}
int main() {
    ImGui::CreateContext();
    auto& io = ImGui::GetIO(); io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
    unsigned char* pixels; int w, h;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &w, &h);
    controls(true, false); frame();
    assert(!m::show(m::Cheats, true));
    controls(true, true); frame(); frame();
    assert(!bodies[0] && !bodies[1] && !bodies[2]);
    assert(!m::captures_advanced(true));
    for (auto* window : ImGui::GetCurrentContext()->Windows)
        if (window->Active) {
            assert(!(window->Flags & ImGuiWindowFlags_NoMouseInputs));
            assert(window->Flags & ImGuiWindowFlags_NoBackground);
            assert((window->Flags & ImGuiWindowFlags_NoNav) == ImGuiWindowFlags_NoNav);
            assert(window->WindowPadding.x == 0 && window->WindowPadding.y == 0);
            assert(window->WindowBorderSize == 0);
            assert(window->Size.y < 80);
        }
    auto* cheats = ImGui::FindWindowByName("Cheat menu###c");
    auto* advanced = ImGui::FindWindowByName("Advanced settings###a");
    assert(cheats && advanced && cheats->Pos.x == 8);
    assert(cheats->Pos.y == 8 && advanced->Pos.y == 8);
    assert(header_sizes[0].x == header_sizes[1].x && header_sizes[0].y == header_sizes[1].y);
    assert(advanced->Pos.x == cheats->Pos.x + header_sizes[0].x + 4);
    ImGui::FocusWindow(advanced);
    for (int n = 0; n < 12; ++n) {
        io.NavInputs[ImGuiNavInput_DpadDown] = n % 2 == 0 ? 1.f : 0.f;
        io.NavInputs[ImGuiNavInput_Activate] = n % 3 == 0 ? 1.f : 0.f;
        frame();
        assert(!m::expanded[m::Cheats] && !m::expanded[m::Advanced] && !m::expanded[m::Information]);
    }
    for (auto& input : io.NavInputs) input = 0.f;
    io.MousePos = header; frame(true); frame(false); frame();
    assert(m::expanded[m::Advanced] && bodies[1]);
    assert(!bodies[0] && !bodies[2]);
    assert(m::captures_advanced(false));
    io.MousePos = header; frame(true); frame(false); frame();
    assert(!m::expanded[m::Advanced] && !m::captures_advanced(true));
    io.MousePos = {20, 30}; frame(true); frame(false); frame();
    assert(m::expanded[m::Cheats]);
    io.MousePos = cheat_box; frame(true); frame(false); frame();
    assert(cheat);
    controls(true, false); controls(true, true); frame();
    assert(!m::expanded[m::Advanced] && !m::expanded[m::Cheats] && cheat);
    controls(false, false);
    assert(m::show(m::Cheats, true) && !m::show(m::Cheats, false));
    assert(m::captures_advanced(true) && !m::captures_advanced(false));
    ImGui::DestroyContext();
    std::puts("PASS mobile panels: collapse, tap, reopen, pointer flags, desktop fallback");
}
