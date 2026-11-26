#pragma once
#include "core/stages/ExBossEarlyTimelineEngine.hpp"
namespace kaikeidou { namespace ex_boss_early_timeline_detail {
inline void Engine::phase4_marisa(){
    if(t.frame==1)target(0x6d1a90,0x6b25f8);
    else if(t.frame==150){ex_boss_early_timeline_name(context.spell_name,0x6c5ccc);services.configure_phase(1,60,extra.boss_hp_multiplier*17000u,129);}
    else if(t.frame==250){const auto y=player.quad(0x98),x=player.quad(0x90);effect_at(13,{{0,0,255}},x,y);real(0)=constant(0x6d2d10);costume(3);}
    if(frame()<290)return;
    if(remainder(t.frame,2)==0){
        const auto slot=allocate_projectile(pool,{12,1,{{0,0,255}},0,255,1,41});
        if(slot!=-1){auto b=bullet(static_cast<std::uint32_t>(slot));b.word(8,135);
            auto value=math.cosine(real(0));b.quad(0x50,m.store(m.mul(value,m.c(0x6d1910))));value=math.sine(real(0));b.quad(0x58,m.store(m.mul(value,m.c(0x6d1910))));
            b.quad(0x40,m.store(m.add(m.mul(m.q(b.quad(0x50)),m.c(0x6d1e20)),m.q(player.quad(0x90)))));b.quad(0x48,m.store(m.add(m.mul(m.q(b.quad(0x58)),m.c(0x6d1e20)),m.q(player.quad(0x98)))));
            const auto dx=m.store(m.sub(m.q(b.quad(0x40)),m.q(player.quad(0x90))));const auto dy=m.store(m.sub(m.q(b.quad(0x48)),m.q(player.quad(0x98))));b.quad(0x60,m.store(math.atan2(dy,dx)));
            value=m.q(b.quad(0x50));value.negative=!value.negative;b.quad(0xa0,m.store(m.mul(value,m.c(0x6d1c18))));value=m.q(b.quad(0x58));value.negative=!value.negative;b.quad(0xa8,m.store(m.mul(value,m.c(0x6d1c18))));
            b.word(0x78,30);b.word(0x7c,0);b.word(0x80,remainder(t.frame,8)==0?1u:0u);
        }
        real(0)=m.store(m.add(m.q(real(0)),m.c(0x6d1c10)));
    }
    if(remainder(t.frame,200)==80){auto value=random.real_number(constant(0x6d1f60),precision);put(0xa8,m.add(value,m.c(0x6d25b0)));value=random.real_number(constant(0x6d1af8),precision);put(0xb0,m.add(m.mul(value,m.c(0x6d1960)),m.c(0x6d1a90)));}
}
}}
