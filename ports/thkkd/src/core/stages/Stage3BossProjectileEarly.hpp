// 原弹42..44及配置2发射链；保留发射后的源字段重读和整数除法故障前写入。
#pragma once
#include "core/stages/Stage3BossEarlyArithmetic.hpp"
#include "core/shared/FinalProjectileRecord.hpp"
#include "core/entities/ProjectileEmitter.hpp"
namespace kaikeidou {
inline void update_stage3_boss_projectile_early(ProjectilePools& pools,std::uint32_t index,std::uint32_t behavior,
 std::uint64_t player_x,std::uint64_t player_y,const PatternParameters& parameters,Random& random,RandomPrecision precision,GameMath& math){
 using namespace binary_float;using namespace stage3_boss_early_detail;const auto bits=static_cast<unsigned>(precision);final_projectile_detail::Record b{pools,index};
 const auto c=[](std::uint32_t a){return binary64(constant(a));};const auto q=[&](std::size_t o){return binary64(b.quad(o));};const auto put=[&](std::size_t o,Number v){b.quad(o,to_binary64(v));};
 if(behavior==42){
  put(0x40,add(multiply(q(0x50),q(0xa0),bits),q(0x40),bits));put(0x48,add(multiply(q(0x58),q(0xa0),bits),q(0x48),bits));put(0xa0,add(q(0xa0),q(0xa8),bits));
  if(signed_word(parameters.difficulty)>=2){if(b.word(0xc)==40)put(0xa8,multiply(q(0xa8),c(0x6d1ee8),bits));else if(signed_word(b.word(0xc))>=90)put(0xa8,multiply(q(0xa8),c(0x6d1bd0),bits));}
 }else if(behavior==43){put(0x40,add(q(0x40),q(0x50),bits));put(0x48,add(q(0x48),q(0x58),bits));}
 else if(behavior==44){put(0x48,add(q(0x48),q(0x58),bits));put(0x58,add(q(0x58),q(0xa0),bits));put(0xa0,multiply(q(0xa0),c(0x6d1b08),bits));}
 else throw std::out_of_range("Stage3 early boss projectile outside 42..44");
 if(!remainder(b.word(0xc),parameters.integers[2])){
  ProjectileEmission in{{12,2,{{128,128,128}},0,255,1,41},b.quad(0x40),b.quad(0x48),2,constant(0x6d1860),parameters.reals[0],behavior==44?constant(0x6d1b60):b.quad(0xb0),1,2};
  emit_projectiles(pools,in,player_x,player_y,parameters,random,precision,math);
 }
 const auto x=crt_sse2::real(b.quad(0x40)),y=crt_sse2::real(b.quad(0x48));
 if(behavior==42){if(y>704.||x< -256.||x>640.||y< -256.)b.word(0,0);}
 else if(behavior==43){if(x< -128.||x>512.)b.word(0,0);}
 else if(y>480.)b.word(0,0);
}
}
