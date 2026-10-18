#pragma once
#include "Interpolation.hpp"
namespace th11 {
struct SceneFog {
    float near_distance=0,far_distance=0;
    float channels[4]{};
    u32 color=0;
    void pack() noexcept;
    void set(u32 packed,float near_value,float far_value) noexcept;
};
struct FogInterpolator {
    SceneFog start,end,initial_tangent,final_tangent;
    Timer timer;
    i32 duration=0;
    InterpolationMode mode=InterpolationMode::Linear;
};
static_assert(sizeof(SceneFog)==28);
#if UINTPTR_MAX == UINT32_MAX
static_assert(sizeof(FogInterpolator)==0x8c);
#endif
SceneFog sample(FogInterpolator&,const float*) noexcept;
}
