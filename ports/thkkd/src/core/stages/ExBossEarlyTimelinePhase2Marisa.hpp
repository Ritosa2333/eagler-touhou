#pragma once
#include "core/stages/ExBossEarlyTimelineEngine.hpp"
namespace kaikeidou { namespace ex_boss_early_timeline_detail {
inline void Engine::phase2_marisa(){
    if(t.frame==1){enemy().word(8,17);enemy().quad(0xa8,constant(0x6d1a60));enemy().quad(0xb0,constant(0x6d1cc0));enemy().quad(0xb8,constant(0x6b25f8));enemy().quad(0xc0,constant(0x6b25f8));}
    else if(t.frame==150){
        ex_boss_early_timeline_name(context.spell_name,0x6c5c78);services.configure_phase(1,61,27000,126);
        real(0)=constant(0x6d1a60);real(1)=constant(0x6d1cc0);real(2)=m.store(m.add(m.mul(random.real_number(constant(0x6d1c60),precision),m.c(0x6d1950)),m.c(0x6d1de0)));real(3)=0;
        if(random.integer_bits(1,precision,true))real(3)=m.store(m.mul(m.q(real(3)),m.c(0x6aca58)));
        real(4)=constant(0x6d1a90);real(5)=real(0);real(6)=real(1);
    }else if(t.frame==240){const auto color=services.color(0,127,255);allocate_effect(effects,pool.sound_triggers,{10,color,constant(0x6d1a60),constant(0x6d1cc0)});costume(3);}
    else if(t.frame==280){
        for(unsigned i=0;i<6;++i){
            const auto slot=allocate_projectile(pool,{1,3,{{0,127,255}},2,255,1,41});if(slot==-1)continue;auto b=bullet(static_cast<std::uint32_t>(slot));
            b.word(0x78,i);b.quad(0xa0,m.store(m.mul(m.i(b.word(0x78)),m.c(0x6d2750))));b.quad(0xa8,real(4));
            auto value=math.cosine(m.store(m.add(m.q(real(2)),m.q(b.quad(0xa0)))));b.quad(0x40,m.store(m.add(m.mul(value,m.q(b.quad(0xa8))),m.q(real(0)))));
            value=math.sine(m.store(m.add(m.q(real(2)),m.q(b.quad(0xa0)))));b.quad(0x48,m.store(m.add(m.mul(value,m.q(b.quad(0xa8))),m.q(real(1)))));
            b.word(8,130);b.word(0x34,1);integer(i)=static_cast<std::uint32_t>(slot);
        }
        for(unsigned i=0;i<6;++i)for(unsigned j=1;j<=4;++j){
            const auto slot=allocate_projectile(pool,{9,2,{{0,127,255}},2,255,1,41});if(slot==-1)continue;auto b=bullet(static_cast<std::uint32_t>(slot));
            b.word(0x78,i);b.word(0x7c,i==5?0u:i+1u);b.word(0x80,j);
            auto first=bullet(integer(b.word(0x78))),second=bullet(integer(b.word(0x7c)));
            b.quad(0x40,m.store(m.add(m.mul(m.div(m.sub(m.q(second.quad(0x40)),m.q(first.quad(0x40))),m.c(0x6d1910)),m.i(b.word(0x80))),m.q(first.quad(0x40)))));
            const auto first_y=bullet(integer(b.word(0x78))),second_y=bullet(integer(b.word(0x7c)));
            b.quad(0x48,m.store(m.add(m.mul(m.div(m.sub(m.q(second_y.quad(0x48)),m.q(first_y.quad(0x48))),m.c(0x6d1910)),m.i(b.word(0x80))),m.q(first_y.quad(0x48)))));
            b.word(8,131);b.word(0x34,1);
        }
        const auto slot=allocate_projectile(pool,{9,3,{{64,255,64}},2,255,1,41});
        if(slot!=-1){auto b=bullet(static_cast<std::uint32_t>(slot));b.quad(0xa0,m.store(m.mul(random.real_number(constant(0x6d1c60),precision),m.c(0x6d1950))));b.quad(0xa8,0);b.quad(0xb0,0);
            auto value=math.cosine(b.quad(0xa0));b.quad(0x40,m.store(m.add(m.mul(value,m.c(0x6a6100)),m.q(real(0)))));value=math.sine(b.quad(0xa0));b.quad(0x48,m.store(m.add(m.mul(value,m.c(0x6a6100)),m.q(real(1)))));b.word(8,133);b.word(0x34,1);}
        effect(14,{{64,64,255}});
    }
    if(frame()<320)return;
    ex_boss_early_timeline_ease(real(0),real(5),constant(0x6b25f8),m.precision);ex_boss_early_timeline_ease(real(1),real(6),constant(0x6b25f8),m.precision);
    integer(6)=static_cast<std::uint32_t>(remainder(t.frame,940));
    if(signed_word(integer(6))>=320&&signed_word(integer(6))<=559){
        if(remainder(integer(6),4)==0){
            const auto vertex=static_cast<std::uint32_t>(signed_word(integer(6)-320u)/40);
            const auto slot=allocate_projectile(pool,{9,0,{{255,0,0}},2,255,0,41});if(slot==-1)return;auto b=bullet(static_cast<std::uint32_t>(slot));
            b.quad(0x40,bullet(integer(vertex)).quad(0x40));b.quad(0x48,bullet(integer(vertex)).quad(0x48));
            auto value=math.cosine(m.store(m.add(m.mul(m.i(vertex),m.c(0x6d2750)),m.q(real(2)))));b.quad(0xa0,m.store(m.mul(value,m.c(0x6d1c18))));
            value=math.sine(m.store(m.add(m.mul(m.i(vertex),m.c(0x6d2750)),m.q(real(2)))));b.quad(0xa8,m.store(m.mul(value,m.c(0x6d1c18))));b.word(0x78,9);b.word(8,132);
        }
    }else if((signed_word(integer(6))>=620&&signed_word(integer(6))<=860)||(signed_word(integer(6))>=861&&signed_word(integer(6))<=920)){
        const bool speeding=signed_word(integer(6))<=860;
        if(speeding&&integer(6)==620)real(3)=0;
        real(2)=m.store(m.add(m.q(real(2)),m.q(real(3))));ex_boss_early_timeline_ease(real(3),speeding?constant(0x6d1918):0,constant(speeding?0x6d1ae8:0x6b25f8),m.precision);
        if(remainder(integer(6),40)<20&&remainder(integer(6),3)==0){
            const auto slot=allocate_projectile(pool,{9,1,{{200,0,250}},2,255,0,41});if(slot==-1)return;auto b=bullet(static_cast<std::uint32_t>(slot));
            b.quad(0x40,bullet(integer(0)).quad(0x40));b.quad(0x48,bullet(integer(0)).quad(0x48));
            auto value=math.cosine(real(2));b.quad(0xa0,m.store(m.add(value,value)));value=math.sine(real(2));b.quad(0xa8,m.store(m.add(value,value)));b.word(0x78,4);b.word(8,132);
        }
    }else if(integer(6)==100){real(5)=m.store(m.add(m.mul(random.real_number(constant(0x6d1f38),precision),m.c(0x6d1960)),m.c(0x6d1de8)));real(6)=m.store(m.add(m.mul(random.real_number(constant(0x6d1e38),precision),m.c(0x6d1960)),m.c(0x6d2780)));}
    else if(integer(6)==200){auto value=random.real_number(constant(0x6d1f20),precision);put(0xa8,m.add(m.mul(value,m.c(0x6d1960)),m.c(0x6d22b0)));value=random.real_number(constant(0x6d1a78),precision);put(0xb0,m.add(m.mul(value,m.c(0x6d1960)),m.c(0x6d21f0)));}
}
}}
