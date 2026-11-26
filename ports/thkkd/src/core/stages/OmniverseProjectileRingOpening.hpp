#pragma once
#include "core/stages/OmniverseBehaviorEngine.hpp"
namespace kaikeidou::omniverse_behavior_detail {
inline void Engine::ring_initial(){
 auto b=object();if(remainder(age(),40)>19)return;auto angle=b.quad(160);const auto radius=m.store(m.sub(m.c(0x6d1ae8),m.mul(m.i(static_cast<std::uint32_t>(remainder(age(),40))),m.c(0x6d2488))));
 for(unsigned i=0;i<16;++i){const auto slot=allocate(0,0,{{240,240,240}});if(slot!=-1){auto child=bullet(static_cast<std::uint32_t>(slot));auto value=cosine(angle);child.quad(64,m.store(m.add(m.mul(value,m.q(radius)),q(64))));value=sine(angle);child.quad(72,m.store(m.add(m.mul(value,m.q(radius)),q(72))));child.word(8,220);value=cosine(angle);child.quad(80,m.store(m.mul(value,m.c(0x6d2480))));value=sine(angle);child.quad(88,m.store(m.mul(value,m.c(0x6d2480))));child.quad(104,0x3ff0000000000000ull);}angle=m.store(m.add(m.q(angle),m.c(0x6d27f8)));}
 if(remainder(age(),40)!=19)return;if(random_integer(1))put(160,m.add(q(160),m.c(0x6d27f0)));else{const auto positive=random_integer(1);const auto value=rnd(0x6d27e8);const auto change=m.mul(value,m.c(0x6d1950));put(160,positive?m.add(change,q(160)):m.sub(q(160),change));}
 if(signed_word(frame())>=800){b.word(12,0xffffffd8u);b.word(120,1);b.word(124,0);}
}
inline void Engine::ring_waves(){
 auto b=object();if(signed_age()>=0&&remainder(age(),34)==0){auto value=rnd(0x6d1c60);auto angle=m.store(m.mul(value,m.c(0x6d1950)));const auto radius=constant(0x6d1828);
  for(unsigned i=0;i<14;++i){const auto slot=allocate(0,0,{{240,240,240}});if(slot!=-1){auto child=bullet(static_cast<std::uint32_t>(slot));value=cosine(angle);child.quad(64,m.store(m.add(m.mul(value,m.q(radius)),q(64))));value=sine(angle);child.quad(72,m.store(m.add(m.mul(value,m.q(radius)),q(72))));child.word(8,220);value=cosine(angle);child.quad(80,m.store(m.mul(value,m.c(0x6d1880))));value=sine(angle);child.quad(88,m.store(m.mul(value,m.c(0x6d1880))));child.quad(104,0x3ff0000000000000ull);}angle=m.store(m.add(m.q(angle),m.c(0x6d27e0)));}
 }
 if(signed_age()<120)return;const auto part=remainder(age(),120);unsigned group;if(part>=0&&part<=19)group=0;else if(part>=20&&part<=39)group=1;else if(part>=60&&part<=79)group=2;else if(part>=80&&part<=99)group=3;else return;
 constexpr unsigned resets[4]{0,30,60,90};if(part==static_cast<std::int64_t>(resets[group])){auto value=rnd(0x6b25b8);auto change=m.mul(value,m.c(0x6d1950));put(160,group==0?m.add(change,m.q(0)):group==1?m.sub(m.c(0x6d25b8),change):group==2?m.sub(m.c(0x6d1860),change):m.add(change,m.c(0x6d27d0)));value=rnd(0x6b25b8);put(168,m.mul(value,m.c(0x6d1b40)));}
 const auto slot=allocate(0,0,{{240,240,240}});if(slot!=-1){auto child=bullet(static_cast<std::uint32_t>(slot));child.quad(160,b.quad(160));child.quad(168,b.quad(168));child.quad(176,m.store(m.add(m.mul(m.i(19u-(static_cast<std::uint32_t>(remainder(age(),120))-resets[group])),m.c(0x6d27d8)),m.c(0x6d1918))));child.word(120,group<2?1u:0xffffffffu);auto value=cosine(child.quad(160));child.quad(64,m.store(m.add(m.mul(value,m.c(0x6d1ae8)),q(64))));value=sine(child.quad(160));child.quad(72,m.store(m.add(m.mul(value,m.c(0x6d1ae8)),q(72))));child.word(8,221);child.quad(104,constant(0x6d1b28));}
 if(group==3&&remainder(age(),120)==99&&signed_age()>=1100){b.word(12,0xffffffffu);b.word(120,2);}
}
inline void Engine::ring_fade(){
 auto b=object();if(age()==60){const auto y=b.quad(72),x=b.quad(64);ring_effect(255,x,y);}else if(age()==103){const auto y=b.quad(72),x=b.quad(64);ring_effect(127,x,y);}else if(age()==147){const auto y=b.quad(72),x=b.quad(64);ring_effect(0,x,y);for(auto a:{0x6b25f8u,0x6d2288u,0x6d1a60u})ring_effect(0,constant(a),constant(0x6d1f98));}
 if(signed_age()<60)return;const auto shade=255u-(age()*2u-120u);s.background.reals[7]=m.store(m.add(m.q(s.background.reals[7]),m.c(0x6d27c8)));s.background.reals[8]=m.store(m.add(m.q(s.background.reals[8]),m.c(0x6d27c0)));s.background.reals[9]=m.store(m.add(m.q(s.background.reals[9]),m.c(0x6d27c0)));
 if(age()==187){b.word(24,0);b.word(28,0);b.word(32,0);b.word(12,0xffffffffu);b.word(120,3);}else{b.word(24,shade);b.word(28,shade);b.word(32,shade);}
}
}
