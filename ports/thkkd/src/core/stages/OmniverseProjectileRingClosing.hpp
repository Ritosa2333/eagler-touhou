#pragma once
#include "core/stages/OmniverseBehaviorEngine.hpp"
namespace kaikeidou::omniverse_behavior_detail {
inline void Engine::ring_groups(std::uint32_t phase){
 auto b=object();const auto rings=[&](unsigned arms,unsigned radial,std::uint32_t angle_step,std::uint32_t shrink,bool spiral){auto value=rnd(0x6d1c60);auto angle=m.store(m.mul(value,m.c(0x6d1950)));
  for(unsigned i=0;i<arms;++i){for(unsigned j=0;j<radial;++j){const auto slot=allocate(34,0,{{0,0,0}});if(slot==-1)continue;auto child=bullet(static_cast<std::uint32_t>(slot));child.quad(160,b.quad(64));child.quad(168,b.quad(72));child.quad(176,spiral?m.store(m.add(m.mul(m.i(j),m.c(0x6d1b70)),m.q(angle))):angle);child.quad(184,spiral?constant(0x6d1a78):m.store(m.add(m.mul(m.i(j),m.c(0x6d2400)),m.c(0x6d1a78))));child.quad(192,spiral?m.store(m.mul(m.i(b.word(124)),m.c(0x6d1d48))):0);child.quad(200,constant(shrink));finish_ring_child(child);}angle=m.store(m.add(m.q(angle),m.c(angle_step)));}
 };
 if(phase==3){if(remainder(age(),80)!=0)return;rings(14,8,0x6d27e0,0x6d2150,false);if(signed_word(frame())>=2800){b.word(12,0xffffffb0u);b.word(120,4);const auto n=random_integer(1);b.word(124,n*2u-1u);}return;}
 if(signed_age()<0)return;
 if(phase==4){if(remainder(age(),112)!=0)return;rings(5,11,0x6d2618,0x6d2390,true);b.word(124,0u-b.word(124));if(signed_word(frame())>=3800){b.word(12,0xffffff88u);b.word(120,5);const auto n=random_integer(1);b.word(124,n*2u-1u);}return;}
 if(remainder(age(),140)==0)rings(11,10,0x6d27b0,0x6d27b8,false);
 if(remainder(age(),112)!=40)return;rings(5,11,0x6d2618,0x6d2390,true);b.word(124,0u-b.word(124));if(signed_word(frame())>=4800){b.word(12,0xffffff56u);b.word(120,6);}
}
inline void Engine::ring_final(){
 auto b=object();if(age()==0xffffffceu){const auto y=b.quad(72),x=b.quad(64);ring_effect(0,x,y);for(auto a:{0x6b25f8u,0x6d2288u,0x6d1a60u})ring_effect(0,constant(a),constant(0x6d1f98));}
 if(signed_age()<0||remainder(age(),8)!=0)return;auto angle=constant(0x6d1b60);for(unsigned i=0;i<36;++i){const auto slot=allocate(34,0,{{0,0,0}});if(slot!=-1){auto child=bullet(static_cast<std::uint32_t>(slot));child.quad(160,b.quad(64));child.quad(168,b.quad(72));child.quad(176,angle);child.quad(184,constant(0x6d1a78));child.quad(192,0);child.quad(200,constant(0x6d2130));finish_ring_child(child);}angle=m.store(m.add(m.q(angle),m.c(0x6d27a8)));}
}
inline void Engine::projectile219(){switch(object().word(120)){case 0:ring_initial();break;case 1:ring_waves();break;case 2:ring_fade();break;case 3:case 4:case 5:ring_groups(object().word(120));break;case 6:ring_final();break;default:break;}}
}
