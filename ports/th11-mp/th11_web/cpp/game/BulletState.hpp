#pragma once
#include "AnmVm.hpp"
#include "BulletEmitter.hpp"
namespace th11 {
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
};
#if UINTPTR_MAX == UINT32_MAX
static_assert(sizeof(BulletMotionSlot)==0x34);
static_assert(sizeof(BulletState)==0x910);
static_assert(offsetof(BulletState,position)==0x43c);
static_assert(offsetof(BulletState,active_transforms)==0x4a8);
static_assert(offsetof(BulletState,transforms)==0x4d0);
static_assert(offsetof(BulletState,motion)==0x680);
static_assert(offsetof(BulletState,position_interpolation)==0x8c0);
#endif
struct BulletEnvironment {
    float rate=1;Vec3 player{},default_tangent{};
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
