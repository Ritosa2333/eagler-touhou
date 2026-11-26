// 三面中Boss局部整数语义和原x87运算落点。
#pragma once
#include "core/stages/Stage3MidbossDependenciesConstants.hpp"
#include "core/math/FiniteDivide.hpp"
#include <stdexcept>
namespace kaikeidou { namespace stage3_midboss_dependencies_detail {
inline std::int64_t signed_word(std::uint32_t v){return v>>31?std::int64_t(v)-0x100000000ll:v;}
inline std::int64_t remainder(std::uint32_t a,std::uint32_t b){if(!b)throw std::domain_error("Stage3 midboss zero divisor");if(a==0x80000000u&&b==0xffffffffu)throw std::domain_error("Stage3 midboss quotient overflow");return signed_word(a)%signed_word(b);}
inline binary_float::Number subtract(binary_float::Number a,binary_float::Number b,unsigned bits){b.negative=!b.negative;return binary_float::add(a,b,bits);}
}}
