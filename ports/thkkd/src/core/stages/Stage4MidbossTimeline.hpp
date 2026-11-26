// 原6772F0..677863，入口缓存Boss槽，服务后按原地址重新读取阶段字段。
#pragma once
#include "core/stages/Stage4MidbossArithmetic.hpp"
#include "core/stages/Stage1Timeline.hpp"
#include "core/stages/StageBossServices.hpp"
#include "core/entities/BossPhaseContext.hpp"
#include "core/entities/BossAllocationContext.hpp"
#include "core/stages/StagePatternContext.hpp"
#include "core/stages/Stage4PatternContext.hpp"
#include "core/shared/EffectPool.hpp"
namespace kaikeidou {
inline void stage4_midboss_segment(StageTimeline& t,EnemyPool& enemies,ProjectilePools& pool,EffectPool& effects,
 PatternParameters& p,BossPhaseContext& phase,BossStageContext& context,BossAllocationContext& allocation,
 StagePatternContext& extra,Stage4PatternContext& fourth,const std::uint32_t& character,StageBossServices& services,unsigned precision){
 using namespace stage4_midboss_detail;const Arithmetic m{{precision}};auto boss=phase.boss_index;
 const auto enemy=[&]()->Enemy&{return boss==0xffffffffu?context.enemy_prefix:enemies.enemies.at(boss);};
 const auto put=[&](std::size_t o,binary_float::Number value){enemy().quad(o,m.store(value));};
 const auto change=[&](std::size_t o,std::uint32_t c,bool subtract){const auto a=m.q(enemy().quad(o));put(o,subtract?m.sub(a,m.c(c)):m.add(a,m.c(c)));};
 const auto effect=[&](const std::uint32_t* color){const auto y=enemy().quad(0x40),x=enemy().quad(0x38);const auto packed=services.color(color[0],color[1],color[2]);allocate_effect(effects,pool.sound_triggers,{12,packed,x,y});};
 switch(t.subphase){
 case 0:
  if(t.frame==1){
   p.integers[0]=stage4_midboss_pick_character(character,{{255,64,64}});p.integers[1]=stage4_midboss_pick_character(character,{{64,64,255}});p.integers[2]=stage4_midboss_pick_character(character,{{64,255,64}});
   extra.integers_6_to_19[4]=stage4_midboss_pick_character(character,{{64,64,255}});extra.integers_6_to_19[5]=stage4_midboss_pick_character(character,{{64,255,64}});extra.integers_6_to_19[6]=stage4_midboss_pick_character(character,{{255,64,64}});
   fourth.integers_20_to_39[0]=stage4_midboss_pick_character(character,{{64,255,64}});fourth.integers_20_to_39[1]=stage4_midboss_pick_character(character,{{255,64,64}});fourth.integers_20_to_39[2]=stage4_midboss_pick_character(character,{{64,64,255}});
   boss=static_cast<std::uint32_t>(services.allocate_boss(4));enemy().word(8,17);enemy().quad(0x38,constant(0x6d1a60));enemy().quad(0x40,constant(0x6d1c00));enemy().quad(0xa8,constant(0x6d1a60));enemy().quad(0xb0,constant(0x6d1a90));enemy().quad(0xb8,constant(0x6b25b8));enemy().quad(0xc0,constant(0x6b25b8));allocation.field_a7ea5c=0;
  }else if(t.frame==190)services.begin_boss_effect();else if(t.frame==230){t.subphase=1;t.frame=0;p.game_frame=0;}
  if(signed_word(t.frame)>=2){change(0xb8,0x6d18e0,true);change(0xc0,0x6d18e0,true);++allocation.field_a7ea5c;}break;
 case 1:
  if(t.frame==1)effect(extra.integers_6_to_19.data()+4);
  else if(t.frame==21){effect(fourth.integers_20_to_39.data());enemy().word(0x2c,3);enemy().word(0x30,0);enemy().word(0x34,0);}
  else if(t.frame==61){services.configure_phase(0,40,12000,0xfffffff6u);enemy().word(8,68);enemy().quad(0xa8,0);enemy().quad(0xb0,0);enemy().word(0x80,stage4_midboss_pick_integer(p.difficulty,{{59,69,74,79,0,0}}));enemy().word(0x84,stage4_midboss_pick_integer(p.difficulty,{{5,5,4,3,0,0}}));enemy().word(0x88,stage4_midboss_pick_integer(p.difficulty,{{3,2,2,2,0,0}}));p.integers[3]=0;}
  if(signed_word(allocation.field_a7ea5c)<=254)++allocation.field_a7ea5c;break;
 case 2:
  if(t.frame==1){enemy().word(8,17);enemy().quad(0xa8,constant(0x6d1a60));enemy().quad(0xb0,0);enemy().quad(0xb8,constant(0x6b25b8));enemy().quad(0xc0,constant(0x6b25b8));allocation.field_a7ea5c=255;}
  else if(t.frame==160)services.retire_boss(2);else if(t.frame==220){++t.part;t.subphase=0;t.frame=0;p.game_frame=0;}
  if(signed_word(t.frame)>=2){change(0xb0,0x6d2620,false);change(0xb8,0x6d18e0,true);change(0xc0,0x6d18e0,true);--allocation.field_a7ea5c;}break;
 default:break;
 }
}
}
