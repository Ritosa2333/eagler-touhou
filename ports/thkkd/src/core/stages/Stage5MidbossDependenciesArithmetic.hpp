#pragma once
#include "core/stages/Stage5MidbossDependenciesConstants.hpp"
#include "core/player/OptionsMotion.hpp"
#include "core/entities/ProjectilePattern.hpp"
#include "core/shared/FinalProjectileRecord.hpp"
namespace kaikeidou {
inline std::uint64_t stage5_midboss_dependencies_approach(std::uint64_t current,std::uint64_t target,std::uint64_t divisor,unsigned precision){return ease_option_value(current,target,divisor,precision);}
inline void stage5_midboss_dependencies_retire(ProjectilePools& pool,std::uint32_t index){
 final_projectile_detail::Record b{pool,index};if(b.word(0)==1||b.word(0)==2){b.word(0,3);b.word(0x10,0);b.word(8,0);if(!b.word(0x2c))b.word(0x2c,1);}
}
namespace stage5_midboss_dependencies_detail {
inline std::int64_t signed_word(std::uint32_t v){return v>>31?std::int64_t(v)-0x100000000ll:v;}
inline std::int64_t remainder(std::uint32_t a,std::uint32_t b){if(!b)throw std::domain_error("Stage5 midboss zero divisor");if(a==0x80000000u&&b==0xffffffffu)throw std::domain_error("Stage5 midboss quotient overflow");return signed_word(a)%signed_word(b);}
struct Arithmetic:options_motion_detail::Arithmetic{
 binary_float::Number c(std::uint32_t a)const{return q(constant(a));}
 binary_float::Number signed_integer(std::uint32_t v)const{auto n=i(v>>31?0u-v:v);n.negative=v>>31;return n;}
};
}
}
