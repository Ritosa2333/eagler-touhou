#pragma once
#include "StageScript.hpp"
#include "StageCamera.hpp"
#include "StageVisibility.hpp"
#include "AnmManager.hpp"
#include "AnmRenderer.hpp"
namespace th15 {
struct StageSceneServices {
    virtual ~StageSceneServices()=default;
    virtual bool begin_deformation(i32 mode,i32 script)=0;
    virtual bool update_deformation(StageScriptState&,float rate)=0;
};
struct StageSceneFrame {float rate=1;u32 flags=0;i32 transition_age=0;};
class StageScene final:public StageScriptWorld {
    friend class StageCheckpoint;
    const StageResource& file;AnmManager& animations;AnmEnvironment& environment;StageSceneServices& services;i32 bank;
    std::vector<AnmVm> primitives;std::array<AnmVm,8> embedded;std::vector<u8> object_flags;std::vector<i16> instance_flags;
    bool initialized=false,deforming=false;float rate=1;Vec3 view_direction{};
    bool bind(AnmVm&,i32);bool fail(const std::string&);
    bool clear_color(u32)override;bool animation(u32,i32,i32)override;
    bool deformation(i32,i32)override;bool object_interrupt(i32)override;
public:
    StageScript script;std::string error;u32 color=0,frames=0,drawn_instances=0,culled_instances=0,drawn_primitives=0;
    bool draw_objects=true;
    StageScene(const StageResource&,AnmManager&,AnmEnvironment&,StageSceneServices&,i32 bank);
    ~StageScene();
    bool initialize(const StageCamera& initial_camera);bool update(const StageSceneFrame&);
    bool interrupt(i32 label);bool interrupt_objects(i32 label);
    bool draw(AnmRenderer&,ZunGraphics&,i32 layer,const GraphicsViewport&,Vec2 playfield_origin);
    AnmVm* primitive(u32)noexcept;AnmVm* slot(u32)noexcept;u8 object_state(u32)const noexcept;i16 instance_state(u32)const noexcept;
    const Vec3& camera_direction()const noexcept{return view_direction;}
    bool ready()const noexcept{return initialized;}
};
}
