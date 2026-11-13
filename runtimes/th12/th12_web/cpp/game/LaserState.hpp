#pragma once
#include "BulletState.hpp"
#include "LaserCurveTh14.hpp"
#include <array>
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
#if PORT_TITLE == 16
struct Th16LaserLine {
    LaserState base;u8 request[0x358]{};AnmVm body{},origin{},tip{};
    Timer motion_clock,visibility_clock,wait_clock{0,0,0,nullptr,0};u32 native_flags=0;
    i32 turn_mode=0,diagnostic=0;
};
#endif
#if PORT_TITLE == 14
struct Th14LaserLine {
    LaserState base;u8 request[0x354]{};AnmVm body{},origin{},tip{};Timer motion_clock;
    u32 native_flags=0;i32 diagnostic=0;
};
struct Th14LaserInfinite {
    LaserState base;u8 request[0x374]{};AnmVm body{},origin{};
    u32 native_flags=0;i32 diagnostic=0;
};
struct Th14LaserCurve {
    LaserState base;Th14CurvePath path;AnmVm body{},origin{};Timer curve_timer;
    u32 native_flags=0;std::vector<Th14CurveVertex> vertices;i32 diagnostic=0;
};
// Retail factory kind 3 creates vtable 4be364, whose update/draw/collision
// methods return zero. It still occupies the registry and obeys cancellation.
struct Th14LaserShell {
    LaserState base;u8 request[0x354]{};u32 native_flags=0;
    float history[512]{};i32 cursor=0;float native_scalar_74=1;
};
#endif
#if UINTPTR_MAX == UINT32_MAX
static_assert(sizeof(LaserState)==0x440);
static_assert(offsetof(LaserState,motion)==0x70);
static_assert(offsetof(LaserState,offscreen_timer)==0x424);
static_assert(sizeof(LaserLineParameters)==0x1e4);
static_assert(offsetof(LaserLine,body)==0x624);
static_assert(sizeof(LaserLine)==0xe8c+2*kAnmVmExtensionBytes);
static_assert(sizeof(LaserInfiniteParameters)==0x204);
static_assert(offsetof(LaserInfinite,body)==0x648);
static_assert(sizeof(LaserInfinite)==0xeb0+2*kAnmVmExtensionBytes);
#endif
struct LaserWorld {
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
#if PORT_TITLE == 16
    virtual bool th16_line_appearance(Th16LaserLine&,i32){return false;}
    virtual i32 th16_line_cut(Th16LaserLine&,Vec3,Vec2,float,u32,bool){return -2;}
    virtual bool th16_line_graze(Vec3){return false;}
    virtual bool th16_line_reward(Vec3,u32){return false;}
    virtual bool th16_line_cut_effect(Vec3 p,i32 script){return cancel_effect(p,script);}
#endif
#if PORT_TITLE == 14
    virtual bool th14_curve_reward(Vec3,u32){return false;}
    virtual bool th14_curve_graze(Vec3){return false;}
    virtual i32 th14_line_cut(Th14LaserLine&,Vec3,Vec2,float,u32,bool){return -2;}
    virtual i32 th14_infinite_cut(Th14LaserInfinite&,Vec3,Vec2,float,u32,bool){return -2;}
    virtual i32 th14_curve_cut(Th14LaserCurve&,Vec3,Vec2,float,u32,bool){return -2;}
    virtual bool th14_line_spawn(const u8*){return false;}
#endif
};
struct AnmRenderer;
#if PORT_TITLE == 16
void laser_th16_line_initialize_state(Th16LaserLine&,const u8*,LaserWorld&)noexcept;
bool laser_th16_line_initialize(Th16LaserLine&,const u8*,AnmResource&,AnmEnvironment&,LaserWorld&,u16);
i32 laser_th16_line_transforms(Th16LaserLine&,LaserWorld&)noexcept;
i32 laser_th16_line_step(Th16LaserLine&,LaserWorld&)noexcept;
i32 laser_th16_line_collision(Th16LaserLine&,LaserWorld&,bool probe=false);
i32 laser_th16_line_update(Th16LaserLine&,AnmEnvironment&,LaserWorld&);
bool laser_th16_line_draw(Th16LaserLine&,AnmRenderer&);
i32 laser_th16_line_cancel(Th16LaserLine&,bool,bool,LaserWorld&)noexcept;
i32 laser_th16_line_cut_circle(Th16LaserLine&,Vec3,float,u32,bool,std::vector<std::array<u8,0x358>>&,LaserWorld&);
i32 laser_th16_line_cut_rectangle(Th16LaserLine&,Vec3,Vec2,float,u32,bool,std::vector<std::array<u8,0x358>>&,LaserWorld&);
#endif
#if PORT_TITLE == 14
void laser_th14_line_initialize_state(Th14LaserLine&,const u8*,LaserWorld&)noexcept;
bool laser_th14_line_initialize(Th14LaserLine&,const u8*,AnmResource&,AnmEnvironment&,LaserWorld&,u16);
i32 laser_th14_line_step(Th14LaserLine&,LaserWorld&)noexcept;
i32 laser_th14_line_transforms(Th14LaserLine&,LaserWorld&)noexcept;
i32 laser_th14_line_bounce(Th14LaserLine&,LaserWorld&)noexcept;
i32 laser_th14_line_collision(Th14LaserLine&,LaserWorld&,bool probe=false);
i32 laser_th14_line_update(Th14LaserLine&,AnmEnvironment&,LaserWorld&);
bool laser_th14_line_draw(Th14LaserLine&,AnmRenderer&);
i32 laser_th14_line_cancel(Th14LaserLine&,bool reward,bool skip,LaserWorld&)noexcept;
i32 laser_th14_line_cut_circle(Th14LaserLine&,Vec3,float,u32,bool,std::vector<std::array<u8,0x354>>&,LaserWorld&);
i32 laser_th14_line_cut_rectangle(Th14LaserLine&,Vec3,Vec2,float,u32,bool,std::vector<std::array<u8,0x354>>&,LaserWorld&);
bool laser_th14_infinite_initialize(Th14LaserInfinite&,const u8*,AnmResource&,AnmEnvironment&,LaserWorld&,u16);
void laser_th14_infinite_initialize_state(Th14LaserInfinite&,const u8*,LaserWorld&)noexcept;
// State/motion portion of retail 43e040; ANM and collision delivery follow it.
i32 laser_th14_infinite_step(Th14LaserInfinite&,LaserWorld&)noexcept;
i32 laser_th14_infinite_update(Th14LaserInfinite&,AnmEnvironment&,LaserWorld&);
i32 laser_th14_infinite_collision(Th14LaserInfinite&,LaserWorld&,bool probe=false);
bool laser_th14_infinite_draw(Th14LaserInfinite&,AnmRenderer&);
i32 laser_th14_infinite_cancel(Th14LaserInfinite&,bool,bool,LaserWorld&)noexcept;
i32 laser_th14_infinite_cut_circle(Th14LaserInfinite&,Vec3,float,u32,bool,std::vector<std::array<u8,0x354>>&,LaserWorld&);
i32 laser_th14_infinite_cut_rectangle(Th14LaserInfinite&,Vec3,Vec2,float,u32,bool,std::vector<std::array<u8,0x354>>&,LaserWorld&);
bool laser_th14_curve_initialize(Th14LaserCurve&,const u8*,AnmResource&,AnmEnvironment&,LaserWorld&,u16,const std::vector<Th14CurveNode>* inherited=nullptr);
i32 laser_th14_curve_update(Th14LaserCurve&,AnmEnvironment&,LaserWorld&);
i32 laser_th14_curve_collision(Th14LaserCurve&,LaserWorld&,bool probe=false);
bool laser_th14_curve_draw(Th14LaserCurve&,AnmRenderer&);
i32 laser_th14_curve_cancel(Th14LaserCurve&,bool reward,bool skip,LaserWorld&);
i32 laser_th14_curve_cut_circle(Th14LaserCurve&,Vec3,float,u32,bool,LaserWorld&);
i32 laser_th14_curve_cut_rectangle(Th14LaserCurve&,Vec3,Vec2,float,u32,bool,std::vector<std::array<u8,0x354>>&,LaserWorld&);
#endif
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
