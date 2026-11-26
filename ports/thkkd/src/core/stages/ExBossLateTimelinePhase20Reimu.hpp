#pragma once
#include "core/stages/ExBossLateTimelineEngine.hpp"
namespace kaikeidou { namespace ex_boss_late_timeline_detail {
inline void Engine::phase20_reimu(){
    if(t.frame==200){ex_boss_late_timeline_name(context.spell_name,0x6c5f2c);services.configure_phase(1,90,extra.boss_hp_multiplier*27000u,149);}
    else if(t.frame==300){effect(12,{{255,0,0}});effect_at(13,{{255,0,0}},player.quad(0x90),player.quad(0x98));}
    if(frame()<340)return;
    if(t.frame==340){
        real(0)=enemy().quad(0x38);real(1)=enemy().quad(0x40);real(2)=player.quad(0x90);real(3)=player.quad(0x98);integer(0)=100;integer(1)=90000;integer(2)=0;effect_at(14,{{255,0,0}},real(0),real(1));effect_at(14,{{255,0,0}},real(2),real(3));
        auto value=random.real_number(constant(0x6d1c60),precision);const auto angle0=m.store(m.mul(value,m.c(0x6d1950)));value=random.real_number(constant(0x6d1c60),precision);const auto angle1=m.store(m.mul(value,m.c(0x6d1950)));
        for(unsigned i=0;i<16;++i){const auto parent=static_cast<std::uint32_t>(allocate_projectile(pool,{1,0,{{255,0,0}},2,255,1,41}));if(parent!=0xffffffffu){auto b=bullet(parent);b.quad(0xa0,m.store(m.add(m.mul(m.i(i),m.c(0x6d27f8)),m.q(angle0))));b.quad(0xa8,constant(0x6d2710));
                value=math.cosine(b.quad(0xa0));b.quad(0x40,m.store(m.add(m.mul(value,m.q(b.quad(0xa8))),m.q(real(2)))));value=math.sine(b.quad(0xa0));b.quad(0x48,m.store(m.add(m.mul(value,m.q(b.quad(0xa8))),m.q(real(3)))));b.word(8,183);b.word(0x34,1);
            }
            const auto slot=allocate_projectile(pool,{1,0,{{255,0,0}},2,255,1,41});if(slot==-1)continue;auto b=bullet(static_cast<std::uint32_t>(slot));b.word(0x78,parent);b.word(0x7c,i);b.word(0x80,(i&1u)*60u);b.quad(0xa0,m.store(m.add(m.mul(m.i(i),m.c(0x6d27f8)),m.q(angle1))));b.quad(0xa8,constant(0x6d1a88));
            value=math.cosine(b.quad(0xa0));b.quad(0x40,m.store(m.add(m.mul(value,m.q(b.quad(0xa8))),m.q(real(0)))));value=math.sine(b.quad(0xa0));b.quad(0x48,m.store(m.add(m.mul(value,m.q(b.quad(0xa8))),m.q(real(1)))));b.word(8,182);b.word(0x34,1);
        }
    }
    ex_boss_late_timeline_ease(real(0),enemy().quad(0x38),constant(0x6b25b8),m.precision);ex_boss_late_timeline_ease(real(1),enemy().quad(0x40),constant(0x6b25b8),m.precision);ex_boss_late_timeline_ease(real(2),enemy().quad(0x38),constant(0x6d1cc0),m.precision);ex_boss_late_timeline_ease(real(3),enemy().quad(0x40),constant(0x6d1cc0),m.precision);
    if(remainder(t.frame,50)==0){ex_boss_late_timeline_emit(pool,{{16,0,{{255,0,0}},0,255,1,41},enemy().quad(0x38),enemy().quad(0x40),16,constant(0x6d27f8),constant(0x6d1948),0,0,0},player,p,random,precision,math);ex_boss_late_timeline_emit(pool,{{16,0,{{255,0,0}},0,255,1,41},enemy().quad(0x38),enemy().quad(0x40),16,constant(0x6d27f8),constant(0x6d1c80),constant(0x6d1ba0),0,0},player,p,random,precision,math);}
    if(remainder(t.frame,400)==339){auto value=random.real_number(constant(0x6d1f38),precision);put(0xa8,m.add(m.mul(value,m.c(0x6d1960)),m.c(0x6d1de8)));value=random.real_number(constant(0x6d1a78),precision);put(0xb0,m.add(m.mul(value,m.c(0x6d1960)),m.c(0x6d1d50)));}
    if(signed_word(enemy().word(0x14))<=9000){if(signed_word(integer(1))>9000){clear_projectiles(pool,0);integer(2)=0;}}
    else if(signed_word(enemy().word(0x14))<=18000&&signed_word(integer(1))>18000){clear_projectiles(pool,0);integer(2)=0;}
    integer(1)=enemy().word(0x14);++integer(2);
}
}}
