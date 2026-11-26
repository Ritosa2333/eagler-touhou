// 原普通弹79/81/86，保留FILD整数差值回绕和中间binary64存回。
#pragma once
#include "core/stages/Stage5BossFinalArithmetic.hpp"
#include "core/stages/Stage5BossFinalRecords.hpp"
#include "core/math/GameMath.hpp"
namespace kaikeidou {
inline void update_stage5_boss_final_projectile(ProjectilePools& pool,std::uint32_t index,std::uint32_t behavior,GameMath& math,RandomPrecision precision){
 using namespace stage5_boss_final_detail;const Arithmetic m{static_cast<unsigned>(precision)};PoolRecord b{pool,0xa8e660u+index*240u};
 const auto q=[&](std::size_t off){return m.q(b.quad(off));};const auto put=[&](std::size_t off,binary_float::Number value){b.quad(off,m.store(value));};
 const auto value=[&](std::size_t off){return crt_sse2::real(b.quad(off));};
 if(behavior==79){const auto sine=math.sine(b.quad(0xa0));put(0x40,m.add(m.mul(sine,m.c(0x6d1878)),q(0xa8)));put(0x48,m.add(q(0x48),m.c(0x6d1868)));put(0xa0,m.add(q(0xa0),m.c(0x6d1be8)));if(value(0x48)>456.)b.word(0,0);return;}
 if(behavior==81){
  if(signed_word(b.word(12))>=signed_word(b.word(0x7c))){put(0x40,m.add(q(0x40),q(0x50)));put(0x48,m.add(q(0x48),q(0x58)));if(value(0x40)< -8.||value(0x40)>392.||value(0x48)< -8.||value(0x48)>456.)b.word(0,0);}
  else if(signed_word(b.word(12))>=signed_word(b.word(0x78))){const auto factor=m.store(m.div(m.i(b.word(12)-b.word(0x78)),m.i(b.word(0x7c)-b.word(0x78))));put(0x40,m.add(m.mul(q(0x50),m.q(factor)),q(0x40)));put(0x48,m.add(m.mul(q(0x58),m.q(factor)),q(0x48)));}return;
 }
 if(behavior==86){put(0x40,m.add(q(0x40),q(0x50)));put(0x48,m.add(q(0x48),q(0x58)));put(0x50,m.mul(q(0x50),m.c(0x6d1bd0)));if(value(0x48)< -8.||value(0x48)>456.)b.word(0,0);return;}
 throw std::out_of_range("Stage5 final projectile outside79/81/86");
}
}
