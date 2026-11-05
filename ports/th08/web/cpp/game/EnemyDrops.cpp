#include "EnemyDrops.hpp"
namespace th08 {
void drop_enemy_items(EclVm& enemy,bool bomb,EnemyDropSequence& sequence,Rng& random,EnemyDropActions& actions){
    static constexpr u8 items[32]={0,0,1,0,1,0,0,0,1,1,0,0,1,1,1,0,1,0,1,0,1,0,1,0,1,0,0,1,1,1,0,0};
    if(enemy.item_reward>=0){actions.effect(i32(enemy.death_effects[1])+4,enemy.resolved_position,3,0xffffffff);actions.item(enemy.resolved_position,enemy.item_reward,bomb);}
    else if(enemy.item_reward==-1){
        if(sequence.enemies%3==0){
            if(sequence.item>=32){enemy.invalid=true;return;}
            actions.effect(i32(enemy.death_effects[1])+4,enemy.resolved_position,6,0xffffffff);actions.item(enemy.resolved_position,items[sequence.item],bomb);
            if(++sequence.item>=32)sequence.item=0;
        }
        ++sequence.enemies;
    }
    const auto scatter=[&](){Vec3 p=enemy.resolved_position;p.x=(random.unit()*number(128)-number(64)+number(p.x)).to_float();p.y=(random.unit()*number(128)-number(64)+number(p.y)).to_float();return p;};
    for(i32 i=0;i<enemy.power_items;++i){const Vec3 p=scatter();actions.item(p,actions.power()<128?0:1,0);}enemy.power_items=0;
    for(i32 i=0;i<enemy.point_items;++i)actions.item(scatter(),1,0);enemy.point_items=0;
}
}
