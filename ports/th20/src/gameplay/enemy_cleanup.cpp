#include "enemy_cleanup.hpp"
namespace th20::source::gameplay {
void clear_enemy_group(EnemyController& c,EnemyClearKind kind,int group,EnemyCleanupServices& env){
    for(scheduler::Iterator it(c.enemies.sentinel.next);it.current;it.advance()){
        auto& enemy=*reinterpret_cast<Enemy*>(it.current->value);auto& value=enemy.state;
        const auto first=value.fields_2c8[0],second=value.fields_2c8[1];
        if(!((!(first&0x4a0u)&&!(second&0xc0u))||(first&0x100u)))continue;
        if(kind==EnemyClearKind::group&&recovered::signed_bits(value.fields_1c[5])!=group)continue; //Enemy+b8, State+30
        if(kind==EnemyClearKind::exclude_special&&(second&0x40000000u))continue;
        reset_enemy_pattern(value.pattern_1a8);value.vector_6c.x=0;value.vector_6c.y=192;value.fields_250[13]=0;
        if(kind==EnemyClearKind::no_effect)value.fields_250[11]=0xffffffffu; //Enemy+304
        env.defeat(enemy);
        for(auto& animation:value.animations)env.delete_animation(animation.handle);
        value.animations.clear();value.fields_2c8[1]|=0x200u;
    }
    recovered::timer_tick(c.data.timer_8c,env.timer_rate());
}
}
