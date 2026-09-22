#include "PlayerFrame.hpp"
#ifdef TH_ENABLE_THPRAC
#include "PracticeRuntime.hpp"
#endif
namespace th10 {
namespace {
void set_timer(Timer& timer,u32& flags,i32 frames,const float* rate){
    if(!(flags&1)){flags|=1;timer.rate=rate;}
    timer.previous=wrapping_add(frames,-1);timer.current=frames;timer.fractional=Extended::from_int(frames).to_float();
}
void power_changed(Player& player,PlayerFrameEnvironment& env){
    env.update_options(player);const i32 power=env.economy->power;env.update_power(power/20,(power%20)*100/20);
}
void consume_bomb(Player& player,PlayerFrameEnvironment& env){
    env.start_bomb();
#ifdef TH_ENABLE_THPRAC
    // thprac_th10.cpp:2267-2268 (0x4259CF / 0x425C3E) count both the normal and
    // the deathbomb consumption, independently of the F3 power patches.
    if(env.practice)++env.practice->tracker_bombs;
    // F3 infinite power: bombs still fire, but the -20 power cost is skipped
    // (0x425C4A / 0x4259DB upstream) rather than rewritten every frame.
    if(!(env.practice&&practice_infinite_power(*env.practice)))
#endif
    env.economy->power=static_cast<std::int16_t>(env.economy->power-20);power_changed(player,env);
}
void set_bounds(PlayerBounds& bounds,const Vec3& point,Extended x,Extended y,Extended z){
    bounds.minimum={(number(point.x)-x).to_float(),(number(point.y)-y).to_float(),(number(point.z)-z).to_float()};
    bounds.maximum={(x+number(point.x)).to_float(),(y+number(point.y)).to_float(),(z+number(point.z)).to_float()};
}
}
// 0x425730. State changes fall through only at the two points present in the
// original: entering active play and exhausting the deathbomb window.
i32 Player::update(PlayerFrameEnvironment& env){
    auto& economy=*env.economy;
    if(state==0){
        const i32 distance=static_cast<i32>(static_cast<u32>(state_timer.current)*8000u)/60;
        fixed_position.y=wrapping_add(48000,static_cast<i32>(0u-static_cast<u32>(distance)));
        position.y=(Extended::from_int(fixed_position.y)*number(.01f)).to_float();for(auto& option:options)option.snap_next=1;
        update_position_history();position_history[0]=fixed_position;
        if(state_timer.current<30){
            const float radius=(Extended::from_int(state_timer.current)*number(17.066668f)+number(64.f)).to_float();
            env.cancel_bullet_circle(death_position,radius,true);env.cancel_bullet_circle(death_position,Scalar::mul(radius,.25f),false);
            env.cancel_laser_circle(death_position,radius);
        }else{env.clear_bullets(false);env.clear_lasers(false);}
        if(state_timer.current>=60){state=1;set_timer(state_timer,state_timer_flags,0,env.default_rate);}
    }
    if(state==1){
        if(env.gui_present&&!*env.dialogue&&env.bomb&&!env.bomb->active&&economy.power/20&&(*env.input_keys&2)){
            set_timer(invulnerability,invulnerability_flags,270,env.default_rate);consume_bomb(*this,env);
            economy.item_value=wrapping_add(economy.item_value,-300);if(economy.item_value<5000)economy.item_value=5000;
        }
        if(state_timer.current<30){env.clear_bullets(false);env.clear_lasers(false);}move(*env.movement);
    }else if(state==4){
        if(state_timer.current>=8)die(*env.lifecycle);
        else if(env.bomb&&!env.bomb->active&&economy.power/20&&(*env.input_keys&2)){
            set_timer(state_timer,state_timer_flags,60,env.default_rate);set_timer(invulnerability,invulnerability_flags,200,env.default_rate);
            consume_bomb(*this,env);state=1;
        }
    }else if(state==3&&state_timer.current==15){env.clear_bullets(true);env.clear_lasers(true);}
    if(state==2){
        if(state_timer.current==3){
#ifdef TH_ENABLE_THPRAC
            if(!(env.practice&&practice_infinite_power(*env.practice))){
#endif
            economy.power=static_cast<std::int16_t>(economy.power-64);if(economy.power<0)economy.power=0;
#ifdef TH_ENABLE_THPRAC
            }
#endif
            const i32 power=economy.power;env.update_power(power/20,(power%20)*100/20);
            const Vec3 center{0,Scalar::sub(position.y,224.f),0};const float base=angle_towards(center).to_float();
            for(i32 i=0;i<7;++i){const float angle=(Extended::from_int(i)*number(.11219974f)+number(base)-number(.3926991f)).to_float();env.drop_power(position,i&1?4:1,angle);}
            env.update_options(*this);
        }
        if(state_timer.current>=30){
            if(economy.lives<0)env.game_over(*env.replay_mode==1);
            else{
                state=0;*env.default_rate=1.f;emit_circle(position,32.f,16.f,30,150,env.default_rate);death_position=position;
                fixed_position={0,48000};position.x=0;position.y=480.f;
                set_timer(invulnerability,invulnerability_flags,280,env.default_rate);set_timer(state_timer,state_timer_flags,0,env.default_rate);
            }
        }
    }
    for(auto& area:damage_areas)if(area.flags&1){
        area.motion.update_velocity();area.motion.update();area.radius=Scalar::add(area.radial_speed,area.radius);
        area.angle=Scalar::add(area.angular_velocity,area.angle);area.timer.advance(-1.f);if(area.timer.current<=0)area.flags&=~1u;
    }
    if(invulnerability.current>0){
        invulnerability.advance(-1.f);
        if(state_timer.current!=state_timer.previous&&state_timer.current%3==0){animation.secondary_color=0xff0000ff;animation.flags|=0x8000;}
        else animation.flags&=~0x8000u;
    }else animation.flags&=~0x8000u;
    animation.update(*env.animations);
    set_bounds(collision_bounds,position,number(hitbox_half_size.x),number(hitbox_half_size.y),number(hitbox_half_size.z));
    set_bounds(pickup_bounds,position,number(fast_pickup_size.x)*number(.5f),number(fast_pickup_size.y)*number(.5f),number(Scalar::mul(fast_pickup_size.z,.5f)));
    set_bounds(slow_pickup_bounds,position,number(slow_pickup_size.x),number(slow_pickup_size.y),number(slow_pickup_size.z));
    set_bounds(fast_pickup_bounds,position,number(fast_pickup_size.x),number(fast_pickup_size.y),number(fast_pickup_size.z));
    state_timer.tick();focus_timer.tick();
    if(!*env.dialogue&&env.enemy_count&&*env.enemy_count){if(state_timer.current%60==0)economy.add_rank(1);update_firing(*env.shooting);}
    else{set_timer(fire_timer,fire_timer_flags,-1,env.default_rate);target=nullptr;target_seen=0;}
    update_shots(*env.shooting);return 1;
}
}
