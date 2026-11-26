#pragma once
#include "core/stages/OmniverseTimelineEngine.hpp"
namespace kaikeidou::omniverse_timeline_detail {
inline void Engine::part6(){
 if(t().subphase==0){if(frame()==1){if(s.player.word(36)==0)services.music(12,1);boss=static_cast<std::uint32_t>(services.allocate_boss(6));enemy().word(8,14);enemy().quad(56,constant(0x6d1a60));const auto y=omniverse_timeline_character(s.lifecycle.character,{{0u-30u,0u-30u,478}});put(64,m.i(y));enemy().quad(168,constant(0x6d1a60));const auto target=omniverse_timeline_character(s.lifecycle.character,{{80,80,368}});put(176,m.i(target));}else if(frame()==10)services.begin_boss_effect();else if(frame()==60){t().subphase=1;reset(0xffffffffu);}return;}
 if(t().subphase!=1)return;if(character()==0)part6_reimu();else if(character()==1)part6_marisa();else part6_sanae();
}
}
