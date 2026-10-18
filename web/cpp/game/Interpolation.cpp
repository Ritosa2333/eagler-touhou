#include "Interpolation.hpp"
#include <cmath>
namespace th15 {
float interpolation_weight(i32 mode,float elapsed,float duration)noexcept{
    if(duration==0||std::isnan(duration))return 1;
    float t=float(elapsed/duration);
    auto power=[](float v,u32 exponent){float r=v;for(u32 i=1;i<exponent;i++)r=float(r*v);return r;};
    if(mode>=1&&mode<=3)return power(t,u32(mode+1));
    if(mode>=4&&mode<=6)return float(1-power(float(1-t),u32(mode-2)));
    if(mode>=9&&mode<=14){const u32 exponent=u32((mode-9)%3+2);t=float(t*2);
        if(mode<=11)return t<1?float(power(t,exponent)*.5f):float(float(2-power(float(2-t),exponent))*.5f);
        return t<1?float(.5f-float(power(float(1-t),exponent)*.5f)):float(float(power(float(t-1),exponent)*.5f)+.5f);
    }
    if(mode==15)return 0;if(mode==16)return 1;
    auto sine=[](float angle){return float(std::sin(double(angle)));};
    if(mode==18)return sine(float(float(t*3.1415927410125732421875f)*.5f));
    if(mode==19)return float(1-sine(float(float(float(t*3.1415927410125732421875f)*.5f)+1.57079637050628662109375f)));
    if(mode==20){t=float(t*2);const float s=sine(float(float(t*3.1415927410125732421875f)*.5f));return t<1?float(s*.5f):float(float(float(1-s)*.5f)+.5f);}
    if(mode==21){t=float(t*2);if(t<1)return float(float(1-sine(float(float(float(t*3.1415927410125732421875f)*.5f)+1.57079637050628662109375f)))*.5f);return float(.5f+float(sine(float(float(float(t-1)*3.1415927410125732421875f)*.5f))*.5f));}
    if(mode>=22&&mode<=31){constexpr float shift[]={.25f,.300000011920928955078125f,.3499999940395355224609375f,.37999999523162841796875f,.4000000059604644775390625f};
        constexpr float divisor[]={.5625f,.4899999797344207763671875f,.422499954700469970703125f,.38440001010894775390625f,.36000001430511474609375f};
        constexpr float subtract[]={.111111111938953399658203125f,.18367348611354827880859375f,.28994083404541015625f,.375650346279144287109375f,.4444444477558135986328125f};
        constexpr float scale[]={.888888895511627197265625f,.81632649898529052734375f,.71005916595458984375f,.624349653720855712890625f,.5555555820465087890625f};
        const u32 i=u32((mode-22)%5);if(mode>=27)t=float(1-t);t=float(t-shift[i]);t=float(t*t);t=float(t/divisor[i]);t=float(t-subtract[i]);t=float(t/scale[i]);return mode>=27?float(1-t):t;
    }
    return t;
}
}
