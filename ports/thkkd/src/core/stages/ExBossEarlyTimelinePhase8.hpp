#pragma once
#include "core/stages/ExBossEarlyTimelineEngine.hpp"
namespace kaikeidou { namespace ex_boss_early_timeline_detail {
inline void Engine::phase8(){
    random.real_number(0,precision);
    const auto who=character==0?0u:character==1?1u:2u;
    auto& s=pool.secondary[0];
    if(t.frame==1){enemy().word(8,17);enemy().quad(0xa8,constant(who==2?0x6d1ae8:0x6d1a60));enemy().quad(0xb0,constant(who==1?0x6d1a90:0x6d1aa8));enemy().quad(0xb8,constant(0x6b25f8));enemy().quad(0xc0,constant(0x6b25f8));protection.boss_state_a7ea60=2;}
    else if(t.frame==60){
        if(who==2)real(1)=0;
        s.word(0,1);pool.sound_triggers[18]=1;s.quad(0x10,constant(who==2?0x6d23e0:0x6d1a60));s.quad(0x18,constant(0x6d1af0));s.quad(0x20,constant(0x6d1e38));s.quad(0x28,constant(0x6d1e48));
        s.quad(0x30,0x3ff0000000000000ull);s.quad(0x38,who==0?constant(0x6d18b0):0x3ff0000000000000ull);s.quad(0x40,0x3ff0000000000000ull);s.word(4,who==0?146u:who==1?149u:151u);s.word(0xc,0);s.word(0x48,0);costume(3);
    }
    if(compare(m.add(m.q(s.quad(0x28)),m.c(0x6d1868)),m.q(s.quad(0x20)))>0&&stage5_boss_secondary_contact(pool,player,m.precision)){
        ++t.subphase;reset();s.word(0x48,1);s.quad(0x20,s.quad(0x28));
    }
}
}}
