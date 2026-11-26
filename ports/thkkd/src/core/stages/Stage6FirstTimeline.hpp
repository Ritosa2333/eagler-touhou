// 原66EC10..66F635；六面首段出敌、参数推进和转段。
#pragma once
#include "core/stages/Stage1Timeline.hpp"
#include "core/stages/StagePatternContext.hpp"
#include "core/stages/Stage6FirstArithmetic.hpp"
#include "core/stages/Stage6FirstRecords.hpp"
#include "core/entities/ProjectilePattern.hpp"
namespace kaikeidou {
inline void stage6_first_segment(StageTimeline& timeline,EnemyPool& enemies,ProjectilePools& pool,PatternParameters& p,
 BossStageContext& context,const StagePatternContext& extra,Random& random,StageServices& services,RandomPrecision precision){
 using namespace stage6_first_detail;const Arithmetic m{static_cast<unsigned>(precision)};const auto frame=[&](){return signed_word(timeline.frame);};
 const auto spawn=[&]()->Enemy*{const auto index=allocate_enemy(enemies);return index==-1?nullptr:&enemies.enemies[static_cast<std::size_t>(index)];};
 const auto header=[](Enemy& e,std::uint32_t kind,std::uint32_t hp,std::uint32_t items,std::uint32_t behavior){e.word(4,kind);e.word(0x18,hp);e.word(0x14,e.word(0x18));e.word(0x1c,items);e.word(8,behavior);};
 if(timeline.frame==1)services.music(11,1);
 else if(timeline.frame==30){if(auto* e=spawn()){header(*e,1,extra.boss_hp_multiplier*9000u,16,99);e->quad(0x38,constant(0x6d1de8));e->quad(0x40,constant(0x6d19f0));e->quad(0xb0,constant(0x6d1a08));e->word(0x80,stage6_first_pick_integer(p.difficulty,{{10,12,14,16,0,0}}));p.integers[0]=static_cast<std::uint32_t>(e-enemies.enemies.data());}}
 else if(frame()>=31&&frame()<=1999){if(enemy_record(enemies,context,p.integers[0]).word(0)==0){timeline.frame=2000;p.game_frame=2000;}}
 else if(timeline.frame==2100){services.title();p.reals[0]=constant(0x6d1a40);p.reals[1]=stage6_first_pick_real(p.difficulty,{{constant(0x6d1a00),constant(0x6d2488),constant(0x6d24e8),constant(0x6d1e40),0,0}});context.extra_reals[0]=stage6_first_pick_real(p.difficulty,{{constant(0x6d1f78),constant(0x6d24e0),constant(0x6d1cf0),constant(0x6d1fb8),0,0}});p.integers[0]=0;p.integers[1]=50;p.integers[2]=stage6_first_pick_integer(p.difficulty,{{2,3,6,8,0,0}});p.integers[3]=stage6_first_pick_integer(p.difficulty,{{3,4,5,7,0,0}});p.integers[4]=1;}
 else if(frame()>=2300&&frame()<=3500){if(remainder(timeline.frame,30)==0){if(auto* e=spawn()){header(*e,1,70,1,100);e->quad(0x38,m.store(m.i(p.integers[4]*215u+192u)));e->quad(0x40,p.reals[0]);e->quad(0xa8,m.store(m.mul(m.i(p.integers[4]),m.c(0x6d24d8))));}p.reals[0]=m.store(m.add(m.q(p.reals[0]),m.c(0x6d1a40)));if(crt_sse2::real(p.reals[0])>428.){p.reals[0]=m.store(m.sub(m.q(p.reals[0]),m.c(0x6d1a50)));p.reals[1]=m.store(m.add(m.q(p.reals[1]),m.c(0x6d1868)));}p.integers[4]=0u-p.integers[4];}}
 else if((frame()>=3700&&frame()<=4000)||(frame()>=4300&&frame()<=4600)){const bool second=frame()>=4300;if(remainder(timeline.frame,4)==0)if(auto* e=spawn()){header(*e,1,1,2,101);e->quad(0x38,m.store(m.div(random.real_number(constant(0x6d24d0),precision),m.c(0x6b25b8))));e->quad(0x40,constant(0x6d19f0));e->word(0x80,0);e->word(0x84,second?1u:0u);e->quad(0xa8,constant(0x6d1ad8));e->quad(0xb0,m.store(m.sub(m.c(0x6d1960),m.mul(m.i(timeline.frame-(second?4300u:3700u)),m.c(0x6d24c8)))));}}
 else if((frame()>=4900&&frame()<=5300)||(frame()>=5500&&frame()<=5900)){const bool second=frame()>=5500;
  if(!second&&timeline.frame==4900)p.reals[0]=stage6_first_pick_real(p.difficulty,{{constant(0x6d1b18),constant(0x6d1ce0),constant(0x6d24c0),constant(0x6d2480),0,0}});
  if(remainder(timeline.frame,2)==0)if(auto* e=spawn()){header(*e,second?0u:1u,30,1,second?103u:102u);const auto random_x=random.real_number(constant(0x6d24b8),precision);e->quad(0x38,m.store(second?m.sub(m.c(0x6d19f0),m.div(random_x,m.c(0x6b25b8))):m.add(m.div(random_x,m.c(0x6b25b8)),m.c(0x6d19e8))));e->quad(0x40,m.store(m.add(m.div(random.real_number(constant(0x6d24b0),precision),m.c(0x6b25b8)),m.c(0x6d1ba8))));e->word(0x80,stage6_first_pick_integer(p.difficulty,{{22,20,18,16,0,0}}));e->quad(0xa8,m.store(m.add(m.div(random.real_number(constant(0x6d2028),precision),m.c(0x6b25b8)),m.c(0x6d1910))));e->word(12,random.integer_bits(e->word(0x80)-1u,precision,true));}
 }
 else if(timeline.frame==6200)clear_projectiles(pool,100);
 else if(timeline.frame==6250){++timeline.part;timeline.subphase=0;timeline.frame=0;p.game_frame=0;}
}
}
