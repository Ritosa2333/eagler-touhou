#pragma once
#include "core/stages/OmniverseBehaviorEngine.hpp"
namespace kaikeidou::omniverse_behavior_detail {
inline void Engine::projectile194(){
 auto b=object();if(signed_age()<=49){ease(64,b.quad(160),constant(0x6d1ba8));ease(72,b.quad(168),constant(0x6d1ba8));}
 else{
  if(age()==50){omniverse_behavior_recolor(b,255,0,0);const auto dx=m.store(m.sub(m.q(s.player.quad(144)),q(64))),dy=m.store(m.sub(m.q(s.player.quad(152)),q(72)));put(184,atan2(dy,dx));auto value=cosine(b.quad(184));put(192,value);value=sine(b.quad(184));put(200,value);}
  put(64,m.add(m.mul(q(192),q(176)),q(64)));put(72,m.add(m.mul(q(200),q(176)),q(72)));ease(176,constant(0x6d1908),constant(0x6d1a40));
  if(static_cast<std::uint32_t>(remainder(age(),4))==b.word(120)){const auto slot=allocate(9,0,{{0,0,255}},2,1);if(slot!=-1){auto child=bullet(static_cast<std::uint32_t>(slot));child.quad(64,b.quad(64));child.quad(72,b.quad(72));child.word(8,195);child.quad(160,b.quad(192));child.quad(168,b.quad(200));child.word(12,age());}}
 }clip(0x6d1840,0x6d1a38,0x6d1840,0x6d18c8);
}
inline void Engine::projectile195(){
 auto b=object();if(signed_age()>=signed_word(600u-p().integers[0])){
  if(age()==600u-p().integers[0])omniverse_behavior_recolor(b,255,0,255);
  put(64,m.sub(q(64),m.mul(q(160),m.c(0x6d1908))));put(72,m.sub(q(72),m.mul(q(168),m.c(0x6d1908))));
  if(ge(q(168),m.q(0))){if(lt(q(72),m.c(0x6d1a80)))b.word(0,0);}else if(gt(q(72),m.c(0x6d1a68)))b.word(0,0);
  if(ge(q(160),m.q(0))){if(lt(q(64),m.c(0x6d1a80)))b.word(0,0);}else if(gt(q(64),m.c(0x6d1a78)))b.word(0,0);
 }else if(signed_age()>=350){if(age()==350)omniverse_behavior_recolor(b,0,255,255);put(64,m.add(m.mul(q(160),m.c(0x6d1908)),q(64)));put(72,m.add(m.mul(q(168),m.c(0x6d1908)),q(72)));}
}
inline void Engine::projectile199_201(){
 auto b=object();auto value=cosine(b.quad(160));if(behavior==200)value=cosine(m.store(m.add(m.mul(value,q(192)),m.c(0x6d1b60))));put(64,m.add(m.mul(value,behavior==201?m.c(0x6d1ab0):q(168)),m.c(0x6d1a60)));
 if(behavior==201)put(72,m.add(m.c(0x6d1a88),q(168)));else{if(behavior==200){value=cosine(b.quad(160));value=sine(m.store(m.add(m.mul(value,q(192)),m.c(0x6d1b60))));}else value=sine(b.quad(160));put(72,m.add(m.mul(value,q(168)),m.c(0x6d1a88)));}
 if(signed_age()>=210){auto change=m.mul(m.mul(q(176),m.c(behavior==199?0x6d24c0:behavior==200?0x6d1b18:0x6d2808)),m.i(b.word(120)));put(96,behavior==199?m.sub(q(96),change):m.add(change,q(96)));change=m.mul(m.i(b.word(120)),q(176));put(160,behavior==199?m.sub(q(160),change):m.add(change,q(160)));ease(176,behavior==199?b.quad(192):constant(behavior==200?0x6d1b68:0x6d2800),constant(0x6b25b8));
  const auto slot=allocate(9,0,behavior==199?std::array<std::uint32_t,3>{{0,255,0}}:behavior==200?std::array<std::uint32_t,3>{{0,0,255}}:std::array<std::uint32_t,3>{{255,0,0}},2,1);
  if(slot!=-1){auto child=bullet(static_cast<std::uint32_t>(slot));value=cosine(b.quad(96));child.quad(64,m.store(m.add(m.mul(value,m.c(0x6d1d18)),q(64))));value=sine(b.quad(96));child.quad(72,m.store(m.add(m.mul(value,m.c(0x6d1d18)),q(72))));child.word(8,202);child.word(120,remainder(age(),21)==0?200u:behavior==199?60u:40u);}
 }if(gt(q(184),q(168)))put(168,m.add(q(168),m.c(0x6d1948)));
}
}
