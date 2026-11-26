// EX关底阶段0..8余下普通弹140..145与副弹146/149/151。
#pragma once
#include "core/stages/ExBossEarlyFinalArithmetic.hpp"
#include "core/stages/ExBossEarlyFinalRecords.hpp"
#include "core/stages/ExBossEarlyFinalEmitter.hpp"
#include "core/shared/EffectUpdate.hpp"
#include "core/entities/BossPhaseContext.hpp"

namespace kaikeidou {
inline void update_ex_boss_early_final_projectile(ProjectilePools& pool,std::uint32_t index,std::uint32_t behavior,EnemyPool& enemies,EffectPool& effects,EffectShake& shake,ObjectRecord<240>& player,PatternParameters& p,const BossPhaseContext& phase,BossStageContext& boss,const StagePatternContext& extra,Random& random,RandomPrecision precision,GameMath& math,BossStageServices& services){
 using namespace ex_boss_early_final_detail;const Arithmetic m{static_cast<unsigned>(precision)};
 const bool secondary=behavior==146||behavior==149||behavior==151;
 PoolRecord b{pool,(secondary?0xa8ce60u:0xa8e660u)+index*(secondary?192u:240u),&player};
 const auto q=[&](unsigned o){return m.q(b.quad(o));};const auto put=[&](unsigned o,binary_float::Number v){b.quad(o,m.store(v));};
 const auto value=[&](unsigned o){return crt_sse2::real(b.quad(o));};const auto c=[](std::uint32_t a){return crt_sse2::real(constant(a));};
 const auto enemy=[&]()->Enemy&{return enemy_record(enemies,boss,phase.boss_index);};
 if(secondary){
  if(b.word(72)==0){ex_boss_early_final_ease(b,32,b.quad(40),constant(0x6d1a40),m.precision);return;}
  if(b.word(72)==1){
   ex_boss_early_final_ease(b,32,b.quad(40),constant(behavior==146?0x6d1898:0x6d1a40),m.precision);
   if(behavior==146)return;
   const auto dx=m.store(m.sub(m.q(player.quad(144)),q(16))),dy=m.store(m.sub(m.q(player.quad(152)),q(24)));
   const auto angle=m.store(math.atan2(dy,dx));
   const auto squared=m.store(m.add(m.mul(m.q(dx),m.q(dx)),m.mul(m.q(dy),m.q(dy))));
   auto distance=ex_boss_early_final_sqrt(squared,m.precision);const auto speed=m.store(m.mul(distance,behavior==149?m.c(0x6d1aa0):m.q(p.reals[1])));
   auto trig=math.cosine(angle);put(16,m.add(m.mul(trig,m.q(speed)),q(16)));
   trig=math.sine(angle);put(24,m.add(m.mul(trig,m.q(speed)),q(24)));
   if(behavior==151&&crt_sse2::real(p.reals[1])<c(0x6d1b48))p.reals[1]=m.store(m.add(m.q(p.reals[1]),m.c(0x6d1b40)));
   return;
  }
  if(b.word(72)==0xffffffffu){ex_boss_early_final_ease(b,32,0,constant(0x6d1a40),m.precision);if(value(32)<=1.)b.word(0,0);}return;
 }
 if(behavior==140){
  if(signed_word(b.word(12))<=120){ex_boss_early_final_ease(b,64,constant(0x6d19f0),constant(0x6d1ae8),m.precision);ex_boss_early_final_ease(b,72,constant(0x6d1a88),constant(0x6d1ae8),m.precision);return;}
  if(signed_word(b.word(12))<=200){ex_boss_early_final_ease(b,64,constant(0x6d19f0),constant(0x6d1a40),m.precision);ex_boss_early_final_ease(b,72,constant(0x6d1af0),constant(0x6d1a40),m.precision);
   if(b.word(12)==160){const auto y=enemy().quad(64),x=enemy().quad(56);const auto color=services.color(255,255,255);allocate_effect(effects,pool.sound_triggers,{12,color,x,y});}
   else if(b.word(12)==200){b.quad(64,constant(0x6d19f0));b.quad(72,constant(0x6d1af0));}return;
  }
  enemy().quad(56,m.store(m.sub(m.q(enemy().quad(56)),m.c(0x6d2868))));enemy().quad(64,m.store(m.add(m.q(enemy().quad(64)),m.c(0x6d2860))));
  put(96,m.add(q(96),m.q(p.reals[0])));
  const auto ring=[&](std::uint64_t radius,bool red){for(std::uint32_t j=0;j<6;++j){const auto slot=allocate_projectile(pool,{10,0,{{red?255u:0u,red?0u:255u,red?0u:255u}},0,255,0,41});if(slot==-1)continue;
   PoolRecord target{pool,0xa8e660u+static_cast<std::uint32_t>(slot)*240u,&player};target.word(8,141);target.quad(160,radius);
   auto angle=m.sub(m.c(0x6d1de0),m.mul(m.i(j),m.add(m.mul(m.sub(m.c(0x6d2850),m.mul(m.q(radius),m.q(radius))),m.c(0x6d2848)),m.c(0x6a7708))));target.quad(168,m.store(angle));
   auto trig=math.cosine(target.quad(168));target.quad(64,m.store(m.add(m.mul(trig,m.q(target.quad(160))),m.c(0x6d19f0))));
   trig=math.sine(target.quad(168));target.quad(72,m.store(m.add(m.mul(trig,m.q(target.quad(160))),m.c(0x6d1af0))));target.quad(96,m.store(m.add(m.q(target.quad(168)),m.c(0x6d1b60))));
  }};
  if(!remainder(b.word(12),6)){const auto radius=m.store(m.add(random.real_number(constant(0x6d2858),precision),m.c(0x6d1a90)));ring(radius,false);}
  if(!remainder(b.word(12),36)){const auto dx=m.sub(m.q(player.quad(144)),m.c(0x6d19f0)),dy=m.sub(m.q(player.quad(152)),m.c(0x6d1af0));auto radius=m.store(ex_boss_early_final_sqrt(m.store(m.add(m.mul(dx,dx),m.mul(dy,dy))),m.precision));if(crt_sse2::real(radius)<c(0x6d1a90))radius=m.store(m.add(random.real_number(constant(0x6d2858),precision),m.c(0x6d1a90)));ring(radius,true);}return;
 }
 if(behavior==141){auto trig=math.cosine(b.quad(168));put(64,m.add(m.mul(trig,q(160)),m.c(0x6d19f0)));trig=math.sine(b.quad(168));put(72,m.add(m.mul(trig,q(160)),m.c(0x6d1af0)));put(96,m.add(q(168),m.c(0x6d1b60)));put(168,m.add(q(168),m.q(p.reals[0])));if(value(168)>c(0x6d1b60))b.word(0,0);return;}
 if(behavior==142){
  const auto ellipse=[&](){auto trig=math.cosine(b.quad(160));auto a=m.store(m.mul(trig,m.q(p.reals[0])));trig=math.cosine(boss.extra_reals[0]);auto left=m.store(m.mul(trig,m.q(a)));trig=math.sine(b.quad(160));a=m.store(m.mul(trig,m.q(p.reals[1])));trig=math.sine(boss.extra_reals[0]);put(64,m.add(m.sub(m.q(left),m.mul(trig,m.q(a))),m.q(boss.extra_reals[1])));
   trig=math.cosine(b.quad(160));a=m.store(m.mul(trig,m.q(p.reals[0])));trig=math.sine(boss.extra_reals[0]);left=m.store(m.mul(trig,m.q(a)));trig=math.sine(b.quad(160));a=m.store(m.mul(trig,m.q(p.reals[1])));trig=math.cosine(boss.extra_reals[0]);put(72,m.add(m.add(m.mul(trig,m.q(a)),m.q(left)),m.q(extra.real_4)));};
  if(signed_word(b.word(12))<=200){ellipse();put(160,m.add(q(160),q(168)));const auto dx=m.store(m.sub(q(64),m.q(enemy().quad(56)))),dy=m.store(m.sub(q(72),m.q(enemy().quad(64))));const auto angle=m.store(math.atan2(dy,dx));
   const auto emit=[&](std::uint32_t kind,std::uint32_t red,std::uint32_t green,std::uint32_t blue,std::uint32_t speed,std::uint64_t direction){const auto y=b.quad(72),x=b.quad(64);ex_boss_early_final_emit(pool,{{kind,0,{{red,green,blue}},0,255,1,41},x,y,1,0,constant(speed),direction,1,0},player,p,random,precision,math);};
   if(remainder(b.word(12),4)==0){constexpr std::uint32_t speeds[]{0x6d2548,0x6d1ad8,0x6d26e0,0x6d31a0,0x6d1f68};for(unsigned j=0;j<5;++j)emit(11,j*32,j*32,255,speeds[j],angle);}
   else if(remainder(b.word(12),4)==2){constexpr std::uint32_t speeds[]{0x6d2480,0x6d26e8,0x6d1f68,0x6d2cb8,0x6d2be8};for(unsigned j=0;j<5;++j)emit(11,j*32,j*32,255,speeds[j],angle);}
   if(!remainder(b.word(12),3))emit(4,0,255,0,0x6d1948,m.store(m.add(m.q(angle),m.c(0x6d1860))));
   if(b.word(12)==200){b.quad(176,b.quad(64));b.quad(184,b.quad(72));}return;
  }
  if(b.word(12)==201){ellipse();const auto dx=m.store(m.sub(q(64),q(176))),dy=m.store(m.sub(q(72),q(184)));const auto angle=m.store(math.atan2(dy,dx));auto trig=math.cosine(angle);put(192,m.mul(trig,m.c(0x6d2410)));trig=math.sine(angle);put(200,m.mul(trig,m.c(0x6d2410)));}
  put(64,m.add(q(64),q(192)));put(72,m.add(q(72),q(200)));if(value(64)<c(0x6d1ff0)||value(72)<c(0x6d1ff0)||value(64)>c(0x6d2000)||value(72)>c(0x6d3198))b.word(0,0);return;
 }
 if(behavior==143||behavior==145){
  if(signed_word(b.word(12))>=signed_word(b.word(124))){put(64,m.add(q(64),q(80)));put(72,m.add(q(72),q(88)));if(behavior==143){if(value(72)>=c(0x6d1f98)){b.word(8,144);if(!shake.word(0))set_effect_screen_shake(shake,4,8);}}else if(value(72)<c(0x6d1b58))b.word(0,0);}
  else if(signed_word(b.word(12))>=signed_word(b.word(120))){const auto scale=m.store(m.div(m.i(b.word(12)-b.word(120)),m.i(b.word(124)-b.word(120))));put(64,m.add(m.mul(q(80),m.q(scale)),q(64)));put(72,m.add(m.mul(q(88),m.q(scale)),q(72)));}return;
 }
 if(behavior==144){
  if(b.word(24)==255){for(std::uint32_t j=0;j<3000;++j){PoolRecord other{pool,0xa8e660u+j*240u,&player};if(other.word(0)!=1||other.word(4)!=9)continue;const auto dx=m.store(m.sub(m.q(other.quad(64)),q(64))),dy=m.store(m.sub(m.q(other.quad(72)),q(72)));const auto distance=ex_boss_early_final_sqrt(m.store(m.add(m.mul(m.q(dx),m.q(dx)),m.mul(m.q(dy),m.q(dy)))),m.precision);if(compare(distance,m.c(0x6d1a90))<=0)b.word(24,254);}
   if((value(64)<c(0x6d2840)||value(64)>c(0x6d2838))&&signed_word(b.word(12))>=600)b.word(0,0);return;
  }
  b.word(24,b.word(24)-4);if(signed_word(b.word(24))>3)return;
  for(unsigned j=0;j<5;++j){const auto slot=allocate_projectile(pool,{11,0,{{0,255,0}},0,255,1,41});if(slot==-1)continue;PoolRecord target{pool,0xa8e660u+static_cast<std::uint32_t>(slot)*240u,&player};target.word(120,0);target.word(124,60);
   const auto angle=m.store(m.mul(random.real_number(constant(0x6d1c60),precision),m.c(0x6d1950)));const auto radius=m.store(m.add(m.mul(random.real_number(constant(0x6d1ab0),precision),m.c(0x6d1960)),m.c(0x6d1ae8)));
   auto trig=math.cosine(angle);target.quad(64,m.store(m.add(m.mul(trig,m.q(radius)),q(64))));trig=math.sine(angle);target.quad(72,m.store(m.add(m.mul(trig,m.q(radius)),q(72))));
   auto velocity=b.quad(80)^0x8000000000000000ull;auto number=random.real_number(constant(0x6d1a78),precision);target.quad(80,m.store(m.mul(m.add(m.mul(number,m.c(0x6d2830)),m.c(0x6d1900)),m.q(velocity))));
   velocity=b.quad(88)^0x8000000000000000ull;number=random.real_number(constant(0x6d1a78),precision);target.quad(88,m.store(m.mul(m.add(m.mul(number,m.c(0x6d2830)),m.c(0x6d1900)),m.q(velocity))));target.quad(96,b.quad(96));target.word(8,145);
  }
  ex_boss_early_final_remove(pool,index,100);return;
 }
 throw std::out_of_range("EX early final projectile closure");
}
}
