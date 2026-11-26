#pragma once
#include "core/stages/ExBossLateOtherEngine.hpp"
namespace kaikeidou { namespace ex_boss_late_other_detail {
inline void Engine::normal185(){
 if(b().word(0x78)==0){auto value=math.cosine(b().quad(0xa0));put(0x40,m.add(m.mul(value,m.c(0x6d1c90)),q(0x40)));value=math.sine(b().quad(0xa0));put(0x48,m.add(m.mul(value,m.c(0x6d1c90)),q(0x48)));put(0xa0,m.add(q(0xa0),q(0xa8)));if(compare(m.i(0),q(0x48))>0||compare(m.c(0x6d19a0),q(0x48))<0||compare(m.i(0),q(0x40))>0||compare(m.c(0x6d1cb0),q(0x40))<0){b().word(0x78,1);b().word(12,0xffffffffu);}return;}
 if(b().word(0x78)!=1||remainder(b().word(12),2)!=0)return;
 const auto slot=allocate_projectile(pool,{9,0,{{0,0,255}},2,255,1,41});if(slot!=-1){auto c=at(static_cast<std::uint32_t>(slot));c.quad(0x40,b().quad(0x40));c.quad(0x48,b().quad(0x48));c.quad(0xa0,m.store(m.add(q(0xa0),m.c(0x6d1860))));auto v=q(0xa8);v.negative=!v.negative;c.quad(0xa8,m.store(v));c.word(8,186);}
 if(b().word(12)!=38)return;
 const auto emit=[&](std::uint32_t count,std::uint32_t spacing,std::uint32_t speed,std::uint64_t angle,std::uint32_t absolute){ex_boss_late_other_emit(pool,{{16,0,{{0,0,255}},0,255,1,41},b().quad(0x40),b().quad(0x48),count,constant(spacing),constant(speed),angle,absolute,0},player,p,random,precision,math);};
 if(b().word(0x7c)==2)emit(19,0x6d2d60,0x6d1ff8,0,0);
 else if(b().word(0x7c)==1){emit(11,0x6d27b0,0x6d1c80,constant(0x6d2140),0);const auto angle=m.store(m.mul(random.real_number(constant(0x6d1c60),precision),m.c(0x6d1950)));emit(9,0x6d2290,0x6d2488,angle,1);}
 else{emit(11,0x6d27b0,0x6d1b18,0,0);auto angle=m.store(m.mul(random.real_number(constant(0x6d1c60),precision),m.c(0x6d1950)));emit(8,0x6d22d8,0x6d1948,angle,1);angle=m.store(m.mul(random.real_number(constant(0x6d1c60),precision),m.c(0x6d1950)));emit(7,0x6d2970,0x6d1ad0,angle,1);}b().word(0,0);
}
inline void Engine::normal187(){const auto angle=m.store(m.add(m.c(0x6d1de0),q(0xa0)));auto value=math.cosine(angle);put(0x40,m.add(m.mul(value,q(0xb0)),m.c(0x6d1a60)));value=math.sine(angle);put(0x48,m.add(m.mul(value,q(0xb0)),m.c(0x6d1f08)));
 if(age()<=19){ex_boss_late_other_ease(b(),0xb0,b().quad(0xb8),constant(0x6d18e8),m.precision);return;}
 if(age()>=200){ex_boss_late_other_ease(b(),0xb0,0,constant(0x6d1898),m.precision);if(compare(m.c(0x6d1e20),q(0xb0))>0)ex_boss_late_other_remove(pool,index,100);return;}
 if(b().word(0x78)==1&&remainder(b().word(12),8)==0){const auto slot=allocate_projectile(pool,{9,0,{{0,255,0}},2,255,1,41});if(slot!=-1){auto c=at(static_cast<std::uint32_t>(slot));c.quad(0xa0,angle);c.quad(0xa8,0);c.quad(0xb0,constant(compare(m.i(0),q(0xa8))<=0?0x6d1a08:0x6d2350));c.quad(0xb8,b().quad(0xb0));value=math.cosine(c.quad(0xa0));c.quad(0x40,m.store(m.add(m.mul(value,m.q(c.quad(0xb8))),m.c(0x6d1a60))));value=math.sine(c.quad(0xa0));c.quad(0x48,m.store(m.add(m.mul(value,m.q(c.quad(0xb8))),m.c(0x6d1f08))));c.word(8,188);c.word(12,b().word(12)-200u);}}
 ex_boss_late_other_ease(b(),0xa0,b().quad(0xa8),b().quad(0xc0),m.precision);
}
inline void Engine::normal188(){if(age()<0)return;auto value=math.cosine(b().quad(0xa0));put(0x40,m.add(m.mul(value,q(0xb8)),m.c(0x6d1a60)));value=math.sine(b().quad(0xa0));put(0x48,m.add(m.mul(value,q(0xb8)),m.c(0x6d1f08)));put(0xa0,m.add(q(0xa0),q(0xa8)));ex_boss_late_other_ease(b(),0xa8,b().quad(0xb0),constant(0x6d1aa8),m.precision);if(compare(m.c(0x6d1f00),q(0xa0))>0||compare(m.c(0x6d1ea8),q(0xa0))<0)ex_boss_late_other_remove(pool,index,0);}
}}
