#pragma once
#include "core/stages/ExBossLateOtherConstants.hpp"
#include "core/stages/Stage6BossTimelineArithmetic.hpp"
#include "core/stages/Stage5BossFinalArithmetic.hpp"
namespace kaikeidou { namespace ex_boss_late_other_detail {
using Bullet=stage6_boss_timeline_detail::BulletView;using Number=binary_float::Number;
using stage6_boss_timeline_detail::signed_word;using stage6_boss_timeline_detail::remainder;using stage5_boss_final_detail::compare;
struct Arithmetic:stage6_boss_timeline_detail::Arithmetic{Number c(std::uint32_t address)const{return q(constant(address));}};
}
inline void ex_boss_late_other_ease(ex_boss_late_other_detail::Bullet b,std::size_t offset,std::uint64_t target,std::uint64_t divisor,unsigned bits){b.quad(offset,ease_option_value(b.quad(offset),target,divisor,bits));}
inline std::int32_t ex_boss_late_other_remove(ProjectilePools& pool,std::uint32_t index,std::int32_t mode){ex_boss_late_other_detail::Bullet b{pool,index*240u};if(mode>99||!b.word(0x34)){if(b.word(0)==1||b.word(0)==2){b.word(0,3);b.word(16,0);b.word(8,0);if(!b.word(44))b.word(44,1);}}return static_cast<std::int32_t>(index*240u);}
}
