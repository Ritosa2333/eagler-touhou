#pragma once
#include "StageResource.hpp"
#include "SceneCamera.hpp"
#include "AnmManager.hpp"
#include "ScreenDeformation.hpp"
#include <memory>
namespace th11 {
class AnmRenderer;
struct ScreenFades;
bool stage_visible(const StageObject&,Vec3,const SceneCamera&,float) noexcept;
// 4023f0..4051d0. Kept separately from resource ownership so comparisons can
// inspect the same camera, interpolation, fade and embedded-animation state.
struct StageState {
    u32 flags,state,update_entry,draw_entry,file,objects,instances,script_begin;
    u8 camera_effect,reserved21[3];
    Timer effect_timer,script_timer;
    u32 instruction_offset;
    Vec3Interpolator direction_interpolation,position_interpolation;
    FogInterpolator fog_interpolation;
    u32 reserved174,animation_file,object_animations;
    AnmVm script_animations[8];
    float draw_distance_squared;
    u32 effects_enabled,tint;
    i32 frame_effect;
    Vec3 effect_position,effect_size;
    AnmVm effect_animations[3];
    u32 drawn_objects,culled_objects,drawn_primitives,draw_flags;
    Timer fade_timer;
    i32 stage_number;
    u32 frame_count,deformation;
    float deformation_target,deformation_radius;
    u32 deformation_color;
    float phase_x,phase_y;
    i32 deformation_mode;
    u32 reserved302c,reserved3030,foreground_entry,source,source_size;
    SceneCamera camera;
};
#if UINTPTR_MAX == UINT32_MAX
static_assert(sizeof(StageState)==0x3158);
static_assert(offsetof(StageState,camera)==0x3040&&offsetof(StageState,script_animations)==0x180);
static_assert(offsetof(StageState,fog_interpolation)==0xe8&&offsetof(StageState,effect_animations)==0x2348);
#endif
struct StageScriptWorld {
    virtual ~StageScriptWorld()=default;
    virtual bool stage_animation(AnmVm&,i32)=0;
    virtual bool stage_deformation(i32)=0;
    virtual void stage_color(u32)=0;
    virtual void stage_camera_position_interpolated(){}
};
bool stage_script(StageState&,const StageResource&,const float*,StageScriptWorld&);
bool stage_interrupt(StageState&,const StageResource&,i32,const float*);
class Stage final:public StageScriptWorld {
public:
    StageState state{};
    StageResource resource;
    AnmManager& manager;
    AnmResource& animations;
    AnmResource& text;
    std::vector<AnmVm> object_animations;
    std::unique_ptr<ScreenDeformation> mesh;
    u32& background_color;
    u16 file_id;
    SceneCamera* active_camera=nullptr;
    const u32* practice_game_frame=nullptr;
    bool practice_replay_has_stage5=false;
    std::string error;
    Stage(AnmManager& m,AnmResource& a,AnmResource& t,u32& color,u16 id):manager(m),animations(a),text(t),background_color(color),file_id(id){}
    ~Stage();
    bool initialize(const StageResource&,i32,const SceneCamera&,bool start=true);
    bool start_objects();
    bool update_script(){return stage_script(state,resource,&manager.rate,*this);}
    bool interrupt(i32 id){return stage_interrupt(state,resource,id,&manager.rate);}
    bool update();
    bool update_objects();
    void publish_camera();
    bool draw_layer(AnmRenderer&,SceneCamera&,u32);
    bool draw_background(AnmRenderer&,SceneCamera&,ScreenFades&);
    bool draw_foreground(AnmRenderer&,SceneCamera&);
    void fade_out(ScreenFades&);
    void resume_background(){state.fade_timer.set(60,&manager.rate);state.draw_flags|=4;}
    bool stage_animation(AnmVm&,i32)override;
    bool stage_deformation(i32)override;
    void stage_color(u32 color)override{background_color=color;}
    void stage_camera_position_interpolated()override;
private:
    bool update_deformation();
};
}
