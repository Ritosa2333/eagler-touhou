#pragma once
#include "Timer.hpp"
namespace th10 {
enum class InterpolationMode : i32 {
    Linear = 0, Accelerate2 = 1, Accelerate3 = 2, Accelerate4 = 3,
    Decelerate2 = 4, Decelerate3 = 5, Decelerate4 = 6,
    Velocity = 7, Hermite = 8, Smooth2 = 9, Smooth3 = 10, Smooth4 = 11,
    FastSlow2 = 12, FastSlow3 = 13, FastSlow4 = 14,
    HoldStart = 15, HoldEnd = 16, Acceleration = 17
};
Extended easing(float elapsed, float duration, InterpolationMode mode) noexcept;
template<class T, unsigned Dimensions> struct Interpolator {
    T start[Dimensions];
    T end[Dimensions];
    T initial_tangent[Dimensions];
    T final_tangent[Dimensions];
    Timer timer;
    u32 flags;
    i32 duration;
    InterpolationMode mode;
};
using Vec2Interpolator = Interpolator<float, 2>;
using Vec3Interpolator = Interpolator<float, 3>;
using RgbInterpolator = Interpolator<i32, 3>;
using AlphaInterpolator = Interpolator<i32, 1>;
static_assert(sizeof(Vec3Interpolator) == 0x4c);
static_assert(sizeof(Vec2Interpolator) == 0x3c);
static_assert(sizeof(AlphaInterpolator) == 0x2c);
Vec2 sample(Vec2Interpolator& interpolation, const float* default_rate) noexcept;
Vec3 sample(Vec3Interpolator& interpolation, const float* default_rate) noexcept;
struct Rgb {i32 blue,green,red;};
Rgb sample(RgbInterpolator& interpolation,const float* default_rate) noexcept;
i32 sample(AlphaInterpolator& interpolation,const float* default_rate) noexcept;
}
