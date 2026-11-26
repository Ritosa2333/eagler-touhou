#pragma once
#include "core/stages/ExBossMiddleTimelineEngine.hpp"
namespace kaikeidou { namespace ex_boss_middle_timeline_detail {
inline void Engine::phase10(){
    if(t.frame==40){target();services.configure_phase(0,40,extra.boss_hp_multiplier*17000u,0xfffffff6u);effect(12,{{255,0,0}});integer(0)=random.integer_bits(1,precision,true)*2u-1u;costume(3);}
    if(frame()<80)return;
    if(remainder(t.frame,160)==80){
        const auto angle=m.store(m.mul(random.real_number(constant(0x6d1c60),precision),m.c(0x6d1950)));std::array<std::uint32_t,2> parents{};
        for(unsigned side=0;side<2;++side){parents[side]=static_cast<std::uint32_t>(allocate_projectile(pool,{18,1,{{255,0,0}},2,255,1,41}));if(parents[side]==0xffffffffu)continue;auto b=bullet(parents[side]);
            b.quad(0x40,enemy().quad(0x38));b.quad(0x48,enemy().quad(0x40));b.word(8,117);b.quad(0x60,angle);b.quad(0x58,constant(0x6d1e40));b.quad(0xa0,constant(side?0x6d2160:0x6d1a40));b.quad(0xa8,m.store(m.mul(m.i(integer(0)),m.c(0x6d1918))));b.quad(0xe8,constant(0x6d1bf0));b.word(0x34,1);
        }
        for(unsigned i=0;i<16;++i){const auto first=orbit_child(parents[0],i,{{255,0,0}},10,false);const auto second=orbit_child(parents[1],i,{{255,0,0}},10,true);if(i>=6&&i<=10)for(unsigned j=1;j<7;++j)link(first,second,j,0x6d2410,{{255,0,0}});}
        integer(0)=0u-integer(0);
    }
    if(remainder(t.frame,300)==79)move_target();
}
}}
