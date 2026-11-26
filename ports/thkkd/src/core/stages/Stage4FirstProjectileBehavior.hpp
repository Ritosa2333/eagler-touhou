#pragma once
#include "core/stages/Stage4FirstDependenciesArithmetic.hpp"
#include "core/shared/FinalProjectileRecord.hpp"
namespace kaikeidou {
inline void update_stage4_first_projectile(ProjectilePools& pool,std::uint32_t index,std::uint32_t behavior,
 std::uint64_t px,std::uint64_t py,PatternParameters& p,Random&,RandomPrecision precision,GameMath& math){
 if(behavior!=51)throw std::out_of_range("Stage4 first projectile outside 51");using namespace stage4_first_dependencies_detail;const Arithmetic m{{static_cast<unsigned>(precision)}};final_projectile_detail::Record b{pool,index};const auto q=[&](std::size_t o){return m.q(b.quad(o));};const auto put=[&](std::size_t o,binary_float::Number n){b.quad(o,m.store(n));};
 put(0x40,m.add(q(0x40),q(0x50)));put(0x48,m.add(q(0x48),q(0x58)));const auto age=signed_word(b.word(12));
 if(age>=60&&age<=86){if(b.word(12)==60)final_projectile_recolor(pool,index,255,0,0);const auto dx=m.store(m.sub(m.q(px),q(0x40))),dy=m.store(m.sub(m.q(py),q(0x48)));const auto angle=m.store(math.atan2(dy,dx));const auto cosine=math.cosine(angle);put(0x50,m.add(m.mul(cosine,m.q(p.reals[0])),q(0x50)));const auto sine=math.sine(angle);put(0x58,m.add(m.mul(sine,m.c(0x6d1fa8)),q(0x58)));}
 else if(age>=87&&age<=130){put(0x50,m.mul(q(0x50),m.c(0x6d1bd0)));put(0x58,m.mul(q(0x58),m.c(0x6d1bd0)));}
 const auto x=crt_sse2::real(b.quad(0x40)),y=crt_sse2::real(b.quad(0x48));if(y>480.||x< -32.||x>416.||y< -64.)b.word(0,0);
}
}
