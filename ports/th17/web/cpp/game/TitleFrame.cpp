#include "TitleFrame.hpp"
namespace th17 {
bool TitleFrame::check(bool ok,const char* why){if(!ok&&error.empty())error=why;return ok;}
bool TitleFrame::menu(const TitleFrameInput& input){return check(host.update_title_menu(state.screen,input),"Title menu update failed");}
bool TitleFrame::restore_music(bool stop){
 if(stop&&(!check(host.queue_title_music(alternate_audio?4:3,0,"dummy"),"Title music stop failed")||!check(host.clear_current_wave(),"Title music filename reset failed")))return false;
 if(!check(host.queue_title_music(1,0,"th17_01.wav"),"Title music preload failed"))return false;
 if(alternate_audio&&!check(host.queue_title_music(4,0,"dummy"),"Title music release failed"))return false;
 return check(host.queue_title_music(2,0,"dummy"),"Title music playback failed")&&check(host.unlock_title_music(),"Title music unlock failed");
}
bool TitleFrame::demo(const TitleFrameInput& input){
 demo_idle=wrapping_add(demo_idle,1);if(input.held&0xffff){demo_idle=0;return true;}if(demo_idle<1800)return true;
 if(demo_index<0||demo_index>=3)return check(false,"Title demonstration index outside range");
 selection.mode_flags=(selection.mode_flags&~0x80u)|0x40;std::shared_ptr<Replay> replay;const i32 index=demo_index;
 if(!check(host.read_demo(index,replay),"Title demonstration unavailable")||!replay)return check(false,"Title demonstration metadata unavailable");
 demo_index=(demo_index+1)%3;i32 stage=1;while(stage<=7&&!replay->stage(stage))stage++;if(stage>7)return check(false,"Title demonstration has no stage");
 selection.stage=stage;selection.character=replay->character();selection.animal=replay->animal();saved_difficulty=selection.difficulty;selection.difficulty=replay->difficulty();state.return_reason=1;demo_idle=0;
 return check(host.begin_demo({"demo/demo"+std::to_string(index+1)+".rpy",index,stage,selection.character,selection.animal,selection.difficulty}),"Title demonstration destination failed");
}
bool TitleFrame::initialize(const TitleFrameInput& input){
 if(!check(host.release_transient_animations(),"Title transient animation release failed")||!check(host.clear_title_overlay(),"Title overlay release failed"))return false;
 const i32 reason=state.return_reason;
 if(reason==3){state.menu.count=10;state.menu.select(0);state.menu.push();state.change_screen(TitleScreen::Ranking);state.return_reason=1;draw_enabled=true;return menu(input);}
 if(!(selection.mode_flags&0x40)){music_age=0;state.flags|=1;}else{state.flags&=~1u;selection.difficulty=saved_difficulty;}selection.mode_flags&=~0x40u;
 state.flags=reason==0?state.flags|2:state.flags&~2u;
 if(reason==2||reason==6){selection.mode(0);state.menu.count=10;state.menu.select(reason==2?4:5);state.menu.push();if(reason==6){state.menu.count=10;state.menu.select(2);state.menu.push();}}
 if(reason==4||reason==5||reason==6){if(!check(host.selection_return_transition(),"Title return transition failed")||!check(visuals.create(86),"Title return background failed")||!check(visuals.interrupt(86,3,true),"Title return background interrupt failed"))return false;state.change_screen(reason==4?TitleScreen::Difficulty:reason==5?TitleScreen::SpellPractice:TitleScreen::Trophies);if(reason!=4)state.return_reason=1;}
 else if(reason==2){state.change_screen(TitleScreen::Replay);state.return_reason=1;}
 else{if(reason==1&&selection.difficulty==4)state.menu.select(1);state.change_screen(TitleScreen::Main);if(reason==0)state.return_reason=1;}
 draw_enabled=true;return menu(input);
}
bool TitleFrame::update(const TitleFrameInput& input){
 if(!error.empty())return false;
 if(host.ending_active()){if(!host.ending_complete())return true;return check(host.retire_ending(),"Ending retirement failed")&&restore_music(false);}
 if(state.screen==TitleScreen::Main&&!demo(input))return false;
 if(state.flags&1){music_age=wrapping_add(music_age,1);if(state.return_reason||music_age>=60){if(!restore_music(true))return false;state.flags&=~1u;music_age=0;}}
 switch(i32(state.screen)){
 case 0:if(!initialize(input))return false;break;
 case 1:draw_enabled=true;if(!menu(input))return false;break;
 case 2:if(!check(host.title_exit((~(input.system_flags>>13)&1)|2),"Title exit failed"))return false;[[fallthrough]];
 case 9:case 13:if(!check(host.fade_out_title_music(),"Title music fade failed"))return false;break;
 case 3:case 4:case 5:case 6:case 7:case 8:case 10:case 11:case 12:case 14:case 15:case 16:case 17:case 18:case 19:case 20:case 21:case 23:if(!menu(input))return false;break;
 default:break;
 }
 state.age.tick(&input.rate);return true;
}
bool TitleFrame::draw(){if(!error.empty())return false;if(host.ending_active())return true;switch(i32(state.screen)){case 8:case 11:case 12:case 15:case 16:case 19:case 20:case 23:return check(host.draw_title_menu(state.screen),"Title menu drawing failed");default:return true;}}
}
