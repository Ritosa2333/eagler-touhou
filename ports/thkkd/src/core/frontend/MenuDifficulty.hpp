#pragma once
#include "core/frontend/MenuStartInitialization.hpp"
namespace kaikeidou {
inline void update_menu_difficulty(MenuStartContext& c,const SaveData& save,unsigned precision,MenuStartServices& api){
 using namespace menu_start_detail;for(unsigned i=0;i<4;++i)draw_card(c,i,precision,api);api.blend(0,255);
 switch(c.phase){
 case 0:
  if(api.input_count(save.bindings[2])==1){
   c.difficulty_selected=c.difficulty_selected==0?3:c.difficulty_selected-1;
   for(std::uint32_t i=0;i<4;++i){c.cards[i].quad(24,position(i,c.difficulty_selected));c.cards[i].quad(32,i==c.difficulty_selected?0x3ff0000000000000ull:constant(0x6d18e0));}c.sound_requests[2]=1;
  }
  if(api.input_count(save.bindings[3])==1){
   c.difficulty_selected=c.difficulty_selected==3?0:c.difficulty_selected+1;
   for(std::uint32_t i=0;i<4;++i){c.cards[i].quad(24,position(i,c.difficulty_selected));c.cards[i].quad(32,i==c.difficulty_selected?0x3ff0000000000000ull:constant(0x6d18e0));}c.sound_requests[2]=1;
  }
  if(api.input_count(save.bindings[4])==1){c.phase=1;c.timer=0;for(std::uint32_t i=0;i<4;++i)c.cards[i].quad(40,i==c.difficulty_selected?constant(0x6a7eb8):0);c.sound_requests[0]=1;}
  else if(api.input_count(save.bindings[5])==1){c.phase=2;c.timer=0;for(unsigned i=0;i<4;++i)c.cards[i].quad(40,0);c.sound_requests[1]=1;}break;
 case 1:++c.timer;if(c.timer==20)card_store(c,c.difficulty_selected,24,constant(0x6d2ab0));else if(c.timer==40){c.difficulty=c.difficulty_selected;initialize_menu_start_character_selection(c,precision);}break;
 case 2:++c.timer;if(c.timer==20)initialize_menu_start(c,api);break;
 default:break;
 }
}
inline void present_menu_difficulty(MenuStartContext& c,const SaveData& save,unsigned precision,MenuStartServices& api){
 api.present_component(MenuStartComponent::routine_5e71e0);api.present_component(MenuStartComponent::routine_5e6ff0);update_menu_difficulty(c,save,precision,api);menu_start_detail::finish_wrapper(api,0x6c74e8,0x6c74dc);
}
}
