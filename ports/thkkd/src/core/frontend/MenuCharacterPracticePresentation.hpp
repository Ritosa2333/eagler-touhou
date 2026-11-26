// 原页面前置绘制及5E64F0/5E6360；字体和坐标按各自服务调用后的读序取得。
#pragma once
#include "core/frontend/MenuCharacterPracticeContext.hpp"
#include "core/frontend/MenuCharacterPracticeServices.hpp"
#include "core/frontend/MenuCharacterPracticeConstants.hpp"
#include "core/frontend/MenuStartInitialization.hpp"
#include "core/frontend/MenuUnlock.hpp"
namespace kaikeidou::menu_character_practice_detail {
inline std::int32_t signed32(std::uint32_t n){return static_cast<std::int32_t>(music_presentation_detail::signed_word(n));}
inline std::uint32_t integer_coordinate(std::uint64_t n){return music_presentation_detail::truncate_sse(binary_float::binary64(n));}
inline std::uint64_t card_value(const MenuStartContext& m,std::uint32_t index,std::uint32_t offset){
 const auto relative=index*48u+offset;if(relative>280u||relative%8u)throw std::out_of_range("Character menu card alias outside records");return m.cards[relative/48u].quad(relative%48u);
}
inline std::uint32_t card_graph(const MenuStartContext& m,std::uint32_t index){const auto relative=index*4u;if(relative>=24u)throw std::out_of_range("Character menu card graph alias outside records");return m.card_graphs[relative/4u];}
inline void character_store(MenuStartContext& m,std::uint32_t index,std::uint32_t offset,std::uint64_t value){
 const auto relative=index*64u+offset;if(relative>184u||relative%8u)throw std::out_of_range("Character menu character alias outside records");m.characters[relative/64u].quad(relative%64u,value);
}
inline std::uint32_t progress(const MenuStartContext& m,const MenuCharacterPracticeContext& c,const SaveData& save,std::uint32_t stage){
 (void)c;return menu_unlock_detail::progress_word(save,m.difficulty,m.character_selected,m.weapon_selected,stage);
}
inline void draw_page(MenuStartContext& m,MenuCharacterPracticeContext& c,const SaveData& save,unsigned precision,MenuCharacterPracticeServices& api){
 api.blend(2,255);
 const auto target=card_value(m,m.difficulty,24);const auto index=m.difficulty;
 menu_start_detail::card_store(m,index,0,ease_option_value(card_value(m,index,0),target,constant(0x6d1898),precision));
 const auto graph=card_graph(m,m.difficulty);const auto y=binary_float::to_binary32(binary_float::binary64(card_value(m,m.difficulty,0)));
 api.draw_rotated_sprite(static_cast<std::uint32_t>(constant(0x6d1ed8)),y,constant(0x6d1900),0,graph,0,0);api.blend(0,255);
 const bool dim_header=m.difficulty==4&&!extra_unlocked(save,signed32(m.character_selected),-1);
 auto font=c.name_font;auto ink=dim_header?api.color(127,103,0):api.color(255,207,0);
 auto label=character_text(m.character_selected,3,0);api.draw_centered_font_text(constant(0x6d18d8),constant(0x6d1a88),label,ink,font);
 for(unsigned line=0;line<2;++line){font=c.detail_font;ink=dim_header?api.color(127,127,111):api.color(255,255,223);label=character_text(m.character_selected,4u+line,0);api.draw_font_text(192,98+static_cast<std::int32_t>(20*line),label,ink,font,0,0);}
 m.character_transition_bits=ease_option_value(m.character_transition_bits,m.weapon_transition_bits,constant(0x6d1c18),precision);
 const bool dim_weapons=m.difficulty==4&&!extra_unlocked(save,signed32(m.character_selected),signed32(m.weapon_selected));
 for(unsigned i=0;i<3;++i){auto& r=m.weapons[i];r.quad(0,ease_option_value(r.quad(0),r.quad(8),constant(0x6d1c18),precision));r.quad(16,ease_option_value(r.quad(16),r.quad(24),constant(0x6d1c18),precision));
  api.blend(1,integer_coordinate(m.character_transition_bits));
  if(dim_weapons){font=c.name_font;ink=api.color(64,128,128);}
  else{ink=i==m.weapon_selected?api.color(128,255,255):api.color(64,128,128);font=c.name_font;}
  label=character_text(m.character_selected,i,0);
  api.draw_font_text(signed32(integer_coordinate(r.quad(0))),static_cast<std::int32_t>(i*100+170),label,ink,font,0,0);
  api.blend(1,integer_coordinate(r.quad(16)));
  for(unsigned line=1;line<=2;++line){font=c.detail_font;ink=dim_weapons?api.color(111,128,128):api.color(223,255,255);label=character_text(m.character_selected,i,line);api.draw_font_text(signed32(integer_coordinate(r.quad(0))),static_cast<std::int32_t>(i*100+(line==1?210:230)),label,ink,font,0,0);}
 }
 api.blend(0,255);
}
inline void draw_character_order(MenuStartContext& m,MenuCharacterPracticeContext& c,const std::array<unsigned,3>& order,MenuCharacterPracticeServices& api){
 for(auto i:order){auto& r=m.characters[i];api.blend(1,integer_coordinate(r.quad(24)));
  const auto graph=i==0?m.title_graph:c.other_character_graphs[i-1];const auto scale=r.quad(16);
  const auto y=binary_float::to_binary32(binary_float::binary64(r.quad(8)));const auto x=binary_float::to_binary32(binary_float::binary64(r.quad(0)));
  api.draw_rotated_sprite(x,y,scale,0,graph,1,0);
 }api.blend(0,255);
}
inline void update_characters(MenuStartContext& m,MenuCharacterPracticeContext& c,unsigned precision,MenuCharacterPracticeServices& api){
 for(auto& r:m.characters)for(unsigned offset=0;offset<32;offset+=8)r.quad(offset,ease_option_value(r.quad(offset),r.quad(offset+32),constant(0x6d1898),precision));
 switch(m.character_selected){
 case 0:draw_character_order(m,c,m.character_phase==0?std::array<unsigned,3>{{2,1,0}}:std::array<unsigned,3>{{1,2,0}},api);break;
 case 1:draw_character_order(m,c,m.character_phase==0?std::array<unsigned,3>{{0,2,1}}:std::array<unsigned,3>{{2,0,1}},api);break;
 case 2:draw_character_order(m,c,m.character_phase==0?std::array<unsigned,3>{{1,0,2}}:std::array<unsigned,3>{{0,1,2}},api);break;
 default:break;
 }
}
inline void draw_list(MenuStartContext& m,MenuCharacterPracticeContext& c,const SaveData& save,bool omniverse,MenuCharacterPracticeServices& api){
 api.blend(4,255);constexpr std::array<std::uint32_t,5> shades{{200,170,140,110,80}};
 for(unsigned i=0;i<5;++i){const auto ink=api.color(shades[i],shades[i],shades[i]);api.rectangle(static_cast<std::int32_t>((omniverse?80u:0u)+60*i),144,static_cast<std::int32_t>((omniverse?720u:640u)-60*i),456,ink,1);}api.blend(0,255);
 for(std::uint32_t i=0;i<(omniverse?8u:6u);++i){const bool locked=!omniverse&&progress(m,c,save,i)==0;
  const auto ink=i==c.practice_selected?api.color(255,255,0):api.color(160,160,160);const auto font=c.list_font;
  const auto label=omniverse?boss_text(i):(locked?text(0x6c7584):stage_text(i));const auto y=binary_float::to_binary64(binary_float::integer(i*(omniverse?32u:44u)+176u));
  api.draw_centered_font_text(constant(omniverse?0x6d1a78:0x6d18d8),y,label,ink,font);
 }
}
}
