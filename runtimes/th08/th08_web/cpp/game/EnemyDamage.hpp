#pragma once
#include "EnemyContact.hpp"
#include "PlayerFrame.hpp"
namespace th08 {
struct EnemyDamageContext {Vec3 player;u8 character=0,bomb=0,time_spell=0,spell_bomb_damage=0;};
struct EnemyDamageActions {
    virtual ~EnemyDamageActions()=default;
    virtual i32 damage(const Vec3& position,const Vec3& size,i32& time_items,i32& bomb_hit)=0;
};
// Damage and target-selection block of the original manager (42d06d..42d4df).
// Visibility, enemy contact and trail contact are earlier manager phases.
bool damage_enemy(EclVm&,const EnemyDamageContext&,PlayerFrameState&,GameValues&,i32& bomb_hit,EnemyDamageActions&);
}
