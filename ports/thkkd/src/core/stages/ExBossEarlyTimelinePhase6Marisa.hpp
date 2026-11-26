#pragma once
#include "core/stages/ExBossEarlyTimelineEngine.hpp"
namespace kaikeidou { namespace ex_boss_early_timeline_detail {
inline void Engine::phase6_marisa(){
    if(t.frame==1){target(0x6d1a90,0x6b25f8);return;}
    if(t.frame==150){ex_boss_early_timeline_name(context.spell_name,0x6c5d30);services.configure_phase(1,60,extra.boss_hp_multiplier*16000u,132);return;}
    if(t.frame==210){effect(12,{{0,0,255}});integer(1)=random.integer_bits(1,precision,true)*2u-1u;real(2)=constant(0x6d2f60);costume(3);return;}
    if(frame()<250)return;
    integer(0)=static_cast<std::uint32_t>(remainder(t.frame,250));real(0)=constant(0x6d1f60);real(1)=constant(0x6d1fe8);real(2)=m.store(m.sub(m.q(real(2)),m.c(0x6d2f58)));
    auto value=math.cosine(real(2));real(3)=m.store(m.add(m.mul(value,m.c(0x6d1858)),q(0x38)));value=math.sine(real(2));real(4)=m.store(m.add(m.mul(value,m.c(0x6d1858)),q(0x40)));
    for(std::uint64_t theta=0;compare(m.c(0x6d2208),m.q(theta))>0;theta=m.store(m.add(m.q(theta),m.c(0x6d2358)))){
        const auto color=services.color(255,255,255);
        const auto y=ex_boss_early_timeline_convert(m.add(ellipse(true,[&](){return theta;},[&](){return real(2);},[&](){return m.q(real(0));},[&](){return m.q(real(1));},[&](){return m.q(real(4));}),m.c(0x6d1e20)));
        const auto x=ex_boss_early_timeline_convert(m.add(ellipse(false,[&](){return theta;},[&](){return real(2);},[&](){return m.q(real(0));},[&](){return m.q(real(1));},[&](){return m.q(real(3));}),m.c(0x6d1e48)));
        services.draw_pixel(x,y,color);
    }
    if(integer(0)==0){
        const auto slot=allocate_projectile(pool,{26,1,{{0,0,255}},2,255,1,41});
        if(slot!=-1){auto b=bullet(static_cast<std::uint32_t>(slot));b.quad(0xa0,constant(0x6d1860));b.quad(0xa8,m.store(m.mul(m.i(integer(1)),m.c(0x6d1e00))));
            b.quad(0x40,m.store(ellipse(false,[&](){return b.quad(0xa0);},[&](){return real(2);},[&](){return m.q(real(0));},[&](){return m.q(real(1));},[&](){return m.q(real(3));})));
            b.quad(0x48,m.store(ellipse(true,[&](){return b.quad(0xa0);},[&](){return real(2);},[&](){return m.q(real(0));},[&](){return m.q(real(1));},[&](){return m.q(real(4));})));
            b.word(8,142);b.word(0x34,1);
        }
        integer(1)=0u-integer(1);
    }
}
}}
