#pragma once
#include "Timer.hpp"
namespace th11 {
enum class InterpolationMode : i32 {
    Linear=0,Accelerate2=1,Accelerate3=2,Accelerate4=3,
    Decelerate2=4,Decelerate3=5,Decelerate4=6,Velocity=7,Hermite=8,
    Smooth2=9,Smooth3=10,Smooth4=11,FastSlow2=12,FastSlow3=13,FastSlow4=14,
    HoldStart=15,HoldEnd=16,Acceleration=17
};
float easing(float elapsed,i32 duration,InterpolationMode mode) noexcept;
template<class T,u32 N> struct Interpolator {
    T start[N]{},end[N]{},initial_tangent[N]{},final_tangent[N]{};
    Timer timer;
    i32 duration=0;
    InterpolationMode mode=InterpolationMode::Linear;
};
using Vec2Interpolator=Interpolator<float,2>;
using Vec3Interpolator=Interpolator<float,3>;
using FloatInterpolator=Interpolator<float,1>;
using RgbInterpolator=Interpolator<i32,3>;
using AlphaInterpolator=Interpolator<i32,1>;
struct Rgb {i32 blue,green,red;};
Vec2 sample(Vec2Interpolator& value,const float* rate) noexcept;
Vec3 sample(Vec3Interpolator& value,const float* rate) noexcept;
float sample(FloatInterpolator& value,const float* rate) noexcept;
Rgb sample(RgbInterpolator& value,const float* rate) noexcept;
i32 sample(AlphaInterpolator& value,const float* rate) noexcept;
}
