#pragma once
#include "core/stages/ExBossMiddleTimelineEngine.hpp"
namespace kaikeidou { namespace ex_boss_middle_timeline_detail {
inline void Engine::phase13(){
    random.real_number(0,precision);const auto who=character==0?0u:character==1?1u:2u;
    if(t.frame==1){target(who==0?0x6d1b20:0x6d1af0,0x6b25f8);protection.boss_state_a7ea60=2;}
    else if(who==0&&t.frame==60){
        auto& s=pool.secondary[1];s.word(0,1);pool.sound_triggers[18]=1;s.quad(0x70,constant(0x6d2618));s.quad(0x78,constant(0x6d1f60));s.quad(0x80,constant(0x6d2ec0));s.quad(0x88,0);
        auto value=math.cosine(s.quad(0x70));s.quad(0x10,m.store(m.add(m.mul(value,m.q(s.quad(0x78))),m.c(0x6d1a60))));value=math.sine(s.quad(0x70));s.quad(0x18,m.store(m.add(m.mul(value,m.q(s.quad(0x78))),m.c(0x6d1b38))));
        s.quad(0x20,constant(0x6d1e38));s.quad(0x28,constant(0x6d1a90));s.quad(0x30,constant(0x6d18f0));s.quad(0x38,constant(0x6d1900));s.quad(0x40,constant(0x6d19d8));s.word(4,158);s.word(0xc,0);s.word(0x48,0);costume(3);
    }else if(who==1&&frame()>=60&&frame()<=210&&remainder(t.frame,10)==0){
        const auto index=t.frame/10u-6u;const auto angle=m.store(m.add(m.mul(m.i(index),m.c(0x6d27f8)),m.c(0x6d1b60)));auto& s=pool.secondary[index];s.word(0,1);pool.sound_triggers[18]=1;
        auto value=math.cosine(angle);s.quad(0x10,m.store(m.add(m.mul(value,m.c(0x6d1cc0)),m.c(0x6d1a60))));value=math.sine(angle);s.quad(0x18,m.store(m.add(m.mul(value,m.c(0x6d1cc0)),m.c(0x6d1af0))));
        s.quad(0x20,constant(0x6d1e38));s.quad(0x28,constant(0x6d1a40));s.quad(0x30,constant(0x6d19c0));s.quad(0x38,constant(0x6d19c0));s.quad(0x40,constant(0x6d19c0));s.word(4,163);s.word(0xc,0);s.word(0x48,0);s.word(0x4c,0);if(t.frame==60)costume(3);
    }else if(who==2&&frame()>=60&&frame()<=198&&remainder(t.frame,6)==0){
        const auto index=t.frame/6u-10u;const auto angle=m.store(m.sub(m.mul(m.i(index),m.c(0x6d2198)),m.c(0x6d2140)));auto& s=pool.secondary[index];s.word(0,1);pool.sound_triggers[18]=1;
        s.quad(0x70,angle);s.quad(0x78,constant(0x6d25c0));s.quad(0x80,s.quad(0x70));s.quad(0x88,s.quad(0x78));s.quad(0x90,constant(0x6b25b8));s.quad(0x98,constant(0x6b25b8));
        auto value=math.cosine(s.quad(0x70));s.quad(0x10,m.store(m.add(m.mul(value,m.q(s.quad(0x78))),m.c(0x6d1a60))));value=math.sine(s.quad(0x70));s.quad(0x18,m.store(m.add(m.mul(value,m.q(s.quad(0x78))),m.c(0x6d1af0))));
        s.quad(0x20,constant(0x6d1e38));s.quad(0x28,constant(0x6b25f8));
        const auto one=m.i(1);const auto part=[&](unsigned base){return m.mul(m.i(index-base),m.c(0x6d1fb8));};
        if(index<=3){s.quad(0x30,m.store(one));s.quad(0x38,m.store(part(0)));s.quad(0x40,0);}
        else if(index<=7){s.quad(0x30,m.store(m.sub(one,part(4))));s.quad(0x38,m.store(one));s.quad(0x40,0);}
        else if(index<=11){s.quad(0x30,0);s.quad(0x38,m.store(one));s.quad(0x40,m.store(part(8)));}
        else if(index<=15){s.quad(0x30,0);s.quad(0x38,m.store(m.sub(one,part(12))));s.quad(0x40,m.store(one));}
        else if(index<=19){s.quad(0x30,m.store(part(16)));s.quad(0x38,0);s.quad(0x40,m.store(one));}
        else{s.quad(0x30,m.store(one));s.quad(0x38,0);s.quad(0x40,m.store(m.sub(one,part(20))));}
        s.word(4,166);s.word(0xc,0);s.word(0x48,0);if(t.frame==60)costume(3);
    }
    auto& last=pool.secondary[who==0?1u:who==1?15u:23u];
    if((who==0||frame()>=(who==1?210:198))&&compare(m.add(m.q(last.quad(0x28)),m.c(0x6d1868)),m.q(last.quad(0x20)))>0&&stage5_boss_secondary_contact(pool,player,m.precision)){
        ++t.subphase;reset();if(who==0)pool.secondary[0].quad(0x20,pool.secondary[0].quad(0x28));else for(unsigned i=0;i<(who==1?16u:24u);++i){auto& s=pool.secondary[i];s.word(0x48,1);s.quad(0x20,s.quad(0x28));}
    }
}
}}
