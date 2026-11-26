#pragma once
#include "core/stages/OmniverseBehaviorEngine.hpp"
namespace kaikeidou::omniverse_behavior_detail {
inline void Engine::enemy143(){
 auto b=object();put(56,m.add(q(56),q(72)));put(64,m.add(q(64),q(80)));if(remainder(age(),45)!=40)return;
 emit({{11,2,{{0,0,0}},0,255,1,41},b.quad(56),b.quad(64),5,constant(0x6d2618),constant(0x6d1c18),0,0,0});
 const auto value=rnd(0x6d1c60);const auto angle=m.store(m.mul(value,m.c(0x6d1950)));
 emit({{10,3,{{0,0,0}},0,255,1,41},b.quad(56),b.quad(64),5,constant(0x6d2618),constant(0x6d1a98),angle,1,0});
}
inline void Engine::enemy144(){
 auto b=object();ease(56,s.player.quad(144),constant(0x6d1a90));ease(64,constant(0x6d1d50),constant(0x6d1a90));const auto previous=b.word(132),limit=b.word(128);b.word(132,b.word(132)+1u);if(previous!=limit)return;
 emit({{2,2,{{0,0,0}},0,255,1,41},b.quad(56),b.quad(64),7,constant(0x6d2970),constant(0x6d1ad8),0,0,0});b.word(132,0);if(signed_word(b.word(128))>2)b.word(128,b.word(128)-1u);
}
inline void Engine::enemy146_150(){
 auto b=object();put(56,m.add(q(56),q(72)));
 if(gt(q(56),m.q(0))&&lt(q(56),m.c(0x6d1cb0))&&remainder(age(),100)==0){
  if(behavior==146||behavior==147)emit({{35,1,{{0,0,0}},0,255,0,41},b.quad(56),b.quad(64),15,constant(0x6d2978),constant(behavior==146?0x6d1ad8:0x6d1d60),behavior==146?0:constant(0x6d27e0),0,0});
  else if(behavior==148){const auto value=rnd(0x6d1c60);const auto angle=m.store(m.mul(value,m.c(0x6d1950)));emit({{35,1,{{0,0,0}},0,255,0,41},b.quad(56),b.quad(64),7,constant(0x6d2970),constant(0x6d1ce0),angle,0,0});}
  else if(behavior==149){for(auto speed:{0x6d1ad0u,0x6d1948u,0x6d1b18u,0x6d1ad8u})emit({{35,1,{{0,0,0}},0,255,0,41},b.quad(56),b.quad(64),1,0,constant(speed),0,0,0});}
  else{for(auto angle:{0x6d2008u,0x6d2968u})emit({{35,1,{{0,0,0}},0,255,0,41},b.quad(56),b.quad(64),2,constant(0x6d1918),constant(0x6d1ce0),constant(angle),0,0});}
 }
 if(gt(q(56),m.c(0x6d19e8))||lt(q(56),m.c(0x6d19f0)))b.word(0,0);
}
}
