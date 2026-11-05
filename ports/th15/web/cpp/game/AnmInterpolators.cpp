#include "AnmInterpolators.hpp"
namespace th15 {
namespace {
Vec3 v3(const std::array<float,3>& v){return {v[0],v[1],v[2]};}
Vec2 v2(const std::array<float,2>& v){return {v[0],v[1]};}
void rgb(u32& color,const std::array<i32,3>& c){color=(color&0xff000000)|u8(c[0])|(u32(u8(c[1]))<<8)|(u32(u8(c[2]))<<16);}
}
void AnmInterpolators::advance(AnmVariables& variables,AnmVisualState& visual,float rate)noexcept{
    if(position.duration){const Vec3 value=v3(position.step(rate));if(visual.flags&0x400)visual.child_anchor=value;else variables.position=value;}
    if(color.duration)rgb(visual.color,color.step(rate));
    if(alpha.duration)visual.color=(visual.color&0xffffff)|(u32(u8(alpha.step(rate)[0]))<<24);
    if(scale.duration){visual.scale=v2(scale.step(rate));visual.flags|=8;}
    if(secondary_scale.duration){visual.secondary_scale=v2(secondary_scale.step(rate));visual.flags|=8;}
    if(uv_scale.duration){visual.uv_scale=v2(uv_scale.step(rate));visual.flags|=0x10;}
    if(rotation.duration){variables.rotation=v3(rotation.step(rate));visual.flags|=4;}
    if(angle.duration){variables.rotation.z=angle.step(rate)[0];visual.flags|=4;}
    if(secondary_color.duration)rgb(visual.secondary_color,secondary_color.step(rate));
    if(secondary_alpha.duration)visual.secondary_color=(visual.secondary_color&0xffffff)|(u32(u8(secondary_alpha.step(rate)[0]))<<24);
    if(uv_x.duration)visual.uv_velocity.x=uv_x.step(rate)[0];
    if(uv_y.duration)visual.uv_velocity.y=uv_y.step(rate)[0];
}
}
