// 原敌90..93；93的状态表保留四次趋近、切换字段及成功分配后随机的原顺序。
#pragma once
#include "core/stages/Stage5BossEarlyArithmetic.hpp"
#include "core/stages/Stage5BossEarlyEnemyPhases.hpp"
#include "core/entities/EnemyPool.hpp"
namespace kaikeidou {
inline void update_stage5_boss_early_enemy(std::uint32_t behavior,Enemy& e,ProjectilePools& pool,const ObjectRecord<240>&,
 PatternParameters& p,Random& random,RandomPrecision precision,GameMath& math){
 using namespace stage5_boss_early_detail;const Arithmetic m{{static_cast<unsigned>(precision)}};const auto q=[&](std::size_t o){return m.q(e.quad(o));};const auto put=[&](std::size_t o,binary_float::Number v){e.quad(o,m.store(v));};
 const auto approach=[&](std::size_t o,std::uint64_t target,std::uint64_t divisor){e.quad(o,stage5_boss_early_approach(e.quad(o),target,divisor,m.precision));};
 if(behavior==90){approach(0x38,e.quad(0xa8),e.quad(0xb8));approach(0x40,e.quad(0xb0),e.quad(0xb8));approach(0xb8,constant(0x6d1a40),constant(0x6b25f8));return;}
 if(behavior==91){const auto first=math.cosine(e.quad(0xa8));const auto x_angle=m.store(m.add(m.mul(first,m.c(0x6d1ad0)),m.c(0x6d1de0)));const auto x_cosine=math.cosine(x_angle);put(0x38,m.add(m.mul(x_cosine,m.c(0x6d18c0)),m.c(0x6d1a60)));const auto second=math.cosine(e.quad(0xa8));const auto y_angle=m.store(m.add(m.mul(second,m.c(0x6d1ad0)),m.c(0x6d1de0)));const auto y_sine=math.sine(y_angle);put(0x40,m.add(m.mul(y_sine,m.c(0x6d18c0)),m.c(0x6d1858)));put(0xa8,m.add(q(0xa8),m.c(0x6d1a08)));return;}
 if(behavior==92){const auto cosine=math.cosine(e.quad(0xa8));put(0x38,m.add(m.mul(cosine,m.c(0x6d18c0)),m.c(0x6d1a60)));const auto sine=math.sine(e.quad(0xa8));put(0x40,m.add(m.mul(sine,m.c(0x6d18c0)),m.c(0x6d1af0)));approach(0xa8,e.quad(0xb0),constant(0x6d1ae8));if(!remainder(e.word(12),60))put(0xb0,m.sub(q(0xb0),m.c(0x6d1dd8)));return;}
 if(behavior!=93)throw std::out_of_range("Stage5 early enemy outside90..93");
 const bool low=signed_word(p.difficulty)<=1;const auto state=e.word(0x80);if(state>=(low?4u:8u))return;const auto index=low?state:p.difficulty==2?4u+state:12u+state;const auto& phase=enemy_phases[index];
 approach(0x38,phase.x,e.quad(0xb8));approach(0x40,phase.y,e.quad(0xc0));approach(0xb8,phase.b8_target,phase.b8_divisor);approach(0xc0,phase.c0_target,phase.c0_divisor);
 if(e.word(12)!=phase.event_age)return;e.word(0x80,phase.next_state);e.quad(0xb8,phase.reset_b8);e.quad(0xc0,phase.reset_c0);e.word(12,0);
 if(phase.spawn){const auto child=allocate_projectile(pool,{0,0,{{0,0,0}},0,255,0,41});if(child>=0)initialize_phase_spawn(index,pool.bullets[static_cast<std::size_t>(child)],random,precision,m);}
}
}
