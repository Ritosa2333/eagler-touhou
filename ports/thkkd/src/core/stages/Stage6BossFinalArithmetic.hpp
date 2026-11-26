#pragma once
#include "core/stages/Stage6BossFinalConstants.hpp"
#include "core/stages/Stage5BossFinalArithmetic.hpp"
#include "core/shared/FinalProjectileRecord.hpp"
#include "core/entities/ProjectilePattern.hpp"
#include "core/math/FiniteSqrt.hpp"
namespace kaikeidou {
inline std::uint64_t stage6_boss_final_absolute(std::uint64_t value){return ((value>>52)&2047u)==2047u&&(value&0xfffffffffffffull)?value:value&0x7fffffffffffffffull;}
inline binary_float::Number stage6_boss_final_sqrt(std::uint64_t value,unsigned precision){return finite_square_root(value,precision);}
inline std::uint32_t stage6_boss_final_convert(binary_float::Number value){return stage5_boss_final_color_integer(value,true);}
namespace stage6_boss_final_detail {
using stage5_boss_final_detail::signed_word;using stage5_boss_final_detail::remainder;using stage5_boss_final_detail::compare;
struct Arithmetic:options_motion_detail::Arithmetic{binary_float::Number c(std::uint32_t address)const{return q(constant(address));}};
inline std::uint32_t canonical(std::uint32_t index){const auto low=index&0x0fffffffu;if(low<3000)return low;if(low==0x0fffffffu)return 0xffffffffu;throw std::out_of_range("Stage6 final projectile effective address outside [-1,2999]");}
inline final_projectile_detail::Record record(ProjectilePools& pool,std::uint32_t index){return {pool,canonical(index)};}
}
template<class Record> inline void stage6_boss_final_approach(Record& record,std::size_t offset,std::uint64_t target,std::uint64_t divisor,unsigned precision){
 if(!(divisor&0x7fffffffffffffffull)){const options_motion_detail::Arithmetic m{precision};const auto delta=m.sub(m.q(target),m.q(record.quad(offset)));record.quad(offset,delta.mantissa?((std::uint64_t(delta.negative)^(divisor>>63))<<63)|0x7ff0000000000000ull:0xfff8000000000000ull);return;}
 record.quad(offset,ease_option_value(record.quad(offset),target,divisor,precision));
}
inline void stage6_boss_final_remove(ProjectilePools& pool,std::uint32_t index,std::uint32_t mode){auto b=stage6_boss_final_detail::record(pool,index);if(stage6_boss_final_detail::signed_word(mode)>99||!b.word(0x34)){if(mode%100u==0&&(b.word(0)==1||b.word(0)==2)){b.word(0,3);b.word(0x10,0);b.word(8,0);if(!b.word(0x2c))b.word(0x2c,1);}}}
inline void stage6_boss_final_recolor(ProjectilePools& pool,std::uint32_t index,std::uint32_t red,std::uint32_t green,std::uint32_t blue){auto b=stage6_boss_final_detail::record(pool,index);b.word(0x18,red);b.word(0x1c,green);b.word(0x20,blue);}
}
