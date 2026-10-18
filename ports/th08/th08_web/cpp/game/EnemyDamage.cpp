#include "EnemyDamage.hpp"
#include <cmath>
namespace th08 {
bool damage_enemy(EclVm& enemy,const EnemyDamageContext& input,PlayerFrameState& player,GameValues& values,i32& bomb_hit,EnemyDamageActions& actions){
    enemy.last_damage=0;if(!(enemy.flags&0x40))return false;
    i32 damage=input.time_spell&&enemy.parent&&input.bomb?0:actions.damage(enemy.resolved_position,enemy.hitbox,enemy.time_items,bomb_hit);
    if(enemy.low_damage_hitbox.x>0){
        const i32 low=actions.damage(enemy.resolved_position,enemy.low_damage_hitbox,enemy.time_items,bomb_hit);
        if(!bomb_hit)damage=(Extended::from_int(damage)+Extended::from_int(low)/number(input.character==3||input.character==11?6.5f:1.7f)).truncate_int();
    }
    const bool hit=damage>0;
    if(hit){
        if(damage>=70)damage=70;values.add_score(damage/5*10);
        if(enemy.flags&8){
            if(input.time_spell){
                if(!bomb_hit)damage=damage>7?damage/7:damage?1:0;
                else if(!input.spell_bomb_damage||enemy.parent)damage=0;
                else damage=damage>2?(Extended::from_int(damage)/number(2.5f)).truncate_int():damage?1:0;
            }
            if(enemy.damage_protection.current>0)damage=enemy.flags&2?damage/9:0;
            // Purple NOPs only the enemy and familiar-parent life subtraction.
            // Keep hit feedback, score and damage calculation in original order.
            if(!enemy.environment||!enemy.environment->practice_enemy_invincible)enemy.life=wrapping_sub(enemy.life,damage);
            enemy.last_damage=damage;damage_familiar_parent(enemy,damage,input.bomb);
        }
    }
    if(enemy.flags&2){
        const float old=std::fabs(Scalar::sub(player.homing_target.x,input.player.x)),current=std::fabs(Scalar::sub(enemy.resolved_position.x,input.player.x));
        if(!player.boss_target||current<old)player.homing_target=enemy.resolved_position;player.boss_target=1;
    }
    if(!player.boss_target&&player.homing_target.y<enemy.resolved_position.y)player.homing_target=enemy.resolved_position;
    if(std::fabs(Scalar::sub(enemy.resolved_position.x,input.player.x))<64&&!enemy.parent){
        const auto* previous=static_cast<const EclVm*>(player.target_reference);
        if(!previous||enemy.resolved_position.y<previous->position.y)player.target_reference=&enemy;
    }
    return hit;
}
}
