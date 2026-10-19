#include "movement.hpp"
#include "../runtime_state/state.hpp"
#include "../ecl_vm/math.hpp"
#include "../sprite_renderer/anm_vm.hpp"
#include "../sprite_renderer/binding.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../sprite_renderer/render_mesh.hpp"
#include "../program_entry/program_entry.hpp"
#include "../audio_runtime/audio.hpp"
#include "../player_entity/player.hpp"
#include <cmath>
#include <cstring>
namespace th20::source::bullet {
namespace n=recovered;namespace m=ecl::math;namespace pe=program_entry;
namespace {
float sub(float a,float b){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float div(float a,float b){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b)));}
void add_scaled(sprite::Vec3& a,const sprite::Vec3& b,float scale){a.x=n::add32(a.x,n::mul32(b.x,scale));a.y=n::add32(a.y,n::mul32(b.y,scale));a.z=n::add32(a.z,n::mul32(b.z,scale));}
float length(const sprite::Vec3& v){return m::square_root(n::add32(n::mul32(v.x,v.x),n::mul32(v.y,v.y)));}
bool moving_xy(const sprite::Vec3& v){return std::fabs(v.x)>0.00009999999747378752f||std::fabs(v.y)>0.00009999999747378752f;}
void clear_command(Bullet& b,std::uint32_t mask){b.field_90&=~static_cast<std::uint64_t>(mask);}
void sound(Bullet& b){if(n::signed_bits(b.field_40)>=0)pe::thread_registry.request_effect(n::signed_bits(b.field_40),0);}
}
float angle_to_player(const Bullet& b){
    return player_entity::angle_to_player(b.context->objects_04[0],b.position);
}
int update_launch_acceleration(Bullet& b){auto& c=b.commands[0];if(c.timer.current>16){clear_command(b,1);return 1;}
    m::polar(b.velocity.x,b.velocity.y,b.angle,n::add32(sub(5,div(n::mul32(c.timer.current_f,5),16)),b.field_20));n::timer_tick(c.timer,state::timer_rate);return 0;
}
int update_linear_acceleration(Bullet& b,bool secondary){auto& c=b.commands[secondary?10:1];if(c.timer.current>=n::signed_bits(c.field_30)){clear_command(b,secondary?0x200000u:4u);return 1;}
    b.field_20=n::add32(b.field_20,n::mul32(state::clock_scale,c.field_10));add_scaled(b.velocity,c.vector_24,state::clock_scale);
    if(moving_xy(b.velocity)){b.angle=m::wrap_angle(m::arctangent(b.velocity.y,b.velocity.x));b.field_20=length(b.velocity);}n::timer_tick(c.timer,state::timer_rate);return 0;
}
int update_angular_acceleration(Bullet& b){auto& c=b.commands[2];if(c.timer.current>=n::signed_bits(c.field_30)){clear_command(b,8);return 1;}
    b.angle=m::wrap_angle(m::wrap_angle(n::add32(b.angle,n::mul32(state::clock_scale,c.field_14))));b.field_20=n::add32(b.field_20,n::mul32(state::clock_scale,c.field_10));m::polar(b.velocity.x,b.velocity.y,b.angle,b.field_20);n::timer_tick(c.timer,state::timer_rate);return 0;
}
int update_repeated_turn(Bullet& b){auto& c=b.commands[3];float speed;
    if(c.timer.current>=n::signed_bits(c.field_30)){
        sound(b);++c.field_38;
        switch(c.field_3c){case 0:case 5:b.angle=m::wrap_angle(n::add32(b.angle,c.field_14));break;case 1:case 6:b.angle=m::wrap_angle(n::add32(angle_to_player(b),c.field_14));break;case 2:case 3:case 4:b.angle=m::wrap_angle(c.field_14);break;default:break;}
        b.field_20=c.field_10;speed=b.field_20;n::timer_set(c.timer,0);
        if(n::signed_bits(c.field_38)>=n::signed_bits(c.field_34)){m::polar(b.velocity.x,b.velocity.y,b.angle,speed);clear_command(b,16);return 1;}
    }else speed=sub(b.field_20,div(n::mul32(c.timer.current_f,b.field_20),n::int_float(n::signed_bits(c.field_30))));
    m::polar(b.velocity.x,b.velocity.y,b.angle,speed);n::timer_tick(c.timer,state::timer_rate);return 0;
}
int update_homing(Bullet& b){auto& c=b.commands[7];if(c.timer.current>=n::signed_bits(c.field_30)){clear_command(b,0x80000000u);return 1;}
    sprite::Vec3 desired{};m::polar(desired.x,desired.y,m::wrap_angle(n::add32(c.field_14,angle_to_player(b))),c.field_10);
    const sprite::Vec3 difference{sub(desired.x,b.velocity.x),sub(desired.y,b.velocity.y),sub(desired.z,b.velocity.z)};add_scaled(b.velocity,difference,c.vector_24.x);b.velocity.z=0;
    b.field_20=length(b.velocity);b.angle=m::wrap_angle(m::arctangent(b.velocity.y,b.velocity.x));n::timer_tick(c.timer,state::timer_rate);return 0;
}
int update_position_interpolation(Bullet& b){auto& c=b.commands[8];if(c.timer.current>=n::signed_bits(c.field_30)){clear_command(b,0x20000);b.position=c.vector_24;b.field_20=c.field_10;m::polar(b.velocity.x,b.velocity.y,b.angle,b.field_20);b.velocity.z=0;return 1;}
    if(c.timer.current==0)b.interpolation_420.start=b.position;
    sprite::Vec3 target;sprite::sample_animation_interpolation(&b.interpolation_420,3,false,false,&target,state::timer_rate);b.velocity={sub(target.x,b.position.x),sub(target.y,b.position.y),sub(target.z,b.position.z)};
    if(moving_xy(b.velocity))b.angle=m::wrap_angle(m::arctangent(b.velocity.y,b.velocity.x));b.velocity.z=0;n::timer_tick(c.timer,state::timer_rate);return 0;
}
int update_position_offset(Bullet& b){auto& c=b.commands[9];if(c.timer.current>=n::signed_bits(c.field_30)){clear_command(b,0x80000);return 1;}
    add_scaled(b.position,c.vector_24,state::clock_scale);n::timer_set(c.timer,0);return 0;
}
int update_screen_wrap(Bullet& b){auto& c=b.commands[6];auto& s=sprite::current_sprite(*pe::sprite_controller,*b.animation);
    if(!outside_viewport(b.position,div(s.extent_4c,2),div(s.extent_48,2)))return 0;
    if((c.field_38&1u)&&b.position.y<0)b.position.y=n::add32(n::add32(b.position.y,448),s.extent_48);
    else if((c.field_38&2u)&&b.position.y>448)b.position.y=sub(b.position.y,n::add32(448,s.extent_48));
    else if((c.field_38&4u)&&b.position.x<-192)b.position.x=n::add32(n::add32(b.position.x,384),s.extent_4c);
    else if((c.field_38&8u)&&b.position.x>192)b.position.x=sub(b.position.x,n::add32(384,s.extent_4c));
    else return 0;
    ++c.field_30;sound(b);if(n::signed_bits(c.field_30)>=n::signed_bits(c.field_34)){clear_command(b,0x1000);return 1;}return 0;
}
void retire(Bullet& b){if(!b.state)return;
    scheduler::unlink(b.link);auto& owner=*static_cast<Controller*>(b.context->primary_owner);scheduler::insert_after(owner.free.sentinel,b.link);b.link.owner=&owner.free;if(owner.free.tail==&owner.free.sentinel)owner.free.tail=&b.link;b.state=0;
    for(auto* timer:{&b.timer_4f8,&b.timer_508,&b.timer_4a0,&b.timer_4b0,&b.timer_4c0})n::timer_set(*timer,0);
    b.field_54=b.field_2c=0;b.flags&=~(1u|0x100u|0x200u|64u);
    auto& sprites=*pe::sprite_controller;sprite::request_animation_deletion(sprites,b.animation_handle);sprite::retire_animation(sprites,*b.animation);b.metadata.reset();
}
}
