// Ported from purple thprac thprac_igi_key_render.h, revision 17780056107a014e88d35fceb3ad86c121d94287.
#pragma once
#include "../../third_party/imgui/imgui.h"
#include <cstdint>
#include <vector>
namespace th5w { namespace practice {
struct KeyRectStyle {
    bool separated = true;
    bool show_aps = true;
    uint32_t fill_color_press = 0xFFFFFFFF;
    uint32_t fill_color_release = 0xFFFFFFFF;
    uint32_t border_color_press = 0xFFFF4444;
    uint32_t border_color_release = 0xFFFFCCCC;
    uint32_t text_color_press = 0xFFFFFFFF;
    uint32_t text_color_release = 0xFFFFFFFF;
    ImVec2 padding = { 0.05f, 0.05f };
    ImVec2 size = { 34.0f, 34.0f };
    int type = 2;
};
void RecordKey(int ver, uint32_t cur_key);
void KeysHUD(int ver, ImVec2 render_pos_arrow, ImVec2 render_pos_key, const KeyRectStyle& style, bool align_right_arrow = true, bool align_right_key = false); // assume game window's 1280*960
bool SaveKeyRecorded();
extern bool g_record_key_aps;
int CurrentKeyAPS();
void ClearKeyRecord();
std::vector<int>& GetKeyAPS();
} }
