// Actual title ImGui owner, with only SDL window services replaced for Node.
#include "../src/ThpracImGui.cpp"
#include <eagler/thprac/PracticeMobilePanels.hpp>
#include <cassert>
GameWindow g_GameWindow{};
extern "C" bool SDL_GetWindowSize(SDL_Window*,int* w,int* h){*w=1280;*h=960;return true;}
extern "C" SDL_Window* SDL_GetMouseFocus(){return nullptr;}
extern "C" SDL_Window* SDL_GetKeyboardFocus(){return nullptr;}
extern "C" SDL_MouseButtonFlags SDL_GetMouseState(float* x,float* y){*x=*y=0;return 0;}
extern "C" bool SDL_CaptureMouse(bool){return true;}
namespace mobile=eagler::thprac::mobile;
static void frame(){
    auto& io=ImGui::GetIO();io.DisplaySize={640,480};io.DeltaTime=1.f/60;
    ThpracImGui::ApplyMouseInput();ImGui::NewFrame();
    if(mobile::show(mobile::Cheats,false)){
        mobile::begin(mobile::Cheats,"Cheats###test",ImGuiWindowFlags_NoMove|ImGuiWindowFlags_AlwaysAutoResize,1);
        ImGui::End();
    }
    if(mobile::show(mobile::Information,false)){
        ImGui::SetNextWindowPos({450,220},ImGuiCond_Always);
        if(mobile::begin(mobile::Information,"Info###test-information",ImGuiWindowFlags_AlwaysAutoResize,1))ImGui::Dummy({150,100});
        ImGui::End();
    }
    ImGui::Render();
}
int main(){
    ImGui::CreateContext();auto& io=ImGui::GetIO();io.IniFilename=nullptr;
    unsigned char* pixels;int width,height;io.Fonts->GetTexDataAsRGBA32(&pixels,&width,&height);
    ThpracImGui::g_Initialized=true;g_GameWindow.window=reinterpret_cast<SDL_Window*>(1);
    EM_ASM({Module.eaglerControls={};Module.eaglerControls.thpracMobilePanelsEnabled=true;Module.eaglerControls.thpracMobilePanelsVisible=true;});
    mobile::update(true,true);frame();frame();
    assert(!mobile::expanded[mobile::Cheats]);
    const std::int64_t id=(std::int64_t(1)<<40)+17;
    // Both edges arrive between ticks: the real owner queues them separately.
    assert(ThpracImGui::RouteTouchPointer(0,id,20.f/640,20.f/480,true));
    assert(ThpracImGui::RouteTouchPointer(2,id,20.f/640,20.f/480,true));
    frame();assert(io.MouseDown[0]);frame();assert(!io.MouseDown[0]);frame();
    assert(mobile::expanded[mobile::Cheats]);
    // A native cancellation inside the header cannot act as a click.
    assert(ThpracImGui::RouteTouchPointer(0,id,20.f/640,20.f/480,true));
    frame();
    assert(ThpracImGui::RouteTouchPointer(3,id,20.f/640,20.f/480,true));
    frame();frame();assert(mobile::expanded[mobile::Cheats]);
    // A game drag crossing the trainer remains game-owned.
    assert(!ThpracImGui::RouteTouchPointer(0,2,.99f,.99f,true));
    assert(!ThpracImGui::RouteTouchPointer(1,2,20.f/640,20.f/480,true));
    assert(!ThpracImGui::RouteTouchPointer(2,2,20.f/640,20.f/480,true));
    // The black Information window keeps a tap, but forwards the original
    // pointer-down to gameplay when the same gesture becomes a drag.
    assert(ThpracImGui::RouteTouchPointer(0,id,470.f/640,230.f/480,true));
    assert(ThpracImGui::RouteTouchPointer(2,id,470.f/640,230.f/480,true));
    frame();frame();frame();frame();frame();assert(mobile::expanded[mobile::Information]);
    float startX=-1,startY=-1;
    assert(ThpracImGui::RouteTouchPointer(0,id,470.f/640,270.f/480,true));
    assert(!ThpracImGui::RouteTouchPointer(1,id,495.f/640,280.f/480,true,&startX,&startY));
    assert(std::abs(startX-470.f/640)<.00001f&&std::abs(startY-270.f/480)<.00001f);
    assert(!ThpracImGui::RouteTouchPointer(2,id,495.f/640,280.f/480,true));
    // Coalesced input can first exceed the drag threshold on release.
    assert(ThpracImGui::RouteTouchPointer(0,id,470.f/640,270.f/480,true));
    startX=startY=-1;
    assert(!ThpracImGui::RouteTouchPointer(2,id,495.f/640,280.f/480,true,&startX,&startY));
    assert(std::abs(startX-470.f/640)<.00001f&&std::abs(startY-270.f/480)<.00001f);
    // Native SDL window-normalized input retains its own coordinate space.
    assert(ThpracImGui::RouteTouchPointer(0,id,940.f/1280,540.f/960,false));
    assert(!ThpracImGui::RouteTouchPointer(1,id,990.f/1280,560.f/960,false,&startX,&startY));
    assert(std::abs(startX-940.f/1280)<.00001f&&std::abs(startY-540.f/960)<.00001f);
    assert(!ThpracImGui::RouteTouchPointer(2,id,990.f/1280,560.f/960,false));
    assert(ThpracImGui::RouteTouchPointer(0,id,470.f/640,270.f/480,true));
    assert(ThpracImGui::RouteTouchPointer(3,id,470.f/640,270.f/480,true));
    assert(!ThpracImGui::RouteTouchPointer(1,id,495.f/640,280.f/480,true));
    // Native SDL normalized window coordinates map through the render scale.
    assert(ThpracImGui::RouteTouchPointer(0,id,40.f/1280,40.f/960,false));
    assert(!ThpracImGui::RouteTouchPointer(0,3,.99f,.99f,true));
    assert(ThpracImGui::RouteTouchPointer(2,id,.99f,.99f,false));
    frame();frame();
    assert(ThpracImGui::RouteTouchPointer(0,id,20.f/640,20.f/480,true));
    SDL_Event lost{};lost.type=SDL_EVENT_WINDOW_FOCUS_LOST;ThpracImGui::ProcessEvent(lost);
    assert(ThpracImGui::g_PointerEdges.empty());
    assert(!ThpracImGui::RouteTouchPointer(1,id,20.f/640,20.f/480,true));
    assert(!ThpracImGui::RouteTouchPointer(2,id,20.f/640,20.f/480,true));
    frame();assert(!io.MouseDown[0]);
    // A synthesized SDL mouse stream cannot bypass touch ownership.
    SDL_Event mouse{};mouse.type=SDL_EVENT_MOUSE_BUTTON_DOWN;mouse.button.which=SDL_TOUCH_MOUSEID;
    mouse.button.button=SDL_BUTTON_LEFT;ThpracImGui::ProcessEvent(mouse);frame();assert(!io.MouseDown[0]);
    ThpracImGui::CancelLogicalPointers();ThpracImGui::g_Initialized=false;ImGui::DestroyContext();
    std::puts("PASS actual title pointer owner: folded first tap, queued edges, crossing, native scale, second finger, cancel, synthetic mouse");
}
