#pragma once
#include <imgui.h>
#include <imgui_internal.h>
#include <algorithm>

namespace eagler::thprac::navigation {
inline bool busy() {
    return ImGui::IsAnyItemActive() || ImGui::IsPopupOpen(nullptr,
        ImGuiPopupFlags_AnyPopupId | ImGuiPopupFlags_AnyPopupLevel);
}
// Call after filling NavInputs, before NewFrame. Confirm starts practice;
// activated mouse widgets and popup menus retain their normal input ownership.
inline void prepare(bool practice) {
    if (!practice) return;
    auto& io=ImGui::GetIO();
    if (!busy()) io.NavInputs[ImGuiNavInput_Activate]=io.NavInputs[ImGuiNavInput_Cancel]=0;
    if (io.NavInputs[ImGuiNavInput_DpadUp] || io.NavInputs[ImGuiNavInput_DpadDown] ||
        io.NavInputs[ImGuiNavInput_DpadLeft] || io.NavInputs[ImGuiNavInput_DpadRight]) {
        GImGui->NavDisableHighlight=false;
        GImGui->NavDisableMouseHover=true;
    }
}
inline int direction(const char* label) {
    auto* window=ImGui::GetCurrentWindow();
    if (busy() || (ImGui::GetItemsFlags() & ImGuiItemFlags_Disabled) ||
        GImGui->NavWindow!=window || GImGui->NavId!=window->GetID(label)) return 0;
    const int delta=int(ImGui::GetNavInputAmount(ImGuiNavInput_DpadRight,ImGuiInputReadMode_Repeat)>0)-
        int(ImGui::GetNavInputAmount(ImGuiNavInput_DpadLeft,ImGuiInputReadMode_Repeat)>0);
    if (delta) ImGui::NavMoveRequestCancel();
    return delta;
}
inline void focus(const char* first, bool entering) {
    if (busy()) return;
    ImGui::SetWindowFocus();
    if (entering || !GImGui->NavIdIsAlive) {
        ImGui::SetFocusID(ImGui::GetCurrentWindow()->GetID(first),ImGui::GetCurrentWindow());
        GImGui->NavInitRequest=false;
        GImGui->NavInitResultId=0;
    }
}
template<class T> inline bool adjust(const char* label,T* value,T low,T high,T step) {
    const int delta=direction(label);
    if (!delta) return false;
    if (ImGui::GetIO().KeyShift) step*=10;
    const auto before=*value;
    *value=std::clamp<T>(*value+delta*step,low,high);
    return before!=*value;
}
inline bool Combo(const char* label,int* value,const char* const* items,int count) {
    const int delta=direction(label);
    const bool changed=delta && count>0;
    if (changed) *value=(*value+delta+count)%count;
    return ImGui::Combo(label,value,items,count) || changed;
}
inline bool SliderInt(const char* label,int* value,int low,int high,const char* format="%d") {
    const bool changed=adjust(label,value,low,high,1);
    return ImGui::SliderInt(label,value,low,high,format) || changed;
}
inline bool SliderFloat(const char* label,float* value,float low,float high,const char* format="%.3f") {
    const bool changed=adjust(label,value,low,high,(high-low)/100.f);
    return ImGui::SliderFloat(label,value,low,high,format) || changed;
}
inline bool DragInt(const char* label,int* value,float speed,int low,int high) {
    const bool changed=adjust(label,value,low,high,std::max(1,int(speed)));
    return ImGui::DragInt(label,value,speed,low,high) || changed;
}
inline bool DragFloat(const char* label,float* value,float speed,float low,float high) {
    const bool changed=adjust(label,value,low,high,speed);
    return ImGui::DragFloat(label,value,speed,low,high) || changed;
}
inline bool DragScalar(const char* label,ImGuiDataType type,void* value,float speed,
    const void* low,const void* high,const char* format) {
    const bool changed=type==ImGuiDataType_S64 && low && high &&
        adjust(label,static_cast<ImS64*>(value),*static_cast<const ImS64*>(low),
            *static_cast<const ImS64*>(high),std::max(ImS64(1),ImS64(speed)));
    return ImGui::DragScalar(label,type,value,speed,low,high,format) || changed;
}
inline bool Checkbox(const char* label,bool* value) {
    const bool changed=direction(label)!=0;
    if (changed) *value=!*value;
    return ImGui::Checkbox(label,value) || changed;
}
}
