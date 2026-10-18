#pragma once
#include "EclVm.hpp"
namespace th08 {
struct EnemyContactActions {
    virtual ~EnemyContactActions()=default;
    virtual i32 graze(const Vec3& position,const Vec3& size)=0;
    virtual i32 hit(const Vec3& position,const Vec3& size)=0;
};
void enemy_contact(EclVm&,u8 character,const Vec3& position,const Vec3& size,EnemyContactActions&);
bool contact_enemy_and_trail(EclVm&,u8 character,EnemyContactActions&);
void damage_familiar_parent(EclVm&,i32 damage,bool bomb);
}
