#pragma once
#include "core/stages/OmniverseBehaviorEngine.hpp"
namespace kaikeidou::omniverse_behavior_detail {
inline void Engine::enemy139(){
 auto b=object();if(remainder(age(),300)==260){const auto y=b.quad(64),x=b.quad(56);const auto color=services.color(0,255,0);allocate_effect(s.effects,pool.sound_triggers,{12,color,x,y});b.word(44,3);b.word(48,0);b.word(52,0);p().integers[0]+=6u;}
 const auto step=remainder(age(),300);if(step==0||step==20||step==40||step==60||step==80){const auto group=static_cast<unsigned>(step/20);if(group==0)b.word(44,0);
  constexpr std::uint32_t centers[5][2]{{0x6d1df0,0x6d1df8},{0x6d22b0,0x6d29a8},{0x6d2748,0x6d2998},{0x6d25b0,0x6d2988},{0x6d1b88,0x6d28b0}};
  constexpr std::uint32_t targets[5][2]{{0x6d1a60,0x6d1a60},{0x6d2648,0x6d2650},{0x6d29a0,0x6d2990},{0x6d2468,0x6d2980},{0x6d1b88,0x6d28b0}};
  for(unsigned side=0;side<2;++side){const auto slot=allocate(2,0,{{0,255,0}},2,1);if(slot==-1)continue;auto child=bullet(static_cast<std::uint32_t>(slot));child.quad(64,b.quad(56));child.quad(72,m.store(m.sub(q(64),m.c(0x6d1ae8))));auto value=rnd(0x6b25b8);auto delta=m.mul(value,m.c(0x6d1960));child.quad(160,m.store(side==0?m.sub(m.c(centers[group][side]),delta):m.add(delta,m.c(centers[group][side]))));value=rnd(0x6b25b8);child.quad(168,m.store(m.add(m.mul(value,m.c(0x6d1960)),m.c(0x6d1898))));child.quad(176,group==0?constant(0x6d1948):group==1?constant(0x6d1ad0):group==2?0x3ff0000000000000ull:group==3?constant(0x6d19c0):0);
   const auto dx=m.store(m.sub(m.c(targets[group][side]),m.q(child.quad(160)))),dy=m.store(m.sub(m.c(0x6d19a0),m.q(child.quad(168))));child.quad(184,m.store(atan2(dy,dx)));auto n=random_integer(3);child.word(120,n);n=random_integer(4);child.word(124,n);child.word(8,194);child.word(52,1);
  }
  if(group==4){b.quad(168,0);b.word(128,255);}
 }
 if(remainder(age(),300)<140||remainder(age(),300)>239||remainder(age(),3)!=0)return;
 emit({{11,0,{{255,b.word(128),0}},0,255,1,41},b.quad(56),b.quad(64),3,b.quad(168),constant(0x6d2410),0,0,0});
 emit({{11,0,{{255,0,0}},0,255,1,41},b.quad(56),b.quad(64),10,constant(0x6d19c0),constant(0x6d2410),constant(0x6d1860),0,0});put(168,m.add(q(168),m.c(0x6a7708)));b.word(128,b.word(128)-8u);
}
}
