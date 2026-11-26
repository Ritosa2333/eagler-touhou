#pragma once
#include "core/stages/ExBossLateTimelineEngine.hpp"
namespace kaikeidou { namespace ex_boss_late_timeline_detail {
inline void Engine::phase17_marisa(){
    if(t.frame==200){ex_boss_late_timeline_name(context.spell_name,0x6c5f04);services.configure_phase(1,62,0,147);enemy().word(8,0);}
    else if(t.frame==300){effect_at(13,{{0,0,0}},player.quad(0x90),player.quad(0x98));integer(0)=1;real(4)=constant(0x6d1af8);}
    if(frame()<340)return;
    if(remainder(t.frame,340)<=27){
        if(remainder(t.frame,340)==0){real(0)=player.quad(0x90);real(1)=player.quad(0x98);const auto dx=m.store(m.sub(m.c(0x6d1a60),m.q(player.quad(0x90)))),dy=m.store(m.sub(m.c(0x6d1af0),m.q(player.quad(0x98))));real(2)=m.store(math.atan2(dy,dx));real(3)=0;integer(0)=0u-integer(0);real(4)=m.store(m.sub(m.q(real(4)),m.c(0x6d1a40)));}
        std::uint32_t last=0;for(unsigned i=0;i<4;++i){const auto radius=m.store(m.i(i*30u+160u));const auto slot=allocate_projectile(pool,{14,2,{{0,0,0}},0,255,0,41});last=static_cast<std::uint32_t>(slot);if(slot!=-1){auto b=bullet(static_cast<std::uint32_t>(slot));b.word(8,179);b.quad(0xa0,0);
                auto value=math.cosine(real(2));b.quad(0x40,m.store(m.add(m.mul(value,m.q(radius)),m.q(real(0)))));value=math.sine(real(2));b.quad(0x48,m.store(m.add(m.mul(value,m.q(radius)),m.q(real(1)))));
                const auto dx=m.store(m.sub(m.q(real(0)),m.q(b.quad(0x40)))),dy=m.store(m.sub(m.q(real(1)),m.q(b.quad(0x48))));value=math.atan2(dy,dx);b.quad(0x60,m.store(m.add(value,m.mul(m.sub(m.c(0x6d1920),m.mul(m.i(i),m.c(0x6d1960))),m.i(integer(0))))));
                value=math.cosine(b.quad(0x60));b.quad(0x50,m.store(m.mul(value,m.c(0x6d19d8))));value=math.sine(b.quad(0x60));b.quad(0x58,m.store(m.mul(value,m.c(0x6d19d8))));b.word(0xc,static_cast<std::uint32_t>(remainder(t.frame,340)-28));
            }
            integer(0)=0u-integer(0);
        }
        for(unsigned i=0;i<3;++i){const auto slot=allocate_projectile(pool,{31,1,{{0,0,0}},0,255,1,41});if(slot==-1)continue;auto b=bullet(static_cast<std::uint32_t>(slot));b.word(8,180);b.quad(0xa0,0);
            const auto positive=random.integer_bits(1,precision,true)!=0;auto value=random.real_number(constant(0x6b25b8),precision);value=m.add(m.mul(value,m.c(0x6a7888)),m.c(0x6d1cf8));if(!positive)value.negative=!value.negative;b.quad(0xa8,m.store(value));
            value=math.cosine(real(2));b.quad(0x40,m.store(m.add(m.mul(value,m.add(m.i(i*60u),m.q(real(4)))),m.q(real(0)))));value=math.sine(real(2));b.quad(0x48,m.store(m.add(m.mul(value,m.add(m.i(i*60u),m.q(real(4)))),m.q(real(1)))));
            value=math.cosine(bullet(last).quad(0x60));value.negative=!value.negative;b.quad(0x50,m.store(m.mul(value,m.c(0x6d1900))));value=math.sine(bullet(last).quad(0x60));value.negative=!value.negative;b.quad(0x58,m.store(m.mul(value,m.c(0x6d1900))));
            value=random.real_number(constant(0x6d1c60),precision);b.quad(0x60,m.store(m.mul(value,m.c(0x6d1950))));b.word(0xc,static_cast<std::uint32_t>(remainder(t.frame,340)-28));b.word(0x3c,1);b.word(0x34,1);
        }
        real(2)=m.store(m.add(m.q(real(2)),m.c(0x6d2de0)));
    }else if(remainder(t.frame,340)==300)effect_at(13,{{0,0,0}},player.quad(0x90),player.quad(0x98));
}
}}
