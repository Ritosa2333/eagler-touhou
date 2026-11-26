#pragma once
#include "core/stages/OmniverseBehaviorEngine.hpp"
#include "core/stages/OmniverseBehaviorCollision.hpp"
namespace kaikeidou::omniverse_behavior_detail {
inline void Engine::projectile213(){
 auto b=object();move();
 for(std::uint32_t i=1;i<80;++i){auto e=enemy(i);if(e.word(0)!=1)continue;if(omniverse_behavior_overlap(b.quad(64),b.quad(72),e.quad(56),e.quad(64),constant(0x6d1e48),constant(0x6d1e20),m.precision)){e.word(20,0);e.word(28,0);services.kill_enemy(i);}}
 if(gt(q(72),m.c(0x6d19a0))||lt(q(72),m.q(0))||gt(q(64),m.c(0x6d1cb0))||lt(q(64),m.q(0))){b.quad(80,0);b.quad(88,0);}
}
inline void Engine::projectile225(){
 auto b=object();auto value=sine(b.quad(208));const auto radius=m.store(m.add(m.mul(value,m.c(0x6b25f8)),q(192)));
 value=sine(b.quad(184));const auto dx=m.store(m.sub(m.add(m.mul(value,m.q(radius)),m.q(s.player.quad(144))),q(64)));
 value=cosine(b.quad(184));const auto dy=m.store(m.sub(m.add(m.mul(value,m.q(radius)),m.q(s.player.quad(152))),q(72)));
 const auto angle=m.store(atan2(dy,dx));value=sine(b.quad(160));const auto first=m.store(m.add(m.mul(value,m.c(0x6d19c8)),m.c(0x6d19c0)));
 value=sine(b.quad(168));const auto speed=m.store(m.add(m.add(m.mul(value,m.c(0x6d19b8)),m.q(first)),q(224)));
 value=cosine(angle);put(64,m.add(m.mul(value,m.q(speed)),q(64)));value=sine(angle);put(72,m.add(m.mul(value,m.q(speed)),q(72)));
 put(160,m.add(q(160),q(176)));put(168,m.add(q(168),m.c(0x6d19b0)));put(184,m.add(q(184),q(200)));put(208,m.add(q(208),q(216)));put(224,m.mul(q(224),m.c(0x6d1940)));if(age()==b.word(120))b.word(0,0);
}
}
