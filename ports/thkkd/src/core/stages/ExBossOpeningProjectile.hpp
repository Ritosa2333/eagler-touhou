// 普通弹117..120：转动锚点、绕锚点运动、双锚点插值和共享角度。
#pragma once
#include "core/stages/ExBossOpeningProjectileArithmetic.hpp"
#include "core/entities/ProjectilePattern.hpp"
#include "core/math/GameMath.hpp"
namespace kaikeidou {
inline void update_ex_boss_opening_projectile(ProjectilePools& pool,std::uint32_t index,std::uint32_t behavior,const PatternParameters& p,RandomPrecision precision,GameMath& math){
 using namespace ex_boss_opening_projectile_detail;const Arithmetic m{static_cast<unsigned>(precision)};Record b{pool,0xa8e660u+index*240u};const auto q=[&](unsigned off){return m.q(b.quad(off));};const auto put=[&](unsigned off,binary_float::Number n){b.quad(off,m.store(n));};
 if(behavior==117){put(96,m.add(q(96),q(168)));if(signed_word(b.word(12))<=60)ex_boss_opening_projectile_ease(b,72,constant(0x6d1a40),constant(0x6d1ba8),m.precision);else if(signed_word(b.word(12))<=120)ex_boss_opening_projectile_ease(b,64,b.quad(160),constant(0x6d1ba8),m.precision);else{put(72,m.add(q(72),q(88)));if(crt_sse2::real(b.quad(72))>crt_sse2::real(b.quad(232)))b.word(0,0);}return;}
 if(behavior==118){const auto anchor_index=b.word(120);Record anchor{pool,0xa8e660u+anchor_index*240u};if(signed_word(b.word(12))>=50&&!anchor.word(0)){b.word(0,0);return;}put(96,m.add(m.q(anchor.quad(96)),q(160)));auto value=math.cosine(b.quad(96));put(64,m.add(m.mul(value,q(168)),m.q(anchor.quad(64))));value=math.sine(b.quad(96));put(72,m.add(m.mul(value,q(168)),m.q(anchor.quad(72))));return;}
 if(behavior==119){const auto first=b.word(120),second=b.word(124);Record a{pool,0xa8e660u+first*240u},c{pool,0xa8e660u+second*240u};if(signed_word(b.word(12))>=50&&!a.word(0)&&!c.word(0)){b.word(0,0);return;}put(64,m.add(m.mul(m.sub(m.q(c.quad(64)),m.q(a.quad(64))),q(160)),m.q(a.quad(64))));put(72,m.add(m.mul(m.sub(m.q(c.quad(72)),m.q(a.quad(72))),q(160)),m.q(a.quad(72))));return;}
 if(behavior==120){b.quad(96,p.reals[0]);put(72,m.add(q(72),q(88)));if(crt_sse2::real(b.quad(72))>crt_sse2::real(b.quad(232)))b.word(0,0);return;}
 throw std::out_of_range("EX opening projectile outside117..120");
}
}
