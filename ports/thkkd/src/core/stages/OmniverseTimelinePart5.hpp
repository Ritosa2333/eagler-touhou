#pragma once
#include "core/stages/OmniverseTimelineEngine.hpp"
namespace kaikeidou::omniverse_timeline_detail {
inline void Engine::part5(){
 if(t().subphase==0){
  if(character()!=0&&character()!=1){begin(5,10,0x6d2d20);return;}
  const bool marisa=character()==1;
  if(frame()==1){begin(5,10,marisa?0x6d1d00:0x6d1af0);}
  else if(frame()==60){auto b=secondary(0);b.word(0,1);s.projectiles.sound_triggers[18]=1;b.quad(16,constant(0x6d1a60));b.quad(24,constant(marisa?0x6d1cb0:0x6d1af0));b.quad(32,constant(0x6d1e38));b.quad(40,constant(marisa?0x6d1e48:0x6d2580));b.quad(48,marisa?constant(0x6d1920):0x3ff0000000000000ull);b.quad(56,constant(marisa?0x6d19c0:0x6d1920));b.quad(64,marisa?0x3ff0000000000000ull:constant(0x6d1920));b.word(4,marisa?208u:80u);b.word(12,0);b.word(72,0);enemy().word(44,3);enemy().word(48,0);enemy().word(52,0);}
  else if(frame()==140)services.begin_boss_effect();else if(frame()==190){if(!marisa)secondary(0).word(72,1);secondary(0).quad(32,secondary(0).quad(40));t().subphase=1;reset(0xffffffffu);}return;
 }
 if(t().subphase!=1)return;if(character()==0)part5_reimu();else if(character()==1)part5_marisa();else part5_sanae();
}
inline void Engine::part5_reimu(){
 if(frame()==0){name(0x6c5b94);services.configure_phase(1,60,28000,158);real(0)=constant(0x6d1de0);real(1)=0;real(2)=constant(0x6d1de0);integer(0)=0;return;}
 if(frame()==60){effect(12,{{255,0,0}});const auto y=s.player.quad(152),x=s.player.quad(144);effect_at(13,{{255,0,255}},x,y);return;}if(signed_frame()<100)return;
 if(remainder(frame(),5)==0){const auto slot=allocate(9,0,{{255,0,0}},2,1);if(slot!=-1){auto b=bullet(static_cast<std::uint32_t>(slot));b.quad(160,real(2));b.quad(168,constant(0x6d2580));b.quad(176,real(1));b.quad(184,0);b.quad(192,0);b.word(120,0);if(remainder(frame(),10)==0)b.word(124,0);else if(remainder(frame(),10)==5)b.word(124,1);auto value=cosine(b.quad(160));b.quad(64,m.store(m.add(m.mul(value,m.q(b.quad(168))),m.c(0x6d1a60))));value=sine(b.quad(160));b.quad(72,m.store(m.add(m.mul(value,m.q(b.quad(168))),m.c(0x6d1af0))));b.word(8,207);b.word(52,1);}real(2)=m.store(m.sub(m.q(real(2)),m.c(0x6d2d18)));}
 if(remainder(frame(),25)==0){const auto slot=allocate(9,0,{{255,0,255}},2,1);if(slot!=-1){auto b=bullet(static_cast<std::uint32_t>(slot));const auto dx=m.store(m.sub(m.q(s.player.quad(144)),m.c(0x6d1a60))),dy=m.store(m.sub(m.q(s.player.quad(152)),m.c(0x6d1af0)));b.quad(160,m.store(math.atan2(dy,dx)));b.quad(168,constant(0x6d2240));b.quad(176,real(1));b.quad(184,0);b.quad(192,0);b.word(120,0);b.word(124,2);auto value=cosine(b.quad(160));b.quad(64,m.store(m.add(m.mul(value,m.q(b.quad(168))),m.c(0x6d1a60))));value=sine(b.quad(160));b.quad(72,m.store(m.add(m.mul(value,m.q(b.quad(168))),m.c(0x6d1af0))));b.word(8,207);b.word(52,1);}}
 real(0)=m.store(m.sub(m.q(real(0)),m.c(0x6d1810)));real(1)=m.store(m.add(m.q(real(1)),m.c(0x6d1928)));
}
}
