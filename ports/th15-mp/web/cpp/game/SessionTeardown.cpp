#include "SessionTeardown.hpp"
namespace th15 {
bool SessionTeardown::check(bool ok,const char* reason){if(!ok&&error.empty())error=reason;return ok;}
bool SessionTeardown::release(SessionObject kind){return !host.owns(kind)||check(host.release(kind),"Session object release failed");}
bool SessionTeardown::finish(i32 destination,bool restart_audio){
 if(!error.empty())return false;
 if(!check(host.stop_checkpoint_worker(),"Checkpoint worker stop failed")||!check(host.save_records(),"Session record save failed"))return false;
 player.mode_flags&=~3u;progress.rate=1;
 if(!check(host.clear_session_links(),"Session link cleanup failed"))return false;
 if(destination==10||destination==11||destination==12||destination==4||destination==16||destination==14){
  if(!check(host.reset_transition(),"Scene transition reset failed"))return false;
  if(destination==12)player.mode_flags|=2;
  else if(destination==10||destination==11){if(progress.starting_stage==progress.stage)player.mode_flags|=1;}
  else if(destination==14){if(progress.starting_stage!=progress.stage){progress.continues=wrapping_add(progress.continues,1);if(progress.continues>=10)progress.continues=9;player.mode_flags|=8;}player.mode_flags|=1;}
 }
 if(!(player.mode_flags&2)){
  if(destination!=15&&destination!=16&&!release(SessionObject::Replay))return false;
  if((player.mode_flags&0x300)&&!check(host.stop_checkpoint_worker(),"Checkpoint worker repeat stop failed"))return false;
  if(host.owns(SessionObject::Checkpoint)){
   if(!check(host.stop_checkpoint_worker(),"Checkpoint storage worker stop failed")||!check(host.clear_checkpoint_data(),"Checkpoint storage cleanup failed")||!check(host.stop_checkpoint_worker(),"Checkpoint storage final stop failed")||!release(SessionObject::Checkpoint))return false;
  }
  for(auto kind:{SessionObject::Background,SessionObject::PreviousBackground,SessionObject::PauseMenu,SessionObject::Hud,SessionObject::Player,SessionObject::Bullets,SessionObject::Items,SessionObject::Lasers,SessionObject::Popups})if(!release(kind))return false;
 }else{
  if(!check(host.reset_gui_for_stage(),"Stage GUI retirement failed")||!release(SessionObject::PreviousBackground)||!check(host.carry_background(),"Previous background transfer failed"))return false;
  for(auto kind:{SessionObject::PauseMenu,SessionObject::Bullets,SessionObject::Replay})if(!check(host.disable_callbacks(kind),"Retained run callback disable failed"))return false;
  if(!check(host.reset_items(),"Retained item pool reset failed"))return false;
 }
 if(player.mode_flags&9){if(!check(host.clear_enemies(),"Retained enemy pool clear failed"))return false;}
 else if(!release(SessionObject::Enemies))return false;
 if(!check(host.retire_stage_animations(),"Stage animation retirement failed")||!release(SessionObject::Bomb)||!release(SessionObject::Spell)||!check(host.detach_scene_callbacks(),"Scene callback detach failed"))return false;
 const u32 flags=player.mode_flags;
 if(!(((flags&0x30)==0x20)&&(flags&1))&&!(flags&0x42)&&!check(host.queue_music(restart_audio?4:3),"Scene exit music request failed"))return false;
 if(!check(host.reset_audio_slots(),"Scene audio slot reset failed"))return false;
 presentation.frame_skip=1;presentation.clear_color=flags&1?0:0xff000000u;return true;
}
}
