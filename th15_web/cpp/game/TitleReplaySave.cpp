#include "TitleReplaySave.hpp"
#include "LegacyTextFormat.hpp"
#include <cstdio>
namespace th15 {
namespace {
constexpr char alphabet[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-=.,!?@:;[]()_/{}|~^#$%&*   ";
constexpr const char* characters[]={"Reimu  ","Marisa ","Sanae  ","Reisen "};constexpr const char* ranks[]={"Easy   ","Normal ","Hard   ","Lunatic","Extra  "};constexpr const char* completed[]={"tst","St1","St2","St3","St4","St5","St6","Ex ","All","ExA"};
u32 word(const u8* p){u32 value;std::memcpy(&value,p,4);return value;}
std::string name_string(const char* p){u32 n=0;while(n<8&&p[n])n++;return {p,n};}
}
bool TitleReplaySave::check(bool ok,const char* why){if(!ok&&error.empty())error=why;return ok;}
bool TitleReplaySave::visual(bool ok){return check(ok,visuals.error.c_str());}
bool TitleReplaySave::prepare_name(){
 if(records.name[8])return check(false,"Replay stored name exceeds eight bytes");auto& grid=editor.names;grid.select(0);grid.count=91;grid.wrapping=true;for(u32 n=0;n<9;n++){editor.name[n]=records.name[n];if(!editor.name[n])break;}constexpr std::array<char,9> empty{' ',' ',' ',' ',' ',' ',' ',' ',0};if(editor.name!=empty)grid.move(-1);editor.name_length=8;while(editor.name_length>0&&editor.name[editor.name_length-1]==' ')editor.name_length--;return true;
}
bool TitleReplaySave::append(char value){if(editor.name_length<0||editor.name_length>8)return check(false,"Replay name length outside original range");if(editor.name_length<8){editor.name[editor.name_length++]=value;if(editor.name_length>=8)editor.names.select(90);}else editor.name[editor.name_length-1]=value;return true;}
bool TitleReplaySave::update(u32 pressed,u32 repeated){
 if(!error.empty())return false;auto& grid=editor.names;
 // Completed assisted runs return through the existing cancel/cleanup path.
 // Also handles assistance enabled while the slot or name editor is open.
 if(!host.replay_save_available()&&state.substate!=4)state.change_substate(4);
 switch(state.substate){
 case 0:
  state.menu.count=25;state.menu.wrapping=true;state.menu.select(0);progress.stage=progress.starting_stage=8;for(i32 i=0;i<25;i++)if(!check(host.read_slot(i,files[i]),"Completed replay slot read failed"))return false;
  if(!animations.registry.find(state.handles[89])&&(!visual(visuals.create(89))||!visual(visuals.interrupt(89,3,true))))return false;
  if(!visual(visuals.create(105)))return false;state.change_substate(1);[[fallthrough]];
 case 1:if(state.age.current>6)state.change_substate(2);break;
 case 2:
  state.menu.previous=state.menu.cursor;if((pressed|repeated)&16)state.menu.move(-1);if((pressed|repeated)&32)state.menu.move(1);if(!state.menu.error.empty())return check(false,state.menu.error.c_str());if(state.menu.previous!=state.menu.cursor&&!check(host.sound(10),"Replay slot cursor sound failed"))return false;
  if(pressed&0x102){state.change_substate(4);return check(host.sound(9),"Replay save cancel sound failed");}
  if(pressed&0x80001){selected=state.menu.cursor;if(!check(host.prepare_live_replay(true),"Completed replay metadata preparation failed")||!prepare_name()||!check(host.sound(7),"Replay name entry sound failed"))return false;state.change_substate(3);}break;
 case 3:
  grid.previous=grid.cursor;if((pressed|repeated)&16)grid.move(-13);if((pressed|repeated)&32)grid.move(13);if((pressed|repeated)&64)grid.move(grid.cursor%13==0?12:-1);if((pressed|repeated)&128)grid.move(grid.cursor%13==12?-12:1);if(!grid.error.empty())return check(false,grid.error.c_str());if(grid.previous!=grid.cursor&&!check(host.sound(10),"Replay name cursor sound failed"))return false;
  if(pressed&0x80001){
   if(grid.cursor<0||grid.cursor>=91)return check(false,"Replay name grid cursor outside original range");
   if(grid.cursor<88){if(!append(alphabet[grid.cursor]))return false;}
   else if(grid.cursor==88){if(!append(' '))return false;}
   else if(grid.cursor==89){if(editor.name_length==0)return true;editor.name[--editor.name_length]=' ';}
   else {
    if(!check(host.sound(17),"Replay write sound failed"))return false;files[state.menu.cursor].reset();if(!check(host.save_slot(state.menu.cursor,editor.name),"Completed replay write failed")||!check(host.read_slot(state.menu.cursor,files[state.menu.cursor]),"Completed replay reload failed"))return false;
    for(u32 n=0;n<9;n++){records.name[n]=editor.name[n];if(!editor.name[n])break;}state.change_substate(2);
   }
   if(!check(host.sound(7),"Replay name confirm sound failed"))return false;
  }
  if(pressed&0x102){if(editor.name_length){if(!check(host.sound(9),"Replay name delete sound failed"))return false;editor.name[--editor.name_length]=' ';}else state.change_substate(2);}break;
 case 4:
  if(state.age.current>5){if(!visual(visuals.retire(105))||!visual(visuals.retire(203)))return false;state.change_screen(TitleScreen::Main);state.menu.pop();if(!check(host.release_live_replay(),"Completed replay session release failed")||!check(host.music("th15_01",0),"Replay save title music failed"))return false;files={};}break;
 }
 return true;
}
bool TitleReplaySave::caption(i32 slot,const u8* metadata,bool live,ReplayCalendarServices& dates,HudTextDraw& text){
 i64 timestamp;std::memcpy(&timestamp,metadata+0xc,8);ReplayCalendar date;if(!check(dates.calendar(timestamp,date),"Replay save date unavailable"))return false;const u32 character=word(metadata+0x8c)+word(metadata+0x90),rank=word(metadata+0x94),finish=live?8:word(metadata+0x98);if(character>=4||rank>=5||finish>=10)return check(false,"Replay save caption selection outside original range");float slowdown;std::memcpy(&slowdown,metadata+0x84,4);const auto percent=legacy_decimal_tenth(slowdown),name=live?std::string(8,' '):name_string(reinterpret_cast<const char*>(metadata));char line[256];std::snprintf(line,sizeof line,"No.%.2d %s %.2d/%.2d/%.2d %.2d:%.2d %s %s %s %s%%",slot+1,name.c_str(),(date.year-1900)%100,date.month,date.day,date.hour,date.minute,characters[character],ranks[rank],completed[finish],percent.c_str());text.text=line;return true;
}
bool TitleReplaySave::draw(HudDrawServices& sink,ReplayCalendarServices& dates,const std::array<u8,0xa4>* live){
 if(!error.empty())return false;HudTextDraw text;auto emit=[&](){return check(sink.hud_text(text),"Replay save text submission failed");};
 if(state.substate==2){text.style.font=0;text.style.shadow=true;for(i32 row=0;row<25;row++){text.position={58,float(80+row*15),0};text.style.color=state.menu.cursor==row?0xffffff00:0xff808080;if(files[row]){const auto& bytes=files[row]->decoded();if(bytes.size()<0xa4||!caption(row,bytes.data(),false,dates,text))return check(false,"Replay slot description unavailable");}else {char line[128];std::snprintf(line,sizeof line,"No.%.2d -------- --/--/-- --:-- ------- ------- --- ---%%",row+1);text.text=line;}if(!emit())return false;}return true;}
 if(state.substate!=3)return true;if(!live)return check(false,"Live completed replay unavailable");text.position={58,240,0};if(state.age.current<10){const float origin=float(selected*15+80);text.position.y=float(float(float(float(10.f-state.age.fractional)*float(origin-240.f))/10.f)+240.f);}if(!caption(selected,live->data(),true,dates,text)||!emit())return false;
 if(state.age.current>=10){text.position={112,240,0};text.text=name_string(editor.name.data());if(!emit())return false;text.style.color=0xffffff00;text.position.x=float(112+editor.name_length*9-(editor.name_length==8?9:0));text.text="_";if(!emit())return false;
  for(i32 i=0;i<91;i++){text.position={float(212+(i%13)*18),float(360+(i/13)*16),0};text.style.color=editor.names.cursor==i?0xffffff00:0xff808080;text.text.assign(1,i<88?alphabet[i]:char(i==88?0x81:i==89?0x7f:0x80));if(!emit())return false;}
 }return true;
}
}
