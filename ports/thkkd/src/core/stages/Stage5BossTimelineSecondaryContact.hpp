// 原60DD50..60DE1F；32个副弹圆区域的接触计数，仅对命中区域置flag。
#pragma once
#include "core/entities/ProjectilePool.hpp"
#include "core/math/FiniteSqrt.hpp"
#include "core/math/CrtSse2Trig.hpp"
namespace kaikeidou {
inline std::uint32_t stage5_boss_secondary_contact(ProjectilePools& pools,const ObjectRecord<240>& player,unsigned precision){
 using namespace binary_float;std::uint32_t count=0;
 const auto subtract=[&](std::uint64_t a,std::uint64_t b){auto right=binary64(b);right.negative=!right.negative;return to_binary64(add(binary64(a),right,precision));};
 for(std::size_t i=0;i<32;++i){auto& circle=pools.secondary[i];if(circle.word(0)!=1)continue;
  const auto dx=subtract(player.quad(0x90),circle.quad(0x10));const auto dy=subtract(player.quad(0x98),circle.quad(0x18));
  const auto square=to_binary64(add(multiply(binary64(dx),binary64(dx),precision),multiply(binary64(dy),binary64(dy),precision),precision));
  const auto distance=to_binary64(finite_square_root(square,precision));
  if(crt_sse2::real(circle.quad(0x20))>crt_sse2::real(distance)){circle.word(0x0c,1);++count;}
 }
 return count;
}
}
