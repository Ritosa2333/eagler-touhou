#pragma once
#include "PlayerMotion.hpp"
#include "PlayerLife.hpp"
#include "DamageRegions.hpp"
namespace th08 {
struct PlayerFrameCounters {i32 total=0,stage=0,youkai=0,human=0,stage_youkai=0,stage_human=0;};
struct PlayerFrameState {Vec3 homing_target,shot_target;const void* target_reference=nullptr;PlayerFrameCounters counters;u32 boss_target=0;};
struct PlayerFrameActions {
    virtual ~PlayerFrameActions()=default;
    virtual void update_bomb()=0;
    virtual bool resolve_death()=0;
    virtual void respawn()=0;
    virtual void update_invincibility()=0;
    virtual void update_motion()=0;
    virtual void step_animation(AnmVm&)=0;
    virtual void update_shots()=0;
    virtual void update_shooting()=0;
    virtual bool gui_blocked()=0;
    virtual i32 hud_state()=0;
    virtual void hud_interrupt(i32 value)=0;
    virtual void add_score(i32 value)=0;
};
// Original 0044c390 and 0044d420. Phases use the owning player's existing
// motion/life/bomb/shot objects; these methods neither load nor emulate code.
void reset_player_targets(PlayerFrameState&,const Vec3& position,PlayerFrameActions&);
void update_player_frame(PlayerFrameState&,PlayerMotionState&,PlayerLifeState&,DamageRegions&,GameGauge&,bool paused,PlayerFrameActions&);
}
