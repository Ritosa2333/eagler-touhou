#include "TitleFrame.hpp"
namespace th15 {
TitleFrame::TitleFrame(TitleState& s,SessionState& p,PlayerLifeSession& v,RecordStore& r,AnmManager& a,TitleFrameServices& h,FrameScheduler& f,i32 title,i32 ascii):state(s),progress(p),player(v),records(r),host(h),scheduler(f),visuals(s,a,title,ascii){
 callbacks[0].owner=callbacks[1].owner=this;callbacks[0].enabled=true;callbacks[1].enabled=false;
 callbacks[0].run=[](void* p){auto& f=*static_cast<TitleFrame*>(p);return f.update(f.input)?i32(FrameAction::Continue):i32(FrameAction::Error);};callbacks[1].run=[](void* p){auto& f=*static_cast<TitleFrame*>(p);return f.draw()?i32(FrameAction::Continue):i32(FrameAction::Error);};
 if(scheduler.add(callbacks[0],FramePass::Update,6)<0||scheduler.add(callbacks[1],FramePass::Draw,66)<0)check(false,"Title callback registration failed");
}
TitleFrame::~TitleFrame(){for(auto& callback:callbacks)scheduler.remove(callback);}
bool TitleFrame::check(bool ok,const char* why){if(!ok&&error.empty())error=why;return ok;}
bool TitleFrame::menu(){return check(host.update_title_menu(state.screen,input),"Title menu update failed");}
bool TitleFrame::restore_music(bool stop_previous){
 if(stop_previous&&!check(host.queue_title_music(alternate_audio?4:3,0,"dummy"),"Previous title music stop failed"))return false;
 if(stop_previous&&!check(host.clear_current_wave(),"Title music name reset failed"))return false;
 if(!check(host.queue_title_music(1,0,"th15_01.wav"),"Title music preparation failed"))return false;
 if(alternate_audio&&!check(host.queue_title_music(4,0,"dummy"),"Alternate title music release failed"))return false;
 return check(host.queue_title_music(2,0,"dummy"),"Title music start failed")&&check(records.unlock_music(0),"Title music unlock failed");
}
bool TitleFrame::demo(){
 demo_idle=wrapping_add(demo_idle,1);if(input.held&0xffff){demo_idle=0;return true;}if(demo_idle<1800)return true;
 player.mode_flags=(player.mode_flags&~0x80u)|0x40;std::shared_ptr<Replay> file;const i32 selected=demo_index;if(selected<0||selected>=3)return check(false,"Title demo index outside original range");if(!check(host.read_demo(selected,file),"Title demo file unavailable")||!file)return check(false,"Title demo metadata unavailable");demo_index=wrapping_add(demo_index,1)%3;
 i32 stage=0;while(stage<8&&!file->stage(stage))stage++;if(stage>=8)return check(false,"Title demo has no recorded stage");progress.stage=progress.starting_stage=stage;const auto& metadata=file->decoded();if(metadata.size()<0xa4)return check(false,"Title demo description truncated");auto word=[&](u32 at){u32 value;std::memcpy(&value,metadata.data()+at,4);return signed_bits(value);};progress.character=word(0x8c);progress.subcharacter=word(0x90);saved_difficulty=progress.difficulty;progress.difficulty=word(0x94);player.mode_flags&=~0x300u;state.return_reason=1;demo_idle=0;
 return check(host.begin_demo({std::string("demo/demo")+std::to_string(selected)+".rpy",stage,progress.character,progress.subcharacter,progress.difficulty,-1,false,selected}),"Title demo destination failed");
}
bool TitleFrame::initialize(){
 if(!check(host.release_transient_animations(),"Title transient animation release failed")||!check(host.clear_title_overlay(),"Title text overlay release failed"))return false;
 const i32 reason=state.return_reason;auto main=[&](){state.change_screen(TitleScreen::Main);state.return_reason=1;callbacks[1].enabled=true;return menu();};
 if(reason==3){state.menu.count=10;state.menu.select(0);state.menu.push();state.change_screen(TitleScreen::Records);state.return_reason=1;callbacks[1].enabled=true;return menu();}
 if(!(player.mode_flags&0x40)){state.flags|=1;music_age=0;}else {state.flags&=~1u;progress.difficulty=saved_difficulty;}player.mode_flags&=~0x40u;
 if(reason==0){state.flags|=2;return main();}state.flags&=~2u;
 if(reason==1){if(progress.difficulty==4)state.menu.select(1);return main();}
 if(reason==2){if(!restore_music(false)||!check(host.reset_replay_selection(),"Replay return session reset failed"))return false;state.menu.count=10;state.menu.select(4);state.menu.push();state.change_screen(TitleScreen::Replay);state.return_reason=1;callbacks[1].enabled=true;return menu();}
 if(reason==4){if(!check(host.return_practice_transition(),"Practice return transition failed"))return false;if(!visuals.create(89)||!visuals.interrupt(89,3,true))return check(false,visuals.error.c_str());state.change_screen(TitleScreen::Difficulty);callbacks[1].enabled=true;return menu();}
 return main();
}
bool TitleFrame::update(const TitleFrameInput& value){
 if(!error.empty())return false;input=value;if(state.screen==TitleScreen::Main&&!demo())return false;
 if(state.flags&1){music_age=wrapping_add(music_age,1);if(music_age>=10){if(!restore_music(true))return false;state.flags&=~1u;music_age=0;}}
 switch(i32(state.screen)){
 case 0:if(!initialize())return false;break;
 case 1:callbacks[1].enabled=true;if(!menu())return false;break;
 case 2:if(!check(host.title_exit((~(input.system_flags>>13)&1)|2),"Title exit destination failed"))return false;[[fallthrough]];
 case 10:case 13:if(!check(host.fade_out_title_music(),"Title exit music fade failed"))return false;break;
 case 3:case 4:case 5:case 6:case 7:case 9:case 11:case 12:case 14:case 15:case 16:case 17:case 21:if(!menu())return false;break;
 default:break;
 }
 state.age.tick(&progress.rate);return true;
}
bool TitleFrame::draw(){if(!error.empty())return false;switch(i32(state.screen)){case 9:case 11:case 12:case 15:case 16:return check(host.draw_title_menu(state.screen),"Title menu text drawing failed");default:return true;}}
}
