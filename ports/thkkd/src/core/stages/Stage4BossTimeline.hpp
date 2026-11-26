// 原68D980..692121完整十一阶段，保留Boss入口槽缓存及角色选择。
#pragma once
#include "core/stages/Stage1Timeline.hpp"
#include "core/stages/StageBossServices.hpp"
#include "core/stages/StagePatternContext.hpp"
#include "core/stages/StageResourceContext.hpp"
#include "core/entities/BossPhaseContext.hpp"
#include "core/entities/BossAllocationContext.hpp"
#include "core/entities/EnemyDeathContext.hpp"
#include "core/shared/EffectPool.hpp"
#include "core/entities/ProjectileEmitter.hpp"
#include "core/player/OptionsMotion.hpp"
#include "core/stages/Stage3BossTimelineContext.hpp"
#include "core/stages/Stage4PatternContext.hpp"
#include "core/stages/Stage4BossTimelineArithmetic.hpp"
namespace kaikeidou {
inline void stage4_boss_segment(StageTimeline& timeline,EnemyPool& enemies,ProjectilePools& projectiles,
 EffectPool& effects,ObjectRecord<240>& player,PatternParameters& p,BossPhaseContext& phase,BossAllocationContext& allocation,BossStageContext& context,
 StagePatternContext& extra,Stage3BossTimelineContext& extended,Stage4PatternContext& stage4,EnemyDeathContext& death,StageResourceContext& resources,
 const std::uint32_t& character,Random& random,RandomPrecision precision,GameMath& math,StageBossServices& services,StageResourceServices& platform){
 (void)resources;(void)platform;using namespace stage4_boss_timeline_detail;using Number=binary_float::Number;
 const auto bits=static_cast<unsigned>(precision);const Arithmetic m{bits};const auto boss=phase.boss_index;
 const auto enemy=[&]()->Enemy&{return boss==0xffffffffu?context.enemy_prefix:enemies.enemies.at(boss);};
 const auto c=[](std::uint32_t a){return constant(a);};
 const auto integer=[&](unsigned i)->std::uint32_t&{if(i<5)return p.integers[i];if(i==5)return death.pattern_integer_5;if(i<20)return extra.integers_6_to_19[i-6];return stage4.integers_20_to_39.at(i-20);};
 const auto real_slot=[&](unsigned i)->std::uint64_t&{if(i<2)return p.reals[i];if(i<4)return context.extra_reals[i-2];if(i==4)return extra.real_4;if(i<10)return extended.reals_5_to_9[i-5];if(i<20)return extra.reals_10_to_19[i-10];return extended.reals_20_to_29.at(i-20);};
 const auto pick=[&](std::array<std::uint32_t,6> v){return stage4_boss_timeline_pick_integer(p.difficulty,v);};
 const auto real=[&](std::array<std::uint64_t,6> v){return stage4_boss_timeline_pick_real(p.difficulty,v);};
 const auto who=[&](std::array<std::uint32_t,3> v){return stage4_boss_timeline_pick_character(character,v);};
 const auto colors=[&](unsigned start){return std::array<std::uint32_t,3>{{integer(start),integer(start+1),integer(start+2)}};};
 const auto effect_at=[&](std::uint32_t kind,std::uint64_t x,std::uint64_t y,unsigned rgb){auto color=colors(rgb);auto packed=services.color(color[0],color[1],color[2]);allocate_effect(effects,projectiles.sound_triggers,{kind,packed,x,y});};
 const auto effect=[&](unsigned rgb){const auto y=enemy().quad(0x40),x=enemy().quad(0x38);effect_at(12,x,y,rgb);};
 const auto mark=[&](){enemy().word(0x2c,3);enemy().word(0x30,0);enemy().word(0x34,0);};
 const auto move=[&](std::uint32_t y){enemy().word(8,14);enemy().quad(0xa8,c(0x6d1a60));enemy().quad(0xb0,c(y));};
 const auto spawn=[&](unsigned rgb,std::uint32_t kind=22,std::uint32_t sound=41,std::uint32_t delayed=1,std::int32_t group=1,std::uint32_t blend=0){return allocate_projectile(projectiles,{kind,group,colors(rgb),blend,255,delayed,sound});};
 const auto bullet=[&](std::uint32_t slot){return BulletView{projectiles,slot*240u};};
 const auto r=[&](std::size_t offset){return m.q(enemy().quad(offset));};const auto w=[&](std::size_t offset,Number v){enemy().quad(offset,m.store(v));};
 const auto emit=[&](ProjectileAllocation request,std::uint64_t x,std::uint64_t y,std::uint32_t count,std::uint64_t spacing,std::uint64_t speed,std::uint64_t angle,std::uint32_t absolute,std::uint32_t configuration){emit_projectiles(projectiles,{request,x,y,count,spacing,speed,angle,absolute,configuration},player.quad(0x90),player.quad(0x98),p,random,precision,math);};
 switch(timeline.subphase){
 case 0:
  if(timeline.frame==1){integer(0)=who({{255,64,64}});integer(1)=who({{64,64,255}});integer(2)=who({{64,255,64}});
   integer(10)=who({{64,64,255}});integer(11)=who({{64,255,64}});integer(12)=who({{255,64,64}});
   integer(20)=who({{64,255,64}});integer(21)=who({{255,64,64}});integer(22)=who({{64,64,255}});services.dialog(7);}
  break;
 case 1:
  if(timeline.frame==1)services.begin_boss_effect();else if(timeline.frame==41)effect(10);
  else if(timeline.frame==61){effect(20);mark();}
  else if(timeline.frame==101){allocation.field_a7ea5c=255;services.configure_phase(0,40,extra.boss_hp_multiplier*12000u,0xfffffff6u);enemy().word(8,74);enemy().quad(0xa8,0);enemy().quad(0xb0,0);
   enemy().word(0x80,pick({{90,80,70,60,0,0}}));enemy().quad(0xb8,m.store(m.i(pick({{16,12,15,13,0,0}}))));integer(3)=0;}
  break;
 case 2:
  if(timeline.frame==1)move(0x6d1d50);
  else if(timeline.frame==21){enemy().word(8,77);mark();const bool easy=signed_word(p.difficulty)<=1;
   auto slot=spawn(10);if(slot!=-1){auto b=bullet(static_cast<std::uint32_t>(slot));b.word(0x78,who({{1,2,0}}));b.word(0x7c,0);b.word(0x80,0xffffffffu);b.quad(0xa0,easy?0:c(0x6d1860));b.quad(0xa8,0);b.quad(0xb0,c(0x6d2588));b.quad(0xb8,c(0x6d2668));b.quad(0x40,c(easy?0x6d3120:0x6d1df0));b.quad(0x48,b.quad(0xb0));b.word(8,55);b.word(0x34,1);if(!easy){b.word(0x98,0);b.word(0x9c,0);}}
   slot=spawn(20);if(slot!=-1){auto b=bullet(static_cast<std::uint32_t>(slot));b.word(0x78,who({{2,0,1}}));b.word(0x7c,1);b.word(0x80,easy?0xfffffffeu:0xffffffffu);b.quad(0xa0,easy?c(0x6d1860):0);b.quad(0xa8,0);b.quad(0xb0,c(easy?0x6d19e8:0x6d2588));b.quad(0xb8,c(easy?0x6d1b70:0x6d2668));b.quad(0x40,c(easy?0x6d3118:0x6d1df8));b.quad(0x48,b.quad(0xb0));b.word(8,55);b.word(0x34,1);if(!easy){b.word(0x98,0);b.word(0x9c,0);}}
   integer(3)=pick({{9,6,4,3,0,0}});real_slot(0)=c(0x6d1de0);real_slot(1)=0;real_slot(2)=real({{c(0x6d1e40),c(0x6d1948),c(0x6d1b18),c(0x6d1ce0),0,0}});real_slot(3)=real({{c(0x6d1cf0),c(0x6d1cf0),c(0x6d3110),c(0x6d3110),0,0}});
  }else if(timeline.frame==61){switch(p.difficulty){case 0:copy_name(context.spell_name,name_6c653c);break;case 1:copy_name(context.spell_name,name_6c6558);break;case 2:copy_name(context.spell_name,name_6c6574);break;case 3:copy_name(context.spell_name,name_6c6588);break;}services.configure_phase(1,50,extra.boss_hp_multiplier*12000u,43);}
  else if(timeline.frame==130){const bool easy=signed_word(p.difficulty)<=1;effect_at(10,c(easy?0x6d3120:0x6d1df0),c(0x6d2588),10);effect_at(10,c(easy?0x6d3118:0x6d1df8),c(easy?0x6d19e8:0x6d2588),20);}
  break;
 case 3:
  if(timeline.frame==1)move(0x6b25b8);
  else if(timeline.frame==61){effect(20);mark();}
  else if(timeline.frame==101){services.configure_phase(0,40,extra.boss_hp_multiplier*9000u,0xfffffff6u);enemy().quad(0x38,c(0x6d1a60));enemy().quad(0x40,c(0x6b25b8));enemy().word(8,75);enemy().quad(0xa8,0);enemy().quad(0xb0,0);
   enemy().quad(0xb8,real({{c(0x6d1c80),c(0x6d25f0),c(0x6d1a98),c(0x6d2150),0,0}}));enemy().quad(0xc0,real({{c(0x6d1a00),c(0x6d1e40),c(0x6d1c80),c(0x6d1ff8),0,0}}));enemy().word(0x80,pick({{5,4,3,3,0,0}}));enemy().word(0x84,pick({{9,7,5,4,0,0}}));enemy().word(0x88,pick({{7,4,3,3,0,0}}));integer(3)=0;
  }else if(timeline.frame==161){effect(10);mark();}break;
 case 4:
  if(timeline.frame==1)move(0x6d1af0);else if(timeline.frame==61)effect(10);
  else if(timeline.frame==81){effect(20);mark();enemy().word(8,0);enemy().quad(0x38,c(0x6d1a60));enemy().quad(0x40,c(0x6d1af0));
   for(unsigned side=0;side<2;++side){auto slot=spawn(side?20:10);if(slot!=-1){auto b=bullet(static_cast<std::uint32_t>(slot));b.word(0x78,who(side?std::array<std::uint32_t,3>{{2,0,1}}:std::array<std::uint32_t,3>{{1,2,0}}));b.word(0x7c,side);b.quad(0x40,c(side?0x6d3120:0x6d3118));b.quad(0x48,c(0x6d1af0));b.word(8,57);b.word(0x34,1);b.word(0x98,0);b.word(0x9c,0);
    if(b.word(0x78)==0){b.quad(0x50,c(0x6d3108));b.quad(0x58,c(0x6d3100));}else if(b.word(0x78)==1){b.quad(0x50,c(0x6d1c90));b.quad(0x58,c(0x6d25a8));}else{b.quad(0x50,c(0x6d30f8));b.quad(0x58,0);}
   }}
   integer(3)=pick({{18,15,10,9,0,0}});real_slot(2)=0;real_slot(3)=c(0x6d1860);
   real_slot(4)=real({{c(0x6d30e8),c(0x6d30f0),c(0x6d19e0),c(0x6d2238),0,0}});real_slot(5)=real({{c(0x6d30e8),c(0x6d30f0),c(0x6d30d8),c(0x6d30e0),0,0}});real_slot(6)=real({{c(0x6d1a00),c(0x6d1b28),c(0x6d1ce8),c(0x6d1b18),0,0}});
   auto sine=math.sine(real_slot(2));real_slot(0)=m.store(m.add(m.mul(sine,m.q(real_slot(4))),m.q(real_slot(4))));sine=math.sine(real_slot(3));real_slot(1)=m.store(m.add(m.mul(sine,m.q(real_slot(5))),m.q(real_slot(5))));
  }else if(timeline.frame==121){switch(p.difficulty){case 0:copy_name(context.spell_name,name_6c659c);break;case 1:copy_name(context.spell_name,name_6c65bc);break;case 2:copy_name(context.spell_name,name_6c65dc);break;case 3:copy_name(context.spell_name,name_6c65f4);break;}services.configure_phase(1,50,extra.boss_hp_multiplier*24000u,47);
   for(std::uint32_t i=0;i<=7;++i){auto slot=spawn(10+10*(i%2),2,41,1,2,2);if(slot!=-1){auto b=bullet(static_cast<std::uint32_t>(slot));auto a=m.store(m.mul(m.i(i),m.c(0x6d22d8)));auto cosine=math.cosine(a);b.quad(0x40,m.store(m.add(m.mul(cosine,m.c(0x6d1d18)),m.c(0x6d1a60))));a=m.store(m.mul(m.i(i),m.c(0x6d22d8)));auto sine=math.sine(a);b.quad(0x48,m.store(m.add(m.mul(sine,m.c(0x6d1d18)),m.c(0x6d1af0))));b.word(8,58);
    auto dx=m.store(m.sub(m.q(b.quad(0x40)),m.c(0x6d1a60))),dy=m.store(m.sub(m.q(b.quad(0x48)),m.c(0x6d1af0)));b.quad(0xa0,m.store(math.atan2(dy,dx)));auto sq=m.store(m.add(m.mul(m.q(dx),m.q(dx)),m.mul(m.q(dy),m.q(dy))));b.quad(0xa8,m.store(finite_square_root(sq,bits)));b.quad(0xb0,c(0x6d1860));b.word(0x7c,0u-(i%2)-1u);b.word(0x34,1);b.word(0xc,0xfffe7961u);
   }}
  }else if(timeline.frame==130){effect_at(10,c(0x6d3118),c(0x6d1af0),10);effect_at(10,c(0x6d3120),c(0x6d1af0),20);}
  if(signed_word(timeline.frame)>=81){auto saved=m.store(m.mul(m.q(real_slot(4)),m.q(real_slot(6))));auto sine=math.sine(real_slot(2));real_slot(0)=m.store(m.add(m.mul(sine,m.q(real_slot(4))),m.q(saved)));saved=m.store(m.mul(m.q(real_slot(5)),m.q(real_slot(6))));sine=math.sine(real_slot(3));real_slot(1)=m.store(m.add(m.mul(sine,m.q(real_slot(5))),m.q(saved)));real_slot(2)=m.store(m.add(m.q(real_slot(2)),m.c(0x6d2a58)));real_slot(3)=m.store(m.add(m.q(real_slot(3)),m.c(0x6d2a58)));}
  break;
 case 5:
  if(timeline.frame==1)move(0x6d1af0);else if(timeline.frame==41)effect(10);else if(timeline.frame==61){effect(20);mark();}
  else if(timeline.frame==101){services.configure_phase(0,40,extra.boss_hp_multiplier*8000u,0xfffffff6u);enemy().quad(0x38,c(0x6d1a60));enemy().quad(0x40,c(0x6d1af0));enemy().word(8,76);enemy().quad(0xa8,0);enemy().quad(0xb0,c(0x6d1860));enemy().word(0x80,pick({{15,14,13,12,0,0}}));enemy().word(0x84,pick({{2,4,5,6,0,0}}));w(0xb8,m.div(m.c(0x6d2208),m.i(enemy().word(0x84)+1u)));enemy().quad(0xc0,c(0x6d19c8));enemy().quad(0xc8,c(0x6d30d0));integer(3)=0;
   auto rate=real({{c(0x6d18b0),c(0x6d19d8),c(0x6d1880),c(0x6d2bb0),0,0}});real_slot(0)=m.store(m.mul(m.q(rate),m.c(0x6d2828)));rate=real({{c(0x6d18b0),c(0x6d19d8),c(0x6d1880),c(0x6d2bb0),0,0}});real_slot(1)=m.store(m.mul(m.q(rate),m.c(0x6d30c8)));rate=real({{c(0x6d19d8),c(0x6d1880),c(0x6d18f0),0x3ff0000000000000ull,0,0}});real_slot(2)=m.store(m.mul(m.q(rate),m.c(0x6d1880)));
  }break;
 case 6:
  if(timeline.frame==1){move(0x6d1aa8);integer(3)=pick({{7,6,5,4,0,0}});integer(4)=0;real_slot(0)=0x3ff0000000000000ull;real_slot(1)=real({{0x3ff0000000000000ull,c(0x6d1f50),c(0x6d1a00),c(0x6d2488),0,0}});real_slot(2)=0;}
  else if(timeline.frame==61)effect(10);
  else if(timeline.frame==81){effect(20);mark();enemy().word(8,78);enemy().quad(0x38,c(0x6d1a60));enemy().quad(0x40,c(0x6d1aa8));
   for(unsigned side=0;side<2;++side){auto slot=spawn(side?20:10,22,41,1,3);if(slot!=-1){auto b=bullet(static_cast<std::uint32_t>(slot));b.word(0x78,who(side?std::array<std::uint32_t,3>{{2,0,1}}:std::array<std::uint32_t,3>{{1,2,0}}));b.word(0x7c,side);b.quad(0xa0,c(side?0x6d2998:0x6d2748));b.quad(0xa8,c(0x6b25b8));b.quad(0xb0,b.quad(0xa0));b.quad(0x40,b.quad(0xa0));b.quad(0x48,b.quad(0xa8));b.word(8,signed_word(p.difficulty)>1?61u:59u);b.word(0x34,1);b.word(0xc,side?static_cast<std::uint32_t>(signed_word(integer(3))/2)+186u:94u);b.word(0x98,0);b.word(0x9c,0);}}
  }else if(timeline.frame==121){switch(p.difficulty){case 0:copy_name(context.spell_name,name_6c660c);break;case 1:copy_name(context.spell_name,name_6c6634);break;case 2:copy_name(context.spell_name,name_6c665c);break;case 3:copy_name(context.spell_name,name_6c6684);break;}services.configure_phase(1,50,extra.boss_hp_multiplier*16000u,51);}
  else if(timeline.frame==130){effect_at(10,c(0x6d2748),c(0x6b25b8),10);effect_at(10,c(0x6d2998),c(0x6b25b8),20);}
  if(signed_word(timeline.frame)>=170)stage4_boss_timeline_ease_real(real_slot(0),c(signed_word(p.difficulty)<=1?0x6d1920:0x6d1ad8),c(0x6d1aa8),bits);
  break;
 case 7:if(timeline.frame==180){timeline.subphase=8;timeline.frame=0;p.game_frame=0;}break;
 case 8:
  if(character==0){
   if(timeline.frame==1)move(0x6d1a88);
   else if(timeline.frame==81){switch(p.difficulty){case 0:copy_name(context.spell_name,name_6c66ac);break;case 1:copy_name(context.spell_name,name_6c66c0);break;case 2:copy_name(context.spell_name,name_6c66d4);break;case 3:copy_name(context.spell_name,name_6c66e8);break;}services.configure_phase(1,60,20000,55);enemy().word(8,0);enemy().quad(0x38,c(0x6d1a60));enemy().quad(0x40,c(0x6d1a88));effect(0);mark();}
   else if(timeline.frame==121){integer(3)=60;integer(4)=120;integer(5)=pick({{1,2,2,7,0,0}});integer(8)=pick({{1,1,2,7,0,0}});integer(6)=pick({{16,12,12,33,0,0}});integer(7)=pick({{17,17,21,41,0,0}});
    for(unsigned ring=0;ring<2;++ring){if(ring&&signed_word(p.difficulty)<2)break;const auto count_slot=ring?8u:5u;
     for(std::uint32_t i=0;signed_word(i)<=signed_word(integer(count_slot));++i){auto slot=spawn(0);if(slot!=-1){integer((ring?30u:20u)+i)=static_cast<std::uint32_t>(slot);auto b=bullet(static_cast<std::uint32_t>(slot));b.word(0x78,0);b.word(0x7c,0);b.quad(0xa0,m.store(m.add(m.mul(m.div(m.c(0x6d2208),m.i(integer(count_slot)+1u)),m.i(i)),m.c(0x6d1de0))));b.quad(0xa8,c(ring?0x6d1a60:0x6d1d78));b.quad(0xb0,c(ring?0x6d1b60:0x6d1de0));
      auto cosine=math.cosine(b.quad(0xa0));b.quad(0x40,m.store(m.add(m.mul(cosine,m.q(b.quad(0xa8))),m.q(player.quad(0x90)))));auto sine=math.sine(b.quad(0xa0));b.quad(0x48,m.store(m.add(m.mul(sine,m.q(b.quad(0xa8))),m.q(player.quad(0x98)))));b.word(8,25);b.word(0x34,1);b.word(0xc,quotient(integer(ring?7:6),integer(count_slot)+1u)*i);b.word(0x98,0);b.word(0x9c,0);
     }}
    }
   }
   if(signed_word(timeline.frame)>=121){for(unsigned ring=0;ring<2;++ring){if(ring&&signed_word(p.difficulty)<2)break;
    // 原外环更新也读取integer5，保留创建时使用integer8的差异。
    for(std::uint32_t i=0;signed_word(i)<=signed_word(integer(5));++i){auto b=bullet(integer((ring?30u:20u)+i));auto cosine=math.cosine(b.quad(0xa0));b.quad(0x40,m.store(m.add(m.mul(cosine,m.q(b.quad(0xa8))),m.q(player.quad(0x90)))));auto sine=math.sine(b.quad(0xa0));b.quad(0x48,m.store(m.add(m.mul(sine,m.q(b.quad(0xa8))),m.q(player.quad(0x98)))));sine=math.sine(b.quad(0xb0));auto turn=m.add(m.mul(sine,m.c(0x6d1fa0)),m.c(0x6d1810));b.quad(0xa0,m.store(ring?m.sub(m.q(b.quad(0xa0)),turn):m.add(turn,m.q(b.quad(0xa0)))));b.quad(0xb0,m.store(m.add(m.q(b.quad(0xb0)),m.c(ring?0x6d30c0:0x6d1ab8))));
     if(signed_word(timeline.frame)>=300&&remainder(b.word(0xc),integer(ring?7:6))==0){const bool alternate=b.word(0x7c)!=0;const auto rgb=ring?(alternate?std::array<std::uint32_t,3>{{0,128,255}}:std::array<std::uint32_t,3>{{255,128,0}}):(alternate?std::array<std::uint32_t,3>{{220,0,220}}:std::array<std::uint32_t,3>{{255,0,0}});const auto y=b.quad(0x48),x=b.quad(0x40);emit({16,2,rgb,0,255,1,41},x,y,1,0,c(alternate?0x6d2130:0x6d1b28),alternate?c(ring?0x6d1de0:0x6d1b60):0,0,53);b.word(0x7c,1u-b.word(0x7c));}
    }
   }}
  }else if(character==1){
   if(timeline.frame==1)move(0x6d2240);
   else if(timeline.frame==81){switch(p.difficulty){case 0:copy_name(context.spell_name,name_6c66fc);break;case 1:copy_name(context.spell_name,name_6c6720);break;case 2:copy_name(context.spell_name,name_6c6744);break;case 3:copy_name(context.spell_name,name_6c6760);break;}services.configure_phase(1,60,17000,59);enemy().word(8,0);enemy().quad(0x38,c(0x6d1a60));enemy().quad(0x40,c(0x6d2240));effect(0);mark();}
   else if(timeline.frame==121){auto slot=spawn(0);if(slot!=-1){integer(3)=static_cast<std::uint32_t>(slot);auto b=bullet(static_cast<std::uint32_t>(slot));b.word(0x78,1);b.word(0x7c,0);b.quad(0xa0,c(0x6d1a60));b.quad(0xa8,c(0x6d1d00));b.quad(0xb0,0);b.quad(0xb8,0);b.quad(0xc0,c(0x6d1960));b.quad(0x40,b.quad(0xa0));b.quad(0x48,b.quad(0xa8));b.word(8,25);b.word(0x34,1);b.word(0x3c,1);b.word(0x98,0);b.word(0x9c,0);}
    const std::array<std::array<std::uint32_t,3>,6> palette{{{{255,0,255}},{{255,0,0}},{{255,255,0}},{{0,255,0}},{{0,255,255}},{{0,0,255}}}};
    for(unsigned i=0;i<6;++i){slot=allocate_projectile(projectiles,{25,2,palette[i],0,255,1,41});if(slot!=-1){integer(5+i)=static_cast<std::uint32_t>(slot);auto b=bullet(static_cast<std::uint32_t>(slot));b.quad(0xa0,c(0x6d1a60));b.quad(0xa8,c(0x6d1d00));b.word(8,25);b.word(0x34,1);b.word(0x3c,1);}}
    integer(4)=pick({{52,40,30,22,0,0}});
   }
   if(signed_word(timeline.frame)>=121){auto center=bullet(integer(3));const bool easy=signed_word(p.difficulty)<=1;const auto mode=center.word(0x7c);const auto state=mode<3?mode:3u;
    const std::array<std::uint32_t,4> target_x=easy?std::array<std::uint32_t,4>{{0x6d22b0,0x6d1e48,0x6d29a8,0x6d22c8}}:std::array<std::uint32_t,4>{{0x6d23e0,0x6d1ae8,0x6d23e0,0x6d1ae8}};
    const std::array<std::uint32_t,4> target_y=easy?std::array<std::uint32_t,4>{{0x6d1aa8,0x6d1d00,0x6d1aa8,0x6d1d00}}:std::array<std::uint32_t,4>{{0x6d1ae8,0x6d1ae8,0x6d1a50,0x6d1a50}};
    stage4_boss_timeline_ease(center,0xa0,c(target_x[state]),c(easy?0x6d1ae8:0x6d1aa8),bits);stage4_boss_timeline_ease(center,0xa8,c(target_y[state]),c(easy?0x6d1aa8:state%2?0x6d1aa8:0x6d1a88),bits);
    auto sine=math.sine(center.quad(0xc0));auto turn=m.add(m.mul(sine,m.c(0x6d2370)),m.c(0x6d1b70));center.quad(0xb0,m.store(m.sub(m.q(center.quad(0xb0)),turn)));center.quad(0xc0,m.store(m.add(m.q(center.quad(0xc0)),m.c(0x6d2878))));center.quad(0xb8,m.store(m.add(m.q(center.quad(0xb8)),m.c(0x6d30b8))));stage4_boss_timeline_ease(center,0x40,center.quad(0xa0),c(0x6d1aa8),bits);stage4_boss_timeline_ease(center,0x48,center.quad(0xa8),c(easy?0x6d1ae8:0x6d1aa8),bits);
    sine=math.sine(center.quad(0xb8));const auto radius=m.store(m.mul(sine,m.c(0x6d1d78)));
    for(unsigned i=0;i<6;++i){auto b=bullet(integer(5+i));const auto angle=m.store(m.add(m.mul(m.i(i),m.c(0x6d2750)),m.q(center.quad(0xb0))));auto cosine=math.cosine(angle);b.quad(0x40,m.store(m.add(m.mul(cosine,m.q(radius)),m.q(center.quad(0x40)))));sine=math.sine(angle);b.quad(0x48,m.store(m.add(m.mul(sine,m.q(radius)),m.q(center.quad(0x48)))));}
    if(signed_word(timeline.frame)>=(easy?180:260)&&remainder(timeline.frame,integer(4))==0){const auto dx=m.store(m.sub(m.q(player.quad(0x90)),m.q(center.quad(0x40)))),dy=m.store(m.sub(m.q(player.quad(0x98)),m.q(center.quad(0x48))));const auto angle=m.store(math.atan2(dy,dx));const std::array<std::array<std::uint32_t,3>,6> palette{{{{255,0,255}},{{255,0,0}},{{255,255,0}},{{0,255,0}},{{0,255,255}},{{0,0,255}}}};
     for(unsigned i=0;i<6;++i){auto slot=allocate_projectile(projectiles,{25,2,palette[i],1,128,1,41});if(slot!=-1){auto b=bullet(static_cast<std::uint32_t>(slot));b.quad(0x40,bullet(integer(5+i)).quad(0x40));b.quad(0x48,bullet(integer(5+i)).quad(0x48));auto cosine=math.cosine(angle);b.quad(0x50,m.store(m.mul(cosine,m.c(0x6d1b28))));sine=math.sine(angle);b.quad(0x58,m.store(m.mul(sine,m.c(0x6d1b28))));b.word(8,63);}}
    }
    if(remainder(timeline.frame,200)==0){center.word(0x7c,center.word(0x7c)+1u);if(center.word(0x7c)==4)center.word(0x7c,0);}
   }
  }else{
   if(timeline.frame==1)move(0x6b25b8);else if(timeline.frame==60)mark();
   else if(timeline.frame==81){switch(p.difficulty){case 0:copy_name(context.spell_name,name_6c677c);break;case 1:copy_name(context.spell_name,name_6c67a0);break;case 2:copy_name(context.spell_name,name_6c67c4);break;case 3:copy_name(context.spell_name,name_6c67dc);break;}services.configure_phase(1,60,22000,63);enemy().word(8,0);enemy().quad(0x38,c(0x6d1a60));enemy().quad(0x40,c(0x6b25b8));integer(3)=40;integer(4)=integer(3)+150u;integer(5)=pick({{16,24,15,17,0,0}});integer(6)=0u-pick({{120,120,200,200,0,0}});real_slot(0)=real({{c(0x6d1ad0),c(0x6d1ff8),c(0x6d1ff8),c(0x6d1ff8),0,0}});}
   if(signed_word(p.difficulty)<=1){if(timeline.frame==100){auto slot=spawn(0,22,41,0);auto b=bullet(static_cast<std::uint32_t>(slot));if(slot!=-1){b.word(0x78,2);b.quad(0xa0,c(0x6d1af0));b.quad(0xb0,c(0x6d25a8));b.quad(0x40,c(0x6d1a60));b.quad(0x48,c(0x6d1ae0));b.word(8,64);b.word(0x34,1);b.word(0x98,0);b.word(0x9c,0);}const auto y=b.quad(0xa0),x=b.quad(0x40);effect_at(10,x,y,0);}}
   else if(timeline.frame==100){
    if(p.difficulty==3){auto slot=spawn(0,22,41,0);auto b=bullet(static_cast<std::uint32_t>(slot));if(slot!=-1){b.word(0x78,2);b.quad(0xa0,c(0x6d1af0));b.quad(0xb0,c(0x6d30b0));b.quad(0x40,c(0x6d1a60));b.quad(0x48,c(0x6d1ae0));b.word(8,64);b.word(0x34,1);b.word(0x98,0);b.word(0x9c,0);}const auto y=b.quad(0xa0),x=b.quad(0x40);effect_at(10,x,y,0);}
    for(unsigned side=0;side<2;++side){auto slot=spawn(0,22,41,0);auto b=bullet(static_cast<std::uint32_t>(slot));if(slot!=-1){b.word(0x78,2);b.quad(0xa0,m.store(m.i(p.difficulty==2?200u:90u)));b.quad(0xb0,c(0x6d25a8));b.quad(0x40,m.store(m.i(side?(p.difficulty==2?302u:332u):(p.difficulty==2?82u:52u))));b.quad(0x48,c(0x6d1ae0));b.word(8,64);b.word(0x34,1);b.word(0x98,0);b.word(0x9c,0);}const auto y=b.quad(0xa0),x=b.quad(0x40);effect_at(10,x,y,0);}
   }
  }
  break;
 case 9:if(timeline.frame==32)services.retire_boss(0);else if(timeline.frame==196)services.dialog(8);break;
 case 10:if(timeline.frame==1)services.award_stage_clear();else if(timeline.frame==200){++death.stage;timeline.part=0;timeline.subphase=0;timeline.frame=0;p.game_frame=0;services.change_stage_resources(death.stage);}break;
 default:break;
 }
}
}
