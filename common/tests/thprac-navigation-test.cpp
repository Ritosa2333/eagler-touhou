#include <eagler/thprac/PracticeNavigation.hpp>
#include <cassert>
#include <cmath>
#include <cstdio>

namespace Nav=eagler::thprac::navigation;
static int stage=0,life=3,power=100,point=10000;
static ImS64 score=20;
static float phase=.5f;
static bool dialogue=false,first=true,disabled=false;
static ImVec2 stage_center,mouse{-FLT_MAX,-FLT_MAX};
static bool mouse_down=false,shift=false;
static void frame(int direction=-1,bool activate=false,bool cancel=false) {
    auto& io=ImGui::GetIO();io.DisplaySize={640,480};io.DeltaTime=1.f/60;
    io.MousePos=mouse;io.MouseDown[0]=mouse_down;io.KeyShift=shift;
    for(auto& input:io.NavInputs)input=0;
    if(direction>=0)io.NavInputs[direction]=1;
    io.NavInputs[ImGuiNavInput_Activate]=activate;
    io.NavInputs[ImGuiNavInput_Cancel]=cancel;
    Nav::prepare(true);
    ImGui::NewFrame();ImGui::SetNextWindowPos({10,10});ImGui::SetNextWindowSize({450,450});
    ImGui::Begin("Practice",nullptr,ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoResize);
    const char* stages[]{"1","2","3","4","5","6","Extra"};
    Nav::Combo("Stage",&stage,stages,7);
    auto a=ImGui::GetItemRectMin(),b=ImGui::GetItemRectMax();stage_center={(a.x+b.x)/2,(a.y+b.y)/2};
    ImGui::BeginDisabled(disabled);Nav::SliderInt("Life",&life,0,9);ImGui::EndDisabled(disabled);
    Nav::SliderInt("Power",&power,0,400);
    Nav::DragInt("Point",&point,10,0,999990);
    const ImS64 lo=0,hi=9999999990LL;Nav::DragScalar("Score",ImGuiDataType_S64,&score,10,&lo,&hi,"%lld");
    Nav::SliderFloat("Phase",&phase,0,1);Nav::Checkbox("Dialogue",&dialogue);
    Nav::focus("Stage",first);first=false;
    ImGui::End();ImGui::Render();
}
static void tap(int direction){frame(direction);frame();}
int main(){
    ImGui::CreateContext();auto& io=ImGui::GetIO();io.IniFilename=nullptr;
    io.ConfigFlags|=ImGuiConfigFlags_NavEnableGamepad;io.BackendFlags|=ImGuiBackendFlags_HasGamepad;
    unsigned char* pixels;int w,h;io.Fonts->GetTexDataAsRGBA32(&pixels,&w,&h);
    frame();frame();tap(ImGuiNavInput_DpadRight);assert(stage==1);
    tap(ImGuiNavInput_DpadLeft);assert(stage==0);tap(ImGuiNavInput_DpadLeft);assert(stage==6);
    assert(!ImGui::IsAnyItemActive());
    frame(-1,true);frame();assert(!Nav::busy()); // confirm belongs to Start
    tap(ImGuiNavInput_DpadDown);tap(ImGuiNavInput_DpadRight);assert(life==4);
    shift=true;tap(ImGuiNavInput_DpadRight);shift=false;assert(life==9);
    tap(ImGuiNavInput_DpadRight);assert(life==9); // clamp
    disabled=true;frame();tap(ImGuiNavInput_DpadLeft);assert(life==9);disabled=false;frame();
    // Move through the actual navigation graph rather than setting widget IDs.
    first=true;frame();frame();tap(ImGuiNavInput_DpadDown);tap(ImGuiNavInput_DpadDown);
    tap(ImGuiNavInput_DpadLeft);assert(power==99);
    tap(ImGuiNavInput_DpadDown);tap(ImGuiNavInput_DpadRight);assert(point==10010);
    tap(ImGuiNavInput_DpadDown);tap(ImGuiNavInput_DpadRight);assert(score==30);
    shift=true;tap(ImGuiNavInput_DpadRight);shift=false;assert(score==130);
    tap(ImGuiNavInput_DpadDown);tap(ImGuiNavInput_DpadRight);assert(std::fabs(phase-.51f)<.0001f);
    tap(ImGuiNavInput_DpadDown);tap(ImGuiNavInput_DpadRight);assert(dialogue);
    tap(ImGuiNavInput_DpadLeft);assert(!dialogue);
    // Mouse-opened combos retain popup ownership; arrows cannot cycle the row underneath.
    mouse=stage_center;mouse_down=true;frame();mouse_down=false;frame();assert(Nav::busy());
    const int before=stage;tap(ImGuiNavInput_DpadRight);assert(stage==before);
    frame(-1,false,true);frame();assert(!Nav::busy());
    mouse={-FLT_MAX,-FLT_MAX};first=true;frame();frame();tap(ImGuiNavInput_DpadRight);
    assert(stage==0); // keyboard control resumes after mouse/popup interaction
    for(int n=0;n<45;++n)frame(ImGuiNavInput_DpadRight);frame();assert(stage!=0);
    ImGui::DestroyContext();std::puts("PASS THPrac real ImGui row navigation, repeats, bounds, focus and popup ownership");
}
