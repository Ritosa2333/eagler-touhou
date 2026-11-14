#pragma once
#include "AnmVm.hpp"
#include "BulletEmitter.hpp"
#if PORT_TITLE == 14
#include "EnemyTh14Emitter.hpp"
#endif
#if PORT_TITLE == 16
#include "EnemyTh16Emitter.hpp"
#endif
namespace th11 {
struct EnemySpawn;
struct BulletMotionSlot {
    Timer timer;
    float a,b;
    Vec3 vector;
    i32 duration,limit,count;
};
struct BulletState {
    u32 flags; i32 collision_delay;
    AnmVm vm;
    Vec3 position,velocity;
    float speed,angle;
    Vec2 hitbox;
    Timer lifetime,auxiliary_timer;
    i32 extra;u8 reserved_490[16];
    i32 offscreen_grace,cancel_effect;
    u32 active_transforms,emitter_flags;
    u16 reserved_4b0;i16 state;
    u32 reserved_4b4;BulletState* draw_next;u32 reserved_4bc;
    i32 reserved_4c0,transform_sound,transform_index,draw_group;
    BulletTransform transforms[18];
    BulletMotionSlot motion[11];
    u32 reserved_8bc;
    Vec3Interpolator position_interpolation;
    i16 sprite,color;
#if PORT_TITLE == 16
    Th16BulletTransform th16_transforms[18];
    float th16_size;
    i32 th16_script_state;
    BulletState* th16_free_next;BulletState* th16_active_next;BulletState* th16_active_previous;
#endif
#if PORT_TITLE == 14
    Th14BulletTransform th14_transforms[18];
    BulletState* th14_free_next;BulletState* th14_active_next;BulletState* th14_active_previous;
    // Native +be8 is a script tag, independent of the +c0e lifecycle state.
    i32 th14_script_state;
    Timer th14_wait_timer,th14_freeze_timer;
    i32 th14_step_mode,th14_step_extra;
    FloatInterpolator th14_size_interpolation;
    float th14_size_current,th14_size;
#endif
};
#if UINTPTR_MAX == UINT32_MAX
static_assert(sizeof(BulletMotionSlot)==0x34);
static_assert(sizeof(BulletState)==0x910+kAnmVmExtensionBytes+(kPortTitle==14?18*0x2c+116:kPortTitle==16?18*0x2c+20:0));
static_assert(offsetof(BulletState,position)==0x43c+kAnmVmExtensionBytes);
static_assert(offsetof(BulletState,active_transforms)==0x4a8+kAnmVmExtensionBytes);
static_assert(offsetof(BulletState,transforms)==0x4d0+kAnmVmExtensionBytes);
static_assert(offsetof(BulletState,motion)==0x680+kAnmVmExtensionBytes);
static_assert(offsetof(BulletState,position_interpolation)==0x8c0+kAnmVmExtensionBytes);
#endif
struct BulletEnvironment {
    float rate=1;Vec3 player{},default_tangent{};
#if PORT_TITLE == 14
    Vec2 th14_bounce_bounds{};
    Rng* th14_visual_rng=nullptr;
    virtual bool fire_th14(const Th14BulletEmitter&){return false;}
    virtual bool th14_spawn_enemy(const char*,const EnemySpawn&){return false;}
#endif
    virtual ~BulletEnvironment()=default;
    virtual bool sound(i32,float,bool){return false;}
    virtual bool change_appearance(BulletState&,i32,i32,bool){return false;}
    virtual bool cancel(BulletState&){return false;}
    virtual bool fire(const BulletEmitter&){return false;}
};
float bullet_aim(const Vec3& origin,const Vec3& player)noexcept;
// 0 succeeds; -2 preserves an unimplemented external operation or invalid loop.
i32 bullet_start_transforms(BulletState&,BulletEnvironment&)noexcept;
i32 bullet_motion(BulletState&,u32 transform,BulletEnvironment&)noexcept;
}
