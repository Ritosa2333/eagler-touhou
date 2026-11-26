// 原67AC7C..67AC94：FMUL、FIDIV、FADD，最后一次落binary64。
#pragma once
#include "core/math/FiniteDivide.hpp"
namespace kaikeidou {
inline std::uint64_t stage2_boss_opening_turn_rate(std::uint64_t numerator,std::uint32_t denominator,unsigned precision) {
    using namespace binary_float;
    constexpr auto exponent=0x7ff0000000000000ull;
    constexpr auto fraction=0x000fffffffffffffull;
    if((numerator&exponent)==exponent) {
        if(numerator&fraction)return numerator|0x0008000000000000ull;
        return numerator^(denominator>>31?0x8000000000000000ull:0ull);
    }
    if(denominator==0) {
        if((numerator&0x7fffffffffffffffull)==0)return 0xfff8000000000000ull;
        return (numerator&0x8000000000000000ull)|exponent;
    }
    const auto product=multiply(binary64(numerator),binary64(0x3ff4cccccccccccdull),precision);
    const auto quotient=divide(product,integer(denominator),precision);
    return to_binary64(add(quotient,binary64(0x3fe0c152382d7370ull),precision));
}
}
