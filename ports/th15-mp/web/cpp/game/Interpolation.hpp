#pragma once
#include "Timer.hpp"
#include "AnmVisualState.hpp"
#include <array>
namespace th15 {
float interpolation_weight(i32 mode,float elapsed,float duration)noexcept;
// Arithmetic order is preserved because interpolation changes collision and
// replay state as well as visual effects. All values are ordinary C++ floats.
template<u32 N,bool Angle=false>struct FloatInterpolation {
    std::array<float,N> start{},end{},control1{},control2{},current{};
    Timer timer{0,0,0,0,0};i32 duration=0,mode=0;
    void begin(i32 frames,i32 easing,const std::array<float,N>& from,const std::array<float,N>& to)noexcept{start=from;end=to;duration=frames;mode=easing;timer.set(0);}
    std::array<float,N> step(float rate=1)noexcept{
        if(duration>0){timer.tick(&rate);if(timer.current>=duration){timer.set(duration);duration=0;}}
        return evaluate();
    }
    std::array<float,N> evaluate()noexcept{
        if(duration==0)return mode==7||mode==17?start:end;
        if(mode==7||mode==17){for(u32 i=0;i<N;i++){
            start[i]=float(start[i]+(mode==7?end[i]:control2[i]));
            if constexpr(Angle)start[i]=normalize_angle(start[i]);
            current[i]=start[i];
            if(mode==17){control2[i]=float((Angle?end[i]:control2[i])+(Angle?control2[i]:end[i]));if constexpr(Angle)control2[i]=normalize_angle(control2[i]);}
        }return current;}
        if(mode==8){const float t=float(timer.fractional/float(duration)),twice=float(t*2),a=float(t-1),b=float(1-t);
            const float h00=float(float(a*a)*float(twice+1)),h11=float(float(a*t)*t),h01=float(float(3-twice)*float(t*t)),h10=float(float(b*b)*t);
            for(u32 i=0;i<N;i++){
                const float p=float(start[i]*h00),q=float(end[i]*h01),r=float(control1[i]*h10),s=float(control2[i]*h11);
                if constexpr(Angle)current[i]=normalize_angle(float(normalize_angle(float(normalize_angle(float(normalize_angle(p)+normalize_angle(q)))+normalize_angle(r)))+normalize_angle(s)));
                else if constexpr(N==3)current[i]=i?float(s+float(r+float(p+q))):float(float(r+float(p+q))+s);
                else if constexpr(N==1)current[i]=float(float(float(q+p)+r)+s);
                else current[i]=float(float(float(p+q)+r)+s);
            }
        }else{const float weight=interpolation_weight(mode,timer.fractional,float(duration));for(u32 i=0;i<N;i++){
            float difference=float(end[i]-start[i]);
            if constexpr(Angle){if(difference>3.1415927410125732421875f)difference=float(end[i]-float(start[i]+6.283185482025146484375f));else if(float(start[i]-end[i])>3.1415927410125732421875f)difference=float(end[i]-float(start[i]-6.283185482025146484375f));difference=normalize_angle(difference);current[i]=normalize_angle(float(start[i]+normalize_angle(float(difference*weight))));}
            else current[i]=float(start[i]+float(difference*weight));
        }}
        return current;
    }
};
using ScalarInterpolation=FloatInterpolation<1>;
using Vec2Interpolation=FloatInterpolation<2>;
using Vec3Interpolation=FloatInterpolation<3>;
using AngleInterpolation=FloatInterpolation<1,true>;
template<u32 N>struct IntegerInterpolation {
    std::array<i32,N> start{},end{},control1{},control2{},current{};
    Timer timer{0,0,0,0,0};i32 duration=0,mode=0;
    void begin(i32 frames,i32 easing,const std::array<i32,N>& from,const std::array<i32,N>& to)noexcept{start=from;end=to;duration=frames;mode=easing;timer.set(0);}
    std::array<i32,N> step(float rate=1)noexcept{
        if(duration>0){timer.tick(&rate);if(timer.current>=duration){timer.set(duration);duration=0;}}
        if(duration==0)return mode==7||mode==17?start:end;
        if(mode==7||mode==17){for(u32 i=0;i<N;i++){start[i]=wrapping_add(start[i],mode==7?end[i]:control2[i]);current[i]=start[i];if(mode==17)control2[i]=wrapping_add(control2[i],end[i]);}return current;}
        if(mode==8){const float t=float(timer.fractional/float(duration)),twice=float(t*2),a=float(t-1),b=float(1-t);
            const float h00=float(float(twice+1)*float(a*a)),h11=float(float(a*t)*t),h01=float(float(3-twice)*float(t*t)),h10=float(float(b*b)*t);
            for(u32 i=0;i<N;i++){const float p=float(float(start[i])*h00),q=float(float(end[i])*h01),r=float(float(control1[i])*h10),s=float(float(control2[i])*h11);
                if constexpr(N==1)current[i]=truncate_int(float(float(float(q+p)+r)+s));
                else current[i]=wrapping_add(wrapping_add(wrapping_add(truncate_int(p),truncate_int(q)),truncate_int(r)),truncate_int(s));
            }
        }else{const float weight=interpolation_weight(mode,timer.fractional,float(duration));for(u32 i=0;i<N;i++){const float delta=float(float(wrapping_sub(end[i],start[i]))*weight);if constexpr(N==1)current[i]=truncate_int(float(delta+float(start[i])));else current[i]=wrapping_add(start[i],truncate_int(delta));}}
        return current;
    }
};
using AlphaInterpolation=IntegerInterpolation<1>;
using ColorInterpolation=IntegerInterpolation<3>;
}
