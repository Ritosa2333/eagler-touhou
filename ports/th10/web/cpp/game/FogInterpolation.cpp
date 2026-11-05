#include "FogInterpolation.hpp"
namespace th10 {
namespace {
AnmFog result(const Extended* values){
    AnmFog out{};out.near_distance=values[0].to_float();out.far_distance=values[1].to_float();
    for(u32 i=0;i<4;++i){out.color[i]=values[i+2].to_float();out.packed_color|=(static_cast<u32>(values[i+2].truncate_int())&255)<<(i*8);}return out;
}
void numbers(const AnmFog& value,float* out){out[0]=value.near_distance;out[1]=value.far_distance;for(u32 i=0;i<4;++i)out[i+2]=value.color[i];}
}
// 0x404d40 / 0x404da0 / 0x404e10 convert color bytes before the extended
// color intermediates are discarded. Every compound operation repacks them.
AnmFog fog_add(const AnmFog& a,const AnmFog& b) noexcept {float first[6],second[6];numbers(a,first);numbers(b,second);Extended values[6];for(u32 i=0;i<6;++i)values[i]=number(first[i])+number(second[i]);return result(values);}
AnmFog fog_subtract(const AnmFog& a,const AnmFog& b) noexcept {float first[6],second[6];numbers(a,first);numbers(b,second);Extended values[6];for(u32 i=0;i<6;++i)values[i]=number(first[i])-number(second[i]);return result(values);}
AnmFog fog_scale(const AnmFog& value,float scale) noexcept {float inputs[6];numbers(value,inputs);Extended values[6];for(u32 i=0;i<6;++i)values[i]=number(scale)*number(inputs[i]);return result(values);}
void pack_fog_color(AnmFog& value) noexcept {value.packed_color=0;for(u32 i=0;i<4;++i)value.packed_color|=(static_cast<u32>(Scalar::truncate(value.color[i]))&255)<<(i*8);}
// 0x4049a0. Fog has its own interpolation because each vector operation
// stores six floats and packs four bytes, unlike the sprite vector paths.
AnmFog sample(FogInterpolator& value,const float* default_rate) noexcept {
    if(value.duration>0){value.timer.tick();if(value.timer.current>=value.duration){
        if(!(value.flags&1)){value.timer.rate=default_rate;value.flags|=1;}
        value.timer.current=value.duration;value.timer.previous=wrapping_add(value.duration,-1);value.timer.fractional=Extended::from_int(value.duration).to_float();value.duration=0;
        return value.mode==InterpolationMode::Velocity?value.start:value.end;
    }}
    if(value.mode==InterpolationMode::Velocity){value.start=fog_add(value.end,value.start);return value.start;}
    if(value.mode==InterpolationMode::Acceleration){value.start=fog_add(value.final_tangent,value.start);value.final_tangent=fog_add(value.end,value.final_tangent);return value.start;}
    if(value.mode==InterpolationMode::Hermite){
        const auto t=number(value.timer.fractional)/Extended::from_int(value.duration),one=number(1),twice=t+t,minus_one=number((t-one).to_float());
        const auto first=((one+twice)*minus_one*minus_one).to_float(),second=((number(3)-twice)*t*t).to_float();
        const auto third=((one-t)*(one-t)*t).to_float(),fourth=(minus_one*t*t).to_float();
        const auto a=fog_scale(value.start,first),b=fog_scale(value.end,second),c=fog_scale(value.initial_tangent,third),d=fog_scale(value.final_tangent,fourth);
        return fog_add(d,fog_add(c,fog_add(b,a)));
    }
    const auto t=easing(value.timer.fractional,Extended::from_int(value.duration).to_float(),value.mode).to_float();
    return fog_add(value.start,fog_scale(fog_subtract(value.end,value.start),t));
}
}
