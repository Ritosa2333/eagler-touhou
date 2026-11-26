#pragma once
#include "core/stages/ExBossHeavyConstants.hpp"
#include "core/stages/Stage6BossTimelineArithmetic.hpp"
#include "core/stages/Stage5BossFinalArithmetic.hpp"
namespace kaikeidou { namespace ex_boss_heavy_detail {
using Bullet=stage6_boss_timeline_detail::BulletView;
using Number=binary_float::Number;
using stage6_boss_timeline_detail::signed_word;
using stage6_boss_timeline_detail::remainder;
using stage5_boss_final_detail::compare;
struct Arithmetic:stage6_boss_timeline_detail::Arithmetic{Number c(std::uint32_t address)const{return q(constant(address));}};
}
inline binary_float::Number ex_boss_heavy_radians(std::uint64_t angle,unsigned bits){using namespace binary_float;using ex_boss_heavy_detail::constant;return divide(multiply(binary64(angle),binary64(constant(0x6d1860)),bits),binary64(constant(0x6d1858)),bits);}
inline binary_float::Number ex_boss_heavy_sqrt(std::uint64_t value,unsigned bits){return finite_square_root(value,bits);}
inline std::uint64_t ex_boss_heavy_absolute(std::uint64_t value){return value&0x7fffffffffffffffull;}
inline std::uint64_t ex_boss_heavy_absolute_direct(std::uint64_t value){return value&0x7fffffffffffffffull;}
inline std::uint32_t ex_boss_heavy_integer(binary_float::Number value){return stage5_boss_final_color_integer(value,true);}
inline void ex_boss_heavy_ease(ex_boss_heavy_detail::Bullet b,std::size_t offset,std::uint64_t target,std::uint64_t divisor,unsigned bits){b.quad(offset,ease_option_value(b.quad(offset),target,divisor,bits));}
inline void ex_boss_heavy_recolor(ProjectilePools& pool,std::uint32_t slot,std::uint32_t red,std::uint32_t green,std::uint32_t blue){ex_boss_heavy_detail::Bullet b{pool,slot*240u};b.word(24,red);b.word(28,green);b.word(32,blue);}
}
