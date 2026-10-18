#include "FogInterpolation.hpp"
namespace th11 {
void SceneFog::pack() noexcept {color=0;for(u32 i=0;i<4;++i)color|=(u32(truncate_int(channels[i]))&255)<<(8*i);}
void SceneFog::set(u32 packed,float near_value,float far_value) noexcept {near_distance=near_value;far_distance=far_value;color=packed;for(u32 i=0;i<4;++i)channels[i]=float((packed>>(8*i))&255);}
namespace {
SceneFog combine(const SceneFog& a,const SceneFog& b,i32 operation,float factor=0){
    float x[6],y[6],out[6];std::memcpy(x,&a,24);std::memcpy(y,&b,24);
    for(u32 i=0;i<6;++i)out[i]=operation==0?float(double(x[i])+y[i]):operation==1?float(double(x[i])-y[i]):float(double(x[i])*factor);
    SceneFog result;std::memcpy(&result,out,24);result.pack();return result;
}
SceneFog add(const SceneFog& a,const SceneFog& b){return combine(a,b,0);}
SceneFog scale(const SceneFog& a,float b){return combine(a,{},2,b);}
}
// 4055d0 / 4059d0 / 405a10 / 405a50: every arithmetic operation
// stores its six components as floats before truncating the color bytes.
SceneFog sample(FogInterpolator& value,const float* rate) noexcept {
    if(value.duration>0){value.timer.tick();if(value.timer.current>=value.duration){value.timer.set(value.duration,rate);value.duration=0;return value.mode==InterpolationMode::Velocity?value.start:value.end;}}
    if(value.mode==InterpolationMode::Velocity)return value.start=add(value.end,value.start);
    if(value.mode==InterpolationMode::Acceleration){value.start=add(value.final_tangent,value.start);value.final_tangent=add(value.end,value.final_tangent);return value.start;}
    if(value.mode==InterpolationMode::Hermite){
        const double t=float(double(value.timer.fractional)/value.duration),m=t-1,r=1-t;
        const float a=float((t+t+1)*m*m),b=float((3-(t+t))*t*t),c=float(r*r*t),d=float(m*t*t);
        return add(scale(value.final_tangent,d),add(scale(value.initial_tangent,c),add(scale(value.end,b),scale(value.start,a))));
    }
    return add(scale(combine(value.end,value.start,1),easing(value.timer.fractional,value.duration,value.mode)),value.start);
}
}
