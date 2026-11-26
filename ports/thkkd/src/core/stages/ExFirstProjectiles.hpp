// 原普通弹108：5F27F0..5F2947。
#pragma once
#include "core/stages/ExFirstArithmetic.hpp"
#include "core/stages/ExFirstRecords.hpp"
#include "core/math/GameMath.hpp"
namespace kaikeidou {
inline void update_ex_first_projectile(ProjectilePools& pool,std::uint32_t index,std::uint32_t behavior,GameMath& math,RandomPrecision precision){
 if(behavior!=108)throw std::out_of_range("EX first projectile outside108");using namespace ex_first_detail;const Arithmetic m{static_cast<unsigned>(precision)};PoolRecord b{pool,0xa8e660u+index*240u};
 auto v=math.cosine(b.quad(96));b.quad(64,m.store(m.add(m.mul(v,m.q(b.quad(104))),m.q(b.quad(64)))));v=math.sine(b.quad(96));b.quad(72,m.store(m.add(m.mul(v,m.q(b.quad(104))),m.q(b.quad(72)))));ex_first_ease(b,104,b.quad(160),constant(0x6b25f8),m.precision);
 const auto x=crt_sse2::real(b.quad(64)),y=crt_sse2::real(b.quad(72));if(x<crt_sse2::real(constant(0x6d1a30))||y<crt_sse2::real(constant(0x6d1a30))||x>crt_sse2::real(constant(0x6d1a28))||y>crt_sse2::real(constant(0x6d1a48)))b.word(0,0);
}
}
