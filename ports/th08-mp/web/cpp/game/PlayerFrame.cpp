#include "PlayerFrame.hpp"
namespace th08 {
void reset_player_targets(PlayerFrameState& s,const Vec3& position,PlayerFrameActions& actions){
    s.homing_target=s.shot_target={-999,-999,0};s.boss_target=0;
    if(!(position.y>=400)){if(actions.hud_state()==2)actions.hud_interrupt(3);}
    else if(actions.hud_state()==2||!(position.x<160)){if(actions.hud_state()==2&&position.x>160)actions.hud_interrupt(3);}
    else actions.hud_interrupt(2);
}
void update_player_frame(PlayerFrameState& s,PlayerMotionState& motion,PlayerLifeState& life,DamageRegions& regions,GameGauge& gauge,bool paused,PlayerFrameActions& actions){
    if(motion.form.focus_effect)motion.form.focus_effect->flag19=paused;if(motion.gauge.effect)motion.gauge.effect->flag19=paused;if(paused)return;
    regions.update();actions.update_bomb();
    if(life.state==2){if(actions.resolve_death())actions.respawn();}else if(life.state==1)actions.respawn();
    actions.update_invincibility();if(life.state!=2&&life.state!=1)actions.update_motion();
    actions.step_animation(motion.animation);actions.update_shots();actions.update_shooting();reset_player_targets(s,motion.movement.position,actions);
    if(actions.gui_blocked())return;auto& c=s.counters;c.total=wrapping_add(c.total,1);c.stage=wrapping_add(c.stage,1);
    if(gauge.human_bonus()){c.human=wrapping_add(c.human,1);c.stage_human=wrapping_add(c.stage_human,1);actions.add_score(100);}
    else if(gauge.youkai_bonus()){c.youkai=wrapping_add(c.youkai,1);c.stage_youkai=wrapping_add(c.stage_youkai,1);actions.add_score(100);}
}
}
