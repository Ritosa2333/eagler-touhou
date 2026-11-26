#pragma once
#include "core/stages/OmniverseBehaviorEngine.hpp"
namespace kaikeidou::omniverse_behavior_detail {
inline void Engine::simple_projectile(){
 auto b=object();
 if(behavior==196){auto value=sine(b.quad(160));put(64,m.add(m.mul(value,m.c(0x6d1a60)),m.c(0x6d1a60)));put(72,m.add(m.mul(m.i(b.word(120)),m.q(s.boss.extra_reals[1])),q(72)));put(160,m.add(m.mul(m.i(b.word(120)),m.q(s.boss.extra_reals[0])),q(160)));if(gt(q(72),m.c(0x6d18c8))||lt(q(72),m.c(0x6d1840)))b.word(0,0);else if(compare(q(168),m.q(0))!=0&&ge(m.i(age()),q(168)))b.word(0,0);return;}
 if(behavior==198){move();if(lt(q(72),m.q(0))||gt(q(72),m.c(0x6d19a0))){put(88,m.mul(q(88),m.c(0x6aca58)));put(72,m.add(q(72),q(88)));put(96,atan2(b.quad(88),b.quad(80)));}if(lt(q(64),m.c(0x6d1a58))||gt(q(64),m.c(0x6d1a50)))b.word(0,0);return;}
 if(behavior==202){if(age()==b.word(120))omniverse_behavior_remove(b,100);return;}
 if(behavior>=203&&behavior<=206){const bool horizontal=behavior==203||behavior==205;const unsigned position=horizontal?64u:72u;put(position,m.add(q(position),q(horizontal?80u:88u)));if(behavior==203||behavior==206){if(lt(q(position),m.c(0x6d1a30)))b.word(0,0);}else if(gt(q(position),m.c(behavior==204?0x6d1a48:0x6d1a28)))b.word(0,0);b.word(60,p().integers[0]!=behavior-203u);return;}
 if(behavior==208){if(b.word(72)==0)ease(32,b.quad(40),constant(0x6d1a40));else if(b.word(72)==1){put(24,m.sub(q(24),m.c(0x6d1960)));ease(32,b.quad(40),constant(0x6d1a40));}else if(b.word(72)==0xffffffffu){ease(32,0,constant(0x6d1a40));if(ge(m.q(0x3ff0000000000000ull),q(32)))b.word(0,0);}return;}
 if(behavior==210){b.quad(216,b.quad(64));b.quad(224,b.quad(72));b.quad(232,b.quad(96));b.quad(64,bullet(b.word(120)).quad(216));b.quad(72,bullet(b.word(120)).quad(224));b.quad(96,bullet(b.word(120)).quad(232));return;}
 if(behavior==211){move(80,88,true);ease(104,b.quad(168),b.quad(160));clip(0x6d1840,0x6d1a38,0x6d1840,0x6d18c8);return;}
 if(behavior==215){move(160,168);put(168,m.add(q(168),q(176)));put(176,m.mul(q(176),q(184)));if(gt(q(72),m.q(p().reals[0]))&&gt(q(192),q(168))){put(160,m.mul(m.sub(m.c(0x6d1940),m.q(p().reals[1])),q(160)));put(168,m.mul(q(168),m.c(0x6d1a20)));}if(lt(q(64),m.c(0x6d1a30))||gt(q(64),m.c(0x6d1a28))||lt(q(72),m.c(0x6d1a30)))b.word(0,0);return;}
 if(behavior==217){if(signed_age()<0)return;move(80,88,true);if(signed_age()<=59)put(104,m.mul(q(104),m.c(0x6d1a20)));else if(signed_age()<=109)put(104,m.add(m.add(m.mul(m.c(0x6d1a08),q(160)),m.c(0x6d1a18)),q(104)));clip(0x6d19f0,0x6d19e8,0x6d19f0,0x6d19f8);return;}
 if(behavior==218){const auto dx=m.store(m.sub(m.q(s.player.quad(144)),q(64))),dy=m.store(m.sub(m.q(s.player.quad(152)),q(72)));const auto angle=m.store(atan2(dy,dx));auto value=cosine(angle);put(64,m.add(m.mul(value,m.c(0x6d19c0)),q(64)));value=sine(angle);put(72,m.add(m.mul(value,m.c(0x6d19c0)),q(72)));return;}
 if(behavior==220){move(80,88,true);put(104,m.add(q(104),m.c(0x6d1968)));clip(0x6d19f0,0x6d19e8,0x6d19f0,0x6d19f8);return;}
 if(behavior==221){auto value=cosine(b.quad(160));put(64,m.add(m.mul(value,q(104)),q(64)));value=sine(b.quad(160));put(72,m.add(m.mul(value,q(104)),q(72)));if(signed_age()<=19)put(160,m.add(m.mul(m.i(b.word(120)),q(168)),q(160)));else if(signed_age()<=49){put(104,m.mul(q(104),m.c(0x6d1a10)));put(160,m.add(m.mul(m.i(b.word(120)),m.c(0x6d1918)),q(160)));}else if(signed_age()<=69){put(104,m.add(q(104),q(176)));put(160,m.add(m.mul(m.i(b.word(120)),m.c(0x6d1a08)),q(160)));}else put(104,m.add(m.mul(q(176),m.c(0x6d1a00)),q(104)));clip(0x6d19f0,0x6d19e8,0x6d19f0,0x6d19f8);return;}
 if(behavior==222||behavior==223){auto value=cosine(b.quad(176));put(64,m.add(m.mul(value,q(184)),q(160)));value=sine(b.quad(176));auto dy=m.mul(value,q(184));if(behavior==223)dy=m.mul(dy,m.c(0x6d18f0));put(72,m.add(dy,q(168)));if(behavior==222){put(176,m.add(q(176),q(192)));put(184,m.sub(q(184),q(200)));if(ge(m.c(0x6d1948),q(184)))b.word(0,0);}else{put(176,m.add(m.mul(m.i(b.word(120)),m.c(0x6d19e0)),q(176)));put(184,m.add(q(184),m.c(0x6d19d8)));if(le(m.c(0x6d19d0),q(184)))b.word(0,0);}return;}
 if(behavior==226){move();put(96,m.add(q(96),q(160)));clip(0x6d19a8,0x6d19a0,0x6d19a8,0x6d1998);return;}
 throw std::out_of_range("Omniverse simple projectile behavior");
}
}
