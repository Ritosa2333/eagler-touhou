// 原普通弹73/74/76/77/78与副弹75；73分别在两次三角调用前缓存Boss槽。
#pragma once
#include "core/stages/Stage5BossEarlyArithmetic.hpp"
#include "core/entities/ProjectileEmitter.hpp"
#include "core/shared/FinalProjectileRecord.hpp"
#include "core/entities/EnemyPool.hpp"
#include "core/entities/BossPhaseContext.hpp"
#include "core/entities/BossStageContext.hpp"
namespace kaikeidou {
inline void update_stage5_boss_early_projectile(ProjectilePools& pool,std::uint32_t index,std::uint32_t behavior,const EnemyPool& enemies,
 const BossPhaseContext& phase,BossStageContext& boss,const ObjectRecord<240>& player,PatternParameters& p,Random& random,RandomPrecision precision,GameMath& math){
 using namespace stage5_boss_early_detail;const Arithmetic m{{static_cast<unsigned>(precision)}};
 if(behavior==75){auto& s=pool.secondary.at(index);const auto q=[&](std::size_t o){return m.q(s.quad(o));};
  if(s.word(0x48)==0)s.quad(0x20,stage5_boss_early_approach(s.quad(0x20),s.quad(0x28),constant(0x6d1a40),m.precision));
  else if(s.word(0x48)==1){const auto dx=m.store(m.sub(m.q(player.quad(0x90)),q(0x10))),dy=m.store(m.sub(m.q(player.quad(0x98)),q(0x18)));const auto angle=m.store(math.atan2(dy,dx));const auto square=m.store(m.add(m.mul(m.q(dx),m.q(dx)),m.mul(m.q(dy),m.q(dy))));const auto distance=m.store(m.mul(stage5_boss_early_sqrt(square,m.precision),m.c(0x6a7708)));const auto cosine=math.cosine(angle);s.quad(0x10,m.store(m.add(m.mul(cosine,m.q(distance)),q(0x10))));const auto sine=math.sine(angle);s.quad(0x18,m.store(m.add(m.mul(sine,m.q(distance)),q(0x18))));}
  else if(s.word(0x48)==0xffffffffu){s.quad(0x20,stage5_boss_early_approach(s.quad(0x20),0,constant(0x6d1a40),m.precision));if(crt_sse2::real(s.quad(0x20))<=1.)s.word(0,0);}return;
 }
 final_projectile_detail::Record b{pool,index};const auto q=[&](std::size_t o){return m.q(b.quad(o));};const auto put=[&](std::size_t o,binary_float::Number v){b.quad(o,m.store(v));};
 if(behavior==73){const auto enemy=[&](std::uint32_t at)->const Enemy&{return at==0xffffffffu?boss.enemy_prefix:enemies.enemies.at(at);};b.quad(0x60,p.reals[0]);const auto x_boss=phase.boss_index;const auto x_argument=m.store(m.add(m.q(p.reals[0]),q(0xa8)));const auto cosine=math.cosine(x_argument);put(0x40,m.add(m.mul(cosine,q(0xa0)),m.q(enemy(x_boss).quad(0x38))));const auto y_boss=phase.boss_index;const auto y_argument=m.store(m.add(m.q(p.reals[0]),q(0xa8)));const auto sine=math.sine(y_argument);put(0x48,m.add(m.mul(sine,q(0xa0)),m.q(enemy(y_boss).quad(0x40))));return;}
 if(behavior==74){const auto cosine=math.cosine(p.reals[0]);put(0x40,m.add(m.mul(cosine,m.q(p.reals[1])),q(0x40)));const auto sine=math.sine(p.reals[0]);put(0x48,m.add(m.mul(sine,m.q(p.reals[1])),q(0x48)));if(b.word(12)==420)b.word(0,0);return;}
 const auto speed=[&](std::uint32_t scale,std::uint32_t base,std::uint32_t second_scale){const auto first_sine=math.sine(boss.extra_reals[0]);const auto first=m.store(m.add(m.mul(first_sine,m.c(scale)),m.c(base)));const auto second_sine=math.sine(boss.extra_reals[1]);return m.store(m.add(m.mul(second_sine,m.c(second_scale)),m.q(first)));};
 const auto emit=[&](std::uint32_t kind,std::int32_t group,std::array<std::uint32_t,3> color,std::uint64_t x,std::uint64_t y,std::uint64_t velocity,std::uint64_t angle){emit_projectiles(pool,{{kind,group,color,0,255,1,41},x,y,1,0,velocity,angle,1,1},player.quad(0x90),player.quad(0x98),p,random,precision,math);};
 const auto phase_step=[&](std::uint32_t first,std::uint32_t second){boss.extra_reals[0]=m.store(m.add(m.q(boss.extra_reals[0]),m.c(first)));boss.extra_reals[1]=m.store(m.add(m.q(boss.extra_reals[1]),m.c(second)));};
 if(behavior==76||behavior==77){put(0x40,m.add(q(0x40),q(0xa0)));const auto first_period=behavior==76?8u:b.word(0x78);if(!remainder(p.game_frame,first_period)){
   const auto velocity=speed(behavior==76?0x6d18b0:0x6d19d8,behavior==76?0x6d1f50:0x6d3078,behavior==76?0x6d18e0:0x6d24e0);bool odd=false;if(behavior==77){odd=(b.word(0x7c)&1u)!=0;b.word(0x7c,b.word(0x7c)+1);}
   emit(odd?0:1,odd?2:1,odd?std::array<std::uint32_t,3>{{0,230,230}}:std::array<std::uint32_t,3>{{0,255,255}},b.quad(0x40),0,velocity,constant(0x6d1b60));phase_step(0x6d2ad0,behavior==76?0x6d24e0:0x6d31e0);
  }
  if((behavior==77||p.difficulty==1)&&!remainder(p.game_frame,behavior==76?10u:b.word(0x80))){emit(4,3,{{255,190,64}},b.quad(0x40),0,b.quad(0xa8),constant(0x6d1b60));put(0xa8,m.add(q(0xa8),m.c(0x6d2538)));}
  const auto x=crt_sse2::real(b.quad(0x40));if(x<=-25.||x>=409.)b.word(0,0);return;
 }
 if(behavior!=78)throw std::out_of_range("Stage5 early projectile outside73..78");const auto raw_side=b.word(0x84);const auto side=raw_side<3?raw_side:3u;const bool horizontal=(side&1u)==0;const auto coordinate=horizontal?0x40u:0x48u;
 put(coordinate,side<2?m.add(q(coordinate),m.c(horizontal?0x6d2660:0x6d25a8)):m.sub(q(coordinate),m.c(horizontal?0x6d2660:0x6d25a8)));
 const auto angle=side==0?constant(0x6d1b60):side==1?constant(0x6d1860):side==2?constant(0x6d1de0):0;
 const auto x=[&](){return horizontal?b.quad(0x40):side==1?constant(0x6d1cb0):0;};const auto y=[&](){return horizontal?(side==2?constant(0x6d19a0):0):b.quad(0x48);};
 if(!remainder(p.game_frame,horizontal?6u:5u)){const auto velocity=speed(0x6d1f78,0x6d31d8,0x6d24e0);const bool odd=(b.word(0x7c)&1u)!=0;b.word(0x7c,b.word(0x7c)+1);emit(odd?0:1,odd?2:1,odd?std::array<std::uint32_t,3>{{0,230,230}}:std::array<std::uint32_t,3>{{0,255,255}},x(),y(),velocity,angle);phase_step(0x6d2630,0x6d2b90);}
 if(!remainder(p.game_frame,horizontal?8u:7u)){emit(4,3,{{255,190,64}},x(),y(),b.quad(0xa8),angle);put(0xa8,m.add(q(0xa8),m.c(0x6d2538)));}
 const auto v=crt_sse2::real(b.quad(coordinate));if(side<2?v>=(horizontal?409.:473.):v<=-25.)b.word(0,0);
}
}
