#pragma once
#include "core/stages/Stage5MidbossTimelineConstants.hpp"
#include "core/player/OptionsMotion.hpp"
#include "core/entities/ProjectilePattern.hpp"
namespace kaikeidou {
inline std::uint32_t stage5_midboss_timeline_pick_integer(const std::uint32_t& difficulty,const std::array<std::uint32_t,6>& values){return values[difficulty<6?difficulty:3];}
inline std::uint64_t stage5_midboss_timeline_pick_real(const std::uint32_t& difficulty,const std::array<std::uint64_t,6>& values){return values[difficulty<6?difficulty:3];}
inline std::uint64_t stage5_midboss_timeline_approach(std::uint64_t value,std::uint64_t target,std::uint64_t divisor,unsigned precision){return ease_option_value(value,target,divisor,precision);}
namespace stage5_midboss_timeline_detail {
inline std::int64_t signed_word(std::uint32_t value){return value>>31?std::int64_t(value)-0x100000000ll:value;}
struct Arithmetic:options_motion_detail::Arithmetic{binary_float::Number c(std::uint32_t address)const{return q(constant(address));}};
}
}
