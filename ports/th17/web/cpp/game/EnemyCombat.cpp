#include "EnemyCombat.hpp"
namespace th17 {
bool EnemyCombat::shot_damage(EnemyState& enemy,bool special,EnemyShotDamage& hit){
    const auto query=enemy_shot_query(enemy,special);const bool changed=player_frame.current!=player_frame.previous;i32 bomb=0;
    if(changed&&!world.bomb_damage(query,bomb))return false;
    const auto result=damage.query(query,changed,bomb);if(!damage.error.empty())return false;
    hit.amount=result.amount;hit.direct=result.direct;if(result.position_written)hit.position=result.position;return true;
}
bool EnemyCombat::contact(EnemyState& enemy,AnmVm* animation,i32& result){
    bool handled=false;if(!world.contact_override(enemy,animation,result,handled))return false;
    if(!handled)result=i32(enemy_player_contact(enemy,animation,collision,&player));return true;
}
}
