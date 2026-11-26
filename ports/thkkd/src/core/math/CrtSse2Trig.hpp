// 恢复 0x004f8a08、0x004f99d8 的 SSE2 查表与极小量路径，按原顺序做 binary64 运算。
#pragma once
#include "core/math/CrtTrigTables.hpp"
#include <cstring>
#include <stdexcept>

namespace kaikeidou::crt_sse2 {
inline double real(std::uint64_t bits) { double result; std::memcpy(&result, &bits, 8); return result; }
inline std::uint64_t bits(double value) { std::uint64_t result; std::memcpy(&result, &value, 8); return result; }
// 每个操作单独舍入，防止重排、融合乘加或宿主的额外中间精度。
inline double add(double left, double right) { volatile double result = left + right; return result; }
inline double sub(double left, double right) { volatile double result = left - right; return result; }
inline double mul(double left, double right) { volatile double result = left * right; return result; }
struct Pair { double low = 0, high = 0; };
inline Pair add(Pair left, Pair right) { return {add(left.low, right.low), add(left.high, right.high)}; }
inline Pair sub(Pair left, Pair right) { return {sub(left.low, right.low), sub(left.high, right.high)}; }
inline Pair mul(Pair left, Pair right) { return {mul(left.low, right.low), mul(left.high, right.high)}; }
inline std::uint32_t nearest_integer(double value) {
    const auto encoded = bits(value);
    const auto exponent = static_cast<int>((encoded >> 52) & 2047) - 1023;
    if (exponent < -1) return 0;
    if (exponent >= 31) throw std::range_error("CRT phase integer outside recovered domain");
    const auto mantissa = (encoded & 0xfffffffffffffull) | (std::uint64_t(1) << 52);
    const auto shift = static_cast<unsigned>(52 - exponent);
    auto kept = mantissa >> shift;
    const auto lost = mantissa & ((std::uint64_t(1) << shift) - 1);
    const auto half = std::uint64_t(1) << (shift - 1);
    if (lost > half || (lost == half && (kept & 1))) ++kept;
    const auto integer = static_cast<std::uint32_t>(kept);
    return encoded >> 63 ? 0u - integer : integer;
}
inline bool supports(std::uint64_t argument) noexcept {
    return (argument & 0x7fffffffffffffffull) < 0x40f6000000000000ull;
}

template<std::size_t N> inline double polynomial(double argument, const std::array<std::uint64_t, 256>& table,
    const std::array<std::uint64_t, N>& parameters, std::uint32_t bias) {
    const auto parameter = [&](std::size_t index) { return real(parameters[index]); };
    const auto pair = [&](std::size_t index) { return Pair{parameter(index), parameter(index + 1)}; };
    Pair r0{argument, 0}, r1{parameter(8), 0}, r2{parameter(9), 0};
    r1.low = mul(r1.low, r0.low);
    auto phase = nearest_integer(r1.low);
    r1.low = add(r1.low, r2.low);
    Pair r3{parameter(12), 0};
    r1.low = sub(r1.low, r2.low);
    r2 = pair(10);
    r3.low = mul(r3.low, r1.low);
    r1.high = r1.low;
    phase = (phase + bias) & 63u;
    Pair r4{r0.low, 0}, r5 = pair(6);
    const auto offset = static_cast<std::size_t>(phase) * 4;
    const auto lookup = [&](std::size_t index) { return real(table[offset + index]); };
    r2 = mul(r2, r1);
    r0.low = sub(r0.low, r3.low);
    r1.low = mul(r1.low, parameter(13));
    r4.low = sub(r4.low, r3.low);
    Pair r7{lookup(1), 0};
    r0.high = r0.low;
    r3.low = r4.low;
    r4.low = sub(r4.low, r2.low);
    r5 = mul(r5, r0);
    r0 = sub(r0, r2);
    Pair r6 = pair(2);
    r7.low = mul(r7.low, r4.low);
    r3.low = sub(r3.low, r4.low);
    r5 = mul(r5, r0);
    r0 = mul(r0, r0);
    r3.low = sub(r3.low, r2.low);
    r2 = {lookup(0), lookup(1)};
    r1.low = sub(r1.low, r3.low);
    r3.low = lookup(3);
    r2.low = add(r2.low, r3.low);
    r7.low = sub(r7.low, r2.low);
    r2.low = mul(r2.low, r4.low);
    r6 = mul(r6, r0);
    r3.low = mul(r3.low, r4.low);
    r2 = mul(r2, r0);
    r0 = mul(r0, r0);
    r5 = add(r5, pair(4));
    r4.low = mul(r4.low, lookup(0));
    r6 = add(r6, pair(0));
    r5 = mul(r5, r0);
    r0.low = r3.low;
    r3.low = add(r3.low, lookup(1));
    r1.low = mul(r1.low, r7.low);
    r7.low = r4.low;
    r4.low = add(r4.low, r3.low);
    r6 = add(r6, r5);
    r5.low = lookup(1);
    r5.low = sub(r5.low, r3.low);
    r3.low = sub(r3.low, r4.low);
    r1.low = add(r1.low, lookup(2));
    r6 = mul(r6, r2);
    r5.low = add(r5.low, r0.low);
    r3.low = add(r3.low, r7.low);
    r1.low = add(r1.low, r5.low);
    r1.low = add(r1.low, r3.low);
    r1.low = add(r1.low, r6.low);
    r6 = {r6.high, r6.high};
    r1.low = add(r1.low, r6.low);
    r4.low = add(r4.low, r1.low);
    return r4.low;
}

inline std::uint64_t cosine_bits(std::uint64_t argument) {
    if (!supports(argument)) throw std::range_error("CRT cosine requires large-argument fallback");
    const auto absolute = argument & 0x7fffffffffffffffull;
    if ((absolute >> 48) < 0x3030) return bits(sub(real(crt_trig_data::cos_parameters[18]), real(absolute)));
    return bits(polynomial(real(argument), crt_trig_data::cos_table, crt_trig_data::cos_parameters, 0x1c7610));
}
inline std::uint64_t sine_bits(std::uint64_t argument) {
    if (!supports(argument)) throw std::range_error("CRT sine requires large-argument fallback");
    const auto upper = (argument >> 48) & 0x7fff;
    if (upper < 0x3030) {
        if (upper < 16) return bits(mul(real(argument), real(crt_trig_data::sin_parameters[16])));
        return argument;
    }
    return bits(polynomial(real(argument), crt_trig_data::sin_table, crt_trig_data::sin_parameters, 0x1c7600));
}
}
