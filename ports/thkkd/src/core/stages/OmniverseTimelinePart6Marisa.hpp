#pragma once
#include "core/stages/OmniverseTimelineEngine.hpp"
namespace kaikeidou::omniverse_timeline_detail {
inline void Engine::part6_marisa(){
 const auto clone=[&](std::uint32_t effect_index,std::uint64_t angle,std::uint64_t speed){const auto slot=allocate_enemy(s.enemies);if(slot==-1)return;auto e=enemy_at(static_cast<std::uint32_t>(slot));auto f=effect_view(effect_index);e.word(4,f.word(16));e.word(24,f.word(20));e.word(20,e.word(24));e.word(8,f.word(24));e.quad(56,f.quad(96));e.quad(64,f.quad(104));auto value=cosine(angle);e.quad(72,m.store(m.mul(value,m.q(speed))));value=sine(angle);e.quad(80,m.store(m.mul(value,m.q(speed))));};
 if(frame()==0){name(0x6c5be4);services.configure_phase(1,60,0,162);real(0)=constant(0x6d2c68);integer(0)=0;}
 else if(frame()==60)effect_at(10,{{0,0,255}},constant(0x6d1a60),constant(0x6d1af0));
 else if(frame()==110)enemy().quad(176,constant(0x6d2090));
 else if(signed_frame()>=100&&signed_frame()<=320){if(frame()==200){enemy().quad(56,constant(0x6d2090));enemy().quad(168,enemy().quad(56));}
  if(remainder(frame(),20)==0){auto value=rnd(0x6d1ab0);const auto y=m.store(m.add(m.mul(value,m.c(0x6d1960)),m.c(0x6d2c60)));value=rnd(0x6d1ab0);const auto x=m.store(m.add(m.mul(value,m.c(0x6d1960)),m.c(0x6d2c58)));const auto effect_index=stage6_boss_colored_effect(s.effects,s.runtime.effect_prefix,s.projectiles.sound_triggers,14,1,0x540be3ff,142,x,y,services);
   for(unsigned i=0;i<2;++i){const auto angle=real(0);value=rnd(0x6b25b8);const auto speed=m.store(m.add(m.mul(value,m.c(0x6a7708)),m.c(0x6d1a00)));clone(static_cast<std::uint32_t>(effect_index),angle,speed);value=rnd(0x6b25b8);real(0)=m.store(m.add(m.add(m.mul(value,m.c(0x6d1950)),m.c(0x6d18e0)),m.q(real(0))));}
  }
 }else if(frame()==2400){enemy().quad(56,constant(0x6d1a60));enemy().quad(168,enemy().quad(56));enemy().quad(176,constant(0x6d1a90));}
 else if(frame()==2600){const auto value=rnd(0x6d1c60);auto angle=m.store(m.mul(value,m.c(0x6d1950)));for(unsigned i=0;i<60;++i){const auto slot=allocate(2,1,{{0,0,0}},0,1);if(slot!=-1){auto b=bullet(static_cast<std::uint32_t>(slot));b.quad(64,enemy().quad(56));b.quad(72,enemy().quad(64));auto trig=cosine(angle);b.quad(80,m.store(m.mul(trig,m.c(0x6d1cf0))));trig=sine(angle);b.quad(88,m.store(m.mul(trig,m.c(0x6d2c50))));b.word(8,213);b.word(52,1);}angle=m.store(m.add(m.q(angle),m.c(0x6d2c48)));}}
 else if(signed_frame()>=2700&&remainder(frame(),50)==0){const auto color=random_integer(4);const auto y=enemy().quad(64),x=enemy().quad(56);const auto effect_index=stage6_boss_colored_effect(s.effects,s.runtime.effect_prefix,s.projectiles.sound_triggers,14,color,0x540be3ff,143,x,y,services);auto angle=real(0);for(unsigned i=0;i<5;++i){clone(static_cast<std::uint32_t>(effect_index),angle,0x3ff0000000000000ull);angle=m.store(m.add(m.q(angle),m.c(0x6d2618)));}const auto value=rnd(0x6b25b8);real(0)=m.store(m.add(m.add(m.mul(value,m.c(0x6d1950)),m.c(0x6d18e0)),m.q(real(0))));}
 if(signed_frame()<400||signed_frame()>2600)return;
 if(remainder(frame(),300)==100){effect_at(10,{{0,0,255}},constant(0x6d18e8),constant(0x6d18e8));effect_at(10,{{0,0,255}},constant(0x6d18e8),constant(0x6d1b78));effect_at(10,{{0,0,255}},constant(0x6d1b80),constant(0x6d18e8));effect_at(10,{{0,0,255}},constant(0x6d1b80),constant(0x6d1b78));}
 else if(remainder(frame(),300)==140){for(unsigned corner=0;corner<4;++corner)emit({{2,0,{{0,0,255}},2,255,1,41},constant(corner<2?0x6d18e8:0x6d1b80),constant(corner%2?0x6d1b78:0x6d18e8),1,0,constant(0x6d1948),0,0,0});}
}
}
