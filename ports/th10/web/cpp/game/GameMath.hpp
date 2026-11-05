#pragma once
#include "Arithmetic.hpp"
namespace th10 {
Extended normalize_angle(float radians) noexcept;
Extended add_angle(float radians,float delta) noexcept;
Extended angle_difference(float target,float current) noexcept;
Extended sine(Extended radians) noexcept;
Extended cosine(Extended radians) noexcept;
Extended tangent(Extended radians) noexcept;
Extended arccosine(Extended value) noexcept;
Extended remainder(Extended value,Extended divisor) noexcept;
Extended angle_to(Extended y, Extended x) noexcept;
Vec2 polar(float radians, float length) noexcept;
}
