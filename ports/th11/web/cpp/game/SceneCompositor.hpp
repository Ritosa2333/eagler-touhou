#pragma once
#include "AnmManager.hpp"
#include "AnmRenderer.hpp"
#include "SceneSchedule.hpp"
namespace th11 {
// 42b530 owns three embedded VMs, separate from the two ANM update lists.
// They advance only when an owner explicitly updates/interrupts them.
class SceneCompositor {
    AnmManager& manager;
    AnmResource* resource=nullptr;
public:
    AnmVm copies[3]{};
    SceneCamera full_camera,play_camera,world_camera;
    GraphicsViewport region{13,0,422,480,0,1};
    u32 clear_color=0xff000000;
    explicit SceneCompositor(AnmManager& m):manager(m){}
    ~SceneCompositor(){reset();}
    void reset();
    bool initialize(AnmResource&);
    bool stage_start();
    void reset_cameras(bool restricted=true);
    bool draw(AnmRenderer&,SceneDrawKind);
};
}
