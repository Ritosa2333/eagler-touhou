#pragma once
#include "core/stages/OmniverseTimelineEngine.hpp"
namespace kaikeidou::omniverse_timeline_detail {
inline void Engine::part7_opacity(){
 const auto distance_extended=[&](std::uint64_t x,std::uint64_t y,View b){const auto dx=m.store(m.sub(m.q(x),m.q(b.quad(64)))),dy=m.store(m.sub(m.q(y),m.q(b.quad(72))));return omniverse_timeline_sqrt(m.store(m.add(m.mul(m.q(dx),m.q(dx)),m.mul(m.q(dy),m.q(dy)))),m.precision);};
 const auto distance=[&](std::uint64_t x,std::uint64_t y,View b){return m.store(distance_extended(x,y,b));};
 const auto decrease=[](View b,std::uint32_t amount){b.word(48,b.word(48)-amount);};
 for(std::uint32_t i=1000;i<3000;++i){auto b=bullet(i);if(b.word(0)!=1)continue;const auto dist=distance(s.player.quad(144),s.player.quad(152),b);if(ge(m.c(0x6d1c58),m.q(dist))){const auto amount=192u-omniverse_timeline_convert(m.q(dist));decrease(b,amount);}}
 if(s.phase.weapon==0){
  if(read(4)!=0){const bool slow=read(6)!=0;for(unsigned opt=0;opt<3;++opt){const auto x=m.store(m.sub(m.q(s.options.options[opt].quad(8)),m.c(0x6d1e48))),y=m.store(m.sub(m.q(s.options.options[opt].quad(16)),m.c(0x6d1e20)));
   for(std::uint32_t i=1000;i<3000;++i){auto b=bullet(i);if(b.word(0)!=1)continue;const auto dist=distance(x,y,b);
    if(!slow){if(ge(m.c(0x6d1a88),m.q(dist)))decrease(b,255);else if(ge(m.c(0x6a7eb8),m.q(dist))){const auto amount=255u-omniverse_timeline_convert(m.mul(m.div(m.sub(m.q(dist),m.c(0x6d1a88)),m.c(0x6d2c18)),m.c(0x6a7eb8)));decrease(b,amount);}}
    else if(ge(m.c(0x6d1ec8),m.q(dist))){const auto amount=i<2000?250u-omniverse_timeline_convert(m.q(dist)):90u-omniverse_timeline_convert(m.mul(m.div(m.q(dist),m.c(0x6d1b90)),m.c(0x6d2658)));decrease(b,amount);}
   }
  }}
  for(unsigned shot=0;shot<80;++shot){const auto& source=s.player_shots.weapon[shot];if(source.word(0)!=1)continue;const auto x=source.quad(16),y=source.quad(24);for(std::uint32_t i=1000;i<3000;++i){auto b=bullet(i);if(b.word(0)!=1)continue;const auto dist=distance(x,y,b);if(ge(m.c(0x6b25b8),m.q(dist))){const auto amount=20u-omniverse_timeline_convert(m.div(m.q(dist),m.c(0x6d1910)));decrease(b,amount);if(le(m.sub(m.q(source.quad(24)),m.q(s.player.quad(152))),m.c(0x6d1aa8)))decrease(b,15);}}}
 }else if(s.phase.weapon==1){
  if(read(4)!=0){const auto dx=m.store(m.sub(m.q(s.player.quad(144)),q(56))),dy=m.store(m.sub(m.q(s.player.quad(152)),q(64)));(void)m.store(omniverse_timeline_sqrt(m.store(m.add(m.mul(m.q(dx),m.q(dx)),m.mul(m.q(dy),m.q(dy)))),m.precision));
   for(unsigned opt=0;opt<4;++opt){for(std::uint32_t i=2000;i<3000;++i){auto b=bullet(i);if(b.word(0)!=1)continue;const auto radial=distance_extended(s.player.quad(144),s.player.quad(152),b);const auto delta=omniverse_timeline_absolute(m.store(m.sub(m.q(s.options.options[opt].quad(64)),radial)));
    if(ge(m.c(0x6d1ba8),m.q(delta)))decrease(b,255);else if(ge(m.c(0x6b25f8),m.q(delta))){const auto amount=255u-omniverse_timeline_convert(m.mul(m.div(m.sub(m.q(delta),m.c(0x6d1ba8)),m.c(0x6d1ba8)),m.c(0x6a7eb8)));decrease(b,amount);}
   }}
  }
 }else{
  for(unsigned shot=0;shot<16;++shot){const auto& source=s.player_shots.weapon[shot];if(source.word(0)!=1)continue;const auto x=source.quad(16),y=source.quad(24);
   for(std::uint32_t i=1000;i<3000;++i){auto b=bullet(i);if(b.word(0)!=1)continue;const auto dist=distance(x,y,b);
    if(i<2000){if(ge(m.mul(m.q(source.quad(48)),m.c(0x6d1aa8)),m.q(dist))){const auto amount=omniverse_timeline_convert(m.mul(m.sub(m.mul(m.q(source.quad(48)),m.c(0x6d1aa8)),m.q(dist)),m.c(0x6d1920)));decrease(b,amount);}}
    else if(ge(m.mul(m.q(source.quad(48)),m.c(0x6d1a88)),m.q(dist)))decrease(b,255);
    else if(ge(m.mul(m.q(source.quad(48)),m.c(0x6b25b8)),m.q(dist))){const auto numerator=m.sub(m.q(dist),m.mul(m.q(source.quad(48)),m.c(0x6d1a88)));const auto amount=255u-omniverse_timeline_convert(m.mul(m.div(numerator,m.mul(m.q(source.quad(48)),m.c(0x6d1a88))),m.c(0x6a7eb8)));decrease(b,amount);}
   }
  }
 }
 for(unsigned effect_index=0;effect_index<50;++effect_index){auto f=effect_view(effect_index);if(f.word(0)!=1)continue;const auto x=m.store(m.sub(m.q(f.quad(96)),m.c(0x6d1e48))),y=m.store(m.sub(m.q(f.quad(104)),m.c(0x6d1e20)));for(std::uint32_t i=2000;i<3000;++i){auto b=bullet(i);if(b.word(0)!=1)continue;const auto dist=distance(x,y,b);if(ge(m.i(f.word(4)*6u),m.q(dist)))decrease(b,255);}}
}
}
