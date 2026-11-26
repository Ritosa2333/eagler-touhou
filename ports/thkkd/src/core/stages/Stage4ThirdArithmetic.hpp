#pragma once
#include "core/stages/Stage4ThirdConstants.hpp"
#include "core/player/OptionsMotion.hpp"
#include "core/entities/ProjectilePattern.hpp"
#include "core/entities/BossStageContext.hpp"
#include "core/shared/FloorToInt32.hpp"
namespace kaikeidou {
inline std::uint32_t stage4_third_pick_integer(std::uint32_t d,const std::array<std::uint32_t,6>& v){return v[d<6?d:3];}
inline std::uint64_t stage4_third_pick_real(std::uint32_t d,const std::array<std::uint64_t,6>& v){return v[d<6?d:3];}
inline std::uint64_t stage4_third_absolute(std::uint64_t v){return v&0x7fffffffffffffffull;}
inline std::uint64_t stage4_third_approach(std::uint64_t value,std::uint64_t target,std::uint64_t divisor,unsigned precision){return ease_option_value(value,target,divisor,precision);}
// 原CVTTSD2SI向零截断复用已验收的位型floor转换，含非法输入的80000000落点。
inline std::uint32_t stage4_third_convert(std::uint64_t v){const auto magnitude=floor_to_int32(v&0x7fffffffffffffffull);return v>>63?0u-magnitude:magnitude;}
namespace stage4_third_detail {
inline std::int64_t signed_word(std::uint32_t v){return v>>31?std::int64_t(v)-0x100000000ll:v;}
inline std::int64_t remainder(std::uint32_t a,std::uint32_t b){if(!b)throw std::domain_error("Stage4 third zero divisor");if(a==0x80000000u&&b==0xffffffffu)throw std::domain_error("Stage4 third quotient overflow");return signed_word(a)%signed_word(b);}
struct Arithmetic:options_motion_detail::Arithmetic{binary_float::Number c(std::uint32_t a)const{return q(constant(a));}};
}
}
