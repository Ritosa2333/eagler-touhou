#pragma once
#include "EnemyState.hpp"
#include "DamageSources.hpp"
#include "PlayerCollision.hpp"
#include "AnmVm.hpp"
namespace th15 {
DamageQuery enemy_shot_query(const EnemyState&,bool preview)noexcept;
struct EnemyContactShape {Vec3 position{};float angle=0,length=0,width=0,radius=0;bool rectangle=false;};
EnemyContactShape enemy_contact_shape(const EnemyState&,const AnmVm*)noexcept;
PlayerContact enemy_player_contact(const EnemyState&,const AnmVm*,const PlayerCollision&,PlayerDamageHost* damage=nullptr);
}
