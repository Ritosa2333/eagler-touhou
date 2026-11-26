// 原弹54..56；55的完整线性尾界6061E6及跳表，保留子弹与源槽别名后的字段重读。
#pragma once
#include "core/stages/Stage4BossEarlyArithmetic.hpp"
#include "core/entities/BossStageContext.hpp"
#include "core/shared/FinalProjectileRecord.hpp"
namespace kaikeidou {
inline void update_stage4_boss_projectile_early(ProjectilePools& pool,std::uint32_t index,std::uint32_t behavior,std::uint64_t px,std::uint64_t py,
 PatternParameters& p,const BossStageContext& boss,const StagePatternContext& extra,const Stage4PatternContext& fourth,const std::uint32_t& integer5,
 Random& random,RandomPrecision precision,GameMath& math){
 using namespace stage4_boss_early_detail;const Arithmetic m{{static_cast<unsigned>(precision)}};final_projectile_detail::Record b{pool,index};const auto q=[&](std::size_t o){return m.q(b.quad(o));};const auto put=[&](std::size_t o,binary_float::Number n){b.quad(o,m.store(n));};
 if(behavior==54){const auto cx=math.cosine(b.quad(0xa0));put(0x40,m.add(m.mul(cx,q(0xb0)),m.c(0x6d1a60)));const auto sy=math.sine(b.quad(0xa0));put(0x48,m.add(m.mul(sy,q(0xb0)),m.c(0x6d1af0)));put(0xa0,m.add(q(0xa0),q(0xa8)));put(0xb0,m.add(q(0xb0),q(0xb8)));if(crt_sse2::real(b.quad(0xb0))>330.)b.word(0,0);b.word(0x3c,p.integers[3]!=b.word(0x78));return;}
 if(behavior==55){
  const auto reset=[&](std::uint32_t state){b.word(0x80,state);b.word(12,0);b.quad(0xa8,0);};
  switch(b.word(0x80)){
   case 0xfffffffeu:if(b.word(12)==60)reset(3);return;
   case 0xffffffffu:if(b.word(12)==60)reset(1);return;
   case 0:put(0xa8,m.mul(q(0xa8),m.c(0x6d1a20)));if(b.word(12)==60){reset(1);if(signed_word(p.difficulty)>=2)put(0xb8,m.mul(q(0xb8),m.c(0x6aca58)));}break;
   case 1:b.quad(0x48,b.quad(0xb0));put(0xb0,m.add(q(0xb0),m.c(0x6d1868)));put(0xa8,m.add(q(0xa8),m.q(boss.extra_reals[1])));if(crt_sse2::real(b.quad(0xb0))>=404.){b.word(0x80,2);b.word(12,0);}break;
   case 2:put(0xa8,m.mul(q(0xa8),m.c(0x6d1a20)));if(b.word(12)==60)reset(3);break;
   case 3:b.quad(0x48,b.quad(0xb0));put(0xb0,m.sub(q(0xb0),m.c(0x6d1868)));put(0xa8,m.add(q(0xa8),m.q(boss.extra_reals[1])));if(crt_sse2::real(b.quad(0xb0))<=44.){b.word(0x80,0);b.word(12,0);}break;
   default:break;
  }
  put(0xa0,m.add(q(0xa0),q(0xb8)));if(remainder(b.word(12),p.integers[3]))return;const bool easy=signed_word(p.difficulty)<=1;
  const auto child=allocate_projectile(pool,{21,2,color(p,extra,fourth,integer5,b.word(0x7c)),0,255,1,41});if(child<0)return;auto& c=pool.bullets[static_cast<std::size_t>(child)];
  const auto cosine=math.cosine(b.quad(0xa0));c.quad(0x40,m.store(m.add(m.mul(cosine,q(0xa8)),easy?q(0x40):m.c(0x6d1a60))));const auto sine=math.sine(b.quad(0xa0));c.quad(0x48,m.store(m.add(m.mul(m.mul(sine,q(0xa8)),m.c(easy?0x6d1900:0x6d1920)),q(0x48))));c.word(8,56);c.word(0x3c,1);c.word(0x78,b.word(0x78));c.word(0x7c,b.word(0x7c));c.quad(0xa0,easy?b.quad(0x40):constant(0x6d1a60));c.quad(0xa8,b.quad(0x48));return;
 }
 if(behavior!=56)throw std::out_of_range("Stage4 early projectile outside54..56");const auto age=signed_word(b.word(12));if(age<=179)return;
 if(age<=239){if(b.word(12)==180){b.word(0x3c,0);std::uint64_t angle=0;bool angled=false;
   if(b.word(0x78)==0){const auto dx=m.store(m.sub(m.q(px),q(0xa0))),dy=m.store(m.sub(m.q(py),q(0xa8)));angle=m.store(m.add(math.atan2(dy,dx),m.q(p.reals[1])));p.reals[1]=m.store(m.add(m.q(p.reals[1]),m.c(0x6d1b60)));angled=true;}
   else if(b.word(0x78)==2){angle=m.store(m.mul(random.real_number(constant(0x6d1c60),precision),m.c(0x6d1950)));angled=true;}
   if(angled){const auto cosine=math.cosine(angle);put(0x50,m.mul(cosine,m.q(boss.extra_reals[0])));const auto sine=math.sine(angle);put(0x58,m.mul(sine,m.q(boss.extra_reals[0])));}
   else if(b.word(0x78)==1){const auto cosine=math.cosine(p.reals[0]);put(0x50,m.mul(cosine,m.add(m.q(boss.extra_reals[0]),m.c(0x6d18e0))));const auto sine=math.sine(p.reals[0]);put(0x58,m.mul(sine,m.add(m.q(boss.extra_reals[0]),m.c(0x6d18e0))));p.reals[0]=m.store(m.sub(m.q(p.reals[0]),m.c(0x6d1c68)));}
  }
  put(0x40,m.add(m.div(m.mul(m.i(b.word(12)-180u),q(0x50)),m.c(0x6d1aa8)),q(0x40)));put(0x48,m.add(m.div(m.mul(m.i(b.word(12)-180u),q(0x58)),m.c(0x6d1aa8)),q(0x48)));return;
 }
 put(0x40,m.add(q(0x40),q(0x50)));put(0x48,m.add(q(0x48),q(0x58)));const auto x=crt_sse2::real(b.quad(0x40)),y=crt_sse2::real(b.quad(0x48));if(y>456.||x< -8.||x>392.||y< -8.)b.word(0,0);
}
}
