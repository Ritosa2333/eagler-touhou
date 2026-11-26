#pragma once
#include "core/stages/ExBossEarlyTimelineConstants.hpp"
#include "core/stages/Stage6BossTimelineArithmetic.hpp"
#include "core/stages/Stage5BossFinalArithmetic.hpp"
namespace kaikeidou {
inline void ex_boss_early_timeline_name(std::array<std::uint8_t,64>& target,std::uint32_t address){
    const auto* source=ex_boss_early_timeline_detail::spell_name(address);std::size_t i=0;
    do{target[i]=static_cast<std::uint8_t>(source[i]);}while(source[i++]);
}
inline void ex_boss_early_timeline_ease(std::uint64_t& value,std::uint64_t target,std::uint64_t divisor,unsigned bits){value=ease_option_value(value,target,divisor,bits);}
inline binary_float::Number ex_boss_early_timeline_sqrt(std::uint64_t value,unsigned bits){return finite_square_root(value,bits);}
inline std::uint32_t ex_boss_early_timeline_convert(binary_float::Number value){return stage5_boss_final_color_integer(value,true);}
namespace ex_boss_early_timeline_detail {
using Number=binary_float::Number;
using stage5_boss_final_detail::compare;
using stage5_boss_final_detail::signed_word;
using stage5_boss_final_detail::remainder;
using Bullet=stage6_boss_timeline_detail::BulletView;
struct Arithmetic:options_motion_detail::Arithmetic{Number c(std::uint32_t address)const{return q(constant(address));}};
}
}
