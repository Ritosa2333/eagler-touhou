#pragma once
#include "core/stages/OmniverseBehaviorEngine.hpp"
namespace kaikeidou::omniverse_behavior_detail {
inline void Engine::projectile197(){
 auto b=object();if(signed_age()<=120){ease(64,b.quad(160),constant(0x6d1b90));ease(72,b.quad(168),constant(0x6d1b90));return;}
 if(signed_age()<=220){if(remainder(age(),3)!=0)return;auto value=rnd(0x6d1c60);const auto direction=m.store(m.mul(value,m.c(0x6d1950)));value=rnd(0x6d2820);const auto radius=m.store(m.add(m.mul(value,m.c(0x6a7708)),m.q(0x3ff0000000000000ull)));value=rnd(0x6d2710);const auto angle=m.store(m.add(m.mul(value,m.c(0x6d1950)),m.c(0x6d2818)));value=rnd(0x6d2810);const auto speed=m.store(m.add(m.mul(value,m.c(0x6d1950)),q(176)));const auto slot=allocate(10,0,{{b.word(24),b.word(28),b.word(32)}},0,1);if(slot==-1)return;auto child=bullet(static_cast<std::uint32_t>(slot));value=cosine(direction);child.quad(64,m.store(m.add(m.mul(value,m.q(radius)),q(64))));value=sine(direction);child.quad(72,m.store(m.add(m.mul(value,m.q(radius)),q(72))));child.word(8,198);child.quad(96,angle);value=cosine(angle);child.quad(80,m.store(m.mul(value,m.q(speed))));value=sine(angle);child.quad(88,m.store(m.mul(value,m.q(speed))));return;}
 if(signed_age()<250)return;if(age()==250){const auto dx=m.store(m.sub(m.q(s.player.quad(144)),q(64))),dy=m.store(m.sub(m.q(s.player.quad(152)),q(72)));const auto angle=m.store(atan2(dy,dx));auto value=cosine(angle);put(80,value);value=sine(angle);put(88,value);}
 put(64,m.add(m.mul(q(80),q(184)),q(64)));put(72,m.add(m.mul(q(88),q(184)),q(72)));ease(184,constant(0x6d19c0),constant(0x6d1a88));clip(0x6d1840,0x6d1a38,0x6d1840,0x6d18c8);
}
inline void Engine::projectile207(){
 auto b=object();if(b.word(120)==0){auto value=cosine(b.quad(160));put(64,m.add(m.mul(value,q(168)),m.c(0x6d1a60)));value=sine(b.quad(160));put(72,m.add(m.mul(value,q(168)),m.c(0x6d1af0)));if(!gt(q(168),m.q(0)))return;put(168,m.sub(q(168),q(192)));ease(192,0x3ff0000000000000ull,constant(0x6b25b8));if(!ge(m.q(0),q(168)))return;if(b.word(124)==0){b.word(120,1);b.quad(168,0);return;}
  b.word(120,2);if(b.word(124)==1){const auto dx=m.store(m.sub(m.q(s.player.quad(144)),m.c(0x6d1a60))),dy=m.store(m.sub(m.q(s.player.quad(152)),m.c(0x6d1af0)));auto target=atan2(dy,dx);const auto angle=m.store(m.add(target,m.div(m.mul(m.i(p().integers[0]),m.c(0x6d1860)),m.c(0x6d1910))));++p().integers[0];value=cosine(angle);put(80,m.mul(value,m.c(0x6d1ad0)));value=sine(angle);put(88,m.mul(value,m.c(0x6d1ad0)));omniverse_behavior_recolor(b,255,255,0);}
  else{const auto dx=m.store(m.sub(m.q(s.player.quad(144)),m.c(0x6d1a60))),dy=m.store(m.sub(m.q(s.player.quad(152)),m.c(0x6d1af0)));const auto angle=m.store(atan2(dy,dx));value=cosine(angle);put(80,m.mul(value,m.c(0x6d18e0)));value=sine(angle);put(88,m.mul(value,m.c(0x6d18e0)));omniverse_behavior_recolor(b,0,0,255);}
  b.quad(64,constant(0x6d1a60));b.quad(72,constant(0x6d1af0));return;
 }
 if(b.word(120)==1){auto value=cosine(m.store(m.add(m.q(p().reals[0]),q(176))));put(64,m.add(m.mul(value,q(168)),m.c(0x6d1a60)));value=sine(m.store(m.add(m.q(p().reals[0]),q(176))));put(72,m.add(m.mul(value,q(168)),m.c(0x6d1af0)));put(168,m.add(q(168),m.c(0x6d1ef8)));return;}
 move();clip(0x6d1840,0x6d1a38,0x6d1840,0x6d18c8);
}
}
