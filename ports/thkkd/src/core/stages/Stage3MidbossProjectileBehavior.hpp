// 原弹37..41完整依赖链；子弹分配可别名当前槽，所有源字段在原位置重读。
#pragma once
#include "core/stages/Stage3MidbossDependenciesArithmetic.hpp"
#include "core/shared/FinalProjectileRecord.hpp"
#include "core/math/GameMath.hpp"
#include "core/entities/ProjectilePattern.hpp"
namespace kaikeidou {
inline void update_stage3_midboss_projectile(ProjectilePools& pools,std::uint32_t index,std::uint32_t behavior,
 std::uint64_t player_x,std::uint64_t player_y,const PatternParameters& parameters,Random& random,RandomPrecision precision,GameMath& math){
 using namespace binary_float;using namespace stage3_midboss_dependencies_detail;const auto bits=static_cast<unsigned>(precision);final_projectile_detail::Record b{pools,index};
 const auto c=[](std::uint32_t a){return binary64(constant(a));};const auto q=[&](std::size_t o){return binary64(b.quad(o));};const auto put=[&](std::size_t o,Number v){b.quad(o,to_binary64(v));};
 const auto move=[&](){put(0x40,add(q(0x40),q(0xa0),bits));put(0x48,add(q(0x48),q(0xa8),bits));};
 const auto bounce=[&](std::uint32_t low,std::uint32_t high){const auto x=crt_sse2::real(b.quad(0x40));if(x<crt_sse2::real(constant(low))){put(0xa0,multiply(q(0xa0),c(0x6aca58),bits));b.quad(0x40,constant(low));}else if(x>crt_sse2::real(constant(high))){put(0xa0,multiply(q(0xa0),c(0x6aca58),bits));b.quad(0x40,constant(high));}};
 if(behavior==37||behavior==38){
  move();put(0xa8,add(q(0xa8),c(behavior==37?0x6d1c10:0x6d1e00),bits));bounce(behavior==37?0x6d2400:0x6d1b88,behavior==37?0x6d28b8:0x6d28b0);
  if(b.word(0xc)==80){
   for(std::uint32_t i=0;signed_word(i)<=signed_word(parameters.integers[behavior==37?0:1]);++i){
    const auto slot=allocate_projectile(pools,{behavior==37?1u:0u,0,b.word(0x78)?std::array<std::uint32_t,3>{{255,0,0}}:std::array<std::uint32_t,3>{{0,255,0}},0,255,0,41});
    if(slot==-1)continue;auto& child=pools.bullets[slot];child.word(8,behavior==37?38u:39u);
    std::uint64_t angle;
    if(behavior==37)angle=final_projectile_degrees(to_binary64(random.real_number(constant(0x6d1fc8),precision)),bits);
    else{const auto base=to_binary64(subtract(math.atan2(b.quad(0xa8),b.quad(0xa0)),c(0x6d18e0),bits));angle=to_binary64(add(multiply(random.real_number(constant(0x6d1a90),precision),c(0x6a7708),bits),binary64(base),bits));}
    const auto speed=to_binary64(multiply(random.real_number(constant(0x6d1ab0),precision),c(0x6a7708),bits));
    child.quad(0xa0,to_binary64(multiply(math.cosine(angle),binary64(speed),bits)));
    child.quad(0xa8,to_binary64(subtract(multiply(math.sine(angle),binary64(speed),bits),c(behavior==37?0x6d1868:0x6d1948),bits)));
    if(behavior==38)child.quad(0xb0,constant(0x6d1e00));child.quad(0x40,b.quad(0x40));child.quad(0x48,b.quad(0x48));if(behavior==37)child.word(0x78,b.word(0x78));
   }
   b.word(0,0);
  }
  return;
 }
 if(behavior==39){move();put(0xa8,add(q(0xa8),q(0xb0),bits));put(0xb0,multiply(q(0xb0),c(0x6d1c78),bits));bounce(0x6d18e8,0x6d1b80);if(crt_sse2::real(b.quad(0x48))>crt_sse2::real(constant(0x6d19f8)))b.word(0,0);return;}
 const auto age=signed_word(b.word(0xc));
 if(behavior==40){
  if(age<=9){move();return;}
  if(age<=29){put(0x40,add(multiply(math.cosine(b.quad(0xc0)),c(0x6d1878),bits),q(0xb0),bits));put(0x48,add(multiply(math.sine(b.quad(0xc0)),c(0x6d1878),bits),q(0xb8),bits));put(0xc0,add(q(0xc0),c(0x6d1fd0),bits));return;}
  if(age<=signed_word(b.word(0x78))){
   if(age==30){
    final_projectile_recolor(pools,index,0,255,255);b.word(0x2c,2);b.word(4,9);
    const auto angle=final_projectile_degrees(to_binary64(random.real_number(constant(0x6d1fc8),precision)),bits);
    const auto distance=to_binary64(multiply(random.real_number(constant(0x6d1930),precision),c(0x6d1960),bits));
    put(0x40,add(multiply(math.cosine(angle),binary64(distance),bits),q(0xb0),bits));put(0x48,add(multiply(math.sine(angle),binary64(distance),bits),q(0xb8),bits));
   }else if(b.word(0xc)==b.word(0x78)){
    const bool low=signed_word(parameters.difficulty)<=2;
    const auto offset=to_binary64(add(multiply(random.real_number(constant(0x6d1888),precision),c(0x6a7708),bits),c(0x6d1fc0),bits));
    const auto dx=to_binary64(subtract(binary64(player_x),q(0x40),bits)),dy=to_binary64(subtract(binary64(player_y),q(0x48),bits));
    put(0xc8,add(math.atan2(dy,dx),binary64(offset),bits));
    const auto base=to_binary64(add(multiply(random.real_number(constant(low?0x6b25b8:0x6d1fb0),precision),c(0x6a7708),bits),c(0x6d1a00),bits));
    put(0xd0,multiply(subtract(binary64(base),multiply(binary64(final_projectile_absolute(offset)),c(0x6d1fb8),bits),bits),c(low?0x6d1f50:0x6d19d8),bits));
   }
   const auto angle=final_projectile_degrees(to_binary64(random.real_number(constant(0x6d1fc8),precision)),bits);
   put(0x40,add(multiply(math.cosine(angle),c(0x6d1ad0),bits),q(0x40),bits));put(0x48,add(multiply(math.sine(angle),c(0x6d1ad0),bits),q(0x48),bits));return;
  }
  put(0x40,add(multiply(math.cosine(b.quad(0xc8)),q(0xd0),bits),q(0x40),bits));put(0x48,add(multiply(math.sine(b.quad(0xc8)),q(0xd0),bits),q(0x48),bits));
  const auto x=crt_sse2::real(b.quad(0x40)),y=crt_sse2::real(b.quad(0x48));if(x< -16.||y< -16.||x>400.||y>464.)b.word(0,0);return;
 }
 if(behavior==41){
  if(age<0)return;
  if(age==0){const auto dx=to_binary64(subtract(binary64(player_x),q(0x40),bits)),dy=to_binary64(subtract(binary64(player_y),q(0x48),bits));const auto angle=to_binary64(math.atan2(dy,dx));put(0x50,multiply(math.cosine(angle),q(0xb0),bits));put(0x58,multiply(math.sine(angle),q(0xb0),bits));pools.sound_triggers[43]=1;}
  put(0x40,add(multiply(add(integer(1),q(0xa0),bits),q(0x50),bits),q(0x40),bits));put(0x48,add(multiply(add(integer(1),q(0xa0),bits),q(0x58),bits),q(0x48),bits));put(0xa0,multiply(q(0xa0),q(0xa8),bits));
  const auto x=crt_sse2::real(b.quad(0x40)),y=crt_sse2::real(b.quad(0x48));if(y>480.||x< -32.||x>416.||y< -32.)b.word(0,0);return;
 }
 throw std::out_of_range("Stage3 midboss projectile outside 37..41");
}
}
