#pragma once
#include "EclVm.hpp"
namespace th08 {
struct EclSceneActions {
    virtual ~EclSceneActions()=default;
    virtual void effect(i32 kind,const Vec3&,i32 count,u32 color)=0;
    virtual void parameter_effect(i32 kind,const Vec3&,const Vec3& parameters,i32 count,u32 color)=0;
    virtual void item(const Vec3&,i32 kind,i32 mode)=0;
    virtual i32 power()=0;
    virtual void panned_sound(i32 index,float x)=0;
    virtual bool cancel_enemies(i32 maximum,i32& score)=0;
    virtual bool clear_projectiles(i32 mode)=0;
    virtual void clear_projectiles_near(const Vec3&,float radius)=0;
    virtual EffectState* attached_effect(i32 kind,const Vec3&,i32 count,u32 color,bool overlay)=0;
    virtual bool clock(i32 action)=0;
    virtual void sound(i32 index,i32 mode)=0;
};
bool execute_enemy_scene(EclVm&,const EclInstruction&,const FrameTiming&);
}
