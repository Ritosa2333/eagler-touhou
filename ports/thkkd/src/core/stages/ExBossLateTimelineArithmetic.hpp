#pragma once
#include "core/stages/ExBossLateTimelineConstants.hpp"
#include "core/stages/Stage6BossTimelineArithmetic.hpp"
#include "core/stages/Stage5BossFinalArithmetic.hpp"
#include "core/stages/ExBossLateTimelineEnemyView.hpp"
namespace kaikeidou {
inline void ex_boss_late_timeline_name(std::array<std::uint8_t,64>& target,std::uint32_t address){
    const auto* source=ex_boss_late_timeline_detail::spell_name(address);std::size_t i=0;
    do{target[i]=static_cast<std::uint8_t>(source[i]);}while(source[i++]);
}
inline void ex_boss_late_timeline_ease(std::uint64_t& value,std::uint64_t target,std::uint64_t divisor,unsigned bits){value=ease_option_value(value,target,divisor,bits);}
inline binary_float::Number ex_boss_late_timeline_sqrt(std::uint64_t value,unsigned bits){return finite_square_root(value,bits);}
inline std::uint32_t ex_boss_late_timeline_convert(binary_float::Number value){return stage5_boss_final_color_integer(value,true);}
inline std::uint64_t ex_boss_late_timeline_absolute(std::uint64_t value){return value&0x7fffffffffffffffull;}
inline void ex_boss_late_timeline_enemy_ease(ExBossLateTimelineEnemyView enemy,std::size_t offset,std::uint64_t target,std::uint64_t divisor,unsigned bits){enemy.quad(offset,ease_option_value(enemy.quad(offset),target,divisor,bits));}
inline binary_float::Number ex_boss_late_timeline_degrees(std::uint64_t angle,unsigned bits){using namespace binary_float;return divide(multiply(binary64(angle),integer(180),bits),binary64(0x400921fb54442d28ull),bits);}
namespace ex_boss_late_timeline_detail {
using Number=binary_float::Number;
using stage5_boss_final_detail::compare;
using stage5_boss_final_detail::signed_word;
using stage5_boss_final_detail::remainder;
using Bullet=stage6_boss_timeline_detail::BulletView;
struct Arithmetic:options_motion_detail::Arithmetic{Number c(std::uint32_t address)const{return q(constant(address));}};
}
}
