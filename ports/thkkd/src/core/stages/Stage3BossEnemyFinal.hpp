// 原637400..638E2A：终符敌59，保留共用计时器、20次轨迹随机与两组分配顺序。
#pragma once
#include "core/stages/Stage3BossFinalArithmetic.hpp"
#include "core/entities/EnemyPool.hpp"
#include "core/shared/EffectPool.hpp"
#include "core/entities/ProjectileEmitter.hpp"
namespace kaikeidou {
inline void update_stage3_boss_enemy_final(Enemy& e,ProjectilePools& pool,EffectPool& effects,
    std::uint64_t px,std::uint64_t py,PatternParameters& p,StagePatternContext& extra,Stage3BossTimelineContext& context,
    Random& random,RandomPrecision precision,GameMath& math,BossStageServices& services){
 using namespace stage3_boss_final_detail;const Arithmetic m{static_cast<unsigned>(precision)};
 const auto age=[&](){return signed_word(e.word(0xc));};
 const auto set_real=[&](unsigned i,std::uint64_t v){if(i<20)extra.reals_10_to_19.at(i-10)=v;else context.reals_20_to_29.at(i-20)=v;};
 const auto fire=[&](std::uint32_t count,std::uint32_t spacing,std::uint64_t angle){
  emit_projectiles(pool,{{12,0,{{0,0,180}},0,255,1,41},p.reals[1],constant(0x6d19a0),count,constant(spacing),constant(0x6d1a00),angle,1,0},px,py,p,random,precision,math);
 };
 if(age()<=69){
  e.quad(0x38,m.store(m.add(m.q(e.quad(0x38)),m.q(e.quad(0xa8)))));e.quad(0x40,m.store(m.add(m.q(e.quad(0x40)),m.q(e.quad(0xb0)))));
  if(e.word(0xc)==62){p.reals[1]=m.store(m.i(!(crt_sse2::real(p.reals[1])<=192.)?0u:384u));const auto x=p.reals[1];const auto color=services.color(0,0,255);allocate_effect(effects,pool.sound_triggers,{10,color,x,constant(0x6d19a0)});}
 }else if(e.word(0xc)==100){
  const auto dx=m.store(m.sub(m.q(px),m.q(p.reals[1]))),dy=m.store(m.sub(m.q(py),m.c(0x6d19a0)));e.quad(0xb8,m.store(math.atan2(dy,dx)));fire(1,0x6d1918,e.quad(0xb8));
 }else if(e.word(0xc)==110)fire(2,0x6d1918,e.quad(0xb8));
 else if(e.word(0xc)==120)fire(2,0x6d1810,e.quad(0xb8));
 else if(e.word(0xc)==130)fire(1,0x6d1968,e.quad(0xb8));
 else if(e.word(0xc)==140){
  fire(4,0x6d1810,e.quad(0xb8));
  if(signed_word(p.difficulty)>=2){
   p.integers[0]=1;
   const std::array<std::uint32_t,4> subtracts{{0x6d1a40,0x6d1ae8,0x6d1aa8,0x6d1a90}},maxima{{0x6d1ae8,0x6d1a90,0x6d1d00,0x6d18c0}};
   for(unsigned i=0;i<4;++i){const auto offset=m.store(m.sub(m.q(e.quad(0x38)),m.c(subtracts[i])));const auto value=random.real_number(constant(maxima[i]),precision);set_real(10+i,m.store(m.add(value,m.q(offset))));}
   const std::array<std::uint32_t,3> subtracts2{{0x6b25b8,0x6d1d00,0x6d1f08}},maxima2{{0x6d1ab0,0x6d1d88,0x6d1f60}};
   for(unsigned i=0;i<3;++i){const auto offset=m.store(m.sub(m.div(m.add(m.q(e.quad(0x38)),m.c(0x6d1a60)),m.c(0x6d1948)),m.c(subtracts2[i])));const auto value=random.real_number(constant(maxima2[i]),precision);set_real(14+i,m.store(m.add(value,m.q(offset))));}
   const std::array<std::uint32_t,3> offsets{{0x6d2748,0x6d2468,0x6d25b0}};
   for(unsigned i=0;i<3;++i){const auto value=random.real_number(constant(maxima2[i]),precision);set_real(17+i,m.store(m.add(value,m.c(offsets[i]))));}
   const std::array<std::uint32_t,10> y_offsets{{0x6d1c48,0x6b25f8,0x6d1d00,0x6d1ac8,0x6d1af8,0x6d2ae0,0x6d18c8,0x6d1af8,0x6d2ae0,0x6d18c8}};
   for(unsigned i=0;i<10;++i){const auto value=random.real_number(constant(0x6d1ae8),precision);set_real(20+i,m.store(m.add(value,m.c(y_offsets[i]))));}
  }
 }else if(e.word(0xc)==150)fire(6,0x6d2ad8,e.quad(0xb8));
 else if(e.word(0xc)==160)fire(8,0x6d1918,e.quad(0xb8));
 else if(e.word(0xc)==170||e.word(0xc)==180||e.word(0xc)==190){
  const auto n=e.word(0xc)==170?3u:e.word(0xc)==180?2u:1u;const auto spacing=e.word(0xc)==170?0x6d1810u:0x6d1968u;const auto offset=e.word(0xc)==170?0x6d19b8u:e.word(0xc)==180?0x6d19c8u:0x6d2ad0u;
  fire(n,spacing,e.quad(0xb8));fire(n,spacing,m.store(m.sub(m.q(e.quad(0xb8)),m.c(offset))));fire(n,spacing,m.store(m.add(m.q(e.quad(0xb8)),m.c(offset))));
 }else if(e.word(0xc)==200)fire(2,0x6d1918,e.quad(0xb8));
 else if(e.word(0xc)==210)fire(2,0x6d1c10,e.quad(0xb8));
 else if(e.word(0xc)==220)fire(2,0x6d1fa8,e.quad(0xb8));
 else if(e.word(0xc)==e.word(0x80)){
  e.word(0xc,0xffffffffu);const bool left=crt_sse2::real(e.quad(0x38))<=192.;const auto value=random.real_number(constant(0x6d1aa8),precision);
  const auto target=left?m.sub(m.c(0x6d2980),value):m.add(value,m.c(0x6d2468));e.quad(0xa8,m.store(m.div(m.sub(target,m.q(e.quad(0x38))),m.c(0x6d1d50))));
  const auto y=random.real_number(constant(0x6b25f8),precision);e.quad(0xb0,m.store(m.div(m.sub(m.add(y,m.c(0x6d1f08)),m.q(e.quad(0x40))),m.c(0x6d1d50))));
 }
 if(!remainder(p.game_frame,e.word(0x84))){
  for(unsigned side=0;side<2;++side){
   const auto height_bits=random.integer_bits(side?30u:60u,precision,true)+(side?15u:0u);const auto height=m.store(m.i(height_bits));const auto shade=250u-stage3_boss_final_convert(height)*4u;
   const auto slot=allocate_projectile(pool,{5,2,{{shade,shade,shade}},0,255,0,41});if(slot==-1)continue;auto& b=pool.bullets[static_cast<std::size_t>(slot)];b.word(8,45);
   const auto offset=m.store(m.sub(m.q(e.quad(0x38)),m.c(side?0x6d1ae8:0x6d1a90)));const auto x=random.real_number(constant(side?0x6d1a90:0x6d18c0),precision);b.quad(0x40,m.store(m.add(x,m.q(offset))));b.quad(0x48,height);
   const auto degrees=m.store(random.real_number(constant(0x6d1fc8),precision));const auto angle=stage3_boss_final_degrees(degrees,m.bits);
   const auto cosine=math.cosine(angle);b.quad(0x50,m.store(m.mul(cosine,m.c(side?0x6d1920:0x6d19c0))));const auto sine=math.sine(angle);b.quad(0x58,m.store(m.mul(sine,m.c(side?0x6d1960:0x6d1990))));
  }
 }
 p.reals[0]=m.store(m.add(m.q(p.reals[0]),m.c(0x6d2ac8)));if(signed_word(p.difficulty)<2)return;
 const auto trails=[&](std::uint32_t behavior,bool visible){
  const std::array<std::uint64_t,3> offsets{{0,constant(0x6d2ac0),constant(0x6d2ab8)}};
  for(unsigned i=0;i<(visible?6u:3u);++i){const auto slot=allocate_projectile(pool,{visible?(i<3?2u:1u):0u,visible||i==0?1:0,{{255,255,200}},2,visible?255u:128u,0,41});if(slot==-1)continue;auto& b=pool.bullets[static_cast<std::size_t>(slot)];b.word(8,behavior);b.quad(0xa0,offsets[i%3]);if(!visible)b.word(0x3c,1);}
 };
 if(age()>=180&&age()<=209&&age()%2==0)trails(46,false);
 else if(p.difficulty==3&&age()>=220&&age()<=249&&age()%2==0)trails(47,false);
 else if(p.integers[0]==1&&age()>=30&&age()<=49&&age()%2==0){if(e.word(0xc)==30){e.word(0x2c,3);e.word(0x30,0);e.word(0x34,0);}trails(46,true);}
 else if(p.difficulty==3&&p.integers[0]==1&&age()>=70&&age()<=89&&age()%2==0){if(e.word(0xc)==70){e.word(0x2c,3);e.word(0x30,0);e.word(0x34,0);}trails(47,true);}
}
}
