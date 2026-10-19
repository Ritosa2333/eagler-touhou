#include "movement.hpp"
#include "frame_helpers.hpp"
#include <bit>
#include <cstring>
namespace th20::source::player_entity {
namespace {
int signed32(std::uint32_t value){return std::bit_cast<int>(value);}
int multiply(int a,int b){return signed32(std::uint32_t(a)*std::uint32_t(b));}
int moved_component(float offset,int speed,float scale){const auto delta=recovered::truncate32(offset*128.0f);return recovered::truncate32(float(signed32(std::uint32_t(delta)+std::uint32_t(speed)))*scale);}
// Octant of a continuous direction, in the same 1..8 order as the literal
// `signs` table below (1 up, 2 down, 3 left, 4 right, 5..8 diagonals).
std::uint32_t analog_octant(float x,float y){
    if(x==0.f&&y==0.f)return 0;
    const float ax=x<0.f?-x:x,ay=y<0.f?-y:y;
    if(ay<=ax*0.41421357f)return x>0.f?4u:3u;
    if(ax<=ay*0.41421357f)return y>0.f?2u:1u;
    if(y>0.f)return x>0.f?8u:7u;
    return x>0.f?6u:5u;
}
}
std::uint32_t set_transition_direction(Player& player,std::uint32_t mode){recovered::timer_set(player.timers_2050[2],0);player.entity_flags=(player.entity_flags&~0xc0u)|((mode&3u)<<6);return mode&3u;}
int update_movement(Player& player,MovementServices& env){
    int x=0,y=0;const int slot=env.input_slot(player.view_index);auto& options=env.options();auto& callbacks=options.callbacks();const float rate=callbacks.firing().clock_rate();
    if(const auto mode=(player.entity_flags>>6)&3u;mode){
        auto& timer=player.timers_2050[2];if(timer.current==0)finish_stage_visibility(player,env.reset());recovered::timer_tick(timer,&rate);
        if(timer.current==1)env.sound(mode==1?81:80);x=mode==1?-256:256;
        if(timer.current==8){x=mode==1?49152:-49152;mark_options_changed(player,1);for(auto& option:player.secondary_options)option.fields_f4[2]=1;}
        if(timer.current<16)goto apply_movement;
        restore_stage_visibility(player,env.reset());set_transition_direction(player,0);
    }
    {
        auto held=[&](std::uint32_t mask){return env.held(slot,mask);};
        std::uint32_t direction=0;
        if(held(0x50)==0x50)direction=5;else if(held(0x60)==0x60)direction=7;else if(held(0x90)==0x90)direction=6;else if(held(0xa0)==0xa0)direction=8;
        else if(held(0x20))direction=2;else if(held(0x10))direction=1;else if(held(0x40))direction=3;else if(held(0x80))direction=4;
        // Direct-touch extension: an active gesture supplies a continuous
        // direction that replaces the eight-way table, so the adapter can move
        // the player at any angle instead of a stiff compass step. Keyboard and
        // gamepad frames keep the recovered digital path exactly (the hook is
        // inactive), which is what the CPU comparisons exercise.
        float analog_x=0.f,analog_y=0.f;const bool analog=env.analog(slot,analog_x,analog_y);
        if(analog)direction=analog_octant(analog_x,analog_y);
        player.fields_674[2]=direction;
        if(env.enemy_ready(*player.context)&&player.timers_644[1].current>=4){
            const std::uint8_t focus=held(8)!=0;if(player.focused_204c!=focus)std::memcpy(&player.vectors_628[1],&player.fixed_position,8);player.focused_204c=held(8)!=0;
        }else{player.focused_204c=0;player.fields_20e4[0]=30;}
        // Original immutable table572c48. Components are fixed-point signs.
        constexpr int signs[9][2]{{0,0},{0,-1},{0,1},{-1,0},{1,0},{-1,-1},{1,-1},{-1,1},{1,1}};x=signs[direction][0];y=signs[direction][1];
        if(player.focused_204c){
            if(!player.handle_60c)player.handle_60c=env.spawn_focus_effect(player);
            if(auto* animation=env.animation(player.handle_60c)){const float scale=(player.entity_flags&16u)?(player.collision_expansion-1.0f)*2.0f+1.0f:1.0f;animation->base.vector_58={scale,scale};animation->base.flags[1]|=4;}
        }else if(env.animation(player.handle_60c)){callbacks.interrupt(player.handle_60c,1);player.handle_60c=0;}
        if(analog){x=recovered::truncate32(analog_x);y=recovered::truncate32(analog_y);}
        else{const auto speed=player.speeds_20b4[(direction>=5?2:0)+(player.focused_204c?1:0)];x=multiply(x,speed);y=multiply(y,speed);}
        const float scale=std::bit_cast<float>(player.fields_20e4[2]);x=moved_component(player.vector_20f0.x,x,scale);y=moved_component(player.vector_20f0.y,y,scale);
    }
apply_movement:
    const auto previous=signed32(player.fields_674[0]);
    if(x<0&&previous>=0)env.bind_script(player,signed32(player.animation_scripts[1]));else if(x==0&&previous<0)env.bind_script(player,signed32(player.animation_scripts[2]));
    if(x>0&&previous<=0)env.bind_script(player,signed32(player.animation_scripts[3]));else if(x==0&&previous>0)env.bind_script(player,signed32(player.animation_scripts[4]));
    player.fields_674[0]=std::uint32_t(x);player.fields_674[1]=std::uint32_t(y);advance_fixed_motion(player,x,y,env.clock_scale());
    if(auto* animation=env.animation(player.handle_60c))animation->vector_5bc=player.position_614;
    if(x||y){std::memmove(player.vectors_20fc+1,player.vectors_20fc,32*8);std::memcpy(player.vectors_20fc,&player.fixed_position,8);}
    if(player.entity_flags&2u)++player.fields_20e4[1];
    for(auto& option:player.options)update_option(player,option,options);for(auto& option:player.secondary_options)update_option(player,option,options);
    if(signed32(player.fields_20e4[1])>29)player.fields_674[3]=0;
    return 0;
}
}
