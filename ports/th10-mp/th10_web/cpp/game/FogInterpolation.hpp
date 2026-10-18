#pragma once
#include "AnmProjection.hpp"
namespace th10 {
struct FogInterpolator {
    AnmFog start,end,initial_tangent,final_tangent;
    Timer timer;
    u32 flags;
    i32 duration;
    InterpolationMode mode;
};
static_assert(sizeof(FogInterpolator)==0x8c);
AnmFog fog_add(const AnmFog& first,const AnmFog& second) noexcept;
AnmFog fog_subtract(const AnmFog& first,const AnmFog& second) noexcept;
AnmFog fog_scale(const AnmFog& value,float scale) noexcept;
void pack_fog_color(AnmFog& value) noexcept;
AnmFog sample(FogInterpolator& interpolation,const float* default_rate) noexcept;
}
