#pragma once
#include "core/stages/Stage5BossEarlyConstants.hpp"
#include "core/player/OptionsMotion.hpp"
#include "core/entities/ProjectilePattern.hpp"
#include "core/math/FiniteSqrt.hpp"
namespace kaikeidou {
inline std::uint64_t stage5_boss_early_approach(std::uint64_t value,std::uint64_t target,std::uint64_t divisor,unsigned precision){
 if(!(divisor&0x7fffffffffffffffull)){const options_motion_detail::Arithmetic m{precision};const auto n=m.sub(m.q(target),m.q(value));return n.mantissa?((std::uint64_t(n.negative)^(divisor>>63))<<63)|0x7ff0000000000000ull:0xfff8000000000000ull;}
 return ease_option_value(value,target,divisor,precision);
}
inline binary_float::Number stage5_boss_early_sqrt(std::uint64_t square,unsigned precision){return finite_square_root(square,precision);}
namespace stage5_boss_early_detail {
inline std::int64_t signed_word(std::uint32_t v){return v>>31?std::int64_t(v)-0x100000000ll:v;}
inline std::int64_t remainder(std::uint32_t a,std::uint32_t b){if(!b)throw std::domain_error("Stage5 early zero divisor");if(a==0x80000000u&&b==0xffffffffu)throw std::domain_error("Stage5 early quotient overflow");return signed_word(a)%signed_word(b);}
struct Arithmetic:options_motion_detail::Arithmetic{binary_float::Number c(std::uint32_t address)const{return q(constant(address));}};
}
}
