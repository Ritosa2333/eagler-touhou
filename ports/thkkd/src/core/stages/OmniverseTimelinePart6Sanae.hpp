#pragma once
#include "core/stages/OmniverseTimelineEngine.hpp"
namespace kaikeidou::omniverse_timeline_detail {
inline void Engine::part6_sanae(){
 if(frame()==0){name(0x6c5c00);services.configure_phase(1,60,0,163);}
 else if(frame()==60)effect(12,{{0,0,255}});
 else if(frame()==100){real(0)=constant(0x6d1a60);real(1)=constant(0x6d1918);real(2)=0;const auto slot=allocate(18,0,{{0,0,255}},2,1);if(slot!=-1){auto b=bullet(static_cast<std::uint32_t>(slot));b.quad(64,enemy().quad(56));b.quad(72,enemy().quad(64));b.word(8,214);b.word(52,1);b.quad(104,0);const auto value=random_integer(1);b.word(120,value);b.quad(160,b.word(120)!=0?constant(0x6d1860):0);b.quad(168,0);}enemy().word(44,0);}
 else if(frame()==110)enemy().quad(176,constant(0x6d20d8));
 else if(frame()==200){enemy().quad(56,constant(0x6d2090));enemy().quad(168,enemy().quad(56));}
 else if(frame()==1500){real(1)=constant(0x6d2ad8);real(2)=0;}
 else if(frame()==1600){enemy().quad(56,constant(0x6d1a60));enemy().quad(64,constant(0x6d2090));enemy().quad(168,enemy().quad(56));enemy().quad(176,constant(0x6d1a90));s.presentation.spell_ring_reals[0]=enemy().quad(56);s.presentation.spell_ring_reals[1]=enemy().quad(64);}
 else if(frame()==1660)effect(12,{{255,0,0}});
 else if(frame()==1700){const auto slot=allocate(18,0,{{255,0,0}},2,1);if(slot!=-1){auto b=bullet(static_cast<std::uint32_t>(slot));b.quad(64,enemy().quad(56));b.quad(72,enemy().quad(64));b.word(8,216);b.word(52,1);b.quad(160,constant(0x6d1ba8));b.quad(168,b.quad(64));b.quad(176,b.quad(72));b.quad(184,constant(0x6d18e0));b.word(120,90);}enemy().word(44,0);}
 else if(frame()==1710)enemy().quad(176,constant(0x6d2090));
 else if(frame()==2900){put(56,m.i(le(m.c(0x6d1a60),m.q(s.player.quad(144)))?0u-200u:584u));enemy().quad(64,constant(0x6d1a60));put(168,m.i(ge(m.c(0x6d1a60),q(56))?30u:354u));enemy().quad(176,enemy().quad(64));s.presentation.spell_ring_reals[0]=enemy().quad(56);s.presentation.spell_ring_reals[1]=enemy().quad(64);}
 else if(frame()==2960)effect(12,{{0,255,0}});
 else if(frame()==3000){const auto slot=allocate(18,0,{{0,255,0}},2,1);if(slot!=-1){auto b=bullet(static_cast<std::uint32_t>(slot));b.quad(64,enemy().quad(56));b.quad(72,enemy().quad(64));b.word(8,218);b.word(52,1);}enemy().word(44,0);}
 if(signed_frame()>=100){real(1)=m.store(m.mul(m.q(real(1)),m.c(0x6d2c40)));if(signed_frame()<1500)real(2)=m.store(m.add(m.q(real(2)),m.c(0x6d1f10)));}
}
}
