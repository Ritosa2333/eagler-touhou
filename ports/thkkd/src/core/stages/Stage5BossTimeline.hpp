// 原66F640..674F14十四阶段；保留入口Boss槽、区域接触与原副弹落点。
#pragma once
#include "core/stages/Stage1Timeline.hpp"
#include "core/stages/StageBossServices.hpp"
#include "core/stages/StagePatternContext.hpp"
#include "core/stages/StageResourceContext.hpp"
#include "core/entities/BossPhaseContext.hpp"
#include "core/entities/BossAllocationContext.hpp"
#include "core/entities/BossPresentationContext.hpp"
#include "core/entities/EnemyDamage.hpp"
#include "core/entities/EnemyDeathContext.hpp"
#include "core/shared/EffectPool.hpp"
#include "core/math/GameMath.hpp"
#include "core/stages/Stage3BossTimelineContext.hpp"
#include "core/stages/Stage5BossTimelineArithmetic.hpp"
#include "core/stages/Stage5BossTimelineSecondaryContact.hpp"
namespace kaikeidou {
inline void stage5_boss_segment(StageTimeline& timeline,EnemyPool& enemies,ProjectilePools& projectiles,EffectPool& effects,
 const ObjectRecord<240>& player,PatternParameters& p,BossPhaseContext& phase,BossAllocationContext& allocation,
 BossPresentationContext& presentation,EnemyDamageContext& protection,BossStageContext& context,StagePatternContext& extra,
 Stage3BossTimelineContext& extended,EnemyDeathContext& death,StageResourceContext& background,Random& random,
 RandomPrecision precision,GameMath& math,StageBossServices& services,StageResourceServices& platform){
 (void)platform;using namespace stage5_boss_timeline_detail;using Number=binary_float::Number;const auto bits=static_cast<unsigned>(precision);const Arithmetic m{bits};auto boss=phase.boss_index;const auto route=timeline.subphase;
 const auto enemy=[&]()->Enemy&{return boss==0xffffffffu?context.enemy_prefix:enemies.enemies.at(boss);};
 const auto c=[](std::uint32_t a){return constant(a);};const auto q=[&](std::size_t off){return m.q(enemy().quad(off));};const auto put=[&](std::size_t off,Number n){enemy().quad(off,m.store(n));};
 const auto real_slot=[&](unsigned i)->std::uint64_t&{if(i<2)return p.reals[i];if(i<4)return context.extra_reals[i-2];if(i==4)return extra.real_4;if(i<10)return extended.reals_5_to_9[i-5];if(i<20)return extra.reals_10_to_19[i-10];return extended.reals_20_to_29.at(i-20);};
 const auto pick=[&](std::array<std::uint32_t,6> v){return stage5_boss_timeline_pick_integer(p.difficulty,v);};const auto real=[&](std::array<std::uint64_t,6> v){return stage5_boss_timeline_pick_real(p.difficulty,v);};
 auto& bg=background.reals;const auto frame=[&](){return signed_word(timeline.frame);};const auto rnd=[&](std::uint32_t a){return random.real_number(c(a),precision);};
 const auto effect=[&](std::array<std::uint32_t,3> color){const auto y=enemy().quad(0x40),x=enemy().quad(0x38);const auto value=services.color(color[0],color[1],color[2]);allocate_effect(effects,projectiles.sound_triggers,{12,value,x,y});};
 const auto mark=[&](){enemy().word(0x2c,3);enemy().word(0x30,0);enemy().word(0x34,0);};
 const auto copy_background=[&](){bg[5]=enemy().quad(0x38);bg[6]=enemy().quad(0x40);};
 const auto move=[&](std::uint32_t x,std::uint32_t y,std::uint32_t divisor){enemy().word(8,90);enemy().quad(0xa8,c(x));enemy().quad(0xb0,c(y));enemy().quad(0xb8,c(divisor));};
 const auto aim=[&](){const auto dx=m.store(m.sub(m.q(player.quad(0x90)),q(0x38))),dy=m.store(m.sub(m.q(player.quad(0x98)),q(0x40)));return math.atan2(dy,dx);};
 const auto bullet=[&](std::uint32_t index){return BulletView{projectiles,index*240u};};
 const auto spawn=[&](std::uint32_t kind,std::int32_t group,std::array<std::uint32_t,3> color,std::uint32_t delayed){return allocate_projectile(projectiles,{kind,group,color,0,255,delayed,41});};
 const auto pattern73=[&](std::array<std::uint32_t,3> color){
  const std::array<std::array<std::uint32_t,2>,6> constants{{{{0x6d25d8,0x6d1880}},{{0x6d25d8,0x6d1ee8}},{{0x6d25d0,0x6d24e0}},{{0x6d25d0,0x6d25c8}},{{0x6d25c0,0x6d1900}},{{0x6d25c0,0x6d25b8}}}};
  const auto initialize=[&](BulletView b){auto angle=m.store(m.add(m.q(real_slot(0)),m.q(b.quad(0xa8))));auto cosine=math.cosine(angle);b.quad(0x40,m.store(m.add(m.mul(cosine,m.q(b.quad(0xa0))),q(0x38))));angle=m.store(m.add(m.q(real_slot(0)),m.q(b.quad(0xa8))));auto sine=math.sine(angle);b.quad(0x48,m.store(m.add(m.mul(sine,m.q(b.quad(0xa0))),q(0x40))));b.word(8,73);b.word(0x34,1);};
  for(auto pair:constants){auto index=spawn(11,1,color,0);if(index!=-1){auto b=bullet(static_cast<std::uint32_t>(index));b.quad(0xa0,c(pair[0]));b.quad(0xa8,c(pair[1]));initialize(b);}}
  for(std::int32_t i=-2;i<=3;++i){auto index=spawn(11,1,color,0);if(index!=-1){auto b=bullet(static_cast<std::uint32_t>(index));b.quad(0xa0,m.store(m.i(static_cast<std::uint32_t>(i)*26u)));b.quad(0xa8,0);initialize(b);}}
 };
 const auto pattern74=[&](std::array<std::uint32_t,3> color){
  const auto angle=m.store(m.mul(rnd(0x6d1c60),m.c(0x6d1950)));
  for(std::uint32_t i=0;signed_word(i)<signed_word(enemy().word(0x84));++i){auto index=spawn(0,3,{{(color[0]+1)/2,(color[1]+1)/2,(color[2]+1)/2}},1);if(index!=-1){auto b=bullet(static_cast<std::uint32_t>(index));auto a=m.store(m.add(m.mul(m.div(m.c(0x6d2208),m.i(enemy().word(0x84))),m.i(i)),m.q(angle)));auto cosine=math.cosine(a);b.quad(0x40,m.store(m.add(m.mul(cosine,m.c(0x6d1c58)),q(0x38))));a=m.store(m.add(m.mul(m.div(m.c(0x6d2208),m.i(enemy().word(0x84))),m.i(i)),m.q(angle)));auto sine=math.sine(a);b.quad(0x48,m.store(m.add(m.mul(sine,m.c(0x6d1c58)),q(0x40))));b.word(8,74);}}
  if(p.difficulty!=0){auto index=spawn(1,2,color,1);if(index!=-1){auto b=bullet(static_cast<std::uint32_t>(index));b.quad(0x40,enemy().quad(0x38));b.quad(0x48,enemy().quad(0x40));b.word(8,74);}}
 };
 switch(route){
 case 0:{auto sine=math.sine(bg[1]);bg[0]=m.store(m.add(m.add(m.mul(sine,m.c(0x6d1cf8)),m.c(0x6d19e0)),m.q(bg[0])));sine=math.sine(bg[3]);bg[2]=m.store(m.add(m.add(m.mul(sine,m.c(0x6d1cf8)),m.c(0x6d1aa0)),m.q(bg[2])));sine=math.sine(bg[2]);bg[4]=m.store(m.mul(sine,m.c(0x6d1aa8)));auto cosine=math.cosine(bg[0]);bg[5]=m.store(m.add(m.mul(cosine,m.q(bg[4])),m.c(0x6d1a60)));sine=math.sine(bg[0]);bg[6]=m.store(m.add(m.mul(sine,m.q(bg[4])),m.c(0x6d1af0)));
  if(timeline.frame==1){boss=static_cast<std::uint32_t>(services.allocate_boss(5));enemy().word(8,85);enemy().quad(0x38,bg[5]);enemy().quad(0x40,bg[6]);enemy().word(0x80,0);allocation.field_a7ea5c=0;}else if(timeline.frame==200)services.dialog(10);if(signed_word(allocation.field_a7ea5c)<=254)++allocation.field_a7ea5c;
  break;}
 case 1:case 4:case 7:{copy_background();const auto color=route==1?std::array<std::uint32_t,3>{{255,0,0}}:route==4?std::array<std::uint32_t,3>{{0,255,0}}:std::array<std::uint32_t,3>{{0,0,255}};const auto start=route==1?120u:140u;
  if(timeline.frame==1){if(route==1){move(0x6d1a60,0x6d1a90,0x6d1ac8);services.begin_boss_effect();}else{enemy().word(8,90);auto cosine=math.cosine(c(route==4?0x6d25a0:0x6d1de0));put(0xa8,m.add(m.mul(cosine,m.c(0x6d18c0)),m.c(0x6d1a60)));auto sine=math.sine(c(route==4?0x6d25a0:0x6d1de0));put(0xb0,m.add(m.mul(sine,m.c(0x6d18c0)),m.c(route==4?0x6d1858:0x6d1af0)));enemy().quad(0xb8,c(0x6d1aa8));}}
  else if(timeline.frame==(route==1?81u:101u)){services.configure_phase(0,route==7?50u:40u,extra.boss_hp_multiplier*(route==1?13000u:route==4?8000u:6500u),0xfffffff6u);effect(color);if(route==1)allocation.field_a7ea5c=255;mark();}
  else if(timeline.frame==start){if(route==4){enemy().word(8,91);enemy().quad(0xa8,0);}else if(route==7){enemy().word(8,92);enemy().word(12,1);enemy().quad(0xa8,c(0x6d1de0));enemy().quad(0xb0,c(0x6d1de0));}real_slot(2)=c(route==4?0x6d1860:0x6d25e0);auto angle=aim();real_slot(0)=m.store(m.add(angle,m.q(real_slot(2))));real_slot(1)=c(route==4?0x6d1ad0:0x6d1e40);enemy().word(0x80,route==1?pick({{48,40,32,32,0,0}}):route==4?pick({{32,28,24,24,0,0}}):pick({{70,50,40,30,0,0}}));enemy().word(0x84,route==1?pick({{6,7,9,11,0,0}}):route==4?pick({{6,8,10,12,0,0}}):pick({{6,8,9,10,0,0}}));pattern73(color);}
  if(frame()>=signed_word(start)){auto angle=aim();real_slot(0)=m.store(m.add(angle,m.q(real_slot(2))));stage5_boss_timeline_ease_real(real_slot(2),0,c(0x6d1ae8),bits);if(remainder(timeline.frame,enemy().word(0x80))==0)pattern74(color);
   if(route==1&&remainder(timeline.frame,300)==0){auto x=rnd(0x6d1f60);put(0xa8,m.add(x,m.c(0x6d25b0)));auto y=rnd(0x6d1aa8);put(0xb0,m.add(y,m.c(0x6d1aa8)));enemy().quad(0xb8,c(0x6d1ac8));}}
  break;}

 case 2:case 5:case 8:case 10:{
  copy_background();(void)random.real_number(0,precision);auto& circle=projectiles.secondary[0];
  if(route==2){
   if(timeline.frame==1){move(0x6d1a60,0x6d1b38,0x6d1ac8);protection.boss_state_a7ea60=2;}
   else if(timeline.frame==60){circle.word(0,1);projectiles.sound_triggers[18]=1;circle.quad(0x10,c(0x6d1a60));circle.quad(0x18,c(0x6d20f0));circle.quad(0x20,c(0x6d1e38));circle.quad(0x28,m.store(m.i(pick({{128,128,96,96,0,0}}))));circle.quad(0x30,c(0x6d19c0));circle.quad(0x38,0x3ff0000000000000ull);circle.quad(0x40,0x3ff0000000000000ull);circle.word(4,75);circle.word(12,0);circle.word(0x48,0);mark();}
  }else if(route==5){
   if(timeline.frame==1){enemy().word(8,90);auto cosine=math.cosine(c(0x6d1de0));const auto x=m.store(m.add(m.mul(cosine,m.c(0x6d2598)),m.c(0x6d1a60)));cosine=math.cosine(c(0x6d2590));const auto x2=m.store(m.sub(m.q(x),m.mul(cosine,m.c(0x6d1840))));enemy().quad(0xa8,real({{c(0x6d1a60),c(0x6d1a60),x2,c(0x6d1a60),0,0}}));auto sine=math.sine(c(0x6d1de0));const auto y=m.store(m.add(m.mul(sine,m.c(0x6d2598)),m.c(0x6d1de8)));sine=math.sine(c(0x6d2590));const auto y2=m.store(m.sub(m.q(y),m.mul(sine,m.c(0x6d1840))));enemy().quad(0xb0,real({{c(0x6b25b8),c(0x6d2588),y2,c(0x6b25b8),0,0}}));enemy().quad(0xb8,c(0x6d1ac8));protection.boss_state_a7ea60=2;}
   else if(timeline.frame==60){
    if(p.difficulty==0){circle.word(0,1);projectiles.sound_triggers[18]=1;circle.quad(0x10,c(0x6d1a60));circle.quad(0x18,c(0x6d20f0));circle.quad(0x20,c(0x6d1e38));circle.quad(0x28,c(0x6d1d78));circle.quad(0x30,c(0x6d18b0));circle.quad(0x38,0x3ff0000000000000ull);circle.quad(0x40,c(0x6d18b0));circle.word(4,75);circle.word(12,0);circle.word(0x48,0);}
    else if(p.difficulty==1){circle.word(0,1);projectiles.sound_triggers[18]=1;circle.quad(0x10,c(0x6d1a60));circle.quad(0x18,c(0x6d1af0));circle.quad(0x20,c(0x6d1e38));circle.quad(0x28,c(0x6d2580));circle.quad(0x30,0x3ff0000000000000ull);circle.quad(0x38,0x3ff0000000000000ull);circle.quad(0x40,c(0x6d1920));circle.word(4,80);circle.word(12,0);circle.word(0x48,0);}
    else if(p.difficulty==2){circle.word(0,1);projectiles.sound_triggers[18]=1;circle.quad(0x10,enemy().quad(0xa8));circle.quad(0x18,m.store(m.add(q(0xb0),m.c(0x6d1be0))));circle.quad(0x20,c(0x6d1e38));circle.quad(0x28,c(0x6d1878));circle.quad(0x30,0x3ff0000000000000ull);circle.quad(0x38,c(0x6d18b0));circle.quad(0x40,c(0x6d18b0));circle.word(4,82);circle.word(12,0);circle.word(0x48,0);}
    else{circle.word(0,1);projectiles.sound_triggers[18]=1;circle.quad(0x10,c(0x6d1a60));circle.quad(0x18,c(0x6d20f0));circle.quad(0x20,c(0x6d1e38));circle.quad(0x28,c(0x6d1d18));circle.quad(0x30,c(0x6d18b0));circle.quad(0x38,c(0x6d18b0));circle.quad(0x40,0x3ff0000000000000ull);circle.word(4,75);circle.word(12,0);circle.word(0x48,0);}mark();
   }
  }else if(route==8){
   if(timeline.frame==1){move(0x6d1a60,0x6d1a50,0x6d1ac8);protection.boss_state_a7ea60=2;}
   else if(timeline.frame==60){for(std::uint32_t row=0;row<=3;++row){const auto limit=2u+(row%2!=0);for(std::uint32_t col=0;col<=limit;++col){auto& item=projectiles.secondary[row*4+col];item.word(0,1);item.quad(0x10,m.store(m.i(col*96u+(row%2?48u:96u))));item.quad(0x18,m.store(m.i(row*82u+64u)));item.quad(0x20,c(0x6d1e38));item.quad(0x28,m.store(m.i(signed_word(p.difficulty)<=1?60u:52u)));item.word(4,83);item.word(12,0);item.word(0x48,0);item.quad(0x70,m.store(m.i(row*82u+64u)));item.quad(0x30,row%2?real({{c(0x6d19d8),0x3ff0000000000000ull,0x3ff0000000000000ull,c(0x6d1880),0,0}}):0x3ff0000000000000ull);item.quad(0x38,row%2?real({{0x3ff0000000000000ull,c(0x6d19d8),c(0x6d19d8),c(0x6d18b0),0,0}}):0x3ff0000000000000ull);item.quad(0x40,row%2?real({{c(0x6d19c0),c(0x6d19c0),c(0x6d19c0),0x3ff0000000000000ull,0,0}}):c(0x6d19c0));}}projectiles.sound_triggers[18]=1;mark();}
  }else{
   if(timeline.frame==0)protection.boss_state_a7ea60=2;
   else if(timeline.frame==60)move(0x6d1a60,0x6d1af0,0x6d1f60);
   else if(timeline.frame==180){for(std::uint32_t i=0;i<=7;++i){auto& item=projectiles.secondary[i];item.word(0,1);item.quad(0x70,m.store(m.add(m.mul(m.i(i),m.c(0x6d22d8)),m.c(0x6d1de0))));auto cosine=math.cosine(item.quad(0x70));item.quad(0x10,m.store(m.add(m.mul(cosine,m.c(0x6d1cc0)),m.c(0x6d1a60))));auto sine=math.sine(item.quad(0x70));item.quad(0x18,m.store(m.add(m.mul(sine,m.c(0x6d1cc0)),m.c(0x6d1af0))));item.quad(0x20,c(0x6d1e38));item.quad(0x28,c(0x6d1d18));item.word(4,80);item.word(12,0);item.word(0x48,0);item.quad(0x30,0x3ff0000000000000ull);item.quad(0x38,0x3ff0000000000000ull);item.quad(0x40,0x3ff0000000000000ull);}projectiles.sound_triggers[18]=1;mark();}
  }
  const auto difference=m.sub(m.add(m.q(circle.quad(0x28)),m.c(0x6d1868)),m.q(circle.quad(0x20)));
  if((route!=10||frame()>=180)&&!difference.negative&&difference.mantissa){
   if(stage5_boss_secondary_contact(projectiles,player,bits)){
    ++timeline.subphase;timeline.frame=0;
    if(route==2){bg[10]=0x3ff0000000000000ull;bg[11]=c(0x6d18e0);bg[12]=c(0x6d1900);}
    else if(route==5){if(p.difficulty==0){bg[10]=c(0x6d1900);bg[11]=c(0x6d1880);bg[12]=c(0x6d1900);}else if(p.difficulty==1){bg[10]=c(0x6d1920);bg[11]=c(0x6d1920);bg[12]=0;}else if(p.difficulty==2){bg[10]=0x3ff0000000000000ull;bg[11]=c(0x6d19c0);bg[12]=c(0x6d19c0);}else{bg[10]=c(0x6d18e0);bg[11]=c(0x6d18e0);bg[12]=c(0x6d1880);}}
    else if(route==8){if(p.difficulty==0){bg[10]=c(0x6d19c0);bg[11]=c(0x6d19d8);bg[12]=0;}else if(p.difficulty==1||p.difficulty==2){bg[10]=c(0x6d19d8);bg[11]=c(0x6d19c0);bg[12]=0;}else{bg[10]=c(0x6d18e0);bg[11]=c(0x6d18e0);bg[12]=c(0x6d18e0);}}
    else{bg[10]=0;bg[11]=0;bg[12]=0x3ff0000000000000ull;}
    const auto count=route==8?16u:route==10?8u:1u;for(unsigned i=0;i<count;++i){projectiles.secondary[i].word(0x48,1);projectiles.secondary[i].quad(0x20,circle.quad(0x28));}
   }else if(timeline.frame==(route==10?500u:400u))services.dialog(route==2?20u:route==5?21u:route==8?22u:23u);
  }
  break;}
 case 3:{
  if(timeline.frame==1){switch(p.difficulty){case 0:copy_name(context.spell_name,name_6c633c);break;case 1:copy_name(context.spell_name,name_6c634c);break;case 2:copy_name(context.spell_name,name_6c635c);break;case 3:copy_name(context.spell_name,name_6c6374);break;}const auto hp=pick({{11000,11000,9000,7500,0,0}})*extra.boss_hp_multiplier;services.configure_phase(1,50,hp,71);}
  else if(timeline.frame==61){effect({{0,255,255}});mark();}
  else if(timeline.frame==101)move(0x6b25f8,0x6b25f8,0x6d1898);
  else if(timeline.frame==151){enemy().word(8,93);enemy().word(12,0);enemy().word(0x80,0);enemy().quad(0xb8,c(0x6d1ae8));enemy().quad(0xc0,c(0x6d1ae8));real_slot(0)=real({{c(0x6d25a8),c(0x6d25a8),c(0x6d1948),c(0x6d1e40),0,0}});real_slot(1)=c(0x6d1b10);real_slot(2)=m.store(m.mul(rnd(0x6d1c60),m.c(0x6d1950)));real_slot(3)=m.store(m.mul(rnd(0x6d1c60),m.c(0x6d1950)));}break;}
 case 6:{
  if(timeline.frame==1){switch(p.difficulty){case 0:copy_name(context.spell_name,name_6c6390);break;case 1:copy_name(context.spell_name,name_6c63ac);break;case 2:copy_name(context.spell_name,name_6c63c8);break;case 3:copy_name(context.spell_name,name_6c63e4);break;}const auto hp=pick({{10000,7000,24000,12000,0,0}})*extra.boss_hp_multiplier;const auto seconds=pick({{50,50,50,53,0,0}});services.configure_phase(1,seconds,hp,75);}
  if(p.difficulty==0){if(timeline.frame==61){effect({{0,255,0}});mark();}else if(timeline.frame==101){enemy().word(8,94);enemy().quad(0xa8,0);enemy().quad(0xb8,0);enemy().quad(0xb0,0);}}
  else if(p.difficulty==1){if(timeline.frame==61){effect({{255,255,0}});mark();}else if(timeline.frame==101){enemy().word(8,95);enemy().quad(0xa8,c(0x6d1de0));auto sine=math.sine(enemy().quad(0xa8));auto product=m.mul(sine,m.c(0x6d1860));const auto angle=m.store(m.add(product,product));sine=math.sine(angle);put(0x38,m.add(m.mul(sine,m.c(0x6d1858)),m.c(0x6d1a60)));auto cosine=math.cosine(angle);put(0x40,m.sub(m.c(0x6d1af0),m.mul(cosine,m.c(0x6d1858))));}}
  else if(p.difficulty==2){if(timeline.frame==61){effect({{255,128,128}});mark();}else if(timeline.frame==101){enemy().word(8,96);enemy().quad(0xa8,c(0x6d1de0));enemy().quad(0xb0,c(0x6d1de0));enemy().quad(0xb8,0);real_slot(0)=c(0x6d1e48);real_slot(1)=c(0x6d1828);real_slot(2)=c(0x6d1840);real_slot(3)=c(0x6d1de0);const auto index=allocate_projectile(projectiles,{2,0,{{255,255,0}},2,255,1,41});if(index!=-1){auto b=bullet(static_cast<std::uint32_t>(index));b.word(8,25);b.quad(0x40,c(0x6d1a60));b.quad(0x48,c(0x6d1de8));b.word(0x34,1);}}}
  else{if(timeline.frame==61){effect({{0,0,255}});mark();}else if(timeline.frame==101){enemy().word(8,97);enemy().word(0x80,0);enemy().quad(0xa8,0);enemy().quad(0xb0,0);}}break;}
 case 9:{if(timeline.frame==1){switch(p.difficulty){case 0:copy_name(context.spell_name,name_6c6400);break;case 1:copy_name(context.spell_name,name_6c641c);break;case 2:copy_name(context.spell_name,name_6c6438);break;case 3:copy_name(context.spell_name,name_6c6458);break;}services.configure_phase(1,49,extra.boss_hp_multiplier*8000u,79);}else if(timeline.frame==61){effect({{255,255,255}});mark();}else if(timeline.frame==101){enemy().word(8,98);enemy().word(0x80,0);enemy().word(0x84,0);}break;}

 case 11:{
  const auto gt=[&](Number a,Number b){const auto d=m.sub(a,b);return !d.negative&&d.mantissa!=0;};
  const auto lt=[&](Number a,Number b){return gt(b,a);};
  auto& central=projectiles.secondary[10];
  const auto central_init=[&](bool reversed_flags){central.word(0,1);central.quad(0x10,enemy().quad(0x38));central.quad(0x18,enemy().quad(0x40));central.quad(0x20,0);central.quad(0x28,c(0x6d1878));central.word(4,80);if(reversed_flags){central.word(0x48,0);central.word(12,0);}else{central.word(12,0);central.word(0x48,0);}central.quad(0x30,0x3ff0000000000000ull);central.quad(0x38,0x3ff0000000000000ull);central.quad(0x40,0x3ff0000000000000ull);};
  const auto center_radius=[&](){const auto dx=m.store(m.sub(m.c(0x6d1a60),m.q(player.quad(0x90)))),dy=m.store(m.sub(m.c(0x6d1af0),m.q(player.quad(0x98))));const auto sum=m.store(m.add(m.mul(m.q(dx),m.q(dx)),m.mul(m.q(dy),m.q(dy))));const auto root=finite_square_root(sum,bits);central.quad(0x28,m.store(m.mul(m.div(root,m.c(0x6d1cc0)),m.c(0x6d1878))));};
  const auto rain=[&](){if(frame()>=181&&remainder(timeline.frame,12)==0){const auto index=allocate_projectile(projectiles,{21,0,{{255,255,255}},1,128,0,40});if(index!=-1){auto b=bullet(static_cast<std::uint32_t>(index));const auto x=rnd(0x6d2498);b.quad(0x40,m.store(m.add(m.div(x,m.c(0x6d1898)),m.c(0x6d1b58))));b.quad(0x48,c(0x6d1a48));const auto angle=m.store(m.mul(rnd(0x6d1c60),m.c(0x6d1950)));const auto cosine=math.cosine(angle);b.quad(0x50,m.store(m.add(cosine,cosine)));const auto speed=rnd(0x6d1af8);b.quad(0x58,m.store(m.sub(m.c(0x6d22e0),m.div(speed,m.c(0x6d1ab0)))));b.word(8,86);b.word(0x3c,1);}}};
  const auto limit_angle=[&](bool next){const auto value=real(next?std::array<std::uint64_t,6>{{c(0x6d1948),c(0x6d1c18),c(0x6d18e8),c(0x6d18e8),0,0}}:std::array<std::uint64_t,6>{{0x3ff0000000000000ull,c(0x6d1948),c(0x6d1c18),c(0x6d1c18),0,0}});real_slot(2)=m.store(m.mul(m.q(value),m.c(0x6d1860)));};
  const auto slow=[&](){real_slot(1)=m.store(m.mul(m.q(real_slot(1)),m.q(real_slot(10))));real_slot(10)=m.store(m.sub(m.q(real_slot(10)),m.c(0x6d22b8)));};
  if(timeline.frame==1){switch(p.difficulty){case 0:copy_name(context.spell_name,name_6c6474);break;case 1:copy_name(context.spell_name,name_6c6488);break;case 2:copy_name(context.spell_name,name_6c64a4);break;case 3:copy_name(context.spell_name,name_6c64c0);break;}const auto hp=pick({{20000,0,0,0,0,0}});const auto seconds=pick({{60,44,57,60,0,0}});services.configure_phase(1,seconds,hp,83);enemy().word(8,0);}
  if(p.difficulty==0){
   if(timeline.frame==61){enemy().quad(0x38,c(0x6d1a60));enemy().quad(0x40,c(0x6d1af0));central.word(0,1);projectiles.sound_triggers[18]=1;central.quad(0x10,enemy().quad(0x38));central.quad(0x18,enemy().quad(0x40));central.quad(0x20,0);central.quad(0x28,c(0x6d1878));central.word(4,80);central.word(0x48,0);central.word(12,0);central.quad(0x30,0x3ff0000000000000ull);central.quad(0x38,0x3ff0000000000000ull);central.quad(0x40,0x3ff0000000000000ull);p.integers[0]=0;real_slot(0)=0;real_slot(1)=0;real_slot(2)=0;real_slot(3)=0;enemy().word(0x2c,4);enemy().word(0x30,0);enemy().word(0x34,0);}
   else if(frame()>=181){rain();const auto state=p.integers[0];if(state<=2){real_slot(0)=m.store(m.add(m.q(real_slot(0)),m.q(real_slot(1))));
     if(state==0){if(gt(m.c(0x6d2578),m.q(real_slot(1))))real_slot(1)=m.store(m.add(m.q(real_slot(1)),m.c(0x6a7888)));}
     else if(state==1){if(lt(m.c(0x6d2568),m.q(real_slot(1))))real_slot(1)=m.store(m.sub(m.q(real_slot(1)),m.c(0x6b2618)));}
     else if(gt(m.c(0x6d2560),m.q(real_slot(1))))real_slot(1)=m.store(m.add(m.q(real_slot(1)),m.c(0x6d2558)));
     real_slot(2)=m.store(m.add(m.q(real_slot(2)),m.q(real_slot(3))));
     if(gt(m.c(state==0?0x6d19e0:state==1?0x6b2620:0x6d1aa0),m.q(real_slot(3))))real_slot(3)=m.store(m.add(m.q(real_slot(3)),m.c(state==0?0x6d2570:state==1?0x6d21a8:0x6d2550)));
     const auto cosine=math.cosine(real_slot(2));const auto radius=m.store(m.mul(cosine,m.c(0x6d1cc0)));
     for(unsigned i=0;i<8;++i){auto& item=projectiles.secondary[i];auto a=m.store(m.add(m.q(item.quad(0x70)),m.q(real_slot(0))));auto co=math.cosine(a);item.quad(0x10,m.store(m.add(m.mul(co,m.q(radius)),m.c(0x6d1a60))));a=m.store(m.add(m.q(item.quad(0x70)),m.q(real_slot(0))));auto si=math.sine(a);item.quad(0x18,m.store(m.add(m.mul(si,m.q(radius)),m.c(0x6d1af0))));auto absolute=stage5_boss_timeline_absolute(radius);item.quad(0x20,m.store(m.add(m.mul(m.sub(m.c(0x6d1cc0),m.q(absolute)),m.c(0x6d1b70)),m.c(0x6d1d18))));}
     center_radius();if(state<2&&signed_word(enemy().word(0x14))<=(state==0?14000:7000)){p.integers[0]=state+1;real_slot(3)=0;bg[10]=state==0?0:0x3ff0000000000000ull;bg[11]=state==0?0x3ff0000000000000ull:0;bg[12]=0;projectiles.sound_triggers[18]=1;}
    }}
  }else{
   rain();
   if(timeline.frame==61){enemy().quad(0x38,c(0x6d1a60));enemy().quad(0x40,c(0x6d1af0));enemy().quad(0xa8,0);enemy().quad(0xb0,c(0x6b25f8));central.word(0,1);projectiles.sound_triggers[18]=1;central.quad(0x10,enemy().quad(0x38));central.quad(0x18,enemy().quad(0x40));central.quad(0x20,0);central.quad(0x28,c(0x6d1878));central.word(4,80);central.word(12,0);central.word(0x48,0);central.quad(0x30,0x3ff0000000000000ull);central.quad(0x38,0x3ff0000000000000ull);central.quad(0x40,0x3ff0000000000000ull);}
   else if(frame()>=101&&frame()<=360){const auto sine=math.sine(enemy().quad(0xa8));put(0x38,m.add(m.mul(sine,q(0xb0)),m.c(0x6d1a60)));put(0x40,m.sub(q(0x40),m.c(0x6d2548)));put(0xa8,m.add(q(0xa8),m.c(0x6d1918)));put(0xb0,m.add(q(0xb0),m.c(0x6d1918)));central.quad(0x10,enemy().quad(0x38));central.quad(0x18,enemy().quad(0x40));if(timeline.frame==360){enemy().quad(0x38,c(0x6d2540));central.word(0,0);p.integers[0]=0;real_slot(0)=0;real_slot(1)=0;limit_angle(false);real_slot(3)=real({{c(0x6d1928),c(0x6d2528),c(0x6d2530),c(0x6d2538),0,0}});real_slot(10)=c(0x6d1bd0);}}
   else if(frame()>=361){const auto state=p.integers[0];
    if(state==0||state==1){real_slot(0)=m.store(m.add(m.q(real_slot(0)),m.q(real_slot(1))));if(gt(m.q(real_slot(3)),m.q(real_slot(1))))real_slot(1)=m.store(m.add(m.q(real_slot(1)),m.c(0x6b2618)));
     if(!gt(m.q(real_slot(2)),m.q(real_slot(0)))){p.integers[0]=state+1;real_slot(0)=0;real_slot(1)=0;limit_angle(state==1);real_slot(3)=real(state==0?std::array<std::uint64_t,6>{{c(0x6d1928),c(0x6d2528),c(0x6d2530),c(0x6d2520),0,0}}:std::array<std::uint64_t,6>{{c(0x6d1928),c(0x6d2508),c(0x6d2510),c(0x6d2518),0,0}});
      if(state==1){real_slot(4)=c(0x6d1cc0);real_slot(5)=c(0x6d1d18);real_slot(6)=real({{c(0x6d2500),c(0x6d2500),c(0x6d1b48),c(0x6d2128),0,0}});real_slot(7)=real({{c(0x6d1990),c(0x6d1990),c(0x6d1a18),c(0x6d24f8),0,0}});}real_slot(10)=c(0x6d1bd0);if(state==1){bg[10]=0;bg[11]=0x3ff0000000000000ull;bg[12]=0;projectiles.sound_triggers[18]=1;}}
     else if(!gt(m.sub(m.q(real_slot(2)),m.c(0x6d1b60)),m.q(real_slot(0))))slow();
     for(unsigned i=0;i<8;++i){auto& item=projectiles.secondary[i];const auto angle=m.store(m.add(m.q(item.quad(0x70)),m.q(real_slot(0))));if(state==0){auto sine=math.sine(angle);item.quad(0x18,m.store(m.add(m.mul(sine,m.c(0x6d1cc0)),m.c(0x6d1af0))));}else{auto cosine=math.cosine(angle);item.quad(0x10,m.store(m.add(m.mul(cosine,m.c(0x6d1cc0)),m.c(0x6d1a60))));}}
    }else if(state==2){real_slot(0)=m.store(m.sub(m.q(real_slot(0)),m.q(real_slot(1))));if(gt(m.q(real_slot(3)),m.q(real_slot(1))))real_slot(1)=m.store(m.add(m.q(real_slot(1)),m.c(0x6b2618)));real_slot(4)=m.store(m.sub(m.q(real_slot(4)),m.q(real_slot(7))));real_slot(5)=m.store(m.sub(m.q(real_slot(5)),m.q(real_slot(6))));auto negative=m.q(real_slot(2));negative.negative=!negative.negative;
     if(!lt(negative,m.q(real_slot(0)))){p.integers[0]=3;real_slot(0)=0;real_slot(1)=0;real_slot(3)=real({{c(0x6d1928),c(0x6d1928),c(0x6d24f0),c(0x6d2528),0,0}});bg[10]=0x3ff0000000000000ull;bg[11]=0;bg[12]=0;projectiles.sound_triggers[18]=1;enemy().quad(0x38,c(0x6d1a60));enemy().quad(0x40,c(0x6d1af0));enemy().word(0x2c,4);enemy().word(0x30,0);enemy().word(0x34,0);presentation.spell_ring_reals[0]=c(0x6d1a60);presentation.spell_ring_reals[1]=c(0x6d1ef0);allocation.field_a7ea5c=0;central_init(false);}
     else if(!lt(m.add(negative,m.c(0x6d1b60)),m.q(real_slot(0))))slow();
     for(unsigned i=0;i<8;++i){auto& item=projectiles.secondary[i];auto angle=m.store(m.add(m.div(m.q(real_slot(0)),m.c(0x6d1948)),m.q(item.quad(0x70))));auto cosine=math.cosine(angle);item.quad(0x10,m.store(m.add(m.mul(cosine,m.q(real_slot(4))),m.c(0x6d1a60))));angle=m.store(m.add(m.q(item.quad(0x70)),m.q(real_slot(0))));auto sine=math.sine(angle);item.quad(0x18,m.store(m.add(m.mul(sine,m.q(real_slot(4))),m.c(0x6d1af0))));item.quad(0x20,real_slot(5));}
    }else if(state==3){real_slot(0)=m.store(m.add(m.q(real_slot(0)),m.q(real_slot(1))));if(gt(m.q(real_slot(3)),m.q(real_slot(1))))real_slot(1)=m.store(m.add(m.q(real_slot(1)),m.c(0x6b2618)));else{if(gt(m.c(0x6d1cc0),m.q(real_slot(4))))real_slot(4)=m.store(m.add(m.q(real_slot(4)),m.c(0x6d24f8)));if(gt(m.c(0x6d1d18),m.q(real_slot(5))))real_slot(5)=m.store(m.add(m.q(real_slot(5)),m.c(0x6d1928)));}
     for(unsigned i=0;i<8;++i){auto& item=projectiles.secondary[i];auto angle=m.store(m.add(m.q(item.quad(0x70)),m.q(real_slot(0))));auto cosine=math.cosine(angle);item.quad(0x10,m.store(m.add(m.mul(cosine,m.q(real_slot(4))),m.c(0x6d1a60))));angle=m.store(m.add(m.q(item.quad(0x70)),m.q(real_slot(0))));auto sine=math.sine(angle);item.quad(0x18,m.store(m.add(m.mul(sine,m.q(real_slot(4))),m.c(0x6d1af0))));item.quad(0x20,real_slot(5));}center_radius();if(signed_word(allocation.field_a7ea5c)<=254)++allocation.field_a7ea5c;
    }
   }
  }break;}


 case 12:if(timeline.frame==32)services.retire_boss(0);else if(timeline.frame==196)services.dialog(11);break;
 case 13:if(timeline.frame==1)services.award_stage_clear();else if(timeline.frame==200){++death.stage;timeline.part=0;timeline.subphase=0;timeline.frame=0;p.game_frame=0;services.change_stage_resources(death.stage);}break;
 default:break;
 }
}
}
