#pragma once
#include "core/stages/OmniverseBehaviorEngine.hpp"
namespace kaikeidou::omniverse_behavior_detail {
inline void Engine::simple_enemy(){
 auto b=object();
 if(behavior==140){auto value=sine(b.quad(168));put(56,m.add(m.mul(value,m.c(0x6d1d50)),m.c(0x6d1a60)));put(168,m.add(q(168),m.c(0x6d1d48)));return;}
 if(behavior==145){auto value=cosine(b.quad(184));put(56,m.add(m.mul(value,q(192)),q(168)));value=sine(b.quad(184));put(64,m.add(m.mul(m.mul(value,q(192)),m.c(0x6d18f0)),q(176)));put(184,m.add(m.mul(m.i(b.word(128)),m.c(0x6d19e0)),q(184)));put(192,m.add(q(192),m.c(0x6d19d8)));if(le(m.c(0x6d19d0),q(192))||lt(q(64),m.c(0x6d1d40)))b.word(0,0);return;}
 throw std::out_of_range("Omniverse simple enemy behavior");
}
}
