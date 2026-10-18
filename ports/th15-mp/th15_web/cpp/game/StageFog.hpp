#pragma once
#include "Interpolation.hpp"
namespace th15 {
struct StageFog {
    float near_distance=0,far_distance=0;std::array<float,4> channels{};u32 color=0;
    void set(u32 packed,float near_value,float far_value)noexcept;
    void pack()noexcept;
};
struct StageFogInterpolation {
    StageFog start{},end{},control1{},control2{},current{};
    Timer timer{0,0,0,0,0};i32 duration=0,mode=0;
    StageFog step(float rate=1)noexcept;
};
static_assert(sizeof(StageFog)==28);static_assert(sizeof(StageFogInterpolation)==168);
}
