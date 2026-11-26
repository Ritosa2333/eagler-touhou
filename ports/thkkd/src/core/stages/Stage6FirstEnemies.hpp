// 原六面首段敌99..103；两路发射保留独立随机、参数与玩家坐标重读。
#pragma once
#include "core/stages/Stage6FirstArithmetic.hpp"
#include "core/stages/Stage6FirstEmitter.hpp"
#include "core/entities/BossStageContext.hpp"
namespace kaikeidou {
inline void update_stage6_first_enemy(std::uint32_t behavior,Enemy& e,ProjectilePools& pool,const ObjectRecord<240>& player,
 PatternParameters& p,const BossStageContext& context,Random& random,RandomPrecision precision,GameMath& math){
 using namespace stage6_first_detail;const Arithmetic m{static_cast<unsigned>(precision)};const auto q=[&](std::size_t off){return m.q(e.quad(off));};const auto put=[&](std::size_t off,binary_float::Number v){e.quad(off,m.store(v));};const auto value=[&](std::size_t off){return crt_sse2::real(e.quad(off));};
 const auto emit=[&](std::uint32_t kind,std::array<std::uint32_t,3> color,std::uint32_t count,std::uint64_t spacing,std::uint64_t speed,std::uint64_t angle,std::uint32_t absolute,std::uint32_t configuration){const auto y=e.quad(0x40),x=e.quad(0x38);stage6_first_emit(pool,{{kind,0,color,0,255,1,41},x,y,count,spacing,speed,angle,absolute,configuration},player,p,random,precision,math);};
 if(behavior==99){
  if(signed_word(e.word(12))<=119){stage6_first_ease(e,0x38,constant(0x6d1a60),constant(0x6d1a40),m.precision);const auto divisor=m.store(m.i(120u-e.word(12)));stage6_first_ease(e,0x40,constant(0x6d1af0),divisor,m.precision);return;}
  if(signed_word(e.word(12))>=1600){put(0x38,m.sub(q(0x38),m.c(0x6d1c90)));const auto target=m.store(m.add(q(0x40),m.c(0x6d1898)));stage6_first_ease(e,0x40,target,constant(0x6d1aa8),m.precision);if(value(0x38)< -20.)e.word(0,0);return;}
  if(remainder(e.word(12),120)==0||remainder(e.word(12),120)==10||remainder(e.word(12),120)==20||remainder(e.word(12),120)==30){
   if(remainder(e.word(12),120)==0){put(0xa8,m.mul(random.real_number(constant(0x6d1c60),precision),m.c(0x6d1950)));put(0xb0,m.mul(q(0xb0),m.c(0x6d2228)));}
   put(0xa8,m.add(q(0xa8),q(0xb0)));
   for(std::uint32_t i=0;signed_word(i)<signed_word(e.word(0x80));++i){const auto slot=allocate_projectile(pool,{1,0,{{64,64,255}},0,255,1,41});if(slot==-1)continue;auto& b=pool.bullets[static_cast<std::size_t>(slot)];b.quad(0xa0,e.quad(0x38));b.quad(0xa8,e.quad(0x40));b.word(0x78,60);b.word(0x7c,40);b.quad(0xb0,m.store(m.add(m.mul(m.div(m.c(0x6d2208),m.i(e.word(0x80))),m.i(i)),q(0xa8))));b.quad(0xb8,constant(0x6d1d18));b.quad(0xc0,constant(0x6d2220));b.quad(0xc8,0);b.word(8,85);const auto cosine=math.cosine(b.quad(0xb0));b.quad(0x40,m.store(m.add(m.mul(cosine,m.q(b.quad(0xb8))),m.q(b.quad(0xa0)))));const auto sine=math.sine(b.quad(0xb0));b.quad(0x48,m.store(m.add(m.mul(sine,m.q(b.quad(0xb8))),m.q(b.quad(0xa8)))));}
  }return;
 }
 if(behavior==100){put(0x38,m.add(q(0x38),q(0xa8)));if(signed_word(e.word(12))<=20)put(0xa8,m.mul(q(0xa8),m.c(0x6d1b10)));if(e.word(12)==30){emit(10,{{96,96,255}},p.integers[2],context.extra_reals[0],p.reals[1],0,0,2);const auto speed=m.store(m.sub(m.q(p.reals[1]),m.c(0x6d1900)));const auto spacing=context.extra_reals[0]^0x8000000000000000ull;emit(10,{{96,96,255}},p.integers[3],spacing,speed,0,0,2);}if(value(0x38)< -20.||value(0x38)>404.)e.word(0,0);return;}
 if(behavior==101){put(0x40,m.add(q(0x40),q(0xa8)));put(0xa8,m.sub(q(0xa8),q(0xb0)));if(value(0xa8)<=0.&&e.word(0x80)==0){e.word(0x80,1);
  if(e.word(0x84)==0){if(p.difficulty==3)emit(1,{{0,0,255}},1,0,constant(0x6d1868),0,0,0);emit(1,{{64,0,255}},1,0,constant(0x6d1ad0),0,0,0);emit(1,{{128,0,255}},1,0,constant(0x6d1948),0,0,0);if(signed_word(p.difficulty)>=1){emit(1,{{192,0,255}},1,0,constant(0x6d1b18),0,0,0);if(signed_word(p.difficulty)>=2)emit(1,{{255,0,255}},1,0,constant(0x6d1ad8),0,0,0);}}
  else{if(signed_word(p.difficulty)>=1){if(signed_word(p.difficulty)>=2){if(p.difficulty==3)emit(20,{{0,255,255}},2,constant(0x6d1a00),constant(0x6d2a08),0,0,0);emit(20,{{0,192,255}},2,constant(0x6d18f0),constant(0x6d2a00),0,0,0);}emit(20,{{0,128,255}},2,constant(0x6d18b0),constant(0x6d29f8),0,0,0);}emit(20,{{0,64,255}},2,constant(0x6d1920),constant(0x6d29f0),0,0,0);emit(20,{{0,0,255}},1,0,constant(0x6d1b18),0,0,0);}
 }if(value(0x40)< -20.)e.word(0,0);return;}
 if(behavior==102||behavior==103){const bool right=behavior==103;put(0x38,right?m.add(q(0x38),q(0xa8)):m.sub(q(0x38),q(0xa8)));if(remainder(e.word(12),e.word(0x80))==0){const std::array<std::uint32_t,3> color=right?std::array<std::uint32_t,3>{{255,128,128}}:std::array<std::uint32_t,3>{{128,128,255}};
   auto a=m.div(random.real_number(constant(0x6d2168),precision),m.c(0x6d1e50));auto angle=m.store(right?m.sub(m.c(0x6d29d8),a):m.add(a,m.c(0x6d29e8)));emit(0,color,1,0,p.reals[0],angle,1,0);
   a=m.div(random.real_number(constant(0x6d2168),precision),m.c(0x6d1e50));angle=m.store(right?m.add(a,m.c(0x6d29d0)):m.sub(m.c(0x6d29e0),a));emit(0,color,1,0,p.reals[0],angle,1,0);
  }if(right?value(0x38)>404.:value(0x38)< -20.)e.word(0,0);return;}
 throw std::out_of_range("Stage6 first enemy outside99..103");
}
}
