#include "PauseActivation.hpp"
namespace th15 {
bool PauseActivation::check(bool ok,const char* reason){if(!ok&&error.empty())error=reason;return ok;}
bool PauseActivation::save_presentation(i32 skip){state.saved_rate=progress.rate;progress.rate=1;state.saved_frame_skip=frame_skip;frame_skip=skip;return true;}
bool PauseActivation::freeze_overlays(){return check(host.freeze_dialogue(),"Pause dialogue freeze failed")&&check(host.freeze_chapter_result(),"Pause chapter notice freeze failed");}
bool PauseActivation::open(PauseEntrance entrance,i32 front,bool selection){
 if(!error.empty())return false;const u32 mode=player.mode_flags;const bool spell=(mode&0x30)==0x20;
 if(entrance!=PauseEntrance::RetryPause&&!check(host.record_elapsed_play_time(),"Pause play-time accounting failed"))return false;
 if((entrance==PauseEntrance::GameOver||entrance==PauseEntrance::Results)&&progress.transition==1)return check(host.replay_destination(selection),"Replay pause destination failed");
 if(entrance==PauseEntrance::Pause){
  state.select(PauseScreen::Pause);progress.scene_flags|=0x10;state.front_bank=front;
  const i32 script=progress.transition?151:(mode&0x300)?150:149;
  if(!check(host.menu_visual(state.menu_animation,script,3),"Pause menu visual failed")||!check(host.reset_audio_slots(),"Pause sound reset failed")||!check(host.sound(14),"Pause sound failed"))return false;
  if(!spell&&!check(host.suspend_music(),"Pause music suspend failed"))return false;
  if(!check(host.finish_audio_requests(),"Pause audio queue drain failed")||!check(host.capture_background(state.snapshot_animation,false),"Pause screenshot failed"))return false;
  save_presentation(0);if(!freeze_overlays())return false;state.flags&=~4u;return true;
 }
 if(entrance==PauseEntrance::RetryPause){
  state.select(PauseScreen::Pause);state.select_phase(1);progress.scene_flags|=0x10;
  if(!check(host.capture_background(state.snapshot_animation,false),"Retry pause screenshot failed"))return false;
  state.front_bank=front;
  if(!check(host.menu_visual(state.menu_animation,152,3),"Retry pause menu visual failed")||!check(host.reset_audio_slots(),"Retry pause sound reset failed")||!check(host.suspend_music(),"Retry pause music suspend failed"))return false;
  save_presentation(0);state.flags&=~4u;return true;
 }
 if(entrance==PauseEntrance::GameOver){
  state.select(PauseScreen::GameOver);state.select_phase(2);progress.scene_flags|=0x10;
  if(!check(host.reset_audio_slots(),"Game-over sound reset failed")||!check(host.sound(14),"Game-over sound failed"))return false;
  if(!spell&&(!(mode&0x300)||progress.chapter<43)&&!check(host.suspend_music(),"Game-over music suspend failed"))return false;
  if(!check(host.finish_audio_requests(),"Game-over audio queue drain failed")||!check(host.capture_background(state.snapshot_animation,false),"Game-over screenshot failed"))return false;
  state.front_bank=front;
  if(!spell&&!(mode&0x300)){
   if(!check(host.preserve_current_music(state.saved_music,state.saved_music_position),"Game-over music position unavailable")||!check(host.start_game_over_music(),"Game-over music preparation failed"))return false;
  }
  state.result_mode=0;save_presentation(1);state.flags&=~4u;return true;
 }
 if(entrance==PauseEntrance::Results){
  progress.scene_flags|=0x10;state.select(PauseScreen::Results);state.select_phase(spell?5:3);
  if(!(mode&0x30)&&!check(host.capture_background(state.snapshot_animation,true),"Result playfield capture failed"))return false;
  state.front_bank=front;state.result_mode=1;save_presentation(1);if(!freeze_overlays())return false;state.flags|=4u;return true;
 }
 return check(false,"Unknown pause entrance");
}
}
