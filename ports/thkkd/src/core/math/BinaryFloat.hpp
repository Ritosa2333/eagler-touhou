// 为原 x87 数值路径恢复的有限数运算，使用整数实现指定精度的最近偶数舍入。
#pragma once
#include <cstdint>
#include <stdexcept>
#include <utility>

namespace kaikeidou {
namespace binary_float {
// 有限数以 64 位有效数保存，最高位对应 2^exponent，独立于宿主浮点运算。
struct Number {
    std::uint64_t mantissa = 0;
    int exponent = 0;
    bool negative = false;
};
struct Wide { std::uint64_t high, low; };
inline Number rounded(Number number, std::uint64_t tail, unsigned precision) {
    if (!number.mantissa) return number;
    const auto drop = 64 - precision;
    bool increment;
    std::uint64_t kept;
    if (drop) {
        kept = number.mantissa >> drop;
        const auto lost = number.mantissa & ((std::uint64_t(1) << drop) - 1);
        const auto half = std::uint64_t(1) << (drop - 1);
        increment = lost > half || (lost == half && (tail || (kept & 1)));
    } else {
        kept = number.mantissa;
        increment = tail > (std::uint64_t(1) << 63) ||
                    (tail == (std::uint64_t(1) << 63) && (kept & 1));
    }
    if (increment && ++kept == 0) {
        number.mantissa = std::uint64_t(1) << 63;
        ++number.exponent;
    } else if (drop && kept == (std::uint64_t(1) << precision)) {
        number.mantissa = std::uint64_t(1) << 63;
        ++number.exponent;
    } else number.mantissa = kept << drop;
    return number;
}
inline Number integer(std::uint32_t bits) {
    Number number;
    number.negative = (bits >> 31) != 0;
    auto magnitude = number.negative ? std::uint64_t(0u - bits) : std::uint64_t(bits);
    if (!magnitude) return number;
    number.exponent = 63;
    while (!(magnitude >> 63)) { magnitude <<= 1; --number.exponent; }
    number.mantissa = magnitude;
    return number;
}
inline Number binary64(std::uint64_t bits) {
    const auto field = static_cast<unsigned>((bits >> 52) & 2047);
    if (field == 2047) throw std::domain_error("Random argument must be finite");
    Number number{(bits & 0xfffffffffffffull) << 11, static_cast<int>(field) - 1023, (bits >> 63) != 0};
    if (field) number.mantissa |= std::uint64_t(1) << 63;
    else if (number.mantissa) {
        number.exponent = -1022;
        while (!(number.mantissa >> 63)) { number.mantissa <<= 1; --number.exponent; }
    }
    return number;
}
inline Wide multiply_words(std::uint64_t left, std::uint64_t right) {
    constexpr auto mask = std::uint64_t(0xffffffffu);
    auto product = (left & mask) * (right & mask);
    const auto bottom = product & mask;
    const auto carry = product >> 32;
    product = (left >> 32) * (right & mask) + carry;
    const auto middle = product & mask;
    const auto top = product >> 32;
    product = (left & mask) * (right >> 32) + middle;
    return {(left >> 32) * (right >> 32) + top + (product >> 32), (product << 32) + bottom};
}
inline Number multiply(Number left, Number right, unsigned precision) {
    auto result = multiply_words(left.mantissa, right.mantissa);
    Number number{result.high, left.exponent + right.exponent + 1, left.negative != right.negative};
    if (!left.mantissa || !right.mantissa) return {0, 0, number.negative};
    if (!(result.high >> 63)) {
        number.mantissa = (result.high << 1) | (result.low >> 63);
        result.low <<= 1;
        --number.exponent;
    }
    return rounded(number, result.low, precision);
}
inline Number fraction(std::uint32_t value, unsigned precision) {
    const auto numerator = integer(value), denominator = integer(10000);
    Number number{std::uint64_t(1) << 63, numerator.exponent - denominator.exponent, false};
    auto remainder = numerator.mantissa;
    if (remainder < denominator.mantissa) { remainder <<= 1; --number.exponent; }
    remainder -= denominator.mantissa;
    for (int bit = 62; bit >= 0; --bit) {
        const bool carry = (remainder >> 63) != 0;
        remainder <<= 1;
        if (carry || remainder >= denominator.mantissa) {
            remainder -= denominator.mantissa;
            number.mantissa |= std::uint64_t(1) << bit;
        }
    }
    const bool carry = (remainder >> 63) != 0;
    const auto doubled = remainder << 1;
    const auto half = std::uint64_t(1) << 63;
    const std::uint64_t tail = carry || doubled > denominator.mantissa ? half + 1 :
        (doubled == denominator.mantissa ? half : (remainder ? 1 : 0));
    return rounded(number, tail, precision);
}
inline Wide shift_right(Wide value, unsigned shift) {
    if (!shift) return value;
    if (shift < 64) return {value.high >> shift, (value.high << (64 - shift)) | (value.low >> shift) |
        ((value.low << (64 - shift)) != 0 ? 1ull : 0ull)};
    if (shift == 64) return {0, value.high | (value.low ? 1ull : 0ull)};
    if (shift < 128) return {0, (value.high >> (shift - 64)) |
        (((value.high << (128 - shift)) || value.low) ? 1ull : 0ull)};
    return {0, (value.high || value.low) ? 1ull : 0ull};
}
inline Number add(Number number, Number one, unsigned precision) {
    if (!number.mantissa && !one.mantissa) return {0, 0, number.negative && one.negative};
    if (!number.mantissa) return rounded(one, 0, precision);
    if (!one.mantissa) return rounded(number, 0, precision);
    if (number.exponent < one.exponent || (number.exponent == one.exponent && number.mantissa < one.mantissa))
        std::swap(number, one);
    const auto smaller = shift_right({one.mantissa, 0}, static_cast<unsigned>(number.exponent - one.exponent));
    Wide result{number.mantissa, 0};
    if (number.negative == one.negative) {
        result.low = smaller.low;
        result.high += smaller.high;
        if (result.high < number.mantissa) {
            const auto shifted = shift_right(result, 1);
            result = {shifted.high | (std::uint64_t(1) << 63), shifted.low};
            ++number.exponent;
        }
    } else {
        result.low = 0 - smaller.low;
        result.high -= smaller.high + (smaller.low ? 1u : 0u);
        if (!result.high && !result.low) return {};
        while (!(result.high >> 63)) {
            result.high = (result.high << 1) | (result.low >> 63);
            result.low <<= 1;
            --number.exponent;
        }
    }
    number.mantissa = result.high;
    return rounded(number, result.low, precision);
}
inline std::uint64_t to_binary64(Number number) {
    const auto sign = std::uint64_t(number.negative) << 63;
    if (!number.mantissa) return sign;
    if (number.exponent < -1022) {
        const auto shift = static_cast<unsigned>(-number.exponent - 1011);
        if (shift > 64) return sign;
        if (shift == 64) return sign | (number.mantissa > (std::uint64_t(1) << 63) ? 1ull : 0ull);
        auto kept = number.mantissa >> shift;
        const auto lost = number.mantissa & ((std::uint64_t(1) << shift) - 1);
        const auto half = std::uint64_t(1) << (shift - 1);
        if (lost > half || (lost == half && (kept & 1))) ++kept;
        return sign | kept;
    }
    number = rounded(number, 0, 53);
    if (number.exponent > 1023) return sign | 0x7ff0000000000000ull;
    return sign | (std::uint64_t(number.exponent + 1023) << 52) | ((number.mantissa >> 11) & 0xfffffffffffffull);
}
inline std::uint32_t truncate(Number number) {
    if (!number.mantissa || number.exponent < 0) return 0;
    if (number.exponent > 31) throw std::range_error("Random integer conversion overflow");
    const auto magnitude = static_cast<std::uint32_t>(number.mantissa >> (63 - number.exponent));
    return number.negative ? 0u - magnitude : magnitude;
}
}
}
