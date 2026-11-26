#pragma once
#include "core/stages/OmniverseTimelinePart6GeometryEvents.hpp"
namespace kaikeidou::omniverse_timeline_detail {
inline void Engine::part6_reimu(){
 if(frame()==0){name(0x6c5bd0);services.configure_phase(1,90,0,161);integer(0)=15;for(std::uint32_t i=1;signed_word(i)<=signed_word(integer(0));++i)enemy_at(i).word(128,0);return;}
 if(frame()==60){effect(12,{{255,255,255}});return;}if(frame()==110){enemy().quad(176,constant(0x6d2090));return;}if(signed_frame()<160)return;
 if(frame()==200){enemy().quad(56,constant(0x6d2090));enemy().quad(168,enemy().quad(56));}
 if(signed_frame()<2400)geometry_events(*this);else geometry_late_events();geometry_update();geometry_intersections();
}
inline void Engine::geometry_late_events(){
 if(frame()==2400){enemy().quad(56,constant(0x6d1ff0));enemy().quad(64,constant(0x6d1ff0));enemy().quad(168,constant(0x6d1a60));enemy().quad(176,constant(0x6d1a60));for(std::uint32_t i=1;signed_word(i)<=signed_word(integer(0));++i)enemy_at(i).word(128,0);
  auto b=enemy_at(1);b.word(128,1);b.word(132,0);b.quad(168,0);b.quad(176,enemy().quad(64));b.quad(184,constant(0x6d1cb0));b.quad(192,0);b.quad(200,0);
  auto b2=enemy_at(2);b2.word(128,1);b2.word(132,1);b2.quad(168,enemy().quad(56));b2.quad(176,0);b2.quad(184,constant(0x6d19a0));b2.quad(192,0);b2.quad(200,0);return;
 }
 if(frame()==2500){effect(12,{{255,255,255}});integer(1)=120;integer(2)=1;return;}
 switch(frame()){
 case 2600:case 3200:case 3800:case 4200:{const auto y=s.player.quad(152),x=s.player.quad(144);effect_at(13,{{255,255,255}},x,y);return;}
 case 2800:case 3100:case 3400:case 3700:case 4000:case 4300:effect(12,{{255,255,255}});return;
 case 2840:case 3140:case 3440:case 3740:case 4040:enemy().quad(168,s.player.quad(144));enemy().quad(176,s.player.quad(152));return;
 case 4340:enemy().quad(168,constant(0x6d1a60));enemy().quad(176,constant(0x6d1af0));return;
 default:break;
 }
 const auto circle=[&](std::uint32_t index,std::uint32_t maximum){auto b=enemy_at(index);b.word(128,1);b.word(132,2);b.quad(168,s.player.quad(144));b.quad(176,s.player.quad(152));b.quad(184,0);b.quad(192,0);b.quad(200,0);b.quad(208,constant(maximum));};
 if(frame()==2640)circle(3,0x6d1af8);else if(frame()==3240)circle(4,0x6d1ec8);else if(frame()==3840)circle(5,0x6d1ab0);else if(frame()==4240){circle(6,0x6d1cc0);circle(7,0x6b25b8);}
}
inline void Engine::geometry_update(){
 const bool early=signed_frame()<2400;
 if(!early&&signed_frame()>=2540){--integer(2);if(integer(2)==0){integer(1)-=2;integer(2)=integer(1);const auto value=rnd(0x6d1c60);const auto angle=m.store(m.mul(value,m.c(0x6d1950)));auto y=enemy().quad(64),x=enemy().quad(56);emit({{2,0,{{255,255,255}},2,255,1,41},x,y,10,constant(0x6d1dd8),constant(0x6d25f0),angle,0,0});const auto next=m.store(m.add(m.q(angle),m.c(0x6d29b8)));y=enemy().quad(64);x=enemy().quad(56);emit({{2,0,{{255,255,255}},2,255,1,41},x,y,10,constant(0x6d1dd8),constant(0x6d1c80),next,0,0});}}
 for(std::uint32_t i=1;signed_word(i)<=signed_word(integer(0));++i){auto b=enemy_at(i);if(b.word(128)!=1)continue;
  if(early){b.quad(168,m.store(m.add(m.q(b.quad(168)),m.q(b.quad(192)))));b.quad(176,m.store(m.add(m.q(b.quad(176)),m.q(b.quad(200)))));}
  else if(i==1)b.quad(176,enemy().quad(64));else if(i==2)b.quad(168,enemy().quad(56));else{b.quad(168,s.player.quad(144));b.quad(176,s.player.quad(152));if(gt(m.q(b.quad(208)),m.q(b.quad(184))))b.quad(184,m.store(m.add(m.q(b.quad(184)),m.c(0x6d1868))));}
  if(b.word(132)==0){const auto color=services.color(255,255,255);const auto y2=omniverse_timeline_convert(m.q(b.quad(176)))+16u;const auto x2=omniverse_timeline_convert(m.add(m.q(b.quad(168)),m.q(b.quad(184))))+32u;const auto y1=omniverse_timeline_convert(m.q(b.quad(176)))+16u;const auto x1=omniverse_timeline_convert(m.q(b.quad(168)))+32u;services.draw_line(x1,y1,x2,y2,color,1);}
  else if(b.word(132)==1){const auto color=services.color(255,255,255);const auto y2=omniverse_timeline_convert(m.add(m.q(b.quad(176)),m.q(b.quad(184))))+16u;const auto x2=omniverse_timeline_convert(m.q(b.quad(168)))+32u;const auto y1=omniverse_timeline_convert(m.q(b.quad(176)))+16u;const auto x1=omniverse_timeline_convert(m.q(b.quad(168)))+32u;services.draw_line(x1,y1,x2,y2,color,1);}
  else if(b.word(132)==2){const auto color=services.color(255,255,255);const auto radius=omniverse_timeline_convert(m.q(b.quad(184)));const auto y=omniverse_timeline_convert(m.q(b.quad(176)))+16u;const auto x=omniverse_timeline_convert(m.q(b.quad(168)))+32u;services.draw_circle(x,y,radius,color,0);}
 }
}
inline void Engine::geometry_intersections(){
 for(std::uint32_t i=1;signed_word(i)<=signed_word(integer(0)-1u);++i){auto a=enemy_at(i);if(a.word(128)!=1)continue;
  for(std::uint32_t j=i+1;signed_word(j)<=signed_word(integer(0));++j){auto b=enemy_at(j);if(b.word(128)!=1)continue;std::uint64_t xs[2]{constant(0x6d2c70),constant(0x6d2c70)},ys[2]{};
   const auto circle_line=[&](View circle,View line,bool horizontal){const auto delta=m.store(m.sub(m.q(line.quad(horizontal?176u:168u)),m.q(circle.quad(horizontal?176u:168u))));const auto absolute=omniverse_timeline_absolute(delta);if(!le(m.q(absolute),m.q(circle.quad(184))))return;const auto again=omniverse_timeline_absolute(delta);const auto angle=services.acos_bits(m.store(m.div(m.q(again),m.q(circle.quad(184)))));const auto zero=m.q(0);const auto base=horizontal?(le(zero,m.q(delta))?constant(0x6d1b60):constant(0x6d1de0)):(le(zero,m.q(delta))?0:constant(0x6d1860));
    for(unsigned k=0;k<2;++k){auto theta=m.store(k==0?m.add(m.q(base),m.q(angle)):m.sub(m.q(base),m.q(angle)));auto value=cosine(theta);xs[k]=m.store(m.add(m.mul(value,m.q(circle.quad(184))),m.q(circle.quad(168))));theta=m.store(k==0?m.add(m.q(base),m.q(angle)):m.sub(m.q(base),m.q(angle)));value=sine(theta);ys[k]=m.store(m.add(m.mul(value,m.q(circle.quad(184))),m.q(circle.quad(176))));}
   };
   if(a.word(132)==0){if(b.word(132)==1){xs[0]=b.quad(168);ys[0]=a.quad(176);}else if(b.word(132)==2)circle_line(b,a,true);}
   else if(a.word(132)==1){if(b.word(132)==0){xs[0]=a.quad(168);ys[0]=b.quad(176);}else if(b.word(132)==2)circle_line(b,a,false);}
   else if(a.word(132)==2){if(b.word(132)==0)circle_line(a,b,true);else if(b.word(132)==1)circle_line(a,b,false);}
   for(unsigned k=0;k<2;++k)if(ge(m.q(xs[k]),m.c(0x6a6100))&&ge(m.c(0x6d1cb0),m.q(xs[k]))&&ge(m.q(ys[k]),m.c(0x6a6100))&&ge(m.c(0x6d19a0),m.q(ys[k])))emit({{4,0,{{255,255,255}},0,255,1,41},xs[k],ys[k],1,0,constant(0x6d1ad0),0,0,0});
  }
 }
}
}
