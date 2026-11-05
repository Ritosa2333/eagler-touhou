#include "type0.hpp"
#include "../gameplay/enemy_variables.hpp"
#include "../gameplay/enemy_state.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../sprite_renderer/binding.hpp"
#include "../damage_regions/geometry.hpp"
#include "../program_entry/program_entry.hpp"
#include "../ecl_vm/math.hpp"
namespace th20::source::laser {
namespace n=recovered;namespace m=ecl::math;
namespace {
float sub(float a,float b){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float div(float a,float b){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b)));}
}
int Type0Laser::measure_rectangle(const sprite::Vec3& center,const sprite::Vec3& size,float rotation,int,int check,int& enemy_contact){
    if(check&&field_6cc)return 0;
    sprite::Vec3 local{sub(position.x,center.x),sub(position.y,center.y),0},rotated_step{},step{},sample{};m::rotate(local.x,local.y,rotation);
    m::polar(rotated_step.x,rotated_step.y,m::wrap_angle(n::add32(angle,rotation)),8);
    const sprite::Vec3 half{div(size.x,2),div(size.y,2),div(size.z,2)};local.x=n::add32(local.x,rotated_step.x);local.y=n::add32(local.y,rotated_step.y);rotated_step.x=n::add32(rotated_step.x,rotated_step.x);rotated_step.y=n::add32(rotated_step.y,rotated_step.y);
    m::polar(step.x,step.y,angle,8);sample={n::add32(position.x,step.x),n::add32(position.y,step.y),0};step.x=n::add32(step.x,step.x);step.y=n::add32(step.y,step.y);
    std::uint32_t count=0;
    for(float distance=8;n::add32(8,distance)<=field_74;distance=n::add32(distance,16)){
        if(!enemy_contact&&gameplay::selected_enemy(context->objects_04[1],0)){
            const auto current_enemy=[&](){return gameplay::selected_enemy(context->objects_04[1],0);};
            const auto descriptor=[&]() -> sprite::SpriteData& {auto* object=static_cast<std::uint8_t*>(current_enemy());auto& state=*reinterpret_cast<gameplay::EnemyState*>(object+0x88);auto& handle=state.animations[0].handle;auto& a=*sprite::resolve_animation_handle(*program_entry::sprite_controller,handle);return sprite::current_sprite(*program_entry::sprite_controller,a);}; //4d6b80→48bce0, actual PMR array atEnemy+94
            const float height=n::mul32(descriptor().extent_48,0.75f),width=n::mul32(descriptor().extent_4c,0.75f);
            const float y=gameplay::enemy_position(current_enemy()).y,x=gameplay::enemy_position(current_enemy()).x;
            if(geometry::rectangle_circle(x,y,width,height,0,sample.x,sample.y,8))enemy_contact=1;
        }
        if(!(-half.x>local.x||local.x>half.x||-half.y>local.y||local.y>half.y)){
            ++count;int value=15;
            if(field_7c>=12)value=45;else if(field_7c>=4&&12>field_7c)value=n::truncate32(n::mul32(n::add32(n::mul32(div(sub(field_7c,4),8),2),1),15));
            if(speed>=96)value=n::truncate32(n::add32(n::add32(n::int_float(value),n::mul32(div(sub(96,16),80),3)),1));
            else if(speed>=16&&96>speed)value=n::truncate32(n::add32(n::add32(n::int_float(value),n::mul32(div(sub(speed,16),80),3)),1));
            static_cast<Controller*>(context->objects_04[4])->field_4c+=static_cast<std::uint32_t>(value);
        }
        sample.x=n::add32(sample.x,step.x);sample.y=n::add32(sample.y,step.y);local.x=n::add32(local.x,rotated_step.x);local.y=n::add32(local.y,rotated_step.y);
    }
    return n::signed_bits(count);
}
}
