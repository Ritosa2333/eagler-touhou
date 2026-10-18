#include "GameMath.hpp"
#include <cmath>
namespace th09 {
float script_remainder(float value,float divisor) noexcept {
    // The original CRT remainder path produces positive zero, also for -0 input.
    const float result=float(std::fmod(double(value),double(divisor)));return result==0?0.f:result;
}
double add_angle(float angle,float delta) noexcept {
    // TH09 0x42aed0 retains registers, each arithmetic operation rounded to 24 bits.
    // Its original loop also stops after 18 reductions; large angles must retain that limit.
    float value=angle+delta;u32 iterations=0;
    while(value>float(0x1.921fb6p1f)){value-=float(0x1.921fb6p2f);if(iterations++>16)break;}
    while(value<float(-0x1.921fb6p1f)){value+=float(0x1.921fb6p2f);if(iterations++>16)break;}
    return value;
}
void rotate(Vec2& out,const Vec2& in,float angle) noexcept {
    // 0x42af40 stores sin to float, but keeps cos at x87 arithmetic precision.
    const float s=float(std::sin(double(angle)));const double c=std::cos(double(angle));
    out.x=float(c*double(in.x))-s*in.y;
    out.y=float(c*double(in.y))+s*in.x;
}
double hermite(float a,float b,float ta,float tb,float position) noexcept {
    const float t=position,twice=t+t,minus=t-1.0f,remaining=1.0f-t;
    // Original addition order from 0x401000; FMA/reassociation stay disabled.
    float value=((3.0f-twice)*t*t)*b;
    value+=((twice+1.0f)*minus*minus)*float(a);
    value+=(remaining*remaining*t)*float(ta);
    value+=(minus*t*t)*float(tb);
    return value;
}
}
