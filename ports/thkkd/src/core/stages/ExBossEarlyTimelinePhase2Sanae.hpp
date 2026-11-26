#pragma once
#include "core/stages/ExBossEarlyTimelineEngine.hpp"
namespace kaikeidou { namespace ex_boss_early_timeline_detail {
inline void Engine::phase2_sanae(){
    if(t.frame==1)target(0x6d1af0);
    else if(t.frame==150){ex_boss_early_timeline_name(context.spell_name,0x6c5c94);services.configure_phase(1,70,extra.boss_hp_multiplier*7000u,127);}
    else if(t.frame==250){const auto color=services.color(255,255,255);allocate_effect(effects,pool.sound_triggers,{10,color,constant(0x6d1a60),constant(0x6d1af0)});integer(0)=60;real(2)=constant(0x6d1a60);real(3)=constant(0x6d1af0);real(0)=real(2);real(1)=real(3);costume(3);}
    if(frame()<290)return;
    if(t.frame==290)effect(14,{{255,255,255}});
    ex_boss_early_timeline_ease(real(2),player.quad(0x90),constant(0x6d1a40),m.precision);ex_boss_early_timeline_ease(real(3),player.quad(0x98),constant(0x6d1a40),m.precision);
    ex_boss_early_timeline_ease(real(0),real(2),constant(0x6d1ae8),m.precision);ex_boss_early_timeline_ease(real(1),real(3),constant(0x6d1ae8),m.precision);
    const auto emit=[&](bool reflected){
        const auto angle=m.store(m.mul(random.real_number(constant(0x6d1c60),precision),m.c(0x6d1950)));
        const auto direction=m.store(m.mul(random.real_number(constant(0x6d1c60),precision),m.c(0x6d1950)));
        const auto radius=m.store(m.mul(random.real_number(constant(0x6d2710),precision),m.c(0x6d1960)));
        const auto color=random.integer_bits(97,precision,true);integer(1)=random.integer_bits(80,precision,true)+120u;
        std::uint64_t x,y;
        if(reflected){const auto base=m.store(m.add(m.sub(m.c(0x6d1af0),m.q(real(1))),m.c(0x6d1af0)));const auto value=math.sine(angle);y=m.store(m.add(m.mul(value,m.q(radius)),m.q(base)));}
        else{const auto value=math.sine(angle);y=m.store(m.add(m.mul(value,m.q(radius)),m.q(real(1))));}
        if(reflected){const auto base=m.store(m.add(m.sub(m.c(0x6d1a60),m.q(real(0))),m.c(0x6d1a60)));const auto value=math.cosine(angle);x=m.store(m.add(m.mul(value,m.q(radius)),m.q(base)));}
        else{const auto value=math.cosine(angle);x=m.store(m.add(m.mul(value,m.q(radius)),m.q(real(0))));}
        ex_boss_early_timeline_emit(pool,{{0,0,{{color+(reflected?97u:157u),color+97u,color+(reflected?157u:97u)}},0,255,1,41},x,y,1,0,constant(0x6d1948),direction,1,2},player,p,random,precision,math);
    };
    if(remainder(t.frame,4)==0)emit(false);else if(remainder(t.frame,4)==2)emit(true);
    if(remainder(t.frame,300)==269)move_target(0x6b25b8);
}
}}
