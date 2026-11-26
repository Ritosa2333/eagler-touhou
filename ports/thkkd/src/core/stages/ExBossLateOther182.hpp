#pragma once
#include "core/stages/ExBossLateOtherEngine.hpp"
namespace kaikeidou { namespace ex_boss_late_other_detail {
inline void Engine::normal182(){auto value=math.cosine(b().quad(0xa0));put(0x40,m.add(m.mul(value,q(0xa8)),m.q(real(0))));value=math.sine(b().quad(0xa0));put(0x48,m.add(m.mul(value,q(0xa8)),m.q(real(1))));
 if(signed_word(enemy_word(phase.boss_index,0x14))>18000){put(0xa0,m.add(q(0xa0),m.c(0x6d21c8)));if(remainder(p.integers[2]+b().word(0x80),120)>39||remainder(p.integers[2],2)!=0)return;}
 else if(signed_word(enemy_word(phase.boss_index,0x14))>9000){put(0xa0,m.add(q(0xa0),m.c(0x6d2828)));if(remainder(p.integers[2]+b().word(0x80),45)>9||remainder(p.integers[2],2)!=0)return;}
 else{put(0xa0,m.add(q(0xa0),m.c(0x6d1b48)));if(remainder(p.integers[2],80)>44||remainder(p.integers[2],2)!=0)return;}
 const auto slot=allocate_projectile(pool,{9,0,{{255,0,0}},2,255,1,41});if(slot==-1)return;auto c=at(static_cast<std::uint32_t>(slot));c.word(0x78,index);c.word(0x7c,b().word(0x78));interpolate(pool,c,p.integers[0],m);c.word(8,184);
}
}}
