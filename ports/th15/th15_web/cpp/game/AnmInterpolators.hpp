#pragma once
#include "Interpolation.hpp"
#include "AnmVariables.hpp"
namespace th15 {
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
    void advance(AnmVariables&,AnmVisualState&,float rate)noexcept;
};
}
