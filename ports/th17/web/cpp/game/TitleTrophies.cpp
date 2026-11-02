#include "TitleTrophies.hpp"
#include <cstdio>
namespace th17 {
bool TrophyCatalog::open(const u8* bytes,u32 size,Rng* visual){
 entries={};error.clear();if(!bytes){error="Trophy text unavailable";return false;}u32 cursor=0;
 auto line=[&](std::string& out){const u32 start=cursor;while(cursor<size&&bytes[cursor]!=10&&bytes[cursor]!=13)cursor++;if(cursor-start>=256)return false;out.assign(reinterpret_cast<const char*>(bytes+start),cursor-start);while(cursor<size&&(bytes[cursor]==10||bytes[cursor]==13))cursor++;return true;};
 while(cursor<size&&bytes[cursor]!='\\'){
  if(bytes[cursor]!='@'){std::string ignored;if(!line(ignored)){error="Trophy comment too long";return false;}continue;}
  std::string number;if(!line(number)){error="Trophy entry heading too long";return false;}i32 index=-1;if(std::sscanf(number.c_str(),"@%d",&index)!=1||index<0||index>=40){error="Trophy index outside original range";return false;}
  // Native trophy initialization fills seven 256-byte fields with visual
  // random words before writing their text. Preserve its RNG advance without
  // retaining the encrypted padding in the named string representation.
  if(visual)for(i32 n=0;n<7*256;n++)visual->next32();
  auto& item=entries[index];item.present=true;if(!line(item.title)){error="Trophy title too long";return false;}
  for(auto& state:item.descriptions)for(auto& text:state)if(!line(text)){error="Trophy description too long";return false;}
 }return true;
}
bool TitleTrophies::check(bool ok,const char* why){if(!ok&&error.empty())error=why;return ok;}
bool TitleTrophies::visual(bool ok){return check(ok,visuals.error.c_str());}
bool TitleTrophies::text(i32 slot,const std::string& bytes){auto* vm=animations.registry.find(state.handles[slot]);if(!vm){state.handles[slot]=0;return true;}TextureText request;request.bytes=bytes;request.font=4;request.centered=true;return check(host.text(*vm,request),"Trophy text rendering failed");}
bool TitleTrophies::description(){
 if(description_age<8){const auto* item=catalog.entry(description_index);if(!item)return check(false,"Trophy description unavailable");const bool owned=records.trophies[description_index]!=0;
  if(description_age==0){std::string hidden;for(i32 i=0;i<12;i++){hidden+=char(0x81);hidden+=char(0x48);}if(!text(197,owned?item->title:hidden)||!visual(visuals.interrupt(197,4)))return false;for(i32 i=198;i<=200;i++)if(!visual(visuals.interrupt(i,5)))return false;}
  else if(description_age==3||description_age==5||description_age==7){const i32 row=(description_age-3)/2;if(!text(198+row,item->descriptions[owned?1:0][row])||!visual(visuals.interrupt(198+row,4)))return false;}
 }description_age=wrapping_add(description_age,1);return true;
}
bool TitleTrophies::update(u32 pressed,u32 repeated){
 if(!error.empty())return false;const u32 input=pressed|repeated;
 switch(state.substate){
 case 0:
  if(!visual(visuals.prompt()))return false;state.change_substate(1);state.menu.count=40;state.menu.wrap=true;state.menu.select(saved_selection);for(i32 id:{105,197,198,199,200})if(!visual(visuals.create(id)))return false;
  if(catalog.entry(state.menu.selected)){description_age=0;description_index=state.menu.selected;}break;
 case 1:
  state.menu.reserved=state.menu.selected;if(input&16)state.menu.step(-10);if(input&32)state.menu.step(10);if(input&64)state.menu.step(state.menu.selected%10==0?9:-1);if(input&128)state.menu.step(state.menu.selected%10==9?-9:1);
  if(state.menu.reserved!=state.menu.selected){if(!check(host.sound(10),"Trophy navigation sound failed"))return false;if(catalog.entry(state.menu.selected)){description_age=0;description_index=state.menu.selected;}}
  if((pressed&0x80001)&&records.trophies[state.menu.selected]){
   if(state.menu.selected<10)return check(host.sound(7)&&host.fade_menu_music()&&host.preview_ending(state.menu.selected),"Trophy ending preview failed");
   if(state.menu.selected<21){if(!check(host.sound(7)&&host.fade_menu_music(),"Trophy stage fade failed"))return false;state.change_substate(2);break;}
  }
  if(pressed&0x102){state.change_substate(3);if(!check(host.sound(9),"Trophy cancel sound failed"))return false;for(i32 id:{197,198,199,200,105})if(!visual(visuals.retire(id)))return false;}
  if(state.menu.selected==12){TitleKeyboard keys;bool unlocked=false;if(!check(host.keyboard(keys),"Trophy keyboard unavailable")||!check(cheat.update(pressed,keys,unlocked,true),cheat.error.c_str()))return false;if(unlocked&&!check(host.sound(17),"Trophy unlock sound failed"))return false;}break;
 case 2:
  if(state.age.current==10&&(!check(host.show_loading_indicator()&&host.begin_transition(transition),"Trophy transition failed")||!check(animations.interrupt(transition,7),"Trophy transition animation failed")))return false;
  if(state.age.current>39){state.return_reason=6;saved_selection=state.menu.selected;const i32 character=(state.menu.selected-12)/3;state.change_screen(TitleScreen::Quit);if(!check(host.trophy_stage(character,character),"Trophy stage creation failed"))return false;}break;
 case 3:if(state.age.current>19){saved_selection=state.menu.selected;state.change_screen(TitleScreen::PlayerData);state.menu.pop();}break;
 }return description();
}
bool TitleTrophies::draw(HudDrawServices& sink){if(!error.empty())return false;if(state.substate!=1)return true;HudTextDraw out;out.style.font=4;out.style.shadow=true;for(i32 i=0;i<40;i++){out.position={float(130+i%10*38),float(100+i/10*34),0};out.style.color=i==state.menu.selected?0xffffffff:0xff707070;char number[16];std::snprintf(number,sizeof number,"%2d",i+1);out.text=records.trophies[i]?number:"--";if(!check(sink.hud_text(out),"Trophy grid drawing failed"))return false;}return true;}
}
