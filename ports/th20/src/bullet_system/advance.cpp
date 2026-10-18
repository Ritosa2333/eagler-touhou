#include "movement.hpp"
#include "../runtime_state/state.hpp"
#include "../sprite_renderer/anm_vm.hpp"
#include "../sprite_renderer/binding.hpp"
#include "../program_entry/program_entry.hpp"
namespace th20::source::bullet::unrecovered {
namespace n=recovered;
int advance_bullet_00485b60(Bullet& b){
    auto div=[](float x,float y){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(x),_mm_set_ss(y)));};
    auto add_velocity=[&](float scale,float divisor){b.position.x=n::add32(b.position.x,div(n::mul32(b.velocity.x,scale),divisor));b.position.y=n::add32(b.position.y,div(n::mul32(b.velocity.y,scale),divisor));b.position.z=n::add32(b.position.z,div(n::mul32(b.velocity.z,scale),divisor));};
    auto reset_rate=[&](){assign_timer_float(b.timer_4d8,1);assign_timer_float(b.timer_4e8,1);state::set_clock_scale(1);};
    auto clear=[&](std::uint32_t flag){b.field_90&=~static_cast<std::uint64_t>(flag);};
    b.flags|=0x400u;n::timer_tick(b.timer_508,state::timer_rate);
    if(b.flags&8u){retire(b);return -1;}
    const float previous_rate=state::clock_scale;
    if(b.state!=2&&b.state!=3&&b.state!=4&&!(b.flags&0x200u))state::set_clock_scale(n::mul32(b.timer_4d8.current_f,b.timer_4e8.current_f));
    if(b.field_90&0x400000u){sprite::sample_animation_interpolation(&b.interpolation_474,1,false,false,&b.scale,state::timer_rate);if(b.interpolation_474.duration==0){clear(0x400000u);if(b.scale==1)b.flags&=~64u;}}
    switch(b.state){
    case 2:add_velocity(1,2);reset_rate();if(b.animation->base.fields_444[0]==0)break;b.state=1;[[fallthrough]];
    case 1:{
    retry_commands:
        unsigned finished;
        do{
            while(!(b.field_90&0x4000000u)){
                execute_extended_commands_0047dcf0(b);if(b.state!=2)break;reset_rate();if(b.animation->base.fields_444[0]==0)goto after_movement;b.state=1;
            }
            finished=0;
            if(b.field_90&1u)finished+=update_launch_acceleration(b);
            if(b.field_90&4u)finished+=update_linear_acceleration(b);
            if(b.field_90&0x200000u)finished+=update_linear_acceleration(b,true);
            if(b.field_90&8u)finished+=update_angular_acceleration(b);
            if(b.field_90&16u)finished+=update_repeated_turn(b);
            if(b.field_90&64u)finished+=update_bounce_00482a60(b);
            if(b.field_90&0x80000000u)finished+=update_homing(b);
            if(b.field_90&0x20000u)finished+=update_position_interpolation(b);
            if(b.field_90&0x80000u)finished+=update_position_offset(b);
            if(b.field_90&0x100u)finished+=update_offscreen_delay_00481200(b);
            if(b.field_90&0x40000000u){auto& timer=b.commands[5].timer;if(timer.current<=0){clear(0x40000000u);++finished;}else n::timer_add(timer,-1,state::timer_rate);}
            if(b.field_90&0x4000000u){auto& timer=b.commands[13].timer;if(timer.current<=0){clear(0x4000000u);b.flags&=~0x200u;++finished;goto retry_commands;}b.flags|=0x200u;n::timer_add(timer,-1,state::timer_rate);}
        }while(finished);
        if(!(b.flags&0x200u))add_velocity(state::clock_scale,1);break;
    }
    case 3:reset_rate();add_velocity(state::clock_scale,2);break;
    case 4:reset_rate();break;
    default:break;
    }
after_movement:
    auto& sprites=*program_entry::sprite_controller;
    auto& file=sprite::sprite_file(sprites,*b.animation);
    const auto address=reinterpret_cast<std::uintptr_t>(file.sprites)+sizeof(sprite::SpriteData)*b.animation->base.fields_10_28[4];
    if(address){
        if(b.field_90&0x1000u)update_screen_wrap(b);
        if(!(b.field_90&0x100u)&&n::signed_bits(b.field_30)<1){
            auto& s=sprite::current_sprite(sprites,*b.animation);const auto w=div(n::mul32(s.extent_4c,b.scale),2),h=div(n::mul32(s.extent_48,b.scale),2);
            if(n::add32(w,b.position.x)<=-192||b.position.x-w>=192||n::add32(h,b.position.y)<=-64||b.position.y-h>=448){retire(b);state::set_clock_scale(previous_rate);return -1;}
        }
    }
    if(b.field_18)b.field_18=n::signed_bits(static_cast<std::uint32_t>(b.field_18)-1u);
    if(n::signed_bits(b.field_30)>0)--b.field_30;
    if(!(b.flags&0x200u)&&sprite::execute_animation(*b.animation)){retire(b);state::set_clock_scale(previous_rate);return -1;}
    state::set_clock_scale(previous_rate);return 0;
}
}
