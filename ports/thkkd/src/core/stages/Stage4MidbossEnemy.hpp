// 原敌68；按成功分配逐次随机，第二次除法故障保留已生成的弹。
#pragma once
#include "core/stages/Stage4MidbossArithmetic.hpp"
#include "core/entities/EnemyPool.hpp"
#include "core/stages/StagePatternContext.hpp"
#include "core/stages/Stage4PatternContext.hpp"
#include "core/entities/ProjectilePool.hpp"
namespace kaikeidou {
inline void update_stage4_midboss_enemy(Enemy& e,ProjectilePools& pool,std::uint64_t,std::uint64_t py,PatternParameters& p,
 const StagePatternContext& extra,const Stage4PatternContext& fourth,Random& random,RandomPrecision precision,GameMath& math){
 using namespace stage4_midboss_detail;const Arithmetic m{{static_cast<unsigned>(precision)}};const auto q=[&](std::size_t o){return m.q(e.quad(o));};
 const auto sx=math.sine(e.quad(0xa8));const auto first_x=m.store(m.add(m.mul(sx,m.c(0x6d1aa8)),m.c(0x6d1a60)));const auto tx=math.sine(m.store(m.mul(q(0xb0),m.c(0x6d1920))));const auto x=m.store(m.add(m.mul(tx,m.c(0x6b25f8)),m.q(first_x)));e.quad(0x38,stage4_midboss_approach(e.quad(0x38),x,constant(0x6d1a88),m.precision));
 const auto sy=math.sine(m.store(m.mul(q(0xa8),m.c(0x6d1900))));const auto first_y=m.store(m.add(m.mul(sy,m.c(0x6d1a40)),m.c(0x6d1a90)));const auto ty=math.sine(e.quad(0xb0));const auto y=m.store(m.add(m.mul(ty,m.c(0x6d1a40)),m.q(first_y)));e.quad(0x40,stage4_midboss_approach(e.quad(0x40),y,constant(0x6d1a40),m.precision));
 e.quad(0xa8,m.store(m.add(q(0xa8),m.c(0x6a7708))));e.quad(0xb0,m.store(m.add(q(0xb0),m.c(0x6d1aa0))));
 const auto emit=[&](const std::uint32_t* color,std::uint32_t group){const auto index=allocate_projectile(pool,{21,0,{{color[0],color[1],color[2]}},0,255,1,41});if(index<0)return;auto& b=pool.bullets[static_cast<std::size_t>(index)];const auto r=random.real_number(constant(0x6d22a8),precision);b.quad(0x40,m.store(m.mul(r,m.c(0x6d1960))));b.quad(0x48,e.quad(0x40));b.quad(0x50,0);b.quad(0x58,crt_sse2::real(e.quad(0x40))<=crt_sse2::real(py)?constant(0x6d1a00):constant(0x6d1e70));b.word(8,52);b.word(0x78,group);};
 if(remainder(e.word(12),120)<=signed_word(e.word(0x80))){if(!remainder(e.word(12),e.word(0x84))){emit(extra.integers_6_to_19.data()+4,0);emit(fourth.integers_20_to_39.data(),1);}if(remainder(e.word(12),e.word(0x88))==1){emit(extra.integers_6_to_19.data()+4,0);emit(fourth.integers_20_to_39.data(),1);}}
 if(!remainder(p.game_frame,100)){p.integers[3]=1-p.integers[3];pool.sound_triggers[44]=1;}
}
}
