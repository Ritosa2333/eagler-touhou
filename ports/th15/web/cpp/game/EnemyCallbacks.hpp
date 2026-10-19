#pragma once
#include "EnemyState.hpp"
#include "BulletScene.hpp"
#include "LaserScene.hpp"
namespace th15 {
// Script-selected rules operate on typed game objects, in active-list order.
void inherit_nearby_bullet_motion(BulletManager&)noexcept;
void scale_bullets_near_player(BulletManager&,const PlayerCollision&,float factor,bool dialogue_present)noexcept;
bool cancel_unmarked_bullets(BulletScene&,const Vec3&,float radius);
bool enemy_update_rule(EnemyState&,BulletScene&,LaserScene&,const PlayerCollision&,bool dialogue_present);
bool enemy_additional_damage(EnemyState&,i32 incoming,i32& result)noexcept;
}
