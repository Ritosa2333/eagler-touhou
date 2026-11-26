#pragma once
#include "core/stages/ExBossHeavyEngine.hpp"
namespace kaikeidou { namespace ex_boss_heavy_detail {
inline void Engine::normal172(){if(b().word(0x7c)!=1)return;put(0x40,m.add(m.mul(m.i(b().word(0x78)),q(0x50)),q(0x40)));ex_boss_heavy_ease(b(),0x50,constant(0x6d1ad8),constant(0x6b25b8),m.precision);if(compare(m.c(0x6d1a80),q(0x40))>0||compare(m.c(0x6d1a78),q(0x40))<0)b().word(0,0);}
inline void Engine::normal174(){if(b().word(0x7c)!=1)return;put(0x48,m.add(m.mul(m.i(b().word(0x78)),q(0x58)),q(0x48)));ex_boss_heavy_ease(b(),0x58,constant(0x6d1ad0),constant(0x6b25b8),m.precision);if(compare(m.c(0x6d1a80),q(0x48))>0||compare(m.c(0x6d1a68),q(0x48))<0)b().word(0,0);}
inline void Engine::normal173(){if(age()<0)return;if(b().word(12)==0){b().word(0x78,compare(m.q(player.quad(0x98)),q(0x48))>=0?1u:0xffffffffu);at(b().word(0x7c)).word(0x7c,1);at(b().word(0x80)).word(0x7c,1);}put(0x48,m.add(m.mul(m.i(b().word(0x78)),q(0x58)),q(0x48)));ex_boss_heavy_ease(b(),0x58,constant(0x6d1ad8),constant(0x6b25b8),m.precision);if(compare(m.c(0x6d1a80),q(0x48))>0||compare(m.c(0x6d1a68),q(0x48))<0)b().word(0,0);}
inline void Engine::normal175(){if(age()<0)return;if(b().word(12)==0)at(b().word(0x7c)).word(0x7c,1);put(0x48,m.add(m.mul(m.i(b().word(0x78)),q(0x58)),q(0x48)));ex_boss_heavy_ease(b(),0x58,constant(0x6d1908),constant(0x6d1ab0),m.precision);if(compare(m.c(0x6d1a80),q(0x48))>0||compare(m.c(0x6d1a68),q(0x48))<0)b().word(0,0);}
inline void Engine::normal176(){const auto value=math.cosine(b().quad(0xa0));put(0x40,m.add(m.mul(value,q(0xa8)),q(0xb0)));put(0xa0,m.add(q(0xa0),q(0xc0)));ex_boss_heavy_ease(b(),0xa8,constant(0x6d1ac8),b().quad(0xb8),m.precision);ex_boss_heavy_ease(b(),0xb8,constant(0x6d1ab0),constant(0x6d1ab0),m.precision);}
}}
