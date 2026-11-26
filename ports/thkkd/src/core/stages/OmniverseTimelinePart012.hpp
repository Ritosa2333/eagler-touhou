#pragma once
#include "core/stages/OmniverseTimelineEngine.hpp"
namespace kaikeidou::omniverse_timeline_detail {
inline void Engine::part0(){
 if(t().subphase==0){begin(0,2,0x6d1cc0);return;}if(t().subphase!=1||frame()!=0)return;
 name(0x6c5b28);services.configure_phase(1,60,6000,153);enemy().word(44,0);enemy().word(8,139);enemy().word(12,260);integer(0)=0;
}
inline void Engine::part1(){
 if(t().subphase==0){begin(1,4,0x6d1a88);return;}if(t().subphase!=1)return;
 if(frame()==0){name(0x6c5b40);services.configure_phase(1,60,36000,154);s.protection.field_a7eab0=5;return;}
 if(frame()==60){effect(12,{{255,255,255}});real(0)=constant(0x6d19c0);real(1)=constant(0x6d2878);real(2)=constant(0x6d1968);real(3)=0x3ff0000000000000ull;real(4)=constant(0x6d1cc0);integer(0)=5;integer(1)=255;integer(2)=0;integer(3)=5;integer(4)=55;return;}
 if(signed_frame()<100)return;
 if(frame()==100){enemy().word(8,140);enemy().quad(168,0);}
 switch(integer(0)){
 case 0:integer(2)+=5;if(integer(2)==255)integer(0)=1;break;
 case 1:integer(1)-=5;if(integer(1)==0)integer(0)=2;break;
 case 2:integer(3)+=5;if(integer(3)==255)integer(0)=3;break;
 case 3:integer(2)-=5;if(integer(2)==0)integer(0)=4;break;
 case 4:integer(1)+=5;if(integer(1)==255)integer(0)=5;break;
 case 5:integer(3)-=5;if(integer(3)==0)integer(0)=0;break;
 default:break;
 }
 if(remainder(frame(),5)==0){
  auto slot=allocate(0,0,{{integer(1),integer(2),integer(3)}});
  if(slot!=-1){auto b=bullet(static_cast<std::uint32_t>(slot));auto value=sine(real(0));b.quad(64,m.store(m.add(m.mul(value,m.c(0x6d1a60)),m.c(0x6d1a60))));b.quad(72,constant(0x6d19f0));b.word(8,196);b.quad(160,real(0));b.quad(168,remainder(frame(),80)<=signed_word(integer(4))?0:real(4));b.word(120,1);}
  slot=allocate(0,0,{{integer(1),integer(2),integer(3)}});
  if(slot!=-1){auto b=bullet(static_cast<std::uint32_t>(slot));auto value=sine(real(0)^0x8000000000000000ull);b.quad(64,m.store(m.add(m.mul(value,m.c(0x6d1a60)),m.c(0x6d1a60))));b.quad(72,constant(0x6d19f8));b.word(8,196);b.quad(160,real(0)^0x8000000000000000ull);const auto low=signed_word(80u-integer(4));b.quad(168,remainder(frame(),80)>=low&&remainder(frame(),80)<=80?0:real(4));b.word(120,0xffffffffu);}
 }
 real(0)=m.store(m.add(m.q(real(0)),m.q(real(1))));real(4)=m.store(m.add(m.q(real(4)),m.c(0x6d1a08)));if(frame()==1700)integer(4)=60;
}
inline void Engine::part2(){
 if(t().subphase==0){begin(2,5,0x6b25b8);return;}if(t().subphase!=1||frame()!=0)return;
 name(0x6c5b54);services.configure_phase(1,60,37000,155);enemy().word(8,141);enemy().quad(168,constant(0x6d1a60));enemy().quad(176,constant(0x6d1a00));enemy().word(128,0);
}
}
