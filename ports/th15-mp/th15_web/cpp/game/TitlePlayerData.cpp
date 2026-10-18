#include "TitlePlayerData.hpp"
#include "Localization.hpp"
#include <algorithm>
#include "LegacyTextFormat.hpp"
#include <cstdio>
namespace th15 {
namespace {
constexpr u8 spell_ranks[]={2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3,4,4,4,4,4,4,4,4,4,4,4,4,4};
constexpr const char* stages[]={"test   ","Stage 1","Stage 2","Stage 3","Stage 4","Stage 5","Stage 6","Extra  ","Clear  ","ExClear"};
std::string short_name(const char* p){u32 n=0;while(n<8&&p[n])n++;return {p,n};}
std::string full_digit(i32 n){std::string value;value+=char(0x82);value+=char(0x4f+n);return value;}
std::string full_space(){return {char(0x81),char(0x40)};}
}
bool TitlePlayerData::check(bool ok,const char* why){if(!ok&&error.empty())error=why;return ok;}
bool TitlePlayerData::visual(bool ok){return check(ok,visuals.error.c_str());}
i32 TitlePlayerData::spell_count()const noexcept{i32 count=0;for(u8 rank:spell_ranks)if(rank==ranks.cursor)count++;return count;}
bool TitlePlayerData::bitmap(i32 row,const std::string& bytes,u32 color){auto* vm=animations.registry.find(state.handles[204+row]);if(!vm){state.handles[204+row]=0;return check(false,"Player Data text animation unavailable");}DialogueText request;request.handle=state.handles[204+row];request.bytes=bytes;request.color=color;return check(host.text(*vm,request),"Player Data bitmap text failed");}
bool TitlePlayerData::paint(){
 if(ranks.cursor<0||ranks.cursor>=5||state.menu.cursor<0||state.menu.cursor>=8||pages.cursor<1)return check(false,"Player Data spell page outside original range");
 i32 skipped=0,index=0;const i32 first=pages.cursor*10-10;while(skipped<first&&index<119){if(spell_ranks[index]==ranks.cursor)skipped++;index++;}displayed_spells=0;
 while(index<119&&displayed_spells<10){if(spell_ranks[index]!=ranks.cursor){index++;continue;}
  const auto& aggregate=records.characters[4].modes[0].spells[index];const auto& personal=records.characters[state.menu.cursor/2].modes[state.menu.cursor%2].spells[index];const i32 number=index+1;std::string digits=(number>=100?full_digit(number/100):full_space())+(number>=10?full_digit((number/10)%10):full_space())+full_digit(number%10);
  std::string label;u32 color;
  if(!aggregate.attempts[0]){for(i32 n=0;n<21;n++){label+=char(0x81);label+=char(0x48);}color=0x808080;}
  else {u32 length=0;while(length<aggregate.name.size()&&aggregate.name[length])length++;if(length>=aggregate.name.size())return check(false,"Player Data spell name unterminated");label.assign(aggregate.name.data(),length);if(length<42){for(u32 n=0;n<(41-length)/2+1;n++)label+=full_space();}color=personal.captures[0]?0xffff80:0xefefef;}
  char tail[64];std::snprintf(tail,sizeof tail," %4d/%4d",personal.captures[0],personal.attempts[0]);
  // Do not store translated names in score/replay data; translate for display.
  const std::string fallback(aggregate.name.data(),std::find(aggregate.name.begin(),aggregate.name.end(),char(0))-aggregate.name.begin());
  const char* translated=Localization::SpellName(u32(index),fallback.c_str(),u32(spell_ranks[index]));
  const bool known=aggregate.attempts[0]!=0;
  const char* plain=known?"No.%s%s%s %s %4d/%4d":"No.%s%s%s ? %4d/%4d";
  const char* format=Localization::LayoutFormatStringById(known?"th15 Result Known Spell":"th15 Result Unknown Spell",plain);
  if(format!=plain&&(!known||translated!=fallback.c_str())){
   // Use thcrap's own Result templates and full-width digit IDs, not a
   // separately invented column layout. Original score names remain intact.
   constexpr const char* unicode_digits[]={"０","１","２","３","４","５","６","７","８","９"};
   auto digit=[&](i32 n){const auto key="th15 Full-width "+std::to_string(n);return Localization::StringById(key.c_str(),unicode_digits[n]);};
   const char* hundreds=number>=100?digit(number/100):"　",*tens=number>=10?digit((number/10)%10):"　",*ones=digit(number%10);
   char localized[4096];const int length=known?std::snprintf(localized,sizeof localized,format,hundreds,tens,ones,translated,personal.captures[0],personal.attempts[0]):std::snprintf(localized,sizeof localized,format,hundreds,tens,ones,personal.captures[0],personal.attempts[0]);
   if(!check(length>=0&&length<i32(sizeof localized),"Localized Player Data row too long")||!bitmap(displayed_spells,localized,color))return false;
  }else if(!bitmap(displayed_spells,"No."+digits+" "+label+tail,color))return false;
  displayed_spells++;index++;
 }
 for(i32 row=displayed_spells;row<10;row++)if(!bitmap(row," ",0xffffffff))return false;return true;
}
bool TitlePlayerData::update(u32 pressed,u32 repeated){
 if(!error.empty())return false;const u32 input=pressed|repeated;
 switch(state.substate){
 case 0:
  state.menu.count=8;state.menu.select(0);ranks.count=5;ranks.select(1);ranks.wrapping=true;pages.count=(spell_count()+9)/10+1;pages.select(0);pages.wrapping=true;
  if(!visual(visuals.prompt())||!visual(visuals.create(102)))return false;state.change_substate(1);if(!visual(visuals.create(state.menu.cursor+155))||!visual(visuals.create(ranks.cursor+163)))return false;
  for(i32 script:{171,172,173,174,168,169,170,175})if(!visual(visuals.create(script)))return false;[[fallthrough]];
 case 1:if(state.age.current>6)state.change_substate(2);break;
 case 2:
  state.menu.previous=state.menu.cursor;ranks.previous=ranks.cursor;pages.previous=pages.cursor;
  if(input&16){ranks.move(-1);if(!visual(visuals.interrupt(173,2,true)))return false;}if(input&32){ranks.move(1);if(!visual(visuals.interrupt(174,2,true)))return false;}
  if(ranks.previous!=ranks.cursor){if(!check(host.sound(10),"Player Data rank sound failed")||!visual(visuals.retire(ranks.previous+163))||!visual(visuals.create(ranks.cursor+163)))return false;if(pages.cursor>0){pages.select(1);if(!paint())return false;}pages.count=(spell_count()+9)/10+1;}
  if(input&64){state.menu.move(-1);if(!visual(visuals.interrupt(171,2,true)))return false;}if(input&128){state.menu.move(1);if(!visual(visuals.interrupt(172,2,true)))return false;}
  if(state.menu.previous!=state.menu.cursor){if(!check(host.sound(10),"Player Data character sound failed")||!visual(visuals.retire(state.menu.previous+155))||!visual(visuals.create(state.menu.cursor+155)))return false;if(pages.cursor>0&&!paint())return false;}
  if(!ranks.error.empty()||!pages.error.empty()||!state.menu.error.empty())return check(false,"Player Data cursor movement failed");
  if(pressed&0x80001){if(pages.cursor==0){for(i32 row=0;row<10;row++){state.handles[204+row]=animations.create(text_bank,row+3,-1,0);if(!state.handles[204+row])return check(false,"Player Data text row creation failed");}}pages.move(1);if(pages.cursor==0){for(i32 row=0;row<10;row++)if(!visual(visuals.interrupt(204+row,1)))return false;}else if(!paint())return false;if(!check(host.sound(7),"Player Data page sound failed"))return false;}
  if(ranks.cursor==4&&state.menu.cursor==3){TitleKeyboard keyboard;bool unlocked=false;if(!check(host.keyboard(keyboard),"Player Data keyboard state unavailable")||!check(cheat.update(pressed,keyboard,unlocked),cheat.error.c_str()))return false;if(unlocked&&!check(host.sound(17),"Player Data unlock sound failed"))return false;}
  if(pressed&0x102){state.change_substate(3);if(!check(host.sound(9),"Player Data close sound failed"))return false;for(i32 script:{ranks.cursor+163,state.menu.cursor+155,171,172,173,174,168,169,170,175})if(!visual(visuals.retire(script)))return false;for(i32 row=0;row<10;row++)if(!visual(visuals.interrupt(204+row,1)))return false;}break;
 case 3:if(state.age.current>5){if(!visual(visuals.retire(102))||!visual(visuals.retire(203)))return false;state.change_screen(TitleScreen::Main);state.menu.pop();}break;
 }
 return true;
}
bool TitlePlayerData::draw(HudDrawServices& sink,ReplayCalendarServices& dates){
 if(!error.empty())return false;if(state.substate!=2)return true;const i32 character=state.menu.cursor/2,variant=state.menu.cursor%2,rank=ranks.cursor;if(character<0||character>=4||rank<0||rank>=5)return check(false,"Player Data score selection outside original range");const auto& bank=records.characters[character].modes[variant];HudTextDraw text;text.style.font=0;text.style.shadow=true;auto emit=[&](){return check(sink.hud_text(text),"Player Data ASCII text failed");};char line[256];
 if(pages.cursor==0){for(i32 i=0;i<10;i++){const auto& row=bank.scores[rank][i];const u32 shade=255-i*16;text.position={48,float(160+i*18),0};text.style.color=variant?(0xff0000ff|(shade<<16)|(shade<<8)):(0xffffff00|shade);const auto name=short_name(row.name.data());
  if(row.details[0]||row.details[1]){ReplayCalendar date;const i64 timestamp=i64(u64(u32(row.details[0]))|(u64(u32(row.details[1]))<<32));if(!check(dates.calendar(timestamp,date),"Player Data score date unavailable"))return false;float slowdown;std::memcpy(&slowdown,&row.details[2],4);const auto percent=legacy_decimal_tenth(slowdown);if(variant){const i32 stage=i8(row.stage);if(stage<0||stage>=10)return check(false,"Player Data stage label outside original range");std::snprintf(line,sizeof line,"%2d  %s  %9ld%d  %.4d/%.2d/%.2d %.2d:%.2d  %s  %s%%",i+1,name.c_str(),long(row.score),i32(row.continues),date.year,date.month,date.day,date.hour,date.minute,stages[stage],percent.c_str());}
  else std::snprintf(line,sizeof line,"%2d  %s  %9ld%d  %.4d/%.2d/%.2d %.2d:%.2d  Retry %3d  %s%%",i+1,name.c_str(),long(row.score),i32(row.continues),date.year,date.month,date.day,date.hour,date.minute,row.details[3],percent.c_str());}
  else if(variant)std::snprintf(line,sizeof line,"%2d  %s  %9ld%d  ----/--/-- --:--  Stage -  ---%%",i+1,name.c_str(),long(row.score),i32(row.continues));else std::snprintf(line,sizeof line,"%2d  %s  %9ld%d  ----/--/-- --:--  Retry   0  ---%%",i+1,name.c_str(),long(row.score),i32(row.continues));text.text=line;if(!emit())return false;
 }}
 text.style.color=0xffffffff;text.position={328,378,0};std::snprintf(line,sizeof line,"    %5d",bank.plays);text.text=line;if(!emit())return false;const u64 seconds=bank.play_centiseconds/100;text.position.y=396;std::snprintf(line,sizeof line,"%3lld:%.2lld:%.2lld",static_cast<long long>(seconds/3600),static_cast<long long>((seconds/60)%60),static_cast<long long>(seconds%60));text.text=line;if(!emit())return false;text.position.y=414;std::snprintf(line,sizeof line,"    %5d",bank.clears[rank]);text.text=line;return emit();
}
}
