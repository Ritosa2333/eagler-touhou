// EX关底9..15的17个普通/次弹行为；保留固定槽读取及缓存敌索引。
#pragma once
#include "core/stages/ExBossMiddleFinalArithmetic.hpp"
#include "core/stages/ExBossMiddleFinalRecords.hpp"
#include "core/stages/ExBossMiddleFinalEmitter.hpp"
#include "core/entities/BossPhaseContext.hpp"
namespace kaikeidou {
inline void update_ex_boss_middle_final_projectile(ProjectilePools& pool,std::uint32_t index,std::uint32_t behavior,EnemyPool& enemies,ObjectRecord<240>& player,const PatternParameters& p,const BossPhaseContext& phase,BossStageContext& boss,Random& random,RandomPrecision precision,GameMath& math){
 using namespace ex_boss_middle_final_detail;const Arithmetic m{static_cast<unsigned>(precision)};
 const bool secondary=behavior==147||behavior==158||behavior==163||behavior==166;
 PoolRecord b{pool,(secondary?0xa8ce60u:0xa8e660u)+index*(secondary?192u:240u),&player,&enemies,&boss};
 const auto q=[&](unsigned o){return m.q(b.quad(o));};const auto put=[&](unsigned o,binary_float::Number value){b.quad(o,m.store(value));};
 const auto value=[&](unsigned o){return crt_sse2::real(b.quad(o));};const auto c=[](std::uint32_t a){return crt_sse2::real(constant(a));};
 PoolRecord zero{pool,0xa8ce60u,&player,&enemies,&boss};
 if(secondary){
  if(b.word(72)==0){ex_boss_middle_final_ease(b,32,b.quad(40),constant(0x6d1a40),m.precision);return;}
  if(b.word(72)==0xffffffffu){ex_boss_middle_final_ease(b,32,0,constant(0x6d1a40),m.precision);if(value(32)<=1.)b.word(0,0);return;}
  if(b.word(72)!=1)return;
  if(behavior==147){
   const auto dx=m.store(m.sub(m.q(player.quad(144)),q(16))),dy=m.store(m.sub(m.q(player.quad(152)),q(24)));const auto angle=m.store(math.atan2(dy,dx));
   if(value(112)<c(0x6a7708))put(112,m.add(q(112),m.c(0x6d1b50)));
   auto distance=ex_boss_middle_final_sqrt(m.store(m.add(m.mul(m.q(dx),m.q(dx)),m.mul(m.q(dy),m.q(dy)))),m.precision);const auto speed=m.store(m.mul(distance,q(112)));
   auto trig=math.cosine(angle);put(16,m.add(m.mul(trig,m.q(speed)),q(16)));trig=math.sine(angle);put(24,m.add(m.mul(trig,m.q(speed)),q(24)));return;
  }
  if(behavior==163){
   if(b.word(76)==0){ex_boss_middle_final_ease(b,32,b.quad(40),constant(0x6b25f8),m.precision);return;}
   b.word(76,b.word(76)-1);if(b.word(76)==0){b.quad(48,constant(0x6d19c0));b.quad(56,constant(0x6d19c0));b.quad(64,constant(0x6d19c0));}
   ex_boss_middle_final_ease(b,32,constant(0x6d1a90),constant(0x6d1898),m.precision);return;
  }
  auto trig=math.cosine(b.quad(112));put(16,m.add(m.mul(trig,q(120)),m.c(0x6d1a60)));
  trig=math.sine(b.quad(112));put(24,m.add(m.mul(trig,q(120)),m.c(behavior==158?0x6d1b38:0x6d1af0)));
  if(behavior==158){put(112,m.add(m.mul(q(128),m.q(zero.quad(136))),q(112)));if(crt_sse2::real(zero.quad(136))<1.)zero.quad(136,m.store(m.add(m.q(zero.quad(136)),m.c(0x6d1950))));}
  else{
   const auto divisor=b.quad(144),target=b.quad(128);ex_boss_middle_final_ease(b,112,target,divisor,m.precision);
   const auto radial_divisor=b.quad(152),radius=b.quad(136);ex_boss_middle_final_ease(b,120,radius,radial_divisor,m.precision);
   ex_boss_middle_final_ease(b,144,constant(0x6d1ae8),constant(0x6d1ae8),m.precision);ex_boss_middle_final_ease(b,152,constant(0x6d1ae8),constant(0x6d1ae8),m.precision);
  }return;
 }
 if(behavior==121){put(96,m.add(m.mul(m.i(b.word(120)),m.q(p.reals[0])),m.c(0x6d1ba0)));put(72,m.add(q(72),q(88)));if(value(72)>value(232))b.word(0,0);return;}
 if(behavior==122){put(96,m.add(m.mul(q(160),q(176)),q(96)));if(value(176)<1.)put(176,m.add(q(176),m.c(0x6a7708)));const auto target=m.store(m.add(m.q(player.quad(144)),q(168)));ex_boss_middle_final_ease(b,64,target,constant(0x6b25f8),m.precision);return;}
 if(behavior==148){
  const auto position=[&](){auto angle=m.store(m.add(m.q(p.reals[0]),q(160)));auto trig=math.cosine(angle);put(64,m.add(m.mul(trig,q(176)),m.q(zero.quad(16))));angle=m.store(m.add(m.q(p.reals[0]),q(160)));trig=math.sine(angle);put(72,m.add(m.mul(trig,q(176)),m.q(zero.quad(24))));};
  if(b.word(120)==0){ex_boss_middle_final_ease(b,176,boss.extra_reals[0],constant(0x6d1ba8),m.precision);position();return;}
  if(b.word(120)!=1)return;
  ex_boss_middle_final_ease(b,176,constant(0x6d1a90),constant(0x6d1898),m.precision);
  if(value(176)<=c(0x6d1cc0)){
   b.quad(176,constant(0x6d1cc0));b.word(120,0);const bool alternate=b.word(124)!=0;
   for(unsigned j=0;j<6;++j){
    const auto direction=m.store(m.add(m.add(m.q(p.reals[0]),q(160)),m.c(0x6d1860)));
    auto angle=m.store(m.add(m.q(p.reals[0]),q(160)));auto trig=math.sine(angle);const auto y=m.store(m.add(m.mul(trig,m.c(0x6d1d00)),m.q(zero.quad(24))));
    angle=m.store(m.add(m.q(p.reals[0]),q(160)));trig=math.cosine(angle);const auto x=m.store(m.add(m.mul(trig,m.c(0x6d1d00)),m.q(zero.quad(16))));
    ex_boss_middle_final_emit(pool,{{10,2,{{255,alternate?0u:255u,0}},0,255,1,41},x,y,alternate?7u:8u,constant(0x6d18e0),constant(alternate?0x6d1f50:0x6d1ad0),direction,1,0},player,p,random,precision,math);
   }
   if(alternate)b.word(124,0);
  }
  position();return;
 }
 if(behavior==150){const auto dx=m.store(m.sub(m.q(zero.quad(16)),q(64))),dy=m.store(m.sub(m.q(zero.quad(24)),q(72)));const auto angle=m.store(math.atan2(dy,dx));const auto distance=ex_boss_middle_final_sqrt(m.store(m.add(m.mul(m.q(dx),m.q(dx)),m.mul(m.q(dy),m.q(dy)))),m.precision);const auto speed=m.store(m.mul(distance,m.c(0x6a7708)));auto trig=math.cosine(angle);put(64,m.add(m.mul(trig,m.q(speed)),q(64)));trig=math.sine(angle);put(72,m.add(m.mul(trig,m.q(speed)),q(72)));return;}
 if(behavior==152||behavior==153){put(64,m.add(q(64),q(80)));put(72,m.add(q(72),q(88)));
  if(behavior==152){if(value(64)>=c(0x6d1a28))put(64,m.sub(q(64),m.c(0x6d1a78)));}else if(value(64)<=c(0x6d1a30))put(64,m.add(q(64),m.c(0x6d1a78)));
  if(value(72)>c(0x6d1a68))b.word(0,0);b.word(60,p.integers[3]!=b.word(120));return;
 }
 if(behavior==154||behavior==155){PoolRecord peer{pool,0xa8e660u+(index-200u)*240u,&player,&enemies,&boss};const bool ordinary=behavior==154?b.word(156)==0xffffffffu:peer.word(156)==1;put(64,m.i(b.word(120)*48u+(ordinary?24u:0u-2976u)));PoolRecord anchor{pool,0xac8fe0u,&player,&enemies,&boss};put(72,m.add(m.add(m.q(anchor.quad(232)),m.c(0x6d1828)),m.i(b.word(124)*48u)));return;}
 if(behavior==156||behavior==159||behavior==165){
  if(behavior==156)put(96,m.add(m.q(p.reals[0]),q(176)));
  auto enemy_index=phase.boss_index;auto angle=behavior==156?m.store(m.add(m.add(m.q(p.reals[0]),q(176)),q(168))):b.quad(160);auto trig=math.cosine(angle);
  put(64,m.add(m.mul(trig,q(behavior==156?160u:168u)),m.q(enemy_record(enemies,boss,enemy_index).quad(56))));
  enemy_index=phase.boss_index;angle=behavior==156?m.store(m.add(m.add(m.q(p.reals[0]),q(176)),q(168))):b.quad(160);trig=math.sine(angle);
  put(72,m.add(m.mul(trig,q(behavior==156?160u:168u)),m.q(enemy_record(enemies,boss,enemy_index).quad(64))));
  if(behavior==159){put(160,m.add(q(160),m.c(0x6d1b30)));put(168,m.add(q(168),m.c(0x6d1b28)));if(value(168)>c(0x6d19d0))b.word(0,0);}
  else if(behavior==165){put(160,m.sub(q(160),m.c(0x6d1aa0)));put(168,m.add(q(168),m.c(0x6d1b00)));if(value(168)>c(0x6d1af8))b.word(0,0);}return;
 }
 if(behavior==157){auto trig=math.cosine(m.store(m.add(m.q(p.reals[0]),q(160))));put(64,m.add(m.mul(trig,m.q(p.reals[1])),q(64)));trig=math.sine(m.store(m.add(m.q(p.reals[0]),q(160))));put(72,m.add(m.mul(trig,m.q(p.reals[1])),q(72)));if(b.word(12)==420)b.word(0,0);return;}
 if(behavior==164){put(64,m.add(q(64),q(80)));put(72,m.add(q(72),q(88)));if(signed_word(b.word(12))<=50){put(80,m.mul(q(80),m.c(0x6d1b08)));put(88,m.mul(q(88),m.c(0x6d1b08)));return;}
  if(value(88)>=0.){if(value(72)>c(0x6d18c8))b.word(0,0);}else if(value(72)<c(0x6d1840))b.word(0,0);
  if(value(80)>=0.){if(value(64)>c(0x6d1a38))b.word(0,0);}else if(value(64)<c(0x6d1840))b.word(0,0);return;
 }
 throw std::out_of_range("EX middle final behavior outside verified closure");
}
}
