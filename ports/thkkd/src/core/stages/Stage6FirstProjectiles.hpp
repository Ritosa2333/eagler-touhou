// 原5F3690..5F386C，行为85两次独立取模和阶段末半径跳变。
#pragma once
#include "core/stages/Stage6FirstArithmetic.hpp"
#include "core/stages/Stage6FirstRecords.hpp"
#include "core/math/GameMath.hpp"
namespace kaikeidou {
inline void update_stage6_first_projectile(ProjectilePools& pool,std::uint32_t index,std::uint32_t behavior,GameMath& math,RandomPrecision precision){
 using namespace stage6_first_detail;if(behavior!=85)throw std::out_of_range("Stage6 first projectile outside85");const Arithmetic m{static_cast<unsigned>(precision)};PoolRecord b{pool,0xa8e660u+index*240u};
 const auto q=[&](std::size_t off){return m.q(b.quad(off));};const auto put=[&](std::size_t off,binary_float::Number v){b.quad(off,m.store(v));};
 if(remainder(b.word(12),b.word(0x78))<=signed_word(b.word(0x7c))){const auto cosine=math.cosine(b.quad(0xb0));put(0x40,m.add(m.mul(cosine,q(0xb8)),q(0xa0)));const auto sine=math.sine(b.quad(0xb0));put(0x48,m.add(m.mul(sine,q(0xb8)),q(0xa8)));put(0xb8,m.add(q(0xb8),q(0xc0)));if(remainder(b.word(12),b.word(0x78))==signed_word(b.word(0x7c)))put(0xb8,m.add(q(0xb8),q(0xc8)));}
 const auto x=crt_sse2::real(b.quad(0x40)),y=crt_sse2::real(b.quad(0x48));if(x< -24.||y< -24.||x>408.||y>472.)b.word(0,0);
}
}
