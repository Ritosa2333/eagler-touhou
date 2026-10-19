#include "type0.hpp"
#include "type1.hpp"
#include "../runtime_state/state.hpp"
#include "../ecl_vm/math.hpp"
#include "../sprite_renderer/binding.hpp"
#include "../sprite_renderer/anm_vm.hpp"
#include "../sprite_renderer/draw.hpp"
#include "../program_entry/program_entry.hpp"
#include "../audio_runtime/audio.hpp"
#include <bit>
#include <cmath>
namespace th20::source::laser {
namespace n=recovered;namespace m=ecl::math;
namespace {
float sub(float a,float b){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float div(float a,float b){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b)));}
int accelerate_command(Type0Laser& l,int index,std::uint64_t mask){
    auto& c=l.commands[index];if(c.timer.current>=n::signed_bits(c.field_30)){l.active_commands&=~mask;return 1;}
    l.field_7c=n::add32(l.field_7c,n::mul32(state::clock_scale,c.field_10));
    l.velocity.x=n::add32(l.velocity.x,n::mul32(c.vector_24.x,state::clock_scale));l.velocity.y=n::add32(l.velocity.y,n::mul32(c.vector_24.y,state::clock_scale));l.velocity.z=n::add32(l.velocity.z,n::mul32(c.vector_24.z,state::clock_scale));
    if(std::fabs(l.velocity.x)>0.0001f||std::fabs(l.velocity.y)>0.0001f)l.angle=m::arctangent(l.velocity.y,l.velocity.x);
    n::timer_tick(c.timer,state::timer_rate);return 0;
}
}
int Type0Laser::accelerate(){return accelerate_command(*this,1,4);}
int Type0Laser::approach_speed(){return accelerate_command(*this,10,0x200000);}
int Type0Laser::turn_in_steps(){
    auto& c=commands[3];float current_speed;
    if(c.timer.current>=n::signed_bits(c.field_30)){
        if(parameters.field_4c>=0)program_entry::thread_registry.request_effect(parameters.field_4c,0);++c.field_38;angle=n::add32(angle,c.field_14);field_7c=c.field_10;current_speed=field_7c;n::timer_set(c.timer,0);
        if(n::signed_bits(c.field_38)>=n::signed_bits(c.field_34)){m::polar(velocity.x,velocity.y,angle,current_speed);active_commands&=~std::uint64_t{16};return 1;}
    }else current_speed=sub(field_7c,div(n::mul32(c.timer.current_f,field_7c),n::int_float(n::signed_bits(c.field_30))));
    m::polar(velocity.x,velocity.y,angle,current_speed);n::timer_tick(c.timer,state::timer_rate);return 0;
}
int Type0Laser::advance(){
    if(flags&0x20)return 0;flags=(flags|0x20)&~0x40u;
    for(;;){
        execute_commands();if(!active_commands)break;std::uint32_t completed=0;
        //Original slots50,5c,64,68,70 are all literal412540 zero-return.
        if(active_commands&4)completed+=accelerate();if(active_commands&0x200000)completed+=approach_speed();
        if((active_commands&16)&&commands[3].field_3c==0)completed+=turn_in_steps();if(active_commands&64)completed+=bounce();
        if(active_commands&0x40000000){auto& t=commands[5].timer;if(t.current<=0){active_commands&=~std::uint64_t{0x40000000};++completed;}else n::timer_add(t,-1,state::timer_rate);}
        if(field_6cc)--field_6cc;if(!completed)break;
    }
    const float scale1=std::bit_cast<float>(field_84),scale2=std::bit_cast<float>(field_88);
    const float step=n::mul32(n::mul32(n::mul32(state::clock_scale,field_7c),scale1),scale2);
    if(parameters.length>field_74){field_74=n::add32(field_74,step);if(field_74>parameters.length)field_74=parameters.length;}
    else{
        field_80=n::add32(field_80,step);
        position.x=n::add32(position.x,n::mul32(n::mul32(n::mul32(velocity.x,state::clock_scale),scale1),scale2));
        position.y=n::add32(position.y,n::mul32(n::mul32(n::mul32(velocity.y,state::clock_scale),scale1),scale2));
        position.z=n::add32(position.z,n::mul32(n::mul32(n::mul32(velocity.z,state::clock_scale),scale1),scale2));
        if(parameters.length_limit>0&&n::add32(field_80,field_74)>parameters.length_limit){field_74=sub(parameters.length_limit,field_80);parameters.length=field_74;if(0>=field_74)return 1;}
    }
    if(timer_6ac.current<=0&&timer_6bc.current<=0){sprite::Vec3 end{};m::polar(end.x,end.y,angle,field_74);end.x=n::add32(end.x,position.x);end.y=n::add32(end.y,position.y);end.z=n::add32(end.z,position.z);return bullet::outside_viewport(position,speed,speed)&&bullet::outside_viewport(end,speed,speed)?1:0;}
    if(timer_6ac.current>0)n::timer_add(timer_6ac,-1,state::timer_rate);if(timer_6bc.current>0)n::timer_add(timer_6bc,-1,state::timer_rate);return 0;
}
int Type0Laser::collision_segment(Segment* output){
    auto& s=*static_cast<Segment*>(allocated_6d8);flags|=0x40;s.flags&=~8u;if((state!=4&&state!=2)||!(field_74>16)||!(speed>3))return 0;
    const float width=speed>=32?sub(speed,div(n::add32(speed,16),2)):n::mul32(speed,0.5f);
    const float length=(parameters.flags&2)?field_74:div(n::mul32(field_74,4),5);
    sprite::Vec3 p{};m::polar(p.x,p.y,angle,div(field_74,2));p.x=n::add32(p.x,position.x);p.y=n::add32(p.y,position.y);p.z=n::add32(p.z,position.z);
    s.position=p;s.angle=s.angle_28=angle;s.flags=(s.flags&~3u)|12u;s.field_2c=field_7c;s.size={n::mul32(length,1),n::mul32(width,1)};if(output)*output=s;return 1;
}
int Type0Laser::update(){
    if(advance())return 1;cancel_all(0);auto& sprites=*program_entry::sprite_controller;
    animation.base.vector_50.x=div(speed,sprite::current_sprite(sprites,animation).extent_4c);animation.base.flags[1]|=4;
    animation.base.vector_50.y=div(field_74,sprite::current_sprite(sprites,animation).extent_48);animation.base.flags[1]|=4;
    sprite::execute_animation(animation);if(field_80==0)sprite::execute_animation(origin_animation);sprite::execute_animation(tip_animation);n::timer_tick(timer_48,state::timer_rate);flags&=~0x20u;return 0;
}
int Type0Laser::draw(){
    animation.base.vector_2c=position;animation.base.vector_38.z=m::wrap_angle(n::add32(angle,div(3.1415927410125732f,2)));animation.base.flags[1]|=2;
    sprite::draw_animation(*program_entry::sprite_controller,animation);tip_position(tip_animation.base.vector_2c);sprite::draw_animation(*program_entry::sprite_controller,tip_animation);
    if(field_80==0){origin_animation.base.vector_2c=position;sprite::draw_animation(*program_entry::sprite_controller,origin_animation);}return 0;
}
}
