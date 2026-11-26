// 主导航保留原解锁跳过、确认后仍读取取消、两版菜单位置与Replay路由写法。
#pragma once
#include "core/frontend/MenuDifficulty.hpp"
#include "core/frontend/MenuOmniverseConfirm.hpp"
#include "core/frontend/MenuUnlock.hpp"
namespace kaikeidou {
inline std::string_view menu_start_label(std::uint32_t index,const SaveData& save,bool replay_variant){
 using menu_start_detail::text;
 if(index==0)return text(0x6c7468);if(index==1)return text(0x6c7474);
 if(index==2)return text(omniverse_unlocked(save)?0x6c7480:0x6c7490);if(index==3)return text(0x6c749c);
 if(replay_variant){if(index==4)return text(0xb411ce);--index;}
 switch(index){case 4:return text(0x6c74ac);case 5:return text(0x6c74b8);case 6:return text(0x6c74c4);case 7:return text(0x6c74cc);default:return text(0x6c74d4);}
}
inline bool update_menu_start(MenuStartContext& c,const SaveData& save,unsigned precision,MenuStartServices& api,bool replay_variant=false){
 using namespace menu_start_detail;const std::uint32_t last=replay_variant?8u:7u;
 const auto draw=[&](std::uint32_t i,std::uint32_t y0,std::uint32_t edge_component){
  const auto ink=c.main_selected==i?api.color(255,207,0):api.color(128,128,128);
  const auto edge=api.color(edge_component,edge_component,edge_component);const auto font=c.font;const auto label=menu_start_label(i,save,replay_variant);
  api.draw_font_text(465-static_cast<std::int32_t>(26*i),static_cast<std::int32_t>(y0+30*i),label,ink,font,edge,0);
 };
 if(c.phase==0){
  for(std::uint32_t i=0;i<=last;++i)draw(i,replay_variant?200u:225u,0);
  if(api.input_count(save.bindings[2])==1){
   if(c.main_selected==0)c.main_selected=last;
   else if(c.main_selected==3){if(omniverse_unlocked(save))c.main_selected=2;else c.main_selected=extra_unlocked(save,-1,-1)?1u:0u;}
   else --c.main_selected;c.sound_requests[2]=1;
  }
  if(api.input_count(save.bindings[3])==1){
   if(c.main_selected==last)c.main_selected=0;
   else if(c.main_selected==0)c.main_selected=extra_unlocked(save,-1,-1)?1u:3u;
   else if(c.main_selected==1)c.main_selected=omniverse_unlocked(save)?2u:3u;
   else ++c.main_selected;c.sound_requests[2]=1;
  }
  if(api.input_count(save.bindings[4])==1){if(c.main_selected==last)return true;c.phase=1;c.timer=0;c.sound_requests[0]=1;}
  if(api.input_count(save.bindings[5])==1||api.input_count(save.bindings[7])==1){c.sound_requests[0]=1;if(c.main_selected==last)return true;c.main_selected=last;}
 }else if(c.phase==1){
  const auto alpha=music_presentation_detail::truncate_sse(binary_float::divide(binary_float::integer((10u-c.timer)*255u),binary_float::binary64(constant(0x6d1a40)),precision));
  for(std::uint32_t i=0;i<=last;++i){if(i==c.main_selected)api.blend(0,255);else api.blend(1,alpha);draw(i,225,207);}
  api.blend(0,255);++c.timer;
  if(c.timer==20){
   const auto selected=c.main_selected;
   if(selected==0){c.game_mode=0;initialize_menu_difficulty(c);}
   else if(selected==1){c.game_mode=0;initialize_menu_extra_confirm(c);}
   else if(selected==2){c.game_mode=0;initialize_menu_omniverse_confirm(c);}
   else if(selected==3){c.game_mode=1;initialize_menu_difficulty(c);}
   else if(replay_variant&&selected==4){c.replay_submode=1;initialize_options(c,save);}
   else if(selected==(replay_variant?5u:4u))initialize_music(c);
   else if(selected==(replay_variant?6u:5u))initialize_keys(c,save);
   else if(selected==(replay_variant?7u:6u))initialize_options(c,save);
   else if(selected==last)return true;
  }
 }
 return false;
}
inline bool present_menu_start(MenuStartContext& c,const SaveData& save,unsigned precision,MenuStartServices& api,bool replay_variant=false){
 using namespace menu_start_detail;api.present_component(MenuStartComponent::routine_5e71e0);api.present_component(MenuStartComponent::routine_5e6ff0);api.present_component(MenuStartComponent::routine_5e6c00);
 api.blend(2,255);api.draw_rotated_sprite(static_cast<std::uint32_t>(constant(0x6d1edc)),static_cast<std::uint32_t>(constant(0x6d1ee0)),0x3ff0000000000000ull,0,c.title_graph,1,0);api.blend(0,255);
 if(update_menu_start(c,save,precision,api,replay_variant))return true;api.move_cursor(572,465);api.present_cursor();return false;
}
}
