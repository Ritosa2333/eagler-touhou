// 菜单背景、帮助、FPS与节流及坐标助手，保留原字段与服务调用顺序。
#pragma once
#include "core/frontend/MenuPresentationContext.hpp"
#include "core/frontend/MenuPresentationServices.hpp"
#include "core/frontend/MenuPresentationConstants.hpp"
#include "core/frontend/MenuTitleReturnContext.hpp"
#include "core/frontend/MenuStartContext.hpp"
#include "core/stages/StageResourceContext.hpp"
#include "core/shared/EffectUpdate.hpp"
#include "core/shared/MusicPresentationArithmetic.hpp"
#include "core/math/BinaryFloat32.hpp"
#include <stdexcept>
namespace kaikeidou {
struct MenuPresentationAddressFault:std::out_of_range {
 std::uint32_t address;
 explicit MenuPresentationAddressFault(std::uint32_t a):std::out_of_range("menu timing address outside registered view"),address(a){}
};
namespace menu_presentation_detail {
using Arithmetic=options_motion_detail::Arithmetic;
inline std::int32_t signed32(std::uint32_t n){return static_cast<std::int32_t>(music_presentation_detail::signed_word(n));}
inline std::uint32_t float_bits(std::uint64_t q){
 if((q&0x7ff0000000000000ull)==0x7ff0000000000000ull){const auto fraction=q&0xfffffffffffffull;return std::uint32_t(q>>32)&0x80000000u|0x7f800000u|(fraction?0x400000u|std::uint32_t(fraction>>29):0u);}
 return binary_float::to_binary32(binary_float::binary64(q));
}
inline std::uint64_t fps_value(std::uint64_t average,unsigned precision){
 const auto magnitude=average&0x7fffffffffffffffull;
 if(magnitude>=0x7ff0000000000000ull)return magnitude==0x7ff0000000000000ull?(average&0x8000000000000000ull):(average|0x8000000000000ull);
 const Arithmetic m{precision};return m.store(m.div(m.q(constant(0x6d1e50)),m.q(average)));
}
inline std::uint32_t integer_coordinate(std::uint64_t q){
 if((q&0x7ff0000000000000ull)==0x7ff0000000000000ull)return 0x80000000u;
 return music_presentation_detail::truncate_sse(binary_float::binary64(q));
}
inline std::uint64_t game_coordinate(std::uint64_t q,std::uint64_t shift,std::uint64_t shake,unsigned precision){
 const auto f=float_bits(q);const Arithmetic m{precision};
 if((f&0x7f800000u)!=0x7f800000u&&(shake&0x7ff0000000000000ull)!=0x7ff0000000000000ull)
  return m.store(m.add(m.add(binary_float::finite_binary32(f),m.q(shift)),m.q(shake)));
 // 有限输入转binary32溢出以及非有限坐标沿原FADD传播，保留NaN负号与载荷。
 std::uint64_t left=(f&0x7f800000u)==0x7f800000u?(std::uint64_t(f&0x80000000u)<<32)|0x7ff0000000000000ull|(std::uint64_t(f&0x7fffffu)<<29):0;
 const auto nan=[](std::uint64_t v){return (v&0x7fffffffffffffffull)>0x7ff0000000000000ull;};
 if(nan(left)||nan(shake)){
  if(!nan(left))return shake|0x8000000000000ull;if(!nan(shake))return left|0x8000000000000ull;
  return ((left&0x7fffffffffffffffull)>=(shake&0x7fffffffffffffffull)?left:shake)|0x8000000000000ull;
 }
 if((f&0x7f800000u)!=0x7f800000u)return shake;
 if((shake&0x7fffffffffffffffull)==0x7ff0000000000000ull&&((left^shake)>>63))return 0xfff8000000000000ull;
 return left;
}
}
inline void present_menu_objects(MenuTitleReturnContext& title,const MenuPresentationAssets& assets,unsigned precision,GameMath& math,MenuPresentationServices& api){
 using namespace menu_presentation_detail;const Arithmetic m{precision};api.blend(2,255);
 for(unsigned i=0;i<5;++i){auto& r=title.objects[i];r.quad(40,m.store(m.add(m.q(r.quad(40)),m.q(r.quad(48)))));
  if(r.word(0)==0){r.quad(16,m.store(m.add(m.q(r.quad(16)),m.q(r.quad(24)))));r.quad(24,m.store(m.add(m.q(r.quad(24)),m.q(r.quad(32)))));
   if(crt_sse2::real(r.quad(24))<=0.){r.quad(32,m.store(m.mul(m.q(r.quad(32)),m.q(constant(0x6d18f8)))));r.quad(24,m.store(m.mul(m.q(r.quad(24)),m.q(constant(0x6d18f0)))));if(crt_sse2::real(r.quad(16))<=100.)r.word(0,1);}
  }
  const auto graph=assets.object_graphs[i];const auto angle=m.store(m.div(math.sine(r.quad(40)),m.q(constant(0x6d18e8))));
  const auto y=float_bits(r.quad(16));const auto x=float_bits(r.quad(8));api.draw_rotated_sprite(x,y,0x3ff0000000000000ull,angle,graph,0,0);
 }api.blend(0,255);
}
inline void present_menu_particles(MenuTitleReturnContext& title,const StageResourceContext& resources,unsigned precision,MenuPresentationServices& api){
 using namespace menu_presentation_detail;const Arithmetic m{precision};api.blend(2,160);
 for(auto& r:title.particles){r.quad(0,m.store(m.add(m.q(r.quad(0)),m.q(r.quad(16)))));r.quad(8,m.store(m.add(m.q(r.quad(8)),m.q(r.quad(24)))));r.quad(16,m.store(m.mul(m.q(r.quad(16)),m.q(constant(0x6d1940)))));
  if(crt_sse2::real(r.quad(8))< -50.){r.quad(8,m.store(m.add(m.q(r.quad(8)),m.q(constant(0x6d1930)))));
   auto value=api.platform_random(20);r.quad(32,m.store(m.add(m.mul(m.i(value),m.q(constant(0x6d1928))),m.q(constant(0x6d1920)))));
   value=api.platform_random(740);r.quad(0,m.store(m.i(value-50u)));
   value=api.platform_random(200);r.quad(16,m.store(m.sub(m.mul(m.i(value),m.q(constant(0x6d1918))),m.q(constant(0x6d1910)))));
   auto negative=m.q(r.quad(32));negative.negative=!negative.negative;r.quad(24,m.store(m.sub(m.mul(m.mul(negative,m.q(r.quad(32))),m.q(constant(0x6d1908))),m.q(constant(0x6d1900)))));
  }
  const auto graph=resources.handles[2];const auto scale=r.quad(32);const auto y=float_bits(r.quad(8));const auto x=float_bits(r.quad(0));api.draw_rotated_sprite(x,y,scale,0,graph,0,0);
 }api.blend(0,255);
}
inline void present_menu_strips(const MenuTitleReturnContext& title,const StageResourceContext& resources,MenuPresentationServices& api){
 using namespace menu_presentation_detail;api.draw_graph(0,0,resources.handles[0],0);api.blend(2,96);
 for(const auto& r:title.strips){api.brightness(r.word(16),r.word(20),r.word(24));const auto graph=resources.handles[1];const auto y=float_bits(r.quad(8));const auto x=float_bits(r.quad(0));api.draw_rotated_sprite(x,y,constant(0x6d18e8),0,graph,0,0);}
 api.brightness(254,254,254);api.blend(0,255);
}
inline void present_menu_help(std::string_view first,std::string_view second,const MenuPresentationAssets& assets,MenuPresentationServices& api){
 api.blend(2,255);auto font=assets.help_fonts[1];auto ink=api.color(0,255,0);api.draw_font_text(20,30,first,ink,font,0,0);
 font=assets.help_fonts[0];ink=api.color(96,0,0);api.draw_font_text(10,10,second,ink,font,0,0);api.blend(0,255);
}
inline std::uint32_t present_menu_fps(const MenuPresentationTiming& timing,const MenuTitleReturnContext& title,std::uint32_t x,std::uint32_t y,std::uint32_t entry_ecx,unsigned precision,MenuPresentationServices& api,bool replay_variant=false){
 using namespace menu_presentation_detail;const auto average=timing.data.quad(8);if((average&0x7fffffffffffffffull)==0)return entry_ecx;
 const auto number=fps_value(average,precision);auto ink=api.color(255,255,255);auto result=api.draw_formatted(x,y,ink,text(0x6cf830),true,number);
 if(replay_variant&&title.replay_mode==1){ink=api.color(255,255,255);result=api.draw_formatted(437,464,ink,text(0xb411ce),false,0);}
 return result.ecx;
}
inline std::uint32_t throttle_menu_frame(MenuPresentationTiming& timing,unsigned precision,MenuPresentationServices& api,std::array<std::uint32_t,256>* keyboard_counts=nullptr){
 using namespace menu_presentation_detail;const Arithmetic m{precision};auto& data=timing.data;std::uint32_t wait=0;
 if(data.word(0)==0){if(timing.previous_tick!=0){const auto target=data.word(4)+1000u;wait=target-api.now_count(0);}}
 else{const auto target=music_presentation_detail::truncate_sse(m.add(m.mul(m.i(data.word(0)),m.q(constant(0x6d1e58))),m.i(data.word(4))));wait=target-api.now_count(0);}
 if(signed32(wait)>0)api.sleep_ms(wait);
 const auto now=api.now_count(0);if(data.word(0)==0)data.word(4,now);
 const auto interval=now-timing.previous_tick;const auto address=0xa71d18u+data.word(0)*4u;
 const auto offset=address-0xa71d08u;
 if(offset<=252u)data.word(offset,interval);
 else if(address-0xa71e08u<1024u&&keyboard_counts)(*keyboard_counts)[(address-0xa71e08u)/4u]=interval;
 else throw MenuPresentationAddressFault(address);
 timing.previous_tick=now;
 if(data.word(0)==59){data.quad(8,0);for(unsigned i=0;i<60;++i)data.quad(8,m.store(m.add(m.i(data.word(16+i*4)),m.q(data.quad(8)))));data.quad(8,m.store(m.div(m.q(data.quad(8)),m.q(constant(0x6d1aa8)))));}
 data.word(0,data.word(0)+1u);data.word(0,static_cast<std::uint32_t>(signed32(data.word(0))%60));return 60;
}
inline void present_menu_centered_text(std::uint64_t x,std::uint64_t y,std::string_view text,std::uint32_t ink,std::uint32_t font,MenuPresentationServices& api){
 using namespace menu_presentation_detail;const auto iy=integer_coordinate(y);const auto ix=integer_coordinate(x);
 const auto width=api.text_width(font,text);const auto half=static_cast<std::uint32_t>(signed32(width)/2);api.draw_font_text(signed32(ix-half),signed32(iy),text,ink,font,0,0);
}
inline void present_menu_sprite_double(std::uint64_t x,std::uint64_t y,std::uint64_t scale,std::uint64_t angle,std::uint32_t graph,std::uint32_t transparent,std::uint32_t turn,MenuPresentationServices& api){
 const auto fy=menu_presentation_detail::float_bits(y);const auto fx=menu_presentation_detail::float_bits(x);api.draw_rotated_sprite(fx,fy,scale,angle,graph,transparent,turn);
}
inline void present_menu_sprite_game(std::uint64_t x,std::uint64_t y,std::uint64_t scale,std::uint64_t angle,std::uint32_t graph,std::uint32_t transparent,std::uint32_t turn,const EffectShake& shake,unsigned precision,MenuPresentationServices& api){
 using namespace menu_presentation_detail;const auto yy=game_coordinate(y,constant(0x6d1e20),shake.quad(24),precision);
 const auto xx=game_coordinate(x,constant(0x6d1e48),shake.quad(16),precision);present_menu_sprite_double(xx,yy,scale,angle,graph,transparent,turn,api);
}
inline void present_menu_wrapper(std::uint32_t entry,MenuStartContext& menu,MenuTitleReturnContext& title,const StageResourceContext& resources,const MenuPresentationAssets& assets,MenuPresentationTiming& timing,unsigned precision,MenuPresentationServices& api,bool replay_variant=false,std::array<std::uint32_t,256>* keyboard_counts=nullptr){
 using namespace menu_presentation_detail;if(entry!=0x5e9fa0&&entry!=0x5ea470&&entry!=0x5ea8a0)throw std::out_of_range("menu presentation wrapper entry");
 present_menu_strips(title,resources,api);present_menu_particles(title,resources,precision,api);
 if(entry==0x5e9fa0){if(replay_variant)api.replay_ui_route();else{present_menu_help(text(0x6c7a90),text(0x6c7a88),assets,api);api.menu_body(0x5e9830);}}
 else if(entry==0x5ea470){api.menu_body(0x5e9fe0);if(menu.phase==2)present_menu_help(text(0x6c7a48),text(0x6c7a3c),assets,api);else present_menu_help(text(0x6c7a64),text(0x6c7a58),assets,api);}
 else{api.menu_body(0x5ea4d0);present_menu_help(text(0x6c7a08),text(0x6c79fc),assets,api);}
 (void)present_menu_fps(timing,title,572,465,0,precision,api,replay_variant);(void)throttle_menu_frame(timing,precision,api,keyboard_counts);
}
}
