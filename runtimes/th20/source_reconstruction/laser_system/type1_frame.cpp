#include "type1.hpp"
#include "../runtime_state/state.hpp"
#include "../gameplay/enemy_variables.hpp"
#include "../ecl_vm/math.hpp"
#include "../sprite_renderer/binding.hpp"
#include "../sprite_renderer/anm_vm.hpp"
#include "../sprite_renderer/draw.hpp"
#include "../program_entry/program_entry.hpp"
namespace th20::source::laser {
namespace n=recovered;namespace m=ecl::math;
namespace {
float sub(float a,float b){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float div(float a,float b){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b)));}
}
void Type1Laser::execute_commands(){
    while(command_index<parameters.commands.size()&&n::signed_bits(command_index)<=23){
        const auto& op=parameters.commands[command_index];if(!op.words[8]||(!op.words[9]&&active_commands))return;
        switch(op.words[8]){
        case 7:field_6cc=op.words[4];break;
        case 10:state=3;break;
        case 20:animation.base.flags[0]=(animation.base.flags[0]&~0xff00u)|(op.words[4]?0x100u:0);break;
        case 33:active_commands=(active_commands&~(std::uint64_t{1}<<33))|(std::uint64_t{op.words[4]&1u}<<33);break;
        }
        ++command_index;
    }
}
int Type1Laser::advance(){
    if((flags&0x20)||!allocated_6d8)return 0;flags=(flags|0x20)&~0x40u;execute_commands();
    if(active_commands){if(active_commands&(std::uint64_t{1}<<30)){auto& t=commands[5].timer;if(t.current<=0)active_commands&=~(std::uint64_t{1}<<30);else n::timer_add(t,-1,state::timer_rate);}if(field_6cc)--field_6cc;}
    if(parameters.length_limit>field_74){field_74=n::add32(field_74,n::mul32(state::clock_scale,field_7c));if(field_74>parameters.length_limit)field_74=parameters.length_limit;}
    angle=m::wrap_angle(n::add32(angle,n::mul32(state::clock_scale,parameters.angular_velocity)));position=parameters.position;
    if((parameters.flags&1)&&gameplay::selected_enemy(context->objects_04[1],0))position=gameplay::enemy_position(gameplay::selected_enemy(context->objects_04[1],0));
    position.x=n::add32(position.x,n::mul32(parameters.velocity.x,state::clock_scale));position.y=n::add32(position.y,n::mul32(parameters.velocity.y,state::clock_scale));position.z=n::add32(position.z,n::mul32(parameters.velocity.z,state::clock_scale));
    if(parameters.radial_offset!=0){sprite::Vec3 v{};m::polar(v.x,v.y,angle,parameters.radial_offset);position.x=n::add32(position.x,v.x);position.y=n::add32(position.y,v.y);}
    switch(state){
    case 3:if(age.current>=parameters.delay){n::timer_set(age,0);state=4;}break;
    case 4:if(age.current<parameters.grow){speed=div(n::mul32(age.current_f,parameters.width),n::int_float(parameters.grow));break;}n::timer_set(age,0);state=2;speed=parameters.width;[[fallthrough]];
    case 2:if(age.current<parameters.sustain)break;n::timer_set(age,0);state=5;[[fallthrough]];
    case 5:if(age.current>=parameters.shrink)return 1;speed=sub(parameters.width,div(n::mul32(age.current_f,parameters.width),n::int_float(parameters.shrink)));break;
    }
    return 0;
}
int Type1Laser::collision_segment(Segment* output){
    flags|=0x40;auto& segment=*static_cast<Segment*>(allocated_6d8);segment.flags&=~8u;
    if((state!=4&&state!=2)||!(field_74>16))return 0;
    float width=speed<32?n::mul32(speed,0.5f):sub(speed,div(n::add32(speed,16),3));
    const float length=n::mul32(field_74,0.949999988079071f);if(flags&0x80)width=sub(speed,speed<32?2.0f:4.0f);
    sprite::Vec3 center{};m::polar(center.x,center.y,angle,div(field_74,2));center.x=n::add32(center.x,position.x);center.y=n::add32(center.y,position.y);center.z=n::add32(center.z,position.z);
    segment.position=center;segment.angle=segment.angle_28=angle;segment.flags=(segment.flags&~3u)|12u;segment.field_2c=0;segment.size={n::mul32(length,1),n::mul32(width,1)};if(output)*output=segment;return 1;
}
int Type1Laser::update(){
    if(advance())return 1;cancel_all(0);auto& sprites=*program_entry::sprite_controller;
    animation.base.vector_50.x=div(speed,sprite::current_sprite(sprites,animation).extent_4c);animation.base.flags[1]|=4;
    animation.base.vector_50.y=div(field_74,sprite::current_sprite(sprites,animation).extent_48);animation.base.flags[1]|=4;
    sprite::execute_animation(animation);if(field_80==0)sprite::execute_animation(origin_animation);flags&=~0x20u;return 0;
}
int Type1Laser::draw(){
    animation.base.vector_2c=position;animation.base.vector_38.z=m::wrap_angle(n::add32(angle,div(3.1415927410125732f,2)));animation.base.flags[1]|=2;
    sprite::draw_animation(*program_entry::sprite_controller,animation);if(field_80==0){origin_animation.base.vector_2c=position;sprite::draw_animation(*program_entry::sprite_controller,origin_animation);}return 0;
}
}
