#include "../th08_web/cpp/third_party/imgui/imgui.h"
#include "../th08_web/cpp/third_party/imgui/imgui_internal.h"
#include <cassert>
#include <cstring>
#include <initializer_list>

// Exercise the shipped legacy ImGui implementation, not a mock. Its
// BeginDisabled/EndDisabled(bool) are conditional push/pop helpers.
int main(int argc,char** argv){
 const bool reproduce=argc>1&&!std::strcmp(argv[1],"--reproduce-old-pop");
 ImGui::CreateContext();auto& io=ImGui::GetIO();io.IniFilename=nullptr;
 io.DisplaySize={640,480};io.DeltaTime=1.f/60.f;
 unsigned char* pixels;int width,height;io.Fonts->GetTexDataAsRGBA32(&pixels,&width,&height);
 assert(pixels&&width&&height);
 const auto flags=ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|
  ImGuiWindowFlags_NoMove|ImGuiWindowFlags_AlwaysAutoResize|
  ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoFocusOnAppearing|ImGuiWindowFlags_NoNav;
 for(int frame=0;frame<18;++frame){
  // Menu cold-open, gameplay, replay and close/reopen lifecycle.
  const bool replay=frame>=6&&frame<12;
  ImGui::NewFrame();ImGui::SetNextWindowPos({10,10},ImGuiCond_Always);
  ImGui::SetNextWindowSize({0,0});ImGui::SetNextWindowBgAlpha(.5f);
  if(ImGui::Begin("Mod Menu###th08-thprac-overlay",nullptr,flags)){
   auto* context=ImGui::GetCurrentContext();
   const int items=context->ItemFlagsStack.Size,styles=context->StyleVarStack.Size;
   ImGui::BeginDisabled(replay);
   for(const char* key:{"F1","F2","F3","F4","F5","F6","F7","U"}){
    const auto cursor=ImGui::GetCursorPos();ImGui::Text("%s: Invincibility",key);
    ImGui::SetCursorPos(cursor);
    const ImVec2 size{ImGui::GetWindowWidth()-ImGui::GetStyle().WindowPadding.x*2,ImGui::GetTextLineHeight()};
    assert(size.x>0&&size.y>0);ImGui::InvisibleButton(key,size);
   }
   if(reproduce)ImGui::EndDisabled();else ImGui::EndDisabled(replay);
   assert(context->ItemFlagsStack.Size==items&&context->StyleVarStack.Size==styles);
  }
  ImGui::End();ImGui::Render();assert(ImGui::GetDrawData()->Valid);
 }
 ImGui::DestroyContext();
}
