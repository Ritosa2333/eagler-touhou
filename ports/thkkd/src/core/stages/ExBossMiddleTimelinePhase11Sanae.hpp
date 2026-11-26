#pragma once
#include "core/stages/ExBossMiddleTimelineEngine.hpp"
namespace kaikeidou { namespace ex_boss_middle_timeline_detail {
inline void Engine::phase11_sanae(){
    const auto aim=[&](){const auto dx=m.store(m.sub(m.q(player.quad(0x90)),q(0x38))),dy=m.store(m.sub(m.q(player.quad(0x98)),q(0x40)));real(0)=m.store(math.atan2(dy,dx));};
    const auto shape=[&](unsigned layer){
        const std::array<std::uint32_t,3> color=layer==0?std::array<std::uint32_t,3>{{0,0,255}}:layer==1?std::array<std::uint32_t,3>{{0,255,0}}:std::array<std::uint32_t,3>{{255,0,0}};
        constexpr std::array<std::uint32_t,6> radii{{0x6d2d08,0x6d2d08,0x6d2cf0,0x6d2cf0,0x6d2cd8,0x6d2cd8}},angles{{0x6d2d00,0x6d2cf8,0x6d2ce8,0x6d2ce0,0x6d1990,0x6d2cd0}};
        for(unsigned i=0;i<10;++i){const auto slot=allocate_projectile(pool,{11,1,color,0,255,0,41});if(slot==-1)continue;auto b=bullet(static_cast<std::uint32_t>(slot));
            b.quad(0xa0,i<6?constant(radii[i]):m.store(m.i((i-5u)*26u)));b.quad(0xa8,i<6?constant(angles[i]):0);b.quad(0xb0,layer==0?constant(0x6d2d10):layer==1?0:constant(0x6d2138));
            auto value=math.cosine(m.store(m.add(m.add(m.q(real(0)),m.q(b.quad(0xb0))),m.q(b.quad(0xa8)))));b.quad(0x40,m.store(m.add(m.mul(value,m.q(b.quad(0xa0))),q(0x38))));
            value=math.sine(m.store(m.add(m.add(m.q(real(0)),m.q(b.quad(0xb0))),m.q(b.quad(0xa8)))));b.quad(0x48,m.store(m.add(m.mul(value,m.q(b.quad(0xa0))),q(0x40))));b.word(8,156);b.word(0x34,1);
        }
    };
    if(t.frame==1)target(0x6d1d00,0x6b25f8);
    else if(t.frame==80){ex_boss_middle_timeline_name(context.spell_name,0x6c5dec);services.configure_phase(1,90,extra.boss_hp_multiplier*20000u,139);}
    else if(t.frame==180){effect(12,{{0,0,255}});costume(3);}
    else if(t.frame==220){aim();real(1)=constant(0x6d1e40);real(2)=constant(0x6d2ee0);real(3)=constant(0x6d2ed8);integer(0)=0xffffffffu;shape(0);}
    else if(t.frame==230)effect(12,{{0,255,0}});
    else if(t.frame==270)shape(1);
    else if(t.frame==280)effect(12,{{255,0,0}});
    else if(t.frame==320)shape(2);
    else if(frame()>=340&&remainder(t.frame,2)==0){
        real(2)=m.store(m.add(m.q(real(2)),m.c(0x6d1a18)));auto value=math.cosine(real(2));auto x=m.store(m.add(m.mul(value,m.q(real(3))),m.c(0x6d1a60)));
        value=math.sine(real(2));auto y=m.store(m.add(m.mul(value,m.q(real(3))),m.c(0x6d1af0)));
        if(compare(m.i(0),m.q(x))>0)x=0;else if(compare(m.c(0x6d1cb0),m.q(x))<0)x=constant(0x6d1cb0);
        if(compare(m.i(0),m.q(y))>0)y=0;else if(compare(m.c(0x6d19a0),m.q(y))<0)y=constant(0x6d19a0);
        ++integer(0);const auto layer=signed_word(integer(0))<=3?0u:signed_word(integer(0))>=4&&signed_word(integer(0))<=7?1u:2u;if(layer==2&&integer(0)==11)integer(0)=0xffffffffu;
        const std::array<std::uint32_t,3> color=layer==0?std::array<std::uint32_t,3>{{0,0,128}}:layer==1?std::array<std::uint32_t,3>{{0,128,0}}:std::array<std::uint32_t,3>{{128,0,0}};
        const auto slot=allocate_projectile(pool,{0,3,color,0,255,1,41});if(slot!=-1){auto b=bullet(static_cast<std::uint32_t>(slot));b.quad(0x40,x);b.quad(0x48,y);b.quad(0xa0,layer==0?constant(0x6d2d10):layer==1?0:constant(0x6d2138));b.word(8,157);}
    }
    if(frame()>=220)aim();
    if(frame()>=600&&remainder(t.frame,400)==200){
        auto value=random.real_number(constant(0x6b25b8),precision);put(0xa8,m.add(value,m.c(0x6d2108)));
        const bool below=compare(m.c(0x6d1af0),q(0x40))<0;value=random.real_number(constant(0x6d1ae8),precision);enemy().quad(0xb0,m.store(below?m.add(value,m.c(0x6d1aa8)):m.sub(m.c(0x6d2ed0),value)));enemy().quad(0xb8,constant(0x6d2240));enemy().quad(0xc0,constant(0x6d2240));
    }
}
}}
