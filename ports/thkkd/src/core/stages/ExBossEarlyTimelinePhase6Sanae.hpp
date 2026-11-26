#pragma once
#include "core/stages/ExBossEarlyTimelineEngine.hpp"
namespace kaikeidou { namespace ex_boss_early_timeline_detail {
inline void Engine::phase6_sanae(){
    if(t.frame==1){target(0x6d2780,0x6b25f8);return;}
    if(t.frame==150){ex_boss_early_timeline_name(context.spell_name,0x6c5d50);services.configure_phase(1,60,extra.boss_hp_multiplier*24000u,133);integer(3)=random.integer_bits(1,precision,true)*2u-1u;return;}
    if(frame()<210)return;
    integer(2)=static_cast<std::uint32_t>(remainder(t.frame,600));
    if(integer(2)==210){effect(12,{{255,255,0}});integer(3)=0u-integer(3);costume(3);}
    else if(signed_word(integer(2))>=250&&signed_word(integer(2))<=400){
        if(remainder(integer(2),10)!=0)return;
        const auto slot=allocate_projectile(pool,{18,1,{{255,255,0}},0,255,1,41});if(slot==-1)return;auto b=bullet(static_cast<std::uint32_t>(slot));b.word(0x78,0);b.word(0x7c,static_cast<std::uint32_t>(signed_word(integer(2)-160u)/2)+40u);
        b.quad(0xa0,m.store(m.add(m.mul(random.real_number(constant(0x6d2f50),precision),m.c(0x6d1960)),m.c(0x6d19f0))));
        b.quad(0xa8,m.store(m.add(m.mul(random.real_number(constant(0x6d2710),precision),m.c(0x6d1960)),m.n(0.))));
        b.quad(0xb0,m.store(m.add(m.mul(random.real_number(constant(0x6d1aa8),precision),m.c(0x6a7708)),m.c(0x6d2f48))));b.quad(0x40,b.quad(0xa0));b.quad(0x48,b.quad(0xa8));
        auto value=math.cosine(b.quad(0xb0));b.quad(0x50,m.store(m.mul(value,m.c(0x6d1f68))));value=math.sine(b.quad(0xb0));b.quad(0x58,m.store(m.mul(value,m.c(0x6d1f68))));b.quad(0x60,b.quad(0xb0));b.word(8,143);b.word(0x34,1);
    }else if(integer(2)==460){effect(12,{{0,255,255}});costume(3);}
    else if(signed_word(integer(2))>=500&&signed_word(integer(2))<=548){
        if(remainder(integer(2),2)!=0)return;
        if(integer(2)==500){real(0)=m.store(m.mul(random.real_number(constant(0x6d1c60),precision),m.c(0x6d1950)));real(1)=m.store(m.mul(random.real_number(constant(0x6d1c60),precision),m.c(0x6d1950)));}
        for(unsigned i=0;i<=4;++i){
            const auto rx=m.store(m.i(i*30u+25u)),ry=m.store(m.i(i*18u+15u));const auto rotation=m.store(m.add(m.mul(m.mul(m.i(i),m.c(0x6d1a18)),m.i(integer(3))),m.q(real(0))));
            const auto slot=allocate_projectile(pool,{9,0,{{0,255,255}},2,255,1,41});if(slot==-1)continue;auto b=bullet(static_cast<std::uint32_t>(slot));
            b.quad(0x40,m.store(ellipse(false,[&](){return real(1);},[&](){return rotation;},[&](){return m.q(rx);},[&](){return m.q(ry);},[&](){return q(0x38);})));
            b.quad(0x48,m.store(ellipse(true,[&](){return real(1);},[&](){return rotation;},[&](){return m.q(rx);},[&](){return m.q(ry);},[&](){return q(0x40);})));
            const auto dx=m.store(m.sub(m.q(b.quad(0x40)),q(0x38))),dy=m.store(m.sub(m.q(b.quad(0x48)),q(0x40)));
            auto value=math.atan2(dy,dx);const auto direction=m.store(m.sub(value,m.mul(m.i(integer(3)),m.c(0x6d18f0))));
            const auto x=m.sub(m.q(b.quad(0x40)),q(0x38)),y=m.sub(m.q(b.quad(0x48)),q(0x40));const auto square=m.store(m.add(m.mul(x,x),m.mul(y,y)));
            value=ex_boss_early_timeline_sqrt(square,m.precision);const auto speed=m.store(m.mul(value,m.c(0x6d1968)));
            value=math.cosine(direction);b.quad(0x50,m.store(m.mul(value,m.add(m.c(0x6d1ff8),m.q(speed)))));value=math.sine(direction);b.quad(0x58,m.store(m.mul(value,m.add(m.c(0x6d1ff8),m.q(speed)))));
            b.word(12,integer(2)-550u);b.word(0x78,0);b.word(0x7c,60);if(i==0)b.word(0x34,1);
            b.word(8,8u+(compare(m.q(b.quad(0x50)),m.n(0.))>=0?2u:0u)+(compare(m.q(b.quad(0x58)),m.n(0.))>=0?1u:0u));
        }
        real(1)=m.store(m.add(m.q(real(1)),m.c(0x6d2f40)));
    }else if(integer(2)==160){auto value=random.real_number(constant(0x6d1888),precision);put(0xa8,m.add(value,m.c(0x6d2c80)));value=random.real_number(constant(0x6d1a78),precision);put(0xb0,m.add(m.mul(value,m.c(0x6d1960)),m.c(0x6d1b38)));}
}
}}
