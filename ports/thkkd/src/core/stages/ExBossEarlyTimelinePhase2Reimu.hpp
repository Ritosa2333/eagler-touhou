#pragma once
#include "core/stages/ExBossEarlyTimelineEngine.hpp"
namespace kaikeidou { namespace ex_boss_early_timeline_detail {
inline void Engine::phase2_reimu(){
    if(t.frame==1){enemy().word(8,17);enemy().quad(0xa8,constant(0x6d1a60));enemy().quad(0xb0,constant(0x6d1b38));enemy().quad(0xb8,constant(0x6b25f8));enemy().quad(0xc0,constant(0x6b25f8));}
    else if(t.frame==150){ex_boss_early_timeline_name(context.spell_name,0x6c5c54);services.configure_phase(1,57,extra.boss_hp_multiplier*20000u,125);integer(1)=3;}
    if(frame()<250)return;
    integer(0)=static_cast<std::uint32_t>(remainder(t.frame,420));
    if(integer(0)==250){
        if(integer(1)==0){integer(1)=1;integer(2)=255;integer(3)=0;integer(4)=0;}
        else if(integer(1)==1){integer(1)=2;integer(2)=255;integer(3)=255;integer(4)=0;}
        else if(integer(1)==2){integer(1)=3;integer(2)=0;integer(3)=255;integer(4)=0;}
        else{integer(1)=0;integer(2)=0;integer(3)=0;integer(4)=255;}
        effect(12,{{integer(2),integer(3),integer(4)}});
        real(0)=m.store(m.add(m.mul(random.real_number(constant(0x6d1f20),precision),m.c(0x6d1950)),m.c(0x6d2f98)));real(1)=real(0);
        const auto cosine=math.cosine(real(0));put(0xa8,m.add(m.mul(cosine,m.c(0x6d1cc0)),m.c(0x6d1a60)));
        const auto sine=math.sine(real(0));put(0xb0,m.add(m.mul(sine,m.c(0x6d1cc0)),m.c(0x6d1af0)));costume(3);
    }else if(signed_word(integer(0))>=290||signed_word(integer(0))<=50){
        for(unsigned i=0;i<2;++i){
            std::uint64_t x=0,y=0;
            do{
                const auto angle=m.store(m.mul(random.real_number(constant(0x6d1c60),precision),m.c(0x6d1950)));
                const auto radius=m.store(m.mul(random.real_number(constant(0x6d2f90),precision),m.c(0x6d1960)));
                auto value=math.cosine(real(i));const auto xbase=m.store(m.add(m.mul(value,m.c(0x6d2a48)),m.c(0x6d1a60)));value=math.cosine(angle);x=m.store(m.add(m.mul(value,m.q(radius)),m.q(xbase)));
                value=math.sine(real(i));const auto ybase=m.store(m.add(m.mul(value,m.c(0x6d2a48)),m.c(0x6d1af0)));value=math.sine(angle);y=m.store(m.add(m.mul(value,m.q(radius)),m.q(ybase)));
            }while(compare(m.q(x),m.n(0.))<0||compare(m.q(x),m.c(0x6d1cb0))>0||compare(m.q(y),m.n(0.))<0||compare(m.q(y),m.c(0x6d19a0))>0);
            const auto slot=allocate_projectile(pool,{4,0,{{integer(2),integer(3),integer(4)}},0,255,1,41});if(slot==-1)continue;auto b=bullet(static_cast<std::uint32_t>(slot));b.quad(0x40,x);b.quad(0x48,y);
            const auto direction=m.store(m.mul(random.real_number(constant(0x6d1c60),precision),m.c(0x6d1950)));
            b.quad(0x50,m.store(math.cosine(direction)));b.quad(0x58,m.store(math.sine(direction)));b.quad(0xa0,0);b.word(8,128);
        }
        real(0)=m.store(m.add(m.q(real(0)),m.c(0x6d2f88)));real(1)=m.store(m.sub(m.q(real(1)),m.c(0x6d2f88)));
        if(integer(0)==20)effect(12,{{255u-integer(2),255u-integer(3),255u-integer(4)}});
    }else if(integer(0)==60){
        const auto slot=allocate_projectile(pool,{18,0,{{255u-integer(2),255u-integer(3),255u-integer(4)}},2,255,0,41});if(slot==-1)return;
        auto b=bullet(static_cast<std::uint32_t>(slot));b.quad(0x40,enemy().quad(0x38));b.quad(0x48,enemy().quad(0x40));b.word(8,129);b.word(0x34,1);
    }
}
}}
