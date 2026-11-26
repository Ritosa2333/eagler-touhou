// 原敌93的20个难度/状态块；仅把重复的目标与出生字段提取为数据，写入顺序保持原样。
#pragma once
#include "core/stages/Stage5BossEarlyArithmetic.hpp"
namespace kaikeidou { namespace stage5_boss_early_detail {
struct EnemyPhase {std::uint64_t x,y,b8_target,b8_divisor,c0_target,c0_divisor;std::uint32_t event_age,next_state;std::uint64_t reset_b8,reset_c0;bool spawn;};
inline constexpr std::array<EnemyPhase,20> enemy_phases{{
    {0x403e000000000000ull,0x4061800000000000ull,0x4034000000000000ull,0x403e000000000000ull,0x4034000000000000ull,0x4034000000000000ull,60,1,0x405e000000000000ull,0x4044000000000000ull,true},
    {0x4076200000000000ull,0x403e000000000000ull,0x4034000000000000ull,0x403e000000000000ull,0x4034000000000000ull,0x4034000000000000ull,180,2,0x4044000000000000ull,0x4044000000000000ull,false},
    {0x4076200000000000ull,0x4061800000000000ull,0x4034000000000000ull,0x403e000000000000ull,0x4034000000000000ull,0x4034000000000000ull,60,3,0x405e000000000000ull,0x4044000000000000ull,true},
    {0x403e000000000000ull,0x403e000000000000ull,0x4034000000000000ull,0x403e000000000000ull,0x4034000000000000ull,0x4034000000000000ull,180,0,0x4044000000000000ull,0x4044000000000000ull,false},
    {0x403e000000000000ull,0x4061800000000000ull,0x4034000000000000ull,0x403e000000000000ull,0x4034000000000000ull,0x4034000000000000ull,60,1,0x4034000000000000ull,0x4044000000000000ull,true},
    {0x4076200000000000ull,0x403e000000000000ull,0x4024000000000000ull,0x4034000000000000ull,0x4034000000000000ull,0x4034000000000000ull,40,2,0x4034000000000000ull,0x4044000000000000ull,true},
    {0x403e000000000000ull,0x403e000000000000ull,0x4024000000000000ull,0x4034000000000000ull,0x4034000000000000ull,0x4034000000000000ull,40,3,0x4059000000000000ull,0x4044000000000000ull,true},
    {0x4076200000000000ull,0x403e000000000000ull,0x4034000000000000ull,0x403e000000000000ull,0x4034000000000000ull,0x4034000000000000ull,160,4,0x4044000000000000ull,0x4044000000000000ull,false},
    {0x4076200000000000ull,0x4061800000000000ull,0x4034000000000000ull,0x403e000000000000ull,0x4034000000000000ull,0x4034000000000000ull,60,5,0x4034000000000000ull,0x4044000000000000ull,true},
    {0x403e000000000000ull,0x403e000000000000ull,0x4024000000000000ull,0x4034000000000000ull,0x4034000000000000ull,0x4034000000000000ull,40,6,0x4034000000000000ull,0x4044000000000000ull,true},
    {0x4076200000000000ull,0x403e000000000000ull,0x4024000000000000ull,0x4034000000000000ull,0x4034000000000000ull,0x4034000000000000ull,40,7,0x4059000000000000ull,0x4044000000000000ull,true},
    {0x403e000000000000ull,0x403e000000000000ull,0x4034000000000000ull,0x403e000000000000ull,0x4034000000000000ull,0x4034000000000000ull,160,0,0x4044000000000000ull,0x4044000000000000ull,false},
    {0x403e000000000000ull,0x4061800000000000ull,0x4024000000000000ull,0x403e000000000000ull,0x4024000000000000ull,0x4034000000000000ull,30,1,0x405e000000000000ull,0x4044000000000000ull,true},
    {0x4076200000000000ull,0x403e000000000000ull,0x4024000000000000ull,0x403e000000000000ull,0x4024000000000000ull,0x4034000000000000ull,140,2,0x4044000000000000ull,0x4044000000000000ull,false},
    {0x406e800000000000ull,0x403e000000000000ull,0x4024000000000000ull,0x4034000000000000ull,0x4024000000000000ull,0x403e000000000000ull,30,3,0x4044000000000000ull,0x405e000000000000ull,true},
    {0x4076200000000000ull,0x407a200000000000ull,0x4024000000000000ull,0x4034000000000000ull,0x4024000000000000ull,0x403e000000000000ull,140,4,0x4044000000000000ull,0x4044000000000000ull,false},
    {0x4076200000000000ull,0x4073400000000000ull,0x4024000000000000ull,0x403e000000000000ull,0x4024000000000000ull,0x4034000000000000ull,30,5,0x405e000000000000ull,0x4044000000000000ull,true},
    {0x403e000000000000ull,0x407a200000000000ull,0x4024000000000000ull,0x403e000000000000ull,0x4024000000000000ull,0x4034000000000000ull,140,6,0x4044000000000000ull,0x4044000000000000ull,false},
    {0x4061800000000000ull,0x407a200000000000ull,0x4024000000000000ull,0x4034000000000000ull,0x4024000000000000ull,0x403e000000000000ull,30,7,0x4044000000000000ull,0x405e000000000000ull,true},
    {0x403e000000000000ull,0x403e000000000000ull,0x4024000000000000ull,0x4034000000000000ull,0x4024000000000000ull,0x403e000000000000ull,140,0,0x4044000000000000ull,0x4044000000000000ull,false},
}};
template<class RandomSource> inline void initialize_phase_spawn(unsigned phase,Projectile& b,RandomSource& random,RandomPrecision precision,const Arithmetic& m){switch(phase){
case 0:
    b.quad(0x40,m.store(m.sub(m.q(0xc020000000000000ull),m.mul(random.real_number(0x4074000000000000ull,precision),m.q(0x3fa999999999999aull)))));
    b.quad(0x48,0xc03e000000000000ull);
    b.word(0x8,76u);
    b.quad(0xa0,0x3ffe666666666666ull);
    b.quad(0xa8,0x3fc999999999999aull);
    b.word(0x34,1u);
    break;
case 2:
    b.quad(0x40,m.store(m.add(m.mul(random.real_number(0x4074000000000000ull,precision),m.q(0x3fa999999999999aull)),m.q(0x4078800000000000ull))));
    b.quad(0x48,0xc03e000000000000ull);
    b.word(0x8,76u);
    b.quad(0xa0,0xbffe666666666666ull);
    b.quad(0xa8,0x3fc999999999999aull);
    b.word(0x34,1u);
    break;
case 4:
    b.quad(0x40,m.store(m.sub(m.q(0xc020000000000000ull),m.mul(random.real_number(0x4054000000000000ull,precision),m.q(0x3fa999999999999aull)))));
    b.quad(0x48,0xc03e000000000000ull);
    b.word(0x8,77u);
    b.quad(0xa0,0x4020cccccccccccdull);
    b.quad(0xa8,0x3fc999999999999aull);
    b.word(0x78,3u);
    b.word(0x7c,random.integer_bits(1,precision,true));
    b.word(0x80,3u);
    b.word(0x34,1u);
    break;
case 5:
    b.quad(0x40,m.store(m.add(m.mul(random.real_number(0x4054000000000000ull,precision),m.q(0x3fa999999999999aull)),m.q(0x4078800000000000ull))));
    b.quad(0x48,0xc03e000000000000ull);
    b.word(0x8,77u);
    b.quad(0xa0,0xc020cccccccccccdull);
    b.quad(0xa8,0x3fc999999999999aull);
    b.word(0x78,3u);
    b.word(0x7c,random.integer_bits(1,precision,true));
    b.word(0x80,3u);
    b.word(0x34,1u);
    break;
case 6:
    b.quad(0x40,m.store(m.sub(m.q(0xc020000000000000ull),m.mul(random.real_number(0x4074000000000000ull,precision),m.q(0x3fa999999999999aull)))));
    b.quad(0x48,0xc03e000000000000ull);
    b.word(0x8,77u);
    b.quad(0xa0,0x4002cccccccccccdull);
    b.quad(0xa8,0x3fc999999999999aull);
    b.word(0x78,6u);
    b.word(0x7c,random.integer_bits(1,precision,true));
    b.word(0x80,8u);
    b.word(0x34,1u);
    break;
case 8:
    b.quad(0x40,m.store(m.add(m.mul(random.real_number(0x4054000000000000ull,precision),m.q(0x3fa999999999999aull)),m.q(0x4078800000000000ull))));
    b.quad(0x48,0xc03e000000000000ull);
    b.word(0x8,77u);
    b.quad(0xa0,0xc020cccccccccccdull);
    b.quad(0xa8,0x3fc999999999999aull);
    b.word(0x78,3u);
    b.word(0x7c,random.integer_bits(1,precision,true));
    b.word(0x80,3u);
    b.word(0x34,1u);
    break;
case 9:
    b.quad(0x40,m.store(m.sub(m.q(0xc020000000000000ull),m.mul(random.real_number(0x4054000000000000ull,precision),m.q(0x3fa999999999999aull)))));
    b.quad(0x48,0xc03e000000000000ull);
    b.word(0x8,77u);
    b.quad(0xa0,0x4020cccccccccccdull);
    b.quad(0xa8,0x3fc999999999999aull);
    b.word(0x78,3u);
    b.word(0x7c,random.integer_bits(1,precision,true));
    b.word(0x80,3u);
    b.word(0x34,1u);
    break;
case 10:
    b.quad(0x40,m.store(m.add(m.mul(random.real_number(0x4074000000000000ull,precision),m.q(0x3fa999999999999aull)),m.q(0x4078800000000000ull))));
    b.quad(0x48,0xc03e000000000000ull);
    b.word(0x8,77u);
    b.quad(0xa0,0xc002cccccccccccdull);
    b.quad(0xa8,0x3fc999999999999aull);
    b.word(0x78,6u);
    b.word(0x7c,random.integer_bits(1,precision,true));
    b.word(0x80,8u);
    b.word(0x34,1u);
    break;
case 12:
    b.quad(0x40,m.store(m.sub(m.q(0xc020000000000000ull),m.mul(random.real_number(0x4074000000000000ull,precision),m.q(0x3fa999999999999aull)))));
    b.quad(0x48,0xc034000000000000ull);
    b.word(0x8,78u);
    b.quad(0xa8,0x3fc999999999999aull);
    b.quad(0xb8,0x3ff921fb54442d28ull);
    b.word(0x7c,random.integer_bits(1,precision,true));
    b.word(0x84,0u);
    b.word(0x34,1u);
    break;
case 14:
    b.quad(0x40,0x4079400000000000ull);
    b.quad(0x48,m.store(m.sub(m.q(0xc020000000000000ull),m.mul(random.real_number(0x4074000000000000ull,precision),m.q(0x3fa999999999999aull)))));
    b.word(0x8,78u);
    b.quad(0xa0,0x0000000000000000ull);
    b.quad(0xb0,0x4004000000000000ull);
    b.quad(0xa8,0x3fc999999999999aull);
    b.quad(0xb8,0x400921fb54442d28ull);
    b.word(0x7c,random.integer_bits(1,precision,true));
    b.word(0x84,1u);
    b.word(0x34,1u);
    break;
case 16:
    b.quad(0x40,m.store(m.add(m.mul(random.real_number(0x4074000000000000ull,precision),m.q(0x3fa999999999999aull)),m.q(0x4078800000000000ull))));
    b.quad(0x48,0x407de00000000000ull);
    b.word(0x8,78u);
    b.quad(0xa0,0xbffe666666666666ull);
    b.quad(0xb0,0x0000000000000000ull);
    b.quad(0xa8,0x3fc999999999999aull);
    b.quad(0xb8,0xbff921fb54442d28ull);
    b.word(0x7c,random.integer_bits(1,precision,true));
    b.word(0x84,2u);
    b.word(0x34,1u);
    break;
case 18:
    b.quad(0x40,0xc034000000000000ull);
    b.quad(0x48,m.store(m.add(m.mul(random.real_number(0x4074000000000000ull,precision),m.q(0x3fa999999999999aull)),m.q(0x407c800000000000ull))));
    b.word(0x8,78u);
    b.quad(0xa0,0x0000000000000000ull);
    b.quad(0xb0,0xc004000000000000ull);
    b.quad(0xa8,0x3fc999999999999aull);
    b.quad(0xb8,0x0000000000000000ull);
    b.word(0x7c,random.integer_bits(1,precision,true));
    b.word(0x84,3u);
    b.word(0x34,1u);
    break;
default:throw std::out_of_range("Stage5 early phase has no spawn");}}
}}
