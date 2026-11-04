#include "TitlePlayerData.hpp"
#include "SpellCatalog.hpp"
#include "LegacyTextFormat.hpp"
#include <cstdio>
namespace th17 {
namespace {
constexpr const char* stages[]={"test   ","Stage 1","Stage 2","Stage 3","Stage 4","Stage 5","Stage 6","Extra  ","Clear  ","ExClear"};
std::string short_name(const char* p){u32 n=0;while(n<8&&p[n])n++;return {p,n};}
std::string full_digit(i32 n){return {char(0x82),char(0x4f+n)};}
std::string full_space(){return {char(0x81),char(0x40)};}
}
bool TitlePlayerData::check(bool ok,const char* why){if(!ok&&error.empty())error=why;return ok;}
bool TitlePlayerData::visual(bool ok){return check(ok,visuals.error.c_str());}
bool TitlePlayerData::bitmap(i32 row,const std::string& bytes,u32 color){auto* vm=animations.registry.find(state.handles[291+row]);if(!vm){state.handles[291+row]=0;return check(false,"Player Data text animation unavailable");}TextureText request;request.bytes=bytes;request.color=color;request.centered=true;return check(host.text(*vm,request),"Player Data bitmap text failed");}
bool TitlePlayerData::paint(){
 if(ranks.selected<0||ranks.selected>=5||state.menu.selected<0||state.menu.selected>=9||pages.selected<0)return check(false,"Player Data spell page outside original range");
 i32 skipped=0,index=0,first=pages.selected*10;while(skipped<first&&index<101){if(SpellCatalog::rank(index)==ranks.selected)skipped++;index++;}displayed_spells=0;
 while(index<101&&displayed_spells<10){if(SpellCatalog::rank(index)!=ranks.selected){index++;continue;}
  const auto& aggregate=records.spells.aggregate[index];const auto& personal=records.spells.loadouts[state.menu.selected][index];const i32 number=index+1;
  const std::string digits=(number>=100?full_digit(number/100):full_space())+(number>=10?full_digit(number/10%10):full_space())+full_digit(number%10);
  std::string label;u32 color;
  if(!aggregate.attempts[0]){for(i32 n=0;n<21;n++){label+=char(0x81);label+=char(0x48);}color=0x808080;}
  else {label=aggregate.name;while(label.size()<42)label+=full_space();color=personal.captures[0]?0xffff80:0xefefef;}
  char tail[64];std::snprintf(tail,sizeof tail," %4d/%4d",personal.captures[0],personal.attempts[0]);if(!bitmap(displayed_spells,"No."+digits+" "+label+tail,color))return false;displayed_spells++;index++;
 }
 for(i32 row=displayed_spells;row<10;row++)if(!bitmap(row," ",0xffffffff))return false;return true;
}
bool TitlePlayerData::entry(u32 pressed,u32 repeated){
 switch(state.substate){
 case 0:if(!visual(visuals.prompt()))return false;state.menu.count=3;state.menu.wrap=true;if(!visual(visuals.retire(201))||!visual(visuals.create(201))||!visual(visuals.interrupt(201,3,true))||!visual(visuals.interrupt(201,state.menu.selected+7))||!visual(visuals.create(104)))return false;state.change_substate(1);[[fallthrough]];
 case 1:if(state.age.current>6)state.change_substate(2);break;
 case 2:{state.menu.reserved=state.menu.selected;const u32 input=pressed|repeated;if(input&(16|64))state.menu.step(-1);if(input&(32|128))state.menu.step(1);
  if(state.menu.reserved!=state.menu.selected&&(!check(host.sound(10),"Player Data navigation sound failed")||!visual(visuals.interrupt(201,3,true))||!visual(visuals.interrupt(201,state.menu.selected+7))))return false;
  if(pressed&0x102){state.change_substate(4);if(!check(host.sound(9),"Player Data cancel sound failed")||!visual(visuals.retire(201)))return false;return true;}
  if(pressed&0x80001){if(!visual(visuals.interrupt(201,6))||!visual(visuals.child_interrupt(201,game_difficulty<4?106+state.menu.selected:110,2)))return false;state.change_substate(3);return check(host.sound(7),"Player Data confirm sound failed");}break;}
 case 3:if(state.age.current>13){if(!visual(visuals.retire(104)))return false;state.change_screen(state.menu.selected==2?TitleScreen::Trophies:TitleScreen::PlayerDataDetails);state.menu.push();}break;
 case 4:if(state.age.current>5){if(!visual(visuals.retire(104))||!visual(visuals.retire(TitleAnimations::prompt_slot)))return false;state.change_screen(TitleScreen::Main);state.menu.pop();}break;
 }return true;
}
bool TitlePlayerData::details(u32 pressed,u32 repeated){
 const u32 input=pressed|repeated;
 switch(state.substate){
 case 0:
  kind=state.menu.selected;if(kind==1){pages.count=(SpellCatalog::rank_count(ranks.selected)+9)/10;pages.select(0);pages.wrap=true;}
  state.menu.count=9;state.menu.select(0);ranks.count=5;ranks.select(1);ranks.wrap=true;
  if(kind==1){for(i32 row=0;row<10;row++){state.handles[291+row]=animations.create(text_bank,row+3,-1,0);if(!check(state.handles[291+row]!=0,"Player Data text row creation failed"))return false;}if(!paint())return false;}
  state.change_substate(1);if(!visual(visuals.create(state.menu.selected+148))||!visual(visuals.create(ranks.selected+157)))return false;
  for(i32 script:{165,166,167,168,162,163,164,98})if(!visual(visuals.create(script)))return false;[[fallthrough]];
 case 1:if(state.age.current>6)state.change_substate(2);break;
 case 2:
  state.menu.reserved=state.menu.selected;ranks.reserved=ranks.selected;pages.reserved=pages.selected;
  if(input&16){ranks.step(-1);if(!visual(visuals.interrupt(167,2,true)))return false;}if(input&32){ranks.step(1);if(!visual(visuals.interrupt(168,2,true)))return false;}
  if(ranks.reserved!=ranks.selected){if(!check(host.sound(10),"Player Data rank sound failed")||!visual(visuals.retire(ranks.reserved+157))||!visual(visuals.create(ranks.selected+157)))return false;if(kind==1){pages.select(0);if(!paint())return false;}pages.count=(SpellCatalog::rank_count(ranks.selected)+9)/10;}
  if(input&64){state.menu.step(-1);if(!visual(visuals.interrupt(165,2,true)))return false;}if(input&128){state.menu.step(1);if(!visual(visuals.interrupt(166,2,true)))return false;}
  if(state.menu.reserved!=state.menu.selected){if(!check(host.sound(10),"Player Data loadout sound failed")||!visual(visuals.retire(state.menu.reserved+148))||!visual(visuals.create(state.menu.selected+148)))return false;if(kind==1&&!paint())return false;}
  if(kind==1&&(pressed&0x80001)){pages.step(1);if(!paint()||!check(host.sound(7),"Player Data page sound failed"))return false;}
  if(ranks.selected==3&&state.menu.selected==7){TitleKeyboard keys;bool unlocked=false;if(!check(host.keyboard(keys),"Player Data keyboard unavailable")||!check(cheat.update(pressed,keys,unlocked),cheat.error.c_str()))return false;if(unlocked&&!check(host.sound(17),"Player Data unlock sound failed"))return false;}
  if(pressed&0x102){state.change_substate(3);if(!check(host.sound(9),"Player Data close sound failed"))return false;for(i32 script:{ranks.selected+157,state.menu.selected+148,165,166,167,168,162,163,164,169})if(!visual(visuals.retire(script)))return false;for(i32 row=0;row<10;row++)if(!visual(visuals.interrupt(291+row,1)))return false;}break;
 case 3:if(state.age.current>5){if(!visual(visuals.retire(98)))return false;state.change_screen(TitleScreen::PlayerData);state.menu.pop();}break;
 }return true;
}
bool TitlePlayerData::update(u32 pressed,u32 repeated,i32 difficulty){if(!error.empty())return false;game_difficulty=difficulty;return state.screen==TitleScreen::PlayerData?entry(pressed,repeated):details(pressed,repeated);}
bool TitlePlayerData::draw(HudDrawServices& sink,ReplayCalendarServices& dates){
 if(!error.empty())return false;if(state.screen!=TitleScreen::PlayerDataDetails||state.substate!=2)return true;
 const i32 loadout=state.menu.selected,rank=ranks.selected;if(loadout<0||loadout>=9||rank<0||rank>=5)return check(false,"Player Data score selection outside original range");
 HudTextDraw text;text.style.font=0;text.style.shadow=true;auto emit=[&](){return check(sink.hud_text(text),"Player Data ASCII text failed");};char line[256];
 if(kind==0&&pages.selected==0){for(i32 i=0;i<10;i++){
  const auto& row=records.scores[loadout][rank][i];const u32 shade=255-i*16;text.position={48,float(160+i*18),0};text.style.color=0xffffff00|shade;const auto name=short_name(row.name.data());
  if(row.details[0]||row.details[1]){ReplayCalendar date;const i64 stamp=i64(u64(u32(row.details[0]))|(u64(u32(row.details[1]))<<32));if(!check(dates.calendar(stamp,date),"Player Data calendar unavailable"))return false;float slowdown;std::memcpy(&slowdown,&row.details[2],4);const auto percent=legacy_decimal_tenth(slowdown);const i32 stage=i8(row.stage);if(stage<0||stage>=10)return check(false,"Player Data stage label outside original range");std::snprintf(line,sizeof line,"%2d  %s  %9ld%d  %.4d/%.2d/%.2d %.2d:%.2d  %s  %s%%",i+1,name.c_str(),long(row.score),i32(row.continues),date.year,date.month,date.day,date.hour,date.minute,stages[stage],percent.c_str());}
  else std::snprintf(line,sizeof line,"%2d  %s  %9ld%d  ----/--/-- --:--  Stage -  ---%%",i+1,name.c_str(),long(row.score),i32(row.continues));text.text=line;if(!emit())return false;
 }}
 text.style.color=0xffffffff;text.position={328,378,0};std::snprintf(line,sizeof line,"    %5d",records.game.starts[loadout]);text.text=line;if(!emit())return false;
 const u64 seconds=records.play_centiseconds[loadout]/100;text.position.y=396;std::snprintf(line,sizeof line,"%3lld:%.2lld:%.2lld",static_cast<long long>(seconds/3600),static_cast<long long>(seconds/60%60),static_cast<long long>(seconds%60));text.text=line;if(!emit())return false;
 text.position.y=414;std::snprintf(line,sizeof line,"    %5d",records.game.clears[loadout][rank]);text.text=line;return emit();
}
}
