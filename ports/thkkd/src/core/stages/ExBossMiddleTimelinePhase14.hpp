#pragma once
#include "core/stages/ExBossMiddleTimelineEngine.hpp"
namespace kaikeidou { namespace ex_boss_middle_timeline_detail {
inline void Engine::phase14(){
    if(player.word(0x2c)){protection.boss_state_a7ea60=1;allocation.field_a7ea58=0;}else{protection.boss_state_a7ea60=0;allocation.field_a7ea58=1;}
    if(character==0)phase14_reimu();else if(character==1)phase14_marisa();else phase14_sanae();
}
inline void Engine::phase14_reimu(){
    if(t.frame==1){ex_boss_middle_timeline_name(context.spell_name,0x6c5e0c);services.configure_phase(1,70,20000,140);real(0)=m.store(m.mul(random.real_number(constant(0x6d1c60),precision),m.c(0x6d1950)));}
    else if(t.frame==120){
        const auto initialize=[&](unsigned i,bool outer){auto& s=pool.secondary[i];s.word(0,1);s.quad(0x70,m.store(m.mul(m.i(i),m.c(0x6d2618))));s.quad(0x78,constant(outer?0x6d2ef8:0x6d1f60));s.quad(0x80,constant(outer?0x6d2eb8:0x6d2ec0));s.quad(0x88,0);
            auto value=math.cosine(s.quad(0x70));s.quad(0x10,m.store(m.add(m.mul(value,m.q(s.quad(0x78))),m.c(0x6d1a60))));value=math.sine(s.quad(0x70));s.quad(0x18,m.store(m.add(m.mul(value,m.q(s.quad(0x78))),m.c(0x6d1b38))));
            s.quad(0x20,0);s.quad(0x28,constant(0x6d1a90));s.quad(0x30,constant(0x6d18f0));s.quad(0x38,constant(0x6d1900));s.quad(0x40,constant(0x6d19d8));s.word(4,158);s.word(0xc,0);s.word(0x48,0);
        };
        for(unsigned i=0;i<=4;++i)if(i!=1)initialize(i,false);for(unsigned i=10;i<=14;++i)initialize(i,true);pool.sound_triggers[18]=1;costume(3);
    }else if(t.frame==240){effect(12,{{240,0,200}});costume(3);}
    else if(t.frame==280){for(unsigned i=0;i<5;++i){auto& a=pool.secondary[i];a.word(0x48,1);a.quad(0x20,a.quad(0x28));auto& b=pool.secondary[i+10u];b.word(0x48,1);b.quad(0x20,b.quad(0x28));}}
    else if(t.frame==540){effect(12,{{200,0,240}});costume(3);}
    if(frame()>=580&&remainder(t.frame,20)==0){
        for(unsigned i=0;i<12;++i){const auto slot=allocate_projectile(pool,{0,0,{{200,0,240}},0,255,1,41});if(slot==-1)continue;auto b=bullet(static_cast<std::uint32_t>(slot));b.quad(0xa0,m.store(m.add(m.mul(m.i(i),m.c(0x6d2140)),m.q(real(0)))));b.quad(0xa8,constant(0x6d1ae8));
            auto value=math.cosine(b.quad(0xa0));b.quad(0x40,m.store(m.add(m.mul(value,m.q(b.quad(0xa8))),q(0x38))));value=math.sine(b.quad(0xa0));b.quad(0x48,m.store(m.add(m.mul(value,m.q(b.quad(0xa8))),q(0x40))));b.word(8,159);
        }
        const auto value=random.real_number(constant(0x6d1858),precision);real(0)=m.store(m.sub(m.q(real(0)),m.add(m.mul(value,m.c(0x6d1950)),m.c(0x6d24f8))));
    }
}
inline void Engine::phase14_marisa(){
    if(t.frame==1){ex_boss_middle_timeline_name(context.spell_name,0x6c5e28);services.configure_phase(1,70,6000,141);protection.field_a7eab0=9;integer(1)=0;real(0)=constant(0x6d2eb0);integer(2)=0;real(1)=0;integer(3)=0;}
    else if(t.frame==120){effect_at(12,{{255,255,255}},constant(0x6d1a60),constant(0x6d1af0));costume(3);}
    else if(t.frame==500){effect_at(12,{{0,0,255}},constant(0x6d1a60),constant(0x6d1af0));costume(3);}
    if(frame()>=160&&remainder(t.frame,20)==0){for(unsigned i=0;i<16;++i)if(static_cast<std::int64_t>(i)==remainder(integer(0),16)){auto& s=pool.secondary[i];s.word(0x4c,100);s.quad(0x30,0x3ff0000000000000ull);s.quad(0x38,0x3ff0000000000000ull);s.quad(0x40,0x3ff0000000000000ull);pool.sound_triggers[18]=1;}++integer(0);}
    if(frame()<540||remainder(t.frame,10)!=0)return;
    if(integer(3))--integer(3);++integer(2);
    if(remainder(integer(2),8)<=5&&integer(3)==0){const auto slot=allocate_projectile(pool,{2,1,{{0,0,255}},2,255,1,41});if(slot!=-1){auto b=bullet(static_cast<std::uint32_t>(slot));auto value=math.cosine(real(0));b.quad(0x50,m.store(m.mul(value,m.c(0x6d2150))));value=math.sine(real(0));b.quad(0x58,m.store(m.mul(value,m.c(0x6d2150))));b.quad(0x40,enemy().quad(0x38));b.quad(0x48,enemy().quad(0x40));b.word(8,164);}}
    if(integer(1)==0){real(0)=m.store(m.sub(m.q(real(0)),m.c(0x6d2638)));if(compare(m.c(0x6d1b60),m.q(real(0)))>0){integer(1)=1;integer(3)=6;}}
    else{real(0)=m.store(m.add(m.q(real(0)),m.c(0x6d2638)));if(compare(m.c(0x6d2eb0),m.q(real(0)))<0){integer(1)=0;integer(3)=6;}}
    if(remainder(t.frame,20)==0)for(unsigned i=0;i<3;++i){const auto slot=allocate_projectile(pool,{3,2,{{255,127,255}},0,255,1,41});if(slot==-1)continue;auto b=bullet(static_cast<std::uint32_t>(slot));
        const auto dx=m.store(m.sub(m.q(player.quad(0x90)),q(0x38))),dy=m.store(m.sub(m.q(player.quad(0x98)),q(0x40)));auto value=math.atan2(dy,dx);b.quad(0xa0,m.store(m.add(m.add(value,m.c(0x6d2210)),m.mul(m.i(i),m.c(0x6d2008)))));b.quad(0xa8,constant(0x6d1ae8));
        value=math.cosine(b.quad(0xa0));b.quad(0x40,m.store(m.add(m.mul(value,m.q(b.quad(0xa8))),q(0x38))));value=math.sine(b.quad(0xa0));b.quad(0x48,m.store(m.add(m.mul(value,m.q(b.quad(0xa8))),q(0x40))));b.word(8,165);
    }
}
inline void Engine::phase14_sanae(){
    if(t.frame==1){ex_boss_middle_timeline_name(context.spell_name,0x6c5e48);services.configure_phase(1,77,20000,142);}if(frame()<120)return;
    auto step=remainder(t.frame,1800)-120;if(step<0)step+=1800;
    const auto radial=[&](unsigned first,unsigned last,std::uint32_t radius){pool.sound_triggers[18]=1;for(unsigned i=0;i<6;++i){for(unsigned j=first;j<=last;++j)pool.secondary[i*4u+j].quad(0x88,constant(radius));for(unsigned j=first;j<=last;++j)pool.secondary[i*4u+j].quad(0x98,constant(0x6b25b8));}};
    const auto turn_one=[&](unsigned j,bool subtract,std::uint32_t angle,std::uint32_t rate){pool.sound_triggers[18]=1;for(unsigned i=0;i<6;++i){auto& s=pool.secondary[i*4u+j];s.quad(0x80,m.store(subtract?m.sub(m.q(s.quad(0x80)),m.c(angle)):m.add(m.q(s.quad(0x80)),m.c(angle))));s.quad(0x90,constant(rate));}};
    const auto turn_pair=[&](unsigned j,bool subtract,std::uint32_t angle){pool.sound_triggers[18]=1;for(unsigned i=0;i<6;++i){for(unsigned k=0;k<2;++k){auto& s=pool.secondary[i*4u+j+k*2u];s.quad(0x80,m.store(subtract?m.sub(m.q(s.quad(0x80)),m.c(angle)):m.add(m.q(s.quad(0x80)),m.c(angle))));}pool.secondary[i*4u+j].quad(0x90,constant(0x6d2e98));pool.secondary[i*4u+j+2u].quad(0x90,constant(0x6d2e98));}};
    const auto emit=[&](unsigned i,std::uint32_t speed){auto& s=pool.secondary[i];const auto angle=s.quad(0x70),y=enemy().quad(0x40),x=enemy().quad(0x38);
        const auto blue=ex_boss_middle_timeline_convert(m.mul(m.q(s.quad(0x40)),m.c(0x6d2fa0)));const auto green=ex_boss_middle_timeline_convert(m.mul(m.q(s.quad(0x38)),m.c(0x6d2fa0)));const auto red=ex_boss_middle_timeline_convert(m.mul(m.q(s.quad(0x30)),m.c(0x6d2fa0)));
        ex_boss_middle_timeline_emit(pool,{{10,0,{{red,green,blue}},0,255,1,41},x,y,1,0,constant(speed),angle,1,0},player,p,random,precision,math);
    };
    if(step==0||step==900){effect_at(12,{{255,255,255}},constant(0x6d1a60),constant(0x6d1af0));costume(3);}
    else if(step==40||step==940){radial(step==40?1u:0u,step==40?3u:2u,0x6d2490);for(unsigned i=0;i<24;++i){if((i&1u)==(step==40?0u:1u))emit(i,0x6d1818);emit(i,0x6d1f90);emit(i,0x6d2b78);}}
    else if(step==120)radial(2,2,0x6d1fe8);
    else if(step==180)turn_one(0,true,0x6d2ea8,0x6d2ea0);
    else if(step==220)turn_pair(1,false,0x6d2138);
    else if(step==260)turn_one(2,true,0x6d1b60,0x6d18c8);
    else if(step==550||step==1450){for(unsigned i=0;i<24;++i)emit(i,0x6d2c50);}
    else if(step==650)radial(2,2,0x6d2490);
    else if(step==720||step==1620){radial(step==720?1u:0u,step==720?3u:2u,0x6d25c0);for(unsigned i=0;i<24;++i)emit(i,0x6d1f80);}
    else if(step==1020)radial(1,1,0x6d1fe8);
    else if(step==1080)turn_one(3,false,0x6d2948,0x6d2ea0);
    else if(step==1120)turn_pair(0,true,0x6d2e90);
    else if(step==1160)turn_one(1,false,0x6d2750,0x6d18c8);
    else if(step==1550)radial(1,1,0x6d2490);
}
}}
