#pragma once
#include "core/stages/ExBossLateTimelineEngine.hpp"
namespace kaikeidou { namespace ex_boss_late_timeline_detail {
inline void Engine::phase18(){
    if(t.frame==60||t.frame==70){
        const auto base=m.store(m.mul(random.real_number(constant(0x6d1c60),precision),m.c(0x6d1950)));std::array<std::uint32_t,3> color{};
        if(character==0)color={{254,254,254}};else if(character==1)color={{0,0,0}};else if(character==2){for(unsigned i=0;i<3;++i)color[i]=ex_boss_late_timeline_convert(m.q(background.reals[7u+i]))*255u;}else throw std::out_of_range("Original EX phase18 color locals require character0..2");
        for(unsigned i=0;i<32;++i){const auto slot=allocate_projectile(pool,{31,3,color,0,255,1,41});if(slot==-1)continue;auto b=bullet(static_cast<std::uint32_t>(slot));
            const auto initial=m.store(m.add(m.mul(m.i(i),m.c(0x6d1ba0)),m.q(base)));auto value=random.real_number(constant(0x6d1a88),precision);const auto angle=m.store(m.sub(m.add(m.mul(value,m.c(0x6d1950)),m.q(initial)),m.c(0x6d1960)));b.word(8,178);
            value=random.real_number(constant(0x6d1c60),precision);b.quad(0xa0,m.store(m.mul(value,m.c(0x6d1950))));const auto positive=random.integer_bits(1,precision,true)!=0;value=random.real_number(constant(0x6d1ab0),precision);if(!positive)value.negative=!value.negative;b.quad(0xa8,m.store(m.sub(m.mul(value,m.c(0x6a7888)),m.c(0x6a7708))));
            value=random.real_number(constant(0x6b25b8),precision);const auto speed=m.store(m.add(m.mul(value,m.c(0x6d1cf8)),m.c(0x6d18b0)));
            value=math.cosine(angle);b.quad(0x50,m.store(m.mul(value,m.q(speed))));value=math.sine(angle);b.quad(0x58,m.store(m.mul(value,m.q(speed))));b.quad(0x40,m.store(m.add(m.mul(m.q(b.quad(0x50)),m.c(0x6d1c18)),m.c(0x6d1a60))));b.quad(0x48,m.store(m.add(m.mul(m.q(b.quad(0x58)),m.c(0x6d1c18)),m.c(0x6d1af0))));
            value=random.real_number(constant(0x6d1c60),precision);b.quad(0x60,m.store(m.mul(value,m.c(0x6d1950))));b.word(0x3c,1);b.word(0x34,1);
        }
    }else if(t.frame==80){enemy().quad(0x38,constant(0x6d1a60));enemy().quad(0x40,constant(0x6d1af0));presentation.entry_state=2;}
    else if(t.frame==180){++t.subphase;reset();}
}
}}
