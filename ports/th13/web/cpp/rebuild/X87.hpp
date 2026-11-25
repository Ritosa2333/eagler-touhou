#pragma once

#include <cmath>
#include <cstdint>
#include <cstring>

#include "../game/Types.hpp"

namespace th13::rebuild {

struct Ext80 {
    i32 exponent = 0;
    u64 significand = 0;
};

inline void split_square(double value, double& square, double& error) {
    const double split = value * 134217729.0;
    const double high = split - (split - value);
    const double low = value - high;
    square = value * value;
    error = ((high * high - square) + 2.0 * high * low) + low * low;
}

inline Ext80 square_sum_80(double a, double b) {
    double high = 0.0, low = 0.0, other_high = 0.0, other_low = 0.0;
    split_square(a, high, low);
    split_square(b, other_high, other_low);
    const double sum = high + other_high;
    const double virtual_value = sum - high;
    double tail = (high - (sum - virtual_value)) + (other_high - virtual_value);
    tail += low + other_low;
    if (sum == 0.0) return {};
    i32 exponent = 0;
    const double fraction = std::frexp(sum, &exponent);
    u64 significand = u64(std::ldexp(fraction, 53)) << 11;
    if (tail != 0.0) {
        const i64 count = i64(std::nearbyint(std::ldexp(tail, 64 - exponent)));
        significand = count < 0 ? significand - u64(-count) : significand + u64(count);
    }
    while (significand < (u64(1) << 63)) {
        significand <<= 1;
        --exponent;
    }
    return {exponent, significand};
}

inline float to_float(const Ext80& value) {
    if (value.significand == 0) return 0.0f;
    const u64 remainder = value.significand & ((u64(1) << 40) - 1);
    const u64 half = u64(1) << 39;
    u64 rounded = value.significand >> 40;
    if (remainder > half || (remainder == half && (rounded & 1u) != 0)) ++rounded;
    i32 exponent = value.exponent;
    if (rounded == (u64(1) << 24)) {
        rounded >>= 1;
        ++exponent;
    }
    return float(std::ldexp(double(rounded), exponent - 24));
}

inline bool below_square_sum(float squared, double a, double b) {
    const Ext80 sum = square_sum_80(a, b);
    u32 bits = 0;
    std::memcpy(&bits, &squared, sizeof bits);
    bits &= 0x7fffffffu;
    if (sum.significand == 0) return bits != 0;
    const bool normal = (bits >> 23) != 0;
    const i32 exponent = normal ? i32(bits >> 23) - 126 : -125;
    const u64 significand = (normal ? u64((bits & 0x7fffffu) | 0x800000u) : u64(bits)) << 40;
    if (sum.exponent != exponent) return sum.exponent > exponent;
    return significand < sum.significand;
}

}
