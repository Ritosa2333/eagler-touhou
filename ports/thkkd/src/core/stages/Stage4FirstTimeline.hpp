// 四面首段677870..678BFB；各次分配、数值存回和参数读取保持原顺序。
#pragma once
#include "core/stages/Stage1Timeline.hpp"
#include "core/entities/ProjectilePattern.hpp"
#include "core/entities/BossStageContext.hpp"
#include "core/math/GameMath.hpp"
#include "core/stages/Stage4FirstConstants.hpp"
namespace kaikeidou {
inline void stage4_first_segment(StageTimeline& timeline,EnemyPool& enemies,ProjectilePools& projectiles,
 PatternParameters& p,BossStageContext& context,Random& random,GameMath& math,RandomPrecision precision,StageServices& services){
 using namespace binary_float;using stage4_first_detail::constant;
 const stage_detail::Arithmetic m{static_cast<unsigned>(precision)};
 const auto c=[](std::uint32_t a){return constant(a);};const auto n=[&](std::uint32_t a){return binary64(c(a));};
 const auto frame=[&](){std::int32_t v;std::memcpy(&v,&timeline.frame,4);return v;};
 const auto pick=[&](std::array<std::uint32_t,6> a){return a[p.difficulty<6?p.difficulty:3];};
 const auto real=[&](std::array<std::uint64_t,6> a){return a[p.difficulty<6?p.difficulty:3];};
 const auto spawn=[&]()->Enemy*{auto slot=allocate_enemy(enemies);return slot==-1?nullptr:&enemies.enemies[static_cast<std::size_t>(slot)];};
 const auto header=[](Enemy& e,std::uint32_t kind,std::uint32_t hp,std::uint32_t items,std::uint32_t behavior){e.word(4,kind);e.word(0x18,hp);e.word(0x14,hp);e.word(0x1c,items);e.word(8,behavior);};
 const auto tracked_active=[&](){
  const auto displacement=p.integers[0]*248u;std::uint32_t value;
  if(displacement<=19836u){std::memcpy(&value,reinterpret_cast<const std::uint8_t*>(enemies.enemies.data())+displacement,4);return value;}
  if(displacement>=0xffffff08u&&displacement<=0xfffffffcu)return context.enemy_prefix.word(displacement+248u);
  throw std::out_of_range("Stage4 tracked enemy address outside context");
 };
 if(timeline.frame==1)services.music(7,1);
 else if(timeline.frame==30||timeline.frame==200){
  const bool right=timeline.frame==200;
  for(std::uint32_t i=0;i<12;++i)if(auto* e=spawn()){
   header(*e,i%3,80,1,60);e->quad(0x38,c(right?0x6d1bb8:0x6d1b58));e->quad(0x40,c(right?0x6d1cc0:0x6d1a90));
   auto angle=m.mul(m.i(i),n(0x6d1bb0));e->quad(0xa8,m.store(right?m.sub(n(0x6d1fc0),angle):m.add(angle,n(0x6d2488))));
   e->quad(0xb0,m.store(m.add(m.mul(m.i(i),n(0x6d1810)),n(0x6d2668))));
   auto turn=m.i(i);if(right)turn.negative=!turn.negative;e->quad(0xb8,m.store(m.mul(turn,n(0x6d1900))));
  }
  if(!right){p.integers[0]=pick({{38,30,25,20,0,0}});p.reals[0]=c(0x6d1910);p.reals[1]=c(0x6d18f0);context.extra_reals[0]=real({{c(0x6d1b28),c(0x6d1948),c(0x6d2660),c(0x6d2548),0,0}});}
 }else if(timeline.frame==420)services.title();
 else if(timeline.frame==550){
  for(std::uint32_t i=0;i<3;++i)if(auto* e=spawn()){
   header(*e,5,10000,1,61);e->quad(0xa8,c(0x6d1a60));e->quad(0xb0,c(0x6d1dd0));e->quad(0xb8,m.store(m.mul(m.i(i),n(0x6d2138))));e->quad(0xc0,0);
   auto cosine=math.cosine(e->quad(0xb8));auto a=e->quad(0xc0);auto saved=m.store(cosine);auto sine=math.sine(a);
   e->quad(0x38,m.store(m.add(m.mul(m.mul(sine,binary64(saved)),n(0x6d1d78)),binary64(e->quad(0xa8)))));
   auto sine0=math.sine(e->quad(0xb8));a=e->quad(0xc0);saved=m.store(sine0);sine=math.sine(a);
   e->quad(0x40,m.store(m.add(m.mul(m.mul(sine,binary64(saved)),n(0x6d1d78)),binary64(e->quad(0xb0)))));e->word(0x80,pick({{120,100,80,60,0,0}}));e->word(0xc,0);
  }
 }else if(frame()>=800&&frame()<=1100){
  if(frame()%15==0){if(auto* e=spawn()){
   header(*e,0,80,2,62);e->quad(0xa8,c(0x6d1b58));e->quad(0xb0,m.store(m.mul(m.i(timeline.frame),n(0x6d1920))));e->quad(0xb8,m.store(m.mul(m.i(timeline.frame),n(0x6d1960))));
   auto sine=math.sine(e->quad(0xb0));e->quad(0x38,m.store(m.add(m.mul(sine,n(0x6d2658)),binary64(e->quad(0xa8)))));e->quad(0x40,m.store(m.i(p.integers[1])));
  }p.integers[1]+=9;}
  if(timeline.frame==800){p.integers[0]=pick({{35,25,20,16,0,0}});p.integers[1]=30;context.extra_reals[0]=real({{c(0x6d1b28),c(0x6d1948),c(0x6d2660),c(0x6d2548),0,0}});}
 }else if(frame()>=1400&&frame()<=1600){
  if(frame()%35==0)for(unsigned i=0;i<2;++i)if(auto* e=spawn()){header(*e,1,130,2,63);e->quad(0x38,c(i?0x6d2650:0x6d1ae8));e->quad(0x40,c(0x6d1b58));}
  if(timeline.frame==1400){p.integers[0]=pick({{120,90,40,25,0,0}});p.reals[0]=real({{c(0x6d2488),c(0x6d1ff8),c(0x6d2660),c(0x6d2548),0,0}});}
 }else if(frame()>=1900&&frame()<=2100){
  if(frame()%35==0)for(unsigned i=0;i<2;++i)if(auto* e=spawn()){header(*e,1,130,2,63);e->quad(0x38,c(i?0x6d2648:0x6d23e0));e->quad(0x40,c(0x6d1b58));}
 }else if(frame()>=2300&&frame()<=2500){
  if(frame()%30==0)for(unsigned i=0;i<2;++i)if(auto* e=spawn()){header(*e,2,80,2,64);e->quad(0x38,c(i?0x6d1bb8:0x6d1b58));e->quad(0x40,c(0x6b25f8));e->quad(0xa8,c(i?0x6d2640:0x6d1ad0));e->quad(0xb0,0);e->quad(0xb8,c(0x6a7708));e->word(0xc,static_cast<std::uint32_t>(frame()%19));}
  if(timeline.frame==2300){p.integers[0]=4;p.integers[1]=26;p.integers[2]=pick({{60,50,40,30,0,0}});p.reals[0]=real({{c(0x6d1ff8),c(0x6d2130),c(0x6d2548),c(0x6d25a8),0,0}});}
 }else if(frame()>=2800&&frame()<=3400){
  const auto r=frame()%78;
  if(r==0||r==26||r==52)if(auto* e=spawn()){
   const auto kind=static_cast<std::uint32_t>(r/26);header(*e,kind,99999999,2,65);
   const auto rv=random.real_number(c(0x6b25b8),precision);e->quad(0x38,m.store(kind==1?m.sub(n(0x6d2288),rv):m.add(rv,n(kind==0?0x6b25f8:0x6d2108))));e->quad(0x40,c(0x6d1b58));
   e->quad(0xa8,kind==2?0:c(kind==0?0x6b2650:0x6d19c0));e->quad(0xb0,kind==2?c(0x6b2650):0);
   e->quad(0xb8,c(kind==0?0x6a7eb8:0x6d1e48));e->quad(0xc0,c(kind==2?0x6a7eb8:0x6d1e48));e->quad(0xc8,c(kind==1?0x6a7eb8:0x6d1e48));
  }
 }else if(timeline.frame==3700){
  if(auto* e=spawn()){header(*e,5,9000,1,66);e->quad(0x38,c(0x6d1a60));e->quad(0x40,c(0x6d1b58));e->quad(0xa8,0);e->word(0x80,pick({{24,18,12,8,0,0}}));e->word(0x84,pick({{60,40,15,10,0,0}}));p.integers[0]=static_cast<std::uint32_t>(e-enemies.enemies.data());}
 }else if(frame()>=3701&&frame()<=5500){if(!tracked_active()){timeline.frame=5900;p.game_frame=5900;}}
 else if(timeline.frame==6000){
  if(auto* e=spawn()){header(*e,5,9000,1,67);e->quad(0x38,c(0x6d1b58));e->quad(0x40,c(0x6d1a88));e->word(0x80,pick({{35,25,15,10,0,0}}));e->quad(0xa8,0);p.reals[0]=real({{c(0x6d2628),c(0x6d2630),c(0x6d2638),c(0x6d19c8),0,0}});p.integers[0]=static_cast<std::uint32_t>(e-enemies.enemies.data());}
 }else if(frame()>=6001&&frame()<=8500){if(!tracked_active()){timeline.frame=8800;p.game_frame=8800;}}
 else if(timeline.frame==9000)clear_projectiles(projectiles,100);
 else if(timeline.frame==9040){++timeline.part;timeline.subphase=0;timeline.frame=0;p.game_frame=0;}
}
}
