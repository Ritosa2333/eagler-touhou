// 有限非负 binary64 的平方根：整数平方比较确定64位根，再按原 x87 精度最近偶数舍入。
#pragma once
#include "core/math/BinaryFloat.hpp"

namespace kaikeidou {
inline binary_float::Number finite_square_root(std::uint64_t bits, unsigned precision) {
    using namespace binary_float;
    if (precision != 24 && precision != 53 && precision != 64) throw std::invalid_argument("Square root precision invalid");
    const auto value = binary64(bits);
    if (!value.mantissa) return value;
    if (value.negative) throw std::domain_error("Square root argument must be nonnegative");
    const bool odd = value.exponent % 2 != 0;
    const Wide radicand = odd ? Wide{value.mantissa, 0} : Wide{value.mantissa >> 1, value.mantissa << 63};
    const auto less = [](Wide a, Wide b) { return a.high < b.high || (a.high == b.high && a.low < b.low); };
    std::uint64_t root = 0;
    for (int bit = 63; bit >= 0; --bit) {
        const auto candidate = root | (std::uint64_t(1) << bit);
        if (!less(radicand, multiply_words(candidate, candidate))) root = candidate;
    }
    const auto squared = multiply_words(root, root);
    auto threshold = squared;
    threshold.low += root;
    if (threshold.low < squared.low) ++threshold.high;
    // 对整数被开方数，根的小数部分超过1/2恰好等价于 N>r*r+r。
    const bool remainder = squared.high != radicand.high || squared.low != radicand.low;
    const auto tail = less(threshold, radicand) ? (std::uint64_t(1) << 63) + 1u : (remainder ? 1u : 0u);
    return rounded({root, (value.exponent - (odd ? 1 : 0)) / 2, false}, tail, precision);
}
}
