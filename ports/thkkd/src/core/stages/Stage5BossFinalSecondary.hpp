// 原副弹80/82/83/84；word4为行为、word8由外部调度递增。
#pragma once
#include "core/stages/Stage5BossFinalArithmetic.hpp"
#include "core/stages/Stage5BossFinalRecords.hpp"
#include "core/stages/Stage5BossFinalEmitter.hpp"
#include "core/entities/BossPhaseContext.hpp"
namespace kaikeidou {
inline void update_stage5_boss_final_secondary(ProjectilePools& pool,std::uint32_t index,std::uint32_t behavior,EnemyPool& enemies,
 const BossPhaseContext& phase,BossStageContext& context,const ObjectRecord<240>& player,PatternParameters& p,Random& random,RandomPrecision precision,GameMath& math,bool sse_conversion=true){
 using namespace stage5_boss_final_detail;const Arithmetic m{static_cast<unsigned>(precision)};PoolRecord b{pool,0xa8ce60u+index*192u};
 const auto q=[&](std::size_t off){return m.q(b.quad(off));};const auto put=[&](std::size_t off,binary_float::Number value){b.quad(off,m.store(value));};
 const auto approach=[&](std::size_t off,std::uint64_t target,std::uint32_t divisor){stage5_boss_final_ease(b,off,target,constant(divisor),m.precision);};
 const auto retire=[&](){approach(0x20,0,0x6d1a40);if(crt_sse2::real(b.quad(0x20))<=1.)b.word(0,0);};
 if(behavior==80||behavior==82){if(b.word(0x48)==0)approach(0x20,b.quad(0x28),0x6d1a40);else if(b.word(0x48)==1){if(behavior==82){b.quad(0x10,enemy_record(enemies,context,phase.boss_index).quad(0x38));put(0x18,m.add(m.q(enemy_record(enemies,context,phase.boss_index).quad(0x40)),m.c(0x6d1be0)));}}else if(b.word(0x48)==0xffffffffu)retire();return;}
 if(behavior==83){
  if(b.word(0x48)==0)approach(0x20,b.quad(0x28),0x6d1a40);
  else if(b.word(0x48)==1)approach(0x18,b.quad(0x70),0x6d1a90);
  else if(b.word(0x48)==2){approach(0x18,b.quad(0x70),0x6d1a90);put(0x20,m.mul(q(0x20),m.c(0x6d18f0)));if(crt_sse2::real(b.quad(0x20))<=.8){
   const auto count=stage5_boss_final_pick_integer(p.difficulty,{{2,3,3,4,0,0}});auto speed=constant(0x6d18b0);
   for(std::uint32_t i=0;signed_word(i)<=signed_word(count);++i){const auto difficulty=p.difficulty;const auto spacing=constant(difficulty==0?0x6d2290:difficulty==1?0x6d27b0:difficulty==2?0x6d2978:0x6d2d60);const auto bullets=difficulty==0?9u:difficulty==1?11u:difficulty==2?15u:19u;
    const auto y=b.quad(0x18),x=b.quad(0x10);const auto blue=stage5_boss_final_color_integer(m.mul(q(0x40),m.c(0x6a7eb8)),sse_conversion);const auto green=stage5_boss_final_color_integer(m.mul(q(0x38),m.c(0x6a7eb8)),sse_conversion);const auto red=stage5_boss_final_color_integer(m.mul(q(0x30),m.c(0x6a7eb8)),sse_conversion);
    stage5_boss_final_emit(pool,{{0,0,{{red,green,blue}},0,255,0,41},x,y,bullets,spacing,speed,0,0,0},player,p,random,precision,math);speed=m.store(m.add(m.q(speed),m.c(0x6d1fb8)));
   }
   b.quad(0x18,constant(0x6d31c0));b.word(0x48,1);b.quad(0x70,constant(0x6d1a28));b.quad(0x20,b.quad(0x28));
  }}else if(b.word(0x48)==0xffffffffu)retire();return;
 }
 if(behavior==84){
  if(b.word(0x48)==0){approach(0x20,b.quad(0x28),0x6d1a40);if(crt_sse2::real(b.quad(0x20))>47.){b.word(0x48,1);enemy_record(enemies,context,phase.boss_index).word(0x2c,3);enemy_record(enemies,context,phase.boss_index).word(0x30,0);enemy_record(enemies,context,phase.boss_index).word(0x34,0);}}
  else if(b.word(0x48)==1){put(0x20,m.mul(q(0x20),m.c(0x6d1970)));if(crt_sse2::real(b.quad(0x20))<=.8){auto speed=constant(0x6d18b0);for(unsigned i=0;i<3;++i){const auto y=b.quad(0x18),x=b.quad(0x10);stage5_boss_final_emit(pool,{{0,0,{{255,255,255}},0,255,0,41},x,y,21,constant(0x6d31b0),speed,0,0,0},player,p,random,precision,math);speed=m.store(m.add(m.q(speed),m.c(0x6d1fb8)));}b.word(0,0);}}
  else if(b.word(0x48)==0xffffffffu)retire();return;
 }
 throw std::out_of_range("Stage5 final secondary outside80/82/83/84");
}
}
