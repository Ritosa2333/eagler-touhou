#pragma once
#include "core/stages/ExBossEarlyTimelineEngine.hpp"
namespace kaikeidou { namespace ex_boss_early_timeline_detail {
inline void Engine::phase4_reimu(){
    if(t.frame==1)target(0x6d1d00,0x6b25f8);
    else if(t.frame==150){ex_boss_early_timeline_name(context.spell_name,0x6c5cb4);services.configure_phase(1,60,extra.boss_hp_multiplier*23000u,128);integer(1)=1;}
    if(frame()<250)return;
    integer(0)=static_cast<std::uint32_t>(remainder(t.frame,600));
    const auto period=integer(0);
    if(period==250||period==350||period==450||period==550){
        auto value=m.mul(random.real_number(constant(0x6d1ab0),precision),m.c(0x6d1960));real(0)=m.store(period==350||period==450?m.sub(m.c(0x6d23e0),value):m.add(value,m.c(0x6d1ae8)));
        value=m.mul(random.real_number(constant(0x6d1ab0),precision),m.c(0x6d1960));real(1)=m.store(period==450||period==550?m.sub(m.c(0x6d1a50),value):m.add(value,m.c(0x6d1ae8)));
        effect_at(10,{{255,0,0}},real(0),real(1));if(period==250)integer(1)=0u-integer(1);
    }else if(period==290||period==390||period==490||period==590){
        const auto start=m.store(m.mul(random.real_number(constant(0x6d1c60),precision),m.c(0x6d1950)));
        for(unsigned i=0;i<32;++i){
            const auto slot=allocate_projectile(pool,{1,period>=490?3:2,{{255,0,0}},0,255,0,41});if(slot==-1)continue;auto b=bullet(static_cast<std::uint32_t>(slot));
            b.word(8,134);b.quad(0x40,real(0));b.quad(0x48,real(1));const auto direction=m.store(m.add(m.mul(m.mul(m.i(i),m.c(0x6d1ba0)),m.i(integer(1))),m.q(start)));
            auto value=math.cosine(direction);b.quad(0x50,m.store(m.mul(value,m.c(0x6d1a40))));value=math.sine(direction);b.quad(0x58,m.store(m.mul(value,m.c(0x6d1a40))));b.word(0x78,32);b.word(0x7c,i);
        }
    }else if(period==50){effect(12,{{255,0,255}});costume(3);}
    else if(period==90){
        // 原横向差也减Boss纵坐标，保留这个坐标混用。
        const auto dx=m.store(m.sub(m.q(player.quad(0x90)),q(0x40)));const auto dy=m.store(m.sub(m.q(player.quad(0x98)),q(0x40)));
        const auto start=m.store(m.sub(math.atan2(dy,dx),m.c(0x6d1860)));
        for(unsigned i=0;i<48;++i){
            const auto slot=allocate_projectile(pool,{2,1,{{255,0,255}},2,255,0,41});if(slot==-1)continue;auto b=bullet(static_cast<std::uint32_t>(slot));
            b.word(8,134);b.quad(0x40,enemy().quad(0x38));b.quad(0x48,enemy().quad(0x40));const auto direction=m.store(m.sub(m.q(start),m.mul(m.mul(m.i(i),m.c(0x6d2f80)),m.i(integer(1)))));
            auto value=math.cosine(direction);b.quad(0x50,m.store(m.mul(value,m.c(0x6d25c0))));value=math.sine(direction);b.quad(0x58,m.store(m.mul(value,m.c(0x6d25c0))));b.word(0x78,96);b.word(0x7c,i*2u);
        }
    }else if(period==150){auto value=random.real_number(constant(0x6d1888),precision);put(0xa8,m.add(value,m.c(0x6d2c80)));value=random.real_number(constant(0x6d1a78),precision);put(0xb0,m.add(m.mul(value,m.c(0x6d1960)),m.c(0x6d1d00)));}
}
}}
