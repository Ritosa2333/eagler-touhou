// 0x643633..0x6436EA 的符卡角速度和扣分计算，保留原 x87 舍入及 SSE 转整数结果。
#pragma once
#include "core/math/FiniteDivide.hpp"
#include "core/math/Random.hpp"

namespace kaikeidou {
inline std::uint64_t boss_spell_ring_rate(std::uint32_t seconds, RandomPrecision precision) {
    using namespace binary_float;
    const auto bits = static_cast<unsigned>(precision);
    const auto denominator = multiply(integer(seconds), integer(60), bits);
    if (!denominator.mantissa) return 0x7ff0000000000000ull;
    return to_binary64(divide(binary64(0x3feae147ae147ae1ull), denominator, bits));
}

inline std::uint64_t boss_spell_bonus_argument(std::uint32_t bonus, std::uint32_t seconds,
    RandomPrecision precision) {
    using namespace binary_float;
    const auto bits = static_cast<unsigned>(precision);
    auto five = integer(5); five.negative = true;
    const auto denominator = add(integer(seconds), five, bits);
    const auto numerator = integer(bonus);
    if (!denominator.mantissa) {
        if (!numerator.mantissa) return 0xfff8000000000000ull;
        return (std::uint64_t(numerator.negative) << 63) | 0x7ff0000000000000ull;
    }
    const auto quotient = divide(numerator, denominator, bits);
    const auto scaled = multiply(quotient, binary64(0x3fb3333333333333ull), bits);
    return to_binary64(divide(scaled, integer(60), bits));
}

inline std::uint32_t boss_spell_bonus_decrement(std::uint32_t bonus, std::uint32_t seconds,
    RandomPrecision precision) {
    const auto argument = boss_spell_bonus_argument(bonus, seconds, precision);
    if ((argument & 0x7ff0000000000000ull) == 0x7ff0000000000000ull) return 0;
    const auto value = binary_float::binary64(argument);
    if (!value.mantissa) return 0;
    if (value.exponent < 0) return value.negative ? 0u : 10u;
    if (value.exponent > 31) return 0;
    const auto shift = static_cast<unsigned>(63 - value.exponent);
    auto magnitude = value.mantissa >> shift;
    if (!value.negative && (value.mantissa & ((std::uint64_t(1) << shift) - 1))) ++magnitude;
    // ceil 后超出 int32 的值经 CVTTSD2SI 得到 0x80000000，再乘10回绕为0。
    if (magnitude > (value.negative ? 0x80000000ull : 0x7fffffffull)) return 0;
    const auto converted = value.negative ? 0u - static_cast<std::uint32_t>(magnitude) :
        static_cast<std::uint32_t>(magnitude);
    return converted * 10u;
}
}
