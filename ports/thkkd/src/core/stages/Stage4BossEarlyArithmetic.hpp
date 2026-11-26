#pragma once
#include "core/stages/Stage4BossEarlyConstants.hpp"
#include "core/player/OptionsMotion.hpp"
#include "core/entities/ProjectilePattern.hpp"
#include "core/stages/StagePatternContext.hpp"
#include "core/stages/Stage4PatternContext.hpp"
namespace kaikeidou {
inline std::uint32_t stage4_boss_early_pick_integer(std::uint32_t d,const std::array<std::uint32_t,6>& v){return v[d<6?d:3];}
inline std::uint32_t stage4_boss_early_pick_character(std::uint32_t character,const std::array<std::uint32_t,3>& v){return v[character<3?character:0];}
inline std::uint64_t stage4_boss_early_approach(std::uint64_t value,std::uint64_t target,std::uint64_t divisor,unsigned precision){
 if(!(divisor&0x7fffffffffffffffull)){const options_motion_detail::Arithmetic m{precision};const auto n=m.sub(m.q(target),m.q(value));return n.mantissa?((std::uint64_t(n.negative)^(divisor>>63))<<63)|0x7ff0000000000000ull:0xfff8000000000000ull;}
 return ease_option_value(value,target,divisor,precision);
}
namespace stage4_boss_early_detail {
inline std::int64_t signed_word(std::uint32_t v){return v>>31?std::int64_t(v)-0x100000000ll:v;}
inline std::int64_t remainder(std::uint32_t a,std::uint32_t b){if(!b)throw std::domain_error("Stage4 boss early zero divisor");if(a==0x80000000u&&b==0xffffffffu)throw std::domain_error("Stage4 boss early quotient overflow");return signed_word(a)%signed_word(b);}
struct Arithmetic:options_motion_detail::Arithmetic{binary_float::Number c(std::uint32_t a)const{return q(constant(a));}};
inline std::uint32_t color_component(const PatternParameters& p,const StagePatternContext& extra,const Stage4PatternContext& fourth,const std::uint32_t& integer5,std::uint32_t group,std::uint32_t component){
 const std::uint32_t address=0xa78580u+group*40u+component*4u;const auto offset=address-0xa78558u;
 if(offset>=160u||(offset&3u))throw std::out_of_range("Stage4 early color address outside integers0..39");const auto index=offset/4;
 if(index<5)return p.integers[index];if(index==5)return integer5;if(index<20)return extra.integers_6_to_19[index-6];return fourth.integers_20_to_39[index-20];
}
inline std::array<std::uint32_t,3> color(const PatternParameters& p,const StagePatternContext& extra,const Stage4PatternContext& fourth,const std::uint32_t& integer5,std::uint32_t group){
 const auto blue=color_component(p,extra,fourth,integer5,group,2),green=color_component(p,extra,fourth,integer5,group,1),red=color_component(p,extra,fourth,integer5,group,0);return {{red,green,blue}};
}
}
}
