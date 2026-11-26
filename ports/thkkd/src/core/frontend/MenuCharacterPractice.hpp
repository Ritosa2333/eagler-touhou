// 5E72E0主体与5E8E60包装；最终只写原scene99交接字段，启动初始化由下一帧调度。
#pragma once
#include "core/frontend/MenuCharacterPracticePresentation.hpp"
#include "core/stages/StageResourceContext.hpp"
namespace kaikeidou {
namespace menu_character_practice_detail {
inline void aim_characters(MenuStartContext& m,bool confirmed,unsigned precision){
 if(m.character_selected>=3)return;const auto selected=m.character_selected;constexpr std::array<std::uint32_t,3> xs{{0x6d18d8,0x6d18c8,0x6d18c0}};
 for(unsigned slot=0;slot<3;++slot){const auto i=(selected+slot)%3;
  menu_start_detail::character_target(m,i,constant(slot==0&&confirmed?0x6d1ed0:xs[slot]),constant(slot==0?0x6d18d8:0x6d18d0),slot==0?0x3ff0000000000000ull:constant(0x6d18b0),slot==0||!confirmed?constant(0x6a7eb8):0,precision);
 }
}
inline void weapon_targets(MenuStartContext& m){for(std::uint32_t i=0;i<3;++i){m.weapons[i].quad(8,constant(i==m.weapon_selected?0x6d1ec8:0x6d18d8));m.weapons[i].quad(24,i==m.weapon_selected?constant(0x6a7eb8):0);}}
inline void hide_other_weapons(MenuStartContext& m){for(std::uint32_t i=0;i<3;++i)if(i!=m.weapon_selected)m.weapons[i].quad(8,constant(0x6d1ec0));}
}
inline void update_menu_character_practice(MenuStartContext& m,MenuCharacterPracticeContext& c,StageResourceContext& resources,const SaveData& save,unsigned precision,MenuCharacterPracticeServices& api){
 using namespace menu_character_practice_detail;draw_page(m,c,save,precision,api);
 switch(m.phase){
 case 0:
  if(api.input_count(save.bindings[0])==1){m.character_phase=0;m.character_selected=m.character_selected==0?2:m.character_selected-1;aim_characters(m,false,precision);m.weapon_selected=0;m.sound_requests[2]=1;}
  if(api.input_count(save.bindings[1])==1){m.character_phase=1;m.character_selected=m.character_selected==2?0:m.character_selected+1;aim_characters(m,false,precision);m.weapon_selected=0;m.sound_requests[2]=1;}
  if(api.input_count(save.bindings[4])==1){
   m.phase=10;m.timer=0;aim_characters(m,true,precision);
   for(std::uint32_t i=0;i<3;++i){m.weapons[i].quad(0,constant(0x6d18d8));m.weapons[i].quad(8,constant(i==m.weapon_selected?0x6d1ec8:0x6d18d8));m.weapons[i].quad(24,i==m.weapon_selected?constant(0x6a7eb8):0);}
   m.weapon_transition_bits=constant(0x6a7eb8);m.sound_requests[0]=1;
  }else if(api.input_count(save.bindings[5])==1){m.phase=2;m.timer=0;for(auto& r:m.characters)r.quad(56,0);menu_start_detail::card_store(m,m.difficulty,24,constant(0x6d18a8));m.sound_requests[1]=1;}
  break;
 case 2:
  ++m.timer;if(m.timer==20){if(m.difficulty==4)initialize_menu_extra_confirm(m);else if(m.difficulty==5)initialize_menu_omniverse_confirm(m);else initialize_menu_difficulty(m);}break;
 case 10:
  if(api.input_count(save.bindings[2])==1){m.weapon_selected=m.weapon_selected==0?2:m.weapon_selected-1;weapon_targets(m);m.sound_requests[2]=1;}
  if(api.input_count(save.bindings[3])==1){m.weapon_selected=m.weapon_selected==2?0:m.weapon_selected+1;weapon_targets(m);m.sound_requests[2]=1;}
  if(api.input_count(save.bindings[4])==1){
   if(m.game_mode!=0){m.phase=20;m.timer=0;hide_other_weapons(m);m.sound_requests[0]=1;}
   else if(m.difficulty==5){m.phase=30;m.timer=0;hide_other_weapons(m);m.sound_requests[0]=1;}
   else if(m.difficulty==4&&!extra_unlocked(save,signed32(m.character_selected),signed32(m.weapon_selected)))m.sound_requests[1]=1;
   else{m.phase=11;m.timer=0;hide_other_weapons(m);m.sound_requests[0]=1;}
  }
  if(api.input_count(save.bindings[5])==1){
   m.phase=0;m.timer=0;character_store(m,m.character_selected,32,constant(0x6d18d8));
   for(unsigned i=0;i<3;++i){m.characters[i].quad(56,constant(0x6a7eb8));m.weapons[i].quad(24,0);}m.weapon_transition_bits=0;m.sound_requests[1]=1;
  }break;
 case 11:
  ++m.timer;if(m.timer==20){resources.character=m.character_selected;c.weapon=m.weapon_selected;resources.stage=signed32(m.difficulty)<=3?0u:6u;m.outer_scene=99;}break;
 case 20:case 30:{
  const bool omni=m.phase==30;draw_list(m,c,save,omni,api);const auto last=omni?7u:5u;
  if(api.input_count(save.bindings[2])==1){c.practice_selected=c.practice_selected==0?last:c.practice_selected-1;m.sound_requests[2]=1;}
  if(api.input_count(save.bindings[3])==1){c.practice_selected=c.practice_selected==last?0:c.practice_selected+1;m.sound_requests[2]=1;}
  if(api.input_count(save.bindings[4])==1){
   if(omni||progress(m,c,save,c.practice_selected)!=0){m.phase=omni?31u:21u;m.timer=0;m.sound_requests[0]=1;}else m.sound_requests[1]=1;
  }
  if(api.input_count(save.bindings[5])==1){m.phase=10;m.timer=0;weapon_targets(m);m.sound_requests[1]=1;}break;}
 case 21:
  ++m.timer;if(m.timer==20){resources.character=m.character_selected;c.weapon=m.weapon_selected;resources.stage=c.practice_selected;m.outer_scene=99;}break;
 case 31:
  ++m.timer;if(m.timer==20){resources.character=m.character_selected;c.weapon=m.weapon_selected;resources.stage=7;m.outer_scene=99;}break;
 default:break;
 }
 update_characters(m,c,precision,api);
}
inline void present_menu_character_practice(MenuStartContext& m,MenuCharacterPracticeContext& c,StageResourceContext& resources,const SaveData& save,unsigned precision,MenuCharacterPracticeServices& api){
 api.present_component(MenuStartComponent::routine_5e71e0);api.present_component(MenuStartComponent::routine_5e6ff0);update_menu_character_practice(m,c,resources,save,precision,api);
 using namespace menu_character_practice_detail;
 if(signed32(m.phase)<=9)api.draw_help(text(0x6c7508),text(0x6c74f8));
 else if(signed32(m.phase)<=19)api.draw_help(text(0x6c7528),text(0x6c7518));
 else if(signed32(m.phase)<=29)api.draw_help(text(0x6c754c),text(0x6c753c));
 else if(signed32(m.phase)<=39)api.draw_help(text(0x6c7570),text(0x6c7560));
 api.move_cursor(572,465);api.present_cursor();
}
}
