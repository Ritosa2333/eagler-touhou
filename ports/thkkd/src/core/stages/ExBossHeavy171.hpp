#pragma once
#include "core/stages/ExBossHeavyEngine.hpp"
namespace kaikeidou { namespace ex_boss_heavy_detail {
inline void Engine::normal171(){switch(b().word(0x78)){case 1:mode1();break;case 2:mode2();break;case 3:mode3();break;case 4:mode4();break;case 5:mode5();break;default:break;}}
inline void Engine::mode1(){
    vertical_line();
    if(b().word(12)==0)effect();
    else if(age()>=40&&age()<=signed_word(b().word(0x8c)+40u)){
        const auto variant=signed_word(b().word(0x84));put(0x48,m.add(q(0x48),m.c(variant==5||variant==-5?0x6d1f40:0x6d1f68)));
        if(variant==1){if(compare(q(0x48),m.c(0x6d1ab0))>=0&&compare(m.c(0x6d1f60),q(0x48))>=0)put(0xa0,m.add(q(0xa0),m.c(0x6d1f58)));}
        else if(variant==-1)put(0xa0,m.sub(q(0xa0),m.c(0x6d1f50)));
        else if(variant==2){const auto value=math.cosine(m.store(m.mul(m.i(b().word(12)),m.c(0x6d1f48))));put(0xa0,m.mul(value,m.c(0x6d1b38)));}
        else if(variant==-2){const auto value=math.sine(m.store(m.add(m.mul(m.i(b().word(12)),m.c(0x6d1990)),m.c(0x6d1960))));put(0xa0,m.mul(value,m.c(0x6d1a90)));}
        else if(variant==3||variant==-3){auto value=math.cosine(m.store(m.mul(m.i(b().word(12)),m.c(0x6d1900))));if(variant<0)value.negative=!value.negative;put(0xa0,m.mul(value,m.c(0x6b25f8)));}
        else if(variant==4||variant==-4){const auto angle=m.store(m.mul(m.i(b().word(12)),m.c(0x6d1960)));auto value=variant==4?math.sine(angle):math.cosine(angle);value=m.q(ex_boss_heavy_absolute_direct(m.store(value)));if(variant==4)value.negative=!value.negative;put(0xa0,m.mul(value,m.c(0x6d1ae8)));}
        else if(variant==5||variant==-5){const auto value=random.real_number(constant(0x6d1f38),precision);put(0xa0,m.sub(m.mul(value,m.c(0x6d1960)),m.c(0x6d1a90)));}
        const auto slot=b().word(12)+b().word(0x7c)-40u;initialize(slot,21);auto c=at(slot);c.word(0x78,b().word(0x80));c.word(0x7c,0);c.quad(0xa0,b().quad(0xa0));c.quad(0x40,m.store(m.add(q(0x40),q(0xa0))));c.quad(0x48,b().quad(0x48));c.quad(0x50,0);c.word(8,172);
    }
    if(b().word(12)==b().word(0x8c)+190u)b().word(0x78,0);
}
inline void Engine::mode2(){
    vertical_line();if(b().word(12)==150){effect();return;}if(age()<190)return;
    const auto slot=b().word(12)+b().word(0x7c)-190u;initialize(slot,9,b().word(12)-290u);auto c=at(slot);
    c.word(0x7c,slot-b().word(0x7c)+b().word(0x84));c.word(0x80,slot-b().word(0x7c)+b().word(0x88));c.quad(0x40,m.store(m.add(m.add(m.q(at(c.word(0x7c)).quad(0xa0)),m.q(at(c.word(0x80)).quad(0xa0))),q(0x40))));c.quad(0x48,at(c.word(0x7c)).quad(0x48));c.quad(0x58,0);c.word(8,173);
    if(b().word(12)==b().word(0x8c)+190u)b().word(0x78,0);at(c.word(0x7c)).word(0x3c,0);at(c.word(0x80)).word(0x3c,0);
}
inline void Engine::mode3(){
    horizontal_line();if(b().word(12)==0)effect();else if(age()>=40&&age()<=signed_word(b().word(0x8c)+40u)){
        const auto variant=signed_word(b().word(0x84));
        if(variant==6){put(0x40,m.add(q(0x40),m.c(0x6d1910)));b().quad(0xa0,0);}
        else if(variant==-6){put(0x40,m.sub(q(0x40),m.c(0x6d1910)));put(0xa0,m.i((static_cast<std::uint32_t>(remainder(b().word(12),11))-5u)*3u));}
        else if(variant==7){put(0x40,m.add(q(0x40),m.c(0x6d1910)));const auto angle=m.store(m.sub(m.mul(m.i(b().word(12)),m.c(0x6d19b8)),m.i(p.integers[3])));const auto value=math.sine(angle);put(0xa0,m.mul(value,m.c(0x6d1f30)));}
        else if(variant==-7){put(0x40,m.sub(q(0x40),m.c(0x6d1910)));const auto angle=m.store(m.add(m.mul(m.i(b().word(12)),m.c(0x6d1f28)),m.i(p.integers[4])));const auto value=math.sine(angle);put(0xa0,m.mul(value,m.c(0x6d1d50)));}
        else if(variant==8||variant==-8){put(0x40,variant==8?m.add(q(0x40),m.c(0x6d1910)):m.sub(q(0x40),m.c(0x6d1910)));const auto value=random.real_number(constant(variant==8?0x6d1e50:0x6d1f20),precision);put(0xa0,m.sub(m.mul(value,m.c(0x6d1960)),m.c(variant==8?0x6d1a88:0x6d1aa8)));}
        else put(0x48,m.add(q(0x48),m.c(0x6d1f68)));
        const auto slot=signed_word(b().word(0x84))>0?b().word(12)+b().word(0x7c)-40u:83u-(b().word(12)-40u)+b().word(0x7c);initialize(slot,21);auto c=at(slot);c.word(0x78,b().word(0x80));c.word(0x7c,0);c.quad(0xa0,b().quad(0xa0));c.quad(0x40,b().quad(0x40));c.quad(0x48,m.store(m.add(q(0x48),q(0xa0))));c.quad(0x58,0);c.word(8,174);
    }if(b().word(12)==b().word(0x8c)+40u)b().word(0x78,0);
}
inline void Engine::mode4(){
    horizontal_line();if(b().word(12)==0){effect();return;}if(age()<40)return;
    const auto slot=signed_word(b().word(0x90))>0?b().word(12)+b().word(0x7c)-40u:83u-(b().word(12)-40u)+b().word(0x7c);initialize(slot,9,b().word(12)-140u);auto c=at(slot);
    const auto prior=remainder(b().word(0x94),8);b().word(0x94,b().word(0x94)+1u);c.word(0x78,prior<=3?1u:0xffffffffu);c.word(0x7c,slot-b().word(0x7c)+b().word(0x84));c.word(0x80,slot-b().word(0x7c)+b().word(0x88));c.quad(0x40,at(c.word(0x7c)).quad(0x40));c.quad(0x48,m.store(m.add(m.add(m.q(at(c.word(0x7c)).quad(0xa0)),m.q(at(c.word(0x80)).quad(0xa0))),q(0x48))));c.quad(0x58,0);c.word(8,175);
    if(b().word(12)==b().word(0x8c)+40u)b().word(0x78,0);at(c.word(0x7c)).word(0x3c,0);
}
inline void Engine::mode5(){
    vertical_line();if(b().word(12)==0){effect();return;}if(age()<40||age()>signed_word(b().word(0x8c)+40u))return;
    put(0x48,m.add(q(0x48),m.c(0x6d1f68)));const auto slot=b().word(12)+b().word(0x7c)-40u;initialize(slot,0);auto c=at(slot);c.quad(0xa0,b().quad(0xa0));c.quad(0xa8,0);c.quad(0xb0,b().quad(0x40));c.quad(0xb8,constant(0x6d1af8));c.quad(0xc0,constant(b().word(0x80)==1?0x6d1ab8:b().word(0x80)==0?0x6d1f18:0x6d1f10));const auto value=math.cosine(c.quad(0xa0));c.quad(0x40,m.store(m.add(m.mul(value,m.q(c.quad(0xa8))),m.q(c.quad(0xb0)))));c.quad(0x48,b().quad(0x48));c.word(8,176);put(0xa0,m.add(q(0xa0),m.c(0x6d1920)));
}
}}
