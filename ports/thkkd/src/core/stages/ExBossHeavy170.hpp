#pragma once
#include "core/stages/ExBossHeavyEngine.hpp"
namespace kaikeidou { namespace ex_boss_heavy_detail {
inline void Engine::normal170(){
    const auto circle=[&](Number radius,std::uint32_t color){const auto r=ex_boss_heavy_integer(radius);const auto y=ex_boss_heavy_integer(m.q(at(0).quad(0x48)))+16u;const auto x=ex_boss_heavy_integer(m.q(at(0).quad(0x40)))+32u;services.draw_circle(x,y,r,color,0);};
    const auto line=[&](std::uint64_t angle,Number inner,Number outer){auto value=math.sine(angle);const auto y2=ex_boss_heavy_integer(m.add(m.mul(value,outer),m.q(at(0).quad(0x48))))+16u;value=math.cosine(angle);const auto x2=ex_boss_heavy_integer(m.add(m.mul(value,outer),m.q(at(0).quad(0x40))))+32u;value=math.sine(angle);const auto y1=ex_boss_heavy_integer(m.add(m.mul(value,inner),m.q(at(0).quad(0x48))))+16u;value=math.cosine(angle);const auto x1=ex_boss_heavy_integer(m.add(m.mul(value,inner),m.q(at(0).quad(0x40))))+32u;services.draw_line(x1,y1,x2,y2,0xffff,1);};
    for(std::uint32_t i=0;i<6;++i){
        const auto flag=0x78u+i*4u,radius=0xa0u+i*8u;
        if(at(0).word(flag)==1){
            const bool growing=compare(m.i(i*40u+80u),m.q(at(0).quad(radius)))>0;
            if(growing)at(0).quad(radius,m.store(m.add(m.q(at(0).quad(radius)),m.c(0x6d1868))));
            circle(m.q(at(0).quad(radius)),growing?0xff00ffu:0xffffu);circle(m.add(m.q(at(0).quad(radius)),m.c(0x6d1868)),growing?0xff00ffu:0xffffu);
        }else if(at(0).word(flag)==2){circle(m.q(at(0).quad(radius)),0xff00ff);circle(m.add(m.q(at(0).quad(radius)),m.c(0x6d1868)),0xff00ff);circle(m.sub(m.q(at(0).quad(radius)),m.c(0x6d1868)),0xff00ff);}
        if(at(1).word(flag)!=1)continue;
        const auto inner=m.i(i*40u+40u),outer=m.i(i*40u+80u);auto angle=m.store(ex_boss_heavy_radians(at(1).quad(radius),m.precision));line(angle,inner,outer);
        constexpr std::array<std::uint32_t,6> rates{{0x6a6100,0x6d19c0,0x6d1f90,0x6d1f88,0x6d1f80,0x6d1f78}};
        at(1).quad(radius,m.store(i&1u?m.sub(m.q(at(1).quad(radius)),m.c(rates[i])):m.add(m.q(at(1).quad(radius)),m.c(rates[i]))));
        if(compare(m.c(0x6d1f70),m.q(at(1).quad(radius)))<=0)at(1).quad(radius,m.store(m.sub(m.q(at(1).quad(radius)),m.c(0x6d1f70))));
        else if(compare(m.i(0),m.q(at(1).quad(radius)))>0)at(1).quad(radius,m.store(m.add(m.q(at(1).quad(radius)),m.c(0x6d1f70))));
        angle=m.store(ex_boss_heavy_radians(at(1).quad(radius),m.precision));line(angle,inner,outer);
    }
    for(std::uint32_t i=100;signed_word(i)<=signed_word(p.integers[0]);++i){auto c=at(i);
        if(c.word(4)==1&&c.word(0x78)==0xffffffffu){
            c.quad(0x40,m.store(m.add(m.q(c.quad(0x40)),m.q(c.quad(0x50)))));c.quad(0x48,m.store(m.add(m.q(c.quad(0x48)),m.q(c.quad(0x58)))));
            const auto dx=m.sub(m.q(p.reals[0]),m.q(c.quad(0x40))),dy=m.sub(m.q(p.reals[1]),m.q(c.quad(0x48)));const auto square=m.store(m.add(m.mul(dx,dx),m.mul(dy,dy)));const auto distance=ex_boss_heavy_sqrt(square,m.precision);
            if(compare(m.i(c.word(0x7c)*40u+80u),distance)<=0){c.word(0x78,c.word(0x7c));ex_boss_heavy_recolor(pool,i,255,0,255);for(unsigned j=0;j<6;++j)at(0).word(0x78+j*4u,1);}continue;
        }
        if(signed_word(c.word(0x80))>=1){
            Number radius;
            if(signed_word(c.word(0x7c))>signed_word(c.word(0x78))){c.quad(0xa0,at(1).quad(0xa0+c.word(0x7c)*8u));radius=m.add(m.i(c.word(0x78)*40u+80u),m.mul(m.i(c.word(0x80)),m.c(0x6d1ad0)));}
            else{c.quad(0xa0,at(1).quad(0xa0+c.word(0x78)*8u));radius=m.sub(m.i(c.word(0x78)*40u+80u),m.mul(m.i(c.word(0x80)),m.c(0x6d1ad0)));}
            radius=m.q(m.store(radius));const auto angle=m.store(ex_boss_heavy_radians(c.quad(0xa0),m.precision));auto value=math.cosine(angle);c.quad(0x40,m.store(m.add(m.mul(value,radius),m.q(p.reals[0]))));value=math.sine(angle);c.quad(0x48,m.store(m.add(m.mul(value,radius),m.q(p.reals[1]))));
            c.word(0x80,c.word(0x80)+1u);if(c.word(0x80)==30){c.word(0x84,c.word(0x7c)-c.word(0x78));c.word(0x78,c.word(0x7c));c.word(0x80,0);c.word(0x88,30);}continue;
        }
        c.quad(0xa0,m.store(m.add(m.div(m.q(c.quad(0xa8)),m.c(0x6b25b8)),m.q(c.quad(0xa0)))));
        if(compare(m.c(0x6d1f70),m.q(c.quad(0xa0)))<=0)c.quad(0xa0,m.store(m.sub(m.q(c.quad(0xa0)),m.c(0x6d1f70))));else if(compare(m.i(0),m.q(c.quad(0xa0)))>0)c.quad(0xa0,m.store(m.add(m.q(c.quad(0xa0)),m.c(0x6d1f70))));
        const auto angle=m.store(ex_boss_heavy_radians(c.quad(0xa0),m.precision)),radius=m.store(m.i(c.word(0x78)*40u+80u));auto value=math.cosine(angle);c.quad(0x40,m.store(m.add(m.mul(value,m.q(radius)),m.q(p.reals[0]))));value=math.sine(angle);c.quad(0x48,m.store(m.add(m.mul(value,m.q(radius)),m.q(p.reals[1]))));
        if(signed_word(c.word(0x78))<=4&&(c.word(0x84)==1||c.word(0x88)==0)){const auto delta=m.store(m.sub(m.q(c.quad(0xa0)),m.q(at(1).quad(0xa8+c.word(0x78)*8u))));if(compare(m.q(ex_boss_heavy_absolute(delta)),m.c(0x6d1948))<=0){c.word(0x80,1);c.word(0x7c,c.word(0x78)+1u);}}
        if(signed_word(c.word(0x78))>=1&&(c.word(0x84)==0xffffffffu||c.word(0x88)==0)){const auto delta=m.store(m.sub(m.q(c.quad(0xa0)),m.q(at(1).quad(0xa0+c.word(0x78)*8u))));if(compare(m.q(ex_boss_heavy_absolute(delta)),m.c(0x6d1948))<=0){c.word(0x80,1);c.word(0x7c,c.word(0x78)-1u);}}
        if(signed_word(c.word(0x88))>0)c.word(0x88,c.word(0x88)-1u);
    }
}
}}
