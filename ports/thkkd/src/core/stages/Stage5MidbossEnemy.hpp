// 原敌85/86，背景坐标使用StageResourceContext的槽5/6，52次固定分配逐次重读原字段。
#pragma once
#include "core/stages/Stage5MidbossDependenciesArithmetic.hpp"
#include "core/entities/EnemyPool.hpp"
#include "core/shared/EffectPool.hpp"
#include "core/stages/StageResourceContext.hpp"
#include "core/entities/BossStageContext.hpp"
#include "core/entities/ProjectileEmitter.hpp"
namespace kaikeidou {
inline void update_stage5_midboss_enemy(std::uint32_t behavior,Enemy& e,ProjectilePools& pool,EffectPool& effects,const ObjectRecord<240>& player,
 PatternParameters& p,const StageResourceContext& background,Random& random,RandomPrecision precision,GameMath& math,BossStageServices& services){
 using namespace stage5_midboss_dependencies_detail;const Arithmetic m{{static_cast<unsigned>(precision)}};const auto q=[&](std::size_t o){return m.q(e.quad(o));};const auto put=[&](std::size_t o,binary_float::Number v){e.quad(o,m.store(v));};
 if(behavior==85){e.quad(0x38,background.reals[5]);e.quad(0x40,background.reals[6]);if(e.word(0x80)!=1)return;
  if(!remainder(e.word(12),120))for(const auto& point:points){const auto index=allocate_projectile(pool,{1,1,point.color,0,255,1,41});if(index>=0){auto& b=pool.bullets[static_cast<std::size_t>(index)];b.quad(0x40,e.quad(0x38));b.quad(0x48,e.quad(0x40));b.word(8,71);b.quad(0xa0,point.x);b.quad(0xa8,point.y);}}
  if(signed_word(p.difficulty)>=1){
   if(!remainder(e.word(12),e.word(0x84)))emit_projectiles(pool,{{0,2,{{0,0,0}},0,255,1,41},e.quad(0x38),e.quad(0x40),5,constant(0x6d2618),constant(0x6d1a00),e.quad(0xa8),1,0},player.quad(0x90),player.quad(0x98),p,random,precision,math);
   if(!remainder(e.word(12),e.word(0x88)))emit_projectiles(pool,{{0,2,{{200,200,200}},0,255,1,41},e.quad(0x38),e.quad(0x40),5,constant(0x6d2618),constant(0x6d1e40),e.quad(0xb0),1,0},player.quad(0x90),player.quad(0x98),p,random,precision,math);
  }
  put(0xa8,m.sub(q(0xa8),m.c(0x6d1b48)));put(0xb0,m.add(q(0xb0),m.c(0x6d2878)));return;
 }
 if(behavior!=86)throw std::out_of_range("Stage5 midboss enemy outside85..86");
 e.quad(0x38,stage5_midboss_dependencies_approach(e.quad(0x38),e.quad(0xa8),constant(0x6d1a40),m.precision));e.quad(0x40,stage5_midboss_dependencies_approach(e.quad(0x40),e.quad(0xb0),constant(0x6d1a40),m.precision));
 if(e.word(12)==40){const auto y=player.quad(0x98),x=player.quad(0x90);const auto color=services.color(255,255,255);allocate_effect(effects,pool.sound_triggers,{13,color,x,y});e.word(0x2c,3);e.word(0x30,0);e.word(0x34,0);return;}
 if(e.word(12)==80){const auto angle=m.store(m.mul(random.real_number(constant(0x6d1c60),precision),m.c(0x6d1950)));
  for(std::uint32_t i=0;i<=8;++i){std::array<std::uint32_t,3> color{};color[i%3]=255;const auto direction=m.store(m.add(m.mul(m.i(i),m.c(0x6d2290)),m.q(angle)));const auto index=allocate_projectile(pool,{2,1,color,2,255,1,41});if(index<0)continue;auto& b=pool.bullets[static_cast<std::size_t>(index)];
   b.quad(0xa0,player.quad(0x90));b.quad(0xa8,player.quad(0x98));b.quad(0xb0,direction);b.quad(0xb8,constant(0x6d1af0));b.quad(0xc0,e.quad(0xb8));b.quad(0xc8,direction);b.word(0x78,i*100);b.word(0x7c,color[0]);b.word(0x80,color[1]);b.word(0x84,color[2]);
   const auto cosine=math.cosine(b.quad(0xb0));b.quad(0x40,m.store(m.add(m.mul(cosine,m.q(b.quad(0xb8))),m.q(b.quad(0xa0)))));const auto sine=math.sine(b.quad(0xb0));b.quad(0x48,m.store(m.add(m.mul(sine,m.q(b.quad(0xb8))),m.q(b.quad(0xa8)))));b.word(8,72);b.word(0x34,1);
  }return;
 }
 if(e.word(12)==e.word(0x80)){e.word(12,0);e.quad(0xa8,player.quad(0x90));const auto height=random.real_number(constant(0x6d1a78),precision);put(0xb0,m.add(m.mul(height,m.c(0x6d1960)),m.c(0x6d1a90)));put(0xb8,m.mul(q(0xb8),m.c(0x6aca58)));}
}
}
