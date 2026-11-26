// 三面关底早段局部整数语义和原x87运算落点。
#pragma once
#include "core/stages/Stage3BossEarlyConstants.hpp"
#include "core/math/FiniteDivide.hpp"
#include <stdexcept>
#include <array>
namespace kaikeidou { namespace stage3_boss_early_detail {
inline std::int64_t signed_word(std::uint32_t v){return v>>31?std::int64_t(v)-0x100000000ll:v;}
inline std::int64_t remainder(std::uint32_t a,std::uint32_t b){if(!b)throw std::domain_error("Stage3 boss early zero divisor");if(a==0x80000000u&&b==0xffffffffu)throw std::domain_error("Stage3 boss early quotient overflow");return signed_word(a)%signed_word(b);}
inline binary_float::Number subtract(binary_float::Number a,binary_float::Number b,unsigned bits){b.negative=!b.negative;return binary_float::add(a,b,bits);}
}}

namespace kaikeidou {inline std::uint32_t stage3_boss_early_pick(std::uint32_t difficulty,const std::array<std::uint32_t,6>& values){return values[difficulty<6?difficulty:3];}}
