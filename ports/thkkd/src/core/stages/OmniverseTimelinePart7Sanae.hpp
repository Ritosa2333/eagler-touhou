#pragma once
#include "core/stages/OmniverseTimelineEngine.hpp"
namespace kaikeidou::omniverse_timeline_detail {
inline void Engine::part7_sanae(){
 if(frame()==0){name(0x6c5c44);services.configure_phase(1,60,0,166);}else if(frame()==100)effect(12,{{255,255,255}});if(signed_frame()<140)return;
 const auto initialize=[&](std::uint32_t slot,std::uint64_t angle,std::uint32_t speed){auto b=bullet(slot);b.word(8,226);const bool positive=random_integer(1)!=0;auto value=rnd(0x6b25b8);value=m.add(m.mul(value,m.c(0x6a7888)),m.c(0x6d1cf8));if(!positive)value=negated(value);b.quad(160,m.store(value));b.quad(64,constant(0x6d1a60));b.quad(72,constant(0x6d1af0));value=cosine(angle);b.quad(80,m.store(m.mul(value,m.c(speed))));value=sine(angle);b.quad(88,m.store(m.mul(value,m.c(speed))));value=rnd(0x6d1c60);b.quad(96,m.store(m.mul(value,m.c(0x6d1950))));b.word(60,1);b.word(52,1);};
 if(frame()==140){for(unsigned group=0;group<2;++group){const auto value=rnd(0x6d1c60);put(168,m.mul(value,m.c(0x6d1950)));for(unsigned i=0;i<30;++i){const auto slot=allocate(37,group==0?3:2,{{0,0,0}},1,0);if(slot!=-1){const auto angle=m.store(m.add(m.mul(m.i(i),m.c(0x6d2c30)),q(168)));initialize(static_cast<std::uint32_t>(slot),angle,group==0?0x6d1ad8:0x6d1ad0);}}}
  enemy().word(8,0);enemy().quad(56,constant(0x6d2c38));enemy().quad(64,constant(0x6d1a70));integer(0)=10;integer(1)=10;const auto side=random_integer(1);integer(2)=side;
 }
 if(remainder(frame(),45)==0){const auto kind=random_integer(signed_frame()<=1800?2u:4u);boss=static_cast<std::uint32_t>(allocate_enemy(s.enemies));if(boss!=0xffffffffu){enemy().word(4,kind+100u);enemy().word(24,120);enemy().word(20,120);enemy().word(28,0);enemy().word(8,kind+146u);if(integer(2)!=0){enemy().quad(56,constant(0x6d19f0));enemy().quad(72,0x3ff0000000000000ull);}else{enemy().quad(56,constant(0x6d19e8));enemy().quad(72,constant(0x6aca58));}auto value=rnd(signed_frame()<=1800?0x6d2c28:0x6d2c20);put(64,m.add(m.mul(value,m.c(0x6a7708)),m.c(0x6d1898)));const auto age=random_integer(99);enemy().word(12,age);}integer(2)=1u-integer(2);}
 if(remainder(frame(),140)==0){const auto value=rnd(0x6d1c60);put(168,m.mul(value,m.c(0x6d1950)));for(std::uint32_t i=0;signed_word(i)<signed_word(integer(0));++i){const auto slot=allocate(37,3,{{0,0,0}},1,0);if(slot!=-1){const auto angle=m.store(m.add(m.mul(m.div(m.c(0x6d2208),m.i(integer(0))),m.i(i)),q(168)));initialize(static_cast<std::uint32_t>(slot),angle,0x6d1920);}}if(signed_word(integer(0))<26)++integer(0);}
 else if(remainder(frame(),140)==70&&signed_frame()>=1800){const auto value=rnd(0x6d1c60);put(168,m.mul(value,m.c(0x6d1950)));for(std::uint32_t i=0;signed_word(i)<signed_word(integer(1));++i){const auto slot=allocate(37,2,{{0,0,0}},1,0);if(slot!=-1){const auto angle=m.store(m.add(m.mul(m.div(m.c(0x6d2208),m.i(integer(1))),m.i(i)),q(168)));initialize(static_cast<std::uint32_t>(slot),angle,0x6d1920);}}if(signed_word(integer(1))<14)++integer(1);}
 part7_opacity();
}
}
