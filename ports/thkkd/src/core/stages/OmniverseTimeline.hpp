// 原67F710..68ABE9完整八项选择及共同结束流程。
#pragma once
#include "core/stages/OmniverseTimelinePart012.hpp"
#include "core/stages/OmniverseTimelinePart3.hpp"
#include "core/stages/OmniverseTimelinePart4.hpp"
#include "core/stages/OmniverseTimelinePart5.hpp"
#include "core/stages/OmniverseTimelinePart5Marisa.hpp"
#include "core/stages/OmniverseTimelinePart5Sanae.hpp"
#include "core/stages/OmniverseTimelinePart6.hpp"
#include "core/stages/OmniverseTimelinePart6Reimu.hpp"
#include "core/stages/OmniverseTimelinePart6Marisa.hpp"
#include "core/stages/OmniverseTimelinePart6Sanae.hpp"
#include "core/stages/OmniverseTimelinePart7.hpp"
#include "core/stages/OmniverseTimelinePart7Sanae.hpp"
#include "core/stages/OmniverseTimelinePart7Opacity.hpp"
#include "core/stages/OmniverseTimelineClosing.hpp"
namespace kaikeidou {
inline void update_omniverse_timeline(OmniverseTimelineState& state,Random& random,RandomPrecision precision,GameMath& math,OmniverseTimelineServices& services,PlayerShotFireInput& input,const std::array<std::uint32_t,9>& bindings,const PlayerShotFireContext& fire){
 constexpr std::array<unsigned,6> order{{0,1,2,3,6,4}},counts{{7,1,3,10,5,6}};
 for(unsigned i=0;i<6;++i)if(omniverse_timeline_detail::signed_word(omniverse_timeline_input(bindings[order[i]],fire,input))>0)for(unsigned j=0;j<counts[i];++j)random.real_number(0x3ff0000000000000ull,precision);
 omniverse_timeline_detail::Engine e{state,random,precision,math,services,input,bindings,fire,state.phase.boss_index,{static_cast<unsigned>(precision)}};
 switch(state.timeline.part){case 0:e.part0();break;case 1:e.part1();break;case 2:e.part2();break;case 3:e.part3();break;case 4:e.part4();break;case 5:e.part5();break;case 6:e.part6();break;case 7:e.part7();break;default:break;}e.closing();
}
}
