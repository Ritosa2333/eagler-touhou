#pragma once
#include "core/stages/ExBossMiddleTimelineEngine.hpp"
namespace kaikeidou { namespace ex_boss_middle_timeline_detail {
inline void Engine::phase12(){
    if(t.frame==40){target();services.configure_phase(0,60,extra.boss_hp_multiplier*12000u,0xfffffff6u);integer(0)=random.integer_bits(1,precision,true)*2u-1u;const auto a=random.real_number(constant(0x6d1c60),precision);real(0)=m.store(m.mul(a,m.c(0x6d1950)));effect_at(10,{{255,0,255}},m.store(m.i(integer(0)*90u+192u)),constant(0x6d1a40));costume(3);}
    if(frame()<80)return;
    if(remainder(t.frame,150)==80){
        const auto parent=static_cast<std::uint32_t>(allocate_projectile(pool,{26,1,{{255,0,255}},2,255,0,41}));if(parent!=0xffffffffu){auto b=bullet(parent);
            b.quad(0x40,m.store(m.i(integer(0)*90u+192u)));b.quad(0x48,constant(0x6d1d40));b.word(8,121);b.quad(0x60,m.store(m.add(m.mul(m.i(integer(0)),m.q(real(0))),m.c(0x6d1ba0))));b.quad(0x58,constant(0x6d1a00));b.word(0x78,integer(0));b.quad(0xe8,constant(0x6d2ec8));b.word(0x34,1);
        }
        for(unsigned i=0;i<8;++i)for(unsigned j=0;j<3;++j){
            const auto slot=allocate_projectile(pool,{1,2,{{255,0,255}},0,255,0,41});if(slot==-1)continue;auto b=bullet(static_cast<std::uint32_t>(slot));b.word(0x78,parent);b.word(0x7c,i);b.quad(0xa0,m.store(m.add(m.mul(m.i(b.word(0x7c)),m.c(0x6d22d8)),m.c(0x6d1b60))));b.quad(0xa8,m.store(m.add(m.mul(m.i(j),m.c(0x6d1a40)),m.c(0x6d1b38))));b.quad(0x60,m.store(m.add(m.q(bullet(b.word(0x78)).quad(0x60)),m.q(b.quad(0xa0)))));
            const auto px=bullet(b.word(0x78));auto value=math.cosine(b.quad(0x60));b.quad(0x40,m.store(m.add(m.mul(value,m.q(b.quad(0xa8))),m.q(px.quad(0x40)))));
            const auto py=bullet(b.word(0x78));value=math.sine(b.quad(0x60));b.quad(0x48,m.store(m.add(m.mul(value,m.q(b.quad(0xa8))),m.q(py.quad(0x48)))));b.word(8,118);b.word(0x34,1);
        }
        integer(0)=0u-integer(0);
    }
    real(0)=m.store(m.add(m.q(real(0)),m.c(0x6d2500)));if(remainder(t.frame,300)==79)move_target(0x6d1d50,0x6b25b8);
}
}}
