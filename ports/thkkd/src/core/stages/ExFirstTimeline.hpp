// 原68B8C0..68D972；EX首道中的生成、三敌联动及输入随机消耗。
#pragma once
#include "core/stages/ExFirstServices.hpp"
#include "core/stages/ExFirstArithmetic.hpp"
#include "core/stages/ExFirstRecords.hpp"
#include "core/stages/StagePatternContext.hpp"
#include "core/entities/ProjectilePattern.hpp"
namespace kaikeidou {
inline void ex_first_segment(StageTimeline& t,EnemyPool& enemies,ProjectilePools& pool,PatternParameters& p,BossStageContext& context,
 const StagePatternContext& extra,Random& random,RandomPrecision precision,GameMath& math,ExFirstServices& services,PlayerShotFireInput& input,const std::array<std::uint32_t,5>& bindings,const PlayerShotFireContext& fire){
 using namespace ex_first_detail;const Arithmetic m{static_cast<unsigned>(precision)};const auto frame=[&](){return signed_word(t.frame);};
 const auto spawn=[&]()->Enemy*{const auto slot=allocate_enemy(enemies);return slot==-1?nullptr:&enemies.enemies[static_cast<std::size_t>(slot)];};
 const auto header=[](Enemy& e,std::uint32_t kind,std::uint32_t hp,std::uint32_t items,std::uint32_t behavior){e.word(4,kind);e.word(24,hp);e.word(20,e.word(24));e.word(28,items);e.word(8,behavior);};
 const auto set_frame=[&](std::uint32_t f){t.frame=f;p.game_frame=f;};
 const auto consume=[&](const std::array<unsigned,5>& counts){for(unsigned key=0;key<5;++key)if(signed_word(ex_first_read_input(bindings[key],fire,input))>0)for(unsigned j=0;j<counts[key];++j)random.real_number(0x3ff0000000000000ull,precision);};
 const auto curve=[&](Enemy& e,bool right,std::uint32_t items,std::uint32_t hp,std::uint32_t period){header(e,right?1u:0u,hp,items,right?121u:120u);e.quad(56,constant(right?0x6d1b58:0x6d1bb8));e.quad(64,constant(0x6d1898));e.quad(168,constant(0x6d2180));e.quad(176,constant(0x6d2fe0));e.quad(184,constant(0x6d2668));if(right){e.quad(192,0);e.quad(200,constant(0x6d1918));e.quad(208,constant(0x6d2fd8));}e.word(128,period);};
 const auto orbit=[&](bool second,unsigned group){if(auto* e=spawn()){header(*e,second?2u:0u,extra.boss_hp_multiplier*150u,2,122);e->quad(168,group==0?constant(0x6d1860):group==1?constant(0x6d1ea8):group==2?0:constant(0x6d1b60));e->quad(176,constant(0x6d2fd0));e->quad(184,constant(group%2==0?(second?0x6d1ec8:0x6d2fc8):(second?0x6d2a48:0x6d1888)));e->quad(192,constant(group==1?0x6d1bb8:group==3?0x6d1b58:0x6d1a60));e->quad(200,constant(group==0?0x6d1b58:group==2?0x6d1d68:0x6d1af0));e->word(128,ex_first_absolute_integer(m.store(m.div(m.c(0x6d1860),m.q(e->quad(176))))));auto v=math.cosine(e->quad(168));e->quad(56,m.store(m.add(m.mul(v,m.q(e->quad(184))),m.q(e->quad(192)))));v=math.sine(e->quad(168));e->quad(64,m.store(m.add(m.mul(v,m.q(e->quad(184))),m.q(e->quad(200)))));}};
 if(t.frame==1)services.music(13,1);
 else if(t.frame==30){for(unsigned j=0;j<3;++j)if(auto* e=spawn()){header(*e,j?3u:4u,8500,1,118);e->quad(56,constant(j==0?0x6d1a60:j==1?0x6d19e8:0x6d19f0));e->quad(64,constant(j?0x6d1aa8:0x6d19f0));e->quad(72,constant(j==0?0x6d1a60:j==1?0x6d29a8:0x6d22b0));e->quad(80,constant(j?0x6d1f08:0x6b25b8));p.integers[2+j]=static_cast<std::uint32_t>(e-enemies.enemies.data());}}
 else if(frame()>=31&&frame()<=1999){if(!enemy_record(enemies,context,p.integers[2]).word(0)||!enemy_record(enemies,context,p.integers[3]).word(0)||!enemy_record(enemies,context,p.integers[4]).word(0)){set_frame(2000);for(unsigned j=2;j<5;++j)enemy_record(enemies,context,p.integers[j]).word(20,0);services.kill_enemy(p.integers[2]);services.kill_enemy(p.integers[3]);services.kill_enemy(p.integers[4]);}random.real_number(0x3ff0000000000000ull,precision);consume({{1,2,3,4,5}});}
 else if(t.frame==2070){services.title();p.reals[0]=constant(0x6d1b60);}
 else if(frame()>=2300&&frame()<=2700){if(remainder(t.frame,40)==0){for(unsigned j=0;j<2;++j)if(auto* e=spawn()){header(*e,j?4u:3u,extra.boss_hp_multiplier*300u,2,119);const auto c=math.cosine(p.reals[0]);e->quad(56,m.store(m.add(m.mul(c,m.c(0x6d1a90)),m.c(j?0x6d1de8:0x6d23c0))));e->quad(64,constant(0x6d19f0));e->quad(168,constant(0x6d1ae8));e->quad(176,m.store(m.mul(random.real_number(constant(0x6d1c60),precision),m.c(0x6d1950))));e->quad(184,constant(0x6d18e0));if(random.integer_bits(1,precision,true))e->quad(184,m.store(m.mul(m.q(e->quad(184)),m.c(0x6aca58))));}p.reals[0]=m.store(m.add(m.q(p.reals[0]),m.c(0x6d1940)));}consume({{2,5,4,3,1}});if(t.frame==2700)set_frame(3000);}
 else if(t.frame==3300||t.frame==3660){const bool right=t.frame==3660;p.reals[0]=constant(0x6d1ad8);p.reals[1]=constant(0x6d18f8);if(auto* e=spawn())curve(*e,right,8,extra.boss_hp_multiplier*2400u,right?4u:24u);}
 else if(frame()>=4100&&frame()<=4400){const auto rem=remainder(t.frame,120);if(rem==0||rem==60)if(auto* e=spawn())curve(*e,rem==60,4,extra.boss_hp_multiplier*400u,rem==60?10u:90u);if(t.frame==4400)set_frame(4600);}
 else if((frame()>=5000&&frame()<=5200)||(frame()>=5500&&frame()<=5700)||(frame()>=6000&&frame()<=6200)||(frame()>=6500&&frame()<=6700)){unsigned group=frame()>=6500?3u:frame()>=6000?2u:frame()>=5500?1u:0u;const auto rem=remainder(t.frame,50);if(rem==0||rem==25)orbit(rem==25,group);if(t.frame==5200||t.frame==5700||t.frame==6200||t.frame==6700)set_frame(t.frame+100u); }
 else if(t.frame==7050){if(auto* e=spawn()){header(*e,5,extra.boss_hp_multiplier*5000u,1,123);e->quad(56,constant(0x6d1af0));e->quad(64,constant(0x6d1848));e->quad(168,0);e->quad(176,0);}}
 else if(frame()>=7200&&frame()<=7800){const auto rem=remainder(t.frame,40);if(rem==0||rem==20)if(auto* e=spawn()){header(*e,rem==20?1u:0u,extra.boss_hp_multiplier*100u,1,124);e->quad(56,constant(rem==20?0x6d19f0:0x6d19e8));e->quad(64,0);e->quad(168,rem==20?0x3ff0000000000000ull:constant(0x6aca58));e->quad(176,constant(rem==20?0x6d1e00:0x6d2fc0));}}
 else if(t.frame==8000)clear_projectiles(pool,100);
 else if(t.frame==8100){++t.part;t.subphase=0;set_frame(0);}
}
}
