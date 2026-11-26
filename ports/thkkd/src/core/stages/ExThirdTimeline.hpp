// 原66CD30..66DA90；EX后道中、两次追踪敌退场等待与转入关底。
#pragma once
#include "core/stages/Stage1Timeline.hpp"
#include "core/stages/StagePatternContext.hpp"
#include "core/stages/ExThirdArithmetic.hpp"
#include "core/stages/ExThirdRecords.hpp"
#include "core/entities/ProjectilePattern.hpp"
#include "core/math/GameMath.hpp"
namespace kaikeidou {
inline std::uint32_t ex_third_convert(binary_float::Number n){return binary_float::truncate(binary_float::rounded(n,0,53));}
inline void ex_third_segment(StageTimeline& t,EnemyPool& enemies,ProjectilePools& pool,PatternParameters& p,BossStageContext& context,const StagePatternContext& extra,Random& random,RandomPrecision precision,GameMath& math){
 using namespace ex_third_detail;const Arithmetic m{static_cast<unsigned>(precision)};const auto f=[&](){return signed_word(t.frame);};
 const auto spawn=[&]()->Enemy*{const auto slot=allocate_enemy(enemies);return slot==-1?nullptr:&enemies.enemies[static_cast<std::size_t>(slot)];};
 const auto header=[](Enemy& e,std::uint32_t kind,std::uint32_t hp,std::uint32_t items,std::uint32_t behavior){e.word(4,kind);e.word(24,hp);e.word(20,e.word(24));e.word(28,items);e.word(8,behavior);};const auto frame=[&](std::uint32_t n){t.frame=n;p.game_frame=n;};
 if(t.frame==100){if(auto* e=spawn()){header(*e,5,extra.boss_hp_multiplier*3000u,8,125);e->quad(56,constant(0x6d1b58));e->quad(64,constant(0x6b25b8));e->word(128,0);e->quad(168,0x3ff0000000000000ull);}p.reals[0]=0;}
 else if(f()>=660&&f()<=1200){if(remainder(t.frame,20)==0){if(auto* e=spawn()){header(*e,0,extra.boss_hp_multiplier*10u,1,126);auto v=math.cosine(p.reals[0]);e->quad(56,m.store(m.add(m.mul(v,m.c(0x6b25f8)),m.c(0x6d1c30))));e->quad(64,constant(0x6d1830));v=math.sine(p.reals[0]);e->word(128,ex_third_convert(m.add(m.mul(v,m.c(0x6d2490)),m.c(0x6d2490))));}p.reals[0]=m.store(m.add(m.q(p.reals[0]),m.c(0x6d19c0)));}if(t.frame==790)if(auto* e=spawn()){header(*e,4,extra.boss_hp_multiplier*2300u,2,127);e->quad(56,constant(0x6d1b58));e->quad(64,constant(0x6d1a40));}}
 else if(t.frame==1400||t.frame==1520||t.frame==1640||t.frame==1760){if(auto* e=spawn()){header(*e,5,extra.boss_hp_multiplier*4000u,1,128);e->quad(56,constant(0x6d1bb8));e->quad(64,constant(0x6d1898));e->word(140,0);if(t.frame==1400){e->word(128,0);e->word(132,0);e->word(136,255);}else if(t.frame==1520){e->word(128,0);e->word(132,255);e->word(136,0);}else if(t.frame==1640){e->word(128,255);e->word(132,255);e->word(136,0);}else{e->word(128,255);e->word(132,0);e->word(136,0);}}}
 else if((f()>=1900&&f()<=2300)||(f()>=2400&&f()<=2800)){const bool right=f()>=2400;if(remainder(t.frame,8)==0)if(auto* e=spawn()){header(*e,right?1u:0u,extra.boss_hp_multiplier*80u,1,right?130u:129u);auto v=m.mul(random.real_number(constant(0x6b25b8),precision),m.c(0x6d1960));e->quad(56,m.store(right?m.sub(m.c(0x6d1bb8),v):m.add(v,m.c(0x6d1b58))));v=m.mul(random.real_number(constant(0x6d1af8),precision),m.c(0x6d1960));e->quad(64,m.store(m.add(v,m.c(right?0x6d1d50:0x6d19f0))));e->quad(168,constant(right?0x6d21f8:0x6d2488));}}
 else if(f()>=2900&&f()<=3300){if(remainder(t.frame,8)==0)if(auto* e=spawn()){header(*e,remainder(t.frame,16)!=0?1u:0u,extra.boss_hp_multiplier*100u,1,131);auto v=m.mul(random.real_number(constant(0x6d2028),precision),m.c(0x6d1960));e->quad(56,m.store(m.sub(m.add(v,m.c(0x6d1a60)),m.c(0x6d1b90))));e->quad(64,constant(0x6d19f0));v=m.mul(random.real_number(constant(0x6b25f8),precision),m.c(0x6a7708));e->quad(168,m.store(m.add(v,m.c(0x6d1920))));if(random.integer_bits(1,precision,true))e->quad(168,m.store(m.mul(m.q(e->quad(168)),m.c(0x6aca58))));}}
 else if(f()>=3500&&f()<=3900){const auto rem=remainder(t.frame,36);if(rem==0||rem==18)if(auto* e=spawn()){header(*e,1,extra.boss_hp_multiplier*100u,2,132);e->quad(56,constant(0x6d1a60));e->quad(64,constant(0x6d19f0));e->quad(168,constant(rem==0?0x6d2480:0x6d2478));e->quad(176,constant(rem==0?0x6d2178:0x6d1968));}}
 else if(t.frame==4000||t.frame==5000){const bool final=t.frame==5000;if(auto* e=spawn()){header(*e,5,extra.boss_hp_multiplier*(final?10000u:5000u),1,final?134u:133u);e->quad(56,constant(0x6d1a60));e->quad(64,constant(0x6d1b58));e->quad(168,constant(0x6d1910));p.integers[0]=static_cast<std::uint32_t>(e-enemies.enemies.data());}}
 else if(f()>=4001&&f()<=4998){if(!enemy_record(enemies,context,p.integers[0]).word(0))frame(4999);}
 else if(f()>=5001&&f()<=6999){if(!enemy_record(enemies,context,p.integers[0]).word(0))frame(7000);}
 else if(t.frame==7050)clear_projectiles(pool,100);
 else if(t.frame==7300){++t.part;t.subphase=0;frame(0);}
}
}
