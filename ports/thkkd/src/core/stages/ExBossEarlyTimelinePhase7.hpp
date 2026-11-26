#pragma once
#include "core/stages/ExBossEarlyTimelineEngine.hpp"
namespace kaikeidou { namespace ex_boss_early_timeline_detail {
inline void Engine::phase7(){
    if(t.frame==40){target();services.configure_phase(0,40,extra.boss_hp_multiplier*16000u,0xfffffff6u);integer(0)=random.integer_bits(1,precision,true)*2u-1u;real(0)=m.store(m.mul(random.real_number(constant(0x6d1c60),precision),m.c(0x6d1950)));const auto x=m.store(m.i(integer(0)==1?20u:364u));effect_at(10,{{255,255,0}},x,constant(0x6d1a40));costume(3);}
    if(frame()<80)return;
    const std::array<std::uint32_t,3> yellow{{255,255,0}};
    const auto anchor=[&](std::uint32_t y){
        const auto slot=static_cast<std::uint32_t>(allocate_projectile(pool,{18,1,yellow,2,255,0,41}));if(slot==0xffffffffu)return slot;
        auto b=bullet(slot);b.quad(0x40,m.store(m.i(integer(0)==1?20u:364u)));b.quad(0x48,constant(y));b.word(8,120);b.quad(0x60,real(0));b.quad(0x58,constant(0x6d1ad0));b.quad(0xe8,constant(0x6d2f38));b.word(0x34,1);return slot;
    };
    if(remainder(t.frame,100)==80){
        if(t.frame==80){const auto parent=anchor(0x6d1bf8);for(unsigned i=0;i<8;++i){const auto slot=orbit_child(parent,i,yellow,7,false,0);if(i==0)integer(1)=slot;else if(i==4)integer(2)=slot;}integer(0)=0u-integer(0);}
        const auto parent=anchor(0x6d2f30);integer(3)=integer(1);integer(4)=integer(2);
        for(unsigned i=0;i<8;++i){
            const auto slot=orbit_child(parent,i,yellow,7,false,0);
            if(i==0){for(unsigned j=1;j<9;++j)link(integer(3),slot,j,0x6d2658,yellow,0);integer(1)=slot;}
            else if(i==4){for(unsigned j=1;j<9;++j)link(integer(4),slot,j,0x6d2658,yellow,0);integer(2)=slot;}
        }
        integer(0)=0u-integer(0);
    }
    real(0)=m.store(m.add(m.q(real(0)),m.c(0x6d2fb0)));
    if(remainder(t.frame,300)==79)move_target();
}
}}
