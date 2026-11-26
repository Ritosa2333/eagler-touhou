#pragma once
#include "core/stages/OmniverseTimelineEngine.hpp"
namespace kaikeidou::omniverse_timeline_detail {
inline void Engine::part5_sanae(){
 if(signed_frame()>=100){if(signed_frame()>=380){const auto target=s.player.quad(144);omniverse_timeline_ease_object(enemy(),168,target,constant(0x6d1d00),m.precision);auto value=sine(enemy().quad(184));put(176,m.add(m.mul(value,m.c(0x6d2240)),m.c(0x6d1af0)));put(184,m.add(q(184),q(192)));if(ge(m.c(0x6d1b48),q(192)))put(192,m.add(q(192),m.c(0x6a7888)));}const auto dx=m.store(m.sub(m.q(s.player.quad(144)),q(56))),dy=m.store(m.sub(m.q(s.player.quad(152)),q(64)));real(0)=m.store(math.atan2(dy,dx));}
 if(frame()==0){name(0x6c5bc0);services.configure_phase(1,60,20000,160);s.protection.field_a7eab0=4;enemy().quad(184,constant(0x6d1de0));enemy().quad(192,0);enemy().quad(168,constant(0x6d1a60));auto value=sine(enemy().quad(184));put(176,m.add(m.mul(value,m.c(0x6d2240)),m.c(0x6d1af0)));return;}
 if(frame()==60){effect(12,{{255,255,255}});return;}
 if(frame()==100){
  constexpr std::uint32_t radii[6]{0x6d2d08,0x6d2d08,0x6d2cf0,0x6d2cf0,0x6d2cd8,0x6d2cd8};constexpr std::uint32_t angles[6]{0x6d2d00,0x6d2cf8,0x6d2ce8,0x6d2ce0,0x6d1990,0x6d2cd0};
  for(unsigned group=0;group<3;++group){const std::array<std::uint32_t,3> color=group==0?std::array<std::uint32_t,3>{{0,127,255}}:group==1?std::array<std::uint32_t,3>{{127,255,0}}:std::array<std::uint32_t,3>{{255,0,127}};const auto rotation=group==0?constant(0x6d2d10):group==1?0:constant(0x6d2138);
   for(unsigned shape=0;shape<10;++shape){const auto slot=allocate(11,1,color);if(slot==-1)continue;auto b=bullet(static_cast<std::uint32_t>(slot));b.quad(160,shape<6?constant(radii[shape]):m.store(m.i((shape-5)*26u)));b.quad(168,shape<6?constant(angles[shape]):0);b.quad(176,rotation);auto value=cosine(m.store(m.add(m.add(m.q(real(0)),m.q(b.quad(176))),m.q(b.quad(168)))));b.quad(64,m.store(m.add(m.mul(value,m.q(b.quad(160))),q(56))));value=sine(m.store(m.add(m.add(m.q(real(0)),m.q(b.quad(176))),m.q(b.quad(168)))));b.quad(72,m.store(m.add(m.mul(value,m.q(b.quad(160))),q(64))));b.word(8,156);b.word(52,1);}
  }
  const auto dx=m.store(m.sub(m.q(s.player.quad(144)),q(56))),dy=m.store(m.sub(m.q(s.player.quad(152)),q(64)));auto angle=m.store(math.atan2(dy,dx));
  for(unsigned i=0;i<12;++i){const auto slot=allocate(1,0,{{200,0,0}},0,1);if(slot!=-1){auto b=bullet(static_cast<std::uint32_t>(slot));b.quad(64,enemy().quad(56));b.quad(72,enemy().quad(64));b.word(8,211);auto value=cosine(angle);b.quad(80,m.store(value));value=sine(angle);b.quad(88,m.store(value));b.quad(104,0);b.quad(160,constant(0x6b25f8));b.quad(168,constant(0x6d1ad8));}angle=m.store(m.add(m.q(angle),m.c(0x6d2140)));}return;
 }
 if(frame()==130){effect(12,{{0,127,255}});return;}if(frame()==180){effect(12,{{127,255,0}});return;}if(frame()==230){effect(12,{{255,0,127}});return;}
 if(frame()!=170&&frame()!=220&&frame()!=270)return;const unsigned group=frame()==170?0u:frame()==220?1u:2u;const auto first=random_integer(1);const auto angle=first?0:constant(0x6d1860);
 constexpr std::uint32_t rates[3]{0x6d1900,0x6d18e0,0x6d1920},speeds[3]{0x6d1c90,0x6d1910,0x6d18e8},scales[3]{0x6d2cc8,0x6d2cc0,0x6d2cb8};
 for(unsigned i=0;i<50;++i){const auto slot=allocate(15,0,group==0?std::array<std::uint32_t,3>{{16,32,64}}:group==1?std::array<std::uint32_t,3>{{32,64,16}}:std::array<std::uint32_t,3>{{64,16,32}},2,0);
  if(slot!=-1){auto b=bullet(static_cast<std::uint32_t>(slot));b.quad(160,angle);b.quad(168,group==0?constant(0x6d2d10):group==1?0:constant(0x6d2138));b.quad(176,constant(rates[group]));b.quad(184,constant(speeds[group]));b.quad(64,enemy().quad(56));b.quad(72,enemy().quad(64));b.quad(104,constant(scales[group]));b.quad(96,group==0?m.store(m.sub(m.q(real(0)),m.c(0x6d2138))):group==1?real(0):m.store(m.add(m.q(real(0)),m.c(0x6d2138))));b.word(8,i==0?209u:210u);b.word(120,i==0?group*72u:integer(0));b.word(12,0u-i*(group==0?1u:group==1?2u:7u));b.word(52,1);}integer(0)=static_cast<std::uint32_t>(slot);
 }
}
}
