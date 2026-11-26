// 原CRT的SSE floor(binary64)再经CVTTSD2SI转int32，直接按输入位型计算。
#pragma once
#include <cstdint>

namespace kaikeidou {
inline std::uint32_t floor_to_int32(std::uint64_t argument) noexcept {
    const bool negative = (argument >> 63) != 0;
    const auto exponent_bits = static_cast<unsigned>((argument >> 52) & 0x7ffu);
    const auto fraction = argument & 0xfffffffffffffull;
    if (exponent_bits == 0x7ffu) return 0x80000000u;
    if (exponent_bits < 1023u) return negative && (exponent_bits || fraction) ? 0xffffffffu : 0u;
    const auto exponent = exponent_bits - 1023u;
    // 合法的-2^31与原无效转换都返回同一位型0x80000000。
    if (exponent >= 31u) return 0x80000000u;
    const auto shift = 52u - exponent;
    const auto significand = (1ull << 52) | fraction;
    auto magnitude = static_cast<std::uint32_t>(significand >> shift);
    if (negative && (significand & ((1ull << shift) - 1u))) ++magnitude;
    return negative ? 0u - magnitude : magnitude;
}
}
