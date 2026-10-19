#include "TitleMusicRoom.hpp"
#include "Localization.hpp"
#include <cmath>
#include <cstdio>
namespace th15 {
bool MusicComments::open(const u8* bytes,u32 size){
 entries.clear();error.clear();if(!bytes){error="Music comments unavailable";return false;}u32 offset=0;
 auto skip=[&](){while(offset<size&&bytes[offset]!=10&&bytes[offset]!=13)offset++;while(offset<size&&(bytes[offset]==10||bytes[offset]==13))offset++;};
 auto line=[&](std::string& target,u32 maximum){const u32 start=offset;while(offset<size&&bytes[offset]!=10&&bytes[offset]!=13)offset++;if(offset==size||offset-start>=maximum)return false;target.assign(reinterpret_cast<const char*>(bytes+start),offset-start);while(offset<size&&(bytes[offset]==10||bytes[offset]==13))offset++;return true;};
 while(offset<size){if(bytes[offset]!=64){skip();continue;}if(entries.size()==20){error="Music comment entry capacity exceeded";return false;}offset++;MusicComment entry;if(!line(entry.file,64)||!line(entry.title,66)){error="Invalid music comment heading";return false;}for(auto& text:entry.comments)if(!line(text,66)){error="Invalid music comment line";return false;}entries.push_back(std::move(entry));}
 if(entries.empty()){error="Empty music comment catalog";return false;}
 // BP_music_cmt maps a literal @ to Music Room Note Title, not a skipped row.
 // Its eight patch rows correspond to the eight retail comment slots.
 for(u32 i=0;i<entries.size();i++){auto& entry=entries[i];entry.title=Localization::MusicTitle(i+1,entry.title.c_str());for(u16 line=0;line<entry.comments.size();line++){
  entry.comments[line]=Localization::MusicComment(i+1,line,entry.comments[line].c_str());
  if(entry.comments[line]=="@"){
   const char* format=Localization::LayoutFormatStringById("Music Room Note Title","%d %s");char title[4096];
   const int length=std::snprintf(title,sizeof title,format,int(i+1),entry.title.c_str());
   if(length<0||length>=int(sizeof title)){error="Music note title too long";return false;}entry.comments[line]=title;
  }
 }}
 return true;
}
bool TitleMusicRoom::check(bool ok){if(!ok&&error.empty())error=visuals.error.empty()?animations.error:visuals.error;return ok;}
bool TitleMusicRoom::interrupt(u32 handle,i32 label){return check(animations.interrupt(handle,label));}
bool TitleMusicRoom::text(u32 handle,const std::string& bytes,u32 color,bool detail){auto* vm=animations.registry.find(handle);if(!vm){error="Music text animation unavailable";return false;}DialogueText request;request.handle=handle;request.bytes=bytes;request.color=color;request.music_detail=detail;return services.text(*vm,request)||check(false);}
bool TitleMusicRoom::create_entries(){
 const i32 start=2*state.age.current-2,end=2*state.age.current;float y=float(float(float(96.f-float(float(scroll)*20.f))+float(state.age.current*40-40))*2.f);
 for(i32 i=start;i<end&&i<i32(catalog.count());i++){
  if(i<0){error="Music Room entered before its first frame";return false;}auto& handle=state.handles[220+i];handle=animations.create(title_bank,176+i,-1,0);auto* vm=animations.registry.find(handle);if(!vm)return check(false);
  std::string caption=catalog.entry(i)->title;if(!unlocked[i]){char number[16];std::snprintf(number,sizeof number,"No.%2d  ",i+1);caption=number;for(i32 j=0;j<11;j++){caption+=char(0x81);caption+=char(0x48);}}
  const char* plain=unlocked[i]?"No.%2d  %s":"No.%2d  ?";
  const char* format=Localization::LayoutFormatStringById(unlocked[i]?"Music Room Numbered Title":"Music Room Unknown Title",plain);
  if(format!=plain){char localized[4096];const int length=unlocked[i]?std::snprintf(localized,sizeof localized,format,i+1,catalog.entry(i)->title.c_str()):std::snprintf(localized,sizeof localized,format,i+1);if(length<0||length>=i32(sizeof localized)){error="Localized Music Room row too long";return false;}caption=localized;}
  if(!text(handle,caption,0xffffff)||!check(animations.pause(handle,i<scroll||i>=scroll+10)))return false;
  const float x=state.menu.cursor==i?float(double(128.f)-8.):128.f;const Vec3 target{x,y,0};auto& p=vm->interpolators.position;p.control1={};p.control2={};p.begin(4,0,{vm->variables.position.x,vm->variables.position.y,vm->variables.position.z},{target.x,target.y,target.z});
  y=float(y+40.f);if(!interrupt(handle,state.menu.cursor==i?2:3))return false;
 }
 return true;
}
bool TitleMusicRoom::position_entries(){
 float y=float(float(96.f-float(float(scroll)*20.f))*2.f);
 for(i32 i=0;i<i32(catalog.count());i++){
  auto& handle=state.handles[220+i];auto* vm=animations.registry.find(handle);if(!vm){handle=0;error="Music entry animation unavailable";return false;}
  if(!check(animations.pause(handle,i<scroll||i>=scroll+10)))return false;
  const Vec3 target{state.menu.cursor==i?float(double(128.f)-8.):128.f,y,0};
  if(float(std::abs(double(float(vm->variables.position.y-y))))<80.f){auto& p=vm->interpolators.position;p.control1={};p.control2={};p.begin(4,0,{vm->variables.position.x,vm->variables.position.y,vm->variables.position.z},{target.x,target.y,target.z});}
  else vm->variables.position=target;
  y=float(y+40.f);if(!interrupt(handle,state.menu.cursor==i?2:3))return false;
 }
 if(comment_line>=8)warning=false;return true;
}
namespace {
const std::array<std::string,8> music_warning={std::string("\x81\x40"),std::string("\x81\x40\x81\x40\x81\x96\x81\x96\x91\x49\x91\xf0\x82\xb5\x82\xbd\x8b\xc8\x82\xcd\x82\xdc\x82\xbe\x83\x51\x81\x5b\x83\x80\x92\x86\x82\xc5\x8d\xc4\x90\xb6\x82\xb3\x82\xea\x82\xc4\x82\xa2\x82\xdc\x82\xb9\x82\xf1\x81\x96\x81\x96"),std::string("\x20"),std::string("\x81\x40\x81\x40\x81\x40\x81\x40\x8b\xc8\x82\xcc\x83\x52\x83\x81\x83\x93\x83\x67\x82\xaa\x83\x6c\x83\x5e\x83\x6f\x83\x8c\x82\xc9\x82\xc8\x82\xe9\x8b\xb0\x82\xea\x82\xaa\x82\xa0\x82\xe8\x82\xdc\x82\xb7\x81\x42"),std::string("\x81\x40\x81\x40\x81\x40\x81\x40\x81\x40\x81\x40\x81\x40\x81\x40\x81\x40\x82\xbb\x82\xea\x82\xc5\x82\xe0\x8d\xc4\x90\xb6\x82\xb5\x82\xdc\x82\xb7\x82\xa9\x81\x48"),std::string("\x81\x40"),std::string("\x81\x40\x81\x40\x81\x40\x8d\xc4\x90\xb6\x82\xb5\x82\xbd\x82\xa2\x95\xfb\x82\xcd\x81\x41\x82\xe0\x82\xa4\x88\xea\x93\x78\x8c\x88\x92\xe8\x83\x7b\x83\x5e\x83\x93\x82\xf0\x89\x9f\x82\xb5\x82\xc4\x82\xad\x82\xbe\x82\xb3\x82\xa2\x81\x42"),std::string("\x81\x40\x81\x40\x81\x40\x8d\xc4\x90\xb6\x82\xb5\x82\xbd\x82\xad\x82\xc8\x82\xa2\x95\xfb\x82\xcd\x81\x41\x83\x4a\x81\x5b\x83\x5c\x83\x8b\x82\xf0\x88\xda\x93\xae\x82\xb5\x82\xc4\x82\xad\x82\xbe\x82\xb3\x82\xa2\x81\x42")};
}
bool TitleMusicRoom::paint_comment(){
 if(state.age.current%2||comment_line>=8)return true;if(comment_track<0||comment_track>=i32(catalog.count())){error="Music comment selection out of range";return false;}
 const bool locked=!unlocked[comment_track]&&warning;
 const std::string key="th10 Music Room spoiler "+std::to_string(comment_line==1?1:comment_line==3?2:comment_line==4?3:comment_line==6?4:comment_line==7?5:0);
 const std::string value=locked?Localization::StringById(key.c_str(),music_warning[comment_line].c_str()):catalog.entry(comment_track)->comments[comment_line];
 if(!text(state.handles[240+comment_line],value,locked?0x8080ff:0xffffff,comment_line>0)||!interrupt(state.handles[240+comment_line],2))return false;comment_line++;return true;
}
bool TitleMusicRoom::cancel(){
 for(i32 i=0;i<i32(catalog.count());i++)if(!interrupt(state.handles[220+i],1))return false;
 for(i32 i=0;i<8;i++)if(!interrupt(state.handles[240+i],1))return false;
 if(!services.sound(9)){error="Music menu sound request failed";return false;}state.change_substate(3);return true;
}
bool TitleMusicRoom::update(u32 pressed,u32 repeated){
 switch(state.substate){
 case 0:
  if(state.age.current==1){
   state.menu.count=6;state.menu.select(0);
   if(!check(visuals.prompt())||!check(visuals.create(103)))return false;
   if(!catalog.count())return cancel();state.menu.count=catalog.count();state.menu.select(0);scroll=0;
   for(i32 i=0;i<8;i++){state.handles[240+i]=animations.create(text_bank,19+i,-1,0);if(!state.handles[240+i])return check(false);}
   comment_line=comment_track=0;warning=false;
  }
  if(state.age.current<10&&!create_entries())return false;
  if(state.age.current>=10)state.change_substate(1);
  break;
 case 1:
  if(!paint_comment())return false;if(state.age.current>4)state.change_substate(2);break;
 case 2:{
  if(!paint_comment())return false;state.menu.previous=state.menu.cursor;
  if((pressed|repeated)&16)state.menu.move(-1);if((pressed|repeated)&32)state.menu.move(1);
  if(!state.menu.error.empty()){error=state.menu.error;return false;}
  if(state.menu.previous!=state.menu.cursor){if(!services.sound(10)){error="Music menu sound request failed";return false;}if(state.menu.cursor<scroll)scroll=state.menu.cursor;else if(state.menu.cursor>=scroll+10)scroll=state.menu.cursor-9;if(!position_entries())return false;}
  if(pressed&0x80001){
   for(i32 i=0;i<8;i++)if(!interrupt(state.handles[240+i],3))return false;
   comment_track=state.menu.cursor;comment_line=0;state.age.set(0);
   if(!unlocked[comment_track]&&!warning){if(!services.music_command(alternate_audio?4:3)){error="Music warning fade failed";return false;}warning=true;return true;}
   if(!services.music(catalog.entry(state.menu.cursor)->file)||(alternate_audio&&!services.music_command(4))||!services.music_command(2)){error="Music selection request failed";return false;}unlocked[0]=true;warning=false;return true;
  }
  if(pressed&0x102)return cancel();break;}
 case 3:
  if(state.age.current>=10){if(!check(visuals.retire(103))||!check(visuals.retire(203)))return false;state.change_screen(TitleScreen::Main);if(!services.music("th15_01")||!services.start_title_music()){error="Title music restoration failed";return false;}state.menu.pop();}
  break;
 }
 return true;
}
}
