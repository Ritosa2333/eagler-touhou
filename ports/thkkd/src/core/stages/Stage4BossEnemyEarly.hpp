// 原敌73..76；保留随机取得位置、分配失败、逐次除法与阶段公共尾部。
#pragma once
#include "core/stages/Stage4BossEarlyArithmetic.hpp"
#include "core/entities/EnemyPool.hpp"
#include "core/entities/BossStageContext.hpp"
#include "core/entities/BossAllocationContext.hpp"
namespace kaikeidou {
inline void update_stage4_boss_enemy_early(std::uint32_t behavior,Enemy& e,ProjectilePools& pool,std::uint64_t px,std::uint64_t py,
 PatternParameters& p,const BossStageContext& boss,const StagePatternContext& extra,const Stage4PatternContext& fourth,const std::uint32_t& integer5,
 BossAllocationContext& allocation,Random& random,RandomPrecision precision,GameMath& math){
 using namespace stage4_boss_early_detail;const Arithmetic m{{static_cast<unsigned>(precision)}};const auto q=[&](std::size_t o){return m.q(e.quad(o));};const auto put=[&](std::size_t o,binary_float::Number v){e.quad(o,m.store(v));};
 const auto make=[&](std::uint32_t group){return allocate_projectile(pool,{21,0,color(p,extra,fourth,integer5,group),0,255,1,41});};
 const auto vertical=[&](std::uint64_t speed){return crt_sse2::real(e.quad(0x40))<=crt_sse2::real(py)?speed:speed^0x8000000000000000ull;};
 if(behavior==73){const auto sine=math.sine(e.quad(0xa8));put(0x38,m.add(m.mul(sine,q(0xb8)),m.c(0x6d1a60)));e.quad(0x40,stage4_boss_early_approach(e.quad(0x40),e.quad(0xb0),e.quad(0xc0),m.precision));put(0xa8,m.add(q(0xa8),m.c(0x6d1e00)));put(0xb8,m.mul(q(0xb8),m.c(0x6d1940)));if(signed_word(allocation.field_a7ea5c)<=254){allocation.field_a7ea5c+=2;if(signed_word(allocation.field_a7ea5c)>=256)allocation.field_a7ea5c=255;}return;}
 if(behavior==74||behavior==75){
  const auto sx=math.sine(e.quad(0xa8));const auto scale=m.mul(sx,m.c(behavior==74?0x6d1b38:0x6d1aa8));const auto first_x=m.store(behavior==74?m.add(scale,m.c(0x6d1a60)):m.sub(m.c(0x6d1a60),scale));const auto tx=math.sine(m.store(m.mul(q(0xb0),m.c(0x6d1920))));const auto x=m.store(m.add(m.mul(tx,m.c(0x6b25f8)),m.q(first_x)));e.quad(0x38,stage4_boss_early_approach(e.quad(0x38),x,constant(0x6d1a88),m.precision));
  const auto sy=math.sine(m.store(m.mul(q(0xa8),m.c(0x6d1900))));const auto yscale=m.mul(sy,m.c(0x6d1a40));const auto first_y=m.store(behavior==74?m.sub(m.c(0x6b25b8),yscale):m.add(yscale,m.c(0x6b25b8)));const auto ty=math.sine(e.quad(0xb0));const auto y=m.store(m.add(m.mul(ty,m.c(0x6d1a40)),m.q(first_y)));e.quad(0x40,stage4_boss_early_approach(e.quad(0x40),y,constant(0x6d1a40),m.precision));put(0xa8,m.add(q(0xa8),m.c(0x6a7708)));put(0xb0,m.add(q(0xb0),m.c(0x6d1aa0)));
 }
 if(behavior==74){
  if(!remainder(e.word(12),e.word(0x80))){auto group=random.integer_bits(1,precision,true);auto start=m.mul(random.real_number(constant(0x6d18c0),precision),m.c(0x6d1960));start.negative=!start.negative;auto x=m.store(start);
   const auto emit=[&](){const auto index=make(group);if(index<0)return;auto& b=pool.bullets[static_cast<std::size_t>(index)];b.quad(0x40,x);b.quad(0x48,e.quad(0x40));b.quad(0x50,0);b.quad(0x58,vertical(constant(0x6d1a00)));b.word(8,52);b.word(0x78,group);};
   while(crt_sse2::real(x)<=400.){if(signed_word(p.difficulty)<=1){emit();x=m.store(m.add(m.q(x),q(0xb8)));emit();group=1-group;}else{emit();group=1-group;}x=m.store(m.add(m.q(x),q(0xb8)));}
  }
 }else if(behavior==75){
  if(signed_word(e.word(12))>=100&&!remainder(e.word(12),e.word(0x80))){const auto index=make(0);if(index>=0){auto& b=pool.bullets[static_cast<std::size_t>(index)];b.quad(0x40,px);b.quad(0x48,e.quad(0x40));b.quad(0x50,0);b.quad(0x58,vertical(e.quad(0xb8)));b.word(8,52);b.word(0x78,0);}}
  for(const auto period:{0x84u,0x88u})if(!remainder(e.word(12),e.word(period))){const auto index=make(1);if(index>=0){auto& b=pool.bullets[static_cast<std::size_t>(index)];const auto x=random.real_number(constant(0x6d2188),precision);b.quad(0x40,m.store(m.add(m.mul(x,m.c(0x6d1960)),m.c(0x6d1a30))));b.quad(0x48,e.quad(0x40));const auto vx=random.real_number(constant(0x6d1ab0),precision);b.quad(0x50,m.store(m.add(m.mul(vx,m.c(0x6d1950)),m.c(0x6d22a0))));b.quad(0x58,vertical(e.quad(0xc0)));b.word(8,52);b.word(0x78,1);}}
 }else if(behavior==76){
  const auto first=remainder(e.word(12),e.word(0x80));const bool second=first!=0&&remainder(e.word(12),e.word(0x80))==6;
  if(!first||second){std::uint64_t angle=0;for(std::uint32_t i=0;signed_word(i)<=signed_word(e.word(0x84));++i){const auto index=make(second?1u:0u);if(index>=0){auto& b=pool.bullets[static_cast<std::size_t>(index)];b.quad(0x40,constant(0x6d1a60));b.quad(0x48,constant(0x6d1af0));b.word(8,54);b.quad(0xa0,m.store(second?m.sub(q(0xb0),m.q(angle)):m.add(q(0xa8),m.q(angle))));b.quad(0xa8,p.reals[second?1:0]);b.quad(0xb0,0);b.quad(0xb8,boss.extra_reals[0]);b.word(0x78,second?1u:0u);}angle=m.store(m.add(m.q(angle),q(0xb8)));}const auto at=second?0xb0u:0xa8u;put(at,m.add(q(at),q(second?0xc8:0xc0)));}
 }else throw std::out_of_range("Stage4 early enemy outside73..76");
 if(!remainder(p.game_frame,100)){p.integers[3]=1-p.integers[3];pool.sound_triggers[44]=1;}
}
}
