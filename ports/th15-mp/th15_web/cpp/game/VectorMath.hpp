#pragma once
#include "Types.hpp"
#include <cmath>
namespace th15 {
inline Vec2 normalize_vector(const Vec2& value)noexcept{
    const float squared=float(float(value.x*value.x)+float(value.y*value.y));if(squared<0x1p-46f)return {};
    const float inverse=float(1.f/float(std::sqrt(double(squared))));
    const float correction=float(3.f-float(float(squared*inverse)*inverse));
    const float refined=float(float(.5f*inverse)*correction);
    return {float(value.x*refined),float(value.y*refined)};
}
}
