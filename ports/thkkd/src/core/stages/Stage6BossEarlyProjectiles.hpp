// 六面关底普通弹93..99；每个原浮点存回与服务后读取分别保留。
#pragma once
#include "core/stages/Stage6BossEarlyArithmetic.hpp"
#include "core/stages/Stage6BossEarlyEmitter.hpp"
namespace kaikeidou {
inline void update_stage6_boss_early_projectile(ProjectilePools& pool,std::uint32_t index,std::uint32_t behavior,const ObjectRecord<240>& player,
 PatternParameters& p,const std::uint32_t& integer5,Random& random,RandomPrecision precision,GameMath& math){
 using namespace stage6_boss_early_detail;const Arithmetic m{static_cast<unsigned>(precision)};PoolRecord b{pool,0xa8e660u+index*240u};
 const auto q=[&](std::size_t off){return m.q(b.quad(off));};const auto put=[&](std::size_t off,binary_float::Number value){b.quad(off,m.store(value));};const auto value=[&](std::size_t off){return crt_sse2::real(b.quad(off));};
 const auto selector=[&](){return stage6_boss_early_pick_real(p.difficulty,{{constant(0x6d19d8),constant(0x6d1880),constant(0x6d18f0),0x3ff0000000000000ull,0,0}});};
 const auto random_shot=[&](bool green){
  const auto position_angle=m.store(m.mul(random.real_number(constant(0x6d1c60),precision),m.c(0x6d1950)));
  const auto direction=m.store(m.mul(random.real_number(constant(0x6d1c60),precision),m.c(0x6d1950)));
  const auto radius=m.store(m.mul(random.real_number(constant(0x6d2188),precision),m.c(0x6a7708)));
  const bool blue=!green&&b.word(0x78)==1;const auto speed=p.reals[0];
  const auto sine=math.sine(position_angle);const auto y=m.store(m.add(m.mul(sine,m.q(radius)),q(0x48)));
  const auto cosine=math.cosine(position_angle);const auto x=m.store(m.add(m.mul(cosine,m.q(radius)),q(0x40)));
  stage6_boss_early_emit(pool,{{9,0,{{green?32u:blue?32u:255u,green?255u:32u,blue?255u:32u}},2,255,1,41},x,y,1,0,speed,direction,1,2},player,p,random,precision,math);
 };
 if(behavior==93){
  const auto cosine=math.cosine(b.quad(0xa0));put(0x40,m.add(m.mul(cosine,q(0xa8)),q(0x40)));
  const auto sine=math.sine(b.quad(0xa0));put(0x48,m.add(m.mul(sine,q(0xa8)),q(0x48)));put(0xa0,m.add(q(0xa0),q(0xb0)));
  if(signed_word(b.word(12))<=60){put(0xa8,m.mul(q(0xa8),m.c(0x6d1b08)));put(0xb0,m.mul(q(0xb0),m.c(0x6d1bd0)));}
  else{stage6_boss_early_ease(b,0xa8,constant(0x6d1c18),constant(0x6d1ae8),m.precision);put(0xb0,m.mul(q(0xb0),m.c(0x6d1bd0)));}
  if(remainder(b.word(12),p.integers[2])==0)random_shot(false);if(remainder(b.word(12),p.integers[4])==0)random_shot(false);
  if(value(0x40)<-80.||value(0x40)>464.||value(0x48)<-80.||value(0x48)>528.)b.word(0,0);return;
 }
 if(behavior==94){
  if(signed_word(b.word(12))<=60){const auto cosine=math.cosine(b.quad(0xc0));put(0x40,m.add(m.mul(cosine,q(0xa0)),m.c(0x6d1a60)));const auto sine=math.sine(b.quad(0xc0));put(0x48,m.add(m.mul(sine,q(0xa0)),m.c(0x6d1af0)));stage6_boss_early_ease(b,0xa0,b.quad(0xa8),constant(0x6d1a40),m.precision);}
  else{const auto cosine=math.cosine(b.quad(0xc0));put(0x40,m.add(m.mul(cosine,q(0xa8)),m.c(0x6d1a60)));const auto sine=math.sine(b.quad(0xc0));put(0x48,m.add(m.mul(sine,q(0xa8)),m.c(0x6d1af0)));put(0xc0,m.add(q(0xc0),q(0xb0)));stage6_boss_early_ease(b,0xb0,b.quad(0xb8),constant(0x6d1ae8),m.precision);}
  if(remainder(b.word(12),p.integers[2])==0)random_shot(false);if(remainder(b.word(12),p.integers[3])==0)random_shot(false);return;
 }
 if(behavior==95){
  if(signed_word(b.word(12))<=59){stage6_boss_early_ease(b,0x40,b.quad(0xa0),constant(0x6d1a40),m.precision);stage6_boss_early_ease(b,0x48,b.quad(0xa8),constant(0x6d1a40),m.precision);return;}
  if(remainder(b.word(12),p.integers[2])==0||remainder(b.word(12),p.integers[3])==0){const bool blue=b.word(0x78)==0;const auto angle=b.quad(0xb0),speed=b.quad(0xc0),y=b.quad(0x48),x=b.quad(0x40);stage6_boss_early_emit(pool,{{9,0,{{blue?32u:255u,32,blue?255u:32u}},2,255,1,41},x,y,1,0,speed,angle,1,96},player,p,random,precision,math);}
  put(0xb0,m.add(q(0xb0),q(0xb8)));if(b.word(12)!=200)return;
  const auto offset=constant(random.integer_bits(1,precision,true)?0x6d18f0:0x6d2a50);
  for(std::uint32_t i=0;signed_word(i)<=signed_word(integer5);++i){
   const auto angle=m.store(m.mul(random.real_number(constant(0x6d1c60),precision),m.c(0x6d1950)));const bool blue=b.word(0x78)==0;
   const auto direction=m.store(m.add(m.q(angle),m.q(offset)));const auto selected=selector();const auto speed=m.store(m.mul(m.q(selected),m.q(p.reals[0])));
   const auto sine=math.sine(angle);const auto y=m.store(m.add(m.mul(sine,m.c(0x6d1ae8)),q(0x48)));const auto cosine=math.cosine(angle);const auto x=m.store(m.add(m.mul(cosine,m.c(0x6d1ae8)),q(0x40)));
   stage6_boss_early_emit(pool,{{9,0,{{blue?32u:255u,32,blue?255u:32u}},2,255,1,41},x,y,1,0,speed,direction,1,2},player,p,random,precision,math);
  }
  if(p.difficulty==2||p.difficulty==3){const bool third=p.difficulty==3;const bool blue=b.word(0x78)==0;const auto y=b.quad(0x48),x=b.quad(0x40);stage6_boss_early_emit(pool,{{1,0,{{blue?0u:255u,0,blue?255u:0u}},2,255,1,41},x,y,third?13u:5u,constant(third?0x6d31a8:0x6d2290),0x3ff0000000000000ull,0,0,2},player,p,random,precision,math);}
  stage6_boss_early_remove100(pool,index);return;
 }
 if(behavior==96){
  if(signed_word(b.word(12))>=signed_word(b.word(0x7c))){put(0x40,m.add(q(0x40),q(0x50)));put(0x48,m.add(q(0x48),q(0x58)));}
  else if(signed_word(b.word(12))>=signed_word(b.word(0x78))){
   if(b.word(12)==b.word(0x78)){const auto x=selector();put(0x50,m.mul(m.q(x),q(0x50)));const auto y=selector();put(0x58,m.mul(m.q(y),q(0x58)));}
   const auto factor=m.store(m.div(m.i(b.word(12)-b.word(0x78)),m.i(b.word(0x7c)-b.word(0x78))));put(0x40,m.add(m.mul(q(0x50),m.q(factor)),q(0x40)));put(0x48,m.add(m.mul(q(0x58),m.q(factor)),q(0x48)));
  }else{put(0x40,m.add(m.mul(q(0x50),q(0xa0)),q(0x40)));put(0x48,m.add(m.mul(q(0x58),q(0xa0)),q(0x48)));put(0xa0,m.mul(q(0xa0),m.q(p.reals[1])));}
  if(value(0x40)<-8.||value(0x48)<-8.||value(0x40)>392.||value(0x48)>456.)b.word(0,0);return;
 }
 if(behavior==97){
  const auto x_angle=m.store(m.add(m.q(p.reals[1]),q(0xa8)));const auto cosine=math.cosine(x_angle);put(0x40,m.add(m.mul(cosine,m.c(0x6d1f08)),m.c(0x6d1a60)));
  const auto base=m.add(m.q(p.reals[1]),q(0xa8));const auto y_angle=m.store(m.add(base,base));const auto sine=math.sine(y_angle);put(0x48,m.add(m.mul(sine,m.c(0x6d1a90)),m.c(0x6d21f0)));
  if(remainder(b.word(12),p.integers[2])==0)random_shot(false);if(remainder(b.word(12),p.integers[3])==0)random_shot(false);return;
 }
 if(behavior==98){
  const auto dx=m.store(m.sub(m.q(player.quad(0x90)),q(0x40))),dy=m.store(m.sub(m.q(player.quad(0x98)),q(0x48)));const auto angle=m.store(math.atan2(dy,dx));
  const auto distance2=m.store(m.add(m.mul(m.q(dx),m.q(dx)),m.mul(m.q(dy),m.q(dy))));const auto root=stage6_boss_early_sqrt(distance2,m.precision);const auto speed=m.store(m.mul(root,m.c(0x6d21e0)));
  const auto cosine=math.cosine(angle);put(0x40,m.add(m.mul(cosine,m.q(speed)),q(0x40)));const auto sine=math.sine(angle);put(0x48,m.add(m.mul(sine,m.q(speed)),q(0x48)));
  if(p.difficulty==0)return;if(remainder(b.word(12),p.integers[2])==0)random_shot(true);if(remainder(b.word(12),p.integers[3])==0)random_shot(true);return;
 }
 if(behavior==99){stage6_boss_early_ease(b,0x40,b.quad(0xa0),b.quad(0xa8),m.precision);if(compare(m.i(b.word(12)),q(0xb0))==0)stage6_boss_early_remove100(pool,index);const auto absolute=stage6_boss_early_absolute(m.store(m.sub(q(0x40),q(0xa0))));if(crt_sse2::real(absolute)<=30.)stage6_boss_early_recolor(pool,index,255,0,0);return;}
 throw std::out_of_range("Stage6 boss early projectile outside93..99");
}
}
