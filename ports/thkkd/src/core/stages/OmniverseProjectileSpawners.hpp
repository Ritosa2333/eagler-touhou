#pragma once
#include "core/stages/OmniverseBehaviorEngine.hpp"
namespace kaikeidou::omniverse_behavior_detail {
inline void Engine::projectile216(){
 auto b=object();ease(64,b.quad(168),b.quad(160));ease(72,b.quad(176),b.quad(160));
 if(age()==0){const auto dx=m.store(m.sub(m.q(s.player.quad(144)),q(64))),dy=m.store(m.sub(m.q(s.player.quad(152)),q(72)));auto target=atan2(dy,dx);auto angle=m.store(m.sub(target,m.mul(q(184),m.c(0x6d1ad8))));
  for(unsigned arm=0;arm<7;++arm){for(unsigned j=0;j<8;++j){const auto green=random_integer(127);const auto slot=allocate(9,0,{{255,green,0}},2,0);if(slot==-1)continue;auto child=bullet(static_cast<std::uint32_t>(slot));auto value=cosine(angle);child.quad(64,m.store(m.add(m.mul(value,m.c(0x6d1ae8)),q(64))));value=sine(angle);child.quad(72,m.store(m.add(m.mul(value,m.c(0x6d1ae8)),q(72))));child.word(8,217);value=cosine(angle);child.quad(80,m.store(value));value=sine(angle);child.quad(88,m.store(value));child.quad(104,constant(0x6d1ad8));child.quad(160,m.store(m.i(7u-j)));child.word(12,0u-2u*j);}angle=m.store(m.add(m.q(angle),q(184)));}
  put(184,m.sub(q(184),m.c(0x6d1d28)));return;
 }
 if(age()==40){put(160,m.mul(q(160),m.c(0x6d1bd0)));b.quad(168,s.player.quad(144));const auto value=rnd(0x6d2710);put(176,m.add(m.mul(value,m.c(0x6d1960)),m.c(0x6b25f8)));return;}
 if(age()==b.word(120)){b.word(12,0xffffffffu);b.word(120,b.word(120)-1u);}
}
inline void Engine::projectile224(){
 if(signed_word(frame())<340||remainder(frame(),300)!=40)return;auto b=object();const auto color=random_integer(4);constexpr std::array<std::array<std::uint32_t,3>,5> colors{{{{255,0,0}},{{0,0,255}},{{0,255,0}},{{0,255,255}},{{255,127,0}}}};
 if(color>=colors.size())throw std::out_of_range("Omniverse random color outside original initialized domain");
 const auto effect_index=stage6_boss_colored_effect(s.effects,s.effect_prefix,pool.sound_triggers,14,color,100,145,constant(0x6d1a60),constant(0x6d1f08),services);auto source=effect(static_cast<std::uint32_t>(effect_index));auto value=rnd(0x6d1c60);auto angle=m.store(m.mul(value,m.c(0x6d1950)));
 for(unsigned i=0;i<130;++i){
  if(i%10>=3){const auto slot=allocate(1,1,colors[color],2,0);if(slot!=-1){auto child=bullet(static_cast<std::uint32_t>(slot));child.quad(160,b.quad(64));child.quad(168,b.quad(72));child.quad(176,angle);child.quad(184,constant(0x6d1a88));child.word(120,p().integers[3]);value=cosine(child.quad(176));child.quad(64,m.store(m.add(m.mul(value,m.q(child.quad(184))),m.q(child.quad(160)))));value=sine(child.quad(176));child.quad(72,m.store(m.add(m.mul(m.mul(value,m.q(child.quad(184))),m.c(0x6d18f0)),m.q(child.quad(168)))));child.word(8,223);child.word(52,1);}}
  else{const auto slot=allocate_enemy(enemies);if(slot!=-1){auto child=enemy(static_cast<std::uint32_t>(slot));child.word(4,source.word(16));child.word(24,source.word(20));child.word(20,child.word(24));child.word(8,source.word(24));child.quad(168,b.quad(64));child.quad(176,b.quad(72));child.quad(184,angle);child.quad(192,constant(0x6d1a88));child.word(128,p().integers[3]);value=cosine(child.quad(184));child.quad(56,m.store(m.add(m.mul(value,m.q(child.quad(192))),m.q(child.quad(168)))));value=sine(child.quad(184));child.quad(64,m.store(m.add(m.mul(m.mul(value,m.q(child.quad(192))),m.c(0x6d18f0)),m.q(child.quad(176)))));}}
  angle=m.store(m.add(m.q(angle),m.c(0x6d27a0)));
 }
 p().integers[3]=0u-p().integers[3];
}
}
