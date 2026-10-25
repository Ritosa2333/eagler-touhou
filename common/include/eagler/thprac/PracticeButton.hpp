#pragma once
#include <imgui.h>

namespace eagler::thprac {
// This workspace's ImGui extension takes the same condition at both ends.
inline bool button(const char* label,bool enabled=true){
    const bool disabled=!enabled;
    ImGui::BeginDisabled(disabled);
    const bool clicked=ImGui::Button(label);
    ImGui::EndDisabled(disabled);
    return clicked;
}
}
