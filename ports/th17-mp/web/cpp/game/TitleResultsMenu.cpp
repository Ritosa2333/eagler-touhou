#include "TitleResultsMenu.hpp"
#include <cstdio>
#include "LegacyTextFormat.hpp"
namespace th17 {
namespace {
constexpr char alphabet[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-=.,!?@:;[]()_/{}|~^#$%&*   ";
constexpr const char* stages[]={"test   ","Stage 1","Stage 2","Stage 3","Stage 4","Stage 5","Stage 6","Extra  ","Clear  ","ExClear"};
std::string short_name(const char* text){u32 n=0;while(n<8&&text[n])n++;return {text,n};}
}
bool TitleResultsMenu::check(bool ok,const char* reason){if(!ok&&error.empty())error=reason;return ok;}
bool TitleResultsMenu::visual(bool ok){return check(ok,visuals.error.c_str());}
bool TitleResultsMenu::prepare_name(){
 if(records.name[8])return check(false,"Result stored name exceeds eight bytes");names.select(0);names.count=91;names.wrap=true;for(u32 n=0;n<9;n++){name[n]=records.name[n];if(!name[n])break;}
 constexpr std::array<char,9> empty{' ',' ',' ',' ',' ',' ',' ',' ',0};if(name!=empty)names.step(-1);name_length=8;while(name_length>0&&name[name_length-1]==' ')name_length--;return true;
}
bool TitleResultsMenu::append(char value){if(name_length<0||name_length>8)return check(false,"Result name length outside original range");if(name_length<8){name[name_length++]=value;if(name_length>=8)names.select(90);}else name[name_length-1]=value;return true;}
bool TitleResultsMenu::finish_name(){if(!check(records.score_name(progress.character*3+progress.animal,progress.difficulty,state.menu.selected,name),"Completed result name save failed"))return false;state.change_substate(3);return true;}
bool TitleResultsMenu::update(u32 pressed,u32 repeated){
 if(!error.empty())return false;const i32 character=progress.character*3+progress.animal;if(character<0||character>=9||progress.difficulty<0||progress.difficulty>=5)return check(false,"Completed result selection outside original range");
 switch(state.substate){
 case 0:{
  state.menu.count=30;if(!check(host.music("th128_08",17),"Completed result music failed")||!visual(visuals.prompt())||!visual(visuals.create(100)))return false;state.change_substate(1);
  if(!visual(visuals.create(character+148))||!visual(visuals.create(progress.difficulty+157)))return false;
  if(!animations.registry.find(state.handles[86])&&(!visual(visuals.create(86))||!visual(visuals.interrupt(86,3,true))))return false;
  progress.stage=8;i32 rank=0;const auto& rows=records.scores[character][progress.difficulty];while(rank<10&&u32(rows[rank].score)>u32(score.score))rank++;if(rank==10)rank=-1;
  if(rank>=0){PauseScoreDetails details;if(!check(host.capture_score_details(details),"Completed result timestamp unavailable"))return false;RunScoreSubmission row;row.score=score.score;row.stage=8;row.continues=progress.scoring.continues;row.timestamp=details.timestamp;row.slowdown=float(100.f-float(float(details.delivered/details.requested)*100.f));if(records.insert_score(character,progress.difficulty,row)!=rank)return check(false,"Completed result rank changed during registration");}
  progress.stage=progress.initial_stage=0;state.menu.select(rank);name_not_required=rank<0;
  if(rank>=0){state.age.set(0);state.menu.wrap=true;if(!prepare_name())return false;}
  [[fallthrough]];
 }
 case 1:if(state.age.current>6)state.change_substate(2);break;
 case 2:
  if(!name_not_required){names.reserved=names.selected;const u32 input=pressed|repeated;if(input&16)names.step(-13);if(input&32)names.step(13);if(input&64)names.step(names.selected%13==0?12:-1);if(input&128)names.step(names.selected%13==12?-12:1);if(names.reserved!=names.selected&&!check(host.sound(10),"Result name cursor sound failed"))return false;}
  if(pressed&0x80001){
   if(name_not_required)state.change_substate(3);
   else if(names.selected<0||names.selected>=91)return check(false,"Result name grid cursor outside original range");
   else if(names.selected<88){if(!append(alphabet[names.selected]))return false;}
   else if(names.selected==88){if(!append(' '))return false;}
   else if(names.selected==89){if(name_length==0)return true;name[--name_length]=' ';}
   else if(!finish_name())return false;
   if(!check(host.sound(7),"Result name confirm sound failed"))return false;
  }
  if(pressed&0x102){if(name_not_required){state.change_substate(3);return check(host.sound(7),"Result dismissal sound failed");}if(name_length){if(!check(host.sound(9),"Result name deletion sound failed"))return false;name[--name_length]=' ';}}
  break;
 case 3:
  if(state.age.current>5){if(!visual(visuals.retire(100))||!visual(visuals.retire(character+148))||!visual(visuals.retire(progress.difficulty+157)))return false;
   if(progress.scoring.continues==0){state.change_screen(TitleScreen::RankingName);return true;}
   if(!check(host.release_live_replay(),"Completed result replay release failed")||!visual(visuals.retire(TitleAnimations::prompt_slot)))return false;state.menu.pop();state.change_screen(TitleScreen::Main);return check(host.music("th17_01",0),"Completed result title music failed");
  }break;
 }
 return true;
}
bool TitleResultsMenu::draw(HudDrawServices& sink,ReplayCalendarServices& dates){
 if(!error.empty())return false;if(state.substate!=2)return true;const i32 character=progress.character*3+progress.animal,rank=progress.difficulty;if(character<0||character>=9||rank<0||rank>=5)return check(false,"Completed score drawing selection outside original range");
 const auto& rows=records.scores[character][rank];HudTextDraw text;text.style.font=0;text.style.shadow=true;auto emit=[&](){return check(sink.hud_text(text),"Completed score text submission failed");};
 for(i32 i=0;i<10;i++){
  const auto& row=rows[i];text.position={22,float(160+i*18),0};const u32 shade=255-i*16;text.style.color=name_not_required?(0xffffff00|shade):(state.menu.selected==i?0xffffffff:0xff808040);
  char line[256];const auto title=short_name(row.name.data());
  if(row.details[0]||row.details[1]){ReplayCalendar date;const i64 timestamp=i64(u64(u32(row.details[0]))|(u64(u32(row.details[1]))<<32));if(!check(dates.calendar(timestamp,date),"Completed score date unavailable"))return false;float slowdown;const u32 bits=u32(row.details[2]);std::memcpy(&slowdown,&bits,4);const auto percent=legacy_decimal_tenth(slowdown);const i32 stage=i8(row.stage);if(stage<0||stage>=10)return check(false,"Completed score stage label outside original range");std::snprintf(line,sizeof line,"%2d  %s  %9ld%d  %.4d/%.2d/%.2d %.2d:%.2d  %s  %s%%",i+1,title.c_str(),long(row.score),i32(row.continues),date.year,date.month,date.day,date.hour,date.minute,stages[stage],percent.c_str());
  }else std::snprintf(line,sizeof line,"%2d  %s  %9ld%d  ----/--/-- --:--  Stage -  ---%%",i+1,title.c_str(),long(row.score),i32(row.continues));text.text=line;if(!emit())return false;
 }
 if(!name_not_required){text.style.color=0xffffffff;text.position={58,float(160+state.menu.selected*18),0};text.text=short_name(name.data());if(!emit())return false;text.style.color=0xffffff00;text.position.x=float(58+name_length*9-(name_length==8?9:0));text.text="_";if(!emit())return false;
  for(i32 i=0;i<91;i++){text.position={float(212+(i%13)*18),float(360+(i/13)*16),0};text.style.color=names.selected==i?0xffffff00:0xff808080;text.text.assign(1,i<88?alphabet[i]:char(i==88?0x81:i==89?0x7f:0x80));if(!emit())return false;}
 }return true;
}
}
