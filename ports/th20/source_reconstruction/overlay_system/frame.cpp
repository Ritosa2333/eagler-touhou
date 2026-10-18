#include "frame.hpp"
#include "../gameplay/player_state.hpp"
namespace th20::source::overlay {
namespace ps=gameplay::player_state;namespace n=recovered;
int update(WeaponStoneInf& owner,FrameEnvironment& host){
 if(host.game_requests_end()&&owner.phase){host.interrupt(owner.animation_handle,1);owner.animation_handle=0;owner.phase=0;owner.main_weapon->end_phase();}
 if(!host.game_active()||!host.hud_present()||host.boss_collecting()){
  if(!host.hud_present())return 1;if(!host.boss_collecting())return 1;host.delete_animation(owner.animation_handle);owner.main_weapon->cancel_phase();return 1;
 }
 auto& p=*game_session::context(0).current_player;const int id=ps::read<int>(p,0xc);bool cancel=id!=5&&id!=7;bool ended=false;
 if(owner.phase==2){if(owner.main_weapon->end_phase()!=0)owner.phase=1;else{owner.cancellation_radius=128.f;if(cancel)n::timer_set(owner.cancellation_age,60);ended=true;}}
 if(!ended&&owner.phase==1){
  if(!host.animation_exists(owner.animation_handle))owner.animation_handle=host.spawn_aura(*owner.file,host.player_position());
  host.animation_position(owner.animation_handle,host.player_position());
  if(owner.main_weapon->update_phase()!=0){owner.main_weapon->end_phase();if(ps::read<int>(p,0xc)==6)cancel=false;if(cancel)n::timer_set(owner.cancellation_age,4);owner.cancellation_radius=24.f;ended=true;}
 }
 if(ended){host.interrupt(owner.animation_handle,1);owner.animation_handle=0;owner.phase=0;host.phase_visuals(owner,true);ps::write(p,0x4c,0);}
 if(owner.cancellation_age.current>0&&cancel){host.clear_bullets(host.player_position(),owner.cancellation_radius);host.clear_lasers(host.player_position(),owner.cancellation_radius);n::timer_add(owner.cancellation_age,-1.f,state::timer_rate);}
 owner.main_weapon->update_main();owner.focused_weapon->update_focused();owner.unfocused_weapon->update_unfocused();owner.passive_weapon->update_passive();n::timer_tick(owner.age,state::timer_rate);return 1;
}
int update(WeaponStoneInf& owner){return update(owner,frame_environment());}
void start_phase(WeaponStoneInf& owner,FrameEnvironment& host){host.sound(28,host.player_position().x);host.phase_visuals(owner,false);if(owner.animation_handle)host.interrupt(owner.animation_handle,1);owner.animation_handle=host.spawn_aura(*owner.file,host.player_position());owner.main_weapon->start_phase();owner.phase=1;}
void start_phase(WeaponStoneInf& owner){start_phase(owner,frame_environment());}
}
