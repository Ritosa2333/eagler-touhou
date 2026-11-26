// 四面关底晚期的原地址映射及精度落点，动态EA先按32位回绕再访问已映射槽。
#pragma once
#include "core/stages/Stage4BossFinalConstants.hpp"
#include "core/stages/Stage3BossTimelineContext.hpp"
#include "core/stages/StagePatternContext.hpp"
#include "core/stages/Stage4PatternContext.hpp"
#include "core/entities/BossStageContext.hpp"
#include "core/entities/ProjectilePattern.hpp"
#include "core/player/OptionsMotion.hpp"
#include "core/math/FiniteSqrt.hpp"
#include "core/shared/FinalProjectileRecord.hpp"
#include <stdexcept>
namespace kaikeidou {
inline std::uint64_t stage4_boss_final_approach(std::uint64_t value,std::uint64_t target,std::uint64_t divisor,unsigned precision){return ease_option_value(value,target,divisor,precision);}
inline binary_float::Number stage4_boss_final_sqrt(std::uint64_t value,unsigned precision){return finite_square_root(value,precision);}
inline std::int32_t stage4_boss_final_remove100(ProjectilePools& pool,std::uint32_t index){
    if(index!=0xffffffffu)return remove_projectile(pool,index,100);
    final_projectile_detail::Record b{pool,index};
    if(b.word(0)==1||b.word(0)==2){b.word(0,3);b.word(0x10,0);b.word(8,0);if(!b.word(0x2c))b.word(0x2c,1);}
    return -240;
}
namespace stage4_boss_final_detail {
inline std::int64_t signed_word(std::uint32_t value){return value>>31?std::int64_t(value)-0x100000000ll:value;}
inline std::int64_t remainder(std::uint32_t a,std::uint32_t b){if(!b)throw std::domain_error("Stage4 final zero divisor");if(a==0x80000000u&&b==0xffffffffu)throw std::domain_error("Stage4 final quotient overflow");return signed_word(a)%signed_word(b);}
struct Arithmetic:options_motion_detail::Arithmetic{binary_float::Number c(std::uint32_t a)const{return q(constant(a));}};
inline int compare(binary_float::Number a,binary_float::Number b){
    if(!a.mantissa)a.negative=false;if(!b.mantissa)b.negative=false;
    if(a.negative!=b.negative)return a.negative?-1:1;
    int order=0;if(!a.mantissa||!b.mantissa)order=(a.mantissa!=0)-(b.mantissa!=0);
    else if(a.exponent!=b.exponent)order=a.exponent<b.exponent?-1:1;else if(a.mantissa!=b.mantissa)order=a.mantissa<b.mantissa?-1:1;
    return a.negative?-order:order;
}
inline std::uint32_t integer_address(std::uint32_t address,const PatternParameters& p,const StagePatternContext& extra,const Stage4PatternContext& stage4,const std::uint32_t& integer5){
    if(address<0xa78558u||address>=0xa785f8u||(address-0xa78558u)%4)throw std::out_of_range("Stage4 final integer EA outside mapped0..39");
    const auto index=(address-0xa78558u)/4;
    if(index<5)return p.integers[index];if(index==5)return integer5;if(index<20)return extra.integers_6_to_19[index-6];return stage4.integers_20_to_39[index-20];
}
inline std::uint64_t real_address(std::uint32_t address,const PatternParameters& p,const BossStageContext& boss,const StagePatternContext& extra,const Stage3BossTimelineContext& final){
    if(address<0xa87dc0u||address>=0xa87eb0u||(address-0xa87dc0u)%8)throw std::out_of_range("Stage4 final real EA outside mapped0..29");
    const auto index=(address-0xa87dc0u)/8;
    if(index<2)return p.reals[index];if(index<4)return boss.extra_reals[index-2];if(index==4)return extra.real_4;
    if(index<10)return final.reals_5_to_9[index-5];if(index<20)return extra.reals_10_to_19[index-10];return final.reals_20_to_29[index-20];
}
}
}
