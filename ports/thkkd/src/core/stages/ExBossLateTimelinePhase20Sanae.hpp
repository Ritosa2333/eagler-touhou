#pragma once
#include "core/stages/ExBossLateTimelineEngine.hpp"
namespace kaikeidou { namespace ex_boss_late_timeline_detail {
inline void Engine::phase20_sanae(){
    if(t.frame==200){ex_boss_late_timeline_name(context.spell_name,0x6c5f54);services.configure_phase(1,90,extra.boss_hp_multiplier*35000u,151);enemy().word(8,0);integer(0)=0;integer(1)=90000;}if(frame()<300)return;
    const auto tier=signed_word(enemy().word(0x14))>21000?0u:signed_word(enemy().word(0x14))>14000?1u:2u;
    if(tier==1&&signed_word(integer(1))>21000){integer(0)=0;clear_projectiles(pool,100);}else if(tier==2&&signed_word(integer(1))>14000){integer(0)=0;clear_projectiles(pool,100);}
    const auto flash=[&](bool burst){effect(burst?14u:12u,{{0,255,0}});effect_at(burst?14u:13u,{{0,255,0}},player.quad(0x90),player.quad(0x98));};
    const auto emit=[&](unsigned count,std::uint32_t spacing,std::uint64_t speed,std::uint64_t angle,bool absolute=false){ex_boss_late_timeline_emit(pool,{{16,0,{{0,255,0}},0,255,1,41},enemy().quad(0x38),enemy().quad(0x40),count,constant(spacing),speed,angle,absolute?1u:0u,0},player,p,random,precision,math);};
    const auto pair=[&](std::uint64_t speed0,std::uint64_t speed1){emit(16,0x6d27f8,speed0,0);emit(16,0x6d27f8,speed1,constant(0x6d1ba0));};
    const auto lattice=[&](unsigned variant){const auto selected=random.integer_bits(variant==4?2u:5u,precision,true);
        constexpr std::array<std::uint32_t,6> angles{{0x6d2e90,0x6d2d78,0x6d2ea8,0x6d2d70,0x6d1860,0x6d25e0}};
        for(unsigned i=0;i<32;++i){const auto which=variant==4?(i%4u<=1?4u:5u):variant;const bool red=(which&1u)!=0;const auto slot=allocate_projectile(pool,{1,0,red?std::array<std::uint32_t,3>{{255,0,0}}:std::array<std::uint32_t,3>{{0,0,255}},2,255,0,41});if(slot==-1)continue;auto b=bullet(static_cast<std::uint32_t>(slot));
            b.quad(0xa0,0);b.quad(0xa8,constant(angles[which]));b.quad(0xb0,0);b.quad(0xb8,m.store(m.i(i*12u+((which==1||which==3)?22u:16u))));b.quad(0xc0,constant(which<2?0x6b25f8:which<4?0x6d1aa8:0x6d1ae8));b.word(0x78,i%(variant==4?3u:6u)==selected?1u:0u);
            auto value=math.cosine(b.quad(0xa0));b.quad(0x40,m.store(m.add(m.mul(value,m.q(b.quad(0xb0))),m.c(0x6d1a60))));value=math.sine(b.quad(0xa0));b.quad(0x48,m.store(m.add(m.mul(value,m.q(b.quad(0xb0))),m.c(0x6d1f08))));b.word(8,187);b.word(0x34,1);
        }
    };
    const auto step=remainder(integer(0),tier==0?300u:tier==1?440u:370u);
    if(step==0)flash(false);else if(step==40)flash(true);
    else if(tier==0){if(step==60)lattice(0);else if(step==120)lattice(1);else if(step==180)pair(constant(0x6d1948),0x3ff0000000000000ull);else if(step==230)pair(constant(0x6d2a68),constant(0x6d1b00));else if(step==280)pair(constant(0x6d1ad0),constant(0x6d19c0));}
    else if(tier==1){if(step==60)lattice(2);else if(step==280)lattice(3);else if(step==100||step==320)pair(constant(0x6d1948),0x3ff0000000000000ull);else if(step==160||step==380)pair(constant(0x6d1ad0),constant(0x6d19c0));}
    else if(step==60)lattice(4);
    if(tier==2){
        if(remainder(integer(0),80)==0){emit(5,0x6d2618,constant(0x6d1ad0),0);emit(5,0x6d2618,constant(0x6d19d8),constant(0x6d1860));
            for(unsigned i=1;i<=2;++i){emit(5,0x6d2618,constant(0x6d1ad0),m.store(m.mul(m.i(i),m.c(0x6d1810))));emit(5,0x6d2618,constant(0x6d1ad0),m.store(m.mul(m.i(i),m.c(0x6d2d68))));emit(5,0x6d2618,constant(0x6d19d8),m.store(m.add(m.mul(m.i(i),m.c(0x6d1810)),m.c(0x6d1860))));emit(5,0x6d2618,constant(0x6d19d8),m.store(m.add(m.mul(m.i(i),m.c(0x6d2d68)),m.c(0x6d1860))));}
        }
        if(remainder(integer(0),160)==0)emit(19,0x6d2d60,constant(0x6d1948),constant(0x6d1b60),true);
    }
    ++integer(0);integer(1)=enemy().word(0x14);
}
}}
