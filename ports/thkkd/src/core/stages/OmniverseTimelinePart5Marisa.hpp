#pragma once
#include "core/stages/OmniverseTimelineEngine.hpp"
namespace kaikeidou::omniverse_timeline_detail {
inline void Engine::part5_marisa(){
 if(frame()==0){name(0x6c5bac);services.configure_phase(1,60,600,159);s.protection.field_a7eab0=4;return;}
 if(frame()==60){effect_at(10,{{0,0,255}},constant(0x6d1a60),constant(0x6d1cb0));enemy().quad(176,constant(0x6d2090));return;}
 const auto item_header=[&](View b){b.word(0,1);b.word(16,3);b.word(4,50);b.word(12,0);};const auto item_motion=[&](View b){b.quad(40,0);b.quad(48,0);b.quad(56,constant(0x6d1ad0));};
 if(frame()==100){secondary(0).word(72,1);for(unsigned col=0;col<5;++col)for(unsigned row=0;row<5;++row){const auto slot=find_free_item(s.items);if(slot==-1)continue;auto b=item(static_cast<std::uint32_t>(slot));item_header(b);b.word(8,col==0||col==4?3u:col==1||col==3?0u:4u);b.quad(24,m.store(m.i(172+col*10)));b.quad(32,m.store(m.i(0u-20u-row*10)));item_motion(b);}integer(0)=0;integer(1)=0;integer(2)=0;integer(3)=0;return;}
 if(signed_frame()<160)return;
 if(frame()==160){enemy().quad(56,constant(0x6d2090));enemy().quad(168,enemy().quad(56));}
 else if(frame()==260){std::uint64_t direction=0;if(s.phase.weapon==0){std::uint64_t high=0,low=constant(0x6d1cb0);for(unsigned i=0;i<4;++i){const auto x=s.options.options[i].quad(8);if(lt(m.q(high),m.q(x)))high=x;if(gt(m.q(low),m.q(x)))low=x;}direction=m.store(m.sub(m.sub(m.q(high),m.c(0x6d1a60)),m.sub(m.c(0x6d1a60),m.q(low))));if(compare(m.q(0),m.q(direction))==0){auto value=random.real_number(0x3ff0000000000000ull,precision);direction=m.store(m.sub(m.add(value,value),m.c(0x6d1868)));}}
  else{auto value=random.real_number(0x3ff0000000000000ull,precision);direction=m.store(m.sub(m.add(value,value),m.c(0x6d1868)));}
  if(lt(m.q(0),m.q(direction))){enemy().quad(56,constant(0x6d2090));enemy().quad(168,constant(0x6b25f8));}else{enemy().quad(56,constant(0x6d23d8));enemy().quad(168,constant(0x6d2288));}enemy().quad(64,constant(0x6d1a38));enemy().quad(176,constant(0x6d1a38));s.presentation.spell_ring_reals[0]=enemy().quad(56);s.presentation.spell_ring_reals[1]=enemy().quad(64);
 }
 if(remainder(frame(),20)==0){const auto slot=find_free_item(s.items);if(slot!=-1){auto b=item(static_cast<std::uint32_t>(slot));item_header(b);b.word(8,integer(0)==0?0u:integer(0)==1?3u:4u);b.quad(24,s.player.quad(144));b.quad(32,constant(0x6d19f0));item_motion(b);}
  const auto color=integer(0)==0?std::array<std::uint32_t,3>{{255,0,0}}:integer(0)==1?std::array<std::uint32_t,3>{{0,0,255}}:std::array<std::uint32_t,3>{{0,255,0}};const auto y=constant(0x6d19f0),x=s.player.quad(144);emit({{9,2,color,2,255,0,41},x,y,1,0,constant(0x6d1ad0),constant(0x6d1b60),1,0});const auto old=integer(0);++integer(0);if(old==2)integer(0)=0;
 }
 if(remainder(frame(),80)==40){const auto slot=find_free_item(s.items);if(slot!=-1){auto b=item(static_cast<std::uint32_t>(slot));item_header(b);if(integer(2)==0){b.word(8,5);b.quad(24,constant(0x6b25b8));}else{b.word(8,7);b.quad(24,constant(0x6d20f0));}b.quad(32,constant(0x6d1ff0));item_motion(b);auto value=rnd(0x6d1c60);auto angle=m.store(m.mul(value,m.c(0x6d1950)));const bool first=integer(2)==0;const auto color=first?std::array<std::uint32_t,3>{{0,255,255}}:std::array<std::uint32_t,3>{{255,255,0}};
   for(unsigned i=0;i<9;++i){value=sine(angle);const auto y=m.store(m.add(m.mul(value,m.c(0x6d1aa8)),m.q(b.quad(32))));value=cosine(angle);const auto x=m.store(m.add(m.mul(value,m.c(0x6d1aa8)),m.q(b.quad(24))));emit({{1,2,color,2,255,0,41},x,y,1,0,constant(0x6d1ad0),constant(0x6d1b60),1,0});angle=m.store(m.add(m.q(angle),m.c(0x6d2290)));}
   const auto y=b.quad(32),x=b.quad(24);emit({{2,2,color,2,255,0,41},x,y,1,0,constant(0x6d1ad0),constant(0x6d1b60),1,0});
  }integer(2)=1u-integer(2);
 }
 auto speed=constant(0x6d1948);while(signed_word(integer(1))>=1){const auto y=enemy().quad(64),x=enemy().quad(56);emit({{10,1,{{255,0,127}},0,255,0,41},x,y,4,constant(0x6d2210),speed,0,0,0});--integer(1);speed=m.store(m.add(m.q(speed),m.c(0x6d1920)));}
 speed=constant(0x6d1c80);while(signed_word(integer(3))>=1){const auto y=enemy().quad(64),x=enemy().quad(56);emit({{11,1,{{127,0,255}},0,255,0,41},x,y,3,constant(0x6d19c0),speed,0,0,0});--integer(3);speed=m.store(m.add(m.q(speed),m.c(0x6d19d8)));put(176,m.sub(q(176),m.c(0x6d19d8)));}
}
}
