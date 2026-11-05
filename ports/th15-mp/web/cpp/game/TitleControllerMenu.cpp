#include "TitleControllerMenu.hpp"
namespace th15 {
bool TitleControllerMenu::check(bool ok){if(!ok&&error.empty())error=visuals.error.empty()?animations.error:visuals.error;return ok;}
bool TitleControllerMenu::sound(i32 id){if(services.sound(id))return true;error="Controller menu sound request failed";return false;}
void TitleControllerMenu::restore(){for(u32 i=0;i<4;i++)pending[i]=settings.values[i];}
bool TitleControllerMenu::refresh(){
 auto* root=animations.registry.find(state.handles[2]);if(!root){error="Controller menu root unavailable";return false;}
 for(i32 i=0;i<4;i++)for(i32 digit=0;digit<2;digit++)for(i32 style=0;style<2;style++){
  auto* child=animations.registry.find_child_script(*root,59+i*2+digit+style*8,0);if(!child){error="Controller menu digit unavailable";return false;}
  const i32 index=38+(digit?i32(pending[i])%10:i32(pending[i])/10);if(!check(child->select_sprite(index)))return false;
 }return true;
}
bool TitleControllerMenu::paint(){
 auto row=[&](i32 index,i32 label){if(!check(visuals.child_interrupt(2,index+47,label,true))||!check(visuals.child_interrupt(2,index+53,label,true)))return false;
  if(index<4)for(i32 script:{59+index*2,60+index*2,67+index*2,68+index*2})if(!check(visuals.child_interrupt(2,script,label,true)))return false;return true;};
 for(i32 i=0;i<state.menu.cursor;i++)if(!row(i,30))return false;
 for(i32 i=state.menu.cursor+1;i<6;i++)if(!row(i,31))return false;return true;
}
bool TitleControllerMenu::bind(i32 action,i32 button){
 if(action<0||action>=6||button<0||button>=31){error="Controller binding out of range";return false;}const i16 old=pending[action];if(old==button)return true;
 for(i32 i=0;i<6;i++)if(i!=action&&pending[i]==button)pending[i]=old;pending[action]=i16(button);
 return refresh()&&sound(7);
}
bool TitleControllerMenu::close(){if(!sound(9)||!check(visuals.interrupt(2,6)))return false;state.change_substate(4);return true;}
bool TitleControllerMenu::update(u32 pressed,u32 repeated,u32 gamepad_buttons){
 switch(state.substate){
 case 0:
  state.menu.count=6;state.menu.select(0);if(!check(visuals.create(2)))return false;state.change_substate(1);restore();if(!refresh())return false;[[fallthrough]];
 case 1:
  if(state.age.current>6){state.change_substate(2);if(!check(visuals.interrupt(2,3,true))||!check(visuals.interrupt(2,i32(i16(state.menu.cursor))+17))||!paint())return false;}break;
 case 2:
  state.menu.previous=state.menu.cursor;if((pressed|repeated)&16)state.menu.move(-1);if((pressed|repeated)&32)state.menu.move(1);
  if(!state.menu.error.empty()){error=state.menu.error;return false;}
  if(state.menu.previous!=state.menu.cursor&&(!sound(10)||!check(visuals.interrupt(2,3,true))||!check(visuals.interrupt(2,i32(i16(state.menu.cursor))+7))||!paint()))return false;
  for(i32 button=0;button<31;button++)if(gamepad_buttons&(1u<<button)){if(state.menu.cursor<=3&&!bind(state.menu.cursor,button))return false;break;}
  if((pressed&0x102)&&state.menu.cursor==5){restore();return refresh()&&close();}
  if(pressed&0x80001){if(state.menu.cursor==4){restore();return refresh()&&sound(7);}if(state.menu.cursor==5){for(u32 i=0;i<4;i++)settings.values[i]=pending[i];if(!services.save(settings)){error="Controller configuration save failed";return false;}return close();}}break;
 case 4:if(state.age.current>=10){state.change_screen(TitleScreen::Options);state.menu.pop();}break;
 }
 return true;
}
}
