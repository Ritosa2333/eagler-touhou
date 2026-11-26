#pragma once
#include "core/stages/ExBossEarlyTimelineEngine.hpp"
namespace kaikeidou { namespace ex_boss_early_timeline_detail {
inline void Engine::phase4_sanae(){
    if(t.frame==1)target(0x6d1d00,0x6b25f8);
    else if(t.frame==160){ex_boss_early_timeline_name(context.spell_name,0x6c5ce8);services.configure_phase(1,60,extra.boss_hp_multiplier*20000u,130);}
    else if(t.frame==260){real(0)=0;real(1)=constant(0x6d2f78);real(2)=m.store(m.mul(random.real_number(constant(0x6d1f70),precision),m.c(0x6d1960)));real(3)=0;real(4)=m.store(m.add(m.mul(random.real_number(constant(0x6d1a90),precision),m.c(0x6d1960)),m.c(0x6d2a88)));costume(3);}
    if(frame()<290)return;
    integer(0)=static_cast<std::uint32_t>(remainder(t.frame,270));
    if(integer(0)==20||integer(0)==155){
        const bool second=integer(0)==155;
        if(compare(m.c(0x6d19d0),m.q(real(1)))>0)real(1)=m.store(m.add(m.q(real(1)),m.c(0x6d2f70)));
        real(2)=m.store(m.mul(random.real_number(constant(0x6d1af8),precision),m.c(0x6d1960)));
        const auto shift=random.real_number(constant(0x6b25b8),precision);real(3)=m.store(m.add(m.mul(shift,m.c(0x6a7708)),m.q(real(3))));
        real(4)=m.store(m.add(m.mul(random.real_number(constant(0x6d1a90),precision),m.c(0x6d1960)),m.c(0x6d2a88)));
        const auto y=m.store(m.add(m.q(real(1)),m.q(real(2))));effect_at(10,second?std::array<std::uint32_t,3>{{0,255,127}}:std::array<std::uint32_t,3>{{255,127,0}},constant(second?0x6d2288:0x6b25f8),y);
    }else if((signed_word(integer(0))>=60&&signed_word(integer(0))<=79)||(signed_word(integer(0))>=195&&signed_word(integer(0))<=214)){
        const bool second=signed_word(integer(0))>=195;
        const auto slot=allocate_projectile(pool,{1,0,second?std::array<std::uint32_t,3>{{0,255,127}}:std::array<std::uint32_t,3>{{255,127,0}},0,255,0,41});
        if(slot!=-1){auto b=bullet(static_cast<std::uint32_t>(slot));b.word(8,134);
            const auto offset=m.i((integer(0)-(second?195u:60u))*20u);b.quad(0x40,m.store(second?m.sub(m.sub(m.c(0x6d1cb0),m.q(real(4))),offset):m.add(m.add(m.n(0.),m.q(real(4))),offset)));
            const auto base=m.store(m.add(m.q(real(1)),m.q(real(2))));auto value=math.sine(real(3));b.quad(0x48,m.store(m.add(m.mul(value,m.c(0x6d1c18)),m.q(base))));b.quad(0x50,0);
            value=math.sine(real(3));b.quad(0x58,m.store(m.add(m.add(value,value),m.c(0x6d1938))));b.word(0x78,50);b.word(0x7c,49);
        }
        real(3)=m.store(m.add(m.q(real(3)),m.c(0x6d19c0)));
    }else if(integer(0)==19){auto value=random.real_number(constant(0x6b25b8),precision);put(0xa8,m.add(value,m.c(0x6d2108)));value=random.real_number(constant(0x6d1a78),precision);put(0xb0,m.add(m.mul(value,m.c(0x6d1960)),m.c(0x6b25b8)));}
}
}}
