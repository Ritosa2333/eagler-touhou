#pragma once
#include "core/stages/ExBossLateTimelineEngine.hpp"
namespace kaikeidou { namespace ex_boss_late_timeline_detail {
inline void Engine::phase16_sanae_moves(){
    struct Event{std::uint32_t frame,slot,mode;bool right;unsigned extra;bool advance;};
    constexpr std::array<Event,11> events{{{3300,2000,3,false,0,false},{3500,2001,3,true,0,false},{3600,2005,4,true,0,true},{3700,2002,3,false,1,false},{3800,2003,4,false,0,false},{3900,2000,3,true,1,false},{4000,2004,4,true,0,true},{4100,2001,3,false,2,false},{4200,2005,4,false,0,false},{4300,2002,3,true,3,false},{4400,2003,4,true,0,true}}};
    for(const auto& e:events)if(t.frame==e.frame){
        real(2)=constant(e.right?0x6d1a78:0x6d1a80);real(3)=constant(e.mode==4?0x6d1af0:e.right?0x6d1e30:0x6b25b8);integer(2)=83;auto b=bullet(e.slot);b.word(0x78,e.mode);
        if(e.mode==3){b.word(0x80,e.right?1u:0xffffffffu);b.word(0x84,e.right?0u-integer(1):integer(1));b.word(0x8c,integer(2));if(e.extra==2)b.word(0x90,random.integer_bits(1,precision,true));}
        else{b.word(0x8c,integer(2));b.word(0x90,e.right?0u-integer(1):integer(1));b.word(0x94,random.integer_bits(7,precision,true));}
        b.word(0x9c,ex_boss_late_timeline_convert(m.q(real(3))));b.word(0x98,0);
        if(e.extra==1){const auto value=random.real_number(constant(0x6d1c60),precision);b.quad(0xa8,m.store(m.mul(value,m.c(0x6d1950))));}
        else if(e.extra==2){b.quad(0xa0,m.store(m.i(random.integer_bits(6,precision,true)*5u-15u)));b.quad(0xa8,0);}
        else if(e.extra==3)b.quad(0xa0,constant(0x6d1ae8));
        b.quad(0x40,real(2));b.quad(0x48,real(3));b.word(0xc,0);if(e.advance)++integer(1);return;
    }
}
}}
