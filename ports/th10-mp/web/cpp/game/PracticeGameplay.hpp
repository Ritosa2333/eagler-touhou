#pragma once
#include "PracticeConfig.hpp"
namespace th10 {
inline void practice_reset_lock_timer(PracticeState& p){p.lock_timer=0;p.lock_timer_pending=false;}
// Native update hook sets a flag; GUI consumes it once, even if several
// enemies update the countdown or high-refresh draws the cached GUI again.
inline void practice_consume_lock_timer(PracticeState& p){
    if(!p.enabled)return;
    if(p.lock_timer_pending){++p.lock_timer;p.lock_timer_pending=false;}
}
// Purple th10_bossmovedown, 0x40FA33: run only when ECL sets the clamp.
// Keep the lower boundary; reduce the range by the selected fraction.
inline void practice_boss_clamp(PracticeState& p,float& y,float& range){
    if(!p.enabled||!p.force_boss_move_down)return;
    const float maximum=y+range*.5f;
    const float minimum=maximum-range*(1.f-p.boss_move_down_range);
    y=(maximum+minimum)*.5f;range=maximum-minimum;p.assisted=true;
}
// Native white/yellow hooks count actual point collection, not item spawns.
// Observational only: replay counters are allowed without changing the run.
inline void practice_point_collected(PracticeState& p,bool yellow){
    if(!p.enabled||!p.show_point_items)return;
    if(yellow)++p.tracker_yellow;else ++p.tracker_white;
}
}
