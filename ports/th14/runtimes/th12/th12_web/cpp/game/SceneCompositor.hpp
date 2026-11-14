#pragma once
#include "AnmManager.hpp"
#include "AnmRenderer.hpp"
#include "SceneSchedule.hpp"
#include "PortTitle.hpp"
#include <string>
namespace th11 {
// 42b530 owns three embedded VMs, separate from the two ANM update lists.
// They advance only when an owner explicitly updates/interrupts them.
class SceneCompositor {
    AnmManager& manager;
    AnmResource* resource=nullptr;
public:
    AnmVm copies[kPortTitle==13||kPortTitle==14||kPortTitle==16?4:3]{};
    SceneCamera full_camera,play_camera;
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_WORKER)
    // Keep shared right XY naturally aligned for Wasm's qword atomic access.
    alignas(8) SceneCamera world_camera;
#else
    SceneCamera world_camera;
#endif
    SceneCamera scaled_play_camera;
    GraphicsViewport region{13,0,422,480,0,1};
    u32 clear_color=0xff000000;
    // Which step of initialize() failed, and for a failing update the copy and the ANM opcode it stopped on. The
    // compositor's VMs are updated directly instead of through the manager's lists, so the manager's own
    // last_error stays 0 for them and this is where that readout has to be taken.
    std::string last_error;
    explicit SceneCompositor(AnmManager& m):manager(m){}
    ~SceneCompositor(){reset();}
    void reset();
    bool initialize(AnmResource&,u32 width=640,u32 height=480);
    bool stage_start();
    bool update();
    void reset_cameras(bool restricted=true,u32 width=640,u32 height=480);
    bool draw_animation(AnmRenderer&,u32 layer);
    bool draw(AnmRenderer&,SceneDrawKind);
};
}
