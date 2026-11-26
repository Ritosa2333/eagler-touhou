// 普通弹128..139；原效果、发射、参数别名和同对象行为切换。
#pragma once
#include "core/stages/ExBossMiddleProjectileArithmetic.hpp"
#include "core/stages/ExBossMiddleProjectileRecords.hpp"
#include "core/stages/ExBossMiddleProjectileEmitter.hpp"
#include "core/shared/EffectPool.hpp"

namespace kaikeidou::ex_boss_middle_projectile_detail {
inline std::uint32_t integer_slot(const PatternParameters& p,const EnemyDeathContext& death,const StagePatternContext& extra,std::uint32_t index){const auto a=0xa78558u+index*4u;if(a>=0xa78558u&&a<0xa7856cu)return p.integers[(a-0xa78558u)/4u];if(a==0xa7856cu)return death.pattern_integer_5;if(a>=0xa78570u&&a<0xa785a8u)return extra.integers_6_to_19[(a-0xa78570u)/4u];throw std::out_of_range("EX middle global integer outside0..19");}
}
namespace kaikeidou {
inline void update_ex_boss_middle_projectile(ProjectilePools& pool,std::uint32_t index,std::uint32_t behavior,EffectPool& effects,const ObjectRecord<240>& player,const PatternParameters& p,const BossStageContext& context,const EnemyDeathContext& death,const StagePatternContext& extra,Random& random,RandomPrecision precision,GameMath& math,BossStageServices& services){
 using namespace ex_boss_middle_projectile_detail;const Arithmetic m{static_cast<unsigned>(precision)};PoolRecord b{pool,0xa8e660u+index*240u};const auto q=[&](unsigned o){return m.q(b.quad(o));};const auto put=[&](unsigned o,binary_float::Number n){b.quad(o,m.store(n));};const auto value=[&](unsigned o){return crt_sse2::real(b.quad(o));};const auto c=[](std::uint32_t a){return crt_sse2::real(constant(a));};
 if(behavior==128){put(64,m.add(m.mul(q(80),q(160)),q(64)));put(72,m.add(m.mul(q(88),q(160)),q(72)));ex_boss_middle_projectile_ease(b,160,constant(0x6d1900),constant(0x6b25f8),m.precision);if(b.word(12)==350)ex_boss_middle_projectile_remove(pool,index,0);return;}
 if(behavior==129){ex_boss_middle_projectile_ease(b,64,constant(0x6d1a60),constant(0x6d1ba8),m.precision);ex_boss_middle_projectile_ease(b,72,constant(0x6d1af0),constant(0x6d1ba8),m.precision);
  const auto effect=[&](std::uint32_t kind){const auto blue=255u-p.integers[4],green=255u-p.integers[3],red=255u-p.integers[2];const auto color=services.color(red,green,blue);allocate_effect(effects,pool.sound_triggers,{kind,color,constant(0x6d1a60),constant(0x6d1af0)});};
  if(b.word(12)==60)effect(10);
  else if(b.word(12)==100){effect(14);for(unsigned j=0;j<30;++j){for(unsigned slow=0;slow<2;++slow){const auto angle=m.store(m.mul(random.real_number(constant(0x6d1c60),precision),m.c(0x6d1950)));const auto v=m.mul(random.real_number(constant(0x6b25b8),precision),m.c(0x6a7708));const auto speed=m.store(m.add(v,slow?m.q(0x3ff0000000000000ull):m.c(0x6d1948)));const auto blue=255u-p.integers[4],green=255u-p.integers[3],red=255u-p.integers[2];ex_boss_middle_projectile_emit(pool,{{11,1,{{red,green,blue}},2,255,0,41},constant(0x6d1a60),constant(0x6d1af0),1,0,speed,angle,1,0},player,p,random,precision,math);}}}
  else if(b.word(12)==101)ex_boss_middle_projectile_remove(pool,index,100);return;
 }
 if(behavior==130){auto angle=m.store(m.add(m.q(context.extra_reals[0]),q(160)));auto v=math.cosine(angle);put(64,m.add(m.mul(v,q(168)),m.q(p.reals[0])));angle=m.store(m.add(m.q(context.extra_reals[0]),q(160)));v=math.sine(angle);put(72,m.add(m.mul(v,q(168)),m.q(p.reals[1])));return;}
 if(behavior==131){const auto anchor=[&](unsigned field){return PoolRecord{pool,0xa8e660u+integer_slot(p,death,extra,b.word(field))*240u};};for(unsigned off:{64u,72u}){auto a=anchor(120),end=anchor(124);put(off,m.add(m.mul(m.div(m.sub(m.q(end.quad(off)),m.q(a.quad(off))),m.c(0x6d1910)),m.i(b.word(128))),m.q(a.quad(off))));}return;}
 if(behavior==132){put(64,m.add(q(64),q(160)));put(72,m.add(q(72),q(168)));if(signed_word(b.word(120))>=0){if(value(64)<c(0x6d18e8)||value(64)>c(0x6d1b80)){put(160,m.mul(q(160),m.c(0x6aca58)));b.word(120,b.word(120)-1);}if(value(72)<c(0x6d18e8)||value(72)>c(0x6d1b78)){put(168,m.mul(q(168),m.c(0x6aca58)));b.word(120,b.word(120)-1);}}else if(value(72)>c(0x6d19f8)||value(64)<c(0x6d19f0)||value(64)>c(0x6d19e8)||value(72)<c(0x6d19f0))b.word(0,0);return;}
 if(behavior==133){put(160,m.sub(q(160),m.c(0x6d1b70)));put(168,m.add(q(168),m.c(0x6d1968)));put(176,m.add(q(176),m.c(0x6d1b68)));auto v=math.sine(b.quad(168));const auto first=m.store(m.mul(v,m.c(0x6d1ae8)));v=math.sine(b.quad(176));const auto radius=m.store(m.add(m.mul(v,m.c(0x6d1a40)),m.q(first)));v=math.cosine(b.quad(160));put(64,m.add(m.mul(v,m.q(radius)),m.q(p.reals[0])));v=math.sine(b.quad(160));put(72,m.add(m.mul(v,m.q(radius)),m.q(p.reals[1])));return;}
 if(behavior==135){put(64,m.add(q(64),q(80)));put(72,m.add(q(72),q(88)));put(80,m.mul(q(80),m.c(0x6d1b10)));put(88,m.mul(q(88),m.c(0x6d1b10)));if(b.word(12)==90){if(!b.word(128)){ex_boss_middle_projectile_remove(pool,index,100);return;}b.word(8,value(160)>=0.?(value(168)>=0.?139u:138u):(value(168)>=0.?137u:136u));b.word(24,255);b.word(28,255);b.word(32,0);put(96,m.add(q(96),m.c(0x6d1860)));}return;}
 if(behavior==134||(behavior>=136&&behavior<=139)){if(remainder(b.word(12),b.word(120))!=signed_word(b.word(124)))return;put(64,m.add(q(64),q(behavior==134?80u:160u)));put(72,m.add(q(72),q(behavior==134?88u:168u)));bool outside=false;
  if(behavior==134)outside=value(64)<c(0x6d1840)||value(72)<c(0x6d1840)||value(64)>c(0x6d1a38)||value(72)>c(0x6d18c8);
  else if(behavior==136)outside=value(64)<c(0x6d1840)||value(72)<c(0x6d1840);
  else if(behavior==137)outside=value(64)<c(0x6d1840)||value(72)>c(0x6d18c8);
  else if(behavior==138)outside=value(64)>c(0x6d1a38)||value(72)<c(0x6d1840);
  else outside=value(64)>c(0x6d1a38)||value(72)>c(0x6d18c8);
  if(outside)b.word(0,0);return;
 }
 throw std::out_of_range("EX middle projectile outside128..139");
}
}
