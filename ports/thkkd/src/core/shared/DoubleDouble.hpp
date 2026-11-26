// 使用两个 binary64 分量保存有限数，为移植扩展精度超越函数提供额外有效位。
#pragma once
#include "core/math/CrtSse2Trig.hpp"

namespace kaikeidou::wide_float {
struct DoubleDouble { double high = 0, low = 0; };
inline double quotient(double left, double right) { volatile double result = left / right; return result; }
inline DoubleDouble quick_sum(double left, double right) {
    const auto sum = crt_sse2::add(left, right);
    return {sum, crt_sse2::sub(right, crt_sse2::sub(sum, left))};
}
inline DoubleDouble two_sum(double left, double right) {
    const auto sum = crt_sse2::add(left, right);
    const auto split = crt_sse2::sub(sum, left);
    return {sum, crt_sse2::add(crt_sse2::sub(left, crt_sse2::sub(sum, split)), crt_sse2::sub(right, split))};
}
inline DoubleDouble add(DoubleDouble left, DoubleDouble right) {
    auto upper = two_sum(left.high, right.high);
    const auto lower = two_sum(left.low, right.low);
    upper.low = crt_sse2::add(upper.low, lower.high);
    upper = quick_sum(upper.high, upper.low);
    upper.low = crt_sse2::add(upper.low, lower.low);
    return quick_sum(upper.high, upper.low);
}
inline DoubleDouble sub(DoubleDouble left, DoubleDouble right) { return add(left, {-right.high, -right.low}); }
inline DoubleDouble product(double left, double right) {
    const auto result = crt_sse2::mul(left, right);
    const auto left_split = crt_sse2::mul(left, 134217729.);
    const auto right_split = crt_sse2::mul(right, 134217729.);
    const auto left_high = crt_sse2::sub(left_split, crt_sse2::sub(left_split, left));
    const auto right_high = crt_sse2::sub(right_split, crt_sse2::sub(right_split, right));
    const auto left_low = crt_sse2::sub(left, left_high);
    const auto right_low = crt_sse2::sub(right, right_high);
    const auto first = crt_sse2::sub(crt_sse2::mul(left_high, right_high), result);
    const auto second = crt_sse2::add(first, crt_sse2::mul(left_high, right_low));
    const auto third = crt_sse2::add(second, crt_sse2::mul(left_low, right_high));
    return {result, crt_sse2::add(third, crt_sse2::mul(left_low, right_low))};
}
inline DoubleDouble mul(DoubleDouble left, DoubleDouble right) {
    auto result = product(left.high, right.high);
    const auto cross = crt_sse2::add(crt_sse2::mul(left.high, right.low), crt_sse2::mul(left.low, right.high));
    result = quick_sum(result.high, crt_sse2::add(result.low, cross));
    result.low = crt_sse2::add(result.low, crt_sse2::mul(left.low, right.low));
    return quick_sum(result.high, result.low);
}
inline DoubleDouble divide(DoubleDouble numerator, DoubleDouble denominator) {
    const auto first = quotient(numerator.high, denominator.high);
    const auto remainder = sub(numerator, mul(denominator, {first, 0}));
    const auto second = quotient(remainder.high, denominator.high);
    const auto tail = sub(remainder, mul(denominator, {second, 0}));
    const auto third = quotient(tail.high, denominator.high);
    return add(quick_sum(first, second), {third, 0});
}
}
