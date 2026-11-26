// 5E9490、5E9800及Replay B419A7/B42C98；保留平台返回、字段写序和原整数除法。
#pragma once
#include "core/frontend/MenuTitleReturnContext.hpp"
#include "core/frontend/MenuTitleReturnServices.hpp"
#include "core/frontend/MenuTitleReturnConstants.hpp"
#include "core/frontend/MenuStartContext.hpp"
#include "core/shared/MusicPresentation.hpp"
#include "core/frontend/SaveData.hpp"
namespace kaikeidou {
namespace menu_title_return_detail {
inline std::uint64_t integer_bits(std::uint32_t value){return binary_float::to_binary64(binary_float::integer(value));}
inline void title_resources(StageResourceContext& context,MenuTitleReturnServices& api){
 // 固定参数-1执行60ECB0的8删3载，其他资源字段按原分支保留。
 for(unsigned i=0;i<8;++i)api.delete_graph(context.handles[i],0);
 const auto first=api.load_graph(text(0x6c7af4),0);context.handles[0]=first;
 const auto second=api.load_graph(text(0x6c7b10),0);context.handles[1]=second;
 const auto third=api.load_graph(text(0x6c7b2c),0);context.handles[2]=third;
}
}
inline void return_menu_title(MenuStartContext& menu,MenuTitleReturnContext& title,StageResourceContext& resources,MenuTitleReturnServices& api,bool replay_variant=false){
 if(replay_variant){
  menu.phase=0;menu.timer=0;
  if(title.replay_mode==0&&title.save_disabled==0){
   menu.outer_scene=16;menu.replay_submode=3;title.save_choice=1;
   title.prompt.word(0,0xe6b4a2b4u);title.prompt.word(4,0xc3d6bbceu);
  }else{menu.outer_scene=10;title.replay_mode=0;title.eof_flags=0;}
  title.accumulator=0;
 }else{menu.outer_scene=10;menu.phase=0;menu.timer=0;}
 menu_title_return_detail::title_resources(resources,api);
}
inline void initialize_menu_title(MenuTitleReturnContext& title,MenuStartContext& menu,MusicPresentationContext& music,
 const std::uint32_t& game_scene,const SaveData& save,unsigned precision,MenuTitleReturnServices& api){
 using namespace menu_title_return_detail;const options_motion_detail::Arithmetic m{precision};
 auto value=api.platform_random(400);title.strips[0].quad(0,integer_bits(value-100u));
 value=api.platform_random(400);title.strips[0].quad(8,integer_bits(0xffffff9cu-value));
 value=api.platform_random(400);title.strips[1].quad(0,integer_bits(value-100u));
 value=api.platform_random(400);title.strips[1].quad(8,integer_bits(580u-value));
 value=api.platform_random(400);title.strips[2].quad(0,integer_bits(value+740u));
 value=api.platform_random(400);title.strips[2].quad(8,integer_bits(0xffffff9cu-value));
 value=api.platform_random(400);title.strips[3].quad(0,integer_bits(value+740u));
 value=api.platform_random(400);title.strips[3].quad(8,integer_bits(580u-value));
 title.strips[4].quad(0,constant(0x6d18d8));title.strips[4].quad(8,constant(0x6d1d88));
 for(unsigned i=0;i<4;++i){
  value=api.platform_random(64);title.strips[i].word(16,value);
  value=api.platform_random(16);title.strips[i].word(20,value);
  value=api.platform_random(96);title.strips[i].word(24,value+32u);
 }
 title.strips[4].word(16,0);title.strips[4].word(20,0);title.strips[4].word(24,96);
 for(unsigned i=0;i<50;++i){auto& r=title.particles[i];
  value=api.platform_random(20);r.quad(32,m.store(m.add(m.mul(m.i(value),m.q(constant(0x6d1928))),m.q(constant(0x6d1920)))));
  value=api.platform_random(740);r.quad(0,integer_bits(value-50u));
  value=api.platform_random(580);r.quad(8,integer_bits(value-50u));
  value=api.platform_random(20);r.quad(16,m.store(m.sub(m.mul(m.i(value),m.q(constant(0x6d1918))),m.q(constant(0x6d19c0)))));
  auto negative=m.q(r.quad(32));negative.negative=!negative.negative;
  r.quad(24,m.store(m.sub(m.mul(m.mul(negative,m.q(r.quad(32))),m.q(constant(0x6d1908))),m.q(constant(0x6d1900)))));
 }
 for(std::uint32_t i=0;i<5;++i){auto& r=title.objects[i];
  r.quad(8,integer_bits(i*100u+120u));r.quad(16,constant(0x6d1ef0));
  value=api.platform_random(30);r.quad(24,m.store(m.add(m.mul(m.i(value),m.q(constant(0x6d1960))),m.q(constant(0x6d1a40)))));
  r.quad(32,constant(0x6d1ee8));
  value=api.platform_random(314);r.quad(40,integer_bits(static_cast<std::uint32_t>(music_presentation_detail::signed_word(value)/100)));
  value=api.platform_random(120);const auto quotient=static_cast<std::uint32_t>(music_presentation_detail::signed_word(value)/2000);
  r.quad(48,m.store(m.add(m.i(quotient),m.q(constant(0x6d1968)))));r.word(0,0);
 }
 switch_music(0,1,music,menu.outer_scene,game_scene,save.options[1],precision,api);
}
inline void return_and_initialize_menu_title(MenuStartContext& menu,MenuTitleReturnContext& title,StageResourceContext& resources,
 MusicPresentationContext& music,const std::uint32_t& game_scene,const SaveData& save,unsigned precision,MenuTitleReturnServices& api,bool replay_variant=false){
 return_menu_title(menu,title,resources,api,replay_variant);initialize_menu_title(title,menu,music,game_scene,save,precision,api);
}
}
