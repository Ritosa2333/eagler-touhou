// scene0真实初始化链；平台返回和输出数组由宿主提供，原游戏字段在同一地址视图同步。
#pragma once
#include "core/frontend/MenuBootstrap.hpp"
#include "core/frontend/MenuBootstrapBindings.hpp"
#include "core/frontend/MenuBootstrapPlatform.hpp"
#include "core/frontend/MenuTitleReturn.hpp"
#include "core/frontend/MenuScenePeripheral.hpp"
#include "core/frontend/MenuOptions.hpp"
#include <type_traits>
namespace kaikeidou {
class MenuBootstrapRuntime {
 using U=std::uint32_t;
 GameStartEntryRefs game_;MenuSceneRuntimeContext& menu_;MenuBootstrapPlatform& platform_;SaveIO& save_;
 unsigned precision_;bool replay_variant_;GameStartEntryMemory memory_;bool sealed_=false;
 MenuBootstrapRegisters last_;U current_pc_=0;bool platform_fault_=false;
 void ready(){if(!sealed_)throw std::logic_error("Menu bootstrap initial state must be sealed");memory_.reconcile();}
 template<class F>auto synchronized(F f){memory_.reconcile();if constexpr(std::is_void_v<decltype(f())>){f();memory_.reconcile();}else{auto value=f();memory_.reconcile();return value;}}
 MenuBootstrapPlatformResult invoke(MenuBootstrapRequest request){
  current_pc_=request.call_site?request.call_site:static_cast<U>(request.api);memory_.reconcile();auto result=platform_.invoke(request);memory_.reconcile();
  for(const auto& w:result.writes){try{memory_.write(w.address,w.bytes);}catch(const GameStartEntryAccessFault&){platform_fault_=true;throw;}}
  last_={result.eax,result.ecx,result.edx};return result;
 }
 MenuBootstrapPlatformResult api(MenuBootstrapApi kind,std::vector<U> words={},std::vector<std::string_view> texts={},U site=0){return invoke(MenuBootstrapRequest{kind,std::move(words),std::move(texts),site,0,0,{}});}
 struct IO final:SaveIO {MenuBootstrapRuntime& owner;explicit IO(MenuBootstrapRuntime& o):owner(o){}
  bool open(std::string_view p,bool w)override{return owner.synchronized([&]{return owner.save_.open(p,w);});}
  std::size_t read(std::uint8_t* p,std::size_t n)override{return owner.synchronized([&]{return owner.save_.read(p,n);});}
  void write(const std::uint8_t* p,std::size_t n)override{owner.synchronized([&]{owner.save_.write(p,n);});}
  void close()override{owner.synchronized([&]{owner.save_.close();});}
 } io_;
 struct Volumes final:SaveVolumes {MenuBootstrapRuntime& owner;explicit Volumes(MenuBootstrapRuntime& o):owner(o){}
  void bgm(U value)override{const auto handle=owner.game_.music.audio_handle;owner.api(MenuBootstrapApi::set_volume,{music_presentation_detail::volume(value,owner.precision_),handle});}
  void sound(U value)override{const auto adjusted=music_presentation_detail::volume(value,owner.precision_);for(U i=0;i<100;++i)owner.api(MenuBootstrapApi::set_volume,{adjusted,owner.menu_.sound_handles[i]});}
 } volumes_;
 struct TitleServices final:MenuTitleReturnServices {MenuBootstrapRuntime& owner;explicit TitleServices(MenuBootstrapRuntime& o):owner(o){}
  std::int32_t delete_graph(U h,U f)override{return static_cast<std::int32_t>(owner.api(MenuBootstrapApi::delete_graph,{h,f}).eax);}
  U load_graph(std::string_view p,U f)override{return owner.api(MenuBootstrapApi::load_graph,{f},{p}).eax;}
  U platform_random(U n)override{return owner.api(MenuBootstrapApi::platform_random,{n}).eax;}
  std::int32_t movie_seek(U h,U p)override{return static_cast<std::int32_t>(owner.api(MenuBootstrapApi::movie_seek,{h,p}).eax);}
  std::int32_t movie_play(U h,U m,U f)override{return static_cast<std::int32_t>(owner.api(MenuBootstrapApi::movie_play,{h,m,f}).eax);}
  std::int32_t set_create_sound_data_type(U n)override{return static_cast<std::int32_t>(owner.api(MenuBootstrapApi::set_sound_type,{n}).eax);}
  std::int32_t stop_sound(U h)override{return static_cast<std::int32_t>(owner.api(MenuBootstrapApi::stop_sound,{h}).eax);}
  std::int32_t delete_sound(U h,U f)override{return static_cast<std::int32_t>(owner.api(MenuBootstrapApi::delete_sound,{h,f}).eax);}
  std::int32_t load_sound(std::string_view p,U n,U h)override{return static_cast<std::int32_t>(owner.api(MenuBootstrapApi::load_sound,{n,h},{p}).eax);}
  std::int32_t set_volume(U v,U h)override{return static_cast<std::int32_t>(owner.api(MenuBootstrapApi::set_volume,{v,h}).eax);}
  std::int32_t play_sound(U h,U t,U p)override{return static_cast<std::int32_t>(owner.api(MenuBootstrapApi::play_sound,{h,t,p}).eax);}
 } title_api_;
 void load_save(){current_pc_=0x5e50e0;memory_.reconcile();load_savedata(game_.savedata,io_,volumes_);memory_.reconcile();}
public:
 MenuBootstrapRuntime(GameStartEntryRefs game,GameStartEntryGaps& game_gaps,GameStartEntryReplayContext& replay,
  MenuSceneRuntimeContext& menu,CollisionContext& collision,EndingCreditsAssets& ending,MenuBootstrapContext& context,
  MenuBootstrapGaps& gaps,bool replay_variant,unsigned precision,MenuBootstrapPlatform& platform,const GameStartEntryMemoryExtensions& extra={}):
  game_(game),menu_(menu),platform_(platform),save_(platform.savedata_io()),precision_(precision),replay_variant_(replay_variant),memory_(game.random),io_(*this),volumes_(*this),title_api_(*this){
  bind_menu_bootstrap(memory_,game_,game_gaps,replay,menu,collision,ending,context,gaps,replay_variant,extra);
 }
 GameStartEntryMemory& memory(){return memory_;}
 void seal(){memory_.seal();sealed_=true;}
 MenuBootstrapRegisters last_registers()const{return last_;}
 void initialize_resources(U sound_flag){ready();for(const auto& op:menu_bootstrap_detail::asset_operations){
  if(op.api==static_cast<U>(MenuBootstrapApi::load_sound)&&sound_flag==0)continue;
  MenuBootstrapRequest request{static_cast<MenuBootstrapApi>(op.api),std::vector<U>(op.words.begin(),op.words.begin()+op.count),{},op.site,op.text_address,op.format_address,std::vector<U>(op.format_words.begin(),op.format_words.begin()+op.format_count)};
  if(!op.text.empty())request.texts.push_back(op.text);const auto result=invoke(std::move(request));
  if(op.destination){current_pc_=op.store_site;memory_.word(op.destination,result.eax);}
 }}
 void initialize_random_table(){ready();current_pc_=0x653540;initialize_menu_bootstrap_random_table(memory_);}
 void initialize_config(){ready();load_save();current_pc_=0x656f58;initialize_menu_bootstrap_config_fields(memory_,precision_);}
 MenuBootstrapOutcome run_scene_zero(){ready();platform_fault_=false;MenuBootstrapOutcome result;
  try{
   api(MenuBootstrapApi::system_flag,{1},{},0x6532c7);load_save();initialize_resources(1);initialize_random_table();initialize_config();
   current_pc_=0x5e9800;memory_.reconcile();return_menu_title(game_.menu,menu_.title,game_.resources,title_api_,false);memory_.reconcile();
   current_pc_=0x5e9490;initialize_menu_title(menu_.title,game_.menu,game_.music,game_.lifecycle.scene,game_.savedata,precision_,title_api_);memory_.reconcile();
   draw_menu_scene_footer(2,2,false,replay_variant_,[&](const MenuSceneRequest& r){return api(static_cast<MenuBootstrapApi>(r.api),r.words,r.texts);});
  }catch(const MenuBootstrapFault& fault){result.kind=MenuBootstrapOutcomeKind::memory_fault;result.fault_pc=fault.pc;result.fault_address=fault.address;result.fault_bytes=fault.bytes;result.write_fault=fault.writing;}
  catch(const GameStartEntryAccessFault& fault){result.kind=platform_fault_?MenuBootstrapOutcomeKind::platform_output_fault:MenuBootstrapOutcomeKind::memory_fault;result.fault_pc=current_pc_;result.fault_address=fault.address;result.fault_bytes=fault.bytes;result.write_fault=fault.writing;}
  result.registers=last_;return result;
 }
};
}
