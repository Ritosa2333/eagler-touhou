#pragma once
#include "core/stages/OmniverseTimelineEngine.hpp"
namespace kaikeidou::omniverse_timeline_detail {
inline void Engine::part3(){
 if(t().subphase==0){begin(3,6,0x6d1a88);return;}if(t().subphase!=1)return;
 if(frame()==0){name(0x6c5b68);services.configure_phase(1,60,14000,156);s.protection.field_a7eab0=6;return;}
 if(frame()==60){effect(12,{{0,255,0}});return;}if(frame()==80){effect(12,{{0,0,255}});return;}if(frame()==100){effect(12,{{255,0,0}});return;}
 if(frame()==140){
  constexpr std::uint32_t speeds[6]{0x6d1ae8,0x6d2d48,0x6d1fe8,0x6d2d40,0x6d1f60,0x6d2d38};
  constexpr std::uint32_t limits[4]{0x6d2d50,0x6d2528,0x6d27b8,0x6d18f0};
  for(unsigned i=0;i<6;++i){const auto slot=allocate(18,1,i<2?std::array<std::uint32_t,3>{{0,255,0}}:i<4?std::array<std::uint32_t,3>{{0,0,255}}:std::array<std::uint32_t,3>{{255,0,0}},2,1);if(slot==-1)continue;auto b=bullet(static_cast<std::uint32_t>(slot));
   b.quad(160,constant(0x6d1b60));b.quad(168,0);b.quad(176,0);b.quad(184,constant(speeds[i]));if(i<4)b.quad(192,constant(limits[i]));b.word(120,i<4?(i%2?0xffffffffu:1u):(i==4?0xffffffffu:1u));
   auto value=cosine(b.quad(160));if(i>=2&&i<4)value=cosine(m.store(m.add(m.mul(value,m.c(0x6d1ad0)),m.c(0x6d1b60))));
   b.quad(64,m.store(m.add(m.mul(value,i<4?m.q(b.quad(168)):m.c(0x6d1ab0)),m.c(0x6d1a60))));
   if(i<4){if(i>=2)value=cosine(b.quad(160));value=sine(i<2?b.quad(160):m.store(m.add(m.mul(value,m.c(0x6d1ad0)),m.c(0x6d1b60))));b.quad(72,m.store(m.add(m.mul(value,m.q(b.quad(168))),m.c(0x6d1a88))));}
   else b.quad(72,m.store(m.add(m.c(0x6d1a88),m.q(b.quad(168)))));
   b.quad(96,constant(0x6d1b60));b.word(8,199+i/2);b.word(52,1);
  }return;
 }
 if(signed_frame()<500)return;
 if(remainder(frame(),200)==100){effect(12,{{255,255,0}});return;}
 if(remainder(frame(),200)<140||remainder(frame(),200)>169)return;
 if(remainder(frame(),200)==140){const auto value=random_integer(1);real(0)=value?0:constant(0x6d1860);const auto dx=m.store(m.sub(m.q(s.player.quad(144)),q(56)));const auto dy=m.store(m.sub(m.q(s.player.quad(152)),q(64)));real(1)=m.store(math.atan2(dy,dx));}
 real(0)=m.store(m.add(m.q(real(0)),m.c(0x6d1960)));const auto slot=allocate(15,3,{{64,64,0}},2,0);if(slot==-1)return;auto b=bullet(static_cast<std::uint32_t>(slot));
 b.quad(160,real(0));b.quad(168,real(1));b.quad(176,constant(0x6d1920));b.quad(184,constant(0x6d1948));b.quad(64,enemy().quad(56));b.quad(72,enemy().quad(64));b.quad(104,constant(0x6d1908));b.quad(96,real(1));b.word(8,212);
}
}
