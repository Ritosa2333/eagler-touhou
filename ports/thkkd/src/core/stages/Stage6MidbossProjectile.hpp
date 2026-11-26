// 原普通弹90..92、副弹87/88，保留副弹阶段写入和发弹前颜色转换顺序。
#pragma once
#include "core/stages/Stage6MidbossArithmetic.hpp"
#include "core/stages/Stage3InitialMath.hpp"
#include "core/stages/Stage6MidbossEmission.hpp"
#include "core/shared/FinalProjectileRecord.hpp"
namespace kaikeidou {
inline void update_stage6_midboss_projectile(ProjectilePools& pool,std::uint32_t index,std::uint32_t behavior,const ObjectRecord<240>& player,PatternParameters& p,Random& random,RandomPrecision precision,GameMath& math){
 using namespace stage6_midboss_detail;const Arithmetic m{{static_cast<unsigned>(precision)}};const auto real=[](std::uint64_t v){return crt_sse2::real(v);};
 if(behavior==87||behavior==88){auto& s=pool.secondary.at(index);const auto q=[&](std::size_t o){return m.q(s.quad(o));};
  if(s.word(0x48)==0){stage6_midboss_approach_record(s,0x20,s.quad(0x28),constant(0x6d1a40),m.precision);if(behavior==88&&stage5_boss_final_detail::compare(m.sub(q(0x28),m.c(0x6d1960)),q(0x20))<=0)s.word(0x48,1);}
  else if(s.word(0x48)==1){if(behavior==87){if(signed_word(p.game_frame)>=200){s.quad(0x20,m.store(m.mul(q(0x20),m.c(0x6d1bd0))));if(real(s.quad(0x20))<=1.)s.word(0,0);}}
   else{s.quad(0x20,m.store(m.mul(q(0x20),m.c(0x6d1a20))));if(real(s.quad(0x20))<=real(constant(0x6d1a00))){const auto spacing=stage3_initial_turn_rate(p.integers[0],m.precision);const auto count=p.integers[0];const auto y=s.quad(0x18),x=s.quad(0x10);const auto blue=stage6_midboss_convert(m.mul(q(0x40),m.c(0x6a7eb8)));const auto green=stage6_midboss_convert(m.mul(q(0x38),m.c(0x6a7eb8)));const auto red=stage6_midboss_convert(m.mul(q(0x30),m.c(0x6a7eb8)));stage6_midboss_emit(pool,{{4,0,{{red,green,blue}},0,255,0,40},x,y,count,spacing,constant(0x6d19c0),0,0,0},player,p,random,precision,math);s.word(0,0);}}
  }else if(s.word(0x48)==0xffffffffu){stage6_midboss_approach_record(s,0x20,0,constant(0x6d1a40),m.precision);if(real(s.quad(0x20))<=1.)s.word(0,0);}return;
 }
 final_projectile_detail::Record b{pool,index};const auto q=[&](std::size_t o){return m.q(b.quad(o));};const auto put=[&](std::size_t o,binary_float::Number v){b.quad(o,m.store(v));};
 if(behavior==90){put(0x48,m.add(q(0x48),q(0xa0)));if(real(b.quad(0x48))>real(constant(0x6d1a48)))b.word(0,0);b.word(0x3c,p.integers[3]!=b.word(0x78));}
 else if(behavior==91){put(0x40,m.add(m.mul(m.i(b.word(0x7c)),m.q(p.reals[1])),q(0xb0)));put(0xa8,m.add(q(0xa8),q(0xa0)));put(0x48,m.add(m.mul(m.i(b.word(0x7c)),m.q(p.reals[0])),q(0xa8)));if(real(b.quad(0xa8))>real(constant(0x6d1bc8)))b.word(0,0);b.word(0x3c,p.integers[3]!=b.word(0x78));}
 else if(behavior==92){const auto cosine=math.cosine(m.store(m.add(m.q(p.reals[0]),q(0xa0))));put(0x40,m.add(m.mul(cosine,q(0xa8)),m.c(0x6d1a60)));const auto sine=math.sine(m.store(m.add(m.q(p.reals[0]),q(0xa0))));put(0x48,m.add(m.mul(sine,q(0xa8)),m.c(0x6d1af0)));put(0xa8,m.add(q(0xa8),q(0xb0)));if(real(b.quad(0xa8))>real(constant(0x6d1bc0)))b.word(0,0);}
 else throw std::out_of_range("Stage6 midboss projectile behavior");
}
}
