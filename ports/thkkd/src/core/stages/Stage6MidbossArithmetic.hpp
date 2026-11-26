#pragma once
#include "core/stages/Stage6MidbossConstants.hpp"
#include "core/player/OptionsMotion.hpp"
#include "core/entities/ProjectilePattern.hpp"
#include "core/math/FiniteSqrt.hpp"
#include "core/stages/Stage5BossFinalArithmetic.hpp"
namespace kaikeidou {
inline std::uint64_t stage6_midboss_approach(std::uint64_t value,std::uint64_t target,std::uint64_t divisor,unsigned precision){
 if(!(divisor&0x7fffffffffffffffull)){const options_motion_detail::Arithmetic m{precision};const auto n=m.sub(m.q(target),m.q(value));return n.mantissa?((std::uint64_t(n.negative)^(divisor>>63))<<63)|0x7ff0000000000000ull:0xfff8000000000000ull;}
 return ease_option_value(value,target,divisor,precision);
}
template<std::size_t N> inline void stage6_midboss_approach_record(ObjectRecord<N>& object,std::size_t offset,std::uint64_t target,std::uint64_t divisor,unsigned precision){object.quad(offset,stage6_midboss_approach(object.quad(offset),target,divisor,precision));}
inline binary_float::Number stage6_midboss_sqrt(std::uint64_t square,unsigned precision){return finite_square_root(square,precision);}
inline std::uint32_t stage6_midboss_convert(binary_float::Number v){return stage5_boss_final_color_integer(v,true);}
inline std::uint64_t stage6_midboss_absolute(std::uint64_t v){return v&0x7fffffffffffffffull;}
inline std::uint32_t stage6_midboss_pick_integer(const std::uint32_t& d,const std::array<std::uint32_t,6>& v){return v[d<6?d:3];}
inline std::uint64_t stage6_midboss_pick_real(const std::uint32_t& d,const std::array<std::uint64_t,6>& v){return v[d<6?d:3];}
inline std::uint64_t stage6_midboss_pick_character(const std::uint32_t& c,const std::array<std::uint64_t,3>& v){return v[c<3?c:0];}
inline void stage6_midboss_name(std::array<std::uint8_t,64>& destination,std::uint32_t address){const auto* s=stage6_midboss_detail::spell_name(address);std::size_t i=0;do{destination[i]=static_cast<std::uint8_t>(s[i]);}while(s[i++]);}
namespace stage6_midboss_detail {
inline std::int64_t signed_word(std::uint32_t v){return v>>31?std::int64_t(v)-0x100000000ll:v;}
inline std::int64_t remainder(std::uint32_t a,std::uint32_t b){if(!b)throw std::domain_error("Stage6 midboss zero divisor");if(a==0x80000000u&&b==0xffffffffu)throw std::domain_error("Stage6 midboss quotient overflow");return signed_word(a)%signed_word(b);}
struct Arithmetic:options_motion_detail::Arithmetic{binary_float::Number c(std::uint32_t address)const{return q(constant(address));}};
}
}
