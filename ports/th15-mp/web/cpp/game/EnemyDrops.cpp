#include "EnemyDrops.hpp"
#include <cmath>
namespace th15 {
bool drop_enemy_items_now(EnemyState& enemy,Rng& random,ItemSpawnHost& host){
    if(enemy.primary_drop){ItemSpawnRequest request;request.type=enemy.primary_drop;request.position=enemy.motion.position;if(!host.spawn_item(request))return false;}
    if(!drop_enemy_items(enemy,random,host))return false;enemy.primary_drop=0;return true;
}
bool drop_enemy_items(EnemyState& enemy,Rng& random,ItemSpawnHost& host){
    constexpr float pi=3.1415927410125732421875f,tau=6.283185482025146484375f,half_pi=1.57079637050628662109375f;
    float angle=float(random.signed_unit()*pi);
    // The original visits fifteen types, then clears all sixteen counters.
    for(i32 type=0;type<15;type++)for(i32 count=0;count<enemy.item_drops[type];count++){
        const float x=float(std::cos(double(angle))*double(enemy.drop_radius.x)),y=float(std::sin(double(angle))*double(enemy.drop_radius.y));
        const float scale=float(float(random.unit()*.5f)+.5f);
        ItemSpawnRequest request;request.type=type+1;request.position={float(enemy.motion.position.x+float(scale*x)),float(enemy.motion.position.y+float(y*scale)),float(enemy.motion.position.z+0.f)};
        if(!host.spawn_item(request))return false;
        const float quarter=float(float(random.signed_unit()*pi)*.25f);angle=float(quarter+float(angle+half_pi));
        i32 iterations=0;while(angle>pi){angle=float(angle-tau);if(iterations++>32)break;}while(angle<-pi){angle=float(angle+tau);if(iterations++>32)break;}
    }enemy.item_drops={};return true;
}
}
