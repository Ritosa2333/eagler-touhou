#pragma once
#include "EclVm.hpp"
namespace th08 {
struct BulletManagerState;
struct EclNativeActions {
    virtual ~EclNativeActions()=default;
    virtual void screen(i32 type,i32 duration,i32 a,i32 b,i32 c,i32 priority)=0;
    virtual bool spell_background(i32 variant,const Vec3& position)=0;
    virtual void background_interrupt(i16 value)=0;
    virtual void tint(u32 color)=0;
    virtual void laser(const Vec2& center,const Vec2& size,const Vec3& origin,float angle,bool graze)=0;
    virtual void item(const Vec3& position,i32 kind,i32 mode)=0;
    virtual bool bomb_active()=0;
    virtual bool spell_announcement(i32 portrait,const char* name,i32 style)=0;
};
struct EclNativeServices {
    FrameTiming* timing=nullptr;EclNativeActions* actions=nullptr;BulletManagerState* projectiles=nullptr;
    i32 screen_effect_counter=0;i32* screen_effect_counter_value=nullptr;
};
// IDs are entries in the original 004c6cb0 callback table, not x86 addresses.
bool execute_ecl_native(EclVm&,i32 id,const EclInstruction&);
bool configure_ecl_native(EclVm&,const EclInstruction&);
bool execute_ecl_native_projectiles(EclVm&,i32,const EclInstruction&,EclNativeServices&);
}
