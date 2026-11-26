#pragma once
#include "core/stages/OmniverseBehaviorEngine.hpp"
namespace kaikeidou::omniverse_behavior_detail {
inline void Engine::enemy141(){
 auto b=object();ease(56,b.quad(168),constant(0x6d1a40));
 if(remainder(age(),250)==249){b.quad(168,s.player.quad(144));if(lt(q(168),m.c(0x6d1a90))){const auto value=rnd(0x6d1ab0);put(168,m.add(m.mul(value,m.c(0x6d1960)),m.c(0x6d1a90)));}else if(gt(q(168),m.c(0x6d2110))){const auto value=rnd(0x6d1ab0);put(168,m.sub(m.c(0x6d2110),m.mul(value,m.c(0x6d1960))));}}
 if(remainder(age(),600)==60){constexpr std::array<std::array<std::uint32_t,3>,6> colors{{{{0,0,255}},{{0,255,255}},{{0,255,0}},{{255,255,0}},{{255,0,0}},{{255,0,255}}}};const auto phase=b.word(128);if(phase<6){b.word(132,colors[phase][0]);b.word(136,colors[phase][1]);b.word(140,colors[phase][2]);}const auto y=b.quad(64),x=b.quad(56);const auto color=services.color(b.word(132),b.word(136),b.word(140));allocate_effect(s.effects,pool.sound_triggers,{12,color,x,y});return;}
 if(remainder(age(),600)!=100)return;
 const auto spawn=[&](std::uint32_t target_x,std::uint32_t target_y){const auto slot=allocate(2,0,{{b.word(132),b.word(136),b.word(140)}},2,1);if(slot==-1)return;auto child=bullet(static_cast<std::uint32_t>(slot));child.quad(64,b.quad(56));child.quad(72,m.store(m.sub(q(64),m.c(0x6d1a88))));child.word(8,197);child.quad(160,constant(target_x));child.quad(168,constant(target_y));child.quad(176,b.quad(176));child.quad(184,0);child.word(52,1);};
 if(signed_word(b.word(128))<=2){spawn(0x6d1a60,0x6d1858);spawn(0x6d1a90,0x6d1f08);spawn(0x6d2110,0x6d1f08);}else{spawn(0x6d2108,0x6d1f08);spawn(0x6d2100,0x6d1f08);}
 if(b.word(128)==0){spawn(0x6d1a60,0x6d1a78);spawn(0x6b25b8,0x6d20f8);spawn(0x6d20f0,0x6d20f8);}
 put(176,m.add(q(176),m.c(0x6d2008)));b.word(128,b.word(128)+1u);
}
}
