// 原敌60..67；保留移动后的除法故障、两次独立发射判定与趋近调用落点。
#pragma once
#include "core/stages/Stage4FirstDependenciesArithmetic.hpp"
#include "core/entities/EnemyPool.hpp"
#include "core/entities/ProjectileEmitter.hpp"
namespace kaikeidou {
inline void update_stage4_first_enemy(std::uint32_t behavior,Enemy& e,ProjectilePools& pool,std::uint64_t px,std::uint64_t py,
 PatternParameters& p,const BossStageContext& context,Random& random,RandomPrecision precision,GameMath& math){
 using namespace stage4_first_dependencies_detail;const Arithmetic m{{static_cast<unsigned>(precision)}};
 const auto q=[&](std::size_t o){return m.q(e.quad(o));};const auto put=[&](std::size_t o,binary_float::Number n){e.quad(o,m.store(n));};
 const auto age=[&](){return signed_word(e.word(12));};const auto num=[&](std::array<std::uint32_t,6> v){return stage4_first_pick_integer(p.difficulty,v);};const auto real=[&](std::array<std::uint64_t,6> v){return stage4_first_pick_real(p.difficulty,v);};
 const auto emit=[&](std::uint32_t kind,std::int32_t group,std::array<std::uint32_t,3> color,std::uint32_t blend,std::uint32_t count,std::uint64_t spacing,std::uint64_t speed,std::uint64_t angle,std::uint32_t absolute,std::uint32_t config){emit_projectiles(pool,{{kind,group,color,blend,255,1,41},e.quad(0x38),e.quad(0x40),count,spacing,speed,angle,absolute,config},px,py,p,random,precision,math);};
 const auto cull_x=[&](){const auto x=crt_sse2::real(e.quad(0x38));if(x< -30.||x>414.)e.word(0,0);};
 if(behavior==60){put(0x38,m.add(q(0x38),q(0xa8)));put(0x40,m.add(q(0x40),q(0xb0)));if(!remainder(e.word(12),p.integers[0])){emit(3,0,{{255,0,255}},0,3,constant(0x6d2138),context.extra_reals[0],e.quad(0xb8),1,1);put(0xb8,m.add(q(0xb8),m.c(0x6d1cf0)));}cull_x();return;}
 if(behavior==61){
  const auto cosine=m.store(math.cosine(e.quad(0xb8)));const auto sx=math.sine(e.quad(0xc0));put(0x38,m.add(m.mul(m.mul(sx,m.q(cosine)),m.c(0x6d1d78)),q(0xa8)));
  const auto sine=m.store(math.sine(e.quad(0xb8)));const auto sy=math.sine(e.quad(0xc0));put(0x40,m.add(m.mul(m.mul(sy,m.q(sine)),m.c(0x6d1d78)),q(0xb0)));
  e.quad(0xa8,stage4_first_approach(e.quad(0xa8),px,constant(0x6d1aa8),m.precision));const auto distance=stage4_first_absolute(m.store(m.sub(q(0xa8),m.q(px))));put(0xb8,m.add(m.mul(m.sub(m.c(0x6d1cb0),m.q(distance)),m.c(0x6a7888)),q(0xb8)));put(0xc0,m.add(q(0xc0),m.c(0x6d21c8)));
  if(crt_sse2::real(e.quad(0xa8))>0.&&!remainder(e.word(12),e.word(0x80)))emit(2,0,{{210,16,230}},2,1,0,constant(0x6d1a00),constant(0x6d1b60),1,0);
  if(signed_word(p.game_frame)>=3600){put(0xb0,m.sub(q(0xb0),m.c(0x6d1948)));if(crt_sse2::real(e.quad(0xb0))< -170.)e.word(0,0);}
  else if(crt_sse2::real(e.quad(0xb0))<60.)put(0xb0,m.add(q(0xb0),m.c(0x6d1948)));return;
 }
 if(behavior==62){const auto sine=math.sine(e.quad(0xb0));put(0x38,m.add(m.mul(sine,m.c(0x6d2658)),q(0xa8)));put(0xa8,m.add(q(0xa8),m.c(0x6d1e40)));put(0xb0,m.add(q(0xb0),m.c(0x6d1918)));if(!remainder(e.word(12),p.integers[0])){emit(1,0,{{255,0,0}},0,4,constant(0x6d1860),context.extra_reals[0],e.quad(0xb8),1,1);put(0xb8,m.add(q(0xb8),m.c(0x6d1920)));}if(crt_sse2::real(e.quad(0x38))>424.)e.word(0,0);return;}
 if(behavior==63){put(0x40,m.add(q(0x40),m.c(0x6d1948)));if(!remainder(e.word(12),p.integers[0])&&crt_sse2::real(e.quad(0x40))<268.){const auto count=num({{3,5,7,7,0,0}});emit(10,0,{{16,16,255}},0,count,constant(0x6d1cf0),p.reals[0],0,0,0);}if(crt_sse2::real(e.quad(0x40))>478.)e.word(0,0);return;}
 if(behavior==64){put(0x38,m.add(q(0x38),q(0xa8)));put(0x40,m.add(q(0x40),q(0xb0)));put(0xb0,m.add(q(0xb0),q(0xb8)));if(!remainder(e.word(12),p.integers[2])&&crt_sse2::real(e.quad(0x40))<328.){const auto angle=real({{0,0,constant(0x6d22d8),constant(0x6d22d8),0,0}});const auto speed=p.reals[0];const auto spacing=real({{constant(0x6d1860),constant(0x6d1860),constant(0x6d1b60),constant(0x6d1b60),0,0}});const auto count=num({{2,2,4,4,0,0}});emit(16,0,{{100,200,0}},0,count,spacing,speed,angle,1,2);}cull_x();return;}
 if(behavior==65){
  if(age()<=50){put(0x38,m.sub(q(0x38),q(0xa8)));put(0x40,m.add(q(0x40),m.c(0x6d1b18)));}
  else if(age()<=200){if(e.word(12)==100){e.word(0x18,40);e.word(0x14,40);}if(age()>=100&&!remainder(e.word(12),num({{60,50,35,20,0,0}}))){const auto blue=stage4_first_convert(e.quad(0xc8)),green=stage4_first_convert(e.quad(0xc0)),red=stage4_first_convert(e.quad(0xb8));emit(11,0,{{red,green,blue}},0,5,constant(0x6d2688),constant(0x6d1f50),0,0,2);}}
  else{put(0x38,m.add(q(0x38),q(0xa8)));put(0x40,m.add(q(0x40),q(0xb0)));const auto x=crt_sse2::real(e.quad(0x38)),y=crt_sse2::real(e.quad(0x40));if(x< -30.||x>414.||y< -30.)e.word(0,0);}return;
 }
 if(behavior==66){
  if(age()<=50)put(0x40,m.add(q(0x40),m.c(0x6d1ad8)));
  else if(age()<=990){if(age()>=100){if(!remainder(e.word(12),e.word(0x80))){const auto angle=e.quad(0xa8);const auto speed=real({{constant(0x6d2548),constant(0x6d25a8),constant(0x6d1ad8),constant(0x6d1d60),0,0}});emit(1,1,{{255,0,0}},0,5,constant(0x6d2618),speed,angle,1,0);const auto second_angle=m.store(m.add(q(0xa8),q(0xa8)));const auto second_speed=real({{constant(0x6d2130),constant(0x6d2548),constant(0x6d25a8),constant(0x6d1ad8),0,0}});emit(0,3,{{0,255,255}},0,5,constant(0x6d2618),second_speed,second_angle,1,0);put(0xa8,m.sub(q(0xa8),m.c(0x6d2008)));}if(!remainder(e.word(12),e.word(0x84))){const auto speed=real({{constant(0x6d24c0),constant(0x6d2480),constant(0x6d2a98),constant(0x6d1c18),0,0}});emit(11,2,{{170,0,255}},0,1,0,speed,0,0,0);}}}
  else{put(0x38,m.sub(q(0x38),m.c(0x6d1948)));put(0x40,m.add(q(0x40),m.c(0x6d1880)));if(crt_sse2::real(e.quad(0x38))< -30.)e.word(0,0);}return;
 }
 if(behavior==67){
  if(age()<=99){e.quad(0x38,stage4_first_approach(e.quad(0x38),constant(0x6d1a60),constant(0x6d1a40),m.precision));e.quad(0x40,stage4_first_approach(e.quad(0x40),constant(0x6d1a90),constant(0x6b25f8),m.precision));}
  else if(age()<=1200){e.quad(0x38,stage4_first_approach(e.quad(0x38),px,constant(0x6d1ae8),m.precision));if(age()>=100&&!remainder(e.word(12),e.word(0x80))){emit(3,0,{{128,0,0}},0,5,constant(0x6d2618),constant(0x6d1ad0),e.quad(0xa8),0,51);put(0xa8,m.sub(q(0xa8),m.c(0x6d1920)));}}
  else if(age()>=1350){e.quad(0x38,stage4_first_approach(e.quad(0x38),constant(0x6d1c30),constant(0x6d1a88),m.precision));e.quad(0x40,stage4_first_approach(e.quad(0x40),constant(0x6d1898),constant(0x6d1a40),m.precision));put(0x38,m.add(q(0x38),m.c(0x6d1900)));if(crt_sse2::real(e.quad(0x38))>414.)e.word(0,0);}return;
 }
 throw std::out_of_range("Stage4 first enemy outside 60..67");
}
}
