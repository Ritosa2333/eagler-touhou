#include "frame.hpp"
#include "../ecl_vm/math.hpp"
#include <algorithm>
#include <bit>
#include <cmath>
#include <cstring>
namespace th20::source::player_entity {
namespace {
int& record_field(game_session::Player& player,unsigned offset){return *reinterpret_cast<int*>(reinterpret_cast<std::uint8_t*>(&player)+offset);}
int clamp_record(game_session::Player& player,unsigned offset,int lo,int hi){auto& value=record_field(player,offset);return value=std::clamp(value,lo,hi);}
float bearing_from_player(const Player& player,const sprite::Vec3& target){const float x=target.x-player.position_614.x,y=target.y-player.position_614.y;return x==0&&y==0?0x1.921fb6p+0f:ecl::math::arctangent(y,x);}
void animation_scale(sprite::Animation& animation,float scale){animation.base.vector_50={scale,scale};animation.base.flags[1]|=4;}
}
int update_player(Player& player,FrameServices& env){
    constexpr float pi=0x1.921fb6p+1f;auto& movement=env.movement();auto& callbacks=movement.options().callbacks();auto& session=callbacks.firing().session();auto& global=*session.contexts[0].current_player;
    auto rate=[&]{return callbacks.firing().clock_rate();};auto tick=[&](recovered::Timer& timer){const float r=rate();recovered::timer_tick(timer,&r);};auto decrement=[&](recovered::Timer& timer){const float r=rate();recovered::timer_add(timer,-1,&r);};
    auto& timer=player.timers_644[0];env.select_view(player.view_index);const int slot=movement.input_slot(player.view_index);
    switch(player.state){
    case 0:{
        if(timer.current<=1)movement.bind_script(player,std::bit_cast<int>(player.animation_scripts[0]));
        const int movement_y=recovered::signed_bits(std::uint32_t(timer.current)*std::uint32_t(-10240))/60;
        player.fixed_position.y=recovered::signed_bits(std::uint32_t(movement_y)+61440u);player.position_614.y=float(player.fixed_position.y)/128.f;mark_options_changed(player,1);
        for(auto& point:player.vectors_20fc)std::memcpy(&point,&player.fixed_position,8);
        if(timer.current>=30){env.cancel_bullets(*player.context,player.position_614,640);env.cancel_lasers(*player.context,player.position_614,640,0);env.create_damage(*player.context,player.position_614,120,0,1,10,true);}
        else{const float radius=(float(timer.current)*512.f)/30.f+64.f;env.cancel_lasers(*player.context,player.vector_638,radius,1);env.cancel_lasers(*player.context,player.vector_638,radius/4.f,0);}
        if(timer.current<60)break;restore_stage_visibility(player,movement.reset());player.state=1;recovered::timer_set(timer,0);
    }[[fallthrough]];
    case 1:
        if(timer.current<=1)movement.bind_script(player,std::bit_cast<int>(player.animation_scripts[0]));
        if(!env.shots().input_blocked()&&env.bomb_exists(*player.context)&&env.can_bomb(*player.context)&&env.pressed(slot,4))env.trigger_bomb(*player.context);
        update_movement(player,movement);break;
    case 3:
        if(timer.current!=4&&timer.current==15)env.finish_lasers(*player.context,1,0);break;
    case 4:
        if(timer.current<std::bit_cast<int>(player.field_2204)){
            if(env.bomb_exists(*player.context)&&env.pressed(slot,4)&&env.can_bomb(*player.context)){env.trigger_bomb(*player.context);cancel_death(player);}break;
        }
        begin_death(player,env.death());[[fallthrough]];
    case 2:
        if(timer.current==3){
            int percent=40;if(power_level(global)>=4)percent=80;else if(power_level(global)>=3)percent=60;else if(power_level(global)>=2)percent=50;
            subtract_power(global,(clamped_power_unit(global)*percent)/100);
            const sprite::Vec3 reference{0,player.position_614.y-224.f,0};const float angle=bearing_from_player(player,reference);
            for(int i=0;i<7;++i){const float direction=((float(i)*pi)/28.f+angle)-(pi*3.5f)/28.f;env.spawn_power_item(player.position_614,direction);}
            refresh_power(player,-1,movement.options().power());
        }
        if(timer.current>=30){
            if(player.entity_flags&0x400u)env.cancel_near_bullets(*player.context,player.position_614,80);
            if(clamp_record(global,0xb8,-1,7)<0&&timer.current==30){if(!env.replay_playing())env.finish_game();tick(timer);break;}
            player.state=0;env.clock_scale(1);env.create_damage(session.contexts[0],player.position_614,32,16,30,150,false);
            const int count=clamp_record(global,0xcc,0,10);const int restored=count<2?clamp_record(global,0xd4,2,10):clamp_record(global,0xcc,0,10);env.set_bombs(global,restored);
            player.vector_638=player.position_614;set_position(player,0,480);recovered::timer_set(player.timers_2050[0],280);recovered::timer_set(timer,0);
        }break;
    case 5:{
        sprite::Vec3 direction{};ecl::math::polar(direction.x,direction.y,std::bit_cast<float>(player.handle_18),1);
        const float length=ecl::math::square_root((direction.x*direction.x+direction.y*direction.y)+direction.z*direction.z);const float magnitude=2.f-(float(timer.current)*2.f)/40.f;
        if(std::fabs(length)>=.01f){direction.x=direction.x/length;direction.y=direction.y/length;direction.z=direction.z/length;}direction.x*=magnitude;direction.y*=magnitude;direction.z*=magnitude;
        advance_fixed_motion(player,recovered::truncate32(direction.x*128.f),recovered::truncate32(direction.y*128.f),movement.clock_scale());
        if(timer.current>=40){player.state=1;recovered::timer_set(timer,0);}break;
    }
    case 6:recovered::timer_set(timer,0);player.position_614={0,480,0};player.fixed_position={0,480};break;
    default:break;
    }
    if(player.timers_2050[1].current>0)decrement(player.timers_2050[1]);
    auto& animation=player.animation;auto flash=[&](std::uint32_t color){animation.base.field_494=color;animation.base.flags[2]=(animation.base.flags[2]&~0x1c00u)|0x400u;};
    if(player.timers_2050[0].current>0){decrement(player.timers_2050[0]);if(timer.current!=timer.previous&&timer.current%3==0)flash(0xff0000ffu);else animation.base.flags[2]&=~0x1c00u;}
    else{animation.base.flags[2]&=~0x1c00u;if(player.entity_flags&32u){if(timer.current%8<4)flash(0xffff0000u);}else if(std::bit_cast<float>(player.fields_20e4[2])>1.01f&&timer.current%8<4)flash(0xffffff00u);}
    player.fields_20e4[2]=0x3f800000u;player.vector_20f0={0,0,0};env.execute_animation(animation);
    if(!(player.entity_flags&16u))animation_scale(animation,1);
    else{if(player.interpolation_220c.duration)player.collision_expansion=env.sample_expansion(player.interpolation_220c);const float scale=player.interpolation_220c.duration&&timer.current%3==0?1.f:player.collision_expansion;animation_scale(animation,scale);}
    tick(timer);tick(player.timers_644[1]);update_shot_controller(player.shots,env.shots());update_feedback(player.feedback,callbacks);return 1; //4f83e8; retains the scheduler node
}
}
