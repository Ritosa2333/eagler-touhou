#include "EnemyFrame.hpp"
#include "AnmFile.hpp"
namespace th10 {
namespace {
Extended abs_value(Extended value){return value<number(0.0f)?-value:value;}
}
// 0x40dc80. Movement, ECL, damage, phase changes, contact, direction animation,
// target selection, hit flash and timers execute in the original order.
i32 EnemyState::update(EnemyFrameEnvironment& env){
    const auto movement=advance_movement(env);
    if(movement==EnemyMotionResult::AlreadyUpdated)return 0;
    if(movement==EnemyMotionResult::Despawn)return -1;
    const auto change_animation=[&](i32 script){
        if(auto* vm=env.registry->find(animations[0]))vm->animation_file->start_script(*vm,script,*env.animations,*env.started_animations);
    };
    if(flags&0x100000){
        if(*env.alternate_active){
            if(!(flags&0x200000)){base_animation=alternate_animation;change_animation(alternate_animation);flags|=0x200001;}
        }else if(flags&0x200000){base_animation=saved_animation;change_animation(saved_animation);flags&=~0x200001u;}
    }
    if(script_owner->update_threads(*lifetime.rate,*env.scripts))return -1;
    flags&=~0x2000u;
    const auto switch_phase=[&](const char* name){script_owner->reset_threads(*env.scripts);script_owner->select_subroutine(name);};
    if(!(flags&0x11)){
        auto damage=env.player_damage(current.position,hitbox);
        if(*env.player_state==2||*env.player_state==0)damage/=5;
        if(damage){
            if((*env.phase.spell_flags&1)&&(flags&0x8000)){damage/=5;if(damage<1)damage=1;}
            if(!(flags&8)&&damage_immunity.current<1)health=wrapping_add(health,static_cast<i32>(0u-static_cast<u32>(damage)));
            if(const auto* name=check_interrupts(env.phase)){
                switch_phase(name);if(script_owner->update_threads(*lifetime.rate,*env.scripts))return -1;
            }
            if(health<1&&!(flags&0x40)){
                *env.score=wrapping_add(*env.score,score/10);if(*env.score>999999999)*env.score=999999999;
                if(env.destroy(*this))return 1;
            }
            flags|=0x2000;
        }
        *env.enemy_activity=1;
    }
    if(const auto* name=check_interrupts(env.phase))switch_phase(name);
    if(!(flags&0x12)&&collision_immunity.current<1)env.player_collision(current.position,collision_box);
    if(flags&0x1000){
        const auto x=number(current.velocity.x);
        const i32 next=x<number(-0.1f)?-1:number(0.1f)<x?1:0;
        if(direction!=next){
            i32 animation=0;
            if(direction==-1)animation=next==0?3:2;
            else if(direction==0)animation=next==-1?1:2;
            else if(direction==1)animation=next==0?4:1;
            direction=next;change_animation(wrapping_add(base_animation,animation));
        }
    }
    for(unsigned i=0;i<8;++i)env.registry->set_position(animations[i],current.position,!(flags&0x40000));
    if(!(flags&0x11)&&!(flags&0xc0000)){
        const auto player_x=number(env.player_position->x);
        if(!*env.player_target||abs_value(number((*env.player_target)->state.current.position.x)-player_x)<abs_value(number(current.position.x)-player_x)){
            if(!*env.player_target_seen)*env.player_target=reinterpret_cast<Enemy*>(script_owner);
            *env.player_target_seen=1;
        }
    }
    auto* vm=env.registry->find_and_clear(animations[0]);
    if(flash_frames){
        vm->flags&=~0x8000u;if(flags&0x8000)*env.boss_hp_flags&=~0x8000u;
        flash_frames=wrapping_add(flash_frames,-1);
    }else if(flags&0x2000){
        vm->secondary_color=0xff0000ff;vm->flags|=0x8000;flash_frames=4;
        const auto spell=*env.phase.spell_flags;
        const bool low_health=(flags&0x8000)&&(!((spell&1)&&(spell&8)))&&health_to_interrupt<=(spell&1?299:899);
        env.play_sound(low_health?0x23:0x13,current.position.x);
    }
    if(damage_immunity.current>0)damage_immunity.advance(-1);
    if(collision_immunity.current>0)collision_immunity.advance(-1);
#ifdef TH_ENABLE_THPRAC
    // 0x40e5b0: F4 holds the lifetime count without stopping the fractional
    // accumulation, exactly like the upstream `inc edx` -> nop patch.
    if(env.time_locked())lifetime.tick_time_locked();else lifetime.tick();
#else
    lifetime.tick();
#endif
    return 0;
}
}
