#include "SessionRuntime.hpp"
namespace th15 {
bool SessionRuntime::check(bool ok,const char* reason){if(!ok&&error.empty())error=reason;return ok;}
i32 SessionRuntime::update(const SessionFrameState& frame){
 if(!error.empty())return i32(FrameAction::Error);
 if(progress.scene_flags&0x4000){
  if(progress.scene_flags&0x10)return i32(FrameAction::StopSuccess);
  ending_frames=wrapping_add(ending_frames,1);
  if(ending_frames==180&&!check(host.ending_fade(),"Ending fade unavailable"))return i32(FrameAction::Error);
  if(ending_frames>=380){if(progress.transition){if(!check(host.finish_replay(),"Replay completion failed"))return i32(FrameAction::Error);}else if(!check(host.destination(frame.return_to_selection?SessionDestination::ReturnSelection:progress.difficulty==4?SessionDestination::ExtraEnding:SessionDestination::NormalEnding),"Ending destination unavailable"))return i32(FrameAction::Error);}
 }
 if((frame.gui_flags&0x100)&&frame.gui_age<120&&!(progress.scene_flags&0x10))return i32(FrameAction::Continue);
 if(age.current==0){bool waiting=false;if(!check(host.load_scene(waiting),"Scene preparation failed"))return i32(FrameAction::Error);if(waiting)return i32(FrameAction::Continue);}
 else if(age.current==30&&!check(host.activate_scene(),"Scene activation failed"))return i32(FrameAction::Error);
 if(frame.background_finished&&!check(host.release_background(),"Previous background release failed"))return i32(FrameAction::Error);
 if(age.current==1&&(player.mode_flags&0x300)==0x200){
  bool restored=false;if(!check(host.load_checkpoint_file(restored),"Checkpoint file load failed"))return i32(FrameAction::Error);
  if(restored){progress.startup_frames=180;if(!check(host.restart_overlay(249),"Loaded checkpoint overlay failed"))return i32(FrameAction::Error);if(progress.chapter>=43&&!check(host.start_boss_music(),"Loaded chapter boss music failed"))return i32(FrameAction::Error);}
  player.mode_flags=(player.mode_flags&~0x200u)|0x100;
 }
 if(age.current==5)sampling_mode=2;
 if(progress.scene_flags&4){progress.scene_flags|=0x80;return i32(FrameAction::Continue);}
 if(player.mode_flags&0x40){
  if((frame.pressed&0x80103)||(progress.scene_flags&0x70))if(!check(host.destination(frame.return_to_selection?SessionDestination::ReturnSelection:SessionDestination::DemoFinished),"Demo exit unavailable"))return i32(FrameAction::Error);
  if(age.current==3840){if(!check(host.demo_fade(),"Demo fade failed"))return i32(FrameAction::Error);}
  else if(age.current==3900&&!check(host.destination(frame.return_to_selection?SessionDestination::ReturnSelection:SessionDestination::DemoFinished),"Demo timeout unavailable"))return i32(FrameAction::Error);
 }
 if(!check(host.update_score(),"HUD score update failed"))return i32(FrameAction::Error);
 if(progress.scene_flags&0x70)return i32(FrameAction::StopSuccess);
 if(progress.scene_flags&0x10000){
  if(progress.restart_frames==0){if(progress.chapter<43&&!check(host.prepare_stage_music(),"Retry stage music preparation failed"))return i32(FrameAction::Error);if(!check(host.restart_effect(1),"Retry effect interrupt failed"))return i32(FrameAction::Error);}
  progress.restart_frames=wrapping_add(progress.restart_frames,1);
  if(progress.restart_frames<progress.startup_frames){if(progress.restart_frames>1)return i32(FrameAction::StopSuccess);}
  else{
   if(progress.chapter<43){if(!check(host.start_stage_music(),"Retry music restart failed"))return i32(FrameAction::Error);if(progress.chapter<43&&!check(host.seek_stage_music(double(progress.stage_frame)/60.),"Retry music seek failed"))return i32(FrameAction::Error);}
   player.mode_flags&=~0x400u;progress.scene_flags&=~0x10000u;progress.restart_frames=0;
  }
 }
 if(progress.scene_flags&0x8000){
  if(!(player.mode_flags&0x300)||frame.player_life_state==0||frame.player_life_state==1){
   if(!host.skip_chapter_reward()&&requested_chapter!=0&&!check(host.chapter_reward(requested_chapter>=43||(requested_chapter>=22&&requested_chapter<=40)),"Chapter reward failed"))return i32(FrameAction::Error);
   if(requested_chapter>=0&&!check(host.chapter_checkpoint(requested_chapter),"Chapter snapshot failed"))return i32(FrameAction::Error);
  }
  progress.scene_flags&=~0x8000u;
 }
 if(!check(host.update_overlays(),"Scene overlay update failed"))return i32(FrameAction::Error);
 progress.stage_frame=wrapping_add(progress.stage_frame,1);progress.run_clock=wrapping_add(progress.run_clock,1);if(frame_skip)frame_skip=wrapping_add(frame_skip,-1);age.tick(&progress.rate);
 return i32(FrameAction::Continue);
}
}
