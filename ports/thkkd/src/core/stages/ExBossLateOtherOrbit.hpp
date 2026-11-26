#pragma once
#include "core/stages/ExBossLateOtherEngine.hpp"
namespace kaikeidou { namespace ex_boss_late_other_detail {
inline void Engine::normal123(){if(signed_word(p.game_frame)>=331)put(0x60,m.add(m.mul(m.q(real(0)),m.q(real(4))),q(0x60)));}
inline void Engine::normal124(){if(signed_word(p.game_frame)<331)return;put(0x60,m.add(m.mul(m.q(real(1)),m.q(real(4))),q(0x60)));auto value=math.cosine(b().quad(0xa0));put(0x40,m.add(m.mul(value,m.q(real(10))),m.c(0x6d1a60)));value=math.sine(b().quad(0xa0));put(0x48,m.add(m.mul(value,m.q(real(10))),m.c(0x6d1af0)));put(0xa0,m.add(m.mul(m.q(real(3)),m.q(real(4))),q(0xa0)));}
inline void Engine::normal125(){if(signed_word(p.game_frame)<331)return;auto value=m.q(real(1));value.negative=!value.negative;put(0x60,m.add(m.mul(m.mul(value,m.c(0x6d1b98)),m.q(real(4))),q(0x60)));}
inline void Engine::normal126(){if(signed_word(p.game_frame)>=331)put(0x60,m.add(m.mul(m.mul(m.q(real(1)),m.c(0x6d1b98)),m.q(real(4))),q(0x60)));}
inline void Engine::normal127(){put(0x40,m.add(m.mul(q(0xa0),m.c(0x6d1b90)),m.q(at(b().word(0x78)).quad(0x40))));put(0x48,m.sub(m.q(at(b().word(0x78)).quad(0x48)),m.mul(q(0xa0),m.c(0x6d1b88))));}
inline void Engine::normal192(){put(0x48,m.add(q(0x48),m.q(real(10))));if(compare(m.c(0x6d1a68),q(0x48))<0)b().word(0,0);}
inline void Engine::normal193(){put(0x48,m.add(q(0x48),m.q(real(10))));if(p.integers[1]==1){const auto first=m.add(m.sub(m.q(at(b().word(0x78)).quad(0x40)),m.q(real(8))),m.q(real(2)));const auto second=m.sub(m.q(at(b().word(0x7c)).quad(0x40)),m.q(real(9)));put(0x40,m.add(first,second));}else b().quad(0x40,constant(0x6d1a70));if(compare(m.c(0x6d1a68),q(0x48))<0)b().word(0,0);}
}}
