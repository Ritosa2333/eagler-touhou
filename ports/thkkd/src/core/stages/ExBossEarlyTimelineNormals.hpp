#pragma once
#include "core/stages/ExBossEarlyTimelineEngine.hpp"
namespace kaikeidou { namespace ex_boss_early_timeline_detail {
inline void Engine::normals(unsigned variant){
    const std::array<std::uint32_t,3> color=variant==1?std::array<std::uint32_t,3>{{0,0,255}}:variant==3?std::array<std::uint32_t,3>{{0,255,255}}:std::array<std::uint32_t,3>{{0,255,0}};
    if(variant==1&&t.frame==1){enemy().word(8,14);enemy().quad(0xa8,constant(0x6d1a60));enemy().quad(0xb0,constant(0x6d1a90));services.begin_boss_effect();}
    else if(t.frame==(variant==1?81u:40u)){
        target();services.configure_phase(0,40,extra.boss_hp_multiplier*(variant==1?17000u:variant==3?16000u:15000u),0xfffffff6u);
        effect(12,color);integer(0)=random.integer_bits(1,precision,true)*2u-1u;if(variant==3)integer(1)=random.integer_bits(3,precision,true);costume(3);
    }
    const auto start=variant==1?120u:80u;if(frame()<start)return;
    if(variant==1){random.real_number(0x3ff0000000000000ull,precision);consume({{5,4,1,2,3}});}
    if(remainder(t.frame,170)==start){
        const auto angle=m.store(m.mul(random.real_number(constant(0x6d1c60),precision),m.c(0x6d1950)));
        std::array<std::uint32_t,2> parents{};
        for(unsigned side=0;side<2;++side){
            const auto kind=variant==5?18u+8u*(side?integer(0)==1:integer(0)!=1):18u;
            parents[side]=static_cast<std::uint32_t>(allocate_projectile(pool,{kind,1,color,2,255,1,41}));if(parents[side]==0xffffffffu)continue;
            auto b=bullet(parents[side]);b.quad(0x40,enemy().quad(0x38));b.quad(0x48,enemy().quad(0x40));b.word(8,117);
            b.quad(0x60,variant==3&&side?m.store(m.add(m.mul(m.i(integer(0)),m.c(0x6d1b60)),m.q(angle))):angle);b.quad(0x58,constant(0x6d1ad0));
            b.quad(0xa0,variant==5?m.store(m.i((side?344u:20u)+(integer(0)!=1?20u:0u))):constant(side?0x6d2160:0x6d1a40));
            if(variant==3){b.quad(0xa8,constant(0x6d2fb0));if(signed_word(integer(1))<=1)b.quad(0xa8,m.store(m.mul(m.q(b.quad(0xa8)),m.c(0x6aca58))));}
            else b.quad(0xa8,m.store(m.mul(m.i(integer(0)),m.c(0x6d2fb0))));
            b.quad(0xe8,constant(variant==5?0x6d2f68:0x6d1bf0));b.word(0x34,1);
        }
        for(unsigned i=0;i<8;++i){
            const auto first=orbit_child(parents[0],i,color,variant,false);const auto second=orbit_child(parents[1],i,color,variant,true);
            if(i==0||i==4)for(unsigned j=1;j<(variant==1?10u:variant==3?8u:9u);++j)link(first,second,j,variant==1?0x6d1898:variant==3?0x6d18e8:0x6d2658,color);
        }
        integer(0)=0u-integer(0);if(variant==3){++integer(1);if(integer(1)==4)integer(1)=0;}
    }
    if(remainder(t.frame,300)==(variant==1?119:79))move_target();
}
}}
