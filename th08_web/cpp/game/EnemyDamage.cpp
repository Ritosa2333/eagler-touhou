#include "EnemyDamage.hpp"
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#include "../multiplayer/Balance.hpp"
#endif
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
            enemy.life=wrapping_sub(enemy.life,damage);enemy.last_damage=damage;damage_familiar_parent(enemy,damage,input.bomb);
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
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
bool damage_enemy_multiplayer(EclVm& enemy,u8 time_spell,u8 spell_bomb_damage,GameValues& values,i32& bomb_hit,u32& owner,EnemyDamageActions& actions){
    enemy.last_damage=0;bomb_hit=0;owner=0;
    if(!(enemy.flags&0x40))return false;
    i32 ordinary=0,bomb=0,max_source=-1;
    for(u32 seat=0;seat<actions.participant_count();++seat){
        EnemyDamageParticipant pilot;
        if(!actions.participant(seat,pilot)||!pilot.frame)continue;
        i32 local_bomb=0,damage=0;
        if(!(time_spell&&enemy.parent&&pilot.bomb))
            damage=actions.participant_damage(seat,enemy.resolved_position,enemy.hitbox,enemy.time_items,local_bomb);
        if(enemy.low_damage_hitbox.x>0){
            const i32 low=actions.participant_damage(seat,enemy.resolved_position,enemy.low_damage_hitbox,enemy.time_items,local_bomb);
            if(!local_bomb)damage=(Extended::from_int(damage)+Extended::from_int(low)/number(pilot.character==3||pilot.character==11?6.5f:1.7f)).truncate_int();
        }
        if(damage>max_source){max_source=damage;owner=seat;}
        if(local_bomb){bomb=wrapping_add(bomb,damage);bomb_hit=1;}
        else ordinary=wrapping_add(ordinary,damage);
        auto& target=*pilot.frame;
        if(enemy.flags&2){
            const float old=std::fabs(Scalar::sub(target.homing_target.x,pilot.position.x));
            const float current=std::fabs(Scalar::sub(enemy.resolved_position.x,pilot.position.x));
            if(!target.boss_target||current<old)target.homing_target=enemy.resolved_position;
            target.boss_target=1;
        }
        if(!target.boss_target&&target.homing_target.y<enemy.resolved_position.y)target.homing_target=enemy.resolved_position;
        if(std::fabs(Scalar::sub(enemy.resolved_position.x,pilot.position.x))<64&&!enemy.parent){
            const auto* previous=static_cast<const EclVm*>(target.target_reference);
            if(!previous||enemy.resolved_position.y<previous->position.y)target.target_reference=&enemy;
        }
    }
    i32 raw=wrapping_add(ordinary,bomb);
    const bool hit=raw>0;
    if(!hit)return false;
    if(raw>70){ordinary=i32(i64(ordinary)*70/raw);bomb=70-ordinary;raw=70;}
    values.add_score(raw/5*10);
    if(enemy.flags&8){
        if(time_spell){
            ordinary=ordinary>7?ordinary/7:ordinary?1:0;
            if(!spell_bomb_damage||enemy.parent)bomb=0;
            else bomb=bomb>2?(Extended::from_int(bomb)/number(2.5f)).truncate_int():bomb?1:0;
        }
        i32 effective=wrapping_add(ordinary,bomb);
        if(enemy.damage_protection.current>0)effective=enemy.flags&2?effective/9:0;
        if(enemy.flags&2)effective=multiplayer::boss_damage(effective,actions.participant_count());
        enemy.life=wrapping_sub(enemy.life,effective);enemy.last_damage=effective;
        if(ordinary>0&&effective>0){
            const i32 parent_share=bomb?i32(i64(effective)*ordinary/(ordinary+bomb)):effective;
            damage_familiar_parent(enemy,parent_share,false);
        }
    }
    return hit;
}
#endif
}
