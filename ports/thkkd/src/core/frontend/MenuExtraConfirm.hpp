#pragma once
#include "core/frontend/MenuStartInitialization.hpp"
namespace kaikeidou {
namespace menu_start_detail {
inline void update_special_confirm(MenuStartContext& c,const SaveData& save,unsigned precision,MenuStartServices& api,unsigned card){
 draw_card(c,card,precision,api);api.blend(0,255);
 switch(c.phase){
 case 0:
  if(api.input_count(save.bindings[4])==1){c.phase=1;c.timer=0;c.cards[card].quad(40,constant(0x6a7eb8));c.sound_requests[0]=1;}
  else if(api.input_count(save.bindings[5])==1){c.phase=2;c.timer=0;c.cards[card].quad(40,0);c.sound_requests[1]=1;}break;
 case 1:++c.timer;if(c.timer==20)c.cards[card].quad(24,constant(0x6d1dd0));else if(c.timer==40){c.difficulty=card;initialize_menu_start_character_selection(c,precision);}break;
 case 2:++c.timer;if(c.timer==20)initialize_menu_start(c,api);break;
 default:break;
 }
}
}
inline void update_menu_extra_confirm(MenuStartContext& c,const SaveData& save,unsigned precision,MenuStartServices& api){menu_start_detail::update_special_confirm(c,save,precision,api,4);}
inline void present_menu_extra_confirm(MenuStartContext& c,const SaveData& save,unsigned precision,MenuStartServices& api){
 api.present_component(MenuStartComponent::routine_5e71e0);api.present_component(MenuStartComponent::routine_5e6ff0);update_menu_extra_confirm(c,save,precision,api);menu_start_detail::finish_wrapper(api,0x6c79d0,0x6c79c4);
}
}
