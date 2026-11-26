// 中Boss局部数值入口，便于分别观察原选择器、IDIV及数学调用。
#pragma once
#include "core/stages/Stage2MidbossConstants.hpp"
#include "core/math/FiniteDivide.hpp"
#include <array>
namespace kaikeidou {
inline std::uint32_t midboss_pick_integer(std::uint32_t difficulty,const std::array<std::uint32_t,6>& values){return values[difficulty<6?difficulty:3];}
inline std::uint64_t midboss_pick_real(std::uint32_t difficulty,const std::array<std::uint64_t,6>& values){return values[difficulty<6?difficulty:3];}
namespace stage2_midboss_detail {
inline std::int64_t signed_word(std::uint32_t value){return value>>31?std::int64_t(value)-0x100000000ll:value;}
inline binary_float::Number subtract(binary_float::Number a,binary_float::Number b,unsigned bits){b.negative=!b.negative;return binary_float::add(a,b,bits);}
inline std::int64_t remainder(std::uint32_t a,std::uint32_t b){
    if(b==0)throw std::domain_error("Original enemy29 IDIV zero divisor");
    if(a==0x80000000u&&b==0xffffffffu)throw std::domain_error("Original enemy29 IDIV quotient overflow");
    return signed_word(a)%signed_word(b);
}
}
}
