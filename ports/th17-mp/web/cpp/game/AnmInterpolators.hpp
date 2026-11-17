#pragma once
#include "Interpolation.hpp"
#include "AnmVariables.hpp"
namespace th17 {
struct AnmInterpolators {
    Vec3Interpolation position;
    ColorInterpolation color;
    AlphaInterpolation alpha;
    Vec3Interpolation rotation;
    AngleInterpolation angle;
    Vec2Interpolation scale,secondary_scale,uv_scale;
    ColorInterpolation secondary_color;
    AlphaInterpolation secondary_alpha;
    ScalarInterpolation uv_x,uv_y;
    u32 active_mask=0;bool mask_valid=false;
    void invalidate()noexcept{mask_valid=false;}
    void advance(AnmVariables&,AnmVisualState&,float rate)noexcept;
    void advance_cached(AnmVariables&,AnmVisualState&,float rate)noexcept;
};
}
