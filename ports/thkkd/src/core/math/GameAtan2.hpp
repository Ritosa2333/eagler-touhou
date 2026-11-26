// 有限输入的高精度 atan2，按原 CRT 的四象限顺序使用 64 位有效数与原 FLDPI 常量。
#pragma once
#include "core/shared/DoubleDouble.hpp"
#include "core/math/FiniteDivide.hpp"
#include "core/shared/AtanConstants.hpp"

namespace kaikeidou::game_atan {
using binary_float::Number;
using wide_float::DoubleDouble;
inline Number pi() { return {0xc90fdaa22168c235ull, 1, false}; }
inline DoubleDouble pair(const std::array<std::uint64_t, 2>& values) { return {crt_sse2::real(values[0]), crt_sse2::real(values[1])}; }
inline Number number(DoubleDouble value) {
    return binary_float::add(binary_float::binary64(crt_sse2::bits(value.high)),
                             binary_float::binary64(crt_sse2::bits(value.low)), 64);
}
inline DoubleDouble pair(Number value) {
    const auto high_bits = binary_float::to_binary64(value);
    auto high_number = binary_float::binary64(high_bits);
    high_number.negative = !high_number.negative;
    const auto remainder = binary_float::add(value, high_number, 64);
    return {crt_sse2::real(high_bits), crt_sse2::real(binary_float::to_binary64(remainder))};
}
inline Number first_quadrant(Number y, Number x) {
    const bool swapped = y.exponent > x.exponent || (y.exponent == x.exponent && y.mantissa > x.mantissa);
    const auto small = swapped ? x : y;
    const auto large = swapped ? y : x;
    const auto ratio = binary_float::divide(small, large, 64);
    if (ratio.exponent < -64) {
        if (!swapped) return ratio;
        if (ratio.exponent < -110) return {pi().mantissa, 0, false};
        return number(wide_float::sub(pair(atan_data::half_pi), pair(ratio)));
    }
    const auto normal = [](Number value) { value.exponent = 0; return crt_sse2::real(binary_float::to_binary64(value)); };
    auto quotient = wide_float::divide({normal(small), 0}, {normal(large), 0});
    const auto shift = small.exponent - large.exponent;
    const auto factor = crt_sse2::real(std::uint64_t(1023 + shift) << 52);
    quotient = {crt_sse2::mul(quotient.high, factor), crt_sse2::mul(quotient.low, factor)};
    const auto index = crt_sse2::nearest_integer(crt_sse2::mul(quotient.high, 16.));
    if (index > 16) throw std::range_error("Atan reduction index outside range");
    const DoubleDouble center{static_cast<double>(index) * .0625, 0};
    const auto reduced = wide_float::divide(wide_float::sub(quotient, center),
        wide_float::add({1, 0}, wide_float::mul(quotient, center)));
    const auto squared = wide_float::mul(reduced, reduced);
    auto power = reduced;
    auto sum = reduced;
    for (unsigned term = 1; term <= 10; ++term) {
        power = wide_float::mul(power, {-squared.high, -squared.low});
        sum = wide_float::add(sum, wide_float::divide(power, {static_cast<double>(term * 2 + 1), 0}));
    }
    auto angle = wide_float::add(pair(atan_data::table[index]), sum);
    if (swapped) angle = wide_float::sub(pair(atan_data::half_pi), angle);
    return number(angle);
}
inline Number finite_atan2(std::uint64_t y_bits, std::uint64_t x_bits) {
    auto y = binary_float::binary64(y_bits), x = binary_float::binary64(x_bits);
    const bool negative_y = y.negative, negative_x = x.negative;
    y.negative = x.negative = false;
    Number result;
    if (!y.mantissa) result = negative_x ? pi() : Number{};
    else if (!x.mantissa) result = {pi().mantissa, 0, false};
    else {
        result = first_quadrant(y, x);
        if (negative_x) { result.negative = true; result = binary_float::add(pi(), result, 64); }
    }
    result.negative = negative_y;
    return result;
}
}
