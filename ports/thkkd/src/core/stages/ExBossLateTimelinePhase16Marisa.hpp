#pragma once
#include "core/stages/ExBossLateTimelineEngine.hpp"
namespace kaikeidou { namespace ex_boss_late_timeline_detail {
inline void Engine::phase16_marisa(){
    const auto initialize=[&](std::uint32_t slot,unsigned state,unsigned kind,unsigned blend,std::array<std::uint32_t,3> color){auto b=bullet(slot);b.word(0,state);b.word(0xc,0);b.word(0x10,0);b.word(0x14,0);b.word(4,kind);b.word(0x2c,blend);b.word(0x30,255);b.word(0x34,1);b.word(0x3c,0);b.quad(0x70,0x3ff0000000000000ull);b.word(0x38,slot);ex_boss_late_timeline_color(pool,slot,color[0],color[1],color[2]);pool.sound_triggers[41]=1;};
    const auto tracked=[&](std::uint32_t slot){auto b=bullet(slot);b.word(0x78,0xffffffffu);b.word(0x7c,0);b.word(0x80,1);b.word(0x84,1);b.word(0x88,0);const Bullet source{pool,integer(0)*8u-400u};b.quad(0xa0,source.quad(0));};
    if(frame()>=60&&frame()<=339){
        if(frame()<=109)real(11)=m.store(m.add(m.q(real(11)),m.c(0x6d2410)));else if(frame()>=290)real(10)=m.store(m.add(m.q(real(10)),m.c(0x6d2410)));
        for(unsigned i=0;i<=10;++i){const auto angle=m.store(m.sub(m.mul(m.i(i),m.c(0x6d27b0)),m.c(0x6d1b60)));const auto cosine=m.store(math.cosine(angle)),sine=m.store(math.sine(angle));
            const auto y2=ex_boss_late_timeline_convert(m.mul(m.q(sine),m.q(real(11))))+166u;const auto x2=ex_boss_late_timeline_convert(m.mul(m.q(cosine),m.q(real(11))))+224u;const auto y1=ex_boss_late_timeline_convert(m.mul(m.q(sine),m.q(real(10))))+166u;const auto x1=ex_boss_late_timeline_convert(m.mul(m.q(cosine),m.q(real(10))))+224u;services.draw_line(x1,y1,x2,y2,0xffff,1);
        }
    }
    if(t.frame==1){target(0x6d1cc0,0x6d1a40);real(10)=0;real(11)=0;return;}
    if(t.frame==110){ex_boss_late_timeline_name(context.spell_name,0x6c5e94);services.configure_phase(1,80,0,144);return;}
    if(t.frame==200){effect_at(10,{{0,255,255}},constant(0x6d1a60),constant(0x6d1cc0));costume(3);return;}
    if(t.frame==240){constexpr std::array<std::uint32_t,6> speeds{{0x6d1f68,0x6d26f0,0x6d2958,0x6d2e40,0x6d2480,0x6d1ce0}};for(unsigned kind:{2u,1u})for(auto speed:speeds)ex_boss_late_timeline_emit(pool,{{kind,0,{{0,255,255}},2,255,1,41},constant(0x6d1a60),constant(0x6d1cc0),11,constant(0x6d27b0),constant(speed),constant(0x6d1de0),1,0},player,p,random,precision,math);return;}
    if(t.frame==245){enemy().word(8,0);enemy().quad(0x38,constant(0x6d2540));enemy().quad(0x40,constant(0x6d2540));return;}
    if(frame()>=280&&frame()<=349){ex_boss_late_timeline_ease(real(1),constant(0x6d1af0),constant(0x6d1a40),m.precision);return;}if(frame()<350)return;
    if(t.frame==350){real(0)=constant(0x6d1a60);real(1)=constant(0x6d1af0);effect_at(10,{{0,255,255}},real(0),real(1));costume(3);return;}
    if(t.frame==390){initialize(0,2,30,2,{{0,127,127}});for(unsigned i=0;i<6;++i){bullet(0).word(0x78+i*4u,0);bullet(1).word(0x78+i*4u,0);}bullet(0).quad(0x40,real(0));bullet(0).quad(0x48,real(1));bullet(0).word(8,170);integer(0)=0;return;}
    if(t.frame==410){for(unsigned i=0;i<6;++i){bullet(0).word(0x78+i*4u,1);bullet(0).quad(0xa0+i*8u,0);}return;}
    if(t.frame==720){for(unsigned i=0;i<6;++i){bullet(1).word(0x78+i*4u,1);bullet(1).quad(0xa0+i*8u,constant(0x6d2e38));}return;}
    if(t.frame==760){effect_at(10,{{64,255,0}},real(0),real(1));integer(0)=99;return;}
    if(frame()>=800&&frame()<=1150){if(remainder(t.frame,50)==0){++integer(0);const auto slot=integer(0);initialize(slot,1,2,2,{{32,200,0}});tracked(slot);constexpr std::array<std::uint32_t,8> radii{{0x6d1aa8,0x6d1c48,0x6d23e8,0x6d2e30,0x6d1cd8,0x6d2e28,0x6d2920,0x6d2680}};for(unsigned i=0;i<8;++i)if(t.frame==800u+i*50u){bullet(slot).quad(0xa8,constant(radii[i]));break;}bullet(slot).quad(0x40,real(0));bullet(slot).quad(0x48,real(1));bullet(slot).word(8,25);}return;}
    if(frame()>=1600&&frame()<=2600){
        if(t.frame==1600)effect_at(10,{{255,0,0}},real(0),real(1));
        else if(frame()>=1640&&frame()<=1899){if(remainder(t.frame,6)==0){++integer(0);const auto slot=integer(0);initialize(slot,2,3,0,{{255,0,0}});tracked(slot);bullet(slot).quad(0xa8,constant(0x6d1ff0));bullet(slot).quad(0x40,real(0));bullet(slot).quad(0x48,real(1));bullet(slot).word(8,25);}}
        else if(t.frame==2100)effect_at(10,{{0,0,255}},real(0),real(1));
        else if(frame()>=2140&&frame()<=2399&&remainder(t.frame,6)==0){++integer(0);const auto slot=integer(0);initialize(slot,2,3,0,{{0,0,255}});tracked(slot);bullet(slot).quad(0xa8,constant(0x6d1d00));bullet(slot).quad(0x40,real(0));bullet(slot).quad(0x48,real(1));bullet(slot).word(8,25);}return;
    }
    if(frame()<3000)return;
    const auto select_ring=[&](){const auto dx=m.sub(m.q(real(0)),m.q(player.quad(0x90))),dy=m.sub(m.q(real(1)),m.q(player.quad(0x98)));const auto distance=m.store(ex_boss_late_timeline_sqrt(m.store(m.add(m.mul(dx,dx),m.mul(dy,dy))),m.precision));constexpr std::array<std::uint32_t,5> bounds{{0x6d1a90,0x6d1d00,0x6d18c0,0x6d1ab0,0x6d1d88}};for(unsigned i=0;i<5;++i)if(compare(m.c(bounds[i]),m.q(distance))>=0)return i;return 5u;};
    if(remainder(t.frame,200)<=39){if(remainder(t.frame,200)==0)effect_at(10,{{255,0,255}},real(0),real(1));const auto ring=select_ring();for(unsigned i=0;i<6;++i)bullet(0).word(0x78+i*4u,i==ring?2u:1u);}
    else if(remainder(t.frame,200)==40){const auto ring=select_ring();bullet(0).word(0x78+ring*4u,2);const auto direction=m.store(m.i(remainder(integer(0),2)!=0?80u:0xffffffb0u));const auto dx=m.store(m.sub(m.q(player.quad(0x90)),m.q(real(0)))),dy=m.store(m.sub(m.q(player.quad(0x98)),m.q(real(1))));auto angle=m.store(math.atan2(dy,dx));
        for(unsigned i=0;i<3;++i){if(i)angle=m.store(m.add(m.q(angle),m.c(0x6d2138)));++integer(0);const auto slot=integer(0);initialize(slot,2,1,0,{{127,0,127}});auto b=bullet(slot);b.word(0x78,0xffffffffu);b.word(0x7c,ring);b.word(0x80,0);b.word(0x84,1);b.word(0x88,0);b.quad(0xa0,m.store(ex_boss_late_timeline_degrees(angle,m.precision)));b.quad(0xa8,direction);b.quad(0x40,real(0));b.quad(0x48,real(1));auto value=math.cosine(angle);b.quad(0x50,m.store(m.add(value,value)));value=math.sine(angle);b.quad(0x58,m.store(m.add(value,value)));b.word(8,25);}
    }
}
}}
