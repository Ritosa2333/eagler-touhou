#pragma once
#include "Interpolation.hpp"
#include "AnmVariables.hpp"
#include "AnmVisualState.hpp"
namespace th13 {
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
    // 仅色/α 两通道步进（装载趟插值进度补步用——脚本与矩阵不动，
    // 证据：oracle VM+0x15c α=6/+0x160 timer=2 而 world 矩阵 1 步）。
    void advance_color(AnmVisualState&,float rate)noexcept;
};
}
