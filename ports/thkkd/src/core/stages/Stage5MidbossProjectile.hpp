// 原弹71/72；先更新轨道，半径耗尽后的发射与移除在下一次调用发生。
#pragma once
#include "core/stages/Stage5MidbossDependenciesArithmetic.hpp"
#include "core/stages/StageResourceContext.hpp"
#include "core/entities/ProjectileEmitter.hpp"
namespace kaikeidou {
inline void update_stage5_midboss_projectile(ProjectilePools& pool,std::uint32_t index,std::uint32_t behavior,const ObjectRecord<240>& player,
 PatternParameters& p,const StageResourceContext& background,Random& random,RandomPrecision precision,GameMath& math){
 using namespace stage5_midboss_dependencies_detail;const Arithmetic m{{static_cast<unsigned>(precision)}};final_projectile_detail::Record b{pool,index};const auto q=[&](std::size_t o){return m.q(b.quad(o));};const auto put=[&](std::size_t o,binary_float::Number n){b.quad(o,m.store(n));};
 if(behavior==71){put(0x40,m.add(m.add(m.mul(m.signed_integer(b.word(12)),q(0xa0)),m.q(background.reals[5])),m.mul(m.sub(m.c(0x6d1a60),m.q(background.reals[5])),m.c(0x6a7708))));put(0x48,m.add(m.add(m.mul(m.signed_integer(b.word(12)),q(0xa8)),m.q(background.reals[6])),m.mul(m.sub(m.c(0x6d1af0),m.q(background.reals[6])),m.c(0x6a7708))));const auto x=crt_sse2::real(b.quad(0x40)),y=crt_sse2::real(b.quad(0x48));if(x<crt_sse2::real(constant(0x6d1bf8))||y<crt_sse2::real(constant(0x6d1bf8))||x>crt_sse2::real(constant(0x6d1a68))||y>crt_sse2::real(constant(0x6d1bf0)))b.word(0,0);return;}
 if(behavior!=72)throw std::out_of_range("Stage5 midboss projectile outside71..72");
 if(crt_sse2::real(b.quad(0xb8))>0.){const auto cosine=math.cosine(b.quad(0xb0));put(0x40,m.add(m.mul(cosine,q(0xb8)),q(0xa0)));const auto sine=math.sine(b.quad(0xb0));put(0x48,m.add(m.mul(sine,q(0xb8)),q(0xa8)));put(0xb0,m.add(q(0xb0),q(0xc0)));put(0xb8,m.sub(q(0xb8),m.q(p.reals[0])));if(crt_sse2::real(b.quad(0xb8))<0.)b.quad(0xb8,0);return;}
 if(signed_word(p.difficulty)>=2){if(remainder(b.word(12),900)==signed_word(b.word(0x78)))emit_projectiles(pool,{{12,2,{{b.word(0x7c),b.word(0x80),b.word(0x84)}},0,255,1,41},b.quad(0xa0),b.quad(0xa8),p.integers[0],p.reals[1],constant(0x6d1b28),b.quad(0xc8),1,0},player.quad(0x90),player.quad(0x98),p,random,precision,math);}
 else if(p.difficulty==0&&b.word(12)==800)stage5_midboss_dependencies_retire(pool,index);
}
}
