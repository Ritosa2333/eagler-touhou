#pragma once
#include "core/stages/ExBossLateTimelineEngine.hpp"
namespace kaikeidou { namespace ex_boss_late_timeline_detail {
inline void Engine::phase17_sanae(){
    constexpr std::array<std::array<std::uint32_t,3>,6> colors{{{{0,0,255}},{{0,255,255}},{{0,255,0}},{{255,255,0}},{{255,0,0}},{{255,0,255}}}};
    const auto positive=[&](unsigned i){return compare(m.i(0),m.q(real(i)))<0;};
    const auto advance_color=[&](unsigned i){++integer(i);if(integer(i)==6)integer(i)=0;};
    if(t.frame==200){ex_boss_late_timeline_name(context.spell_name,0x6c5f18);services.configure_phase(1,62,0,148);enemy().word(8,0);integer(1)=random.integer_bits(5,precision,true);}
    else if(t.frame==300){
        effect_at(10,{{254,254,254}},constant(0x6d1a40),constant(0x6d1a40));effect_at(10,{{254,254,254}},constant(0x6d2160),constant(0x6d1a40));integer(0)=random.integer_bits(5,precision,true);real(0)=constant(random.integer_bits(1,precision,true)?0x6d1b90:0x6d2dd8);
        const auto to_right=positive(0);const auto value=random.real_number(constant(0x6d2dd0),precision);real(1)=m.store(to_right?m.sub(m.c(0x6d1bf8),m.mul(value,m.c(0x6d1960))):m.add(m.mul(value,m.c(0x6d1960)),m.c(0x6d1a68)));
        integer(2)=0;real(2)=constant(positive(0)?0x6d1b58:0x6b25f8);real(3)=m.store(m.i(positive(0)?464u:0xffffffb0u));real(4)=constant(0x6d1f08);integer(3)=2;integer(4)=integer(0);
    }
    if(frame()<340)return;
    if(remainder(t.frame,integer(3))==0){
        const auto color=colors.at(integer(0));const auto angle=constant(positive(0)?0x6d2dc8:0x6d2dc0);
        ex_boss_late_timeline_emit(pool,{{14,2,color,0,255,1,41},real(1),constant(0x6d1a30),1,0,constant(0x6d2180),angle,1,0},player,p,random,precision,math);
        real(1)=m.store(m.add(m.q(real(1)),m.q(real(0))));
        if(positive(0)){
            if(compare(m.add(m.c(0x6d1cb0),m.q(real(4))),m.q(real(1)))<0){real(0)=m.store(m.mul(m.q(real(0)),m.c(0x6aca58)));const auto value=random.real_number(constant(0x6d2db8),precision);real(1)=m.store(m.add(m.mul(value,m.c(0x6d1960)),m.c(0x6d1a68)));advance_color(0);}
        }else if(compare(m.sub(m.i(0),m.q(real(4))),m.q(real(1)))>0){real(0)=m.store(m.mul(m.q(real(0)),m.c(0x6aca58)));const auto value=random.real_number(constant(0x6d2db8),precision);real(1)=m.store(m.sub(m.c(0x6d1bf8),m.mul(value,m.c(0x6d1960))));advance_color(0);}
    }
    const auto background_color=[&](bool dim){advance_color(1);if(integer(1)<6)for(unsigned i=0;i<3;++i)background.reals[7u+i]=colors[integer(1)][i]?0x3ff0000000000000ull:0;if(dim)for(unsigned i=0;i<3;++i)background.reals[7u+i]=m.store(m.mul(m.q(background.reals[7u+i]),m.c(0x6d1920)));};
    if(frame()>=2430){if(t.frame==3100){integer(3)=1;real(4)=constant(0x6d2db0);}if(remainder(t.frame,100)==0)background_color(false);return;}
    if(remainder(t.frame,100)==0)background_color(true);
    if(frame()<900||remainder(t.frame,13)!=0)return;
    const auto color=colors.at(integer(4));
    for(unsigned i=0;i<3;++i){const auto slot=allocate_projectile(pool,{31,1,color,0,255,0,41});if(slot==-1)continue;auto b=bullet(static_cast<std::uint32_t>(slot));b.word(8,181);b.quad(0xa0,0);
        const auto sign=random.integer_bits(1,precision,true)!=0;auto value=random.real_number(constant(0x6b25b8),precision);value=m.add(m.mul(value,m.c(0x6a7888)),m.c(0x6d1cf8));if(!sign)value.negative=!value.negative;b.quad(0xa8,m.store(value));
        const auto xbase=m.store(m.sub(m.q(real(3)),m.c(0x6d1898)));value=random.real_number(constant(0x6d24b8),precision);b.quad(0x40,m.store(m.add(m.mul(value,m.c(0x6a7708)),m.q(xbase))));b.quad(0x48,m.store(m.i(i*40u+518u)));b.quad(0x50,0);b.quad(0x58,constant(0x6d1e80));
        value=random.real_number(constant(0x6d1c60),precision);b.quad(0x60,m.store(m.mul(value,m.c(0x6d1950))));b.word(0x3c,1);b.word(0x34,1);
    }
    real(3)=m.store(m.add(m.q(real(3)),m.q(real(2))));
    if(positive(2)){if(compare(m.c(0x6d1a68),m.q(real(3)))<0){real(2)=m.store(m.mul(m.q(real(2)),m.c(0x6aca58)));advance_color(4);}}
    else if(compare(m.c(0x6d1bf8),m.q(real(3)))>0){real(2)=m.store(m.mul(m.q(real(2)),m.c(0x6aca58)));advance_color(4);}
}
}}
