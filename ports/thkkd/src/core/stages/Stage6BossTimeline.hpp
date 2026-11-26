// 原660CD0..669CD3完整关底时间轴，结局跨场景显示由外层原顺序调用。
#pragma once
#include "core/stages/Stage1Timeline.hpp"
#include "core/stages/Stage6BossTimelineServices.hpp"
#include "core/stages/Stage6BossTimelineArithmetic.hpp"
#include "core/stages/Stage6BossTimelineColoredEffect.hpp"
#include "core/stages/Stage6BossTimelineEmitter.hpp"
#include "core/stages/Stage6RuntimeContext.hpp"
#include "core/stages/Stage3BossTimelineContext.hpp"
#include "core/stages/StagePatternContext.hpp"
#include "core/stages/StageResourceContext.hpp"
#include "core/entities/BossPhaseContext.hpp"
#include "core/entities/BossAllocationContext.hpp"
#include "core/entities/EnemyDamage.hpp"
#include "core/entities/EnemyDeathContext.hpp"
#include "core/player/ItemPool.hpp"
#include "core/player/PlayerShotHit.hpp"
#include "core/math/BinaryFloat32.hpp"
#include "core/stages/Stage5BossTimelineSecondaryContact.hpp"
#include "core/stages/Stage6Float3Rotation.hpp"
namespace kaikeidou {
inline void stage6_boss_segment(StageTimeline& timeline,EnemyPool& enemies,ProjectilePools& projectiles,EffectPool& effects,ItemPool& items,
 const ObjectRecord<240>& player,PatternParameters& p,BossPhaseContext& phase,BossAllocationContext& allocation,EnemyDamageContext& protection,
 BossStageContext& context,StagePatternContext& extra,Stage3BossTimelineContext& extended,Stage6RuntimeContext& runtime,EnemyDeathContext& death,
 StageResourceContext& background,const std::uint32_t& character,Random& random,RandomPrecision precision,GameMath& math,Stage6BossTimelineServices& services){
 using namespace stage6_boss_timeline_detail;using Number=binary_float::Number;const auto bits=static_cast<unsigned>(precision);const Arithmetic m{bits};auto boss=phase.boss_index;
 const auto enemy_at=[&](std::uint32_t slot)->Enemy&{return slot==0xffffffffu?context.enemy_prefix:enemies.enemies.at(slot);};const auto enemy=[&]()->Enemy&{return enemy_at(boss);};
 const auto c=[](std::uint32_t address){return constant(address);};const auto frame=[&](){return signed_word(timeline.frame);};
 const auto integer=[&](unsigned i)->std::uint32_t&{if(i<5)return p.integers[i];if(i==5)return death.pattern_integer_5;return extra.integers_6_to_19.at(i-6);};
 const auto real_slot=[&](unsigned i)->std::uint64_t&{if(i<2)return p.reals[i];if(i<4)return context.extra_reals[i-2];if(i==4)return extra.real_4;if(i<10)return extended.reals_5_to_9[i-5];if(i<20)return extra.reals_10_to_19[i-10];return extended.reals_20_to_29.at(i-20);};
 const auto pick=[&](std::array<std::uint32_t,6> values){return stage6_boss_timeline_pick_integer(p.difficulty,values);};const auto real=[&](std::array<std::uint64_t,6> values){return stage6_boss_timeline_pick_real(p.difficulty,values);};const auto who=[&](std::array<std::uint32_t,3> values){return stage6_boss_timeline_pick_character(character,values);};
 const auto q=[&](std::size_t offset){return m.q(enemy().quad(offset));};const auto put=[&](std::size_t offset,Number value){enemy().quad(offset,m.store(value));};
 const auto rnd=[&](std::uint32_t maximum){return random.real_number(c(maximum),precision);};const auto rndi=[&](std::uint32_t maximum){return random.integer_bits(maximum,precision,true);};
 const auto convert=[&](Number value){if(value.exponent>31)return 0x80000000u;const auto stored=m.q(m.store(value));return stored.exponent>=31?0x80000000u:binary_float::truncate(stored);};
 const auto bullet=[&](std::uint32_t index){return BulletView{projectiles,index*240u};};
 const auto effect_at=[&](std::uint32_t index)->Effect&{return index==0xffffffffu?runtime.effect_prefix:effects.effects.at(index);};
 const auto mark=[&](){enemy().word(0x2c,3);enemy().word(0x30,0);enemy().word(0x34,0);};
 const auto move=[&](std::uint32_t y){enemy().word(8,14);enemy().quad(0xa8,c(0x6d1a60));enemy().quad(0xb0,c(y));};
 const auto effect=[&](std::array<std::uint32_t,3> color){const auto y=enemy().quad(0x40),x=enemy().quad(0x38);const auto packed=services.color(color[0],color[1],color[2]);return allocate_effect(effects,projectiles.sound_triggers,{12,packed,x,y});};
 const auto colored=[&](std::uint32_t color,std::uint32_t hp,std::uint32_t behavior,std::uint64_t x,std::uint64_t y){return stage6_boss_colored_effect(effects,runtime.effect_prefix,projectiles.sound_triggers,14,color,hp,behavior,x,y,services);};
 const auto from_effect=[&](std::uint32_t index,std::uint32_t effect_index){auto& target=enemy_at(index);auto& source=effect_at(effect_index);target.word(4,source.word(0x10));target.word(0x18,source.word(0x14));target.word(0x14,target.word(0x18));target.word(8,source.word(0x18));target.quad(0x38,source.quad(0x60));target.quad(0x40,source.quad(0x68));};
 const auto emit=[&](ProjectileAllocation request,std::uint64_t x,std::uint64_t y,std::uint32_t count,std::uint64_t spacing,std::uint64_t speed,std::uint64_t angle,std::uint32_t absolute,std::uint32_t configuration){stage6_boss_timeline_emit(projectiles,{request,x,y,count,spacing,speed,angle,absolute,configuration},player,p,random,precision,math);};
 switch(timeline.subphase){
 case 0:stage6_boss_timeline_ease_real(background.reals[4],c(0x6d1cc0),c(0x6d1aa8),bits);if(timeline.frame==300)services.dialog(12);(void)random.real_number(0,precision);break;
 case 1:if(timeline.frame==1){move(0x6d1a90);services.begin_boss_effect();}else if(timeline.frame==81){enemy().quad(0x38,c(0x6d1a60));enemy().quad(0x40,c(0x6d1a90));integer(0)=0;integer(1)=120;integer(2)=pick({{11,7,9,7,0,0}});integer(4)=pick({{13,11,11,11,0,0}});integer(3)=pick({{220,210,200,190,0,0}});real_slot(0)=real({{c(0x6d1948),c(0x6d1ce8),c(0x6d1b18),c(0x6d25f0),0,0}});services.configure_phase(0,40,extra.boss_hp_multiplier*15000u,0xfffffff6u);enemy().word(8,109u+(signed_word(p.difficulty)>1?1u:0u));enemy().word(0x80,1);}break;

 case 3:{
  const auto ribbon=[&](std::uint32_t direction,std::uint32_t divisor,std::uint32_t factor,std::uint32_t angle,bool red){const auto index=allocate_projectile(projectiles,{18,0,red?std::array<std::uint32_t,3>{{255,0,0}}:std::array<std::uint32_t,3>{{0,0,255}},2,255,0,41});if(index==-1)return;auto b=bullet(static_cast<std::uint32_t>(index));b.word(0x78,direction);b.quad(0xa0,m.store(m.sub(m.c(0x6d1af0),q(0x40))));b.quad(0xa8,c(divisor));b.quad(0xb0,0);const auto value=real({{c(0x6d19d8),c(0x6d1880),0x3ff0000000000000ull,0x3ff0000000000000ull,0,0}});b.quad(0xb8,m.store(m.mul(m.q(value),m.c(factor))));b.quad(0xc0,angle?c(angle):0);b.quad(0x40,enemy().quad(0x38));b.quad(0x48,enemy().quad(0x40));b.word(8,94);b.word(0x34,1);};
  if(timeline.frame==1){enemy().word(8,14);enemy().quad(0xa8,c(0x6d1a60));enemy().quad(0xb0,m.store(m.i(pick({{100,100,100,224,0,0}}))));}
  else if(timeline.frame==81){enemy().quad(0x38,c(0x6d1a60));enemy().quad(0x40,m.store(m.i(pick({{100,100,100,224,0,0}}))));integer(0)=0;integer(1)=120;integer(2)=pick({{15,13,7,7,0,0}});integer(3)=pick({{19,15,13,13,0,0}});real_slot(0)=real({{c(0x6d1948),c(0x6d1ce8),c(0x6d25a8),c(0x6d25a8),0,0}});services.configure_phase(0,45,extra.boss_hp_multiplier*15000u,0xfffffff6u);}
  else if(timeline.frame==100){effect({{0,0,255}});mark();}
  else if(timeline.frame==140){if(p.difficulty==3){ribbon(1,0x6d1fe8,0x6d2500,0,false);ribbon(1,0x6d1fe8,0x6d2500,0x6d1860,false);}else ribbon(1,0x6d1fe8,0x6d2500,0x6d1de0,false);enemy().word(0x2c,0);}
  else if(timeline.frame==200)enemy().quad(0xb0,c(0x6b25b8));
  else if(timeline.frame==360){effect({{255,0,0}});mark();}
  else if(timeline.frame==400){ribbon(0xffffffffu,0x6d1b38,0x6d30a0,0x6d1de0,true);enemy().word(0x2c,0);}break;}
 case 5:{
  if(timeline.frame==1)move(0x6d1af0);
  else if(timeline.frame==81){enemy().quad(0x38,c(0x6d1a60));enemy().quad(0x40,c(0x6d1af0));integer(2)=pick({{7,5,3,3,0,0}});integer(3)=pick({{8,7,5,4,0,0}});integer(4)=1;integer(5)=pick({{23,25,27,29,0,0}});real_slot(0)=c(0x6d1ce0);real_slot(1)=real({{c(0x6d3098),c(0x6d3098),c(0x6d3098),c(0x6d3098),0,0}});services.configure_phase(0,60,extra.boss_hp_multiplier*18000u,0xfffffff6u);}
  if(frame()>=90){if(remainder(timeline.frame,300)==90){integer(0)=340;integer(1)=integer(0)+60u;integer(4)=1u-integer(4);effect(integer(4)==0?std::array<std::uint32_t,3>{{0,0,255}}:std::array<std::uint32_t,3>{{255,0,0}});mark();}
   else if(remainder(timeline.frame,300)==130){const bool vertical=integer(4)!=0;for(unsigned side=0;side<2;++side){const auto index=allocate_projectile(projectiles,{18,0,vertical?std::array<std::uint32_t,3>{{255,0,0}}:std::array<std::uint32_t,3>{{0,0,255}},2,255,0,41});if(index==-1)continue;auto b=bullet(static_cast<std::uint32_t>(index));b.word(0x78,vertical?1u:0u);b.quad(0xa0,c(vertical?0x6d1a60:side==0?0x6d2998:0x6d2748));b.quad(0xa8,c(vertical?(side==0?0x6d2398:0x6d2ba8):0x6d1af0));b.quad(0xb0,m.store(m.mul(rnd(0x6d1c60),m.c(0x6d1950))));const auto sign=rndi(1);const auto value=m.mul(rnd(0x6b25b8),m.c(0x6d3090));const auto turn=m.store(sign?m.add(value,m.c(0x6d3088)):m.sub(m.c(0x6d3080),value));b.quad(0xb8,turn);if(vertical)b.quad(0xc0,m.store(m.mul(m.q(real_slot(0)),m.c(side==0?0x6d3078:0x6d3070))));else b.quad(0xc0,real_slot(0));b.quad(0x40,enemy().quad(0x38));b.quad(0x48,enemy().quad(0x40));b.word(8,95);b.word(0x34,1);}enemy().word(0x2c,0);}
   --integer(0);--integer(1);
  }break;}
 case 7:{
  if(timeline.frame==1)move(0x6d21f0);
  else if(timeline.frame==81){enemy().quad(0x38,c(0x6d1a60));enemy().quad(0x40,c(0x6d21f0));integer(0)=0;integer(1)=140;integer(2)=pick({{15,15,9,7,0,0}});integer(3)=pick({{19,19,13,11,0,0}});real_slot(0)=real({{c(0x6d1ce8),c(0x6d25f0),c(0x6d2480),c(0x6d26e0),0,0}});real_slot(1)=c(0x6d1ea8);real_slot(2)=0;real_slot(3)=0;services.configure_phase(0,60,extra.boss_hp_multiplier*13000u,0xfffffff6u);}
  else if(timeline.frame==90){effect({{0,0,255}});mark();}
  else if(timeline.frame==120)effect({{255,0,0}});
  else if(timeline.frame==160){for(unsigned side=0;side<2;++side){const auto index=allocate_projectile(projectiles,{18,0,side==0?std::array<std::uint32_t,3>{{0,0,255}}:std::array<std::uint32_t,3>{{255,0,0}},2,255,0,41});if(index==-1)continue;auto b=bullet(static_cast<std::uint32_t>(index));b.word(0x78,side==0?1u:0xffffffffu);b.quad(0xa0,c(0x6d1de0));b.quad(0xa8,side==0?0:c(0x6d1860));b.quad(0x40,enemy().quad(0x38));b.quad(0x48,enemy().quad(0x40));b.word(8,97);b.word(0x34,1);}enemy().word(0x2c,0);}
  else if(timeline.frame==350){effect({{0,255,0}});mark();}
  else if(timeline.frame==390){const auto index=allocate_projectile(projectiles,{18,0,{{0,255,0}},2,255,0,41});if(index!=-1){auto b=bullet(static_cast<std::uint32_t>(index));b.quad(0x40,enemy().quad(0x38));b.quad(0x48,enemy().quad(0x40));b.word(8,98);b.quad(0xa0,c(0x6d1de0));b.word(0x34,1);}enemy().word(0x2c,0);}
  if(frame()>=170){auto cosine=math.cosine(real_slot(2));const auto base=m.store(m.sub(m.c(0x6d1b48),m.mul(cosine,m.c(0x6d1d28))));cosine=math.cosine(real_slot(3));real_slot(1)=m.store(m.add(m.sub(m.q(base),m.mul(cosine,m.c(0x6d2358))),m.q(real_slot(1))));real_slot(2)=m.store(m.add(m.q(real_slot(2)),m.c(0x6d1928)));real_slot(3)=m.store(m.add(m.q(real_slot(3)),m.c(0x6d21c8)));}break;}

 case 2:{
  if(timeline.frame==1){enemy().word(8,17);enemy().quad(0xa8,c(0x6d1a60));enemy().quad(0xb0,c(0x6d1d00));enemy().quad(0xb8,c(0x6d1ae8));enemy().quad(0xc0,c(0x6d1ae8));}
  else if(timeline.frame==100){switch(p.difficulty){case 0:copy_name(context.spell_name,name_6c5fc8);break;case 1:copy_name(context.spell_name,name_6c5fe4);break;case 2:copy_name(context.spell_name,name_6c6000);break;case 3:copy_name(context.spell_name,name_b410be,2);break;}services.configure_phase(1,60,extra.boss_hp_multiplier*13000u,94);integer(0)=pick({{16,14,12,10,0,0}});integer(1)=pick({{6,8,10,12,0,0}});real_slot(0)=m.store(m.i(pick({{260,280,300,320,0,0}})));real_slot(1)=real({{c(0x6d25a8),c(0x6d1ad8),c(0x6d1d60),c(0x6d2480),0,0}});}
  else if(timeline.frame==160){effect({{255,255,255}});mark();}
  if(frame()>=200){if(timeline.frame==200)enemy().word(0x2c,0);
   if(remainder(timeline.frame,integer(0))==0){std::uint64_t x,y;for(;;){x=m.store(rnd(0x6d1cb0));y=m.store(random.real_number(real_slot(0),precision));const auto dx=m.store(m.sub(m.q(player.quad(0x90)),m.q(x))),dy=m.store(m.sub(m.q(player.quad(0x98)),m.q(y)));const auto squared=m.store(m.add(m.mul(m.q(dx),m.q(dx)),m.mul(m.q(dy),m.q(dy))));if(greater(finite_square_root(squared,bits),m.c(0x6d1f08)))break;}
    const auto color=rndi(4u+(signed_word(p.difficulty)>2?1u:0u));const auto effect_index=static_cast<std::uint32_t>(colored(color,color==5?200u:30u,111,x,y));const auto index=static_cast<std::uint32_t>(allocate_enemy(enemies));if(index!=0xffffffffu)from_effect(index,effect_index);
    const auto selected=enemy_at(index).word(4)-100u;if(selected<=5){constexpr std::array<std::array<std::uint32_t,3>,6> colors{{{{255,0,0}},{{0,0,255}},{{0,255,0}},{{0,255,255}},{{255,255,0}},{{255,0,255}}}};const auto spacing=m.divide_integer_bits(m.c(0x6d2208),integer(1));const auto count=integer(1);const auto origin_y=enemy_at(index).quad(0x40),origin_x=enemy_at(index).quad(0x38);emit({15,1,colors[selected],0,255,0,41},origin_x,origin_y,count,spacing,c(0x6d1c90),0,0,2);}
    if(signed_word(p.difficulty)>=2){const auto angle=m.store(m.add(m.mul(rnd(0x6d1d00),m.c(0x6d1950)),m.c(0x6d30a8)));const auto speed=m.store(m.add(m.mul(rnd(0x6d1ab0),m.c(0x6d1950)),m.c(0x6d1ff8)));const auto origin_y=m.store(m.sub(m.c(0x6d1a80),m.mul(rnd(0x6d1a78),m.c(0x6a7708))));const auto origin_x=m.store(m.add(m.mul(rnd(0x6d2f50),m.c(0x6d1960)),m.c(0x6d19f0)));emit({4,2,{{255,255,255}},0,255,0,41},origin_x,origin_y,1,0,speed,angle,1,0);}
   }
   if(remainder(timeline.frame,480)==190){enemy().quad(0xa8,player.quad(0x90));const auto value=rnd(0x6d2710);put(0xb0,m.add(m.mul(value,m.c(0x6d1960)),m.c(0x6d1b38)));if(greater(m.c(0x6d1d00),q(0xa8)))enemy().quad(0xa8,c(0x6d1d00));else if(greater(q(0xa8),m.c(0x6d23a0)))enemy().quad(0xa8,c(0x6d23a0));const auto offset=rnd(0x6d1a40);put(0xa8,m.add(m.add(offset,m.c(0x6d22c0)),q(0xa8)));}
  }break;}

 case 4:{
  if(timeline.frame==1){move(0x6b25b8);integer(9)=timeline.frame;}
  else if(timeline.frame==100){switch(p.difficulty){case 0:copy_name(context.spell_name,name_6c602c);break;case 1:copy_name(context.spell_name,name_6c6044);break;case 2:copy_name(context.spell_name,name_6c605c);break;case 3:copy_name(context.spell_name,name_6c6074);break;}services.configure_phase(1,60,extra.boss_hp_multiplier*14000u,98);integer(0)=0;integer(1)=pick({{15,20,20,25,0,0}});integer(2)=pick({{9,15,10,12,0,0}});}
  if(frame()>=200){const bool high=signed_word(p.difficulty)>1;const auto cycle=remainder(integer(9),high?600u:400u);
   if(integer(0)==0){if(cycle==200||(player_shot_hit_detail::count_visible(enemies)==1&&player.word(0x2c)==0)){integer(0)=1;effect({{255,255,255}});integer(9)=200;mark();}}
   else{const auto old_counter=integer(0)++;if(old_counter==40){integer(0)=0;const auto y=enemy().quad(0x40),x=enemy().quad(0x38);const auto effect_index=static_cast<std::uint32_t>(colored(1,high?100u:120u,112,x,y));
     for(std::uint32_t i=0;signed_word(i)<=signed_word(integer(1));++i){const auto angle=m.store(m.mul(rnd(0x6d1c60),m.c(0x6d1950)));const auto speed=m.store(m.add(m.mul(rnd(0x6b25b8),m.c(0x6a7708)),m.c(0x6d1a00)));const auto index=static_cast<std::uint32_t>(allocate_enemy(enemies));if(index==0xffffffffu)continue;auto& child=enemy_at(index);
      if(high)from_effect(index,effect_index);else{child.word(4,i==0?105u:static_cast<std::uint32_t>(remainder(i,5))+100u);child.word(0x18,i==0?(1000u+(p.difficulty!=0?200u:0u)):(80u+(p.difficulty!=0?10u:0u)));child.word(0x14,child.word(0x18));child.word(8,112);child.word(12,1000);child.quad(0x38,effect_at(effect_index).quad(0x60));child.quad(0x40,effect_at(effect_index).quad(0x68));}
      const auto cosine=math.cosine(angle);child.quad(0x48,m.store(m.mul(cosine,m.q(speed))));const auto sine=math.sine(angle);child.quad(0x50,m.store(m.mul(sine,m.q(speed))));child.word(0x80,high?90u:60u);
     }
     const auto shoot=[&](std::array<std::uint32_t,3> color,std::uint32_t spacing_address,std::uint32_t speed_address){const auto spacing=m.divide_integer_bits(m.c(spacing_address),integer(2));const auto count=integer(2);const auto origin_y=enemy_at(phase.boss_index).quad(0x40),origin_x=enemy_at(phase.boss_index).quad(0x38);emit({11,0,color,0,255,0,41},origin_x,origin_y,count,spacing,c(speed_address),0,0,0);};
     if(high)shoot({{0,0,255}},0x6d2248,0x6d2008);shoot({{32,32,255}},0x6d1b18,0x6d1920);if(high)shoot({{64,64,255}},0x6d2248,0x6d24e0);enemy().word(0x2c,0);
   }}
  }++integer(9);break;}

 case 6:{
  if(timeline.frame==1)move(0x6d1aa8);
  else if(timeline.frame==100){switch(p.difficulty){case 0:copy_name(context.spell_name,name_6c608c);break;case 1:copy_name(context.spell_name,name_6c60ac);break;case 2:copy_name(context.spell_name,name_6c60cc);break;case 3:copy_name(context.spell_name,name_6c60f0);break;}services.configure_phase(1,60,extra.boss_hp_multiplier*16000u,102);real_slot(0)=0x3ff0000000000000ull;real_slot(1)=0;}
  else if(timeline.frame==160){effect({{0,0,255}});mark();}
  if(frame()>=200){const auto mode=p.difficulty;real_slot(0)=m.store(m.add(m.q(real_slot(0)),m.c(mode<=1?0x6d1aa0:0x6a7708)));if(mode>2)real_slot(1)=m.store(m.add(m.q(real_slot(1)),m.c(0x6d21e0)));
   auto angle=m.store(m.sub(m.q(real_slot(0)),m.c(0x6d1868)));auto sine=math.sine(angle);put(0x38,m.add(m.mul(sine,m.c(mode<=1?0x6d1d00:0x6d1a90)),m.c(0x6d1a60)));angle=m.store(m.mul(m.sub(m.q(real_slot(0)),m.c(0x6d1868)),m.c(0x6d1ce8)));sine=math.sine(angle);put(0x40,m.add(m.mul(sine,m.c(0x6d1a40)),m.c(0x6d1aa8)));
   if(mode<=1){if(remainder(timeline.frame,100)==0){if(mode==0){const auto y=enemy().quad(0x40),x=enemy().quad(0x38);emit({11,2,{{220,0,255}},0,255,1,41},x,y,7,c(0x6d1900),c(0x6d1948),0,0,0);}else{const auto dx=m.store(m.sub(m.q(player.quad(0x90)),q(0x38))),dy=m.store(m.sub(m.q(player.quad(0x98)),q(0x40)));const auto aim=m.store(math.atan2(dy,dx));const auto y=enemy().quad(0x40),x=enemy().quad(0x38);emit({11,2,{{220,0,255}},0,255,1,41},x,y,1,0,c(0x6d2660),aim,1,0);
     constexpr std::array<std::array<std::uint32_t,2>,3> radii{{{{0x6d21d0,0x6d1e48}},{{0x6d2598,0x6d1878}},{{0x6d2910,0x6d1c58}}}};
     for(auto radii_pair:radii)for(unsigned side=0;side<2;++side){auto offset=m.store(side==0?m.add(m.q(aim),m.c(0x6d1b60)):m.sub(m.q(aim),m.c(0x6d1b60)));const auto si=math.sine(offset);const auto origin_y=m.store(m.add(m.mul(si,m.c(radii_pair[0])),q(0x40)));offset=m.store(side==0?m.add(m.q(aim),m.c(0x6d1b60)):m.sub(m.q(aim),m.c(0x6d1b60)));const auto co=math.cosine(offset);const auto origin_x=m.store(m.add(m.mul(co,m.c(radii_pair[1])),q(0x38)));emit({11,2,{{220,0,255}},0,255,1,41},origin_x,origin_y,1,0,c(0x6d2660),aim,1,0);}
    }}}
   else if(remainder(timeline.frame,mode==2?16u:8u)==0){const auto y=m.store(m.sub(q(0x40),m.c(0x6d18e8))),x=m.store(m.sub(q(0x38),m.c(0x6d1908)));emit({10,2,{{0,220,0}},0,255,1,41},x,y,1,0,c(0x6d1b28),0,0,0);}
   if(remainder(timeline.frame,60)==0){
    const auto spawn_child=[&](std::uint32_t behavior,std::uint32_t hp,std::uint32_t x_address,std::uint32_t y_address,unsigned phase_real,std::uint32_t amplitude,std::uint32_t center,std::uint32_t displacement,bool minus,std::uint32_t target_y,std::uint32_t divisor,std::uint32_t grid){boss=static_cast<std::uint32_t>(allocate_enemy(enemies));if(boss==0xffffffffu)return;enemy().word(4,rndi(4)+100u);enemy().word(0x18,hp);enemy().word(0x14,hp);enemy().word(8,behavior);enemy().quad(0x38,c(x_address));enemy().quad(0x40,c(y_address));const auto wave=math.sine(real_slot(phase_real));auto target=m.add(m.mul(wave,m.c(amplitude)),m.c(center));if(displacement)target=minus?m.sub(target,m.c(displacement)):m.add(target,m.c(displacement));put(0xa8,target);enemy().quad(0xb0,c(target_y));enemy().quad(0xb8,c(divisor));enemy().word(0x80,grid);enemy().word(12,rndi(grid-1u));};
    if(mode<=1){const auto amplitude=mode==0?0x6d1b38u:0x6b25b8u,displacement=mode==0?0x6d2700u:0x6d1ae8u,divisor=mode==0?0x6d25c0u:0x6d1a90u,grid=mode==0?8u:7u;spawn_child(113,500,0x6d1b88,0x6d1b58,0,amplitude,0x6d1a60,displacement,true,0x6d1a40,divisor,grid);spawn_child(113,500,0x6d28b0,0x6d1b58,0,amplitude,0x6d1a60,displacement,false,0x6d1a40,divisor,grid);}
    else{const auto target=mode==2?0x6d1aa8u:0x6d1d50u,divisor=mode==2?0x6d1fe8u:0x6d1ec8u,grid=mode==2?12u:25u;spawn_child(113,500,0x6d1908,0x6d1b58,0,0x6d1aa8,0x6d1a60,0,false,target,divisor,grid);spawn_child(113,500,0x6d3068,0x6d1b58,0,0x6d1aa8,0x6d1a60,0,false,target,divisor,grid);if(mode!=2){spawn_child(114,100,0x6d1bb8,0x6d1898,1,0x6d1aa8,0x6d1af0,0,false,0x6d1d50,0x6d1ec8,25);spawn_child(114,100,0x6d1bb8,0x6d1b78,1,0x6d1aa8,0x6d1af0,0,false,0x6d1d50,0x6d1ec8,25);}}
   }enemy().word(0x2c,0);
  }break;}

 case 9:{
  const auto vector=[&](unsigned index,unsigned component){return binary_float::finite_binary32(runtime.vectors.word(index*12+component*4));};
  if(character==0){
   if(timeline.frame==70){switch(p.difficulty){case 0:copy_name(context.spell_name,name_6c6108);break;case 1:copy_name(context.spell_name,name_6c6120);break;case 2:copy_name(context.spell_name,name_6c6138);break;case 3:copy_name(context.spell_name,name_6c6154);break;}services.configure_phase(1,60,16000,106);}
   else if(timeline.frame==160){enemy().word(8,0);enemy().quad(0x38,c(0x6d1a60));enemy().quad(0x40,c(0x6d1af0));effect({{255,0,0}});mark();}
   else if(timeline.frame==200){integer(0)=60;integer(1)=120;integer(19)=pick({{5,9,13,15,0,0}});real_slot(0)=c(0x6d1ce8);real_slot(1)=c(0x6d1970);real_slot(3)=0;real_slot(4)=real({{c(0x6d2130),c(0x6d2130),c(0x6d2130),c(0x6d2660),0,0}});std::uint64_t angle=0;
    for(unsigned i=0;i<12;++i){const auto index=static_cast<std::uint32_t>(allocate_projectile(projectiles,{1,0,{{255,0,0}},2,255,1,41}));if(index!=0xffffffffu){auto b=bullet(index);b.word(8,102);b.word(0x34,1);b.word(0x78,pick({{64,50,44,38,0,0}}));b.quad(0xa0,real({{c(0x6d2660),c(0x6d1ce8),c(0x6d2548),c(0x6d1b18),0,0}}));b.quad(0xa8,angle);integer(i+3)=index;}const auto cosine=math.cosine(angle);runtime.vectors.word(i*12,binary_float::to_binary32(cosine));const auto sine=math.sine(angle);runtime.vectors.word(i*12+4,binary_float::to_binary32(sine));runtime.vectors.word(i*12+8,0);angle=m.store(m.add(m.q(angle),m.c(0x6d2140)));}enemy().word(0x2c,0);
   }
   if(frame()>=200){stage6_boss_timeline_ease_real(real_slot(3),c(0x6d18c0),c(0x6d1b38),bits);for(unsigned i=0;i<12;++i){auto* point=runtime.vectors.data.data()+i*12;stage6_rotate_float3(1,point,point,c(0x6d1ab8),bits);stage6_rotate_float3(2,point,point,c(0x6d1b68),bits);auto b=bullet(integer(i+3));b.quad(0x40,m.store(m.add(m.mul(vector(i,0),m.q(real_slot(3))),m.c(0x6d1a60))));b.quad(0x48,m.store(m.add(m.mul(vector(i,1),m.q(real_slot(3))),q(0x40))));b.word(0x30,convert(m.div(m.mul(vector(i,2),m.c(0x6d1d00)),m.c(0x6d2f00)))+200u);}
    if(remainder(timeline.frame,70)==60){std::uint64_t angle=0;if(p.difficulty==3)angle=m.store(m.mul(rnd(0x6d1c60),m.c(0x6d1950)));const auto speed=real_slot(4);const auto spacing=m.divide_integer_bits(m.c(0x6d2208),integer(19));const auto count=integer(19);const auto y=enemy().quad(0x40),x=enemy().quad(0x38);emit({2,0,{{255,0,0}},2,255,1,43},x,y,count,spacing,speed,angle,0,1);}
   }
  }else if(character==1){
   if(timeline.frame==70){switch(p.difficulty){case 0:copy_name(context.spell_name,name_6c6170);break;case 1:copy_name(context.spell_name,name_6c6190);break;case 2:copy_name(context.spell_name,name_6c61b0);break;case 3:copy_name(context.spell_name,name_6c61cc);break;}services.configure_phase(1,60,23000,110);}
   else if(timeline.frame==160){enemy().word(8,0);enemy().quad(0x38,c(0x6d1a60));enemy().quad(0x40,c(0x6d1af0));effect({{0,0,255}});mark();}
   else if(timeline.frame==200){real_slot(0)=m.store(m.mul(rnd(0x6d1c60),m.c(0x6d1950)));real_slot(1)=m.store(m.mul(rnd(0x6d1c60),m.c(0x6d1950)));real_slot(2)=m.store(m.mul(rnd(0x6d1c60),m.c(0x6d1950)));real_slot(3)=c(0x6d1860);real_slot(5)=0;real_slot(7)=0;real_slot(6)=m.store(m.i(pick({{4,5,7,8,0,0}})));real_slot(10)=real({{c(0x6d21a0),c(0x6d2f28),c(0x6d3058),c(0x6d3060),0,0}});real_slot(11)=real({{c(0x6d19e0),c(0x6d3040),c(0x6d3048),c(0x6d3050),0,0}});real_slot(12)=real({{c(0x6d2238),c(0x6d3028),c(0x6d3030),c(0x6d3038),0,0}});
    for(unsigned i=0;i<8;++i){const auto index=static_cast<std::uint32_t>(allocate_projectile(projectiles,{2,0,{{0,0,255}},2,255,1,41}));if(index!=0xffffffffu){auto b=bullet(index);b.quad(0x40,c(0x6d1a60));b.quad(0x40,c(0x6d1af0));b.word(8,25);b.word(0x34,1);integer(i)=index;}}
    for(unsigned i=0;i<8;++i){runtime.vectors.word(i*12,i&4?0xbf800000u:0x3f800000u);runtime.vectors.word(i*12+4,i&2?0xbf800000u:0x3f800000u);runtime.vectors.word(i*12+8,i&1?0xbf800000u:0x3f800000u);}enemy().word(0x2c,0);
   }else if(timeline.frame==201){constexpr std::array<std::array<unsigned,2>,12> edges{{{{0,1}},{{0,2}},{{0,4}},{{1,3}},{{1,5}},{{2,3}},{{2,6}},{{3,7}},{{4,5}},{{4,6}},{{5,7}},{{6,7}}}};
    for(auto edge:edges)for(std::uint32_t j=1;signed_word(j)<signed_word(convert(m.q(real_slot(6))));++j){const auto index=static_cast<std::uint32_t>(allocate_projectile(projectiles,{9,0,{{0,0,255}},2,255,1,41}));if(index==0xffffffffu)continue;auto b=bullet(index);b.word(8,103);b.word(0x34,1);b.word(0x78,integer(edge[0]));b.word(0x7c,integer(edge[1]));b.quad(0xa0,m.store(m.div(m.i(j),m.q(real_slot(6)))));b.quad(0x40,c(0x6d1a60));b.quad(0x48,c(0x6d1af0));}
   }
   if(frame()>=200){real_slot(0)=m.store(m.add(m.q(real_slot(0)),m.c(0x6d3020)));real_slot(1)=m.store(m.add(m.q(real_slot(1)),m.c(0x6d3018)));real_slot(2)=m.store(m.add(m.q(real_slot(2)),m.c(0x6d3010)));real_slot(3)=m.store(m.add(m.q(real_slot(3)),m.c(0x6d2f18)));stage6_boss_timeline_ease_real(real_slot(5),real_slot(6),c(0x6d1cc0),bits);stage6_boss_timeline_ease_real(real_slot(6),c(0x6d2240),c(0x6d1cc0),bits);const auto wave=math.sine(real_slot(3));const auto radius=m.store(m.add(m.mul(wave,m.c(0x6b25f8)),m.q(real_slot(5))));
    for(unsigned i=0;i<8;++i){auto* point=runtime.vectors.data.data()+i*12;for(unsigned axis=0;axis<3;++axis){const auto sine=math.sine(real_slot(axis));const auto angle=m.store(m.add(m.mul(sine,m.c(0x6d2f10)),m.q(real_slot(axis+10))));stage6_rotate_float3(axis,point,point,angle,bits);}auto b=bullet(integer(i));b.quad(0x40,m.store(m.add(m.mul(vector(i,0),m.q(radius)),m.c(0x6d1a60))));b.quad(0x48,m.store(m.add(m.mul(vector(i,1),m.q(radius)),m.c(0x6d1af0))));b.word(0x30,convert(m.div(m.mul(vector(i,2),m.c(0x6d2f08)),m.c(0x6d2f00)))+168u);}
   }
  }else{
   if(timeline.frame==70){switch(p.difficulty){case 0:copy_name(context.spell_name,name_6c61e8);break;case 1:copy_name(context.spell_name,name_6c6208);break;case 2:copy_name(context.spell_name,name_6c6228);break;case 3:copy_name(context.spell_name,name_6c6244);break;}services.configure_phase(1,60,14000,114);}
   else if(timeline.frame==160){enemy().word(8,0);enemy().quad(0x38,c(0x6d1a60));enemy().quad(0x40,c(0x6d1a90));effect({{0,255,0}});integer(0)=pick({{15,11,16,13,0,0}});integer(1)=pick({{28,22,17,14,0,0}});integer(2)=128;real_slot(0)=real({{c(0x6d1ff8),c(0x6d2130),c(0x6d1ce8),c(0x6d1b18),0,0}});real_slot(1)=m.store(m.i(pick({{348,378,388,398,0,0}})));mark();}
   if(frame()>=200){if(timeline.frame==200)enemy().word(0x2c,0);
    if(remainder(timeline.frame,integer(0))==0){if(greater(m.q(real_slot(1)),m.i(integer(2))))integer(2)+=12;const auto leaf=[&](bool right){const auto index=static_cast<std::uint32_t>(allocate_projectile(projectiles,{11,1,{{0,255,0}},0,255,0,41}));if(index==0xffffffffu)return;auto b=bullet(index);b.word(8,104);const auto x=m.mul(rnd(0x6d1ab0),m.c(0x6a7708));b.quad(0x40,m.store(right?m.sub(m.c(0x6d3000),x):m.add(x,m.c(0x6d3008))));const auto y=random.real_number(m.store(m.i(integer(2)*10u)),precision);b.quad(0x48,m.store(m.add(m.mul(y,m.c(0x6d1960)),m.c(0x6d1e48))));const auto speed=m.mul(rnd(0x6d1f08),m.c(0x6a7708));b.quad(0x50,m.store(right?m.sub(m.c(0x6d2a88),speed):m.add(speed,m.c(0x6d1948))));b.quad(0x60,right?c(0x6d1860):0);};leaf(false);if(signed_word(p.difficulty)>=2)leaf(true);}
    if(frame()>=300&&remainder(timeline.frame,integer(1))==0){const auto blue=rndi(127)+128u;const auto red=rndi(127)+128u;const auto index=static_cast<std::uint32_t>(allocate_projectile(projectiles,{2,2,{{red,0,blue}},2,255,1,41}));if(index!=0xffffffffu){auto b=bullet(index);b.word(8,105);b.quad(0x40,m.store(m.add(m.mul(rnd(0x6d2ff8),m.c(0x6d1960)),m.c(0x6d1a80))));b.quad(0x48,m.store(m.add(rnd(0x6d1ae8),m.c(0x6d1a80))));b.quad(0x50,m.store(m.add(m.mul(rnd(0x6d1a78),m.c(0x6d1950)),m.c(0x6d25b8))));const auto speed=rnd(0x6d1cc0);b.quad(0x58,m.store(m.add(m.mul(speed,m.c(0x6a7708)),m.q(real_slot(0)))));}}
   }
  }break;}

 case 11:{
  auto& circle=projectiles.secondary[0];const auto shrink=[&](std::uint32_t amount){circle.quad(0x28,m.store(m.sub(m.q(circle.quad(0x28)),m.c(amount))));if(greater({},m.q(circle.quad(0x28))))circle.quad(0x28,0);};
  if(player.word(0x2c)==0)circle.quad(0x28,m.store(m.sub(m.q(circle.quad(0x28)),m.c(0x6d1a08))));real_slot(1)=real({{c(0x6d1d50),c(0x6d2740),c(0x6d1ae8),c(0x6b25f8),0,0}});if(greater({},m.q(circle.quad(0x28))))circle.quad(0x28,0);
  if(timeline.frame==1){switch(p.difficulty){case 0:copy_name(context.spell_name,name_6c6260);break;case 1:copy_name(context.spell_name,name_6c6278);break;case 2:copy_name(context.spell_name,name_6c6290);break;case 3:copy_name(context.spell_name,name_6c62a0);break;}services.configure_phase(1,180,26000,118);enemy().quad(0xa8,c(0x6d1a60));enemy().quad(0xb0,c(0x6d18c0));enemy().quad(0xb8,c(0x6d1aa8));integer(0)=0;integer(1)=0;integer(2)=pick({{1,1,3,3,0,0}});integer(3)=pick({{1,2,3,5,0,0}});integer(4)=pick({{1,2,2,3,0,0}});integer(5)=pick({{1,1,3,3,0,0}});real_slot(0)=real({{c(0x6d2268),c(0x6d2b38),c(0x6d2fe8),c(0x6d2ff0),0,0}});}
  else if(timeline.frame==61)effect({{255,0,0}});
  else if(timeline.frame==101){enemy().quad(0x38,c(0x6d1a60));enemy().quad(0x40,c(0x6d18c0));
   const auto item_begin=[&](std::uint32_t index,bool special)->Item&{auto& item=items.items.at(index);item.word(0,1);item.word(0x10,2);item.word(8,special?1u:0u);item.word(4,0);item.word(12,0);item.quad(0x18,enemy().quad(0x38));if(greater({},m.q(item.quad(0x18))))item.quad(0x18,0);else if(greater(m.q(item.quad(0x18)),m.c(0x6d1cb0)))item.quad(0x18,c(0x6d1cb0));item.quad(0x20,enemy().quad(0x40));return item;};
   for(std::uint32_t ring=0;ring<=2;++ring){const auto base=m.store(m.mul(rnd(0x6d1c60),m.c(0x6d1950)));for(std::uint32_t j=0;j<=19;++j){const auto index=static_cast<std::uint32_t>(find_free_item(items));if(index==0xffffffffu)continue;auto& item=item_begin(index,false);const auto part=m.mul(m.i(j),m.c(0x6d1860));const auto angle=m.store(m.add(m.div(m.add(part,part),m.c(0x6d1a40)),m.q(base)));const auto speed=m.store(m.div(m.i(ring*12u+48u),m.c(0x6d1a40)));const auto cosine=math.cosine(angle);item.quad(0x28,m.store(m.mul(cosine,m.q(speed))));const auto sine=math.sine(angle);item.quad(0x30,m.store(m.mul(sine,m.q(speed))));item.quad(0x38,c(0x6d23b0));}}
   const auto index=static_cast<std::uint32_t>(find_free_item(items));if(index!=0xffffffffu){auto& item=item_begin(index,true);const auto cosine=math.cosine(0);item.quad(0x28,m.store(m.mul(cosine,{})));const auto sine=math.sine(0);item.quad(0x30,m.store(m.mul(sine,{})));item.quad(0x38,c(0x6d23b0));}
  }else if(timeline.frame==240)effect({{255,255,255}});
  if(frame()<280){shrink(0x6d1fa8);break;}
  if(player.word(0x2c)!=0){protection.boss_state_a7ea60=1;allocation.field_a7ea58=0;}else{protection.boss_state_a7ea60=0;allocation.field_a7ea58=1;}
  const auto create_enemy=[&](std::uint32_t color,std::uint32_t hp,std::uint32_t behavior,std::uint32_t x_address,std::uint32_t y_address)->std::uint32_t{const auto source_index=static_cast<std::uint32_t>(colored(color,hp,behavior,c(x_address),c(y_address)));const auto index=static_cast<std::uint32_t>(allocate_enemy(enemies));if(index==0xffffffffu)return index;auto& target=enemy_at(index);auto& source=effect_at(source_index);target.word(4,source.word(0x10)-100u);target.word(0x18,source.word(0x14));target.word(0x14,target.word(0x18));target.word(8,source.word(0x18));target.quad(0x38,source.quad(0x60));target.quad(0x40,source.quad(0x68));return index;};
  if(signed_word(p.difficulty)>=1){const auto dx=m.store(m.sub(m.q(enemy_at(phase.boss_index).quad(0x38)),m.q(player.quad(0x90)))),dy=m.store(m.sub(m.q(enemy_at(phase.boss_index).quad(0x40)),m.q(player.quad(0x98))));const auto squared=m.store(m.add(m.mul(m.q(dx),m.q(dx)),m.mul(m.q(dy),m.q(dy))));const auto distance=finite_square_root(squared,bits);if(!greater(distance,m.c(0x6d1d50))){if(remainder(timeline.frame,50)==0){const auto index=create_enemy(5,1,138,0x6d1ae8,0x6d1a40);if(index!=0xffffffffu)enemy_at(index).quad(0xa8,c(0x6d1948));}else if(remainder(timeline.frame,50)==25){const auto index=create_enemy(5,1,138,0x6d23e0,0x6d1a40);if(index!=0xffffffffu)enemy_at(index).quad(0xa8,c(0x6d1948));}}}
  const auto clear_enemies=[&](){for(std::uint32_t i=0;i<80;++i){auto& child=enemy_at(i);if(child.word(0)!=1)continue;const auto kind=signed_word(child.word(4));if(kind>9&&kind<100)continue;child.word(0x14,0);child.word(0x1c,0);services.resolve_enemy_death(i);}};
  const auto global_effect=[&](std::array<std::uint32_t,3> color){const auto y=enemy_at(phase.boss_index).quad(0x40),x=enemy_at(phase.boss_index).quad(0x38);const auto packed=services.color(color[0],color[1],color[2]);allocate_effect(effects,projectiles.sound_triggers,{12,packed,x,y});};
  const auto state=integer(0);
  if(state<=4){constexpr std::array<std::uint32_t,5> decrements{{0x6d1aa0,0x6d2510,0x6d24f8,0x6d19b8,0x6d1900}};shrink(decrements[state]);
   bool spawn=true;if(state==4&&integer(1)!=0){--integer(1);spawn=false;}
   if(spawn){
    if(state==4){const auto cycle=remainder(timeline.frame,360);if(cycle==0||cycle==120||cycle==240){const auto color=cycle==0?std::array<std::uint32_t,3>{{0,0,255}}:cycle==120?std::array<std::uint32_t,3>{{0,255,0}}:std::array<std::uint32_t,3>{{255,0,0}};const auto y=enemy().quad(0x40),x=enemy().quad(0x38);emit({11,0,color,0,255,1,41},x,y,9,c(0x6d1a18),c(0x6d1910),0,0,0);}}
    const auto cycle=remainder(timeline.frame,42);
    const auto spawn_lane=[&](unsigned lane,std::uint32_t hp){constexpr std::array<std::uint32_t,6> behavior{{116,117,116,117,116,117}};constexpr std::array<std::uint32_t,6> y{{0x6d1d00,0x6b25b8,0x6d1a90,0x6d1aa8,0x6d1ae8,0x6d1a40}};constexpr std::array<std::uint32_t,6> distance{{0x6d1ae8,0x6d1aa8,0x6d1a90,0x6b25b8,0x6d1d00,0x6d1f08}};constexpr std::array<std::uint32_t,6> period{{336,336,336,420,462,504}};const auto index=create_enemy(lane,hp,behavior[lane],0x6d1a60,y[lane]);if(index!=0xffffffffu){auto& child=enemy_at(index);child.quad(0xa8,c(0x6d1de0));child.quad(0xb0,c(distance[lane]));child.word(0x80,period[lane]);}};
    constexpr std::array<std::uint32_t,5> common_hp{{160,120,80,20,40}};
    if(cycle==0){spawn_lane(0,common_hp[state]);if(state>=1){constexpr std::array<std::uint32_t,5> hp{{0,320,160,40,100}};spawn_lane(3,hp[state]);}}
    else if(cycle==14){spawn_lane(1,common_hp[state]);if(state>=2){constexpr std::array<std::uint32_t,5> hp{{0,0,320,60,200}};spawn_lane(4,hp[state]);}}
    else if(cycle==28){spawn_lane(2,common_hp[state]);if(state>=3)spawn_lane(5,state==3?330u:500u);}
   }
   if(state<=2){if(integer(1)!=0){--integer(1);if(integer(1)==0)++integer(0);}else{constexpr std::array<std::uint32_t,3> threshold{{18000,12000,6000}};if(signed_word(enemy_at(phase.boss_index).word(0x14))<=threshold[state]){clear_enemies();integer(1)=40;global_effect(state==0?std::array<std::uint32_t,3>{{0,255,255}}:std::array<std::uint32_t,3>{{255,0,255}});}}}
  }
  if(signed_word(integer(0))<=2&&signed_word(protection.field_a7eaa4)<=29){integer(0)=4;clear_enemies();integer(1)=40;global_effect({{255,0,255}});}break;}






 case 8:if(timeline.frame==1){enemy().word(8,14);enemy().quad(0xa8,c(0x6d1a60));enemy().quad(0xb0,m.store(m.i(who({{224,224,80}}))));timeline.subphase=9;timeline.frame=0;}break;
 case 10:{(void)random.real_number(0,precision);auto& circle=projectiles.secondary[0];if(timeline.frame==0)protection.boss_state_a7ea60=2;else if(timeline.frame==60){enemy().word(8,90);enemy().quad(0xa8,c(0x6d1a60));enemy().quad(0xb0,c(0x6d18c0));enemy().quad(0xb8,c(0x6d1ac8));}else if(timeline.frame==160){circle.word(0,1);projectiles.sound_triggers[18]=1;circle.quad(0x10,c(0x6d1a60));circle.quad(0x18,c(0x6d1c70));circle.quad(0x20,c(0x6d1e38));circle.quad(0x28,c(0x6d1d00));circle.quad(0x30,0x3ff0000000000000ull);circle.quad(0x38,0x3ff0000000000000ull);circle.quad(0x40,0x3ff0000000000000ull);circle.word(4,106);circle.word(12,0);circle.word(0x48,0);mark();}
  if(greater(m.add(m.q(circle.quad(0x28)),m.c(0x6d1868)),m.q(circle.quad(0x20)))&&stage5_boss_secondary_contact(projectiles,player,bits)){++timeline.subphase;timeline.frame=0;circle.word(0x48,1);circle.quad(0x20,circle.quad(0x28));}break;}
 case 12:if(timeline.frame==32)services.retire_boss(0);else if(timeline.frame==196){++timeline.subphase;timeline.frame=0;}break;
 case 13:if(timeline.frame==1)services.award_stage_clear();else if(timeline.frame==256)runtime.ending.word(0x20,1);else if(timeline.frame==511){services.pause_background();services.music(15,1);runtime.outer_scene=101;if(player.word(0x24)!=0){runtime.ending.word(8,character*2u+1u);runtime.ending.word(12,3);}else{runtime.ending.word(8,character*2u);runtime.ending.word(12,0);}runtime.ending.word(0x20,300);runtime.ending.word(0,0);runtime.ending.word(4,0);for(unsigned i=0;i<4;++i)runtime.ending.data[0x24+i*96]=0;}break;
 default:break;
 }
}
}
