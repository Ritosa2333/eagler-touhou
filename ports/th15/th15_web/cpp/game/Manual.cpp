#include "Manual.hpp"
namespace th15 {
bool Manual::fail(const std::string& reason){if(error.empty())error=reason.empty()?"Manual operation failed":reason;return false;}
bool Manual::sound(i32 id){return services.sound(id)||fail("Manual sound request failed");}
bool Manual::interrupt(u32 handle,i32 label,bool immediate){return animations.interrupt(handle,label,immediate)||fail(animations.error);}
bool Manual::create(i32 script,u32& handle){handle=animations.create(bank,script,-1,4,{offset_x,0,0});auto* vm=animations.registry.find(handle);if(!vm)return fail(animations.error);vm->visual.render_flags&=~0xc000u;return true;}
bool Manual::list(){for(i32 i=0;i<9;i++)if(!create(i,choices[i])||!interrupt(choices[i],menu.cursor==i?2:3,true))return false;return true;}
bool Manual::retire_list(){for(auto handle:choices)if(!interrupt(handle,1,false))return false;return true;}
bool Manual::request(){phase=2;age.set(0);return (services.request_page(menu.cursor)||fail("Manual page request failed"))&&retire_list();}
bool Manual::page_loaded(){if(phase!=2)return fail("Unexpected manual page completion");phase=3;return true;}
bool Manual::update(u32 pressed,u32 repeated,float rate){
 if(!error.empty())return false;
 if(mode==0)mode=1;
 else if(mode==2){if(age.current>=30)finished=true;}
 else if(mode==1){switch(phase){
 case 0:
  menu.count=9;menu.select(0);menu.wrapping=true;if(!list()||!services.clear_page())return fail("Manual page clear failed");phase=1;[[fallthrough]];
 case 1:
  if(age.current>=20){menu.previous=menu.cursor;if((pressed|repeated)&16)menu.move(-1);if((pressed|repeated)&32)menu.move(1);
   if(!menu.error.empty())return fail(menu.error);if(menu.previous!=menu.cursor){if(!sound(10))return false;for(i32 i=0;i<9;i++)if(!interrupt(choices[i],menu.cursor==i?2:3,true))return false;}
   if(pressed&0x80001){if(!sound(7)||!request())return false;}
   else if(pressed&0x102){if(!sound(9)||!retire_list())return false;mode=2;phase=0;age.set(0);}
  }break;
 case 2:break;
 case 3:
  if(!services.upload_page(menu.cursor)||!create(9,page))return fail("Manual page upload failed");phase=4;age.set(0);[[fallthrough]];
 case 4:
  if(age.current>=20){
   if((pressed&32)&&menu.cursor<8){phase=5;age.set(0);if(!sound(7))return false;menu.move(1);if(!interrupt(page,7,true))return false;}
   else if((pressed&16)&&menu.cursor>0){phase=5;age.set(0);if(!sound(7))return false;menu.move(-1);if(!interrupt(page,8,true))return false;}
   else if(pressed&0x80103){if(!sound(9))return false;phase=1;age.set(0);if(!interrupt(page,1,false)||!list())return false;}
  }break;
 case 5:if(age.current>=20&&!request())return false;break;
 default:return fail("Invalid manual phase");
 }}age.tick(&rate);return true;
}
}
