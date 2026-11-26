// 三面敌47..50使用的有符号整数余数、难度选择器与显式精度减法。
#pragma once
#include "core/stages/Stage3EnemyLateConstants.hpp"
#include "core/math/FiniteDivide.hpp"
#include "core/player/OptionsMotion.hpp"
#include <array>
namespace kaikeidou {
inline std::uint32_t stage3_enemy_late_pick_integer(std::uint32_t difficulty,const std::array<std::uint32_t,6>& values){return values[difficulty<6?difficulty:3];}
inline std::uint64_t stage3_enemy_late_pick_real(std::uint32_t difficulty,const std::array<std::uint64_t,6>& values){return values[difficulty<6?difficulty:3];}
namespace stage3_enemy_late_detail {
inline std::int64_t signed_word(std::uint32_t value){return value>>31?std::int64_t(value)-0x100000000ll:value;}
inline binary_float::Number subtract(binary_float::Number a,binary_float::Number b,unsigned precision){b.negative=!b.negative;return binary_float::add(a,b,precision);}
inline std::int64_t remainder(std::uint32_t numerator,std::uint32_t denominator){
    if(!denominator)throw std::domain_error("Original stage3 enemy IDIV zero divisor");
    if(numerator==0x80000000u&&denominator==0xffffffffu)throw std::domain_error("Original stage3 enemy IDIV quotient overflow");
    return signed_word(numerator)%signed_word(denominator);
}
}
}
