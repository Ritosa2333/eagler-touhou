#pragma once
#include "core/stages/ExBossMiddleTimelineEngine.hpp"
#include "core/stages/ExBossMiddleTimelineGrid.hpp"
namespace kaikeidou { namespace ex_boss_middle_timeline_detail {
inline void Engine::phase11_marisa(){
    if(t.frame==1)target(0x6d1d00,0x6b25f8);
    else if(t.frame==100){
        effect(12,{{255,255,255}});integer(0)=0;integer(1)=0;integer(2)=0;bullet(1000).quad(0xe8,constant(0x6d1838));
        for(unsigned x=0;x<8;++x)for(unsigned y=0;y<12;++y)for(unsigned side=0;side<2;++side){const auto slot=y*10u+x+(side?0u:200u);auto b=bullet(slot);
            b.word(0,1);b.word(0xc,0);b.word(0x10,0);b.word(0x14,0);b.word(4,side?27u:28u);b.word(0x2c,0);b.word(0x30,255);b.word(0x34,1);b.word(0x3c,side?0u:1u);b.quad(0x70,0x3ff0000000000000ull);b.word(0x38,slot);
            const auto shade=side?0u:254u;ex_boss_middle_timeline_color(pool,slot,shade,shade,shade);b.word(0x9c,0);b.word(0x78,x);b.word(0x7c,y);
            b.quad(0x40,m.store(m.i(b.word(0x78)*48u-2976u)));b.quad(0x48,m.store(m.add(m.add(m.q(bullet(1000).quad(0xe8)),m.c(0x6d1828)),m.i(b.word(0x7c)*48u))));b.word(8,side?154u:155u);
        }
    }else if(t.frame==240){ex_boss_middle_timeline_name(context.spell_name,0x6c5dcc);services.configure_phase(1,90,extra.boss_hp_multiplier*24000u,138);enemy().quad(0xa8,constant(0x6d28a0));enemy().quad(0xb0,0);enemy().quad(0xb8,constant(0x6d1aa8));enemy().quad(0xc0,constant(0x6b25f8));}
    if(frame()<140)return;
    if(frame()>=320){
        if(t.frame==700){enemy().quad(0xa8,constant(0x6d1df0));enemy().quad(0xb0,constant(0x6d2468));enemy().quad(0xb8,constant(0x6b25b8));enemy().quad(0xc0,constant(0x6b25f8));}
        else if(t.frame==960){effect(12,{{0,0,255}});enemy().quad(0xa8,constant(0x6d1a60));costume(3);}
        else if(frame()>=1000&&remainder(t.frame,60)==0)for(unsigned i=0;i<2;++i){
            ex_boss_middle_timeline_emit(pool,{{12,2,{{0,0,255}},0,255,1,41},enemy().quad(0x38),m.store(m.add(q(0x40),m.c(0x6d1a40))),9,constant(0x6d2290),m.store(m.add(m.mul(m.i(i),m.c(0x6d18e0)),m.c(0x6d1a00))),constant(0x6d1860),0,0},player,p,random,precision,math);
            ex_boss_middle_timeline_emit(pool,{{12,2,{{0,0,255}},0,255,1,41},enemy().quad(0x38),m.store(m.add(q(0x40),m.c(0x6d1a40))),9,constant(0x6d2290),m.store(m.add(m.mul(m.i(i),m.c(0x6d18e0)),m.c(0x6d2548))),0,0,0},player,p,random,precision,math);
        }
        bullet(1000).quad(0xe8,m.store(m.add(m.q(bullet(1000).quad(0xe8)),m.c(0x6d1880))));
        if(compare(m.i(0),m.q(bullet(1000).quad(0xe8)))<=0){
            bullet(1000).quad(0xe8,m.store(m.sub(m.q(bullet(1000).quad(0xe8)),m.c(0x6d1d18))));
            for(unsigned x=0;x<8;++x){for(unsigned y=11;y>=1;--y)bullet(y*10u+x).word(0x9c,bullet((y-1u)*10u+x).word(0x9c));bullet(x).word(0x9c,0);}++integer(2);
            const auto place=[&](std::uint32_t value){for(unsigned i=0;i<3;++i){std::uint32_t x;do{x=random.integer_bits(7,precision,true);}while(bullet(x).word(0x9c)!=0);bullet(x).word(0x9c,value);}};
            const auto residue=remainder(integer(2),6);if(residue<=2){if(residue<=1)place(1);place(0xffffffffu);}else if(residue==4){for(unsigned x=0;x<8;++x)bullet(x).word(0x9c,0xffffffffu);}else if(residue==5){place(1);place(0xffffffffu);}
        }
        if(remainder(t.frame,180)==0)effect_at(13,{{255,255,255}},player.quad(0x90),player.quad(0x98));
        if(remainder(t.frame,180)==40){
            auto x=ex_boss_middle_timeline_convert(m.div(m.q(player.quad(0x90)),m.c(0x6d1d18)));if(signed_word(x)<0)x=0;if(signed_word(x)>7)x=7;
            auto y=ex_boss_middle_timeline_convert(m.div(m.sub(m.q(player.quad(0x98)),m.q(bullet(1000).quad(0xe8))),m.c(0x6d1d18)));if(signed_word(y)<0)y=0;if(signed_word(y)>11)y=11;
            ex_boss_middle_timeline_grid(pool,x,y);pool.sound_triggers[44]=1;
        }
    }
    for(unsigned i=1;i<=8;++i){const auto y2=integer(0)+16u;services.draw_line(i*48u+32u,16,i*48u+32u,y2,0xffffff,1);}
    for(unsigned i=1;i<=10;++i){
        const auto y2=ex_boss_middle_timeline_convert(m.add(m.add(m.add(m.c(0x6d1e20),m.q(bullet(1000).quad(0xe8))),m.c(0x6a6100)),m.i(i*48u)));const auto x2=integer(1)+32u;
        const auto y1=ex_boss_middle_timeline_convert(m.add(m.add(m.add(m.c(0x6d1e20),m.q(bullet(1000).quad(0xe8))),m.c(0x6a6100)),m.i(i*48u)));services.draw_line(32,y1,x2,y2,0xffffff,1);
    }
    if(signed_word(integer(0))<=448)integer(0)+=4;if(signed_word(integer(1))<=384)integer(1)+=4;
}
}}
