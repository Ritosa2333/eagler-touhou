// 原菜单入口的共享字段写入和角色选择初始化；交互由后续模块接入。
#pragma once
#include "core/frontend/MenuStartContext.hpp"
#include "core/frontend/MenuStartServices.hpp"
#include "core/frontend/MenuStartConstants.hpp"
#include "core/frontend/SaveData.hpp"
#include "core/player/OptionsMotion.hpp"
#include "core/math/BinaryFloat32.hpp"
#include "core/shared/MusicPresentationArithmetic.hpp"
namespace kaikeidou {
namespace menu_start_detail {
inline void card_store(MenuStartContext& c,std::uint32_t index,std::uint32_t offset,std::uint64_t bits){
 const auto relative=index*48u+offset;
 if(relative>288u-8u||relative%8u)throw std::out_of_range("Menu card alias outside six records");
 c.cards[relative/48u].quad(relative%48u,bits);
}
inline std::uint64_t position(std::uint32_t i,std::uint32_t selected){return binary_float::to_binary64(binary_float::integer((i-selected)*128u+260u));}
inline void character_target(MenuStartContext& c,std::uint32_t i,std::uint64_t x,std::uint64_t y,std::uint64_t scale,std::uint64_t alpha,unsigned precision){
 auto& r=c.characters[i];r.quad(32,x);r.quad(40,y);
 if(i==2)scale=binary_float::to_binary64(binary_float::multiply(binary_float::binary64(scale),binary_float::binary64(constant(0x6d1890)),precision));
 r.quad(48,scale);r.quad(56,alpha);
}
inline void draw_card(MenuStartContext& c,unsigned i,unsigned precision,MenuStartServices& api){
 auto& r=c.cards[i];
 r.quad(0,ease_option_value(r.quad(0),r.quad(24),constant(0x6d1898),precision));
 r.quad(8,ease_option_value(r.quad(8),r.quad(32),constant(0x6d1898),precision));
 r.quad(16,ease_option_value(r.quad(16),r.quad(40),constant(0x6d1898),precision));
 api.blend(2,music_presentation_detail::truncate_sse(binary_float::binary64(r.quad(16))));
 // 原指令先取图句柄、缩放，再把坐标舍入至binary32，随后调用绘制。
 const auto graph=c.card_graphs[i];const auto scale=r.quad(8);
 const auto y=binary_float::to_binary32(binary_float::binary64(r.quad(0)));
 api.draw_rotated_sprite(static_cast<std::uint32_t>(constant(0x6d1ee0)),y,scale,0,graph,0,0);
}
inline void finish_wrapper(MenuStartServices& api,std::uint32_t first,std::uint32_t second){
 api.draw_help(text(first),text(second));api.move_cursor(572,465);api.present_cursor();
}
}
inline void initialize_menu_start(MenuStartContext& c,MenuStartServices& api){c.outer_scene=10;c.phase=0;c.timer=0;api.change_stage_resources(-1);}
inline void initialize_menu_start_presentation(MenuStartServices& api){api.present_component(MenuStartComponent::initialize_title_5e9490);}
inline void initialize_menu_difficulty(MenuStartContext& c){
 using namespace menu_start_detail;c.outer_scene=11;c.phase=0;c.timer=0;
 for(std::uint32_t i=0;i<4;++i){auto& r=c.cards[i];r.quad(0,position(i,c.difficulty_selected));r.quad(8,constant(0x6d1880));r.quad(16,0);r.quad(24,r.quad(0));r.quad(32,i==c.difficulty_selected?0x3ff0000000000000ull:constant(0x6d18e0));r.quad(40,constant(0x6a7eb8));}
}
inline void initialize_menu_extra_confirm(MenuStartContext& c){
 using menu_start_detail::constant;c.outer_scene=13;c.phase=0;c.timer=0;auto& r=c.cards[4];
 r.quad(0,constant(0x6d1888));r.quad(8,constant(0x6d1880));r.quad(16,0);r.quad(24,constant(0x6d1888));r.quad(32,0x3ff0000000000000ull);r.quad(40,constant(0x6a7eb8));
}
inline void initialize_menu_omniverse_confirm(MenuStartContext& c){
 using menu_start_detail::constant;c.outer_scene=17;c.phase=0;c.timer=0;auto& r=c.cards[5];
 r.quad(0,constant(0x6d1888));r.quad(8,constant(0x6d1880));r.quad(16,0);r.quad(24,constant(0x6d1888));r.quad(32,0x3ff0000000000000ull);r.quad(40,constant(0x6a7eb8));
}
inline void initialize_menu_start_character_selection(MenuStartContext& c,unsigned precision){
 using namespace menu_start_detail;c.outer_scene=12;c.phase=0;c.timer=0;c.character_phase=1;
 if(c.character_selected<3){
  const auto selected=c.character_selected;
  constexpr std::array<std::uint32_t,3> xs{{0x6d18d8,0x6d18c8,0x6d18c0}};
  for(unsigned slot=0;slot<3;++slot){const auto i=(selected+slot)%3;auto& r=c.characters[i];r.quad(0,constant(xs[slot]));r.quad(8,constant(0x6d18d0));r.quad(24,0);r.quad(16,constant(i==2?0x6d18b8:0x6d1880));}
  for(unsigned slot=0;slot<3;++slot){const auto i=(selected+slot)%3;character_target(c,i,constant(xs[slot]),constant(slot==0?0x6d18d8:0x6d18d0),slot==0?0x3ff0000000000000ull:constant(0x6d18b0),constant(0x6a7eb8),precision);}
 }
 card_store(c,c.difficulty,0,constant(0x6d18a8));card_store(c,c.difficulty,24,constant(0x6d18a0));
 for(std::uint32_t i=0;i<3;++i){auto& r=c.weapons[i];r.quad(0,constant(0x6d18d8));r.quad(8,binary_float::to_binary64(binary_float::integer(i==c.weapon_selected?250u:320u)));r.quad(16,0);r.quad(24,0);}
 c.character_transition_bits=0;c.weapon_transition_bits=0;
}
namespace menu_start_detail {
inline void initialize_music(MenuStartContext& c){c.outer_scene=14;c.music_selected=0;c.music_scroll=0;c.phase=0;c.timer=0;for(auto& x:c.music_x_bits)x=constant(0x6d1878);}
inline void initialize_keys(MenuStartContext& c,const SaveData& save){c.outer_scene=15;c.phase=0;c.timer=0;for(unsigned i=0;i<9;++i)c.key_pending[i]=save.bindings[i];}
inline void initialize_options(MenuStartContext& c,const SaveData& save){c.outer_scene=16;c.phase=0;c.timer=0;c.option_pending[1]=save.options[1];c.option_pending[2]=save.options[2];c.option_pending[0]=save.options[0];c.option_pending[3]=save.options[3];c.option_pending[4]=save.options[4];}
}
}
