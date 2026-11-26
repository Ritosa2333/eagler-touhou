// scene101/102将既有绘图坐标、音乐与标题模块组合到同一原地址视图。
#pragma once
#include "core/frontend/EndingCredits.hpp"
#include "core/frontend/EndingCreditsAssets.hpp"
#include "core/runtime/GameStartEntryMemory.hpp"
#include "core/frontend/MenuPresentation.hpp"
#include "core/frontend/MenuTitleReturn.hpp"
#include "core/frontend/MenuSceneReplayInputLeaf.hpp"
#include <type_traits>
namespace kaikeidou {
struct EndingCreditsPlatformRequest {std::uint32_t api;std::vector<std::uint32_t> words;std::vector<std::string_view> texts;};
struct EndingCreditsPlatformWrite {std::uint32_t address;std::vector<std::uint8_t> bytes;};
struct EndingCreditsPlatformResult {std::uint32_t eax=0,ecx=0,edx=0;std::vector<EndingCreditsPlatformWrite> writes;};
struct EndingCreditsPlatform {
 virtual ~EndingCreditsPlatform()=default;
 virtual EndingCreditsPlatformResult invoke(const EndingCreditsPlatformRequest& request)=0;
 // 返回原CALL已经压入返回地址和参数后的真实边界帧；其栈须预先绑定到memory。
 virtual GameStartEntryMachineFrame& replay_input_frame(std::uint32_t caller,std::uint32_t binding)=0;
 virtual void observe_input_leaf(std::uint32_t,const GameStartEntryMachineFrame&,const MenuSceneInputLeafOutcome&){}
 virtual void observe_text_copy(std::uint32_t,std::uint32_t){}
};
template<class Memory>void bind_ending_credits_title(Memory& memory,MenuTitleReturnContext& title){
 memory.bind(0xa773e0,title.strips);memory.bind(0xa785f8,title.particles);memory.bind(0xa73118,title.objects);
 memory.bind(0xb4310c,title.replay_mode);memory.bind(0xb43a00,title.save_disabled);memory.bind(0xb4303e,title.save_choice);
 memory.bind(0xb43c00,title.eof_flags);memory.bind(0xb43c04,title.accumulator);memory.bind(0xb43056,title.prompt.data);
}
// 常规GameStartEntry绑定、EndingAssets、title、输入位图/物理计数和叶栈由宿主共同seal。
class EndingCreditsRuntime:private EndingCreditsServices {
 using U=std::uint32_t;using Q=std::uint64_t;
 GameStartEntryMemory& memory_;GameStartEntryRefs game_;MenuTitleReturnContext& title_;EndingCreditsPlatform& host_;unsigned precision_;bool replay_;U input_caller_=0;
 template<class F>auto synchronized(F&& f){memory_.reconcile();try{if constexpr(std::is_void_v<decltype(f())>){f();memory_.reconcile();}else{auto out=f();memory_.reconcile();return out;}}catch(...){memory_.reconcile();throw;}}
 EndingCreditsPlatformResult api(U which,std::vector<U> words={},std::vector<std::string_view> texts={}){
  memory_.reconcile();auto out=host_.invoke({which,std::move(words),std::move(texts)});memory_.reconcile();for(const auto& write:out.writes)memory_.write(write.address,write.bytes);memory_.reconcile();return out;
 }
 static std::int32_t signed32(U n){return static_cast<std::int32_t>(ending_credits_detail::signed_word(n));}
 std::string text(U address){std::string out;for(U n=0;n<0x100000;++n){auto b=memory_.byte(address+n);if(!b)return out;out.push_back(static_cast<char>(b));}throw GameStartEntryAccessFault{address,0x100000,false};}
 struct Stage:MenuTitleReturnServices {
  EndingCreditsRuntime& r;explicit Stage(EndingCreditsRuntime& v):r(v){}
  std::int32_t delete_graph(U h,U f)override{return signed32(r.api(0x403e20,{h,f}).eax);}
  U load_graph(std::string_view p,U f)override{return r.api(0x43c090,{f},{p}).eax;}
  U platform_random(U n)override{return r.api(0x466760,{n}).eax;}
  std::int32_t movie_seek(U h,U n)override{return signed32(r.api(0x406770,{h,n}).eax);}
  std::int32_t movie_play(U h,U m,U f)override{return signed32(r.api(0x406690,{h,m,f}).eax);}
  std::int32_t set_create_sound_data_type(U n)override{return signed32(r.api(0x43e0d0,{n}).eax);}
  std::int32_t stop_sound(U h)override{return signed32(r.api(0x440240,{h}).eax);}
  std::int32_t delete_sound(U h,U f)override{return signed32(r.api(0x43e010,{h,f}).eax);}
  std::int32_t load_sound(std::string_view p,U n,U h)override{return signed32(r.api(0x4434d0,{n,h},{p}).eax);}
  std::int32_t set_volume(U v,U h)override{return signed32(r.api(0x43fd40,{v,h}).eax);}
  std::int32_t play_sound(U h,U m,U f)override{return signed32(r.api(0x4425f0,{h,m,f}).eax);}
 }stage_;
 struct Front:MenuPresentationServices {
  EndingCreditsRuntime& r;explicit Front(EndingCreditsRuntime& v):r(v){}
  std::int32_t input_count(U b)override{return signed32(r.input_counter(b,r.replay_));}
  U color(U a,U b,U c)override{return r.color(a,b,c);}void blend(U a,U b)override{r.blend(a,b);}
  void draw_font_text(std::int32_t x,std::int32_t y,std::string_view t,U ink,U font,U edge,U vertical)override{r.api(0x437c90,{U(x),U(y),ink,font,edge,vertical},{t});}
  void draw_rotated_sprite(U x,U y,Q z,Q a,U g,U t,U turn)override{r.api(0x41c910,{x,y,U(z),U(z>>32),U(a),U(a>>32),g,t,turn});}
  void change_stage_resources(std::int32_t)override{throw std::logic_error("Ending unexpected resource dispatcher");}
  void present_component(MenuStartComponent)override{throw std::logic_error("Ending unexpected menu component");}
  void draw_help(std::string_view,std::string_view)override{throw std::logic_error("Ending unexpected help");}
  void move_cursor(U,U)override{throw std::logic_error("Ending unexpected cursor");}void present_cursor()override{throw std::logic_error("Ending unexpected frame limit");}
  void brightness(U a,U b,U c)override{r.api(0x4048a0,{a,b,c});}
  void draw_graph(U x,U y,U g,U t)override{r.api(0x41bb70,{x,y,g,t});}
  U platform_random(U n)override{return r.stage_.platform_random(n);}
  U text_width(U f,std::string_view t)override{return r.api(0x437c20,{f},{t}).eax;}
  MenuPresentationRegisters draw_formatted(U x,U y,U ink,std::string_view f,bool n,Q value)override{std::vector<U> args{x,y,ink};if(n){args.push_back(U(value));args.push_back(U(value>>32));}auto out=r.api(0x437f00,std::move(args),{f});return {out.eax,out.ecx};}
  U now_count(U n)override{return r.api(0x437fd0,{n}).eax;}void sleep_ms(U n)override{r.api(0x69722c,{n});}
  void menu_body(U)override{throw std::logic_error("Ending unexpected menu body");}void replay_ui_route()override{throw std::logic_error("Ending unexpected Replay UI");}
 }front_;
 U input_counter(U binding,bool replay)override{
  if(!replay)return api(0x656c70,{binding}).eax;
  memory_.reconcile();auto& frame=host_.replay_input_frame(input_caller_,binding);memory_.reconcile();
  if(memory_.word(frame.registers.esp)!=input_caller_+5u||memory_.word(frame.registers.esp+4u)!=binding)throw std::logic_error("Ending input caller frame mismatch");
  const auto out=execute_menu_scene_replay_input_leaf(memory_,frame);synchronized([&]{host_.observe_input_leaf(input_caller_,frame,out);});
  if(out.kind!=MenuSceneInputLeafKind::returned)throw out;if(out.target!=input_caller_+5u)throw std::logic_error("Ending input return address changed");return out.value;
 }
 U color(U r,U g,U b)override{return api(0x4049a0,{r,g,b}).eax;}
 void blend(U m,U a)override{api(0x404830,{m,a});}
 void rectangle(U l,U t,U r,U b,U c,U f)override{api(0x421760,{l,t,r,b,c,f});}
 void sprite_game(Q x,Q y,Q z,Q a,U g,U t,U turn)override{synchronized([&]{present_menu_sprite_game(x,y,z,a,g,t,turn,game_.shake,precision_,front_);});}
 void draw_text(U x,U y,U address,U ink,U font,U edge,U vertical)override{memory_.reconcile();auto value=text(address);api(0x437c90,{x,y,ink,font,edge,vertical},{value});}
 U load_graph(std::string_view p,U f)override{return stage_.load_graph(p,f);}
 void music(U t,U m)override{synchronized([&]{switch_music(t,m,game_.music,game_.menu.outer_scene,game_.lifecycle.scene,game_.savedata.options[1],precision_,stage_);});}
 void stop_music()override{stage_.stop_sound(game_.music.audio_handle);}
 void return_title(bool replay)override{synchronized([&]{return_menu_title(game_.menu,title_,game_.resources,stage_,replay);});}
 void initialize_title()override{synchronized([&]{initialize_menu_title(title_,game_.menu,game_.music,game_.lifecycle.scene,game_.savedata,precision_,stage_);});}
 void observe_text_copy(U d,U s)override{synchronized([&]{host_.observe_text_copy(d,s);});}
public:
 EndingCreditsRuntime(GameStartEntryMemory& memory,GameStartEntryRefs game,MenuTitleReturnContext& title,EndingCreditsPlatform& host,bool replay,unsigned precision):memory_(memory),game_(game),title_(title),host_(host),precision_(precision),replay_(replay),stage_(*this),front_(*this){if(precision!=24&&precision!=53&&precision!=64)throw std::invalid_argument("Ending precision");}
 void ending(){input_caller_=0x669ddc;synchronized([&]{update_ending(memory_,*this,replay_);});}
 void credits(){input_caller_=0x66ccd1;synchronized([&]{update_credits(memory_,*this,precision_,replay_);});}
};
}
