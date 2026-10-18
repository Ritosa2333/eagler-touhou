#include "StageFog.hpp"
namespace th15 {
void StageFog::set(u32 packed,float n,float f)noexcept{color=packed;near_distance=n;far_distance=f;for(u32 i=0;i<4;++i)channels[i]=float((packed>>(i*8))&255);}
void StageFog::pack()noexcept{color=0;for(u32 i=0;i<4;++i)color|=(u32(truncate_int(channels[i]))&255)<<(i*8);}
namespace {
StageFog add(const StageFog& a,const StageFog& b)noexcept{StageFog out;out.near_distance=float(b.near_distance+a.near_distance);out.far_distance=float(b.far_distance+a.far_distance);for(u32 i=0;i<4;++i)out.channels[i]=float(b.channels[i]+a.channels[i]);out.pack();return out;}
StageFog subtract(const StageFog& a,const StageFog& b)noexcept{StageFog out;out.near_distance=float(a.near_distance-b.near_distance);out.far_distance=float(a.far_distance-b.far_distance);for(u32 i=0;i<4;++i)out.channels[i]=float(a.channels[i]-b.channels[i]);out.pack();return out;}
StageFog multiply(const StageFog& a,float t)noexcept{StageFog out;out.near_distance=float(a.near_distance*t);out.far_distance=float(a.far_distance*t);for(u32 i=0;i<4;++i)out.channels[i]=float(a.channels[i]*t);out.pack();return out;}
}
StageFog StageFogInterpolation::step(float rate)noexcept{
    if(duration>0){timer.tick(&rate);if(timer.current>=duration){timer.set(duration);duration=0;}}
    if(duration==0)return mode==7||mode==17?start:end;
    if(mode==7){start=add(start,end);current=start;return current;}
    if(mode==17){start=add(start,control2);control2=add(control2,end);current=start;return current;}
    if(mode==8){const float t=float(timer.fractional/float(duration)),twice=float(t*2),a=float(t-1),b=float(1-t);const float h00=float(float(a*a)*float(twice+1)),h11=float(float(a*t)*t),h01=float(float(3-twice)*float(t*t)),h10=float(float(b*b)*t);current=add(add(add(multiply(start,h00),multiply(end,h01)),multiply(control1,h10)),multiply(control2,h11));}
    else current=add(multiply(subtract(end,start),interpolation_weight(mode,timer.fractional,float(duration))),start);
    return current;
}
}
