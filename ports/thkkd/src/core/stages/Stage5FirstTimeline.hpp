// 原675C70..676B6C；背景先更新，随后处理首段敌人和转段。
#pragma once
#include "core/stages/Stage1Timeline.hpp"
#include "core/entities/ProjectilePattern.hpp"
#include "core/entities/BossStageContext.hpp"
#include "core/stages/StageResourceContext.hpp"
#include "core/math/GameMath.hpp"
#include "core/stages/Stage5FirstArithmetic.hpp"
namespace kaikeidou {
inline void stage5_first_segment(StageTimeline& timeline,EnemyPool& enemies,ProjectilePools& projectiles,PatternParameters& p,
 BossStageContext& context,StageResourceContext& background,const ObjectRecord<240>& player,Random& random,GameMath& math,RandomPrecision precision,StageServices& services){
 (void)random;using namespace stage5_first_detail;const Arithmetic m{static_cast<unsigned>(precision)};const auto c=[](std::uint32_t a){return constant(a);};
 const auto frame=[&](){return signed_word(timeline.frame);};const auto pick=[&](std::array<std::uint32_t,6> v){return stage5_first_pick_integer(p.difficulty,v);};
 auto& bg=background.reals;auto sine=math.sine(bg[1]);bg[0]=m.store(m.add(m.add(m.mul(sine,m.c(0x6d1cf8)),m.c(0x6d19e0)),m.q(bg[0])));sine=math.sine(bg[3]);bg[2]=m.store(m.add(m.add(m.mul(sine,m.c(0x6d1cf8)),m.c(0x6d1aa0)),m.q(bg[2])));sine=math.sine(bg[2]);bg[4]=m.store(m.mul(sine,m.c(0x6d1aa8)));auto cosine=math.cosine(bg[0]);bg[5]=m.store(m.add(m.mul(cosine,m.q(bg[4])),m.c(0x6d1a60)));sine=math.sine(bg[0]);bg[6]=m.store(m.add(m.mul(sine,m.q(bg[4])),m.c(0x6d1af0)));
 const auto spawn=[&]()->Enemy*{const auto slot=allocate_enemy(enemies);return slot==-1?nullptr:&enemies.enemies[static_cast<std::size_t>(slot)];};
 const auto header=[](Enemy& e,std::uint32_t kind,std::uint32_t hp,std::uint32_t behavior){e.word(4,kind);e.word(0x18,hp);e.word(0x14,hp);e.word(0x1c,1);e.word(8,behavior);};
 const auto position=[&](Enemy& e){e.quad(0x38,bg[5]);e.quad(0x40,bg[6]);};
 const auto tracked_active=[&](std::uint32_t index){const auto displacement=index*248u;std::uint32_t v;if(displacement<=19836u){std::memcpy(&v,reinterpret_cast<const std::uint8_t*>(enemies.enemies.data())+displacement,4);return v;}if(displacement>=0xffffff08u&&displacement<=0xfffffffcu)return context.enemy_prefix.word(displacement+248u);throw std::out_of_range("Stage5 tracked enemy outside context");};
 if(timeline.frame==1)services.music(9,1);
 else if(timeline.frame==30){for(unsigned i=0;i<3;++i)if(auto* e=spawn()){header(*e,5,5100,79);position(*e);e->quad(0xa8,c(i==0?0x6d1ae8:i==1?0x6d23e0:0x6d1a60));e->quad(0xb0,c(i==2?0x6d1d50:0x6d1cc0));e->word(0x80,i==0?255u:0u);e->word(0x84,i==2?255u:0u);e->word(0x88,i==1?255u:0u);p.integers[i]=static_cast<std::uint32_t>(e-enemies.enemies.data());}p.integers[3]=pick({{90,80,70,60,0,0}});p.integers[4]=pick({{180,160,140,120,0,0}});p.reals[0]=c(0x6d1948);p.reals[1]=c(0x6d1988);}
 else if(frame()>=31&&frame()<=1999){if(!tracked_active(p.integers[0])&&!tracked_active(p.integers[1])&&!tracked_active(p.integers[2])){timeline.frame=2000;p.game_frame=2000;}}
 else if(timeline.frame==2100)services.title();
 else if(timeline.frame==2400){for(unsigned side=0;side<2;++side)if(auto* e=spawn()){header(*e,3,1600,80);position(*e);e->quad(0xa8,0);e->word(0x80,pick({{34,32,30,28,0,0}}));e->word(0x84,side?0xffffffffu:1u);}}
 else if((frame()>=2700&&frame()<=3100)||(frame()>=3800&&frame()<=4100)){const auto kind=frame()<=3100?1u:0u;if(remainder(timeline.frame,41)==0)for(unsigned side=0;side<2;++side)if(auto* e=spawn()){header(*e,kind,100,81);e->quad(0x38,m.store(side?m.sub(m.q(bg[5]),m.c(0x6b25f8)):m.add(m.q(bg[5]),m.c(0x6b25f8))));e->quad(0x40,bg[6]);e->quad(0xa8,c(side?0x6d1e70:0x6d1a00));}}
 else if(timeline.frame==3500){for(std::uint32_t i=0;i<10;++i)if(auto* e=spawn()){header(*e,4,p.difficulty*50u+360u,82);position(*e);auto angle=m.store(m.add(m.mul(m.i(i),m.c(0x6d1dd8)),m.c(0x6d1de0)));cosine=math.cosine(angle);e->quad(0xa8,m.store(m.mul(cosine,m.c(0x6d18e0))));angle=m.store(m.add(m.mul(m.i(i),m.c(0x6d1dd8)),m.c(0x6d1de0)));sine=math.sine(angle);e->quad(0xb0,m.store(m.mul(sine,m.c(0x6d18e0))));e->word(0x80,pick({{35,33,31,29,0,0}}));}}
 else if(frame()>=4500&&frame()<=4800){if(remainder(timeline.frame,50)==0){if(timeline.frame==4500)p.integers[0]=0;const auto dx=m.store(m.sub(m.q(player.quad(0x90)),m.q(bg[5]))),dy=m.store(m.sub(m.q(player.quad(0x98)),m.q(bg[6])));const auto base=m.store(math.atan2(dy,dx));for(std::uint32_t i=0;i<5;++i)if(auto* e=spawn()){header(*e,1,p.difficulty*50u+360u,83);position(*e);auto angle=m.store(m.add(m.mul(m.i(i),m.c(0x6d2618)),m.q(base)));cosine=math.cosine(angle);e->quad(0xa8,m.store(m.mul(cosine,m.c(0x6d1f50))));angle=m.store(m.add(m.mul(m.i(i),m.c(0x6d2618)),m.q(base)));sine=math.sine(angle);e->quad(0xb0,m.store(m.mul(sine,m.c(0x6d1f50))));e->word(0x80,pick({{66,62,58,54,0,0}}));e->word(0x84,static_cast<std::uint32_t>(remainder(p.integers[0],e->word(0x80))));p.integers[0]+=11;}}}
 else if(timeline.frame==5000){if(auto* e=spawn()){header(*e,5,4000,84);position(*e);const auto dx=m.store(m.sub(m.q(player.quad(0x90)),m.q(e->quad(0x38)))),dy=m.store(m.sub(m.q(player.quad(0x98)),m.q(e->quad(0x40))));const auto angle=math.atan2(dy,dx);e->quad(0xa8,m.store(m.add(angle,m.c(0x6d1860))));p.integers[0]=static_cast<std::uint32_t>(e-enemies.enemies.data());}}
 else if(frame()>=5001&&frame()<=8999){if(!tracked_active(p.integers[0])){timeline.frame=9000;p.game_frame=9000;}}
 else if(timeline.frame==9400)clear_projectiles(projectiles,100);
 else if(timeline.frame==9500){++timeline.part;timeline.subphase=0;timeline.frame=0;p.game_frame=0;}
}
}
