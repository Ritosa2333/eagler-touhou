#pragma once
#include "core/stages/ExBossLateTimelineEngine.hpp"
namespace kaikeidou { namespace ex_boss_late_timeline_detail {
inline void Engine::phase17(){
    if(player.word(0x2c)){protection.boss_state_a7ea60=1;allocation.field_a7ea58=0;}else{protection.boss_state_a7ea60=0;allocation.field_a7ea58=1;}
    if(t.frame==1){enemy().quad(0x38,constant(0x6d2540));enemy().quad(0x40,constant(0x6d2540));enemy().word(8,0);presentation.entry_state=2;}
    if(character==0)phase17_reimu();else if(character==1)phase17_marisa();else phase17_sanae();
}
inline void Engine::phase17_reimu(){
    if(t.frame==200){ex_boss_late_timeline_name(context.spell_name,0x6c5ef0);services.configure_phase(1,62,0,146);enemy().word(8,0);}
    else if(t.frame==300){effect_at(10,{{254,254,254}},constant(0x6d1a60),constant(0x6d1af0));real(0)=constant(0x6d1918);integer(0)=6;}
    if(frame()<340||remainder(t.frame,150)!=40)return;
    effect_at(14,{{254,254,254}},constant(0x6d1a60),constant(0x6d1af0));auto value=random.real_number(constant(0x6d1c60),precision);put(0xa8,m.mul(value,m.c(0x6d1950)));value=random.real_number(constant(0x6d1c60),precision);put(0xb0,m.mul(value,m.c(0x6d1950)));
    for(unsigned i=0;i<16;++i){
        for(unsigned j=0;j<10;++j){const auto slot=allocate_projectile(pool,{14,2,{{254,254,254}},0,255,0,41});if(slot==-1)continue;auto b=bullet(static_cast<std::uint32_t>(slot));b.word(8,177);b.quad(0xa0,m.store(m.add(m.mul(m.i(j),m.c(0x6d1dd8)),q(0xa8))));b.quad(0xa8,enemy().quad(0xb0));b.quad(0xb0,constant(0x6a7708));b.quad(0xb8,0x3ff0000000000000ull);b.quad(0x40,constant(0x6d1a60));b.quad(0x48,constant(0x6d1af0));
            value=math.sine(b.quad(0xa8));b.quad(0x60,m.store(m.add(m.mul(value,m.c(0x6d1960)),m.q(b.quad(0xa0)))));b.word(0xc,0xfffffffeu-i*5u);
        }
        put(0xb0,m.add(q(0xb0),m.q(real(0))));
    }
    real(0)=m.store(m.mul(m.q(real(0)),m.c(0x6aca58)));if(frame()<700)return;
    value=random.real_number(constant(0x6d1c60),precision);put(0xa8,m.mul(value,m.c(0x6d1950)));
    for(std::uint32_t i=0;signed_word(i)<signed_word(integer(0));++i){const auto slot=allocate_projectile(pool,{31,1,{{254,254,254}},0,255,1,41});if(slot==-1)continue;auto b=bullet(static_cast<std::uint32_t>(slot));
        const auto angle=m.store(m.add(m.mul(m.div(m.c(0x6d2208),m.i(integer(0))),m.i(i)),q(0xa8)));b.word(8,178);value=random.real_number(constant(0x6d1c60),precision);b.quad(0xa0,m.store(m.mul(value,m.c(0x6d1950))));
        const auto positive=random.integer_bits(1,precision,true)!=0;value=random.real_number(constant(0x6b25b8),precision);value=m.add(m.mul(value,m.c(0x6a7888)),m.c(0x6d1cf8));if(!positive)value.negative=!value.negative;b.quad(0xa8,m.store(value));b.quad(0x40,constant(0x6d1a60));b.quad(0x48,constant(0x6d1af0));
        value=math.cosine(angle);b.quad(0x50,m.store(m.mul(value,m.c(0x6d1920))));value=math.sine(angle);b.quad(0x58,m.store(m.mul(value,m.c(0x6d1920))));value=random.real_number(constant(0x6d1c60),precision);b.quad(0x60,m.store(m.mul(value,m.c(0x6d1950))));b.word(0x3c,1);b.word(0x34,1);
    }
    integer(0)+=2;
}
}}
