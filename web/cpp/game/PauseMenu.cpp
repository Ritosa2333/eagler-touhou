#include "PauseMenu.hpp"
namespace th17 {
bool PauseMenu::check(bool ok,const char* reason){if(!ok&&error.empty())error=reason;return ok;}
bool PauseMenu::sound(i32 id){return check(host.sound(id),"Pause sound failed");}
bool PauseMenu::interrupt(u32 handle,i32 label,bool immediate){return check(animations.interrupt(handle,label,immediate),"Pause animation interrupt failed");}
bool PauseMenu::child(i32 script,i32 label){auto* root=animations.registry.find(state.menu_animation);if(!root){state.menu_animation=0;return true;}auto* vm=animations.registry.find_child_script(*root,script,0);return !vm||interrupt(animations.registry.handle(*vm),label);}
bool PauseMenu::freeze(bool paused){return check(animations.pause(state.menu_animation,paused),"Pause menu freeze failed");}
bool PauseMenu::disable(i32 entry){return check(state.menu.disable(entry),"Pause disabled selection failed");}
bool PauseMenu::navigate(u32 pressed,u32 repeated,i32 label,bool visual){auto& menu=state.menu;menu.reserved=menu.selected;if((pressed|repeated)&0x10)menu.step(-1);if((pressed|repeated)&0x20)menu.step(1);return menu.reserved==menu.selected||((!visual||interrupt(state.menu_animation,i32(i16(menu.selected))+label))&&sound(10));}
bool PauseMenu::replay_slots(){for(i32 slot=0;slot<25;slot++)if(!check(host.read_replay_slot(slot,replays[slot]),"Pause replay catalog failed"))return false;return true;}
bool PauseMenu::write_replay(i32 slot,const std::array<char,9>& name){return slot>=0&&slot<25&&check(host.save_named_replay(slot,name),"Pause named replay save failed")&&check(host.read_replay_slot(slot,replays[slot]),"Saved replay catalog refresh failed");}
bool PauseMenu::prepare_results_menu(){
 state.select_phase(6);auto& menu=state.menu;menu.count=5;menu.wrap=1;i32 script,selected=0;
 if(!(state.flags&4)&&!(progress.scoring.mode_flags&0x30)){
  script=163;if(progress.scoring.continues>0&&!disable(2))return false;if(progress.ranking_default<1){if(!disable(0))return false;selected=1;}
 }else{script=164;if(!disable(0)||!disable(3))return false;menu.select(0);selected=(state.flags&4)?menu.selected:4;}
 state.menu_animation=animations.create_overlay(state.front_bank,script);if(!check(state.menu_animation!=0,"Pause result menu creation failed"))return false;
 menu.select(selected);return interrupt(state.menu_animation,3,true)&&interrupt(state.menu_animation,i32(i16(menu.selected))+7);
}
bool PauseMenu::update(u32 pressed,u32 repeated){
 if(!error.empty())return false;auto& menu=state.menu;const i32 age=state.age.current;auto phase=[&](i32 next){state.select_phase(next);};auto close=[&](){return interrupt(state.snapshot_animation,1)&&interrupt(state.menu_animation,1);};
 switch(state.phase){
 case 0:
  if(age<=9)break;phase(6);menu.count=5;
  if(progress.practice_record&&(!disable(2)||!disable(3)))return false;
  if(progress.scoring.continues>0){if(!disable(2)||!interrupt(state.menu_animation,i32(i16(menu.selected))+7)||!child(133,5)||!child(142,5)||!child(154,5))return false;}
  menu.wrap=true;menu.select(0);state.retry_controls=false;return interrupt(state.menu_animation,i32(i16(menu.selected))+7);
 case 1:
  if(age<=9)break;phase(6);menu.count=5;if(!disable(3)||!disable(2)||!disable(0))return false;menu.wrap=true;menu.select(1);state.retry_controls=true;return interrupt(state.menu_animation,i32(i16(menu.selected))+7);
 case 2:case 3:case 4:case 5:
  if(age<10)break;if(!check(host.synchronize_result_score(),"Result score synchronization failed"))return false;
  if(!check(registration.register_result(),"Result score registration failed"))return false;
  if(!state.name_not_required){phase(15);return freeze(true);}return prepare_results_menu();
 case 6:
  if(!navigate(pressed,repeated,7))return false;
  if(pressed&0x80001){if(!sound(7))return false;switch(menu.selected){
   case 0:if(!close())return false;phase(16);break;
   case 1:for(i32 script:{132,141,149,151,153})if(!child(script,6))return false;phase(!progress.practice_record&&state.screen==PauseScreen::Pause?7:16);break;
   case 2:for(i32 script:{133,142,154})if(!child(script,6))return false;phase(state.screen==PauseScreen::Pause?9:10);break;
   case 3:if(!child(134,6)||!child(143,6))return false;phase(14);break;
   case 4:for(i32 script:{135,144,150,152,155})if(!child(script,6))return false;phase(!progress.practice_record&&state.screen==PauseScreen::Pause?7:16);break;
  }state.age.set(0);}
  if(!state.retry_controls){if((pressed&0x200000)&&menu.is_disabled(menu.selected)){for(i32 script:{135,144,150,152,155})if(!child(script,6))return false;phase(!progress.practice_record&&state.screen==PauseScreen::Pause?7:16);return true;}if(pressed&0x100){if(!close())return false;menu.select(0);phase(16);break;}}
  if(pressed&0x10000){if(!sound(7)||!child(132,6))return false;menu.select(1);phase(16);}break;
 case 7:case 9:
  if(age<20)break;if(age==20){menu.push();menu.count=2;menu.wrap=true;menu.select(1);if(!interrupt(state.menu_animation,14))return false;}
  if(age<30)break;if(age==30&&!interrupt(state.menu_animation,i32(i16(menu.selected))+15))return false;if(!navigate(pressed,repeated,15))return false;
  if(pressed&0x80001){if(menu.selected==0){if(!child(157,6))return false;phase(state.phase==9?10:8);if(!sound(7))return false;}else if(menu.selected==1){if(!child(158,6))return false;phase(8);if(!sound(9))return false;}}
  if(pressed&0x102){if(!sound(9))return false;if(menu.selected==0){menu.select(1);if(!interrupt(state.menu_animation,i32(i16(menu.selected))+15))return false;}else if(menu.selected==1){if(!child(158,6))return false;phase(8);}}
  if((pressed&0x100)&&state.screen==PauseScreen::Pause){if(!close())return false;menu.select(0);phase(16);}break;
 case 8:
  if(age<20)break;if(menu.selected==0){if(!interrupt(state.menu_animation,1))return false;menu.pop();phase(16);}else if(menu.selected==1){menu.pop();if(progress.scoring.continues>0)if(!disable(2))return false;if(!interrupt(state.menu_animation,i32(i16(menu.selected))+7))return false;phase(6);}break;
 case 10:
  if(age<20)break;state.flags=(state.flags&~2u)|1;phase(11);if(!freeze(true))return false;menu.push();menu.count=25;menu.wrap=true;menu.select(0);return replay_slots();
 case 11:
  if(age<10)break;if(!navigate(pressed,repeated,0,false))return false;
  if(pressed&0x80001){state.flags=(state.flags&~1u)|2;phase(12);if(!check(host.prepare_replay_save(state.result_mode!=0&&!(progress.scoring.mode_flags&0x30)),"Replay save finalization failed"))return false;return check(names.prepare(),"Replay name preparation failed")&&sound(7);}
  if(pressed&0x102){state.flags&=~3u;menu.pop();menu.count=5;menu.wrap=true;if(!interrupt(state.menu_animation,i32(i16(menu.selected))+7))return false;replays={};if(state.screen==PauseScreen::Pause){phase(16);menu.select(1);}else{phase(6);if(!freeze(false))return false;if(progress.scoring.mode_flags&0x30)if(!disable(0)||!disable(3))return false;}return sound(9);}break;
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
