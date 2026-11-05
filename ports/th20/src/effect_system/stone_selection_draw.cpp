#include "stone_selection.hpp"
#include "rings.hpp"
#include "../ecl_vm/math.hpp"
#include "../sprite_renderer/anm_vm.hpp"
#include "../sprite_renderer/draw.hpp"
#include "../sprite_renderer/render_state.hpp"
#include <cstring>
namespace th20::source::effects {
namespace n=recovered;namespace s=sprite;namespace m=ecl::math;
namespace {
float div(float a,float b){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b)));}
std::uint32_t shade(std::uint32_t color,std::uint32_t other){ //4632b0 then44c650/byte alpha product
    std::uint32_t result=(((color>>24)*(other>>24))>>8)<<24;
    for(unsigned shift=0;shift<24;shift+=8)result|=((((color>>shift)&255u)+((other>>shift)&255u))>>1)<<shift;return result;
}
void position(s::Controller& c,std::uint32_t handle,s::Vec3 p){if(auto* a=s::find_animation(c,handle))a->vector_5bc=p;}
}
void StoneSelection::draw(){
    auto& c=environment::sprites();auto& a=*animation;auto* background=s::resolve_animation_handle(c,background_handle);
    s::apply_animation_render_state(c,*background,s::draw_environment::device());
    auto* scale_owner=s::resolve_animation_handle(c,background_handle);if(!scale_owner)scale_owner=&c.animation_dc;
    if(auto* center=s::find_animation(c,center_handle))center->base.vector_50=scale_owner->base.vector_50;
    const auto back_position=s::animation_position(*background);auto origin=s::animation_position(a);origin={n::add32(origin.x,back_position.x),n::add32(origin.y,back_position.y),n::add32(origin.z,back_position.z)};
    for(auto handle:handles)if(auto* child=s::find_animation(c,handle))child->base.vector_58=background->base.vector_50;
    std::memcpy(points,basis,sizeof(points));
    for(auto& p:points){
        p.x=n::mul32(p.x,a.base.vector_50.x);p.y=n::mul32(p.y,a.base.vector_50.x);m::rotate(p.x,p.y,a.base.vector_38.z);
        p.x=n::mul32(p.x,background->base.vector_50.x);p.y=n::mul32(p.y,background->base.vector_50.x);m::rotate(p.x,p.y,background->base.vector_38.z);
        p.x=n::mul32(p.x,n::mul32(s::anm_environment::screen_scale(),.5f));p.y=n::mul32(p.y,n::mul32(s::anm_environment::screen_scale(),.5f));m::rotate(p.x,p.y,background->base.vector_38.z); //original repeats this rotation
    }
    std::memcpy(draw_colors,colors,sizeof(colors));for(auto& value:draw_colors){value=shade(value,a.base.field_490);value=shade(value,background->base.field_490);}
    if(expanded){const float r=n::mul32(n::mul32(n::mul32(background->base.vector_50.x,112),s::anm_environment::screen_scale()),.5f);s::primitive::p43b960(c,origin.x,origin.y,r,0,8,0x40u,0xa0000020u);}
    for(unsigned i=0;i<4;++i)draw_triangle_fan(c,3,origin,points+i*3,draw_colors+i*3);
    float circle_radius=div(n::mul32(n::mul32(background->base.vector_50.x,radius),a.base.vector_50.x),3);
    circle_radius=n::mul32(n::mul32(s::anm_environment::screen_scale(),.5f),circle_radius);
    const std::uint32_t circle_color=((128u*(a.base.field_490>>24))>>8)<<24;if(expanded)circle_radius=n::mul32(circle_radius,1.7999999523162842f);
    constexpr int selection_order[]={3,4,2,1};
    for(unsigned i=0;i<4;++i){const auto& p=points[i*3+1];s::primitive::p43b960(c,n::add32(p.x,origin.x),n::add32(p.y,origin.y),selection==selection_order[i]?circle_radius:div(circle_radius,1.5f),0,32,circle_color,circle_color);}
    constexpr unsigned child_order[]={2,3,1,0};
    for(unsigned i=0;i<4;++i){const auto& p=points[i*3+1];const s::Vec3 point{n::add32(p.x,origin.x),n::add32(p.y,origin.y),0};const float scale=n::mul32(s::anm_environment::screen_scale(),.5f);position(c,handles[child_order[i]],{div(point.x,scale),div(point.y,scale),div(point.z,scale)});}
    const float scale=n::mul32(s::anm_environment::screen_scale(),.5f);position(c,center_handle,{div(origin.x,scale),div(origin.y,scale),div(origin.z,scale)});
    for(unsigned i=0;i<4;++i)if(pulses[i].enabled){const auto& p=points[i*3+1];s::primitive::p43bc60(c,n::add32(p.x,origin.x),n::add32(p.y,origin.y),n::mul32(circle_radius,pulses[i].radius),0,48,circle_color);}
}
}
