// 原敌51/52；按原分配顺序写入弹37、40、41，保留失败分配及阶段回绕。
#pragma once
#include "core/stages/Stage3MidbossDependenciesArithmetic.hpp"
#include "core/entities/EnemyPool.hpp"
#include "core/entities/ProjectilePattern.hpp"
#include "core/player/OptionsMotion.hpp"
namespace kaikeidou {
inline void update_stage3_midboss_enemy(std::uint32_t behavior,Enemy& enemy,ProjectilePools& pools,
 std::uint64_t player_x,std::uint64_t player_y,PatternParameters& parameters,Random& random,RandomPrecision precision,GameMath& math){
 using namespace binary_float;using namespace stage3_midboss_dependencies_detail;
 (void)player_y;(void)random;const auto bits=static_cast<unsigned>(precision);
 const auto c=[](std::uint32_t a){return binary64(constant(a));};
 const auto q=[&](std::size_t o){return binary64(enemy.quad(o));};
 const auto spawn=[&](std::uint32_t kind,std::int32_t group,std::array<std::uint32_t,3> color,std::uint32_t blend,std::uint32_t delayed){return allocate_projectile(pools,{kind,group,color,blend,255,delayed,41});};
 if(behavior==51){
  enemy.quad(0x38,ease_option_value(enemy.quad(0x38),enemy.quad(0xa8),constant(0x6d1ba8),bits));
  if(remainder(enemy.word(0xc),parameters.integers[2])==40){
   const auto slot=spawn(2,0,enemy.word(0x80)?std::array<std::uint32_t,3>{{255,0,0}}:std::array<std::uint32_t,3>{{0,255,0}},2,1);
   if(slot!=-1){auto& bullet=pools.bullets[slot];bullet.word(8,37);
    bullet.quad(0xa0,to_binary64(divide(subtract(binary64(player_x),q(0x38),bits),c(0x6d1a90),bits)));
    bullet.quad(0xa8,constant(0x6d2328));bullet.quad(0x40,enemy.quad(0x38));bullet.quad(0x48,enemy.quad(0x40));bullet.word(0x78,enemy.word(0x80));enemy.word(0x80,1u-enemy.word(0x80));
   }
  }
  if(static_cast<std::uint32_t>(remainder(enemy.word(0xc),parameters.integers[2]))==parameters.integers[2]-1u)
   enemy.quad(0xa8,to_binary64(divide(add(binary64(player_x),q(0x38),bits),c(0x6d1948),bits)));
  return;
 }
 if(behavior!=52)throw std::out_of_range("Stage3 midboss enemy outside 51..52");
 const auto age=signed_word(enemy.word(0xc));
 if(age<=39){enemy.quad(0x38,ease_option_value(enemy.quad(0x38),enemy.quad(0xa8),constant(0x6d1b88),bits));return;}
 const auto ring=[&](std::uint32_t delay,std::uint32_t right,std::uint32_t left,std::int32_t group){
  const auto slot=spawn(1,group,{{200,100,0}},0,0);if(slot==-1)return;auto& b=pools.bullets[slot];
  b.word(8,40);b.quad(0x40,enemy.quad(0x38));b.quad(0x48,enemy.quad(0x40));b.word(0x78,delay-enemy.word(0xc));b.quad(0xc0,enemy.word(0x80)==1?constant(right):(left?constant(left):0));
  b.quad(0xa0,to_binary64(multiply(math.cosine(b.quad(0xc0)),c(0x6d1b88),bits)));
  b.quad(0xa8,to_binary64(multiply(math.sine(b.quad(0xc0)),c(0x6d1b88),bits)));
  b.quad(0xb0,to_binary64(add(multiply(math.cosine(b.quad(0xc0)),c(0x6d2320),bits),q(0x38),bits)));
  b.quad(0xb8,to_binary64(add(multiply(math.sine(b.quad(0xc0)),c(0x6d2320),bits),q(0x40),bits)));
  b.quad(0xc0,to_binary64(add(binary64(b.quad(0xc0)),c(0x6d1860),bits)));
 };
 const auto aimed=[&](bool circle){
  const auto slot=spawn(2,2,{{255,0,0}},2,1);if(slot==-1)return;auto& b=pools.bullets[slot];b.word(8,41);
  if(!circle){b.quad(0x40,to_binary64(add(integer(enemy.word(0x80)*45u),q(0x38),bits)));b.quad(0x48,enemy.quad(0x40));}
  else{
   const bool right=enemy.word(0x80)==1;
   const auto base=to_binary64(right?subtract(q(0x38),c(0x6d2320),bits):add(q(0x38),c(0x6d2320),bits));
   auto angle=multiply(integer(enemy.word(0xc)-40u),c(0x6d22d8),bits);if(!right)angle=add(angle,c(0x6d1860),bits);
   b.quad(0x40,to_binary64(add(multiply(math.cosine(to_binary64(angle)),c(0x6d1e48),bits),binary64(base),bits)));
   angle=multiply(integer(enemy.word(0xc)-40u),c(0x6d22d8),bits);if(!right)angle=add(angle,c(0x6d1860),bits);
   b.quad(0x48,to_binary64(add(multiply(math.sine(to_binary64(angle)),c(0x6d1e48),bits),q(0x40),bits)));
  }
  b.quad(0xa0,constant(0x6d2300));b.quad(0xa8,constant(0x6d1980));b.quad(0xb0,constant(0x6d1948));b.word(0xc,circle?(47u-enemy.word(0xc))*4u-90u:0xffffff92u);
 };
 switch(parameters.difficulty){
 case 0:if(age>=80&&age<=129)ring(160,0x6d1860,0,0);break;
 case 1:if(age>=40&&age<=79)ring(110,0x6d2318,0x6d1920,0);if(age>=80&&age<=129)ring(160,0x6d2310,0x6d2308,0);break;
 case 2:
  if(age>=40&&age<=74){if(age==40)aimed(false);ring(105,0x6d22f8,0x6d18b0,0);}
  if(age>=70&&age<=104)ring(135,0x6d22f0,0x6d22a0,0);
  if(age>=100&&age<=134)ring(165,0x6d22e8,0x6d22e0,0);break;
 case 3:if(age>=40&&age<=134){if(age<=47)aimed(true);const auto end=age>=85?1u:0u;for(std::uint32_t i=0;i<=end;++i)ring(165,0x6d1860,0,1);}break;
 default:break;
 }
 if(enemy.word(0xc)==enemy.word(0x84)){enemy.word(0x80,0u-enemy.word(0x80));enemy.quad(0xa8,to_binary64(integer(enemy.word(0x80)*120u+192u)));enemy.word(0xc,0xffffffffu);}
}
}
