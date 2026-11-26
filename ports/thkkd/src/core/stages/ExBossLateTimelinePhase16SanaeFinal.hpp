#pragma once
#include "core/stages/ExBossLateTimelineEngine.hpp"
namespace kaikeidou { namespace ex_boss_late_timeline_detail {
inline void Engine::phase16_sanae_final(){
    if(frame()>=4588&&frame()<=4800)for(unsigned i=0;i<3000;++i){auto b=bullet(i);if(b.word(0)==1&&b.word(4)==21&&b.word(0x20)==255&&compare(m.i((4800u-t.frame)*2u),m.add(m.q(b.quad(0x40)),m.c(0x6d1a40)))<=0)remove_projectile(pool,i,100);}
    if(t.frame==4800){clear_projectiles(pool,100);random.real_number(constant(0x6d1c60),precision);random.real_number(constant(0x6d1c60),precision);integer(0)=0;integer(1)=0;real(8)=constant(0x6d1878);real(9)=constant(0x6d18d8);real(2)=constant(0x6d1a60);const auto value=random.real_number(constant(0x6d1c60),precision);real(3)=m.store(m.mul(value,m.c(0x6d1950)));real(4)=real(3);real(5)=constant(0x6d2e10);real(6)=constant(0x6d2e08);real(7)=constant(0x6d1a90);real(10)=constant(0x6d2180);}
    else if(t.frame==4860){effect_at(10,{{255,0,0}},real(8),0);effect_at(10,{{0,0,255}},real(9),0);}
    if(frame()<4900)return;
    if(remainder(t.frame,180)>=140){integer(1)=1;if(remainder(t.frame,180)==140)pool.sound_triggers[44]=1;}
    else{integer(1)=0;if(remainder(t.frame,180)==100)effect_at(10,{{255,0,255}},real(2),0);else if(remainder(t.frame,180)==0)pool.sound_triggers[44]=1;}
    const auto line=[&](unsigned source,std::array<std::uint32_t,3> color){const auto packed=services.color(color[0],color[1],color[2]);const auto y2=integer(0)+16u;const auto x2=ex_boss_late_timeline_convert(m.add(m.c(0x6d1e48),m.q(real(source))));const auto x1=ex_boss_late_timeline_convert(m.add(m.c(0x6d1e48),m.q(real(source))));services.draw_line(x1,16,x2,y2,packed,1);};
    line(8,{{255,0,0}});line(9,{{0,0,255}});line(2,{{255,0,255}});if(signed_word(integer(0))<448)integer(0)+=8;
    if(remainder(t.frame,2)==0){
        const auto first=static_cast<std::uint32_t>(allocate_projectile(pool,{0,0,{{255,0,0}},0,255,0,41}));if(first!=0xffffffffu){auto b=bullet(first);b.word(0x78,0);b.quad(0x48,constant(0x6d19f0));b.word(8,192);b.word(0x34,1);}
        const auto second=static_cast<std::uint32_t>(allocate_projectile(pool,{0,0,{{0,0,255}},0,255,0,41}));if(second!=0xffffffffu){auto b=bullet(second);b.word(0x78,0);b.quad(0x48,constant(0x6d19f0));b.word(8,192);b.word(0x34,1);}
        const auto slot=allocate_projectile(pool,{9,0,{{255,0,255}},2,255,0,41});if(slot!=-1){auto b=bullet(static_cast<std::uint32_t>(slot));b.quad(0x40,constant(0x6d1a70));b.quad(0x48,constant(0x6d19f0));b.word(8,193);b.word(0x34,1);b.word(0x78,first);b.word(0x7c,second);}
        auto value=math.sine(real(3));bullet(first).quad(0x40,m.store(m.add(m.mul(value,m.q(real(7))),m.q(real(8)))));value=math.sine(real(4));bullet(second).quad(0x40,m.store(m.add(m.mul(value,m.q(real(7))),m.q(real(9)))));
    }
    real(8)=m.store(m.add(m.q(real(8)),m.c(0x6d1a08)));real(9)=m.store(m.sub(m.q(real(9)),m.c(0x6d1a08)));real(3)=m.store(m.add(m.q(real(3)),m.q(real(5))));real(4)=m.store(m.add(m.q(real(4)),m.q(real(6))));real(5)=m.store(m.add(m.q(real(5)),m.c(0x6d2e00)));real(7)=m.store(m.add(m.q(real(7)),m.c(0x6b2620)));real(10)=m.store(m.add(m.q(real(10)),m.c(0x6d2df8)));
    if(frame()>=5200){real(5)=m.store(m.add(m.q(real(5)),m.c(0x6d2df0)));if(frame()>=5500)real(6)=m.store(m.add(m.q(real(6)),m.c(0x6d2de8)));}
}
}}
