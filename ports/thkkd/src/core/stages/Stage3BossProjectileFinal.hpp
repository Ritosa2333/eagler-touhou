// 原弹45..50；保留分配别名、轨迹数组跨邻接字段读取与每处binary64落点。
#pragma once
#include "core/stages/Stage3BossFinalArithmetic.hpp"
#include "core/entities/ProjectileEmitter.hpp"
#include "core/shared/FinalProjectileRecord.hpp"
namespace kaikeidou {
inline void update_stage3_boss_projectile_final(ProjectilePools& pool,std::uint32_t index,std::uint32_t behavior,
    std::uint64_t px,std::uint64_t py,PatternParameters& p,BossStageContext& boss,StagePatternContext& extra,
    Stage3BossTimelineContext& context,Random& random,RandomPrecision precision,GameMath& math){
 using namespace stage3_boss_final_detail;const Arithmetic m{static_cast<unsigned>(precision)};final_projectile_detail::Record b{pool,index};
 const auto r=[&](std::int64_t n){return real(n,p,boss,extra,context);};
 const auto emit=[&](std::uint32_t kind,std::int32_t group,std::array<std::uint32_t,3> color,std::uint64_t speed,std::uint64_t angle,std::uint32_t absolute,std::uint32_t delayed){
  emit_projectiles(pool,{{kind,group,color,0,255,delayed,41},b.quad(0x40),b.quad(0x48),1,0,speed,angle,absolute,0},px,py,p,random,precision,math);
 };
 if(behavior==45||behavior==50){
  b.quad(0x40,m.store(m.add(m.q(b.quad(0x40)),m.q(b.quad(0x50)))));b.quad(0x48,m.store(m.add(m.q(b.quad(0x48)),m.q(b.quad(0x58)))));
  if(behavior==45){if(b.word(0xc)==80){
   const auto base=m.store(m.sub(m.add(m.mul(math.sine(r(0)),m.c(0x6d24e0)),m.c(0x6d1b60)),m.c(0x6d1900)));
   const auto angle=m.store(m.add(m.mul(random.real_number(constant(0x6d1a78),precision),m.c(0x6d1950)),m.q(base)));
   emit(10,0,{{0,255,255}},r(2),angle,1,0);b.word(0,0);
  }return;}
  const auto y=crt_sse2::real(b.quad(0x48));if(y< -12.||y>460.){b.word(0,0);return;}
  const auto lower=m.add(m.sub(m.c(0x6d1a60),m.q(r(1))),m.i(b.word(0x78)));const auto upper=m.add(m.add(m.c(0x6d1a60),m.q(r(1))),m.i(b.word(0x78)));
  if(compare(lower,m.q(b.quad(0x40)))>0||compare(upper,m.q(b.quad(0x40)))<0){
   constexpr std::array<std::array<std::uint32_t,3>,12> colors{{{{0,0,255}},{{128,0,255}},{{255,0,255}},{{255,0,128}},{{255,0,0}},{{255,128,0}},{{255,255,0}},{{128,255,0}},{{0,255,0}},{{0,255,128}},{{0,255,255}},{{0,128,255}}}};
   const auto color=colors.at(random.integer_bits(11,precision,true));const auto speed=m.store(m.add(m.mul(random.real_number(constant(0x6d1af8),precision),m.c(0x6d1950)),m.i(1)));
   emit(20,3,color,speed,0,0,1);b.word(0,0);
  }return;
 }
 if(behavior==46||behavior==47){
  const auto age=b.word(0xc);auto segment=signed_word(stage3_boss_final_floor(m.store(m.div(m.i(age),m.c(0x6d1910)))));
  const auto position=m.store(m.add(m.i(static_cast<std::uint32_t>(signed_word(age)%5)),m.q(b.quad(0xa0))));
  std::int64_t next=segment+1;if(behavior==47){if(segment==3)next=7;else if(segment>3){segment+=3;next=segment+1;}}
  const auto lerp=[&](std::int64_t start){return m.store(m.add(m.mul(m.div(m.sub(m.q(r(start+next)),m.q(r(start+segment))),m.c(0x6d1910)),m.q(position)),m.q(r(start+segment))));};
  b.quad(0x40,lerp(10));b.quad(0x48,lerp(20));const auto dx=m.store(m.sub(m.q(r(10+next)),m.q(r(10+segment))));const auto dy=m.store(m.sub(m.q(r(20+next)),m.q(r(20+segment))));b.quad(0x60,m.store(math.atan2(dy,dx)));
  if(b.word(0xc)==29)b.word(0,0);return;
 }
 if(behavior==48){
  const bool low=signed_word(p.difficulty)<=1;
  const auto angle=[&](){if(low)return r(0);const auto a=m.mul(m.i(b.word(0x78)),m.q(r(0)));return m.store(m.add(a,a));};
  auto base=m.store(m.add(m.c(0x6d1a60),m.q(r(2))));const auto cosine=math.cosine(angle());
  auto center=m.store(m.add(m.mul(m.mul(m.i(b.word(0x78)),m.q(r(1))),cosine),m.q(base)));const auto outer_cosine=math.cosine(m.store(m.add(m.q(b.quad(0xa0)),m.q(r(10)))));
  b.quad(0x40,m.store(m.add(m.mul(m.add(m.q(b.quad(0xa8)),m.q(r(12))),outer_cosine),m.q(center))));
  base=m.store(m.add(m.c(0x6d1af0),m.q(r(3))));const auto sine=math.sine(angle());auto radius=m.mul(m.i(b.word(0x78)),m.q(r(1)));if(low)radius=m.mul(radius,m.c(0x6a7888));
  center=m.store(m.add(m.mul(radius,sine),m.q(base)));const auto outer_sine=math.sine(m.store(m.add(m.q(b.quad(0xa0)),m.q(r(10)))));
  b.quad(0x48,m.store(m.add(m.mul(m.add(m.q(b.quad(0xa8)),m.q(r(12))),outer_sine),m.q(center))));return;
 }
 if(behavior==49){
  const auto first=m.store(m.add(m.mul(math.sine(b.quad(0xa0)),m.q(b.quad(0xc8))),m.q(b.quad(0xc0))));
  const auto second=math.sine(b.quad(0xb0));b.quad(0x48,m.store(m.add(m.add(m.mul(second,m.q(b.quad(0xc8))),m.q(first)),m.q(r(0)))));
  b.quad(0xa0,m.store(m.add(m.q(b.quad(0xa0)),m.q(b.quad(0xa8)))));b.quad(0xb0,m.store(m.add(m.q(b.quad(0xb0)),m.q(b.quad(0xb8)))));return;
 }
 throw std::out_of_range("Stage3 final projectile outside 45..50");
}
}
