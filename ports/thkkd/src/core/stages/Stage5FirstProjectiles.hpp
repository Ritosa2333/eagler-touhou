// 原弹68..70；定帧退场与首帧背景偏移保留原读取顺序。
#pragma once
#include "core/stages/Stage5FirstArithmetic.hpp"
#include "core/entities/ProjectilePool.hpp"
#include "core/stages/StageResourceContext.hpp"
#include "core/math/GameMath.hpp"
namespace kaikeidou {
inline void update_stage5_first_projectile(std::uint32_t behavior,ProjectilePools& pool,std::size_t index,const StageResourceContext& background,
 const std::uint32_t& frame,RandomPrecision precision,GameMath& math){
 using namespace stage5_first_detail;const Arithmetic m{static_cast<unsigned>(precision)};auto& b=pool.bullets.at(index);const auto q=[&](std::size_t o){return m.q(b.quad(o));};const auto put=[&](std::size_t o,binary_float::Number n){b.quad(o,m.store(n));};
 if(behavior==68){if(signed_word(b.word(12))<signed_word(b.word(0x78))){put(0x40,m.add(q(0x40),q(0xa0)));put(0x48,m.add(q(0x48),q(0xa8)));if(b.word(12)==b.word(0x78)-1u){put(0xa0,m.sub(q(0x40),m.q(background.reals[5])));put(0xa8,m.sub(q(0x48),m.q(background.reals[6])));}}else{put(0x40,m.add(m.q(background.reals[5]),q(0xa0)));put(0x48,m.add(m.q(background.reals[6]),q(0xa8)));}if(frame==b.word(0x7c))remove_projectile(pool,index,100);return;}
 if(behavior==69){if(b.word(12)==0){put(0xa0,m.sub(q(0x40),m.q(background.reals[5])));put(0xa8,m.sub(q(0x48),m.q(background.reals[6])));}else{put(0x40,m.add(m.q(background.reals[5]),q(0xa0)));put(0x48,m.add(m.q(background.reals[6]),q(0xa8)));}if(frame==b.word(0x7c))remove_projectile(pool,index,100);return;}
 if(behavior==70){if(signed_word(b.word(12))<0)return;auto sine=math.sine(b.quad(0xa0));put(0x60,m.add(m.div(m.mul(m.mul(sine,m.c(0x6d19c0)),q(0x68)),m.c(0x6d1c18)),q(0xa8)));put(0xa0,m.add(m.div(m.mul(m.c(0x6d1c10),q(0x68)),m.c(0x6d1c18)),q(0xa0)));auto cosine=math.cosine(b.quad(0x60));put(0x40,m.add(m.mul(cosine,q(0x68)),q(0x40)));sine=math.sine(b.quad(0x60));put(0x48,m.add(m.mul(sine,q(0x68)),q(0x48)));const auto x=crt_sse2::real(b.quad(0x40)),y=crt_sse2::real(b.quad(0x48));if(x< -40.||y< -40.||x>424.||y>488.)b.word(0,0);return;}
 throw std::out_of_range("Stage5 first projectile outside 68..70");
}
}
