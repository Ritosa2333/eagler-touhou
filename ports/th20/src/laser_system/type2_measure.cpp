#include "type2.hpp"
#include "../gameplay/enemy_variables.hpp"
#include "../gameplay/enemy_state.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../sprite_renderer/binding.hpp"
#include "../damage_regions/geometry.hpp"
#include "../program_entry/program_entry.hpp"
namespace th20::source::laser {
namespace n=recovered;
namespace {
float sub(float a,float b){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float div(float a,float b){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b)));}
}
int Type2Laser::measure_rectangle(const sprite::Vec3& center,const sprite::Vec3& size,float rotation,int,int check,int& enemy_contact){
    if(check&&field_6cc)return 0;std::uint32_t count=0;
    for(std::int32_t i=0;i<n::signed_bits(parameters.count);++i){const auto& sample=samples[i].position;
        if(!enemy_contact){
            const auto current_enemy=[&](){return gameplay::selected_enemy(context->objects_04[1],0);};
            const auto descriptor=[&]() -> sprite::SpriteData& {auto* object=static_cast<std::uint8_t*>(current_enemy());auto& state=*reinterpret_cast<gameplay::EnemyState*>(object+0x88);auto& handle=state.animations[0].handle;auto& a=*sprite::resolve_animation_handle(*program_entry::sprite_controller,handle);return sprite::current_sprite(*program_entry::sprite_controller,a);};
            const float height=n::mul32(descriptor().extent_48,.75f),width=n::mul32(descriptor().extent_4c,.75f);const float y=gameplay::enemy_position(current_enemy()).y,x=gameplay::enemy_position(current_enemy()).x;
            if(geometry::rectangle_circle(x,y,width,height,0,sample.x,sample.y,8))enemy_contact=1;
        }
        if(curve_rectangle_point(center,size,rotation,sample)){
            ++count;int value=15;if(field_7c>=12)value=45;else if(field_7c>=4&&12>field_7c)value=n::truncate32(n::mul32(n::add32(n::mul32(div(sub(field_7c,4),8),2),1),15));
            if(speed>=96)value=n::truncate32(n::add32(n::add32(n::int_float(value),n::mul32(div(sub(96,16),80),3)),1));else if(speed>=16&&96>speed)value=n::truncate32(n::add32(n::add32(n::int_float(value),n::mul32(div(sub(speed,16),80),3)),1));
            static_cast<Controller*>(context->objects_04[4])->field_4c+=static_cast<std::uint32_t>(value);
        }
    }
    return n::signed_bits(count);
}
}
