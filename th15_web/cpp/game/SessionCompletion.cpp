#include "SessionCompletion.hpp"
namespace th15 {
bool SessionCompletion::check(bool ok,const char* message){if(!ok&&error.empty())error=message;return ok;}
void SessionCompletion::award(i32 bonus){score.score=wrapping_add(score.score,bonus/10);if(u32(score.score)>=1000000000u)score.score=999999999;clear_bonus=wrapping_add(clear_bonus,bonus);}
bool SessionCompletion::record_finish(){return progress.transition||check(records.finished_run(progress.character+progress.subcharacter,(player.mode_flags&0x300)==0,progress.difficulty,progress.continues==0),"Run completion record failed");}
bool SessionCompletion::complete(){
 if(!error.empty())return false;
 if((player.mode_flags&0x30)!=0x20&&!check(services.clear_notice(),"Stage clear notice failed"))return false;
 if(!check(services.finish_player_options(),"Stage clear player options failed"))return false;
 if(player.bomb_state&&!check(services.end_bomb(),"Stage clear bomb completion failed"))return false;
 const u32 mode=(player.mode_flags>>4)&3;
 // Purple 43d99d jumps past the practice return only after the original
 // practice records. Replay playback still exits earlier at 43d8f0.
 const bool acb=services.all_clear_bonus();
 if(mode){
  if(progress.transition)return check(services.finish_replay(),"Practice replay completion failed");
  if(mode==2&&!check(records.spell_score(progress.character+progress.subcharacter,(player.mode_flags&0x300)==0,progress.spell_id,signed_bits((u32(score.score)/10)*10)),"Spell practice score record failed"))return false;
  if((player.mode_flags&0x30)!=0x20&&!check(records.stage_clear(progress.character+progress.subcharacter,progress.stage,progress.difficulty),"Practice clear record failed"))return false;
  if(!acb)return check(services.finish_practice(),"Practice completion failed");
 }
 {
  if(progress.stage==6){progress.scene_flags|=0x4000;ending_frames=0;hud_flags|=0x10;if(!check(services.prepare_ending(),"Final stage ending setup failed"))return false;
   if((player.mode_flags&0x300)&&!check(records.remove_checkpoint(progress.character,progress.difficulty),"Completed checkpoint deletion failed"))return false;
   i32 bonus=0;if(progress.difficulty>=0&&progress.difficulty<4){if(!(player.mode_flags&0x300))bonus=wrapping_mul(player.extra_lives,10000000);bonus=wrapping_add(bonus,wrapping_mul(player.bombs,3000000));}award(bonus);
   // 43daac is after the bonus addition, before replay/clear registration.
   if(acb){if(!check(services.synchronize_clear_score(),"Clear score synchronization failed"))return false;if(player.mode_flags&0x10)return check(services.finish_practice(),"Practice completion failed");}
   if(progress.transition)return check(services.finish_replay(),"Final replay completion failed");return record_finish();
  }
  if(progress.stage==7){hud_flags|=0x10;award(wrapping_mul(wrapping_add(player.bombs,wrapping_mul(player.extra_lives,10)),4000000));if(!record_finish())return false;
   if(progress.transition&&!check(services.finish_replay(),"Extra replay completion failed"))return false;
   if(!check(services.prepare_ending(),"Extra ending setup failed"))return false;progress.scene_flags|=0x4000;ending_frames=0;return true;
  }
  // The second hook is the non-final stage branch (43dcb5), NOT Extra's
  // final bonus. Preserve that source asymmetry rather than inventing one.
  if(acb){if(!check(services.synchronize_clear_score(),"Clear score synchronization failed"))return false;if(player.mode_flags&0x10)return check(services.finish_practice(),"Practice completion failed");}
  if(!progress.transition&&!check(records.stage_clear(progress.character+progress.subcharacter,progress.stage,progress.difficulty),"Stage clear record failed"))return false;
  if(!check(services.queue_next_scene(),"Next stage scene request failed"))return false;if(progress.stage<7)progress.stage=wrapping_add(progress.stage,1);return check(services.select_stage_resources(progress.stage),"Next stage resource selection failed");
 }
}
}
