#include "EnemyDeath.hpp"
#include <cmath>
namespace th15 {
int EnemyDeath::fail(EnemyRuntime& runtime,const char* message){runtime.error=message;return -2;}
int EnemyDeath::execute(EnemyRuntime& runtime,float rate){
    auto& enemy=runtime.state;
    if(enemy.death_sound>=0&&!host.sound(enemy.death_sound,enemy.motion.position))return fail(runtime,"Enemy death sound unavailable");
    if(enemy.death_animation>=0){
        EnemyDeathEffect effect;effect.resource=enemy.death_animation_resource;effect.script=enemy.death_animation;effect.position=enemy.motion.position;
        const float x=float(enemy.last_hit_position.x-enemy.motion.position.x),y=float(enemy.last_hit_position.y-enemy.motion.position.y);
        if(float(float(x*x)+float(y*y))>=.0400000028312206268310546875f)effect.rotation=float(std::atan2(double(float(enemy.motion.position.y-enemy.last_hit_position.y)),double(float(enemy.motion.position.x-enemy.last_hit_position.x))));
        if(!host.effect(effect))return fail(runtime,"Enemy death animation unavailable");
    }
    if(enemy.primary_drop){ItemSpawnRequest request;request.type=enemy.primary_drop;request.position=enemy.motion.position;if(!host.spawn_item(request))return fail(runtime,"Enemy primary item drop unavailable");}
    if(!drop_enemy_items(enemy,random,host))return fail(runtime,"Enemy additional item drops unavailable");enemy.primary_drop=0;
    if(enemy.chapter_contribution>0&&enemy.chapter==world.current_chapter){world.chapter_defeated=wrapping_add(world.chapter_defeated,enemy.chapter_contribution);enemy.chapter_contribution=0;}
    if(!enemy.death_script.empty()){
        const std::string routine=enemy.death_script;if(!runtime.switch_routine(routine))return -2;
        // Death scripts execute at time zero without advancing the ECL clock.
        if(runtime.scripts.step(0,rate)==-2){runtime.error=runtime.scripts.error;return -2;}
        // Only the first original byte is cleared; the typed string is no
        // longer callable after this execution.
        enemy.death_script.clear();
    }
    if(!host.callback(runtime))return fail(runtime,"Enemy death callback unavailable");return 1;
}
}
