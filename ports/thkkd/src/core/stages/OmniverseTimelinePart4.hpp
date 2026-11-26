#pragma once
#include "core/stages/OmniverseTimelineEngine.hpp"
namespace kaikeidou::omniverse_timeline_detail {
inline void Engine::part4(){
 if(t().subphase==0){begin(4,8,0x6d1a90);return;}if(t().subphase!=1)return;
 if(frame()==0){name(0x6c5b80);services.configure_phase(1,60,10000,157);integer(0)=0;integer(1)=65;integer(2)=1;integer(3)=120;integer(4)=1;s.allocation.field_a7ea5c=0;s.protection.boss_state_a7ea60=1;return;}
 if(frame()==60){effect_at(10,{{0,0,255}},constant(0x6d2160),constant(0x6d1de8));effect_at(10,{{0,0,255}},constant(0x6d2160),constant(0x6d1d90));return;}
 if(frame()==80){effect_at(10,{{0,255,0}},constant(0x6d1c58),constant(0x6d1a40));effect_at(10,{{0,255,0}},constant(0x6d1dc8),constant(0x6d1a40));return;}
 if(frame()==100){effect_at(10,{{255,0,0}},constant(0x6d1a40),constant(0x6d1de8));effect_at(10,{{255,0,0}},constant(0x6d1a40),constant(0x6d1d90));return;}
 if(frame()==120){effect_at(10,{{255,0,255}},constant(0x6d1c58),constant(0x6d1f98));effect_at(10,{{255,0,255}},constant(0x6d1dc8),constant(0x6d1f98));return;}
 if(signed_frame()<160)return;
 if(remainder(frame(),120)==0){s.projectiles.sound_triggers[44]=1;++integer(0);if(integer(0)==4){integer(0)=0;integer(1)-=2;integer(3)-=2;s.allocation.field_a7ea5c=0;s.protection.boss_state_a7ea60=1;}else if(integer(0)==3){s.allocation.field_a7ea5c=255;s.protection.boss_state_a7ea60=0;}}
 --integer(2);if(integer(2)==0){integer(2)=integer(1);
  for(unsigned lane=0;lane<3;++lane){auto value=rnd(0x6d1f20);auto position=m.store(m.add(m.mul(value,m.c(0x6a7708)),m.c(0x6d1a30)));while(ge(m.c(lane==1?0x6d1a28:0x6d1a48),m.q(position))){
   const auto slot=allocate(21,0,lane==0?std::array<std::uint32_t,3>{{0,0,255}}:lane==1?std::array<std::uint32_t,3>{{0,255,0}}:std::array<std::uint32_t,3>{{255,0,0}});
   if(slot!=-1){auto b=bullet(static_cast<std::uint32_t>(slot));b.quad(64,lane==1?position:constant(lane==0?0x6d2d30:0x6d22c0));b.quad(72,lane==1?constant(0x6d22c0):position);b.quad(lane==1?88u:80u,lane==0?constant(0x6aca58):0x3ff0000000000000ull);b.word(8,203+lane);}
   position=m.store(m.add(m.q(position),m.c(0x6d1b88)));
  }}
 }
 --integer(4);if(integer(4)!=0)return;integer(4)=integer(3);
 for(unsigned side=0;side<2;++side){const auto value=rnd(0x6d1e50);auto x=m.store(side==0?m.add(m.mul(value,m.c(0x6a7708)),m.c(0x6d1a30)):m.sub(m.c(0x6d1a28),m.mul(value,m.c(0x6a7708))));auto y=constant(0x6d2d28);
  while(side==0?ge(m.c(0x6d1a28),m.q(x)):le(m.c(0x6d1a30),m.q(x))){const auto slot=allocate(21,0,{{255,0,255}});if(slot!=-1){auto b=bullet(static_cast<std::uint32_t>(slot));b.quad(64,x);b.quad(72,y);b.quad(88,constant(0x6d1ee8));b.word(8,206);}x=m.store(side==0?m.add(m.q(x),m.c(0x6d1898)):m.sub(m.q(x),m.c(0x6d1898)));y=m.store(m.add(m.q(y),m.c(0x6d2410)));}
 }
}
}
