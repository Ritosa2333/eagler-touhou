// 原弹52；清state后仍更新碰撞组标志。
#pragma once
#include "core/stages/Stage4MidbossArithmetic.hpp"
#include "core/shared/FinalProjectileRecord.hpp"
namespace kaikeidou {
inline void update_stage4_midboss_projectile(ProjectilePools& pool,std::uint32_t index,PatternParameters& p,RandomPrecision precision){
 using namespace stage4_midboss_detail;const Arithmetic m{{static_cast<unsigned>(precision)}};final_projectile_detail::Record b{pool,index};
 b.quad(0x40,m.store(m.add(m.q(b.quad(0x40)),m.q(b.quad(0x50)))));b.quad(0x48,m.store(m.add(m.q(b.quad(0x48)),m.q(b.quad(0x58)))));
 const auto y=crt_sse2::real(b.quad(0x48));if(y>464.||y< -16.)b.word(0,0);b.word(0x3c,p.integers[3]!=b.word(0x78));
}
}
