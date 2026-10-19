#include "PauseRestoration.hpp"
namespace th15 {
bool PauseRestoration::check(bool ok,const char* reason){if(!ok&&error.empty())error=reason;return ok;}
bool PauseRestoration::restore(){
 if(!error.empty())return false;
 if(state.screen!=PauseScreen::Pause&&state.screen!=PauseScreen::GameOver&&state.screen!=PauseScreen::Results)return check(false,"Inactive pause presentation cannot restore");
 if(!progress.transition&&!check(host.restart_elapsed_play_time(),"Pause play-time restart failed"))return false;
 if(state.screen==PauseScreen::Pause){
  progress.scene_flags&=~0x10u;progress.rate=state.saved_rate;
  if(!check(host.resume_dialogue(),"Pause dialogue resume failed")||!check(host.resume_chapter_result(),"Pause chapter notice resume failed"))return false;
  frame_skip=state.saved_frame_skip;return true;
 }
 if(!check(host.interrupt_pause_visual(state.snapshot_animation,1),"Pause snapshot close failed")||!check(host.interrupt_pause_visual(state.menu_animation,1),"Pause menu close failed"))return false;
 progress.rate=state.saved_rate;return true;
}
}
