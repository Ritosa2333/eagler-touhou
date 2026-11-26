// 原敌77/78：先缓动两个坐标，达到signed余数末帧时依序消耗随机并更新目标。
#pragma once
#include "core/stages/Stage4BossFinalArithmetic.hpp"
#include "core/entities/EnemyPool.hpp"
namespace kaikeidou {
inline void update_stage4_boss_enemy_final(std::uint32_t behavior,Enemy& e,ProjectilePools&,std::uint64_t px,std::uint64_t,
    PatternParameters&,Random& random,RandomPrecision precision,GameMath&){
    using namespace stage4_boss_final_detail;const Arithmetic m{{static_cast<unsigned>(precision)}};
    if(behavior!=77&&behavior!=78)throw std::out_of_range("Stage4 final enemy outside77/78");
    e.quad(0x38,stage4_boss_final_approach(e.quad(0x38),e.quad(0xa8),constant(behavior==77?0x6d1ae8:0x6d1aa8),m.precision));
    e.quad(0x40,stage4_boss_final_approach(e.quad(0x40),e.quad(0xb0),constant(0x6d1a40),m.precision));
    if(behavior==77){
        if(remainder(e.word(12),200)==199){
            const auto x=random.real_number(constant(0x6d1aa8),precision);
            e.quad(0xa8,m.store(crt_sse2::real(px)>=192.?m.add(x,m.c(0x6d1df8)):m.sub(m.c(0x6d1df0),x)));
            e.quad(0xb0,m.store(m.add(random.real_number(constant(0x6b25f8),precision),m.c(0x6d1aa8))));
        }
    }else if(remainder(e.word(12),282)==281){
        e.quad(0xa8,m.store(m.add(random.real_number(constant(0x6d18c0),precision),m.c(0x6d1de8))));
        e.quad(0xb0,m.store(m.add(random.real_number(constant(0x6d1ae8),precision),m.c(0x6d1ae8))));
    }
}
}
