#include "PauseMenu.hpp"
namespace th15 {
bool PauseMenu::check(bool ok,const char* reason){if(!ok&&error.empty())error=reason;return ok;}
bool PauseMenu::sound(i32 id){return check(host.sound(id),"Pause sound failed");}
bool PauseMenu::interrupt(u32 handle,i32 label,bool immediate){return check(animations.interrupt(handle,label,immediate),"Pause animation interrupt failed");}
bool PauseMenu::child(i32 script,i32 label){auto* root=animations.registry.find(state.menu_animation);if(!root){state.menu_animation=0;return true;}auto* vm=animations.registry.find_child_script(*root,script,0);return !vm||interrupt(animations.registry.handle(*vm),label);}
bool PauseMenu::freeze(bool paused){return check(animations.pause(state.menu_animation,paused),"Pause menu freeze failed");}
bool PauseMenu::disable(i32 entry){return check(state.menu.disable(entry),"Pause disabled selection failed");}
bool PauseMenu::navigate(u32 pressed,u32 repeated,i32 label,bool visual){auto& menu=state.menu;menu.previous=menu.cursor;if((pressed|repeated)&0x10)menu.move(-1);if((pressed|repeated)&0x20)menu.move(1);if(!menu.error.empty())return check(false,"Pause cursor movement failed");return menu.previous==menu.cursor||((!visual||interrupt(state.menu_animation,i32(i16(menu.cursor))+label))&&sound(10));}
bool PauseMenu::replay_slots(){for(i32 slot=0;slot<25;slot++)if(!check(host.read_replay_slot(slot,replays[slot]),"Pause replay catalog failed"))return false;return true;}
bool PauseMenu::restrict_replay_save(){
 if(host.replay_save_available())return true;
 for(i32 n=0;n<state.menu.disabled_count;n++)if(state.menu.disabled[n]==2)return true;
 // Rechecked while paused: a shortcut may enable assistance after opening.
 // Do not append duplicate disabled entries to the original bounded cursor.
 return disable(2)&&child(123,5)&&child(132,5)&&child(144,5)&&interrupt(state.menu_animation,i32(i16(state.menu.cursor))+7);
}
bool PauseMenu::write_replay(i32 slot,const std::array<char,9>& name){return slot>=0&&slot<25&&check(host.save_named_replay(slot,name),"Pause named replay save failed")&&check(host.read_replay_slot(slot,replays[slot]),"Saved replay catalog refresh failed");}
bool PauseMenu::prepare_results_menu(){
 state.select_phase(6);auto& menu=state.menu;menu.count=5;menu.wrapping=true;i32 script,selected=0;
 script=(state.flags&4)?154:(player.mode_flags&0x300)?156:153;
 state.menu_animation=animations.create_overlay(state.front_bank,script);if(!check(state.menu_animation!=0,"Pause result menu creation failed"))return false;
 if(state.flags&4){if(!disable(0)||!disable(3))return false;}
 else if(!(player.mode_flags&0x300)){if(progress.continues>0&&!disable(2))return false;if(progress.continue_budget<1){if(!disable(0))return false;selected=1;}}
 else{if(!disable(2)||!disable(4))return false;}
 menu.select(selected);return interrupt(state.menu_animation,3,true)&&restrict_replay_save()&&interrupt(state.menu_animation,i32(i16(menu.cursor))+7);
}
bool PauseMenu::update(u32 pressed,u32 repeated){
 if(!error.empty())return false;auto& menu=state.menu;const i32 age=state.age.current;auto phase=[&](i32 next){state.select_phase(next);};auto close=[&](){return interrupt(state.snapshot_animation,1)&&interrupt(state.menu_animation,1);};
 if(!host.replay_save_available()&&(state.phase==10||state.phase==11||state.phase==12)){
  // A cheat can be switched on in the slot/name dialog, too. Unwind only
  // the history pushed by phase 10; never finalize or write this recording.
  if(state.flags&3u)menu.pop();state.flags&=~3u;replays={};phase(6);
  return freeze(false)&&restrict_replay_save();
 }
 switch(state.phase){
 case 0:
  if(age<=9)break;phase(6);menu.count=5;
  if(progress.transition&&(!disable(2)||!disable(3)))return false;
  if(progress.continues>0||(player.mode_flags&0x300)){if(!disable(2)||!interrupt(state.menu_animation,i32(i16(menu.cursor))+7)||!child(123,5)||!child(132,5)||!child(144,5))return false;}
  menu.wrapping=true;menu.select(0);state.retry_controls=false;return restrict_replay_save()&&interrupt(state.menu_animation,i32(i16(menu.cursor))+7);
 case 1:
  if(age<=9)break;phase(6);menu.count=5;if(!disable(3)||!disable(2)||!disable(0))return false;menu.wrapping=true;menu.select(1);state.retry_controls=true;return interrupt(state.menu_animation,i32(i16(menu.cursor))+7);
 case 2:case 3:
  if(age<10)break;if(!check(host.synchronize_result_score(),"Result score synchronization failed"))return false;
  if(!(player.mode_flags&0x300)){if(!check(registration.register_result(),"Result score registration failed"))return false;}else state.name_not_required=true;
  if(!state.name_not_required){phase(15);return freeze(true);}return prepare_results_menu();
 case 6:
  if(!restrict_replay_save())return false;
  if(!navigate(pressed,repeated,7))return false;
  if(pressed&0x80001){if(!sound(7))return false;switch(menu.cursor){
   case 0:if(!close())return false;phase(16);break;
   case 1:for(i32 script:{122,131,139,141,143})if(!child(script,6))return false;phase(!progress.transition&&state.screen==PauseScreen::Pause?7:16);break;
   case 2:for(i32 script:{123,132,144})if(!child(script,6))return false;phase(state.screen==PauseScreen::Pause?9:10);break;
   case 3:if(!child(124,6)||!child(133,6))return false;phase(14);break;
   case 4:for(i32 script:{125,134,140,142,145})if(!child(script,6))return false;phase(!progress.transition&&state.screen==PauseScreen::Pause?7:16);break;
  }state.age.set(0);}
  if(!state.retry_controls){if(pressed&0x200000){if(!sound(7)||!child(125,6)||!interrupt(state.snapshot_animation,1))return false;menu.select(4);phase(16);}if(pressed&0x100){if(!close())return false;menu.select(0);phase(16);break;}}
  if(pressed&0x10000){if(!sound(7)||!child(122,6))return false;menu.select(1);phase(16);}break;
 case 7:case 9:
  if(age<20)break;if(age==20){menu.push();menu.count=2;menu.wrapping=true;menu.select(1);if(!interrupt(state.menu_animation,14))return false;}
  if(age<30)break;if(age==30&&!interrupt(state.menu_animation,i32(i16(menu.cursor))+15))return false;if(!navigate(pressed,repeated,15))return false;
  if(pressed&0x80001){if(menu.cursor==0){if(!child(147,6))return false;phase(state.phase==9?10:8);if(!sound(7))return false;}else if(menu.cursor==1){if(!child(148,6))return false;phase(8);if(!sound(9))return false;}}
  if(pressed&0x102){if(!sound(9))return false;if(menu.cursor==0){menu.select(1);if(!interrupt(state.menu_animation,i32(i16(menu.cursor))+15))return false;}else if(menu.cursor==1){if(!child(148,6))return false;phase(8);}}
  if(pressed&0x100){if(!close())return false;menu.select(0);phase(16);}break;
 case 8:
  if(age<20)break;if(menu.cursor==0){if(!interrupt(state.menu_animation,1))return false;menu.pop();phase(16);}else if(menu.cursor==1){menu.pop();if(progress.continues>0||(player.mode_flags&0x300))if(!disable(2))return false;if(!interrupt(state.menu_animation,i32(i16(menu.cursor))+7))return false;phase(6);}break;
 case 10:
  if(age<20)break;state.flags=(state.flags&~2u)|1;phase(11);if(!freeze(true))return false;menu.push();menu.count=25;menu.wrapping=true;menu.select(0);return replay_slots();
 case 11:
  if(age<10)break;if(!navigate(pressed,repeated,0,false))return false;
  if(pressed&0x80001){state.flags=(state.flags&~1u)|2;phase(12);if(!check(host.prepare_replay_save(state.result_mode!=0&&!(player.mode_flags&0x30)),"Replay save finalization failed"))return false;return check(names.prepare(),"Replay name preparation failed")&&sound(7);}
  if(pressed&0x102){state.flags&=~3u;menu.pop();menu.count=5;menu.wrapping=true;if(!interrupt(state.menu_animation,i32(i16(menu.cursor))+7))return false;replays={};if(state.screen==PauseScreen::Pause){phase(16);menu.select(1);}else{phase(6);if(!freeze(false))return false;}return sound(9);}break;
 case 12:case 15:return check(names.update(pressed,repeated),"Pause name editor failed");
 case 14:
  if(age==20&&(!freeze(true)||!check(host.open_options(32.f),"Pause options opening failed")))return false;
  if(host.options_finished()){if(!check(host.close_options(),"Pause options close failed"))return false;phase(6);return freeze(false);}break;
 case 16:
  if(age<12)break;if(!check(host.finish_pause_selection(),"Pause selected action failed"))return false;state.select(PauseScreen::Inactive);break;
 }
 return true;
}
}
