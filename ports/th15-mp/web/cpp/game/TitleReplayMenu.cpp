#include "TitleReplayMenu.hpp"
namespace th15 {
bool TitleReplayMenu::check(bool ok,const char* reason){if(!ok&&error.empty())error=reason;return ok;}
bool TitleReplayMenu::visual(bool ok){if(!ok&&error.empty())error=visuals.error.empty()?state.menu.error:visuals.error;return ok;}
bool TitleReplayMenu::choose_stage(){
 selected_replay=pages.cursor*25+state.menu.cursor;const auto* entry=catalog.entry(selected_replay);if(!entry)return true;
 state.change_substate(4);state.menu.push();if(!check(services.sound(7),"Replay confirmation sound failed"))return false;state.menu.count=7;state.menu.select(0);
 for(i32 stage=1;stage<=7;stage++)if(!entry->file->stage(stage)&&!state.menu.disable(stage-1))return check(false,state.menu.error.c_str());
 state.menu.move(-1);state.menu.move(1);if(!state.menu.error.empty())return check(false,state.menu.error.c_str());
 return !(entry->file->mode_flags()&2)||launch();
}
bool TitleReplayMenu::launch(){selected_stage=state.menu.cursor;state.change_substate(3);state.flags|=4;return check(services.sound(50),"Replay start sound failed")&&check(services.fade_music(.05f),"Replay start music fade failed");}
bool TitleReplayMenu::update(u32 pressed,u32 repeated){
 if(!error.empty())return false;switch(state.substate){
 case 0:
  state.menu.count=25;state.menu.select(saved_selection%25);pages.count=3;pages.select(saved_selection/25);pages.wrapping=true;saved_selection=0;
  if(!visual(visuals.prompt())||!visual(visuals.create(101)))return false;state.change_substate(1);catalog.clear();state.flags&=~12u;selected_replay=0;
  if(!check(services.request_catalog(),"Replay catalog request failed"))return false;[[fallthrough]];
 case 1:
  if(!animations.registry.find(state.handles[89])){if(!visual(visuals.create(89))||!visual(visuals.interrupt(89,3,true)))return false;}
  if(state.age.current>6)state.change_substate(2);break;
 case 2:
  state.menu.previous=state.menu.cursor;pages.previous=pages.cursor;
  if((pressed|repeated)&16)state.menu.move(-1);if((pressed|repeated)&32)state.menu.move(1);
  if((pressed|repeated)&64)pages.move(-1);if((pressed|repeated)&128)pages.move(1);
  if(!state.menu.error.empty()||!pages.error.empty())return check(false,"Replay cursor navigation failed");
  if(pages.previous!=pages.cursor&&!check(services.sound(10),"Replay page sound failed"))return false;
  if(state.menu.previous!=state.menu.cursor&&!check(services.sound(10),"Replay cursor sound failed"))return false;
  if(pressed&0x102){state.change_substate(5);state.flags|=4;return check(services.sound(9),"Replay cancel sound failed");}
  if((pressed&0x80001)&&catalog.entry(pages.cursor*25+state.menu.cursor))return choose_stage();break;
 case 4:
  if(state.age.current<15)break;state.menu.previous=state.menu.cursor;
  if((pressed|repeated)&16)state.menu.move(-1);if((pressed|repeated)&32)state.menu.move(1);
  if(!state.menu.error.empty())return check(false,state.menu.error.c_str());
  if(state.menu.previous!=state.menu.cursor&&!check(services.sound(10),"Replay stage sound failed"))return false;
  if(pressed&0x102){state.menu.pop();state.menu.count=25;state.menu.disabled_count=0;state.change_substate(2);return check(services.sound(9),"Replay stage cancellation failed");}
  if(pressed&0x80001)return launch();break;
 case 3:
  if(state.age.current==2){if(!check(services.begin_transition(transition),"Replay transition unavailable")||!check(animations.interrupt(transition,7),"Replay transition animation failed")||!check(services.transition_size(392,480),"Replay transition layout failed"))return false;}
  if(state.age.current>=32&&(state.flags&8)){
   const auto* entry=catalog.entry(selected_replay);if(!entry)return check(false,"Selected replay disappeared");const auto& file=*entry->file;const auto& data=file.decoded();
   progress.stage=selected_stage+1;progress.character=file.character();progress.subcharacter=0;progress.difficulty=file.difficulty();player.mode_flags&=~0x300u;
   const bool spell=(file.mode_flags()&2)!=0;player.mode_flags=(player.mode_flags&~0x30u)|(spell?0x20u:0);progress.spell_id=-1;if(spell)std::memcpy(&progress.spell_id,data.data()+0xa0,4);
   saved_selection=selected_replay;state.change_screen(TitleScreen::StartGame);
   return check(services.start_replay({entry->filename,progress.stage,progress.character,progress.subcharacter,progress.difficulty,progress.spell_id,spell,selected_replay}),"Replay scene destination failed");
  }break;
 case 5:
  if(state.age.current>=6&&(state.flags&8)){if(!check(services.release_catalog(),"Replay catalog release failed"))return false;catalog.clear();if(!visual(visuals.retire(101))||!visual(visuals.retire(203)))return false;state.change_screen(TitleScreen::Main);state.menu.pop();}break;
 }
 return true;
}
}

#include <cstdio>
namespace th15 {
namespace {
u32 replay_dword(const u8* p){u32 n;std::memcpy(&n,p,4);return n;}
const char* replay_completed[]={"tst","St1","St2","St3","St4","St5","St6","Ex ","All","ExA"};
const char* replay_stage_names[]={"test   ","Stage 1","Stage 2","Stage 3","Stage 4","Stage 5","Stage 6","Extra  "};
const char* replay_rank_names[]={"Easy   ","Normal ","Hard   ","Lunatic","Extra  "};
const char* replay_character_names[]={"Reimu  ","Marisa ","Sanae  ","Reisen "};
}
bool TitleReplayMenu::draw(HudDrawServices& sink,ReplayCalendarServices& dates){
 if(!error.empty())return false;if(state.substate!=2&&state.substate!=4)return true;
 HudTextDraw text;text.position={58,80,0};text.style.font=0;text.style.shadow=true;
 auto emit=[&](){return check(sink.hud_text(text),"Replay text submission failed");};
 auto caption=[&](i32 index)->bool{
  const auto* entry=catalog.entry(index);char buffer[256];
  if(!entry){if(pages.cursor==0)std::snprintf(buffer,sizeof buffer,"No.%.2d -------- --/--/-- --:-- ------- ------- --- ---%%",index+1);else std::snprintf(buffer,sizeof buffer,"User  -------- --/--/-- --:-- ------- ------- --- ---%%");text.text=buffer;return true;}
  const auto& bytes=entry->file->decoded();const u8* m=bytes.data();i64 timestamp;std::memcpy(&timestamp,m+0xc,8);ReplayCalendar date;
  if(!check(dates.calendar(timestamp,date),"Replay calendar conversion failed"))return false;
  const u32 character=replay_dword(m+0x8c)+replay_dword(m+0x90),rank=replay_dword(m+0x94),completed=replay_dword(m+0x98);
  if(character>=4||rank>=5||completed>=10)return check(false,"Replay display metadata out of range");
  std::string name(reinterpret_cast<const char*>(m),8);name.resize(name.find(char(0))==std::string::npos?name.size():name.find(char(0)));
  float slow;std::memcpy(&slow,m+0x84,4);
  if(pages.cursor==0){
   if(m[10]&2)std::snprintf(buffer,sizeof buffer,"No.%.2d %s %.2d/%.2d/%.2d %.2d:%.2d %s SpellPr %3d %2.1f%%",index+1,name.c_str(),date.year%100,date.month,date.day,date.hour,date.minute,replay_character_names[character],wrapping_add(signed_bits(replay_dword(m+0xa0)),1),double(slow));
   else std::snprintf(buffer,sizeof buffer,"No.%.2d %s %.2d/%.2d/%.2d %.2d:%.2d %s %s %s %2.1f%%",index+1,name.c_str(),date.year%100,date.month,date.day,date.hour,date.minute,replay_character_names[character],replay_rank_names[rank],replay_completed[completed],double(slow));
  }else{
   // The original uses exactly four bytes from filename + 7 for user entries.
   char tag[5]{};for(u32 i=0;i<4&&i+7<entry->filename.size();i++)tag[i]=entry->filename[i+7];
   std::snprintf(buffer,sizeof buffer,"%s  %s %.2d/%.2d/%.2d %.2d:%.2d %s %s %s %2.1f%%",tag,name.c_str(),date.year%100,date.month,date.day,date.hour,date.minute,replay_character_names[character],replay_rank_names[rank],replay_completed[completed],double(slow));
  }
  text.text=buffer;return true;
 };
 if(state.substate==2){for(i32 row=0;row<25;row++){text.style.color=state.menu.cursor==row?0xffffff00:0xff808080;if(!caption(pages.cursor*25+row)||!emit())return false;text.position.y=float(text.position.y+15.f);}return true;}
 const auto* entry=catalog.entry(selected_replay);if(!entry)return check(false,"Selected replay unavailable for drawing");
 if(state.age.current<10)text.position.y=float(float(float(float(10.f-state.age.fractional)*float((selected_replay%25)*15))/10.f)+80.f);
 if(!caption(selected_replay)||!emit())return false;if(state.age.current<10)return true;text.position={220,128,0};
 const auto& file=*entry->file;const auto& metadata=file.decoded();
 for(i32 stage=1;stage<=7;stage++){
  char buffer[80];text.style.color=state.menu.cursor==stage-1?0xffffff00:0xff808080;
  if(!file.stage(stage))std::snprintf(buffer,sizeof buffer,"%s  ---------",replay_stage_names[stage]);
  else {const u8* score=stage<6&&file.stage(stage+1)?file.header(stage+1)+0x30:metadata.data()+0x14;const u8* continues=stage<6&&file.stage(stage+1)?file.header(stage+1)+0x38:metadata.data()+0x9c;
   std::snprintf(buffer,sizeof buffer,"%s  %.8d%d",replay_stage_names[stage],signed_bits(replay_dword(score)),signed_bits(replay_dword(continues)));}
  text.text=buffer;if(!emit())return false;text.position.y=float(text.position.y+18.f);
 }
 return true;
}
}
