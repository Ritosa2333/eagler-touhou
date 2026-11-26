#pragma once
#include "core/stages/OmniverseBehaviorEngine.hpp"
namespace kaikeidou::omniverse_behavior_detail {
inline void Engine::projectile209(){
 auto b=object();b.quad(216,b.quad(64));b.quad(224,b.quad(72));b.quad(232,b.quad(96));const auto base=m.store(m.add(m.q(p().reals[0]),q(168)));auto value=sine(b.quad(160));put(96,m.add(m.div(m.mul(m.mul(value,q(176)),q(104)),q(184)),m.q(base)));
 if(signed_age()>=0){put(160,m.add(m.div(m.mul(m.c(0x6d1900),q(104)),q(184)),q(160)));value=cosine(b.quad(96));put(64,m.add(m.mul(value,q(104)),q(64)));value=sine(b.quad(96));put(72,m.add(m.mul(value,q(104)),q(72)));}
 if(lt(q(64),m.c(0x6d1840)))put(64,m.add(q(64),m.c(0x6d19a0)));else if(gt(q(64),m.c(0x6d1a38)))put(64,m.sub(q(64),m.c(0x6d19a0)));
 if(lt(q(72),m.c(0x6d1840)))put(72,m.add(q(72),m.c(0x6d1998)));else if(gt(q(72),m.c(0x6d18c8)))put(72,m.sub(q(72),m.c(0x6d1998)));
 if(static_cast<std::uint32_t>(remainder(frame(),216))!=b.word(120))return;
 const auto dx=m.store(m.sub(m.q(s.player.quad(144)),q(64))),dy=m.store(m.sub(m.q(s.player.quad(152)),q(72)));const auto angle=m.store(atan2(dy,dx));const auto slot=allocate(1,0,{{200,0,0}},0,1);if(slot==-1)return;auto child=bullet(static_cast<std::uint32_t>(slot));child.quad(64,b.quad(64));child.quad(72,b.quad(72));child.word(8,211);value=cosine(angle);child.quad(80,m.store(value));value=sine(angle);child.quad(88,m.store(value));child.quad(104,0);child.quad(160,constant(0x6d1cc0));child.quad(168,constant(0x6d1948));
}
inline void Engine::projectile212(){
 auto b=object();auto value=sine(b.quad(160));put(96,m.add(m.div(m.mul(m.mul(value,q(176)),q(104)),q(184)),q(168)));put(160,m.add(m.div(m.mul(m.c(0x6d1900),q(104)),q(184)),q(160)));value=cosine(b.quad(96));put(64,m.add(m.mul(value,q(104)),q(64)));value=sine(b.quad(96));put(72,m.add(m.mul(value,q(104)),q(72)));clip(0x6d1840,0x6d1a38,0x6d1840,0x6d18c8);
}
}
