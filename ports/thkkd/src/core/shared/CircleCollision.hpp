// 恢复 0x00657e40..0x00657e8b 的圆形碰撞：先存 double 差值与半径和，再按 x87 精度严格比较。
#pragma once
#include "core/math/BinaryFloat.hpp"

namespace kaikeidou {
inline bool circles_overlap(std::uint64_t ax, std::uint64_t ay, std::uint64_t bx, std::uint64_t by,
    std::uint64_t ar, std::uint64_t br, unsigned precision) {
    using namespace binary_float;
    const auto difference = [precision](std::uint64_t left, std::uint64_t right) {
        auto second = binary64(right);
        second.negative = !second.negative;
        return binary64(to_binary64(add(binary64(left), second, precision)));
    };
    const auto dx = difference(ax, bx);
    const auto dy = difference(ay, by);
    const auto radius = binary64(to_binary64(add(binary64(ar), binary64(br), precision)));
    const auto distance_squared = add(multiply(dx, dx, precision), multiply(dy, dy, precision), precision);
    const auto radius_squared = multiply(radius, radius, precision);
    if (!radius_squared.mantissa) return false;
    if (!distance_squared.mantissa) return true;
    return radius_squared.exponent > distance_squared.exponent ||
        (radius_squared.exponent == distance_squared.exponent && radius_squared.mantissa > distance_squared.mantissa);
}
}
