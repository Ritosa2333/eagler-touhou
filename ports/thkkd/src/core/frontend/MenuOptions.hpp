// 5E9830 设置菜单；绘制和输入的每个服务调用后重读原全局字段。
#pragma once
#include "core/frontend/MenuOptionsContext.hpp"
#include "core/frontend/MenuOptionsServices.hpp"
#include "core/frontend/MenuOptionsConstants.hpp"
#include "core/shared/MusicPresentationContext.hpp"
#include "core/shared/MusicPresentationArithmetic.hpp"
#include "core/frontend/SaveData.hpp"
namespace kaikeidou {
inline void initialize_menu_options(MenuOptionsContext& c,const SaveData& save){
 c.outer_scene=16;c.phase=0;c.timer=0;
 c.pending[1]=save.options[1];c.pending[2]=save.options[2];c.pending[0]=save.options[0];
 c.pending[3]=save.options[3];c.pending[4]=save.options[4];
}
namespace menu_options_detail {
inline void return_title(MenuOptionsSharedContext& c,MenuOptionsServices& api){
 c.outer_scene=10;c.phase=0;c.timer=0;api.change_stage_resources(-1);
}
inline std::uint32_t label_color(bool selected,MenuOptionsServices& api){
 const auto v=selected?255u:128u;return api.color(v,v,v);
}
inline std::uint32_t value_color(bool selected,MenuOptionsServices& api){
 return selected?api.color(255,255,128):api.color(128,128,64);
}
inline void bgm_volume(std::uint32_t value,const MusicPresentationContext& music,unsigned precision,MenuOptionsServices& api){
 const auto handle=music.audio_handle;api.set_volume(music_presentation_detail::volume(value,precision),handle);
}
inline void sound_volume(std::uint32_t value,const std::array<std::uint32_t,100>& handles,unsigned precision,MenuOptionsServices& api){
 const auto scaled=music_presentation_detail::volume(value,precision);
 for(std::size_t i=0;i<handles.size();++i)api.set_volume(scaled,handles[i]);
}
}
inline void update_menu_options(MenuOptionsContext& c,SaveData& save,const MusicPresentationContext& music,
 const std::array<std::uint32_t,100>& sound_handles,unsigned precision,SaveIO& io,MenuOptionsServices& api,bool replay_variant=false){
 using namespace menu_options_detail;
 api.set_font_size(replay_variant?25u:30u);
 constexpr std::array<std::uint32_t,4> labels{{0x6c7aa4,0x6c7ab4,0x6c7ac4,0x6c7ad8}};
 for(std::uint32_t i=0;i<4;++i){
  auto ink=label_color(c.selected==i,api);
  const auto y=90+static_cast<std::int32_t>(38*i);
  api.draw_text(140,y,ink,text(labels[i]),0,{});
  if(i<2){ink=value_color(c.selected==i,api);api.draw_text(335,y,ink,text(i==0?0x6c7ab0:0x6c7ac0),c.pending[i+1],{});}
  else{
   const auto address=i==2?(c.pending[0]?0x6c7ad0u:0x6c7ad4u):(c.pending[3]?0x6c7ae4u:0x6c7ae8u);
   ink=value_color(c.selected==i,api);api.draw_text(335,y,ink,text(address),0,{});
  }
 }
 const auto ink=label_color(c.selected==4,api);
 api.centered_text(coordinate(0x6d18d8),coordinate(0x6d2100),ink,text(0x6c7aec));
 api.set_font_size(16);
 switch(c.phase){
 case 0:
  if(api.input_count(save.bindings[2])==1){c.selected=c.selected==0?4:c.selected-1;c.sound_requests[2]=1;}
  if(api.input_count(save.bindings[3])==1){c.selected=c.selected==4?0:c.selected+1;c.sound_requests[2]=1;}
  switch(c.selected){
  case 0:case 1:{
   // 跳表选择在输入回调前固定；音量值和按键绑定仍逐次重读。
   const auto slot=c.selected+1;
   if(api.input_count(save.bindings[0])==1&&music_presentation_detail::signed_word(c.pending[slot])>=5){
    c.pending[slot]-=5;
    if(slot==1)bgm_volume(c.pending[slot],music,precision,api);else sound_volume(c.pending[slot],sound_handles,precision,api);
    c.sound_requests[2]=1;
   }
   if(api.input_count(save.bindings[1])==1&&music_presentation_detail::signed_word(c.pending[slot])<=95){
    c.pending[slot]+=5;
    if(slot==1)bgm_volume(c.pending[slot],music,precision,api);else sound_volume(c.pending[slot],sound_handles,precision,api);
    c.sound_requests[2]=1;
   }
   break;}
  case 2:
   if(api.input_count(save.bindings[0])==1||api.input_count(save.bindings[1])==1){c.pending[0]=1u-c.pending[0];c.sound_requests[2]=1;}break;
  case 3:
   if(api.input_count(save.bindings[0])==1||api.input_count(save.bindings[1])==1){c.pending[3]=1u-c.pending[3];c.sound_requests[2]=1;}break;
  default:break;
  }
  if(api.input_count(save.bindings[4])==1){
   if(c.selected==4){
    save.options[1]=c.pending[1];save.options[2]=c.pending[2];save.options[0]=c.pending[0];save.options[3]=c.pending[3];
    store_scores_and_settings(save,io);c.phase=1;c.timer=0;c.sound_requests[0]=1;
   }
  }else if(api.input_count(save.bindings[5])==1){
   bgm_volume(save.options[1],music,precision,api);sound_volume(save.options[2],sound_handles,precision,api);
   c.phase=1;c.timer=0;c.sound_requests[1]=1;
  }
  break;
 case 1:++c.timer;if(c.timer==20)return_title(c,api);break;
 default:break;
 }
}
}
