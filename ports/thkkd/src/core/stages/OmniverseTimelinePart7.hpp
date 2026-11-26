#pragma once
#include "core/stages/OmniverseTimelineEngine.hpp"
namespace kaikeidou::omniverse_timeline_detail {
inline void Engine::part7(){
 if(t().subphase==0){if(frame()==1){if(s.player.word(36)==0)services.music(14,1);boss=static_cast<std::uint32_t>(services.allocate_boss(7));const auto x=omniverse_timeline_character(s.lifecycle.character,{{192,999999,192}});put(56,m.i(x));const auto y=omniverse_timeline_character(s.lifecycle.character,{{0u-30u,0u-999999u,0u-30u}});put(64,m.i(y));if(character()==1)enemy().word(8,0);else{enemy().word(8,14);enemy().quad(168,constant(0x6d1a60));const auto target=omniverse_timeline_character(s.lifecycle.character,{{80,80,224}});put(176,m.i(target));}}
  else if(frame()==10){if(character()!=1)services.begin_boss_effect();}else if(frame()==60){t().subphase=1;reset(0xffffffffu);}return;}
 if(t().subphase!=1)return;if(character()==0)part7_reimu();else if(character()==1)part7_marisa();else part7_sanae();
}
inline void Engine::part7_reimu(){
 if(frame()==0){name(0xb410e6);services.configure_phase(1,90,0,164);return;}if(frame()==100){effect(12,{{255,255,255}});return;}if(frame()!=140)return;
 effect(14,{{255,255,255}});const auto slot=allocate(18,0,{{255,255,255}},0,1);if(slot!=-1){auto b=bullet(static_cast<std::uint32_t>(slot));b.quad(64,enemy().quad(56));b.quad(72,enemy().quad(64));const auto value=rnd(0x6d1c60);b.quad(160,m.store(m.mul(value,m.c(0x6d1950))));b.word(120,0);b.word(8,219);b.word(52,1);}
 for(unsigned i=0;i<12;++i)for(unsigned arm=0;arm<5;++arm){const auto speed=m.store(m.add(m.mul(m.i(i),m.c(0x6d19c0)),m.c(0x6d1c90)));const auto y=arm==0?enemy().quad(64):m.store(m.add(q(64),m.c(arm<3?0x6d18e8:0x6d1a40)));const auto x=arm==0?enemy().quad(56):m.store(arm%2?m.add(q(56),m.c(arm<3?0x6d1e78:0x6d1a90)):m.sub(q(56),m.c(arm<3?0x6d1e78:0x6d1a90)));emit({{11,0,{{240,240,240}},0,255,1,41},x,y,1,0,speed,0,0,0});}
 enemy().word(8,0);enemy().quad(56,constant(0x6d2c38));enemy().quad(64,constant(0x6d1a70));
}
inline void Engine::part7_marisa(){
 if(frame()==0){name(0x6c5c2c);services.configure_phase(1,60,0,165);s.phase.spell_overlay=0;integer(1)=0;integer(2)=41;integer(3)=1;}
 else if(frame()==100)effect_at(10,{{255,255,255}},constant(0x6d1a60),constant(0x6d1f08));
 else if(frame()==140){effect_at(14,{{255,255,255}},constant(0x6d1a60),constant(0x6d1f08));const auto slot=allocate(30,0,{{255,255,255}},0,1);if(slot!=-1){auto b=bullet(static_cast<std::uint32_t>(slot));b.quad(64,constant(0x6d1a60));b.quad(72,constant(0x6d1f08));b.word(8,224);b.word(52,1);b.quad(96,0x3ff0000000000000ull);}}
 else if(frame()==300)effect_at(10,{{255,255,255}},constant(0x6d1a60),constant(0x6d1f08));
 if(signed_frame()<200||signed_word(integer(2))<1)return;--integer(2);if(integer(2)==40)effect_at(10,{{0,0,0}},constant(0x6d1a60),constant(0x6d1f08));else if(integer(2)==0){effect_at(14,{{0,0,0}},constant(0x6d1a60),constant(0x6d1f08));const auto slot=static_cast<std::uint32_t>(services.allocate_boss(7));auto e=enemy_at(slot);e.word(8,144);e.quad(56,constant(0x6d1a60));e.quad(64,constant(0x6d1f08));e.word(24,50);e.word(20,50);e.word(128,38);e.word(132,0);integer(0)=slot;s.lifecycle.boss_state=2;s.phase.boss_index=0;}
}
}
