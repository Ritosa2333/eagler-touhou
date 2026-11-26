#pragma once
#include "core/stages/ExBossMiddleTimelineEngine.hpp"
namespace kaikeidou { namespace ex_boss_middle_timeline_detail {
inline void Engine::phase15(){
    if(t.frame==40){target(0x6d1fe8);services.configure_phase(0,60,extra.boss_hp_multiplier*18000u,0xfffffff6u);effect_at(10,{{64,64,64}},constant(0x6d1a88),constant(0x6d1b38));effect_at(10,{{64,64,64}},constant(0x6d2798),constant(0x6d1b38));costume(3);}
    if(frame()<80)return;
    if(t.frame==80){
        std::array<std::uint32_t,2> parents{};
        for(unsigned side=0;side<2;++side){parents[side]=static_cast<std::uint32_t>(allocate_projectile(pool,{26,1,{{64,64,64}},0,255,1,41}));if(parents[side]==0xffffffffu)continue;auto b=bullet(parents[side]);
            b.quad(0x40,constant(side?0x6d2798:0x6d1a88));b.quad(0x48,constant(0x6d1b38));b.word(8,122);b.quad(0x60,constant(0x6d1de0));b.quad(0xa0,constant(side?0x6d25c8:0x6d24e0));b.quad(0xa8,constant(side?0x6d2e80:0x6d2e88));b.quad(0xb0,0);b.word(0x34,1);
        }
        integer(0)=parents[0];integer(1)=parents[1];real(0)=constant(0x6d1ad8);real(1)=constant(0x6d1988);
        for(unsigned i=0;i<8;++i){orbit_child(parents[0],i,{{0,0,0}},15,false,1,33,0x6d2738);orbit_child(parents[1],i,{{0,0,0}},15,true,1,33,0x6d2738);if(i==0){orbit_child(parents[0],i,{{0,0,0}},15,false,1,20,0x6b25f8);orbit_child(parents[1],i,{{0,0,0}},15,true,1,20,0x6b25f8);}}
    }
    const auto distance=ex_boss_middle_timeline_absolute(m.store(m.sub(m.q(bullet(integer(1)).quad(0x40)),m.q(bullet(integer(0)).quad(0x40)))));
    if(compare(m.q(distance),m.c(0x6d2e78))<=0){
        constexpr std::array<unsigned,3> periods{{5,9,7}};constexpr std::array<std::uint32_t,3> angle_range{{0x6d1d00,0x6d1a90,0x6d1cc0}},angle_base{{0x6d1e80,0x6d22e0,0x6d2a78}},speed_base{{0x6d2488,0x6d1ad0,0x6d18f0}};
        for(unsigned k=0;k<3;++k)if(remainder(t.frame,periods[k])==0){
            auto value=random.real_number(constant(angle_range[k]),precision);const auto angle=m.store(m.add(m.mul(value,m.c(0x6a7708)),m.c(angle_base[k])));
            value=random.real_number(constant(0x6b25b8),precision);const auto speed=m.store(m.add(m.mul(value,m.c(0x6a7708)),m.c(speed_base[k])));
            const auto ybase=m.store(m.sub(m.q(bullet(integer(0)).quad(0x48)),m.c(0x6d18e8)));value=random.real_number(constant(0x6d18c0),precision);const auto y=m.store(m.add(m.mul(value,m.c(0x6d1960)),m.q(ybase)));
            const auto x=m.store(m.div(m.add(m.q(bullet(integer(0)).quad(0x40)),m.q(bullet(integer(1)).quad(0x40))),m.c(0x6d1948)));
            const auto blue=random.integer_bits(255,precision,true),green=random.integer_bits(255,precision,true),red=random.integer_bits(255,precision,true);
            ex_boss_middle_timeline_emit(pool,{{k==2?4u:0u,k==2?3u:2u,{{red,green,blue}},0,255,0,41},x,y,2,constant(0x6d1860),speed,angle,1,1},player,p,random,precision,math);
        }
    }
    if(remainder(t.frame,300)==79)move_target(0x6d1858);
}
}}
