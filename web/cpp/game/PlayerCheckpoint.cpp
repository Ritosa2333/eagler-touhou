#include "PlayerCheckpoint.hpp"
namespace th15 {
bool PlayerCheckpoint::retire(u32 handle){auto* vm=registry.find(handle);if(!vm)return true;if(registry.destroy_tree(*vm))return true;error=registry.error;return false;}
u32 PlayerCheckpoint::save_animation(u32 handle){const u32 result=handle?animations.capture(handle,registry):0;if(!animations.error.empty())error=animations.error;return result;}
u32 PlayerCheckpoint::restore_animation(u32 handle){const u32 result=handle?animations.restore(handle,registry):0;if(!animations.error.empty())error=animations.error;return result;}
bool PlayerCheckpoint::capture(){
    available=false;error.clear();auto& m=player.motion;auto& l=player.life;auto& f=player.frame;auto& v=player.visuals;
    saved.position=m.position;saved.fixed=m.position_fixed;saved.step=m.step;saved.velocity=m.velocity;saved.last_direction=m.last_direction;
    saved.normal_speed=m.normal_speed;saved.focus_speed=m.focus_speed;saved.normal_diagonal=m.normal_diagonal_speed;saved.focus_diagonal=m.focus_diagonal_speed;
    saved.focus=m.focus;saved.option_count=m.option_count;saved.follow=m.option_follow_percentage;saved.collapse=m.collapse_frame;saved.behavior_flags=m.behavior_flags;saved.movement_scale=m.movement_scale;saved.external_velocity=m.external_velocity;
    saved.state=l.state;saved.life_age=l.age;saved.invulnerability=l.invulnerability;saved.input_age=f.input_age;saved.ready_age=f.ready_age;saved.shot_age=f.shooting.shot;saved.continuous_age=f.shooting.continuous;saved.barrier_age=m.barrier_timer;saved.power_level=player.options.power_level;
    for(u32 i=0;i<8;i++){const auto& o=m.options[i];saved.options[i]={o.active,o.index,o.snap,o.target,o.position,o.normal_offset,o.focus_offset};}
    saved.option_angles=player.shot_context.option_angles;for(u32 i=0;i<5;i++)saved.laser_power[i]=player.shot_context.laser_power[i];
    saved.damage=player.damage.sources;saved.damage_cursor=player.damage.cursor;saved.shots=player.shots.shots;saved.option_handles=v.option_handles;
    for(auto& shot:saved.shots)if(shot.state){shot.animation=save_animation(shot.animation);if(!shot.animation)shot.state=0;}
    for(u32 i=0;i<8;i++)if(saved.options[i].active)for(auto& h:saved.option_handles[i])h=save_animation(h);
    saved.focus_handle=save_animation(v.focus_handle);saved.barrier_handle=save_animation(v.barrier_handle);available=error.empty();return available;
}
bool PlayerCheckpoint::restore(){
    error.clear();if(!available){error="No saved player chapter";return false;}
    for(const auto& shot:player.shots.shots)if(!retire(shot.animation))return false;
    for(const auto& pair:player.visuals.option_handles)for(auto h:pair)if(!retire(h))return false;
    if(!retire(player.visuals.focus_handle)||!retire(player.visuals.barrier_handle))return false;
    auto& m=player.motion;auto& l=player.life;auto& f=player.frame;auto& v=player.visuals;
    m.position=saved.position;m.position_fixed=saved.fixed;m.step=saved.step;m.velocity=saved.velocity;m.last_direction=saved.last_direction;
    m.normal_speed=saved.normal_speed;m.focus_speed=saved.focus_speed;m.normal_diagonal_speed=saved.normal_diagonal;m.focus_diagonal_speed=saved.focus_diagonal;
    m.focus=saved.focus;m.option_count=saved.option_count;m.option_follow_percentage=saved.follow;m.collapse_frame=saved.collapse;m.behavior_flags=saved.behavior_flags;m.movement_scale=saved.movement_scale;m.external_velocity=saved.external_velocity;
    l.state=saved.state;l.age=saved.life_age;l.invulnerability=saved.invulnerability;f.input_age=saved.input_age;f.ready_age=saved.ready_age;f.shooting.shot=saved.shot_age;f.shooting.continuous=saved.continuous_age;m.barrier_timer=saved.barrier_age;player.options.power_level=saved.power_level;
    m.input_frame=f.input_age.current;m.touch={};
    for(u32 i=0;i<8;i++){auto& o=m.options[i];const auto& s=saved.options[i];o.active=s.active;o.index=s.index;o.snap=s.snap;o.target=s.target;o.position=s.position;o.normal_offset=s.normal_offset;o.focus_offset=s.focus_offset;}
    player.shot_context.option_angles=saved.option_angles;for(u32 i=0;i<5;i++)player.shot_context.laser_power[i]=saved.laser_power[i];
    player.damage.sources=saved.damage;player.damage.cursor=saved.damage_cursor;player.shots.shots=saved.shots;v.option_handles=saved.option_handles;
    for(auto& shot:player.shots.shots)if(shot.state)shot.animation=restore_animation(shot.animation);
    if(player.session.power_step<=0){error="Invalid checkpoint player power step";return false;}
    if(player.options.power_level!=player.session.power/player.session.power_step){if(!player.configure_options()){error=player.options.error;return false;}}
    else for(u32 i=0;i<8;i++)if(m.options[i].active)for(auto& h:v.option_handles[i])h=restore_animation(h);
    v.focus_handle=restore_animation(saved.focus_handle);v.barrier_handle=restore_animation(saved.barrier_handle);return error.empty();
}
}
