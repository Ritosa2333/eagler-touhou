#pragma once
#include "core/stages/ExBossEarlyTimelinePhase6Reimu.hpp"
#include "core/stages/ExBossEarlyTimelinePhase6Marisa.hpp"
#include "core/stages/ExBossEarlyTimelinePhase6Sanae.hpp"
namespace kaikeidou { namespace ex_boss_early_timeline_detail {
inline void Engine::phase6(){
    if(player.word(0x2c)){protection.boss_state_a7ea60=1;allocation.field_a7ea58=0;}else{protection.boss_state_a7ea60=0;allocation.field_a7ea58=1;}
    if(character==0)phase6_reimu();else if(character==1)phase6_marisa();else phase6_sanae();
}
}}
