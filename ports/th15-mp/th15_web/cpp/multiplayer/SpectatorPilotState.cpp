#include "SpectatorStageState.hpp"
#include <tuple>
namespace th15::multiplayer {
namespace {
// Capture/apply only the named motion values. In particular the receiving
// PlayerOption keeps its freshly bound movement callback and native owner.
template<class T> auto motion_fields(T& v){return std::tie(v.touch,v.flip_vertical_step,v.position_fixed,
    v.last_step,v.step,v.position,v.velocity,v.last_direction,v.normal_speed,v.focus_speed,
    v.normal_diagonal_speed,v.focus_diagonal_speed,v.direction,v.focus,v.input_frame,
    v.option_follow_percentage,v.collapse_frame,v.option_count,v.behavior_flags,v.enlargement,
    v.movement_scale,v.external_velocity,v.barrier_timer);}
template<class T> auto option_fields(T& v){return std::tie(v.active,v.target,v.position,v.normal_offset,v.focus_offset,v.snap,v.index);}
void copy_motion(const PlayerMotion& from,PlayerMotion& to){
    motion_fields(to)=motion_fields(from);
    for(unsigned n=0;n<8;++n)option_fields(to.options[n])=option_fields(from.options[n]);
}
}
void CaptureSpectatorPilot(const MultiplayerPilot& from,SpectatorPilotState& out){
    const auto& player=*from.player;out.resources=from.session;copy_motion(player.motion,out.motion);
    out.keys=from.keys;out.input=from.input;out.life_age=player.life.age;out.invulnerability=player.life.invulnerability;
    out.input_age=player.frame.input_age;out.ready_age=player.frame.ready_age;
    out.shot_age=player.frame.shooting.shot;out.continuous_age=player.frame.shooting.continuous;
    out.last_death_position=player.life.last_death_position;out.life_state=player.life.state;
    out.base_bombs=player.life.base_bombs;out.power_level=player.options.power_level;
    out.shoot_auxiliary=player.frame.shooting.auxiliary;out.shoot_auxiliary_active=player.frame.shooting.auxiliary_active;
    out.life_piece_tier=from.owned_life_piece_tier;out.ghost=from.ghost;out.spell_startup_bomb=from.spell_startup_bomb;
    out.life_latched=from.life_latched;out.ghost_clock=from.ghost_clock;out.ghost_random=from.ghost_random;
    out.life_hold=from.life_hold;out.power_taps=from.power_taps;out.power_gap=from.power_gap;
    out.ghost_dx=from.ghost_dx;out.ghost_dy=from.ghost_dy;out.option_angles=player.shot_context.option_angles;
    out.laser_power=player.shot_context.laser_power;out.reward_counters=player.shots.reward_counters;
}
bool RestoreSpectatorPilot(const SpectatorPilotState& from,MultiplayerPilot& out){
    if(!out.player)return false;auto& player=*out.player;out.session=from.resources;
    if(player.options.power_level!=from.power_level&&!player.configure_options())return false;
    copy_motion(from.motion,player.motion);out.keys=from.keys;out.input=from.input;
    player.life.age=from.life_age;player.life.invulnerability=from.invulnerability;
    player.frame.input_age=from.input_age;player.frame.ready_age=from.ready_age;
    player.frame.shooting.shot=from.shot_age;player.frame.shooting.continuous=from.continuous_age;
    player.life.last_death_position=from.last_death_position;player.life.state=from.life_state;
    player.life.base_bombs=from.base_bombs;player.options.power_level=from.power_level;
    player.frame.shooting.auxiliary=from.shoot_auxiliary;player.frame.shooting.auxiliary_active=from.shoot_auxiliary_active;
    out.owned_life_piece_tier=from.life_piece_tier;out.ghost=from.ghost;out.spell_startup_bomb=from.spell_startup_bomb;
    out.life_latched=from.life_latched;out.ghost_clock=from.ghost_clock;out.ghost_random=from.ghost_random;
    out.life_hold=from.life_hold;out.power_taps=from.power_taps;out.power_gap=from.power_gap;
    out.ghost_dx=from.ghost_dx;out.ghost_dy=from.ghost_dy;player.shot_context.option_angles=from.option_angles;
    player.shot_context.laser_power=from.laser_power;player.shots.reward_counters=from.reward_counters;return true;
}
}
