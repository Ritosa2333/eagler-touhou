// 菜单外层声音请求与两处脚注；保留API回调后的句柄重读。
#pragma once
#include "core/frontend/MenuScenePlatform.hpp"
#include "core/frontend/MenuSceneConstants.hpp"
namespace kaikeidou {
template<class Memory,class Invoke>void play_menu_scene_sounds(Memory& memory,Invoke&& invoke){
 for(std::uint32_t i=0;i<100;++i){if(memory.word(0xa73230u+i*4u)!=1)continue;auto handle=memory.word(0xa78e48u+i*4u);if(invoke(MenuSceneRequest{MenuSceneApi::check_sound,{handle},{},{}}).eax!=0){handle=memory.word(0xa78e48u+i*4u);invoke(MenuSceneRequest{MenuSceneApi::stop_sound,{handle},{},{}});}handle=memory.word(0xa78e48u+i*4u);invoke(MenuSceneRequest{MenuSceneApi::play_sound,{handle,1,1},{},{}});}
}
template<class Invoke>void draw_menu_scene_footer(std::uint32_t x,std::uint32_t y,bool translator,bool replay_variant,Invoke&& invoke){
 invoke(MenuSceneRequest{MenuSceneApi::blend,{1,128},{},{}});const auto ink=invoke(MenuSceneRequest{MenuSceneApi::color,{255,255,255},{},{}}).eax;
 invoke(MenuSceneRequest{MenuSceneApi::draw_formatted,{x,y,ink},{menu_scene_detail::text(translator?(replay_variant?0x6c7458u:0xb4114fu):0x6c7430u)}, {}});invoke(MenuSceneRequest{MenuSceneApi::blend,{0,255},{},{}});
}
}
