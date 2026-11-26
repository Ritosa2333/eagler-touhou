#pragma once
#include "core/stages/ExBossLateTimelineEngine.hpp"
namespace kaikeidou { namespace ex_boss_late_timeline_detail {
inline void Engine::phase16_sanae(){
    if(t.frame==1)target(0x6d1cc0,0x6d1a40);
    else if(t.frame==60){presentation.entry_state=4;real(0)=constant(0x6d1a60);real(1)=constant(0x6d1cc0);real(2)=0x3ff0000000000000ull;real(3)=0x3ff0000000000000ull;real(4)=constant(0x6d1a78);integer(0)=1;integer(1)=20;integer(2)=integer(1);}
    else if(frame()>=61&&frame()<=299){put(0x38,m.add(m.mul(m.i(integer(0)),m.q(real(2))),m.c(0x6d1a60)));if(--integer(2)==0){real(2)=m.store(m.add(m.q(real(2)),m.q(real(3))));real(3)=m.store(m.add(m.q(real(3)),m.c(0x6d2008)));integer(0)=0u-integer(0);if(signed_word(integer(1))>=2){--integer(1);integer(2)=integer(1);}else integer(2)=1;}costume(3);ex_boss_late_timeline_ease(real(0),constant(0x6d1a60),real(4),m.precision);ex_boss_late_timeline_ease(real(1),constant(0x6d2e20),real(4),m.precision);ex_boss_late_timeline_ease(real(4),constant(0x6d18c0),constant(0x6d1a88),m.precision);}
    else if(t.frame==300){enemy().word(8,0);enemy().quad(0x38,constant(0x6d2540));enemy().quad(0x40,constant(0x6d2540));}
    else if(t.frame==350){ex_boss_late_timeline_name(context.spell_name,0x6c5ec4);services.configure_phase(1,100,0,145);integer(0)=0xffffffffu;integer(1)=0;auto value=random.real_number(constant(0x6d1c60),precision);integer(3)=ex_boss_late_timeline_convert(m.mul(value,m.c(0x6d1950)));value=random.real_number(constant(0x6d1c60),precision);integer(4)=ex_boss_late_timeline_convert(m.mul(value,m.c(0x6d1950)));
        constexpr std::array<std::array<std::uint32_t,3>,6> colors{{{{255,0,0}},{{0,255,0}},{{0,0,255}},{{0,255,255}},{{255,0,255}},{{255,255,0}}}};
        for(unsigned i=2000;i<=2005;++i){auto b=bullet(i);b.word(0,1);b.word(0xc,0);b.word(0x10,0);b.word(0x14,0);b.word(4,0);b.word(0x2c,1);b.word(0x30,0);b.word(0x34,1);b.word(0x3c,1);b.quad(0x70,0x3ff0000000000000ull);b.word(0x38,i);const auto color=colors[i-2000];ex_boss_late_timeline_color(pool,i,color[0],color[1],color[2]);if(i==2003){b.word(0x84,200);b.word(0x88,400);}else if(i==2004){b.word(0x84,400);b.word(0x88,0);}else if(i==2005){b.word(0x84,0);b.word(0x88,200);}pool.sound_triggers[40]=1;b.word(0x78,0);b.word(0x7c,(i-2000u)*200u);b.word(8,171);}
    }
    if(frame()<360)return;
    if(frame()<=3200){if(remainder(t.frame,480)==360){
            constexpr std::array<std::uint32_t,6> first{{0x6b25f8,0x6b25f8,0x6d1a90,0x6d1a60,0x6d2c58,0x6d2110}},second{{0x6d2288,0x6d2e18,0x6d2110,0x6d1a60,0x6d2c08,0x6d1a90}};
            if(integer(1)<6){real(2)=constant(first[integer(1)]);real(3)=constant(0x6d1a80);integer(2)=integer(1)==5?67u:100u;if(integer(1)<=1)bullet(2000).quad(0xa0,0);}
            auto a=bullet(2000);a.word(0x78,1);a.word(0x80,0xffffffffu);a.word(0x84,integer(1));a.word(0x8c,integer(2));a.word(0x9c,ex_boss_late_timeline_convert(m.q(real(2))));a.word(0x98,0);a.quad(0x40,real(2));a.quad(0x48,real(3));a.word(0xc,0);
            if(integer(1)<6){real(2)=constant(second[integer(1)]);real(3)=constant(0x6d1a80);if(integer(1)<=1)bullet(2002).quad(0xa0,0);}
            auto b=bullet(2002);b.word(0x78,1);b.word(0x80,1);b.word(0x84,0u-integer(1));b.word(0x8c,integer(2));b.word(0x9c,ex_boss_late_timeline_convert(m.q(real(2))));b.word(0x98,0);b.quad(0x40,real(2));b.quad(0x48,real(3));b.word(0xc,0);
            if(integer(1)<6){real(2)=constant(0x6d1a60);real(3)=constant(0x6d1a80);}auto c=bullet(2004);c.word(0x78,2);c.word(0x8c,integer(2));c.word(0x9c,ex_boss_late_timeline_convert(m.q(real(2))));c.word(0x98,0);c.quad(0x40,real(2));c.quad(0x48,real(3));c.word(0xc,0);++integer(1);
        }return;
    }
    if(frame()<=4400){phase16_sanae_moves();return;}phase16_sanae_final();
}
}}
