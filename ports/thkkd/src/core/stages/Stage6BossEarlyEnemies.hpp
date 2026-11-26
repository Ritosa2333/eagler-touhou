// 原六面关底前组敌109..113；保留效果颜色返回值与服务后字段重读。
#pragma once
#include "core/stages/Stage6BossEarlyArithmetic.hpp"
#include "core/stages/Stage6BossEarlyEmitter.hpp"
#include "core/shared/EffectPool.hpp"
#include "core/entities/EnemyDeathContext.hpp"
namespace kaikeidou {
inline void update_stage6_boss_early_enemy(std::uint32_t behavior,Enemy& e,ProjectilePools& pool,EffectPool& effects,const ObjectRecord<240>& player,
 PatternParameters& p,Random& random,RandomPrecision precision,GameMath& math,EnemyDeathServices& services){
 using namespace stage6_boss_early_detail;const Arithmetic m{static_cast<unsigned>(precision)};const auto q=[&](std::size_t off){return m.q(e.quad(off));};const auto put=[&](std::size_t off,binary_float::Number value){e.quad(off,m.store(value));};const auto value=[&](std::size_t off){return crt_sse2::real(e.quad(off));};
 const auto aim=[&](){const auto dx=m.store(m.sub(m.q(player.quad(0x90)),q(0x38)));const auto dy=m.store(m.sub(m.q(player.quad(0x98)),q(0x40)));return math.atan2(dy,dx);};
 if(behavior==109||behavior==110){
  stage6_boss_early_ease(e,0x38,e.quad(0xa8),constant(0x6d1a40),m.precision);stage6_boss_early_ease(e,0x40,e.quad(0xb0),constant(0x6d1a40),m.precision);
  const auto effect=[&](bool blue){const auto y=e.quad(0x40),x=e.quad(0x38);const auto color=services.color(blue?0u:255u,0,blue?255u:0u);allocate_effect(effects,pool.sound_triggers,{12,color,x,y});};
  const auto shot=[&](bool blue,bool dynamic_sign){const auto slot=allocate_projectile(pool,{18,0,{{blue?0u:255u,0,blue?255u:0u}},2,255,0,41});if(slot==-1)return;auto& b=pool.bullets[static_cast<std::size_t>(slot)];b.word(0x78,dynamic_sign?e.word(0x80):blue?1u:0xffffffffu);const auto direction=aim();b.quad(0xa0,m.store(dynamic_sign?m.sub(direction,m.mul(m.i(b.word(0x78)),m.c(0x6d2180))):blue?m.sub(direction,m.c(0x6d2180)):m.add(direction,m.c(0x6d2180))));b.quad(0xa8,constant(0x6d1c18));b.quad(0xb0,dynamic_sign?m.store(m.mul(m.i(b.word(0x78)),m.c(0x6d1968))):constant(blue?0x6d1968:0x6d2178));b.quad(0x40,e.quad(0x38));b.quad(0x48,e.quad(0x40));b.word(8,93);b.word(0x34,1);};
  if(e.word(12)==(behavior==109?40u:20u)){effect(behavior==110||e.word(0x80)==1);e.word(0x2c,3);e.word(0x30,0);e.word(0x34,0);}
  else if(behavior==110&&e.word(12)==40)effect(false);
  else if(e.word(12)==80){e.word(0x2c,0);if(behavior==109)shot(e.word(0x80)==1,true);else{shot(true,false);shot(false,false);}}
  else if(e.word(12)==p.integers[3]-60u){put(0xa8,m.add(random.real_number(constant(0x6b25f8),precision),m.c(0x6d2170)));put(0xb0,m.add(random.real_number(constant(0x6d1a40),precision),m.c(0x6d1d50)));}
  else if(e.word(12)==p.integers[3]){e.word(12,0);if(behavior==109)e.word(0x80,0u-e.word(0x80));}return;
 }
 if(behavior==111){
  if(signed_word(e.word(12))<=29){if(e.word(12)==0){const auto base=m.store(m.sub(aim(),m.c(0x6d1900)));const auto angle=m.store(m.add(m.mul(random.real_number(constant(0x6d1a78),precision),m.c(0x6d1950)),m.q(base)));const auto cosine=math.cosine(angle);put(0x48,m.mul(cosine,m.q(p.reals[1])));const auto sine=math.sine(angle);put(0x50,m.mul(sine,m.q(p.reals[1])));}put(0x38,m.add(m.div(m.mul(m.i(e.word(12)),q(0x48)),m.c(0x6b25f8)),q(0x38)));put(0x40,m.add(m.div(m.mul(m.i(e.word(12)),q(0x50)),m.c(0x6b25f8)),q(0x40)));}
  else{put(0x38,m.add(q(0x38),q(0x48)));put(0x40,m.add(q(0x40),q(0x50)));}
  if(value(0x38)< -30.||value(0x38)>414.||value(0x40)< -30.||value(0x40)>478.)e.word(0,0);return;
 }
 if(behavior==112){put(0x38,m.add(q(0x38),q(0x48)));put(0x40,m.add(q(0x40),q(0x50)));if(value(0x38)<20.||value(0x38)>364.)put(0x48,m.mul(q(0x48),m.c(0x6aca58)));if(value(0x40)<20.||value(0x40)>428.)put(0x50,m.mul(q(0x50),m.c(0x6aca58)));
  if(e.word(12)==60)e.word(4,103);else if(e.word(12)==120)e.word(4,102);else if(e.word(12)==180)e.word(4,104);else if(e.word(12)==240)e.word(4,100);else if(e.word(12)==300)e.word(4,105);
  if(e.word(4)==105&&remainder(e.word(12),e.word(0x80))==0){const auto speed=constant(signed_word(p.difficulty)<=1?0x6d1b18:0x6d25a8);const auto y=e.quad(0x40),x=e.quad(0x38);stage6_boss_early_emit(pool,{{12,0,{{230,20,255}},0,255,1,41},x,y,3,constant(0x6d2138),speed,0,0,0},player,p,random,precision,math);}return;
 }
 if(behavior==113){put(0x40,m.add(q(0x40),m.c(0x6d1948)));if(remainder(e.word(12),e.word(0x80))==0){const auto slot=allocate_projectile(pool,{0,1,{{0,64,255}},0,255,1,41});if(slot!=-1){auto& b=pool.bullets[static_cast<std::size_t>(slot)];const auto x=m.store(m.add(q(0x38),m.c(0x6d1e20)));const auto x_random=random.real_number(constant(0x6d1e48),precision);b.quad(0x40,m.store(m.sub(m.q(x),x_random)));const auto y=m.store(m.add(q(0x40),m.c(0x6d1e20)));const auto y_random=random.real_number(constant(0x6d1e48),precision);b.quad(0x48,m.store(m.sub(m.q(y),y_random)));b.quad(0xa0,e.quad(0xa8));b.quad(0xa8,e.quad(0xb0));b.quad(0xb0,e.quad(0xb8));b.word(8,99);}}if(value(0x40)>478.)e.word(0,0);return;}
 throw std::out_of_range("Stage6 boss early enemy outside109..113");
}
}
