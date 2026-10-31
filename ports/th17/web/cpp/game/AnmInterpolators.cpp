#include "AnmInterpolators.hpp"
namespace th17 {
namespace {
Vec3 v3(const std::array<float,3>& v){return {v[0],v[1],v[2]};}
Vec2 v2(const std::array<float,2>& v){return {v[0],v[1]};}
void rgb(u32& color,const std::array<i32,3>& c){color=(color&0xff000000)|u8(c[0])|(u32(u8(c[1]))<<8)|(u32(u8(c[2]))<<16);}
}
void AnmInterpolators::advance(AnmVariables& variables,AnmVisualState& visual,float rate)noexcept{
    invalidate();advance_cached(variables,visual,rate);
}
void AnmInterpolators::advance_cached(AnmVariables& variables,AnmVisualState& visual,float rate)noexcept{
    if(!mask_valid){const i32 durations[]={position.duration,color.duration,alpha.duration,scale.duration,secondary_scale.duration,uv_scale.duration,rotation.duration,angle.duration,secondary_color.duration,secondary_alpha.duration,uv_x.duration,uv_y.duration};active_mask=0;for(u32 i=0;i<12;i++)if(durations[i])active_mask|=1u<<i;mask_valid=true;}
    // Waiting particles typically use one interpolator. Do not fetch twelve
    // distant timer cache lines for every VM on every frame. Instruction and
    // host mutation sites invalidate this index; completed timers leave it.
    const auto finish=[&](u32 bit,i32 duration){if(!duration)active_mask&=~(1u<<bit);};
    if(active_mask&1){const Vec3 value=v3(position.step(rate));if(visual.flags&0x400)visual.child_anchor=value;else variables.position=value;finish(0,position.duration);}
    if(active_mask&2){rgb(visual.color,color.step(rate));finish(1,color.duration);}
    if(active_mask&4){visual.color=(visual.color&0xffffff)|(u32(u8(alpha.step(rate)[0]))<<24);finish(2,alpha.duration);}
    if(active_mask&8){visual.scale=v2(scale.step(rate));visual.flags|=8;finish(3,scale.duration);}
    if(active_mask&16){visual.secondary_scale=v2(secondary_scale.step(rate));visual.flags|=8;finish(4,secondary_scale.duration);}
    if(active_mask&32){visual.uv_scale=v2(uv_scale.step(rate));visual.flags|=0x10;finish(5,uv_scale.duration);}
    if(active_mask&64){variables.rotation=v3(rotation.step(rate));visual.flags|=4;finish(6,rotation.duration);}
    if(active_mask&128){variables.rotation.z=angle.step(rate)[0];visual.flags|=4;finish(7,angle.duration);}
    if(active_mask&256){rgb(visual.secondary_color,secondary_color.step(rate));finish(8,secondary_color.duration);}
    if(active_mask&512){visual.secondary_color=(visual.secondary_color&0xffffff)|(u32(u8(secondary_alpha.step(rate)[0]))<<24);finish(9,secondary_alpha.duration);}
    if(active_mask&1024){visual.uv_velocity.x=uv_x.step(rate)[0];finish(10,uv_x.duration);}
    if(active_mask&2048){visual.uv_velocity.y=uv_y.step(rate)[0];finish(11,uv_y.duration);}
}
}
