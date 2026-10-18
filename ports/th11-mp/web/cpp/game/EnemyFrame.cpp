#include "EnemyFrame.hpp"
#include <cmath>
namespace th11 {
i32 enemy_death(EnemyState& e,EnemyAnimations& animations,EnemyFrameWorld& world){
    if(e.death_sound>=0&&!world.sound(e.death_sound,e.current.position.x))return -2;
    if(e.death_animation>=0&&!animations.effect_at(e.death_animation_file,e.death_animation,3,e.current.position,true))return -2;
    if(!world.drop_items(e))return -2;e.drops.primary=0;return 1;
}
i32 enemy_frame(EnemyScriptServices& script,EnemyFrameWorld& world){
    auto& e=script.globals.enemy;auto& environment=script.globals.environment;
    if(e.flags&0x4000)return 0;
#ifdef TH11_MULTIPLAYER
    if(world.multiplayer_world())world.multiplayer_enemy_target(e.current.position);
#endif
    if(e.update_movement(&environment.rate,environment.camera_delta))return -1;
    auto* animations=script.commands.animations;if(!animations)return -2;
    if(e.flags&0x1000000){
        if(world.special_active&&!(e.flags&0x2000000)){
            e.base_animation=e.alternate_animation;if(!animations->rebind(e.animations[0],e.alternate_animation))return -2;e.flags|=0x2000001;
        }else if(!world.special_active&&(e.flags&0x2000000)){
            e.base_animation=e.saved_animation;if(!animations->rebind(e.animations[0],e.saved_animation))return -2;e.flags&=0xfdfffffe;
        }
    }
    const float elapsed=*e.lifetime.rate;
    i32 result=script.update(elapsed);if(result)return result==-2?-2:-1;
    if(u32(e.tick_callback)){if(!world.tick_callback(e,result))return -2;if(result)return -1;}
    const bool enlarged=(world.spell_flags&1)&&world.spell_id>=0xa5&&world.spell_id<=0xae&&world.special_active&&!(world.player_flags&4);
    if((e.flags&0x80000)&&!(e.flags&0x20)){
        auto* vm=animations->lookup(e.animations[9]);
        if(enlarged){if(!vm){e.animations[9]=animations->effect(2,0x15,9);if(!e.animations[9])return -2;}
            animations->position(e.animations[9],e.current.position,true);e.hitbox={192,192};
        }else if(vm){animations->interrupt(e.animations[9],1);e.animations[9]=0;e.hitbox={48,48};}
    }
    e.flags&=~0x20000u;
    EnemyPhaseState phase{e.lifetime.rate,&world.countdown_seconds,&world.countdown_hundredths,&world.spell_flags,&world.spell_elapsed,&world.spell_bonus,&world.shared_spell_counter(),world.special_active,environment.character,environment.subtype};
#ifdef TH11_MULTIPLAYER
    phase.multiplayer=world.multiplayer_world();phase.multiplayer_nonshield_bomb=world.special_active&&!(world.player_flags&4);
#endif
    const auto transition=[&](const char* name){e.lifetime.set(0,e.lifetime.rate);auto& owner=*e.script_owner;owner.script.reset_threads(script);owner.script.select_subroutine(name);};
    if(!(e.flags&0x21)){
        i32 damage=0;
        if(u32(e.damage_callback)?!world.damage_callback(e,damage):!world.shot_damage(e,damage))return -2;
#ifdef TH11_MULTIPLAYER
        if(!world.multiplayer_world()){
#endif
            if(world.player_state==0||world.player_state==2)damage/=5;
            if((world.spell_flags&1)&&world.spell_id>=0x9e&&world.spell_id<=0xa1&&(world.special_active||world.special_ending)&&!(world.player_flags&4))damage/=5;
#ifdef TH11_MULTIPLAYER
        }
#endif
        if(!enlarged&&damage){
            if((world.spell_flags&0x21)==0x21)damage/=7;
#ifdef TH11_MULTIPLAYER
            if(world.multiplayer_world())damage=world.multiplayer_boss_damage(e,damage);
#endif
            if(!(e.flags&0x10)&&e.damage_immunity.current<=0)e.take_damage(damage);
            if(const char* name=e.check_interrupts(phase)){transition(name);result=script.update(elapsed);if(result)return result==-2?-2:-1;}
            if(e.health<=0&&!(e.flags&0x80)){
                if(!world.add_score(e.score))return -2;
                result=enemy_death(e,*animations,world);if(result)return result;
            }
            e.flags|=0x20000;
        }
    }
    if(const char* name=e.check_interrupts(phase))transition(name);
    if(!(e.flags&0x22)&&e.collision_immunity.current<=0&&!(e.flags&0x400000)){
        if(u32(e.collision_callback)){if(!world.collision_callback(e))return -2;}
        else{ i32 collision=0;if(!world.player_collision(e,float(double(e.collision_box.x)*.5),collision))return -2;
            if((e.flags&0x200)&&collision==2&&e.lifetime.current%3==0&&!world.graze(e))return -2;
        }
    }
    if(!animations->update_direction(e))return -2;animations->update_positions(e);
#ifdef TH11_MULTIPLAYER
    if(world.multiplayer_world())world.multiplayer_shot_target(e);
#endif
    if(!(e.flags&0xc00021)&&(!world.target||std::abs(float(double(world.target->state.current.position.x)-environment.player_position.x))<std::abs(float(double(e.current.position.x)-environment.player_position.x)))){
        if(!world.target_locked)world.target=e.script_owner;world.target_locked=true;
    }
    if(auto* vm=animations->lookup(e.animations[0])){
        if(e.damage_flash){vm->flags&=~0x8000u;if(e.flags&0x80000)world.bomb_animation_flags&=~0x8000u;e.damage_flash=wrapping_add(e.damage_flash,-1);}
        else{
            const bool flash_frame=e.lifetime.current%4==0;
            if(e.flags&0x8000000){if(flash_frame){vm->flags|=0x8000;vm->secondary_color=0xffff00ff;}else vm->flags&=~0x8000u;}
            if(e.flags&0x20000){vm->flags|=0x8000;vm->secondary_color=0xff0000ff;e.damage_flash=4;
                const bool low=(e.flags&0x4080000)&&!((world.spell_flags&9)==9)&&e.interrupt_health<((world.spell_flags&1)?200:900);
                if(!world.sound(low?0x23:0x13,e.current.position.x))return -2;
            }else if(!flash_frame)vm->flags&=~0x8000u;
            else if((e.flags&0x4080000)&&!((world.spell_flags&9)==9)&&e.interrupt_health<((world.spell_flags&1)?100:500)){vm->flags|=0x8000;vm->secondary_color=0xff0000ff;}
        }
    }
    if(e.deformation&&!world.deform(e))return -2;
    if(e.damage_immunity.current>0)e.damage_immunity.advance(-1);
    if(e.collision_immunity.current>0)e.collision_immunity.advance(-1);
#ifdef TH_ENABLE_THPRAC
    if(!world.practice_time_lock)
#endif
    e.lifetime.tick();return 0;
}
}
