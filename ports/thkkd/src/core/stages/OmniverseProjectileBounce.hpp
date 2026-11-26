#pragma once
#include "core/stages/OmniverseBehaviorEngine.hpp"
namespace kaikeidou::omniverse_behavior_detail {
inline void Engine::projectile214(){
 auto b=object();const bool left=b.word(120)!=0;const bool hit=left?le(q(64),m.c(0x6d1a40)):ge(q(64),m.c(0x6d2160));
 if(!hit){auto value=cosine(b.quad(160));put(64,m.add(m.mul(value,q(104)),q(64)));value=sine(b.quad(160));put(72,m.add(m.mul(value,q(104)),q(72)));put(104,m.add(q(104),q(168)));put(168,m.add(q(168),m.c(0x6d1a08)));return;}
 b.quad(64,constant(left?0x6d1a40:0x6d2160));b.word(120,left?0u:1u);const auto dx=m.store(m.sub(m.c(left?0x6d2160:0x6d1a40),q(64)));auto value=rnd(0x6d1f20);const auto dy=m.store(m.sub(m.sub(m.c(0x6d19f8),m.mul(value,m.c(0x6d1960))),q(72)));put(160,atan2(dy,dx));b.quad(104,s.boss.extra_reals[0]);b.quad(168,0);
 for(unsigned i=0;i<26;++i){const auto red=random_integer(100);const auto blue=red+random_integer(35)+120u;const auto green=red+random_integer(90)+30u;const auto slot=allocate(0,0,{{red,green,blue}},0,0);if(slot==-1)continue;auto child=bullet(static_cast<std::uint32_t>(slot));value=rnd(0x6d1c60);const auto direction=m.store(m.mul(value,m.c(0x6d1950)));value=rnd(0x6d2188);const auto radius=m.store(m.mul(value,m.c(0x6a7708)));value=rnd(0x6d1c60);const auto angle=m.store(m.mul(value,m.c(0x6d1950)));value=rnd(0x6d1cc0);const auto speed=m.store(m.add(m.mul(value,m.c(0x6a7708)),m.q(0x3ff0000000000000ull)));
  value=cosine(direction);child.quad(64,m.store(m.add(m.mul(value,m.q(radius)),q(64))));value=sine(direction);child.quad(72,m.store(m.add(m.mul(value,m.q(radius)),q(72))));child.word(8,215);child.quad(104,0);value=cosine(angle);child.quad(160,m.store(m.mul(value,m.q(speed))));value=sine(angle);child.quad(168,m.store(m.add(m.mul(value,m.q(speed)),m.c(0x6d2660))));value=rnd(0x6b25b8);child.quad(176,m.store(m.sub(m.c(0x6d22a0),m.mul(value,m.c(0x6d1950)))));child.quad(184,constant(0x6d1940));value=rnd(0x6b25b8);child.quad(192,m.store(m.sub(m.c(0x6d2340),m.mul(value,m.c(0x6d1950)))));
 }
 if(s.shake.word(0)==0)set_effect_screen_shake(s.shake,6,12);
}
}
