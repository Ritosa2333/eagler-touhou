#include "type2.hpp"
#include "../runtime_state/state.hpp"
#include "../ecl_vm/math.hpp"
#include "../sprite_renderer/anm_vm.hpp"
#include "../sprite_renderer/draw.hpp"
#include "../program_entry/program_entry.hpp"
#include "../audio_runtime/audio.hpp"
#include "../player_entity/player.hpp"
#include <bit>
#include <cmath>
namespace th20::source::laser {
namespace n=recovered;namespace m=ecl::math;
namespace {
float sub(float a,float b){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float div(float a,float b){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b)));}
void add(sprite::Vec3& p,const sprite::Vec3& v){p.x=n::add32(p.x,v.x);p.y=n::add32(p.y,v.y);p.z=n::add32(p.z,v.z);}
}
int Type2Laser::accelerate(){
    auto& c=commands[1];if(c.timer.current>=n::signed_bits(c.field_30)){active_commands&=~std::uint64_t{4};return 1;}
    field_7c=n::add32(field_7c,n::mul32(state::clock_scale,c.field_10));
    velocity.x=n::add32(velocity.x,n::mul32(c.vector_24.x,state::clock_scale));velocity.y=n::add32(velocity.y,n::mul32(c.vector_24.y,state::clock_scale));velocity.z=n::add32(velocity.z,n::mul32(c.vector_24.z,state::clock_scale));
    if(std::fabs(velocity.x)>.0001f||std::fabs(velocity.y)>.0001f)angle=m::arctangent(velocity.y,velocity.x);n::timer_tick(c.timer,state::timer_rate);return 0;
}
int Type2Laser::angular_accelerate(){
    auto& c=commands[2];if(c.timer.current>=n::signed_bits(c.field_30)){active_commands&=~std::uint64_t{8};return 1;}
    angle=m::wrap_angle(n::add32(angle,n::mul32(state::clock_scale,c.field_14)));field_7c=n::add32(field_7c,n::mul32(state::clock_scale,c.field_10));m::polar(velocity.x,velocity.y,angle,field_7c);n::timer_tick(c.timer,state::timer_rate);return 0;
}
int Type2Laser::turn_in_steps(){
    auto& c=commands[3];float current_speed;
    if(c.timer.current>=n::signed_bits(c.field_30)){
        if(parameters.motion_sound>=0)program_entry::thread_registry.request_effect(parameters.motion_sound,0);++c.field_38;angle=n::add32(angle,c.field_14);field_7c=c.field_10;current_speed=field_7c;n::timer_set(c.timer,0);
        if(n::signed_bits(c.field_38)>=n::signed_bits(c.field_34)){m::polar(velocity.x,velocity.y,angle,current_speed);active_commands&=~std::uint64_t{16};return 1;}
    }else current_speed=sub(field_7c,div(n::mul32(c.timer.current_f,field_7c),n::int_float(n::signed_bits(c.field_30))));
    m::polar(velocity.x,velocity.y,angle,current_speed);n::timer_tick(c.timer,state::timer_rate);return 0;
}
int Type2Laser::steer(){
    auto& c=commands[7];if(c.timer.current>=n::signed_bits(c.field_30)){active_commands&=~std::uint64_t{0x80000000};return 1;}
    position=samples[0].position;const float target_angle=m::wrap_angle(n::add32(c.field_14,player_entity::angle_to_player(context->objects_04[0],position)));sprite::Vec3 v{};m::polar(v.x,v.y,target_angle,c.field_10);
    velocity.x=n::add32(velocity.x,n::mul32(sub(v.x,velocity.x),c.vector_24.x));velocity.y=n::add32(velocity.y,n::mul32(sub(v.y,velocity.y),c.vector_24.x));velocity.z=n::add32(velocity.z,n::mul32(sub(v.z,velocity.z),c.vector_24.x));velocity.z=0;
    field_7c=m::square_root(n::add32(n::mul32(velocity.x,velocity.x),n::mul32(velocity.y,velocity.y)));angle=m::arctangent(velocity.y,velocity.x);n::timer_tick(c.timer,state::timer_rate);return 0;
}
int Type2Laser::wait_command(){auto& t=commands[11].timer;n::timer_add(t,-1,state::timer_rate);if(t.current<=0){active_commands&=~std::uint64_t{0x100};return 1;}return 0;}
int Type2Laser::advance(){
    if(flags&0x20)return 0;flags=(flags|0x20)&~0x40u;
    for(;;){execute_commands();if(!active_commands)break;std::uint32_t completed=0;
        // Original slots50/64/68/6c/70 are literal412540 zero bodies.
        if(active_commands&4)completed+=accelerate();if(active_commands&8)completed+=angular_accelerate();if((active_commands&16)&&commands[3].field_3c==0)completed+=turn_in_steps();if(active_commands&0x80000000)completed+=steer();if(active_commands&0x100)completed+=wait_command();
        if(active_commands&0x40000000){auto& t=commands[5].timer;if(t.current<=0){active_commands&=~std::uint64_t{0x40000000};++completed;}else n::timer_add(t,-1,state::timer_rate);}if(field_6cc)--field_6cc;if(!completed)break;
    }
    if(field_1324){
        for(std::int32_t i=n::signed_bits(parameters.count-1);i>0;--i)samples[i]=samples[i-1];samples[0].speed=field_7c;samples[0].velocity=velocity;add(samples[0].position,velocity);samples[0].angle=angle;
    }else if(!(flags&16)){
        bool backwards=false;for(std::int32_t i=0;i<n::signed_bits(parameters.count);++i){auto& s=samples[i];const float t=sub(timer_48.current_f,n::int_float(i));
            if(t<0)s={parameters.position,{},parameters.angle,parameters.speed};else{const auto& previous=i?samples[i-1]:s;sample_curve_path(&path,s.position,s.speed,s.angle,previous.position,previous.speed,previous.angle,t,backwards);backwards=true;}}
    }
    if(timer_6ac.current<=0&&!(active_commands&0x100)){for(std::int32_t i=0;i<n::signed_bits(parameters.count);++i)if(!bullet::outside_viewport(samples[i].position,speed,speed))return 0;return 1;}
    n::timer_add(timer_6ac,-1,state::timer_rate);return 0;
}
int Type2Laser::update(){
    if(advance())return -1;cancel_all(0);sprite::execute_animation(animation);sprite::execute_animation(origin_animation);n::timer_add(timer_48,n::mul32(std::bit_cast<float>(field_84),std::bit_cast<float>(field_88)),state::timer_rate);flags&=~0x20u;return 0;
}
int Type2Laser::draw(){
    auto* out=static_cast<sprite::Vertex28*>(geometry);float u=0;const float half_pi=div(3.1415927410125732f,2);
    for(std::int32_t i=0;i<n::signed_bits(parameters.count);++i){const auto& s=samples[i];
        for(int side=0;side<2;++side){
            out->rhw=1;out->color=0xffffffff;out->u=u;out->v=animation.base.vectors_378[side*2].y;
            float a=m::wrap_angle(side?sub(s.angle,half_pi):n::add32(half_pi,s.angle));
            if(i){const float previous=m::wrap_angle(side?sub(samples[i-1].angle,half_pi):n::add32(half_pi,samples[i-1].angle));const float difference=m::wrap_angle(m::angle_difference(previous,a));const float divided=m::wrap_angle(div(difference,2));a=m::wrap_angle(n::add32(a,divided));}
            m::polar(out->x,out->y,a,n::mul32(parameters.width,.5f));out->x=n::add32(n::add32(out->x,s.position.x),n::int_float(program_entry::window_state.field_0038[view_index]));out->y=n::add32(n::add32(out->y,s.position.y),n::int_float(program_entry::window_state.field_0040[view_index]));out->z=0;++out;
        }
        u=n::add32(div(1,n::int_float(n::signed_bits(parameters.count-1))),u);
    }
    animation.base.flags[1]|=0x200000;sprite::primitive::p445350(*program_entry::sprite_controller,animation,static_cast<float*>(geometry),parameters.count*2);
    if(timer_48.current<=n::signed_bits(parameters.count)){origin_animation.base.vector_2c=samples[parameters.count-1].position;sprite::draw_animation(*program_entry::sprite_controller,origin_animation);}return 0;
}
}
