#pragma once
#include "BulletState.hpp"
namespace th11 {
struct LaserState {
    u32 dispatch;LaserState* previous;LaserState* next;
    i32 state,type;Timer lifetime,graze_timer;
    Vec3 position,velocity;float angle,length,width,speed,offset;
    u8 marked,rectangle_enabled;u16 reserved_6a;i32 id;
    BulletMotionSlot motion[18];i32 transform_index;u32 active_transforms,reserved_420;
    Timer offscreen_timer;i32 protection;i16 sprite,color;
    void initialize(const float* rate)noexcept;
    i32 probe(Vec2 center,float radius)const noexcept;
};
struct LaserLineParameters {
    Vec3 position;float angle,growth_limit,initial_length,end_distance,width,speed;
    i16 sprite,color;u32 flags;BulletTransform transforms[18];i32 start_sound,transform_sound;
};
struct LaserLine {
    LaserState base;LaserLineParameters parameters;AnmVm body,origin;
};
struct LaserInfiniteParameters {
    Vec3 position,velocity;float angle,angular_velocity,max_length,initial_length,width,speed;
    i32 warning_frames,expand_frames,active_frames,shrink_frames,start_sound,transform_sound,id;
    i16 sprite,color;u32 flags;BulletTransform transforms[18];
};
struct LaserInfinite {
    LaserState base;LaserInfiniteParameters parameters;u32 reserved_644;AnmVm body,origin;
};
#if UINTPTR_MAX == UINT32_MAX
static_assert(sizeof(LaserState)==0x440);
static_assert(offsetof(LaserState,motion)==0x70);
static_assert(offsetof(LaserState,offscreen_timer)==0x424);
static_assert(sizeof(LaserLineParameters)==0x1e4);
static_assert(offsetof(LaserLine,body)==0x624);
static_assert(sizeof(LaserLine)==0xe8c);
static_assert(sizeof(LaserInfiniteParameters)==0x204);
static_assert(offsetof(LaserInfinite,body)==0x648);
static_assert(sizeof(LaserInfinite)==0xeb0);
#endif
struct LaserWorld {
#ifdef TH11_MULTIPLAYER
    virtual bool multiplayer_world()const{return false;}
    virtual Vec3 multiplayer_target(Vec3)const{return {};}
#endif
    float rate=1;Vec3 player{};
    virtual ~LaserWorld()=default;
    virtual bool sound(i32,float,bool){return false;}
    virtual bool change_appearance(LaserLine&,i32){return false;}
    virtual bool spawn_line(const LaserLineParameters&){return false;}
    virtual bool cancel_effect(Vec3,i32){return false;}
    virtual bool cancel_reward(Vec3){return false;}
    virtual bool cancel_shot(Vec3,float){return false;}
    virtual i32 collision(Vec3,float,float,float){return -2;}
    virtual i32 warning_collision(Vec3,float,float,float){return -2;}
    virtual bool cut(LaserLine&,Vec3,Vec3,bool,bool){return false;}
    virtual bool graze_effect(Vec3){return false;}
    virtual bool graze_reward(){return false;}
    virtual bool graze_register(){return false;}
    virtual bool boss_position(Vec3&){return false;}
    virtual bool cut_infinite(LaserInfinite&,Vec3,Vec3,bool,bool){return false;}
};
struct AnmRenderer;
bool laser_line_initialize(LaserLine&,const LaserLineParameters&,AnmResource&,AnmEnvironment&,LaserWorld&,u16 file_id=0);
bool laser_line_appearance(LaserLine&,i32,AnmResource&,AnmEnvironment&,u16 file_id=0);
i32 laser_line_update(LaserLine&,AnmEnvironment&,LaserWorld&);
bool laser_line_draw(LaserLine&,AnmRenderer&);
i32 laser_line_transforms(LaserLine&,LaserWorld&)noexcept;
i32 laser_line_motion(LaserLine&,u32,LaserWorld&)noexcept;
i32 laser_line_cancel(LaserLine&,bool reward,bool skip_protected,LaserWorld&)noexcept;
i32 laser_line_cut_rectangle(LaserLine&,Vec3 center,Vec3 size,bool reward,bool skip_protected,LaserWorld&)noexcept;
i32 laser_line_cut_circle(LaserLine&,Vec3 center,float radius,u32 rewards,bool skip_protected,LaserWorld&)noexcept;
bool laser_infinite_initialize(LaserInfinite&,const LaserInfiniteParameters&,AnmResource&,AnmEnvironment&,LaserWorld&,u16 file_id=0);
i32 laser_infinite_update(LaserInfinite&,AnmEnvironment&,LaserWorld&);
bool laser_infinite_warning(LaserInfinite&,float angular_delta,LaserWorld&);
bool laser_infinite_draw(LaserInfinite&,AnmRenderer&);
i32 laser_infinite_cancel(LaserInfinite&,bool reward,bool skip_protected,LaserWorld&)noexcept;
i32 laser_infinite_cut_rectangle(LaserInfinite&,Vec3 center,Vec3 size,bool reward,bool skip_protected,LaserWorld&)noexcept;
i32 laser_infinite_cut_circle(LaserInfinite&,Vec3 center,float radius,u32 rewards,bool skip_protected,LaserWorld&)noexcept;
}
