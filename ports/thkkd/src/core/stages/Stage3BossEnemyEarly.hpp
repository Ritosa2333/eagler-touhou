// 原敌55..58，保留入场移动、姿态写序、发射优先级与两次随机重定位。
#pragma once
#include "core/stages/Stage3BossEarlyArithmetic.hpp"
#include "core/entities/EnemyPool.hpp"
#include "core/entities/ProjectilePattern.hpp"
#include "core/shared/FinalProjectileRecord.hpp"
#include "core/math/GameMath.hpp"
namespace kaikeidou {
inline void update_stage3_boss_enemy_early(std::uint32_t behavior,Enemy& enemy,ProjectilePools& pools,
 std::uint64_t player_x,std::uint64_t player_y,PatternParameters& parameters,Random& random,RandomPrecision precision,GameMath& math){
 using namespace binary_float;using namespace stage3_boss_early_detail;const auto bits=static_cast<unsigned>(precision);
 const auto c=[](std::uint32_t a){return binary64(constant(a));};const auto q=[&](std::size_t o){return binary64(enemy.quad(o));};
 const auto pose=[&](){enemy.word(0x2c,3);enemy.word(0x30,0);enemy.word(0x34,0);};
 const auto move=[&](){enemy.quad(0x38,to_binary64(add(q(0x38),q(0xa8),bits)));enemy.quad(0x40,to_binary64(add(q(0x40),q(0xb0),bits)));};
 const auto reset=[&](){
  enemy.word(0xc,0xffffffffu);
  const auto rx=random.real_number(constant(0x6d1d00),precision);
  enemy.quad(0xa8,to_binary64(divide(subtract(add(rx,c(0x6d22b0),bits),q(0x38),bits),c(0x6d1ae8),bits)));
  const auto ry=random.real_number(constant(0x6d1ae8),precision);
  enemy.quad(0xb0,to_binary64(divide(subtract(add(ry,c(0x6d1d50),bits),q(0x40),bits),c(0x6d1ae8),bits)));
 };
 const auto spawn=[&](){return allocate_projectile(pools,{2,1,{{64,64,64}},2,255,1,41});};
 const auto velocity=[&](Projectile& b,std::uint64_t angle){b.quad(0x50,to_binary64(multiply(math.cosine(angle),c(0x6d1b18),bits)));b.quad(0x58,to_binary64(multiply(math.sine(angle),c(0x6d1b18),bits)));b.quad(0xa0,0);b.quad(0xa8,enemy.quad(0xb8));b.quad(0xb0,angle);b.word(0x34,1);};
 if(behavior==55){move();return;}
 if(behavior<56||behavior>58)throw std::out_of_range("Stage3 early boss enemy outside 55..58");
 if(behavior==58&&enemy.word(0xc)==10)pose();
 if(signed_word(enemy.word(0xc))<=39)move();
 if(behavior==56){
  if(enemy.word(0xc)==26)pose();
  if(enemy.word(0xc)==56){
   const auto dx=to_binary64(subtract(binary64(player_x),q(0x38),bits)),dy=to_binary64(subtract(binary64(player_y),q(0x40),bits));auto angle=to_binary64(math.atan2(dy,dx));
   for(unsigned i=0;i<=3;++i){const auto slot=spawn();if(slot!=-1){auto& b=pools.bullets[slot];b.word(8,42);b.quad(0x40,enemy.quad(0x38));b.quad(0x48,enemy.quad(0x40));velocity(b,angle);}angle=to_binary64(add(binary64(angle),c(0x6d1b60),bits));}
  }else if(enemy.word(0xc)==enemy.word(0x80))reset();return;
 }
 if(behavior==57){
  const auto rem=remainder(parameters.game_frame,60);
  if(rem==0||rem==30){
   const auto slot=spawn();if(slot!=-1){auto& b=pools.bullets[slot];b.word(8,43);
    const auto angle=to_binary64(rem?subtract(c(0x6d18e0),multiply(random.real_number(constant(0x6d1e38),precision),c(0x6d1950),bits),bits):add(multiply(random.real_number(constant(0x6d1e38),precision),c(0x6d1950),bits),c(0x6d22d0),bits));
    b.quad(0x40,constant(rem?0x6d1a80:0x6d1a78));b.quad(0x48,to_binary64(add(random.real_number(constant(0x6d1ae8),precision),c(0x6d21f0),bits)));velocity(b,angle);
   }
  }else if(enemy.word(0xc)==enemy.word(0x80))reset();return;
 }
 if(enemy.word(0xc)==40){
  const auto count=stage3_boss_early_pick(parameters.difficulty,{{3,4,6,7,0,0}});
  auto position=to_binary64(add(random.real_number(constant(0x6d1e48),precision),Number{},bits));
  // 原零分母生成局部+inf；此时循环只执行一次，首个有限出生位置仍保留。
  const auto stride=count?to_binary64(divide(c(0x6d22c8),integer(count),bits)):0x7ff0000000000000ull;
  for(std::uint32_t i=0;i<=count;++i){
   const auto slot=spawn();if(slot!=-1){auto& b=pools.bullets[slot];b.word(8,44);b.quad(0x40,position);b.quad(0x48,constant(0x6d22c0));b.quad(0x58,0);
    const auto distance=final_projectile_absolute(to_binary64(subtract(q(0x38),binary64(b.quad(0x40)),bits)));
    b.quad(0xa0,to_binary64(add(multiply(binary64(distance),c(0x6d22b8),bits),c(0x6d1e00),bits)));b.word(0x34,1);
   }
   position=count?to_binary64(add(binary64(position),binary64(stride),bits)):stride;
  }
 }else if(enemy.word(0xc)==220)reset();
}
}
