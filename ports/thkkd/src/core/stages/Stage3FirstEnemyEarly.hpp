// 原63B750、63B490、63AE20、63ABC0；保留分支、写序与发射前随机调用。
#pragma once
#include "core/stages/Stage3EnemyEarlyArithmetic.hpp"
#include "core/entities/EnemyPool.hpp"
#include "core/entities/ProjectileEmitter.hpp"
namespace kaikeidou {
inline void update_stage3_first_enemy_early(std::uint32_t behavior,Enemy& enemy,ProjectilePools& pools,
 std::uint64_t player_x,std::uint64_t player_y,PatternParameters& parameters,Random& random,RandomPrecision precision,GameMath& math){
 using namespace binary_float;using namespace stage3_enemy_early_detail;
 const auto bits=static_cast<unsigned>(precision);
 const auto c=[](std::uint32_t a){return binary64(constant(a));};
 const auto q=[&](std::size_t a){return binary64(enemy.quad(a));};
 const auto put=[&](std::size_t a,Number value){enemy.quad(a,to_binary64(value));};
 const auto pick=[&](std::array<std::uint64_t,6> v){return stage3_enemy_early_pick_real(parameters.difficulty,v);};
 const auto count=[&](std::array<std::uint32_t,6> v){return stage3_enemy_early_pick_integer(parameters.difficulty,v);};
 const auto fire=[&](std::uint32_t kind,std::array<std::uint32_t,3> color,std::uint32_t n,std::uint64_t spacing,std::uint64_t speed,std::uint64_t angle){
  ProjectileEmission in{{kind,0,color,0,255,1,41},enemy.quad(0x38),enemy.quad(0x40),n,spacing,speed,angle,1,0};
  emit_projectiles(pools,in,player_x,player_y,parameters,random,precision,math);
 };
 const auto age=signed_word(enemy.word(0xc));
 if(behavior==43||behavior==44){
  if(age<=17){put(0x40,add(q(0x40),c(0x6d1c20),bits));return;}
  if(age>=22&&age<=42){
   put(0x38,add(add(q(0xa8),q(0xa8),bits),q(0x38),bits));
   put(0xa8,multiply(q(0xa8),c(0x6d18f0),bits));
   if(behavior==44&&remainder(enemy.word(0xc),enemy.word(0x80)))return;
   const auto base=crt_sse2::real(player_y)>=crt_sse2::real(enemy.quad(0x40))?constant(0x6d1b60):constant(0x6d1de0);
   if(behavior==44||age%4==0){
    const auto angle=to_binary64(subtract(binary64(base),multiply(q(0xa8),c(0x6d1920),bits),bits));
    const auto speed=pick({constant(0x6d1a00),constant(0x6d1b28),constant(0x6d1b28),constant(0x6d1b28),0,0});
    const auto n=count(behavior==43?std::array<std::uint32_t,6>{{1,2,2,2,0,0}}:std::array<std::uint32_t,6>{{1,1,3,3,0,0}});
    std::array<std::uint32_t,3> color{{96,96,96}};
    if(behavior==44){color[2]=random.integer_bits(255,precision,true);color[1]=random.integer_bits(255,precision,true);color[0]=random.integer_bits(255,precision,true);}
    fire(12,color,n,constant(0x6d1cf0),speed,angle);
   }else if(age%4==2&&(parameters.difficulty==2||parameters.difficulty==3)){
    const auto angle=to_binary64(subtract(binary64(base),multiply(q(0xa8),c(parameters.difficulty==3?0x6d1920:0x6d1fb8),bits),bits));
    fire(12,{{0,0,0}},parameters.difficulty==3?2u:1u,constant(0x6d1cf0),constant(0x6d1f50),angle);
   }
  }else if(age>=(behavior==43?320:160)){
   put(0x38,add(q(0x38),q(0xb0),bits));put(0x40,subtract(q(0x40),c(0x6d1c80),bits));
   if(crt_sse2::real(enemy.quad(0x40))<crt_sse2::real(constant(0x6d19f0)))enemy.word(0,0);
  }
  return;
 }
 if(behavior==45){
  put(0x38,add(multiply(math.cosine(enemy.quad(0xb8)),c(0x6b25b8),bits),q(0xa8),bits));
  put(0x40,add(multiply(math.sine(enemy.quad(0xb8)),c(0x6b25b8),bits),q(0xb0),bits));
  if(signed_word(enemy.word(4))>=3){
   put(0xa8,add(q(0xa8),q(0xc0),bits));put(0xb8,add(q(0xb8),c(0x6d2530),bits));
   if(age>=290&&age<=334)put(0xc0,subtract(q(0xc0),c(0x6d1a18),bits));
   else if(age>=470&&age<=559)put(0xc0,add(q(0xc0),c(0x6d2b08),bits));
  }else{put(0xa8,add(q(0xa8),c(0x6d1b18),bits));put(0xb8,add(q(0xb8),c(0x6d1928),bits));}
  if(!remainder(enemy.word(0xc),enemy.word(0x80))){
   const auto angle=crt_sse2::real(player_y)>=crt_sse2::real(enemy.quad(0x40))?constant(0x6d2b00):constant(0x6d2af8);
   const auto type=enemy.word(4);
   if(type<=4){
    const auto speed=(type==3||type==4)?pick({constant(0x6d1ad0),constant(0x6d1ff8),constant(0x6d1948),constant(0x6d2660),0,0}):pick({constant(0x6d2488),constant(0x6d1ad0),constant(0x6d1e40),constant(0x6d1c80),0,0});
    const std::array<std::array<std::uint32_t,3>,5> colors{{{{220,0,0}},{{40,40,255}},{{0,180,0}},{{0,240,240}},{{240,150,0}}}};
    const auto kind=type==3?3u:(type==1||type==4)?(parameters.difficulty?1u:0u):0u;
    fire(kind,colors[type],1,0,speed,angle);
   }
  }
  if(crt_sse2::real(enemy.quad(0xa8))>crt_sse2::real(constant(0x6d1ec0)))enemy.word(0,0);
  return;
 }
 if(behavior==46){
  if(age<=29){put(0x40,add(q(0x40),q(0x50),bits));put(0x50,multiply(q(0x50),c(0x6d1a20),bits));}
  else if(age>=300){
   put(0x38,subtract(q(0x38),c(0x6d1868),bits));put(0x40,subtract(q(0x40),c(0x6d1b18),bits));
   if(crt_sse2::real(enemy.quad(0x40))<crt_sse2::real(constant(0x6d1b58)))enemy.word(0,0);
  }else if(age>=signed_word(enemy.word(0x8c))){
   if(!remainder(enemy.word(0xc),enemy.word(0x80))){
    fire(10,{{255,enemy.word(0x84),0}},enemy.word(0x88),enemy.quad(0xb0),enemy.quad(0xb8),enemy.quad(0xa8));
    put(0xa8,add(multiply(q(0xb0),c(0x6d1ad0),bits),q(0xa8),bits));
   }
   if(signed_word(enemy.word(0x84))<255)enemy.word(0x84,enemy.word(0x84)+1u);
  }
  return;
 }
 throw std::out_of_range("Stage3 early enemy behavior outside 43..46");
}
}
