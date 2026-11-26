// 三面敌43至46的原有符号除法、精度落点及难度选择。
#pragma once
#include "core/stages/Stage3EnemyEarlyConstants.hpp"
#include "core/math/FiniteDivide.hpp"
#include <array>
#include <stdexcept>
namespace kaikeidou {
inline std::uint32_t stage3_enemy_early_pick_integer(std::uint32_t d,const std::array<std::uint32_t,6>& v){return v[d<6?d:3];}
inline std::uint64_t stage3_enemy_early_pick_real(std::uint32_t d,const std::array<std::uint64_t,6>& v){return v[d<6?d:3];}
namespace stage3_enemy_early_detail {
inline std::int64_t signed_word(std::uint32_t v){return v>>31?std::int64_t(v)-0x100000000ll:v;}
inline std::int64_t remainder(std::uint32_t numerator,std::uint32_t denominator){
 const auto a=signed_word(numerator),b=signed_word(denominator);
 if(!b)throw std::domain_error("Stage3 enemy zero divisor");
 if(a==-2147483648ll&&b==-1)throw std::domain_error("Stage3 enemy quotient overflow");
 return a%b;
}
inline binary_float::Number subtract(binary_float::Number a,binary_float::Number b,unsigned bits){b.negative=!b.negative;return binary_float::add(a,b,bits);}
}
}
