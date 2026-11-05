#include "EnemyDamage.hpp"
namespace th15 {
void EnemyState::apply_damage(i32 amount)noexcept{
    cumulative_damage=wrapping_add(cumulative_damage,amount);
    if(life_flags&1){life_budget=wrapping_sub(life_budget,amount);life=wrapping_add(life_threshold,wrapping_sub(life_budget,wrapping_mul(life_threshold,7))/7);}
    else life=wrapping_sub(life,amount);
}
int EnemyDamage::fail(EnemyRuntime& runtime,const char* message){runtime.error=message;return -2;}
int EnemyDamage::interrupt(EnemyRuntime& runtime,const std::string& name,float rate,bool immediate){
    runtime.state.age_timer.set(0);
    // The source string can belong to the entity; preserve it during restart.
    const std::string routine=name;if(!runtime.switch_routine(routine))return -2;
    if(immediate){const int result=runtime.scripts.step(rate,rate);if(result==-2){runtime.error=runtime.scripts.error;return -2;}if(result)return -1;}
    return 0;
}
int EnemyDamage::update(EnemyRuntime& runtime,float rate){
    auto& enemy=runtime.state;
    if((enemy.flags&0x10000000)&&world.mode!=3&&world.game_state==1){
        if(!(enemy.flags&0x20000000)){enemy.animation_base=enemy.inactive_animation;if(!animations.rebind(enemy.animation_handles[0],enemy.inactive_animation))return fail(runtime,"Enemy inactive animation unavailable");enemy.flags|=0x20000001;}
    }else if(world.game_state!=1&&(enemy.flags&0x20000000)){
        enemy.animation_base=enemy.normal_animation;if(!animations.rebind(enemy.animation_handles[0],enemy.normal_animation))return fail(runtime,"Enemy normal animation unavailable");enemy.flags&=~0x20000001u;
    }
    if(enemy.flags&0x800){EnemyShotDamage hit;hit.position=enemy.last_hit_position;if(!host.shot_damage(enemy,true,hit))return fail(runtime,"Enemy special shot collision unavailable");enemy.last_hit_position=hit.position;if(hit.amount&&hit.direct){const int result=host.die(runtime,rate);if(result)return result;}}
    enemy.flags&=~0x200000u;
    if(!(enemy.flags&0x21)){
        EnemyShotDamage hit;hit.position=enemy.last_hit_position;if(enemy.hitbox.x>0){if(!host.shot_damage(enemy,false,hit))return fail(runtime,"Enemy shot collision unavailable");enemy.last_hit_position=hit.position;}
        i32 extra=0;if(!host.additional_damage(enemy,hit.amount,extra))return fail(runtime,"Enemy additional damage callback unavailable");i32 damage=wrapping_add(hit.amount,extra);
        if(enemy.pending_damage>0){damage=wrapping_add(damage,enemy.pending_damage);enemy.pending_damage=0;}
        if(world.player_damage_state==0||world.player_damage_state==2)damage/=5;
        if(world.damage_disabled||(world.practice&&world.practice->cheat(PracticeEnemyInvincible)))damage=0;
        else if(damage>0){if(hit.direct){const i32 contribution=damage<enemy.life?damage:wrapping_add(enemy.life,wrapping_sub(damage,enemy.life)/4);world.shot_damage=wrapping_add(world.shot_damage,contribution);}else world.other_damage=wrapping_add(world.other_damage,damage);}
        if(world.game_state==1&&enemy.damage_multiplier<1){if(damage&&enemy.damage_multiplier<=0&&!host.sound(36,enemy.motion.position))return fail(runtime,"Enemy damage sound unavailable");damage=truncate_int(float(float(damage)*enemy.damage_multiplier));}
        if(damage){
            if((world.spell_flags()&33)==33)damage/=30;
            if(!(enemy.flags&16)&&enemy.collision_timer.current<=0)enemy.apply_damage(damage);else enemy.cumulative_damage=wrapping_add(enemy.cumulative_damage,damage);
            enemy.damage_timer.set(30);
            if(const auto* name=enemy.check_interrupt(world)){const int result=interrupt(runtime,*name,rate,true);if(result)return result;}
            if(!(enemy.flags&128)&&enemy.life<=0){const int result=host.die(runtime,rate);if(result)return result;}
            enemy.flags|=0x200000;
        }
    }
    if(enemy.life_flags&2){const int result=host.die(runtime,rate);if(result)return result;}
    if(const auto* name=enemy.check_interrupt(world)){const int result=interrupt(runtime,*name,rate,false);if(result)return result;}
    if(!(enemy.flags&0x22)&&enemy.invulnerability_timer.current<=0&&!(enemy.flags&0x4000000)){
        i32 result=0;if(!host.contact(enemy,animations.find(enemy.animation_handles[0]),result))return fail(runtime,"Enemy contact collision unavailable");
        if((enemy.flags&0x200)&&result==2&&enemy.age_timer.current%6==0&&!host.graze(world.player_position))return fail(runtime,"Enemy contact graze unavailable");
    }
    auto* vm=animations.find(enemy.animation_handles[0]);if(!vm)enemy.animation_handles[0]=0;
    else if(enemy.damage_flash_frames){vm->visual.flags&=~0x60000u;enemy.damage_flash_frames=wrapping_add(enemy.damage_flash_frames,-1);}
    else{
        const bool fourth=enemy.age_timer.current%4==0,spell=world.spell_flags()&1,survival=spell&&(world.spell_flags()&8);
        auto tint=[&](u32 colour){vm->visual.secondary_color=colour;vm->visual.flags=(vm->visual.flags&~0x40000u)|0x20000;};
        if(enemy.flags&0x80000000){if(fourth)tint(0xffff00ff);else vm->visual.flags&=~0x60000u;}
        if((enemy.flags&0x200000)&&!(enemy.flags&0x2000)){
            tint(0xff0000ff);enemy.damage_flash_frames=4;i32 sound=enemy.hit_sound;
            if(sound<0)sound=(enemy.flags&0x40800000)&&!survival&&enemy.phase_life<(spell?200:900)?35:34;
            if(!host.sound(sound,enemy.motion.position))return fail(runtime,"Enemy hit sound unavailable");
        }else if(!fourth)vm->visual.flags&=~0x60000u;
        else if((enemy.flags&0x40800000)&&!survival&&enemy.phase_life<(spell?100:500))tint(0xff0000ff);
    }
    if(enemy.damage_timer.current>0)enemy.damage_timer.decrement(&rate);return 0;
}
}
