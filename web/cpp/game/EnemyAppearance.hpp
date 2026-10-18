#pragma once
#include "EclVm.hpp"
namespace th08 {
struct EnemyAppearanceActions {
    virtual ~EnemyAppearanceActions()=default;
    virtual void effect(i32 kind,const Vec3& position,i32 count,u32 color)=0;
    virtual void sound(i32 index,i32 mode)=0;
    virtual void panned_sound(i32 index,float x)=0;
};
void update_enemy_form(EclVm&,bool youkai,EnemyAppearanceActions&);
void update_enemy_hit_flash(EclVm&,bool hit,EnemyAppearanceActions&);
}
