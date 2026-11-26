// 五面关底敌94..98；字段与随机服务顺序按原指令保留。
#pragma once
#include "core/stages/Stage5BossFinalArithmetic.hpp"
#include "core/stages/Stage5BossFinalEmitter.hpp"
#include "core/stages/Stage5BossFinalRecords.hpp"
namespace kaikeidou {
inline void update_stage5_boss_final_enemy(std::uint32_t behavior,Enemy& e,ProjectilePools& pool,const ObjectRecord<240>& player,
 PatternParameters& p,BossStageContext& context,Random& random,RandomPrecision precision,GameMath& math){
 using namespace stage5_boss_final_detail;const Arithmetic m{static_cast<unsigned>(precision)};
 const auto q=[&](std::size_t off){return m.q(e.quad(off));};const auto put=[&](std::size_t off,binary_float::Number v){e.quad(off,m.store(v));};const auto value=[&](std::size_t off){return crt_sse2::real(e.quad(off));};
 const auto emit=[&](std::uint32_t kind,std::array<std::uint32_t,3> color,std::uint32_t count,std::uint64_t spacing,std::uint64_t speed,std::uint64_t angle,std::uint32_t absolute,std::uint32_t delayed,std::uint64_t x,std::uint64_t y){stage5_boss_final_emit(pool,{{kind,0,color,0,255,delayed,41},x,y,count,spacing,speed,angle,absolute,0},player,p,random,precision,math);};
 if(behavior==94){
  auto sine=math.sine(e.quad(0xb8));put(0x38,m.add(m.mul(sine,m.c(0x6d2240)),m.c(0x6d1a60)));
  sine=math.sine(e.quad(0xb0));put(0x40,m.add(m.mul(sine,m.c(0x6d1a90)),m.c(0x6b25b8)));
  put(0xa8,m.add(q(0xa8),m.c(0x6d1d28)));sine=math.sine(e.quad(0xa8));put(0xb8,m.add(m.add(m.mul(sine,m.c(0x6d2238)),m.c(0x6d21e0)),q(0xb8)));put(0xb0,m.add(q(0xb0),m.c(0x6d1ab8)));
  if(remainder(e.word(12),24)==0){const auto angle=m.store(m.mul(random.real_number(constant(0x6d1c60),precision),m.c(0x6d1950)));
   for(unsigned side=0;side<2;++side){const auto slot=allocate_projectile(pool,{0,0,{{128,255,128}},0,255,0,41});if(slot==-1)continue;auto& b=pool.bullets[static_cast<std::size_t>(slot)];b.quad(0xa0,side?m.store(m.add(m.q(angle),m.c(0x6d1860))):angle);b.quad(0xa8,m.store(m.add(random.real_number(constant(0x6d1998),precision),m.c(0x6d19a8))));sine=math.sine(b.quad(0xa0));b.quad(0x40,m.store(m.mul(sine,m.c(0x6d1878))));b.quad(0x48,constant(0x6d1a30));b.word(8,79);}
  }return;
 }
 if(behavior==95){
  auto direction=math.sine(e.quad(0xa8));auto angle_value=m.mul(direction,m.c(0x6d1860));const auto angle=m.store(m.add(angle_value,angle_value));direction=math.sine(angle);put(0x38,m.add(m.mul(direction,m.c(0x6d1858)),m.c(0x6d1a60)));direction=math.cosine(angle);put(0x40,m.sub(m.c(0x6d1af0),m.mul(direction,m.c(0x6d1858))));put(0xa8,m.add(q(0xa8),m.c(0x6d2230)));
  if(remainder(e.word(12),3)==0){const auto slot=allocate_projectile(pool,{3,2,{{255,255,0}},0,255,1,41});if(slot!=-1){auto& b=pool.bullets[static_cast<std::size_t>(slot)];b.quad(0x40,e.quad(0x38));b.quad(0x48,e.quad(0x40));const auto dx=m.store(m.sub(m.c(0x6d1a60),m.q(b.quad(0x40)))),dy=m.store(m.sub(m.c(0x6d1af0),m.q(b.quad(0x48))));const auto target=m.store(math.atan2(dy,dx));direction=math.cosine(target);b.quad(0x50,m.store(m.mul(direction,m.c(0x6d1a00))));direction=math.sine(target);b.quad(0x58,m.store(m.mul(direction,m.c(0x6d1a00))));b.word(0x78,120);b.word(0x7c,180);b.word(8,81);}}return;
 }
 if(behavior==96){
  auto direction=math.cosine(e.quad(0xa8));auto base=m.store(m.add(m.mul(direction,m.add(m.q(p.reals[0]),m.q(p.reals[1]))),m.c(0x6d1a60)));auto angle=m.store(m.mul(m.div(m.add(m.q(p.reals[0]),m.q(p.reals[1])),m.q(p.reals[1])),q(0xb0)));direction=math.cosine(angle);put(0x38,m.sub(m.q(base),m.mul(direction,m.q(context.extra_reals[0]))));
  direction=math.sine(e.quad(0xa8));base=m.store(m.add(m.mul(direction,m.add(m.q(p.reals[0]),m.q(p.reals[1]))),m.c(0x6d1de8)));angle=m.store(m.mul(m.div(m.add(m.q(p.reals[0]),m.q(p.reals[1])),m.q(p.reals[1])),q(0xb0)));direction=math.sine(angle);put(0x40,m.sub(m.q(base),m.mul(direction,m.q(context.extra_reals[0]))));
  put(0xa8,m.add(m.mul(m.c(0x6d2a58),q(0xb8)),q(0xa8)));put(0xb0,m.add(m.mul(m.c(0x6d2128),q(0xb8)),q(0xb0)));if(value(0xb8)<1.)put(0xb8,m.add(q(0xb8),m.c(0x6d2358)));
  if(remainder(e.word(12),3)==0){const auto slot=allocate_projectile(pool,{12,0,{{255,64,64}},0,255,1,41});if(slot!=-1){auto& b=pool.bullets[static_cast<std::size_t>(slot)];b.quad(0x40,e.quad(0x38));b.quad(0x48,e.quad(0x40));direction=math.cosine(context.extra_reals[1]);b.quad(0x50,m.store(m.mul(direction,m.c(0x6d2660))));direction=math.sine(context.extra_reals[1]);b.quad(0x58,m.store(m.mul(direction,m.c(0x6d2660))));b.quad(0x60,context.extra_reals[1]);b.word(0x78,500);b.word(0x7c,540);b.word(8,81);}}
  context.extra_reals[1]=m.store(m.sub(m.q(context.extra_reals[1]),m.c(0x6d1e00)));
  if(signed_word(e.word(12))>=300&&remainder(e.word(12),40)==0)emit(10,{{0,200,30}},5,constant(0x6d2618),constant(0x6d1948),0,0,1,constant(0x6d1a60),constant(0x6d1de8));return;
 }
 if(behavior==97){
  const auto random_shot=[&](std::uint32_t maximum,std::uint32_t bias,std::array<std::uint32_t,3> color,std::uint32_t absolute){const auto angle=m.store(m.add(m.mul(random.real_number(constant(maximum),precision),m.c(0x6a7708)),m.c(bias)));const auto speed=m.store(m.add(m.mul(random.real_number(constant(0x6d1ab0),precision),m.c(0x6d1950)),m.c(0x6d1868)));const auto y=e.quad(0x40),x=e.quad(0x38);emit(4,color,1,constant(0x6d1900),speed,angle,absolute,1,x,y);};
  if(e.word(0x80)==0){put(0x40,m.sub(q(0x40),q(0xb0)));put(0xb0,m.add(q(0xb0),m.c(0x6d1a18)));random_shot(0x6d1ab0,0x6aca58,{{0,0,255}},0);random_shot(0x6d18c0,0x6d1ee8,{{0,0,255}},0);if(value(0x40)< -150.){e.word(0x80,1);e.quad(0x38,constant(0x6d1db8));e.quad(0x40,constant(0x6d1a40));e.quad(0xa8,constant(0x6d1b18));e.quad(0xb0,0);}return;}
  if(e.word(0x80)==1||e.word(0x80)==2){const bool left=e.word(0x80)==1;put(0x38,left?m.sub(q(0x38),q(0xa8)):m.add(q(0x38),q(0xa8)));put(0x40,m.add(q(0x40),q(0xb0)));put(0xa8,m.add(q(0xa8),m.c(0x6d2510)));put(0xb0,m.add(q(0xb0),m.c(0x6d1918)));const std::array<std::uint32_t,3> color=left?std::array<std::uint32_t,3>{{0,255,0}}:std::array<std::uint32_t,3>{{255,0,0}};
   random_shot(0x6d1858,left?0x6d2a50:0x6d2a30,color,1);if(remainder(p.game_frame,4)==0)random_shot(0x6d2a48,left?0x6d2a40:0x6d2a28,color,1);else if(remainder(p.game_frame,4)==2)random_shot(0x6d1f08,left?0x6d2a38:0x6d2a20,color,1);
   if(left&&value(0x38)< -150.){e.word(0x80,2);e.quad(0x38,constant(0x6d1d40));e.quad(0x40,constant(0x6d1a40));e.quad(0xa8,constant(0x6d1b18));e.quad(0xb0,0);}
   else if(!left&&value(0x38)>534.){e.word(0x80,3);e.quad(0x38,constant(0x6d1a60));e.quad(0x40,constant(0x6d1d40));e.quad(0xa8,0);e.quad(0xb0,constant(0x6d1898));emit(1,{{255,255,0}},25,constant(0x6d1bb0),constant(0x6d2a18),constant(0x6d1b60),1,0,constant(0x6d1a60),constant(0x6d1a80));emit(1,{{255,255,0}},26,constant(0x6d24f8),constant(0x6d2a10),constant(0x6d1b60),1,0,constant(0x6d1a60),constant(0x6d19a8));}return;
  }
  if(e.word(0x80)==3){put(0x40,m.add(q(0x40),q(0xb0)));put(0xb0,m.mul(q(0xb0),m.c(0x6d1b10)));if(value(0xb0)<=.02){e.word(0x80,0);e.quad(0xa8,0);e.quad(0xb0,0);}}return;
 }
 if(behavior==98){
  const auto phase=e.word(0x80);
  const auto approach=[&](std::uint64_t target,std::uint32_t divisor){stage5_boss_final_ease(e,0x38,target,constant(divisor),m.precision);};
  const auto signal=[&](std::uint32_t column){PoolRecord target{pool,0xa8ce60u+(e.word(0x84)*4u+column)*192u};target.word(0x48,2);};
  const auto create_secondaries=[&](bool reverse){
   for(unsigned row=0;row<4;++row)for(unsigned column=0;column<4;++column){auto& b=pool.secondary[column+row*4];b.quad(0x70,m.store(m.sub(m.q(b.quad(0x70)),m.c(0x6d1db0))));}
   if(signed_word(p.difficulty)<1)return;
   for(unsigned i=0;i<3;++i){if(i==1&&signed_word(p.difficulty)<2)break;if(i==2&&p.difficulty!=3)break;auto& b=pool.secondary[20+i];b.word(0,1);if(i==0)pool.sound_triggers[18]=1;b.quad(0x10,constant(i==0?0x6d1a60:i==1?(reverse?0x6d1d78:0x6d1da0):(reverse?0x6d1878:0x6d18d8)));b.quad(0x18,constant(0x6d1da8));b.quad(0x20,0);b.quad(0x28,constant(0x6d1d18));b.word(4,84);b.word(12,0);b.word(0x48,0);b.quad(0x30,constant(0x6d18f0));b.quad(0x38,constant(0x6d18f0));b.quad(0x40,constant(0x6d18f0));}
  };
  if(phase==0){approach(constant(0x6d1dd0),0x6d1898);if(value(0x38)<= -150.){e.word(0x80,1);e.quad(0x40,constant(0x6d1878));e.quad(0xa8,constant(0x6d1c58));signal(0);}return;}
  if(phase>=1&&phase<=3){approach(e.quad(0xa8),0x6d1898);if(compare(q(0x38),m.sub(q(0xa8),m.c(0x6d1960)))>0){e.word(0x80,phase+1);if(phase<3){e.quad(0xa8,constant(phase==1?0x6d1a60:0x6d1dc8));signal(phase);}}return;}
  if(phase==4){approach(constant(0x6d1dc0),0x6d1898);if(value(0x38)>534.){e.word(0x80,5);e.quad(0x40,constant(0x6d1a50));create_secondaries(false);}return;}
  if(phase==5){approach(constant(0x6d1a60),0x6d1a40);if(value(0x38)<=193.){e.word(0x80,10);e.word(0x84,e.word(0x84)+1u);if(e.word(0x84)==4)e.word(0x84,0);}return;}
  if(phase==10){approach(constant(0x6d1dc0),0x6d1898);if(value(0x38)>534.){e.word(0x80,11);e.quad(0x40,constant(0x6d1878));e.quad(0xa8,constant(0x6d1d90));signal(3);}return;}
  if(phase>=11&&phase<=14){approach(e.quad(0xa8),0x6d1898);if(compare(q(0x38),m.add(q(0xa8),m.c(0x6d1960)))<=0){e.word(0x80,phase+1);if(phase<14){e.quad(0xa8,constant(phase==11?0x6d1d88:phase==12?0x6d1d80:0x6d1d18));signal(13-phase);}}return;}
  if(phase==15){approach(constant(0x6d1dd0),0x6d1898);if(value(0x38)<= -150.){e.word(0x80,16);e.quad(0x40,constant(0x6d1a50));create_secondaries(true);}return;}
  if(phase==16){approach(constant(0x6d1a60),0x6d1a40);if(value(0x38)>191.){e.word(0x80,0);e.word(0x84,e.word(0x84)+1u);if(e.word(0x84)==4)e.word(0x84,0);}return;}
  return;
 }
 throw std::out_of_range("Stage5 final enemy outside94..98");
}
}
