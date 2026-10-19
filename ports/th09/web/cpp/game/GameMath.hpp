#pragma once
#include "Types.hpp"
namespace th09 {
float script_remainder(float value,float divisor) noexcept;
double add_angle(float angle,float delta) noexcept;
void rotate(Vec2& output,const Vec2& input,float angle) noexcept;
double hermite(float start,float end,float start_tangent,float end_tangent,float t) noexcept;
}
