#pragma once
#include "EnemyContact.hpp"
#include "PlayerFrame.hpp"
namespace th08 {
struct EnemyDamageContext {Vec3 player;u8 character=0,bomb=0,time_spell=0,spell_bomb_damage=0;};
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
struct EnemyDamageParticipant {Vec3 position;PlayerFrameState* frame=nullptr;GameGauge* gauge=nullptr;u8 character=0,bomb=0,focused=0;};
#endif
struct EnemyDamageActions {
    virtual ~EnemyDamageActions()=default;
    virtual i32 damage(const Vec3& position,const Vec3& size,i32& time_items,i32& bomb_hit)=0;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    virtual u32 participant_count()const=0;
    virtual bool participant(u32 seat,EnemyDamageParticipant&)=0;
    virtual i32 participant_damage(u32 seat,const Vec3& position,const Vec3& size,i32& time_items,i32& bomb_hit)=0;
#endif
};
// Damage and target-selection block of the original manager (42d06d..42d4df).
// Visibility, enemy contact and trail contact are earlier manager phases.
bool damage_enemy(EclVm&,const EnemyDamageContext&,PlayerFrameState&,GameValues&,i32& bomb_hit,EnemyDamageActions&);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
bool damage_enemy_multiplayer(EclVm&,u8 time_spell,u8 spell_bomb_damage,GameValues&,i32& bomb_hit,u32& owner,EnemyDamageActions&);
#endif
}
