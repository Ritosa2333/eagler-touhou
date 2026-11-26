// 原x87的有限binary32装载及最近偶数舍入写回，直接处理有效数以保留次正规边界。
#pragma once
#include "core/math/BinaryFloat.hpp"
namespace kaikeidou::binary_float {
inline Number finite_binary32(std::uint32_t bits) {
    const auto field = (bits >> 23) & 255u;
    if (field == 255) throw std::domain_error("binary32 argument must be finite");
    Number number{std::uint64_t(bits & 0x7fffffu) << 40, static_cast<int>(field) - 127, (bits >> 31) != 0};
    if (field) number.mantissa |= std::uint64_t(1) << 63;
    else if (number.mantissa) {
        number.exponent = -126;
        while (!(number.mantissa >> 63)) { number.mantissa <<= 1; --number.exponent; }
    }
    return number;
}
inline std::uint32_t to_binary32(Number number) {
    const auto sign = std::uint32_t(number.negative) << 31;
    if (!number.mantissa) return sign;
    if (number.exponent < -126) {
        // binary32次正规单位为2^-149，规范化64位有效数需右移-exponent-86位。
        if (number.exponent < -150) return sign;
        const auto shift = static_cast<unsigned>(-number.exponent - 86);
        if (shift == 64) return sign | (number.mantissa > (std::uint64_t(1) << 63) ? 1u : 0u);
        auto kept = number.mantissa >> shift;
        const auto lost = number.mantissa & ((std::uint64_t(1) << shift) - 1);
        const auto half = std::uint64_t(1) << (shift - 1);
        if (lost > half || (lost == half && (kept & 1))) ++kept;
        return sign | static_cast<std::uint32_t>(kept);
    }
    if (number.exponent > 127) return sign | 0x7f800000u;
    number = rounded(number, 0, 24);
    if (number.exponent > 127) return sign | 0x7f800000u;
    return sign | (std::uint32_t(number.exponent + 127) << 23) | static_cast<std::uint32_t>((number.mantissa >> 40) & 0x7fffffu);
}
}
