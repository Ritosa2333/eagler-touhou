#include <eagler/thprac/PracticeKeyboard.hpp>
#include <imgui_internal.h>
#include <array>
#include <cassert>
#include <cstdio>

static std::array<bool,512> keys{};
static int value=123;
static bool floating=false;
static double decimal=0;
static ImVec2 field;
static void frame(bool click=false) {
    auto& io=ImGui::GetIO();
    io.DisplaySize={640,480};io.DeltaTime=1.f/60;
    io.ConfigDragClickToInputText=true;
    io.MousePos=field;io.MouseDown[0]=click;
    eagler::thprac::keyboard_frame(io,[](int scan){return keys[scan];});
    ImGui::NewFrame();
    ImGui::SetNextWindowPos({10,10});ImGui::SetNextWindowSize({400,300});
    ImGui::Begin("Advanced tools");
    if(floating) ImGui::DragScalar("Value",ImGuiDataType_Double,&decimal,1,nullptr,nullptr,"%.2f");
    else ImGui::DragInt("Value",&value,1,-100000,100000);
    auto a=ImGui::GetItemRectMin(),b=ImGui::GetItemRectMax();
    field={(a.x+b.x)*.5f,(a.y+b.y)*.5f};
    ImGui::End();ImGui::Render();
}
static void tap(int scan){keys[scan]=true;frame();keys[scan]=false;frame();}
int main(){
    ImGui::CreateContext();auto& io=ImGui::GetIO();io.IniFilename=nullptr;
    unsigned char* pixels;int w,h;io.Fonts->GetTexDataAsRGBA32(&pixels,&w,&h);
    frame();frame();
    // Ctrl-click enters the real DragInt TempInputText, not a mock textbox.
    keys[224]=true;frame(true);keys[224]=false;frame();
    assert(io.WantTextInput);
    tap(35);tap(36); // top-row 6,7 replace selected 123
    tap(40);assert(value==67);
    frame();keys[224]=true;frame(true);keys[224]=false;frame();
    tap(86);tap(93);tap(98); // keypad minus,5,0
    tap(40);assert(value==-50);
    frame();keys[224]=true;frame(true);keys[224]=false;frame();
    // Ctrl+A must select, not inject text; Backspace and Delete edit normally.
    keys[224]=keys[4]=true;frame();keys.fill(false);frame();
    tap(30);tap(31);tap(32);tap(42);tap(80);tap(76);
    tap(40);assert(value==1);
    floating=true;frame();keys[224]=true;frame(true);keys[224]=false;frame();
    tap(89);tap(99);tap(90);tap(93);tap(88);assert(decimal==1.25);
    // A held digit is queued once. Chords never leak their numeric characters.
    keys[33]=true;
    eagler::thprac::keyboard_frame(io,[](int scan){return keys[scan];});
    assert(io.InputQueueCharacters.Size==1 && io.InputQueueCharacters[0]=='4');
    frame();frame();assert(io.InputQueueCharacters.empty());
    keys.fill(false);keys[224]=keys[34]=true;
    eagler::thprac::keyboard_frame(io,[](int scan){return keys[scan];});
    assert(io.InputQueueCharacters.empty());
    ImGui::DestroyContext();std::puts("PASS thprac numeric keyboard / DragInt editing");
}
