#include "EnemyFrame.hpp"
#include "Trace.hpp"
#include <cmath>
#include <algorithm>
namespace th11 {
u32 enemy_frame_entries=0;
u32 enemy_frame_script_steps=0;
i32 enemy_death(EnemyState& e,EnemyAnimations& animations,EnemyFrameWorld& world,EnemyScriptServices* services){
    if constexpr(kPortTitle==16)trace_point=9000;
    if(e.death_sound>=0&&!world.sound(e.death_sound,e.current.position.x))return -2;
    if(e.death_animation>=0){
        if constexpr(kPortTitle==16)trace_point=9001;
        const u32 id=kPortTitle==14?animations.th14_death_effect(e):animations.effect_at(e.death_animation_file,e.death_animation,3,e.current.position,true);
        if(!id)return -2;
    }
#if PORT_TITLE == 16
    trace_point=9002;if(!enemy_th16_time_drop_decay(e))return -2;trace_point=9003;if(!world.drop_items_now_th16(e))return -2;
#else
    if(!world.drop_items(e))return -2;
#endif
    e.drops.primary=0;
    if constexpr(kPortTitle==16){if(!services)return -2;enemy_th16_chapter_death(e,services->commands);}
#if PORT_TITLE == 14
    if(e.th14_death_script){
        // 424c21..424c81 releases secondary contexts, resets the root,
        // selects the death subroutine and executes it once at elapsed zero.
        if(!e.script_owner||!services)return -2;
        auto& owner=*e.script_owner;
        owner.script.reset_threads(*services);owner.script.root.stack.top=owner.script.root.stack.frame_base=0;
        owner.script.select_subroutine(e.th14_death_script);
        const i32 result=owner.script.update_threads(0,*services);e.th14_death_script=nullptr;
        if(result==-2)return -2;
    }
#endif
#if PORT_TITLE == 16
    if(e.th16_death_script[0]){
        trace_point=9005;
        if(!e.script_owner||!services)return -2;
        auto& owner=*e.script_owner;
        owner.script.reset_threads(*services);owner.script.root.stack.top=owner.script.root.stack.frame_base=0;
        owner.script.select_subroutine(e.th16_death_script);
        const i32 result=owner.script.update_threads(0,*services);e.th16_death_script[0]=0;
        if(result==-2)return -2;
    }
#endif
    return 1;
}
i32 enemy_frame(EnemyScriptServices& script,EnemyFrameWorld& world){
    ++enemy_frame_entries;
    auto& e=script.globals.enemy;auto& environment=script.globals.environment;
#if PORT_TITLE == 14
    // Retail 424710 wraps the entire 424810 update, including movement,
    // callbacks and both enemy clocks. Restore on every exit from that update.
    struct KnifeRateScope {
        EnemyState& enemy;float& rate;float* anm_rate;
        float saved_rate,saved_anm_rate;bool slowed;
        ~KnifeRateScope(){if(slowed){rate=saved_rate;if(anm_rate)th14_rate_store(anm_rate,saved_anm_rate);enemy.th14_knife_clock_applied=true;}}
    } knife_rate{e,environment.rate,script.commands.th14_rate_target,environment.rate,
        script.commands.th14_rate_target?th14_rate_load(script.commands.th14_rate_target):environment.rate,e.th14_knife_slowdown>0};
    if(auto* anm=script.commands.animations)if(knife_rate.slowed||e.th14_knife_clock_applied)
        for(u32 i=0;i<16;++i)if(auto* vm=anm->lookup(e.animation_handle(i)))vm->slowdown_factor=knife_rate.slowed?e.th14_knife_slowdown:0;
    if(knife_rate.slowed){environment.rate=enemy_th14_knife_rate(knife_rate.saved_rate,e.th14_knife_slowdown);if(knife_rate.anm_rate)th14_rate_store(knife_rate.anm_rate,environment.rate);}
#endif
    if constexpr(kPortTitle==16){
        // Retail41d2eb..41d301 guards and marks this frame before movement.
        if(e.flags&0x40000)return 0;e.flags|=0x40000;
    }else if(e.flags&(kPortTitle==14?0x40000:0x4000))return 0;
    if(const i32 movement=e.update_movement(&environment.rate,environment.camera_delta))return movement==-2?-2:-1;
    if constexpr(kPortTitle==16)trace_point=9011;
    auto* animations=script.commands.animations;if(!animations)return -2;
    if constexpr(kPortTitle==14){if(!animations->update_direction(e))return -2;if(enemy_th14_visibility(e))return -1;}
    if constexpr(kPortTitle==16){
        if(!animations->update_direction(e)){trace_point=9025;return -2;}
        if(enemy_th16_visibility(e))return -1;
    }
    if constexpr(kPortTitle!=14&&kPortTitle!=16)if(e.flags&0x1000000){
        if(world.special_active&&!(e.flags&0x2000000)){
            e.base_animation=e.alternate_animation;if(!animations->rebind(e.animations[0],e.alternate_animation))return -2;e.flags|=0x2000001;
        }else if(!world.special_active&&(e.flags&0x2000000)){
            e.base_animation=e.saved_animation;if(!animations->rebind(e.animations[0],e.saved_animation))return -2;e.flags&=0xfdfffffe;
        }
    }
    float elapsed=th14_rate_load(e.lifetime.rate);
    i32 result=script.update(elapsed);
    ++enemy_frame_script_steps;if(result)return result==-2?-2:-1;
    if constexpr(kPortTitle==16)trace_point=9012;
    if(u32(e.tick_callback)){if(!world.tick_callback(e,result)){if constexpr(kPortTitle==16)trace_point=9021;return -2;}if(result)return -1;}
    const bool enlarged=(world.spell_flags&1)&&world.spell_id>=0xa5&&world.spell_id<=0xae&&world.special_active&&!(world.player_flags&4);
    if((e.flags&0x80000)&&!(e.flags&0x20)){
        auto* vm=animations->lookup(e.animations[9]);
        if(enlarged){if(!vm){e.animations[9]=animations->effect(2,0x15,9);if(!e.animations[9])return -2;}
            animations->position(e.animations[9],e.current.position,true);e.hitbox={192,192};
        }else if(vm){animations->interrupt(e.animations[9],1);e.animations[9]=0;e.hitbox={48,48};}
    }
#if PORT_TITLE == 14
    // Retail42396c..423a02 probes flag800 enemies before the ordinary21 guard.
    // The collector still records the last contact, but skips hit callbacks/score.
    if(e.flags&0x800){
        i32 probe_damage=0;bool bomb_contact=false;
        if(!world.th14_probe_damage(e,probe_damage,bomb_contact))return -2;
        if(probe_damage&&bomb_contact){result=enemy_death(e,*animations,world,&script);if(result)return result;}
    }
#endif
    e.flags&=~u32(kPortTitle==14||kPortTitle==16?0x200000:0x20000);
    EnemyPhaseState phase{e.lifetime.rate,&world.countdown_seconds,&world.countdown_hundredths,&world.spell_flags,&world.spell_elapsed,&world.spell_bonus,&world.shared_spell_counter(),world.special_active,environment.character,environment.subtype};
    const auto transition=[&](const char* name){if constexpr(kPortTitle!=14&&kPortTitle!=16)e.lifetime.set(0,e.lifetime.rate);auto& owner=*e.script_owner;owner.script.reset_threads(script);owner.script.select_subroutine(name);};
#if PORT_TITLE == 16
    phase.chapter=&script.commands.stage_section;phase.chapter_removed=&script.commands.th16_chapter_removed;
    if(const char* name=e.check_timed_interrupt16(phase)){transition(name);result=script.update(elapsed);if(result)return result==-2?-2:-1;}
#endif
    if(!(e.flags&0x21)){
        if constexpr(kPortTitle==16)trace_point=9014;
        i32 damage=0;
#if PORT_TITLE == 14
        // 423a3c..423a96 first collects the ordinary hitbox, then passes its
        // result in EDX to callback 633 and adds that callback's return.
        if(e.hitbox.x>0&&!world.shot_damage(e,damage))return -2;
        if(u32(e.damage_callback)){i32 extra=damage;if(!world.damage_callback(e,extra))return -2;damage=wrapping_add(damage,extra);}
#else
        if(u32(e.damage_callback)?!world.damage_callback(e,damage):!world.shot_damage(e,damage)){if constexpr(kPortTitle==16)trace_point=9022;return -2;}
#endif
        if(world.player_state==0||world.player_state==2)damage/=5;
#if PORT_TITLE == 14
        // Retail423ad2..423ae2 collects hits/callbacks first, then zeroes
        // damage while the live dialogue owner at4db550+194 is present.
        if(world.th14_damage_dialogue())damage=0;
#endif
        if((world.spell_flags&1)&&world.spell_id>=0x9e&&world.spell_id<=0xa1&&(world.special_active||world.special_ending)&&!(world.player_flags&4))damage/=5;
        if constexpr(kPortTitle==14){
            bool resistant_sound=false;damage=enemy_th14_bomb_damage(e,damage,world.special_active,resistant_sound);
            if(resistant_sound&&!world.sound(36,e.current.position.x))return -2;
        }
        if(!enlarged&&damage){
            // TH14 423b8a..423b9b is signed division by30 (88888889,
            // add numerator, SAR4), before the independent health scaling.
            if((world.spell_flags&0x21)==0x21)damage/=kPortTitle==14?30:7;
            if(!(e.flags&0x10)&&e.damage_immunity.current<=0)e.take_damage(damage);
#if PORT_TITLE == 16
            else e.th16_damage_total=wrapping_add(e.th16_damage_total,damage);
            if(e.th16_damage_drop_step>0&&e.th16_damage_drop_next<e.th16_damage_total&&!world.season_damage_drop(e))return -2;
            const char* name=e.check_health_interrupt16(phase);
#else
            const char* name=e.check_interrupts(phase);
#endif
            if(name){transition(name);result=script.update(elapsed);if(result)return result==-2?-2:-1;}
            if(e.health<=0&&!(e.flags&0x80)){
                // TH14 423c6e calls 424af0 directly. Its complete death path
                // drops items/effects and invokes callbacks without adding spawn score.
                if constexpr(kPortTitle!=14)if(!world.add_score(e.score))return -2;
                result=enemy_death(e,*animations,world,&script);if(result)return result;
            }
            // TH14 423c81 marks damage with 0x200000; 0x20000 is movement clamping.
            e.flags|=kPortTitle==14||kPortTitle==16?0x200000:0x20000;
        }
    }
    if constexpr(kPortTitle!=16)if(const char* name=e.check_interrupts(phase))transition(name);
    if(!(e.flags&0x22)&&e.collision_immunity.current<=0&&!(e.flags&(kPortTitle==14?0x4000000u:0x400000u))){
        if constexpr(kPortTitle==16)trace_point=9016;
        if(u32(e.collision_callback)){if(!world.collision_callback(e))return -2;}
        else{ i32 collision=0;if(!world.player_collision(e,float(double(e.collision_box.x)*.5),collision))return -2;
            // 423ea1..423ebe uses phase+2c0 modulo6, then450410, rather
            // than the legacy lifetime modulo3 and communication reward.
            const i32 graze_clock=kPortTitle==14?e.timer_2bc.current:e.lifetime.current;
            if((e.flags&0x200)&&collision==2&&graze_clock%(kPortTitle==14?6:3)==0&&!world.graze(e))return -2;
        }
    }
    if constexpr(kPortTitle!=14)if(!animations->update_direction(e)){if constexpr(kPortTitle==16)trace_point=9018;return -2;}
    if(!animations->update_positions(e)){if constexpr(kPortTitle==16)trace_point=9019;return -2;}
    if constexpr(kPortTitle==16)trace_point=9017;
    if(!(e.flags&0xc00021)&&(!world.target||std::abs(float(double(world.target->state.current.position.x)-environment.player_position.x))<std::abs(float(double(e.current.position.x)-environment.player_position.x)))){
        if(!world.target_locked)world.target=e.script_owner;world.target_locked=true;
    }
    if(auto* vm=animations->lookup(e.animations[0])){
        if constexpr(kPortTitle==14){
            const i32 sound=enemy_th14_hit_flash(e,*vm,world.spell_flags);
            if(sound>=0&&!world.sound(sound,e.current.position.x))return -2;
        }else if(e.damage_flash){vm->flags&=~0x8000u;if(e.flags&0x80000)world.bomb_animation_flags&=~0x8000u;e.damage_flash=wrapping_add(e.damage_flash,-1);}
        else{
            const bool flash_frame=e.lifetime.current%4==0;
            if(e.flags&0x8000000){if(flash_frame){vm->flags|=0x8000;vm->secondary_color=0xffff00ff;}else vm->flags&=~0x8000u;}
            if(e.flags&(kPortTitle==14?0x200000:0x20000)){vm->flags|=0x8000;vm->secondary_color=0xff0000ff;e.damage_flash=4;
                const bool low=(e.flags&0x4080000)&&!((world.spell_flags&9)==9)&&e.interrupt_health<((world.spell_flags&1)?200:900);
                const i32 hit_sound=kPortTitle==14?enemy_th14_hit_sound(e,world.spell_flags):(low?0x23:0x13);
                if(!world.sound(hit_sound,e.current.position.x))return -2;
            }else if(!flash_frame)vm->flags&=~0x8000u;
            else if((e.flags&0x4080000)&&!((world.spell_flags&9)==9)&&e.interrupt_health<((world.spell_flags&1)?100:500)){vm->flags|=0x8000;vm->secondary_color=0xff0000ff;}
        }
    }
    if(e.deformation&&!world.deform(e)){if constexpr(kPortTitle==16)trace_point=9023;return -2;}
    if(e.damage_immunity.current>0)e.damage_immunity.advance(-1);
    if(e.collision_immunity.current>0)e.collision_immunity.advance(-1);
#if PORT_TITLE == 14
    e.timer_2bc.tick();
#endif
#if PORT_TITLE == 16
    e.th16_phase_timer.tick();enemy_th16_time_drop_tick(e);
#endif
    e.lifetime.tick();return 0;
}
}
