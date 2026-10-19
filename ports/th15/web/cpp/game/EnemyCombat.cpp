#include "EnemyCombat.hpp"
namespace th15 {
bool EnemyCombat::shot_damage(EnemyState& enemy,bool special,EnemyShotDamage& hit){
    const auto query=enemy_shot_query(enemy,special);
    const bool changed=player_frame.current!=player_frame.previous;i32 bomb_damage=0;
    if(changed&&!world.bomb_damage(query,bomb_damage))return false;
    const auto result=damage.query(query,changed,bomb_damage);
    if(!damage.error.empty())return false;
    hit.amount=result.amount;hit.direct=result.direct;
    if(result.position_written)hit.position=result.position;
    return true;
}
bool EnemyCombat::contact(EnemyState& enemy,AnmVm* animation,i32& result){
    bool handled=false;if(!world.contact_override(enemy,animation,result,handled))return false;
    if(!handled)result=i32(enemy_player_contact(enemy,animation,collision,&player));
    return true;
}
bool EnemyCombat::animation_region_damage(EnemyState& enemy,const AnmVm* animation,i32 incoming,i32& amount){
    amount=incoming;if(!animation){enemy.animation_handles[0]=0;return true;}
    DamageQuery query;query.position=enemy.motion.position;query.position.y=float(query.position.y+24.f);query.target=signed_bits(enemy.id);query.rectangle=true;query.size={float(animation->visual.scale.x*192.f),float(animation->visual.scale.y*32.f)};query.angle=animation->variables.rotation.z;
    const bool changed=player_frame.current!=player_frame.previous;
    const auto hit=[&](i32& value){i32 bomb=0;if(changed&&!world.bomb_damage(query,bomb))return false;const auto result=damage.query(query,changed,bomb);if(!damage.error.empty())return false;value=result.amount;return true;};
    i32 first=0,second=0;if(!hit(first))return false;query.position.y=float(query.position.y+32.f);query.rectangle=false;query.radius=48;query.angle=0;if(!hit(second))return false;amount=wrapping_add(incoming,wrapping_add(first,second));return true;
}
}
