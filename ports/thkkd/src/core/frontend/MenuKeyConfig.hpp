// 5E9FE0 按键菜单；低16位中首个置位直接赋值，保留重复绑定。
#pragma once
#include "core/frontend/MenuKeyConfigContext.hpp"
#include "core/frontend/MenuOptions.hpp"
namespace kaikeidou {
inline void initialize_menu_key_config(MenuKeyConfigContext& c,const SaveData& save){
 c.outer_scene=15;c.phase=0;c.timer=0;for(std::size_t i=0;i<9;++i)c.pending[i]=save.bindings[i];
}
inline void update_menu_key_config(MenuKeyConfigContext& c,SaveData& save,SaveIO& io,MenuOptionsServices& api){
 using namespace menu_options_detail;
 api.set_font_size(30);
 constexpr std::array<std::uint32_t,9> names{{0x6c732c,0x6c7334,0x6c733c,0x6c7344,0x6c734c,0x6c7354,0x6c735c,0x6c7364,0x6c736c}};
 for(std::uint32_t i=0;i<9;++i){
  auto ink=label_color(c.selected==i,api);const auto y=90+static_cast<std::int32_t>(38*i);
  api.draw_text(230,y,ink,text(0x6c7a78),0,text(names[i]));
  ink=value_color(c.selected==i,api);api.draw_text(335,y,ink,text(0x6c7a7c),c.pending[i],{});
 }
 const auto ink=label_color(c.selected==9,api);
 api.centered_text(coordinate(0x6d18d8),coordinate(0x6d1830),ink,text(0x6c7a80));api.set_font_size(16);
 switch(c.phase){
 case 0:
  if(api.input_count(save.bindings[2])==1){c.selected=c.selected==0?9:c.selected-1;c.sound_requests[2]=1;}
  if(api.input_count(save.bindings[3])==1){c.selected=c.selected==9?0:c.selected+1;c.sound_requests[2]=1;}
  if(api.input_count(save.bindings[4])==1){
   if(c.selected==9){for(std::size_t i=0;i<9;++i)save.bindings[i]=c.pending[i];store_scores_and_settings(save,io);c.phase=3;}
   else c.phase=1;
   c.timer=0;c.sound_requests[0]=1;
  }else if(api.input_count(save.bindings[5])==1){c.phase=3;c.timer=0;c.sound_requests[1]=1;}
  break;
 case 1:if(api.input_count(save.bindings[4])==0){c.phase=2;c.timer=0;}break;
 case 2:{
  const auto mask=api.pad_state(1);std::uint32_t bit=0;
  while(bit<16&&(mask&(1u<<bit))==0)++bit;
  if(bit!=16){if(c.selected<9)c.pending[c.selected]=bit;c.phase=0;c.timer=0;c.sound_requests[0]=1;}
  break;}
 case 3:++c.timer;if(c.timer==20)return_title(c,api);break;
 default:break;
 }
}
}
