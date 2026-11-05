#include "TitleOptionsMenu.hpp"
#include <algorithm>
namespace th15 {
bool TitleOptionsMenu::check(bool ok){if(!ok&&error.empty())error=visuals.error.empty()?animations.error:visuals.error;return ok;}
AnmVm* TitleOptionsMenu::child(i32 script){auto* root=animations.registry.find(state.handles[1]);auto* vm=root?animations.registry.find_child_script(*root,script,0):nullptr;if(!vm&&error.empty())error="Options animation child unavailable";return vm;}
bool TitleOptionsMenu::visibility(i32 script,bool visible){auto* vm=child(script);if(!vm)return false;vm->visual.flags=visible?vm->visual.flags|2:vm->visual.flags&~2u;return true;}
bool TitleOptionsMenu::sprite(i32 script,i32 index){auto* vm=child(script);return vm&&check(vm->select_sprite(index));}
bool TitleOptionsMenu::sound(i32 id){if(services.sound(id))return true;error="Options sound request failed";return false;}
bool TitleOptionsMenu::paint(){
 const i32 selected=state.menu.cursor;
 for(i32 i=0;i<selected;i++)if(!check(visuals.child_interrupt(1,i+21,30,true))||!check(visuals.child_interrupt(1,i+26,30,true)))return false;
 for(i32 i=selected+1;i<5;i++)if(!check(visuals.child_interrupt(1,i+21,31,true))||!check(visuals.child_interrupt(1,i+26,31,true)))return false;
 if(selected>0)for(i32 i=31;i<39;i++)if(!check(visuals.child_interrupt(1,i,30,true)))return false;
 if(selected!=1)for(i32 i=39;i<47;i++)if(!check(visuals.child_interrupt(1,i,selected>1?30:31,true)))return false;
 return true;
}
bool TitleOptionsMenu::refresh(){
 // The original volume service derives the SE attenuation from the music
 // setting, with a separate mute test on the SE setting. Retain that curve.
 float t=float(1.f-float(settings.music_volume)/100.f);t=float(t*t);t=float(t*t);
 const float attenuation=float(float(1.f-t)*-5000.f);
 const i32 se=settings.sound_volume?wrapping_add(-5000,-truncate_int(attenuation)):-10000;
 if(!services.volume(settings.music_volume,settings.sound_volume,se)){error="Options volume request failed";return false;}
 for(i32 row=0;row<2;row++){
  const i32 value=row?settings.sound_volume:settings.music_volume,base=31+row*8;
  const i32 digits[3]={value/100,(value/10)%10,value%10};
  for(i32 i=0;i<3;i++)if(!sprite(base+i,38+digits[i]))return false;
  for(i32 i=0;i<3;i++)if(!sprite(base+4+i,49+digits[i]))return false;
  if(!visibility(base,value>=100)||!visibility(base+1,value>=10)||!visibility(base+4,value>=100)||!visibility(base+5,value>=10))return false;
 }
 return true;
}
bool TitleOptionsMenu::update(u32 pressed,u32 repeated){
 switch(state.substate){
 case 0:
  state.menu.count=5;state.menu.select(0);
  if(!check(visuals.create(1))||!refresh())return false;
  state.change_substate(1);[[fallthrough]];
 case 1:
  if(state.age.current>6){state.change_substate(2);if(!check(visuals.interrupt(1,3,true))||!check(visuals.interrupt(1,i32(i16(state.menu.cursor))+17))||!paint())return false;}
  break;
 case 2:{
  state.menu.previous=state.menu.cursor;
  if((pressed|repeated)&16)state.menu.move(-1);
  if((pressed|repeated)&32)state.menu.move(1);
  if(!state.menu.error.empty()){error=state.menu.error;return false;}
  if(state.menu.previous!=state.menu.cursor&&(!sound(10)||!check(visuals.interrupt(1,3,true))||!check(visuals.interrupt(1,i32(i16(state.menu.cursor))+7))||!paint()))return false;
  if(pressed&0x102){
   if(state.menu.cursor==4){if(!check(visuals.interrupt(1,6))||!sound(9))return false;state.change_substate(4);return true;}
   if(!sound(9))return false;state.menu.select(4);
   return check(visuals.interrupt(1,3,true))&&check(visuals.interrupt(1,i32(i16(state.menu.cursor))+7))&&paint();
  }
  if(state.menu.cursor==1&&state.age.current!=state.age.previous&&state.age.current%60==0&&!sound(2))return false;
  if((pressed|repeated)&64){if(state.menu.cursor<2){auto& value=state.menu.cursor?settings.sound_volume:settings.music_volume;value=std::max(0,value-5);if(!refresh())return false;}}
  if((pressed|repeated)&128){if(state.menu.cursor<2){auto& value=state.menu.cursor?settings.sound_volume:settings.music_volume;value=std::min(100,value+5);if(!refresh())return false;}}
  if(pressed&0x80001){switch(state.menu.cursor){
   case 2:if(!check(visuals.interrupt(1,6))||!sound(7))return false;state.change_substate(4);break;
   case 3:settings.music_volume=100;settings.sound_volume=80;settings.controller_option=0;return refresh()&&sound(7);
   case 4:if(!check(visuals.interrupt(1,6))||!sound(9))return false;state.change_substate(4);break;
  }}break;}
 case 4:
  if(state.age.current>=10){if(state.menu.cursor==4){state.change_screen(TitleScreen::Main);state.menu.pop();}else if(state.menu.cursor==2){state.change_screen(TitleScreen::Controller);state.menu.push();}}
  break;
 }
 return true;
}
}
