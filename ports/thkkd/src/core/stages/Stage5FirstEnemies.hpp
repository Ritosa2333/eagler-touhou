// 原敌79..84；背景、玩家与参数按服务返回后的原顺序重读。
#pragma once
#include "core/stages/Stage5FirstArithmetic.hpp"
#include "core/stages/Stage5FirstEmitter.hpp"
#include "core/stages/StageResourceContext.hpp"
namespace kaikeidou {
inline void update_stage5_first_enemy(std::uint32_t behavior,Enemy& e,ProjectilePools& pool,const ObjectRecord<240>& player,
 PatternParameters& p,const StageResourceContext& background,Random& random,RandomPrecision precision,GameMath& math){
 using namespace stage5_first_detail;const Arithmetic m{static_cast<unsigned>(precision)};const auto c=[](std::uint32_t a){return constant(a);};const auto q=[&](std::size_t o){return m.q(e.quad(o));};const auto put=[&](std::size_t o,binary_float::Number n){e.quad(o,m.store(n));};
 const auto age=[&](){return signed_word(e.word(12));};const auto pick=[&](std::array<std::uint32_t,6> v){return stage5_first_pick_integer(p.difficulty,v);};const auto real=[&](std::array<std::uint64_t,6> v){return stage5_first_pick_real(p.difficulty,v);};
 const auto emit=[&](std::uint32_t kind,std::array<std::uint32_t,3> color,std::uint32_t blend,std::uint32_t count,std::uint64_t spacing,std::uint64_t speed,std::uint64_t angle,std::uint32_t absolute,std::uint32_t config,std::uint32_t delayed){const auto y=e.quad(0x40),x=e.quad(0x38);stage5_first_emit(pool,{{kind,2,color,blend,255,delayed,41},x,y,count,spacing,speed,angle,absolute,config},player,p,random,precision,math);};
 const auto spawn69=[&](std::uint32_t kind,std::array<std::uint32_t,3> color,std::uint32_t last){auto index=allocate_projectile(pool,{kind,1,color,0,255,1,41});if(index!=-1){auto& b=pool.bullets[static_cast<std::size_t>(index)];b.quad(0x40,e.quad(0x38));b.quad(0x48,e.quad(0x40));b.word(8,69);b.word(0x7c,e.word(12)+last);b.word(0x34,1);}};
 if(behavior==79){
  if(age()>=1900){stage5_first_ease(e,0x38,background.reals[5],c(0x6b25f8),m.precision);stage5_first_ease(e,0x40,background.reals[6],c(0x6b25f8),m.precision);if(e.word(12)==1990)e.word(0,0);}
  else if(age()>=50){if(remainder(e.word(12),p.integers[3])==0){auto index=allocate_projectile(pool,{2,1,{{e.word(0x80),e.word(0x84),e.word(0x88)}},2,255,1,41});if(index!=-1){auto& b=pool.bullets[static_cast<std::size_t>(index)];b.quad(0x40,e.quad(0x38));b.quad(0x48,e.quad(0x40));b.word(8,68);b.word(0x78,180);b.word(0x7c,2360);b.quad(0xa0,m.store(m.div(m.sub(m.q(player.quad(0x90)),q(0x38)),m.i(b.word(0x78)))));b.quad(0xa8,m.store(m.div(m.sub(m.q(player.quad(0x98)),q(0x40)),m.i(b.word(0x78)))));b.word(0x34,1);}}
   if(remainder(e.word(12),p.integers[4])==0&&signed_word(p.difficulty)>=1){const auto spacing=real({{c(0x6d19c0),c(0x6d19c0),c(0x6d18e0),c(0x6d1cf0),0,0}});const auto count=pick({{4,4,5,7,0,0}});emit(10,{{e.word(0x80),e.word(0x84),e.word(0x88)}},0,count,spacing,c(0x6d1b28),0,0,1,1);}
  }else{stage5_first_ease(e,0x38,e.quad(0xa8),c(0x6d1a40),m.precision);stage5_first_ease(e,0x40,e.quad(0xb0),c(0x6d1a40),m.precision);}return;
 }
 if(behavior==80){auto sine=math.sine(e.quad(0xa8));put(0x38,m.add(m.mul(sine,m.c(0x6d1e48)),m.q(background.reals[5])));put(0x40,m.add(m.mul(m.i(e.word(0x84)),m.c(0x6d1880)),q(0x40)));put(0xa8,m.add(m.mul(m.i(e.word(0x84)),m.c(0x6d1fa0)),q(0xa8)));const auto y=crt_sse2::real(e.quad(0x40));if(y< -80.||y>528.)e.word(0,0);else if(remainder(e.word(12),e.word(0x80))==8)spawn69(1,{{0,255,0}},3250);return;}
 if(behavior==81){put(0x38,m.add(q(0x38),q(0xa8)));put(0x40,m.sub(q(0x40),m.c(0x6d2488)));const auto x=crt_sse2::real(e.quad(0x38));if(x< -20.||x>404.){e.word(0,0);return;}if(e.word(12)!=40&&(e.word(12)!=100||signed_word(p.difficulty)<1))return;
  const auto speed=real({{c(0x6d25a8),c(0x6d25a8),c(0x6d2a90),c(0x6d26e0),0,0}});p.reals[0]=c(random.integer_bits(1,precision,true)?0x6d1b60:0x6d1de0);const auto count=pick({{11,11,15,19,0,0}});
  for(std::uint32_t i=0;signed_word(i)<=signed_word(count);++i){p.integers[0]=0u-i;emit(15,{{128,24,8}},2,1,0,speed,0,0,70,0);}p.reals[0]=m.store(m.add(m.q(p.reals[0]),m.c(0x6d1860)));for(std::uint32_t i=0;signed_word(i)<=signed_word(count);++i){p.integers[0]=0u-i;emit(15,{{8,24,128}},2,1,0,speed,0,0,70,0);}return;
 }
 if(behavior==82||behavior==83){put(0x38,m.add(q(0x38),q(0xa8)));put(0x40,m.add(q(0x40),q(0xb0)));if(behavior==82){put(0xa8,m.mul(q(0xa8),m.c(0x6d2298)));put(0xb0,m.mul(q(0xb0),m.c(0x6d2298)));}const auto x=crt_sse2::real(e.quad(0x38)),y=crt_sse2::real(e.quad(0x40));if(x< -80.||x>464.||y< -80.||y>528.)e.word(0,0);else{const auto rem=remainder(e.word(12),e.word(0x80));if(behavior==82?rem==0:static_cast<std::uint32_t>(rem)==e.word(0x84))spawn69(behavior==82?1u:0u,behavior==82?std::array<std::uint32_t,3>{{255,0,0}}:std::array<std::uint32_t,3>{{0,0,255}},behavior==82?4300u:9000u);}return;}
 if(behavior==84){e.quad(0x38,background.reals[5]);e.quad(0x40,background.reals[6]);if(p.game_frame==5540)e.word(0,0);else if(remainder(e.word(12),2)==0){emit(9,{{255,16,16}},2,1,0,c(0x6d1c90),m.store(m.sub(q(0xa8),m.c(0x6d2138))),1,0,0);emit(9,{{16,255,16}},2,1,0,c(0x6d1c90),e.quad(0xa8),1,0,0);emit(9,{{16,16,255}},2,1,0,c(0x6d1c90),m.store(m.add(q(0xa8),m.c(0x6d2138))),1,0,0);put(0xa8,m.add(q(0xa8),m.c(0x6d2510)));}return;}
 throw std::out_of_range("Stage5 first enemy outside 79..84");
}
}
