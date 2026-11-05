#include "SessionActivation.hpp"
namespace th15 {
bool SessionActivation::check(bool ok,const char* message){if(!ok&&error.empty())error=message;return ok;}
bool SessionActivation::reset_game(){
 if(!check(services.reset_bullets(),"Scene bullet reset failed")||!check(services.reset_player(),"Scene player reset failed")||!check(services.reset_items(),"Scene item reset failed")||!check(services.reset_enemies(),"Scene enemy reset failed")||!check(services.reset_lasers(),"Scene laser reset failed"))return false;
 progress.stage_frame=progress.run_clock=0;
 return check(services.prepare_replay_stage(),"Replay stage preparation failed")&&check(services.start_main_script(),"Stage main script failed")&&check(services.initialize_hud(),"Stage HUD initialization failed")&&check(services.enable_game_callbacks(),"Game callback activation failed")&&check(services.configure_player_options(),"Stage player option configuration failed");
}
bool SessionActivation::load(const SceneActivationFrame& frame,bool& waiting){
 waiting=false;if(!error.empty())return false;if(loaded)return true;
 if(!check(services.clear_stage_intro(),"Stage intro retirement failed"))return false;
 if(progress.scene_flags&8){if(!check(services.discard_stage_assets(),"Discarded stage assets failed")||!check(services.return_to_title(frame.return_to_selection),"Aborted scene destination failed"))return false;waiting=true;return true;}
 if(!check(services.initialize_background(),"Scene background initialization failed"))return false;
 if(frame.previous_background){if(!check(services.capture_previous_background(),"Previous background capture failed")||!check(services.prepare_background_transition(),"Previous background transition failed"))return false;progress.scene_flags|=0x800;loaded=true;return check(services.transition_banner(),"Scene transition banner failed");}
 progress.scene_flags&=~0x800u;if(!reset_game())return false;loaded=true;
 if((player.mode_flags&0x30)!=0x20&&!(player.mode_flags&0x40)&&(player.mode_flags&0x300)!=0x200&&!check(services.start_entry_music(),"Scene entrance music failed"))return false;
 if(!check(services.interrupt_entrance(),"Entrance overlay interrupt failed")||!check(services.retire_restart_overlay(),"Entrance restart overlay retirement failed")||!check(services.interrupt_resume_overlay(),"Resume overlay interrupt failed"))return false;
 if(pending_progress){if(!check(services.restore_pending_progress(),"Pending progress restore failed"))return false;pending_progress=false;}
 return true;
}
bool SessionActivation::activate(const SceneActivationFrame& frame){
 if(!error.empty())return false;if(!(progress.scene_flags&0x800))return true;progress.scene_flags&=~0x800u;if(!reset_game())return false;
 if(!check(services.queue_music(frame.restart_audio?4:3),"Scene music preparation failed"))return false;
 if(frame.restart_audio&&!check(services.queue_music(4),"Scene music reset failed"))return false;
 if(!check(services.queue_music(2),"Scene music start failed")||!check(services.unlock_current_music(),"Scene music record failed")||!check(services.retire_restart_overlay(),"Transition restart overlay retirement failed"))return false;
 age.set(0);return true;
}
}
