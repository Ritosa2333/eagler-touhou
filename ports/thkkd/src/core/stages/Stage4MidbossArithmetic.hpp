#pragma once
#include "core/stages/Stage4MidbossConstants.hpp"
#include "core/player/OptionsMotion.hpp"
#include "core/entities/ProjectilePattern.hpp"
namespace kaikeidou {
inline std::uint32_t stage4_midboss_pick_integer(std::uint32_t d,const std::array<std::uint32_t,6>& v){return v[d<6?d:3];}
inline std::uint32_t stage4_midboss_pick_character(std::uint32_t character,const std::array<std::uint32_t,3>& v){return v[character<3?character:0];}
inline std::uint64_t stage4_midboss_approach(std::uint64_t value,std::uint64_t target,std::uint64_t divisor,unsigned precision){return ease_option_value(value,target,divisor,precision);}
namespace stage4_midboss_detail {
inline std::int64_t signed_word(std::uint32_t v){return v>>31?std::int64_t(v)-0x100000000ll:v;}
inline std::int64_t remainder(std::uint32_t a,std::uint32_t b){if(!b)throw std::domain_error("Stage4 midboss zero divisor");if(a==0x80000000u&&b==0xffffffffu)throw std::domain_error("Stage4 midboss quotient overflow");return signed_word(a)%signed_word(b);}
struct Arithmetic:options_motion_detail::Arithmetic{binary_float::Number c(std::uint32_t a)const{return q(constant(a));}};
}
}
