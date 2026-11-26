#pragma once
#include "core/stages/ExBossMiddleTimelineEngine.hpp"
namespace kaikeidou { namespace ex_boss_middle_timeline_detail {
inline void Engine::phase11(){
    if(player.word(0x2c)){protection.boss_state_a7ea60=1;allocation.field_a7ea58=0;}else{protection.boss_state_a7ea60=0;allocation.field_a7ea58=1;}
    if(character==0)phase11_reimu();else if(character==1)phase11_marisa();else phase11_sanae();
}
inline void Engine::phase11_reimu(){
    if(t.frame==1)target(0x6d1d00,0x6b25f8);
    else if(t.frame==80){effect(12,{{0,0,255}});costume(3);}
    else if(t.frame==110){effect(12,{{0,255,0}});integer(3)=random.integer_bits(1,precision,true);real(0)=constant(0x6d1de0);}
    else if(t.frame==150){ex_boss_middle_timeline_name(context.spell_name,0x6c5db0);services.configure_phase(1,90,extra.boss_hp_multiplier*24000u,137);enemy().quad(0xa8,constant(0x6d2f30));enemy().quad(0xb0,0);enemy().quad(0xb8,constant(0x6d1aa8));enemy().quad(0xc0,constant(0x6b25f8));}
    else if(frame()>=280){
        if(t.frame==900){enemy().quad(0xa8,constant(0x6d1df8));enemy().quad(0xb0,constant(0x6d1b38));enemy().quad(0xb8,constant(0x6b25b8));enemy().quad(0xc0,constant(0x6b25f8));}
        else if(t.frame==1160){effect(12,{{255,0,0}});enemy().quad(0xa8,constant(0x6d1a60));costume(3);}
        else if(frame()>=1200&&remainder(t.frame,6)==0){
            ex_boss_middle_timeline_emit(pool,{{12,2,{{255,0,0}},0,255,1,41},enemy().quad(0x38),m.store(m.sub(q(0x40),m.c(0x6d1a88))),2,constant(0x6d1860),constant(0x6d1ad0),real(0),1,0},player,p,random,precision,math);
            auto negative=m.q(real(0));negative.negative=!negative.negative;
            ex_boss_middle_timeline_emit(pool,{{12,2,{{255,0,0}},0,255,1,41},enemy().quad(0x38),m.store(m.sub(q(0x40),m.c(0x6d1a88))),2,constant(0x6d1860),constant(0x6d1c80),m.store(m.mul(negative,m.c(0x6d2488))),1,0},player,p,random,precision,math);
            real(0)=m.store(m.add(m.q(real(0)),m.c(0x6d19b8)));
        }
        const auto residue=remainder(t.frame,240);
        if(residue==40||residue==160){
            const bool reverse=residue==160;auto value=random.real_number(constant(0x6d1e38),precision);const auto shift=m.store(m.mul(value,m.c(0x6a7708)));
            value=random.real_number(constant(0x6d1a78),precision);const auto speed=m.store(m.add(m.mul(value,m.c(0x6d1950)),m.c(0x6d1a00)));const auto top=random.integer_bits(24,precision,true),bottom=random.integer_bits(24,precision,true);
            const auto spawn=[&](unsigned i,std::uint64_t y){const auto slot=allocate_projectile(pool,{21,1,reverse?std::array<std::uint32_t,3>{{0,255,0}}:std::array<std::uint32_t,3>{{0,0,255}},0,255,1,41});if(slot==-1)return;auto b=bullet(static_cast<std::uint32_t>(slot));
                b.quad(0x40,m.store(reverse?m.sub(m.add(m.c(0x6d1cb0),m.q(shift)),m.i(i*16u)):m.add(m.sub(m.i(0),m.q(shift)),m.i(i*16u))));b.quad(0x48,y);b.quad(0x50,reverse?(speed^0x8000000000000000ull):speed);b.quad(0x58,constant(0x6d18e0));b.word(8,reverse?153u:152u);b.word(0x34,1);b.word(0x78,reverse?1u:0u);
            };
            for(unsigned i=0;i<=24;++i){spawn(i,constant(0x6d1a30));if(i==top)for(unsigned j=1;j<=2;++j)spawn(i,m.store(m.i(j*16u-8u)));if(i==bottom)for(unsigned j=1;j<=2;++j)spawn(i,m.store(m.i(0xfffffff8u-j*16u)));}
        }
        if(remainder(t.frame,100)==0){integer(3)=1u-integer(3);pool.sound_triggers[44]=1;}
    }
}
}}
