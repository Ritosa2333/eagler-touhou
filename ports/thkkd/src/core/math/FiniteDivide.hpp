// 规格化有限数除法，显式计算舍入位以供扩展精度角度计算使用。
#pragma once
#include "core/math/BinaryFloat.hpp"

namespace kaikeidou::binary_float {
inline Number divide(Number numerator, Number denominator, unsigned precision) {
    if (!denominator.mantissa) throw std::domain_error("Finite division by zero");
    const bool negative = numerator.negative != denominator.negative;
    if (!numerator.mantissa) return {0, 0, negative};
    Number result{std::uint64_t(1) << 63, numerator.exponent - denominator.exponent, negative};
    auto remainder = numerator.mantissa;
    if (remainder < denominator.mantissa) { remainder <<= 1; --result.exponent; }
    remainder -= denominator.mantissa;
    for (int bit = 62; bit >= 0; --bit) {
        const bool carry = (remainder >> 63) != 0;
        remainder <<= 1;
        if (carry || remainder >= denominator.mantissa) {
            remainder -= denominator.mantissa;
            result.mantissa |= std::uint64_t(1) << bit;
        }
    }
    const bool carry = (remainder >> 63) != 0;
    const auto doubled = remainder << 1;
    const auto half = std::uint64_t(1) << 63;
    const std::uint64_t tail = carry || doubled > denominator.mantissa ? half + 1 :
        (doubled == denominator.mantissa ? half : (remainder ? 1 : 0));
    return rounded(result, tail, precision);
}
}
