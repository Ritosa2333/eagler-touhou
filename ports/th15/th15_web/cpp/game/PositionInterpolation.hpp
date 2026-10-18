#pragma once
#include "Interpolation.hpp"
namespace th15 {
// Enemy positions can select one easing mode for the vector or a separate mode
// for each axis. The shared timer advances once, regardless of that selection.
struct PositionInterpolation {
    std::array<float,3> current{},start{},end{},control1{},control2{};
    Timer timer{0,0,0,0,0};i32 duration=0;
    std::array<i32,3> axis_modes{};i32 mode=0;u32 flags=0;
    std::array<float,3> step(float rate=1)noexcept;
    void begin(i32 frames,i32 easing,const std::array<float,3>& from,const std::array<float,3>& to)noexcept{duration=frames;mode=easing;start=from;end=to;flags&=~1u;control1=control2={};timer.set(0);}
};
static_assert(sizeof(PositionInterpolation)==104);
}
