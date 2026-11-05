#include "PauseActions.hpp"
namespace th15 {
bool PauseActions::check(bool ok,const char* why){if(!ok&&error.empty())error=why;return ok;}
bool PauseActions::continue_run(){
 if(!(player.mode_flags&0x300)){
  player.mode_flags&=~0x400u;player.extra_lives=2;player.life_pieces=0;player.bombs=2;
  if(!check(gameplay.bomb_hud(2,player.bomb_pieces),"Continue early bomb HUD failed"))return false;
  player.power=0;player.bomb_pieces=0;if(score.max_power<0)player.power=score.max_power;else if(player.power_step>0)player.power=player.power_step;
  if(player.power<score.max_power){player.power=wrapping_add(player.power,wrapping_mul(player.power_step,4));if(player.power>score.max_power){player.power=score.max_power;if(!check(gameplay.power_notice(),"Continue power notice failed"))return false;}}
  if(!check(gameplay.options_changed(),"Continue player options failed")||!check(gameplay.life_hud(player.extra_lives,player.life_pieces),"Continue life HUD failed")||!check(gameplay.bomb_hud(player.bombs,player.bomb_pieces),"Continue bomb HUD failed"))return false;
  progress.continues=wrapping_add(progress.continues,1);if(progress.continues>9)progress.continues=9;score.score=0;progress.continue_budget=wrapping_sub(progress.continue_budget,1);
 }else{player.mode_flags|=0x400;if(!check(gameplay.restore_checkpoint(),"Continue chapter restoration failed"))return false;state.saved_rate=1;}
 progress.scene_flags&=~0x10u;
 if(!(player.mode_flags&0x300)){if(!check(host.resume_looping_sounds(),"Continue looping sounds failed")||!check(host.resume_saved_music(state.saved_music,state.saved_music_position),"Continue saved music restore failed"))return false;}
 else if(!check(host.reset_audio_slots(),"Continue audio reset failed"))return false;
 progress.rate=state.saved_rate;if(!check(gameplay.resume_dialogue(),"Continue dialogue resume failed")||!check(gameplay.resume_chapter_result(),"Continue chapter result resume failed"))return false;frame_skip=state.saved_frame_skip;return true;
}
bool PauseActions::execute(bool selection){
 if(!error.empty())return false;if(!check(restoration.restore(),"Pause presentation restoration failed"))return false;
 const i32 item=state.menu.cursor;
 if(item==0){
  if(state.screen==PauseScreen::Pause)return check(host.resume_looping_sounds(),"Pause looping sounds resume failed")&&check(host.resume_music(),"Pause music resume failed");
  if(state.screen==PauseScreen::GameOver){if(state.result_mode)return check(host.destination(PauseDestination::RestartRun),"Finished run restart failed");if(progress.stage==7)return check(host.destination(PauseDestination::ExtraGameOver),"Extra game-over destination failed");return continue_run();}
 }else if(item==1){
  if(!check(animations.interrupt(state.snapshot_animation,1),"Pause snapshot close failed")||!check(animations.interrupt(state.menu_animation,1),"Pause menu close failed"))return false;
  return check(host.destination(selection?PauseDestination::TitleSelection:PauseDestination::Title),"Pause title destination failed");
 }else if(item==4){
  if(!check(animations.retire(state.snapshot_animation),"Retry snapshot retirement failed")||!check(animations.retire(state.menu_animation),"Retry menu retirement failed"))return false;
  player.mode_flags&=~0x400u;
  if(progress.transition)return check(host.destination(PauseDestination::RestartReplay),"Replay restart destination failed");
  if(!(player.mode_flags&0x300))return check(host.destination(PauseDestination::RestartRun),"Legacy restart destination failed");
  player.mode_flags|=0x400;if(!check(gameplay.restore_checkpoint(),"Retry chapter restoration failed"))return false;
  if(state.screen==PauseScreen::Pause&&progress.chapter>42)return check(host.resume_music(),"Retry late-stage music resume failed");
 }
 return true;
}
}
