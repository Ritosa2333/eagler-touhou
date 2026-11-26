// 原675630..675C43；背景计算先于Boss槽缓存，阶段服务之后按原位置重读字段。
#pragma once
#include "core/stages/Stage5MidbossTimelineArithmetic.hpp"
#include "core/stages/Stage1Timeline.hpp"
#include "core/stages/StageBossServices.hpp"
#include "core/entities/BossPhaseContext.hpp"
#include "core/entities/BossAllocationContext.hpp"
#include "core/entities/BossPresentationContext.hpp"
#include "core/stages/StagePatternContext.hpp"
#include "core/stages/StageResourceContext.hpp"
#include "core/stages/Stage3InitialMath.hpp"
#include "core/shared/EffectPool.hpp"
#include "core/math/GameMath.hpp"
namespace kaikeidou {
inline void stage5_midboss_segment(StageTimeline& t,EnemyPool& enemies,ProjectilePools& pool,EffectPool& effects,
 PatternParameters& p,BossPhaseContext& phase,BossStageContext& context,BossAllocationContext& allocation,
 BossPresentationContext& presentation,const StagePatternContext& extra,StageResourceContext& background,
 GameMath& math,StageBossServices& services,unsigned precision){
 using namespace stage5_midboss_timeline_detail;const Arithmetic m{{precision}};
 {
  const auto wave=math.sine(background.reals[1]);
  background.reals[0]=m.store(m.add(m.add(m.mul(wave,m.c(0x6d1cf8)),m.c(0x6d19e0)),m.q(background.reals[0])));
 }
 {
  const auto wave=math.sine(background.reals[3]);
  background.reals[2]=m.store(m.add(m.add(m.mul(wave,m.c(0x6d1cf8)),m.c(0x6d1aa0)),m.q(background.reals[2])));
 }
 {const auto wave=math.sine(background.reals[2]);background.reals[4]=m.store(m.mul(wave,m.c(0x6d1aa8)));}
 {const auto wave=math.cosine(background.reals[0]);background.reals[5]=m.store(m.add(m.mul(wave,m.q(background.reals[4])),m.c(0x6d1a60)));}
 {const auto wave=math.sine(background.reals[0]);background.reals[6]=m.store(m.add(m.mul(wave,m.q(background.reals[4])),m.c(0x6d1af0)));}
 auto boss=phase.boss_index;
 const auto enemy=[&]()->Enemy&{return boss==0xffffffffu?context.enemy_prefix:enemies.enemies.at(boss);};
 switch(t.subphase){
 case 0:
  if(t.frame==1){
   boss=static_cast<std::uint32_t>(services.allocate_boss(5));enemy().word(8,85);
   enemy().quad(0x38,background.reals[5]);enemy().quad(0x40,background.reals[6]);enemy().word(0x80,0);allocation.field_a7ea5c=0;
  }else if(t.frame==200)services.dialog(9);
  if(signed_word(allocation.field_a7ea5c)<=254)++allocation.field_a7ea5c;
  break;
 case 1:
  if(t.frame==1)services.begin_boss_effect();
  else if(t.frame==41){
   const auto y=enemy().quad(0x40),x=enemy().quad(0x38);const auto color=services.color(255,255,255);
   allocate_effect(effects,pool.sound_triggers,{12,color,x,y});allocation.field_a7ea5c=255;
   enemy().word(0x2c,3);enemy().word(0x30,0);enemy().word(0x34,0);
  }else if(t.frame==81){
   services.configure_phase(0,30,extra.boss_hp_multiplier*12000u,0xfffffff6u);enemy().word(0x80,1);
   enemy().word(0x84,stage5_midboss_timeline_pick_integer(p.difficulty,{{22,22,14,12,0,0}}));
   enemy().word(0x88,stage5_midboss_timeline_pick_integer(p.difficulty,{{28,28,20,14,0,0}}));
   enemy().quad(0xa8,constant(0x6d1de0));enemy().quad(0xb0,constant(0x6d1de0));
  }
  break;
 case 2:
  if(t.frame==41){
   copy_spell_name(context,p.difficulty);
   const auto hp=stage5_midboss_timeline_pick_integer(p.difficulty,{{16000,18000,18000,18000,0,0}});
   services.configure_phase(1,48,hp,67);enemy().word(8,86);
   enemy().word(0x80,stage5_midboss_timeline_pick_integer(p.difficulty,{{220,190,160,150,0,0}}));
   enemy().quad(0xa8,constant(0x6d1a60));enemy().quad(0xb0,constant(0x6d1b38));
   enemy().quad(0xb8,stage5_midboss_timeline_pick_real(p.difficulty,{{constant(0x6d25f8),constant(0x6d2600),constant(0x6d2608),constant(0x6d2610),0,0}}));
   p.integers[0]=stage5_midboss_timeline_pick_integer(p.difficulty,{{1,1,1,3,0,0}});
   p.reals[0]=stage5_midboss_timeline_pick_real(p.difficulty,{{constant(0x6d18b0),constant(0x6d1880),0x3ff0000000000000ull,0x3ff0000000000000ull,0,0}});
   p.reals[1]=stage3_initial_turn_rate(p.integers[0],precision);
   background.reals[10]=0x3ff0000000000000ull;background.reals[11]=constant(0x6d1900);background.reals[12]=constant(0x6d1900);
  }
  break;
 case 3:
  enemy().quad(0x38,stage5_midboss_timeline_approach(enemy().quad(0x38),background.reals[5],constant(0x6b25f8),precision));
  enemy().quad(0x40,stage5_midboss_timeline_approach(enemy().quad(0x40),background.reals[6],constant(0x6b25f8),precision));
  if(t.frame==1)presentation.entry_state=3;
  else if(t.frame==160)services.retire_boss(2);
  else if(t.frame==220){++t.part;t.subphase=0;t.frame=0;p.game_frame=0;}
  if(signed_word(t.frame)>=2){
   enemy().quad(0xb8,m.store(m.sub(m.q(enemy().quad(0xb8)),m.c(0x6d18e0))));
   enemy().quad(0xc0,m.store(m.sub(m.q(enemy().quad(0xc0)),m.c(0x6d18e0))));allocation.field_a7ea5c-=2;
  }
  break;
 default:break;
 }
}
}
