// 原敌114/116/117/138；116/117依次清HP、掉落计数，再调用完整死亡服务。
#pragma once
#include "core/stages/Stage6BossFinalArithmetic.hpp"
#include "core/stages/Stage6BossFinalEmission.hpp"
#include "core/entities/EnemyDamage.hpp"
namespace kaikeidou {
inline void update_stage6_boss_final_enemy(std::uint32_t behavior,Enemy& e,std::uint32_t index,ProjectilePools& pool,const ObjectRecord<240>& player,PatternParameters& p,Random& random,RandomPrecision precision,GameMath& math,EnemyDamageServices& services){
 using namespace stage6_boss_final_detail;const Arithmetic m{{static_cast<unsigned>(precision)}};const auto q=[&](std::size_t o){return m.q(e.quad(o));};const auto put=[&](std::size_t o,binary_float::Number v){e.quad(o,m.store(v));};const auto real=[](std::uint64_t v){return crt_sse2::real(v);};
 if(behavior==114){put(0x38,m.sub(q(0x38),m.c(0x6d1948)));if(!remainder(e.word(12),e.word(0x80))){const auto slot=allocate_projectile(pool,{0,1,{{0,64,255}},0,255,1,41});if(slot!=-1){auto& b=pool.bullets[slot];const auto x=m.store(m.add(q(0x38),m.c(0x6d1e20)));const auto rx=random.real_number(constant(0x6d1e48),precision);b.quad(0x40,m.store(m.sub(m.q(x),rx)));const auto y=m.store(m.add(q(0x40),m.c(0x6d1e20)));const auto ry=random.real_number(constant(0x6d1e48),precision);b.quad(0x48,m.store(m.sub(m.q(y),ry)));b.quad(0xa0,e.quad(0xa8));b.quad(0xa8,e.quad(0xb0));b.quad(0xb0,e.quad(0xb8));b.word(8,100);}}if(real(e.quad(0x38))<real(constant(0x6d1b58)))e.word(0,0);return;}
 if(behavior==116||behavior==117){const auto cosine=math.cosine(e.quad(0xa8));put(0x38,m.add(m.mul(cosine,q(0xb0)),m.c(0x6d1a60)));const auto sine=math.sine(e.quad(0xa8));put(0x40,m.add(m.mul(sine,q(0xb0)),m.c(0x6d18c0)));const auto denominator=e.word(0x80);if(!denominator)e.quad(0xa8,behavior==116?0x7ff0000000000000ull:0xfff0000000000000ull);else{const auto rate=m.div(m.c(0x6d2208),m.i(denominator));put(0xa8,behavior==116?m.add(rate,q(0xa8)):m.sub(q(0xa8),rate));}if(e.word(12)==e.word(0x80)){e.word(0x14,0);e.word(0x1c,0);services.check_enemy_death(index);}return;}
 if(behavior==138){put(0x40,m.add(q(0x40),q(0xa8)));if(remainder(e.word(12),80)==10)stage6_boss_final_emit(pool,{{2,0,{{255,0,255}},2,255,1,41},e.quad(0x38),e.quad(0x40),1,0,constant(0x6d25f0),0,0,0},player,p,random,precision,math);if(real(e.quad(0x40))>real(constant(0x6d1c00)))e.word(0,0);return;}
 throw std::out_of_range("Stage6 final enemy behavior");
}
}
