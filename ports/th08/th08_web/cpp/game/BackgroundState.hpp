#pragma once
#include "AnmExecutor.hpp"
#include "Camera.hpp"
#include "StageProgram.hpp"
namespace th08 {
class AnmRenderer;struct BackgroundState;
using BackgroundDrawCallback=void(*)(BackgroundState&,AnmRenderer&,void*);
struct StageFog {float near_plane,far_plane;ZunColor color;};
struct BackgroundState {
    AnmVm* quad_vms;AnmVm layers[3];AnmLoaded* animation;StageHeader* stage_data;
    i32 quad_count,object_count;StageObject** objects;StageInstance* instances;StageInstruction* instructions;
    Timer time;i32 instruction_index,frames,stage;Vec3 position;u32 clear_color;
    u8 youkai_tint;Timer tint_time;AnmVm tint_vm;AnmVm* moon_effect;
    StageFog fog,fog_initial,fog_final;i32 fog_duration;Timer fog_time;u8 fog_needs_setup;
    i32 spell_state,spell_frames,spell_flag,spell_vm_count,dialogue_state;
    AnmVm spell_vms[32],extra_vm;BackgroundDrawCallback callback;i32 pending_interrupt;
    SceneCamera camera_final,camera_initial,camera_final_derivative,camera_initial_derivative,camera;
    i32 camera_durations[5];Timer camera_timers[5];i32 camera_modes[5];
    Vec3 next_position;i32 next_time;Vec3 previous_position;i32 previous_time;
    u8 jumped;ZunColor tint_color;i32 use_tint;float distance_limit;u8 camera_effect;
    i32 effect_flags,effect_visible;Vec3 effect_positions[32];
    BackgroundState(){std::memset(this,0,sizeof(*this));camera=SceneCamera{};camera_final=camera_initial=camera;}
};
static_assert(sizeof(BackgroundState)==0x6600);
static_assert(offsetof(BackgroundState,time)==0x80c&&offsetof(BackgroundState,fog)==0xaec);
static_assert(offsetof(BackgroundState,camera)==0x6394&&offsetof(BackgroundState,effect_positions)==0x6480);
}
