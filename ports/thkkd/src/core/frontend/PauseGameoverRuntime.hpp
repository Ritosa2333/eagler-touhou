// 与已恢复启动、标题、暂停门、音乐和显示分派组合；仅引用已有生产状态。
#pragma once
#include "core/frontend/PauseGameover.hpp"
#include "core/runtime/GameStartEntry.hpp"
#include "core/frontend/ReplayPauseEntry.hpp"
#include "core/frontend/MenuTitleReturn.hpp"
#include "core/runtime/GameplayPresentationDispatch.hpp"
#include <type_traits>
namespace kaikeidou {
struct PauseGameoverOverlayServices {
 virtual ~PauseGameoverOverlayServices()=default;
 virtual std::int32_t physical_input_count(std::uint32_t binding)=0;
 virtual std::uint32_t color(std::uint32_t r,std::uint32_t g,std::uint32_t b)=0;
 virtual void blend(std::uint32_t mode,std::uint32_t alpha)=0;
 virtual void rectangle(std::int32_t l,std::int32_t t,std::int32_t r,std::int32_t b,std::uint32_t ink,std::uint32_t filled)=0;
 virtual void draw_font_text(std::int32_t x,std::int32_t y,std::string_view text,std::uint32_t ink,std::uint32_t font,std::uint32_t edge,std::uint32_t vertical)=0;
};
struct PauseGameoverHost {
 StageBackgroundServices& background;MusicPresentationServices& audio;GameStartEntryServices& start;
 GameplayPresentationDispatchServices& gameplay;PauseGameoverOverlayServices& overlay;SaveIO& save;
};
struct PauseGameoverOutcome {
 std::uint32_t result=0;bool memory_fault=false;
 std::uint32_t fault_address=0,fault_bytes=0;bool writing=false;
 PauseGameoverAbi abi;
};
inline GameStartEntryMemoryExtensions pause_gameover_extensions(GameStartEntryRefs& refs,MenuTitleReturnContext& title,PauseGameoverContext& context,GameStartEntryMemoryExtensions extras={}){
 const auto add=[&](std::uint32_t a,void* p,std::size_t n){extras.push_back({a,p,n});};
 add(0xa74bdc,&context.font,4);add(0xa74f48,&refs.gameplay.slow_graph,4);
 add(0xa773e0,title.strips.data(),160);add(0xa785f8,title.particles.data(),2000);add(0xa73118,title.objects.data(),280);
 add(0xb4310c,&title.replay_mode,4);add(0xb43a00,&title.save_disabled,4);add(0xb4303e,&title.save_choice,4);
 add(0xb43c00,&title.eof_flags,4);add(0xb43c00,&refs.pause.eof_flags,4);add(0xb43c04,&title.accumulator,4);add(0xb43056,title.prompt.data.data(),8);
 return extras;
}
class PauseGameoverRuntime final:public PauseGameoverServices {
 using U=std::uint32_t;
 GameStartEntryRefs refs_;GameStartEntryGaps& gaps_;GameStartEntryReplayContext& replay_;MenuTitleReturnContext& title_;
 PauseGameoverHost host_;GameMath& math_;unsigned precision_;bool replay_variant_;
 GameStartEntryMemoryExtensions extensions_;GameStartEntryMemory memory_;
 bool starting_=false,movie_called_=false;U last_movie_eax_=0;
 template<class F>auto call(F action){memory_.reconcile();if constexpr(std::is_void_v<decltype(action())>){action();memory_.reconcile();}else{auto value=action();memory_.reconcile();return value;}}
 struct Api final:MenuTitleReturnServices,ReplayPauseEntryServices,GameStartEntryServices {
  PauseGameoverRuntime& owner;explicit Api(PauseGameoverRuntime& x):owner(x){}
  std::int32_t delete_graph(U h,U f)override{return owner.call([&]{return owner.starting_?owner.host_.start.delete_graph(h,f):owner.host_.background.delete_graph(h,f);});}
  U load_graph(std::string_view p,U f)override{return owner.call([&]{return owner.starting_?owner.host_.start.load_graph(p,f):owner.host_.background.load_graph(p,f);});}
  U platform_random(U n)override{return owner.call([&]{return owner.starting_?owner.host_.start.platform_random(n):owner.host_.background.platform_random(n);});}
  std::int32_t movie_seek(U h,U p)override{const auto value=owner.call([&]{return owner.starting_?owner.host_.start.movie_seek(h,p):owner.host_.background.movie_seek(h,p);});if(owner.starting_){owner.movie_called_=true;owner.last_movie_eax_=static_cast<U>(value);}return value;}
  std::int32_t movie_play(U h,U m,U f)override{const auto value=owner.call([&]{return owner.starting_?owner.host_.start.movie_play(h,m,f):owner.host_.background.movie_play(h,m,f);});if(owner.starting_){owner.movie_called_=true;owner.last_movie_eax_=static_cast<U>(value);}return value;}
  std::int32_t movie_pause(U h,U f)override{return owner.call([&]{return owner.host_.background.movie_pause(h,f);});}
  std::int32_t set_viewport_matrix(const std::array<U,16>& m)override{return owner.call([&]{return owner.host_.background.set_viewport_matrix(m);});}
  std::int32_t set_create_sound_data_type(U n)override{return owner.call([&]{return owner.host_.audio.set_create_sound_data_type(n);});}
  std::int32_t stop_sound(U h)override{return owner.call([&]{return owner.host_.audio.stop_sound(h);});}
  std::int32_t delete_sound(U h,U f)override{return owner.call([&]{return owner.host_.audio.delete_sound(h,f);});}
  std::int32_t load_sound(std::string_view p,U n,U h)override{return owner.call([&]{return owner.host_.audio.load_sound(p,n,h);});}
  std::int32_t set_volume(U v,U h)override{return owner.call([&]{return owner.host_.audio.set_volume(v,h);});}
  std::int32_t play_sound(U h,U t,U p)override{return owner.call([&]{return owner.host_.audio.play_sound(h,t,p);});}
  std::int32_t physical_input_count(U b)override{return owner.physical_input_count(b);}
  void draw_pause_interface()override{owner.present_gameplay();}
  GameStartEntryPlatformResult platform(GameStartEntryPlatformApi api,const std::vector<U>& args)override{return owner.call([&]{return owner.host_.start.platform(api,args);});}
 } api_;
 struct Display final:GameplayPresentationDispatchServices {
  PauseGameoverRuntime& owner;explicit Display(PauseGameoverRuntime& x):owner(x){}
  void blend(U m,U a)override{owner.call([&]{owner.host_.gameplay.blend(m,a);});}
  U color(U r,U g,U b)override{return owner.call([&]{return owner.host_.gameplay.color(r,g,b);});}
  void rectangle(U l,U t,U r,U b,U c,U f)override{owner.call([&]{owner.host_.gameplay.rectangle(l,t,r,b,c,f);});}
  void tint_color(U r,U g,U b)override{owner.call([&]{owner.host_.gameplay.tint_color(r,g,b);});}
  void present_component(GameplayPresentationComponent c)override{owner.call([&]{owner.host_.gameplay.present_component(c);});}
  std::int32_t input_count(U b)override{return owner.call([&]{return owner.host_.gameplay.input_count(b);});}
  void draw_slow_marker(std::uint64_t x,std::uint64_t y,std::uint64_t s,std::uint64_t a,U g,U t,U r)override{owner.call([&]{owner.host_.gameplay.draw_slow_marker(x,y,s,a,g,t,r);});}
  void draw_fps(U x,U y)override{owner.call([&]{owner.host_.gameplay.draw_fps(x,y);});}
 } display_;
 struct IO final:SaveIO {
  PauseGameoverRuntime& owner;explicit IO(PauseGameoverRuntime& x):owner(x){}
  bool open(std::string_view p,bool w)override{return owner.call([&]{return owner.host_.save.open(p,w);});}
  std::size_t read(std::uint8_t* p,std::size_t n)override{return owner.call([&]{return owner.host_.save.read(p,n);});}
  void write(const std::uint8_t* p,std::size_t n)override{owner.call([&]{owner.host_.save.write(p,n);});}
  void close()override{owner.call([&]{owner.host_.save.close();});}
 } io_;
public:
 PauseGameoverRuntime(GameStartEntryRefs refs,GameStartEntryGaps& gaps,GameStartEntryReplayContext& replay,MenuTitleReturnContext& title,PauseGameoverContext& context,PauseGameoverHost host,GameMath& math,unsigned precision,bool replay_variant,GameStartEntryMemoryExtensions extensions={}):
 refs_(refs),gaps_(gaps),replay_(replay),title_(title),host_(host),math_(math),precision_(precision),replay_variant_(replay_variant),extensions_(pause_gameover_extensions(refs_,title,context,std::move(extensions))),memory_(refs_.random),api_(*this),display_(*this),io_(*this){
  bind_game_start_entry(memory_,refs_,gaps_,replay_,replay_variant_);for(const auto& x:extensions_)memory_.bind_bytes(x.address,x.data,x.size);memory_.seal();
 }
 GameStartEntryMemory& memory(){return memory_;}
 bool enter_scene_zero()override{memory_.reconcile();const auto result=enter_pause_from_scene_zero(refs_.pause,replay_variant_,refs_.resources,refs_.music,static_cast<ReplayPauseEntryServices&>(api_));memory_.reconcile();return result;}
 std::int32_t physical_input_count(U binding)override{return call([&]{return host_.overlay.physical_input_count(binding);});}
 void pause_background()override{memory_.reconcile();pause_stage_background(refs_.resources,static_cast<ReplayPauseEntryServices&>(api_));memory_.reconcile();}
 void resume_background(U mode)override{memory_.reconcile();control_stage_movies(mode,refs_.resources,static_cast<GameStartEntryServices&>(api_));memory_.reconcile();}
 void resume_music(U track,U mode)override{memory_.reconcile();switch_music(track,mode,refs_.music,refs_.menu.outer_scene,refs_.lifecycle.scene,refs_.savedata.options[1],precision_,static_cast<MenuTitleReturnServices&>(api_));memory_.reconcile();}
 void present_gameplay()override{memory_.reconcile();dispatch_gameplay_presentation(refs_.gameplay,refs_.runtime.ending,display_);memory_.reconcile();}
 U color(U r,U g,U b)override{return call([&]{return host_.overlay.color(r,g,b);});}
 void blend(U mode,U alpha)override{call([&]{host_.overlay.blend(mode,alpha);});}
 void rectangle(std::int32_t l,std::int32_t t,std::int32_t r,std::int32_t b,U ink,U filled)override{call([&]{host_.overlay.rectangle(l,t,r,b,ink,filled);});}
 void draw_font_text(std::int32_t x,std::int32_t y,std::string_view text,U ink,U font,U edge,U vertical)override{call([&]{host_.overlay.draw_font_text(x,y,text,ink,font,edge,vertical);});}
 U initialize_game(U parameter)override{memory_.reconcile();starting_=true;movie_called_=false;try{kaikeidou::initialize_game_start(refs_,gaps_,replay_,parameter,replay_variant_,precision_,math_,static_cast<GameStartEntryServices&>(api_),extensions_);}catch(...){starting_=false;throw;}starting_=false;memory_.reconcile();return movie_called_?last_movie_eax_:memory_.word(0xb3e2ec);}
 void return_title()override{memory_.reconcile();return_menu_title(refs_.menu,title_,refs_.resources,static_cast<MenuTitleReturnServices&>(api_),replay_variant_);memory_.reconcile();}
 void initialize_title()override{memory_.reconcile();initialize_menu_title(title_,refs_.menu,refs_.music,refs_.lifecycle.scene,refs_.savedata,precision_,static_cast<MenuTitleReturnServices&>(api_));memory_.reconcile();}
 void store_scores_settings()override{memory_.reconcile();store_scores_and_settings(refs_.savedata,io_);memory_.reconcile();}
 PauseGameoverOutcome update(PauseGameoverAbi abi={}){PauseGameoverOutcome result;memory_.reconcile();abi.instruction=0;try{result.result=update_pause_gameover(memory_,*this,replay_variant_,abi);}catch(const GameStartEntryAccessFault& fault){result.memory_fault=true;result.fault_address=fault.address;result.fault_bytes=fault.bytes;result.writing=fault.writing||abi.instruction==0xb42948||abi.instruction==0xb4294a;}result.abi=abi;return result;}
};
}
