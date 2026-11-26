// 菜单组件的真实组合与平台适配；冻结主体通过各自既有接口调用。
#pragma once
#include "core/frontend/MenuStart.hpp"
#include "core/frontend/MenuCharacterPractice.hpp"
#include "core/frontend/MenuOptions.hpp"
#include "core/frontend/MenuKeyConfig.hpp"
#include "core/frontend/MenuMusic.hpp"
#include "core/frontend/MenuTitleReturn.hpp"
#include "core/frontend/MenuPresentation.hpp"
#include "core/frontend/MenuReplay.hpp"
#include "core/runtime/GameStartEntry.hpp"
#include "core/frontend/MenuSceneInput.hpp"
#include "core/frontend/MenuScenePeripheral.hpp"
#include <type_traits>
#include <string>
namespace kaikeidou {
struct MenuSceneRuntime::Impl {
 using U=std::uint32_t;using Q=std::uint64_t;
 GameStartEntryRefs game;GameStartEntryGaps& gaps;GameStartEntryReplayContext& replay;GameStartEntryMachineFrame& frame;
 MenuSceneRuntimeContext& context;bool replay_variant;unsigned precision;GameMath& raw_math;MenuScenePlatform& host;
 GameStartEntryMemory memory;bool sealed=false;std::vector<U> mapped_allocations;MenuReplaySaveOutcome replay_save;
 struct Collector{GameStartEntryMemoryExtensions entries;void bind_bytes(U address,void* data,std::size_t size){entries.push_back({address,data,size});}template<class T>void bind(U address,T& value){bind_bytes(address,&value,sizeof(value));}};
 struct GuestText{U address;std::vector<char> bytes;};std::vector<std::unique_ptr<GuestText>> guest_texts;
 static std::int32_t signed32(U n){return static_cast<std::int32_t>(menu_scene_detail::signed_word(n));}
 template<class F>auto module(F&& function){memory.reconcile();try{if constexpr(std::is_void_v<decltype(function())>){function();memory.reconcile();}else{auto result=function();memory.reconcile();return result;}}catch(...){memory.reconcile();throw;}}
 std::string read_text(U address,MenuReplayFormatMemory* stack=nullptr){std::string text;for(U i=0;i<0x100000;++i){const auto byte=stack?stack->byte(address+i):memory.byte(address+i);if(!byte)return text;text.push_back(static_cast<char>(byte));}throw GameStartEntryAccessFault{address,0x100000,false};}
 std::string_view guest_text(U address){auto text=std::make_unique<GuestText>();text->address=address;const auto value=read_text(address);text->bytes.assign(value.begin(),value.end());text->bytes.push_back(0);auto* stored=text.get();guest_texts.push_back(std::move(text));return {stored->bytes.data(),stored->bytes.size()-1};}
 std::string resolve_text(std::string_view text){for(auto i=guest_texts.rbegin();i!=guest_texts.rend();++i)if((*i)->bytes.data()==text.data())return read_text((*i)->address);return std::string(text);}
 struct TextScope{Impl& owner;std::size_t size;explicit TextScope(Impl& o):owner(o),size(o.guest_texts.size()){}~TextScope(){owner.guest_texts.resize(size);}};
 MenuScenePlatformResult invoke(MenuSceneRequest request,MenuReplayFormatMemory* stack=nullptr,bool apply=true){
  refresh_allocations();memory.reconcile();std::vector<std::string> values;values.reserve(request.texts.size());for(auto text:request.texts)values.push_back(resolve_text(text));for(std::size_t i=0;i<values.size();++i)request.texts[i]=values[i];
  auto result=host.invoke(request,stack);memory.reconcile();
  if(apply)for(const auto& write:result.writes){auto* local=dynamic_cast<MenuReplayFormatFrame<GameStartEntryMemory>*>(stack);if(local&&write.address-local->target()<2048u){for(std::size_t i=0;i<write.bytes.size();++i)local->byte(write.address+static_cast<U>(i),write.bytes[i]);}else memory.write(write.address,write.bytes);}
  memory.reconcile();frame.registers.eax=result.eax;frame.registers.ecx=result.ecx;frame.registers.edx=result.edx;return result;
 }
 MenuScenePlatformResult api(MenuSceneApi which,std::vector<U> words={},std::vector<std::string_view> texts={}){return invoke({which,std::move(words),std::move(texts),{}});}
 U input(U binding,bool basic=false){memory.reconcile();U value;
  if(replay_variant&&!basic){const auto mode=memory.word(0xb4310c);if(mode){const auto bitmap=memory.word(0xb43108),bit=binding==11?7u:binding;const auto leaf_value=bit<32&&(bitmap&(1u<<bit))?1u:0u;throw MenuSceneInputRegisterTransfer{0xb4241d,0xb424a5,memory.word(0xa8ce58),memory.word(0xa87f48),binding,mode,bitmap,leaf_value};}if(binding>=16)throw MenuSceneInputTransfer{0x924726,binding,0xb42448};value=memory.word(0xa72208u+binding*4u);}
  else value=read_menu_scene_input(memory,binding,false,[&](const MenuSceneRequest& request){return invoke(request);});frame.registers.eax=value;return value;}
 void resources(std::int32_t stage);void title_initialize();void present(MenuStartComponent component);void help(std::string_view first,std::string_view second);void fps(U x,U y);void throttle();void body(U entry);void replay_ui();
 void centered(Q x,Q y,U ink,std::string_view text);void initialize();void refresh_allocations();MenuSceneOutcome dispatch();MenuSceneOutcome step();MenuSceneOutcome result(MenuSceneOutcomeKind kind,U entry);
 struct Math:GameMath {
  Impl& owner;explicit Math(Impl& o):owner(o){}
  binary_float::Number atan2(Q y,Q x)override{return owner.module([&]{return owner.raw_math.atan2(y,x);});}
  binary_float::Number cosine(Q x)override{return owner.module([&]{return owner.raw_math.cosine(x);});}
  binary_float::Number sine(Q x)override{return owner.module([&]{return owner.raw_math.sine(x);});}
 }math;
 struct Stage:GameStartEntryServices,MenuTitleReturnServices {
  Impl& owner;explicit Stage(Impl& o):owner(o){}
  std::int32_t delete_graph(U h,U f)override{return signed32(owner.api(MenuSceneApi::delete_graph,{h,f}).eax);}
  U load_graph(std::string_view path,U flag)override{return owner.api(MenuSceneApi::load_graph,{flag},{path}).eax;}
  U platform_random(U n)override{return owner.api(MenuSceneApi::platform_random,{n}).eax;}
  std::int32_t movie_seek(U h,U p)override{return signed32(owner.api(MenuSceneApi::movie_seek,{h,p}).eax);}
  std::int32_t movie_play(U h,U m,U f)override{return signed32(owner.api(MenuSceneApi::movie_play,{h,m,f}).eax);}
  std::int32_t set_create_sound_data_type(U n)override{return signed32(owner.api(MenuSceneApi::set_sound_type,{n}).eax);}
  std::int32_t stop_sound(U h)override{return signed32(owner.api(MenuSceneApi::stop_sound,{h}).eax);}
  std::int32_t delete_sound(U h,U f)override{return signed32(owner.api(MenuSceneApi::delete_sound,{h,f}).eax);}
  std::int32_t load_sound(std::string_view path,U n,U h)override{return signed32(owner.api(MenuSceneApi::load_sound,{n,h},{path}).eax);}
  std::int32_t set_volume(U v,U h)override{return signed32(owner.api(MenuSceneApi::set_volume,{v,h}).eax);}
  std::int32_t play_sound(U h,U m,U f)override{return signed32(owner.api(MenuSceneApi::play_sound,{h,m,f}).eax);}
  GameStartEntryPlatformResult platform(GameStartEntryPlatformApi which,const std::vector<U>& args)override{
   const auto result=owner.invoke({static_cast<MenuSceneApi>(static_cast<U>(which)),args,{},{}},nullptr,false);GameStartEntryPlatformResult out{result.eax,result.ecx,result.edx,{}};for(const auto& write:result.writes)out.writes.push_back({write.address,write.bytes});return out;
  }
 }stage;
 struct Front:MenuPresentationServices,MenuCharacterPracticeServices {
  Impl& owner;explicit Front(Impl& o):owner(o){}
  std::int32_t input_count(U b)override{return signed32(owner.input(b));}
  U color(U r,U g,U b)override{return owner.api(MenuSceneApi::color,{r,g,b}).eax;}
  void blend(U mode,U alpha)override{owner.api(MenuSceneApi::blend,{mode,alpha});}
  void draw_font_text(std::int32_t x,std::int32_t y,std::string_view text,U ink,U font,U edge,U vertical)override{owner.api(MenuSceneApi::draw_font_text,{U(x),U(y),ink,font,edge,vertical},{text});}
  void draw_rotated_sprite(U x,U y,Q scale,Q angle,U graph,U transparent,U turn)override{owner.api(MenuSceneApi::rotated_sprite,{x,y,U(scale),U(scale>>32),U(angle),U(angle>>32),graph,transparent,turn});}
  void change_stage_resources(std::int32_t s)override{owner.resources(s);}void present_component(MenuStartComponent c)override{owner.present(c);}void draw_help(std::string_view a,std::string_view b)override{owner.help(a,b);}
  void move_cursor(U x,U y)override{owner.fps(x,y);}void present_cursor()override{owner.throttle();}
  void draw_centered_font_text(Q x,Q y,std::string_view text,U ink,U font)override{owner.module([&]{present_menu_centered_text(x,y,text,ink,font,*this);});}
  void rectangle(std::int32_t l,std::int32_t t,std::int32_t r,std::int32_t b,U ink,U fill)override{owner.api(MenuSceneApi::rectangle,{U(l),U(t),U(r),U(b),ink,fill});}
  void brightness(U r,U g,U b)override{owner.api(MenuSceneApi::brightness,{r,g,b});}void draw_graph(U x,U y,U graph,U trans)override{owner.api(MenuSceneApi::draw_graph,{x,y,graph,trans});}
  U platform_random(U n)override{return owner.stage.platform_random(n);}U text_width(U font,std::string_view text)override{return owner.api(MenuSceneApi::text_width_font,{font},{text}).eax;}
  MenuPresentationRegisters draw_formatted(U x,U y,U ink,std::string_view format,bool number,Q value)override{auto args=std::vector<U>{x,y,ink};if(number){args.push_back(U(value));args.push_back(U(value>>32));}const auto result=owner.api(MenuSceneApi::draw_formatted,std::move(args),{format});return {result.eax,result.ecx};}
  U now_count(U mode)override{return owner.api(MenuSceneApi::now_count,{mode}).eax;}void sleep_ms(U ms)override{owner.api(MenuSceneApi::sleep_ms,{ms});}
  void menu_body(U entry)override{owner.body(entry);}void replay_ui_route()override{owner.replay_ui();}
 }front;
 struct Options:MenuOptionsServices {
  Impl& owner;explicit Options(Impl& o):owner(o){}
  std::int32_t input_count(U b)override{return signed32(owner.input(b));}U color(U r,U g,U b)override{return owner.front.color(r,g,b);}
  std::int32_t draw_text(std::int32_t x,std::int32_t y,U ink,std::string_view format,U number,std::string_view text)override{return signed32(owner.api(MenuSceneApi::draw_formatted,{U(x),U(y),ink,number},{format,text}).eax);}
  void change_stage_resources(std::int32_t s)override{owner.resources(s);}std::int32_t set_font_size(U n)override{return signed32(owner.api(MenuSceneApi::set_font_size,{n}).eax);}
  std::int32_t centered_text(Q x,Q y,U ink,std::string_view text)override{owner.centered(x,y,ink,text);return signed32(owner.frame.registers.eax);}
  U pad_state(U n)override{return owner.api(MenuSceneApi::pad_state,{n}).eax;}
  std::int32_t set_create_sound_data_type(U n)override{return owner.stage.set_create_sound_data_type(n);}std::int32_t stop_sound(U h)override{return owner.stage.stop_sound(h);}std::int32_t delete_sound(U h,U f)override{return owner.stage.delete_sound(h,f);}
  std::int32_t load_sound(std::string_view p,U n,U h)override{return owner.stage.load_sound(p,n,h);}std::int32_t set_volume(U v,U h)override{return owner.stage.set_volume(v,h);}std::int32_t play_sound(U h,U m,U f)override{return owner.stage.play_sound(h,m,f);}
 }options;
 struct Savedata:SaveIO {
  Impl& owner;explicit Savedata(Impl& o):owner(o){}
  bool open(std::string_view path,bool writing)override{return owner.module([&]{return owner.host.savedata_io().open(path,writing);});}
  std::size_t read(std::uint8_t* target,std::size_t size)override{return owner.module([&]{return owner.host.savedata_io().read(target,size);});}
  void write(const std::uint8_t* bytes,std::size_t size)override{owner.module([&]{owner.host.savedata_io().write(bytes,size);});}
  void close()override{owner.module([&]{owner.host.savedata_io().close();});}
 }save_io;
 struct Replay:MenuReplayServices {
  Impl& owner;explicit Replay(Impl& o):owner(o){}
  MenuReplayPlatformResult call(MenuReplayApi which,const std::vector<U>& args,MenuReplayFormatMemory* stack=nullptr)override;
  MenuReplayPlatformResult format_x86(MenuReplayFormatMemory& stack,const MenuReplayFormatRequest& request)override{
   const auto result=owner.invoke({MenuSceneApi::format_x86,{request.format,request.arguments,request.target,request.capacity},{},{}},&stack,false);MenuReplayPlatformResult out{result.eax,result.ecx,result.edx,{}};for(const auto& write:result.writes)out.writes.push_back(write);return out;
  }
 }replay_api;
 Impl(GameStartEntryRefs refs,GameStartEntryGaps& g,GameStartEntryReplayContext& r,GameStartEntryMachineFrame& f,MenuSceneRuntimeContext& c,bool variant,unsigned bits,GameMath& functions,MenuScenePlatform& platform):
  game(refs),gaps(g),replay(r),frame(f),context(c),replay_variant(variant),precision(bits),raw_math(functions),host(platform),memory(refs.random),math(*this),stage(*this),front(*this),options(*this),save_io(*this),replay_api(*this){
  if(bits!=24&&bits!=53&&bits!=64)throw std::invalid_argument("Menu scene precision");bind_game_start_entry(memory,game,gaps,replay,replay_variant);bind_menu_scene_state(memory,game,context,replay_variant);if(!frame.stack.empty())memory.bind_bytes(frame.stack_address,frame.stack.data(),frame.stack.size());if(replay_variant)for(const auto& allocation:replay.allocations)mapped_allocations.push_back(allocation.address);
 }
};
inline void MenuSceneRuntime::Impl::refresh_allocations(){for(auto& a:replay.allocations)if(std::find(mapped_allocations.begin(),mapped_allocations.end(),a.address)==mapped_allocations.end()){memory.bind_runtime_bytes(a.address,a.bytes.data(),a.bytes.size());mapped_allocations.push_back(a.address);}}
inline void MenuSceneRuntime::Impl::resources(std::int32_t s){module([&]{change_stage_resources(U(s),game.resources,game.meshes,game.player,game.random,static_cast<RandomPrecision>(precision),math,static_cast<GameStartEntryServices&>(stage));});}
inline void MenuSceneRuntime::Impl::title_initialize(){module([&]{initialize_menu_title(context.title,game.menu,game.music,game.lifecycle.scene,game.savedata,precision,stage);});}
inline void MenuSceneRuntime::Impl::present(MenuStartComponent component){module([&]{switch(component){case MenuStartComponent::routine_5e71e0:present_menu_strips(context.title,game.resources,front);break;case MenuStartComponent::routine_5e6ff0:present_menu_particles(context.title,game.resources,precision,front);break;case MenuStartComponent::routine_5e6c00:present_menu_objects(context.title,context.assets,precision,math,front);break;case MenuStartComponent::initialize_title_5e9490:title_initialize();break;}});}
inline void MenuSceneRuntime::Impl::help(std::string_view a,std::string_view b){module([&]{present_menu_help(a,b,context.assets,front);});}
inline void MenuSceneRuntime::Impl::fps(U x,U y){frame.registers.ecx=module([&]{return present_menu_fps(context.timing,context.title,x,y,frame.registers.ecx,precision,front,replay_variant);});}
inline void MenuSceneRuntime::Impl::throttle(){frame.registers.ecx=module([&]{return throttle_menu_frame(context.timing,precision,front,&context.keyboard_counts);});}
inline void MenuSceneRuntime::Impl::centered(Q x,Q y,U ink,std::string_view text){module([&]{const auto yy=menu_presentation_detail::integer_coordinate(y),xx=menu_presentation_detail::integer_coordinate(x);const auto current=resolve_text(text);const auto length=current.find('\0');const auto count=static_cast<U>(length==std::string::npos?current.size():length);const auto width=api(MenuSceneApi::text_width_default,{count,0},{text}).eax;const auto half=static_cast<U>(menu_scene_detail::signed_word(width)/2);api(MenuSceneApi::draw_formatted,{xx-half,yy,ink},{text});});}
inline void MenuSceneRuntime::Impl::body(U entry){module([&]{switch(entry){case 0x5e9830:update_menu_options(context.options,game.savedata,game.music,context.sound_handles,precision,save_io,options,replay_variant);break;case 0x5e9fe0:update_menu_key_config(context.keys,game.savedata,save_io,options);break;case 0x5ea4d0:update_menu_music(context.music_room,game.music,game.lifecycle.scene,game.savedata.options[1],precision,options);break;default:throw std::out_of_range("Menu scene body entry");}});}
inline MenuReplayPlatformResult MenuSceneRuntime::Impl::Replay::call(MenuReplayApi which,const std::vector<U>& args,MenuReplayFormatMemory* stack){
 TextScope scope(owner);MenuScenePlatformResult result;
 switch(which){
 case MenuReplayApi::input_count:result.eax=owner.input(args.at(0),true);result.ecx=owner.frame.registers.ecx;result.edx=owner.frame.registers.edx;return {result.eax,result.ecx,result.edx,{}};
 case MenuReplayApi::help:owner.help(owner.guest_text(args.at(0)),owner.guest_text(args.at(1)));break;
 case MenuReplayApi::options_body:owner.body(0x5e9830);break;
 case MenuReplayApi::return_title:owner.module([&]{return_menu_title(owner.game.menu,owner.context.title,owner.game.resources,owner.stage,false);});break;
 case MenuReplayApi::music_volume:owner.module([&]{menu_options_detail::bgm_volume(args.at(0),owner.game.music,owner.precision,owner.options);});break;
 case MenuReplayApi::sound_volume:owner.module([&]{menu_options_detail::sound_volume(args.at(0),owner.context.sound_handles,owner.precision,owner.options);});break;
 case MenuReplayApi::formatted_text:{std::vector<U> values(args.begin(),args.begin()+3);values.insert(values.end(),args.begin()+4,args.end());owner.invoke({MenuSceneApi::draw_formatted,std::move(values),{owner.guest_text(args.at(3))},{}},stack);break;}
 case MenuReplayApi::font_text:{const auto text=owner.read_text(args.at(2),stack);owner.invoke({MenuSceneApi::draw_font_text,{args.at(0),args.at(1),args.at(3),args.at(4),args.at(5),args.at(6)},{text},{}},stack);break;}
 default:owner.invoke({static_cast<MenuSceneApi>(static_cast<U>(which)),args,{},{}},stack);break;
 }
 return {owner.frame.registers.eax,owner.frame.registers.ecx,owner.frame.registers.edx,{}};
}
inline void MenuSceneRuntime::Impl::replay_ui(){module([&]{const auto r=frame.registers;MenuReplay<GameStartEntryMemory> ui(memory,replay_api,{r.eax,r.ecx,r.edx,r.ebx,r.esp,r.ebp,r.esi,r.edi});ui.route();replay_save=ui.last_save;frame.registers.eax=ui.registers.eax;frame.registers.ecx=ui.registers.ecx;frame.registers.edx=ui.registers.edx;frame.registers.ebx=ui.registers.ebx;frame.registers.esi=ui.registers.esi;frame.registers.edi=ui.registers.edi;});}
inline void MenuSceneRuntime::Impl::initialize(){if(!sealed)throw std::logic_error("Seal menu scene state before initialization");module([&]{return_menu_title(game.menu,context.title,game.resources,stage,false);});title_initialize();}
inline MenuSceneOutcome MenuSceneRuntime::Impl::result(MenuSceneOutcomeKind kind,U entry){memory.reconcile();MenuSceneOutcome out;out.kind=kind;out.entry_scene=entry;out.scene=memory.word(0xa8ce58);out.outer_frame=memory.word(0xa87f1c);out.last_ecx=frame.registers.ecx;out.replay_save=replay_save;return out;}
inline MenuSceneOutcome MenuSceneRuntime::Impl::dispatch(){
 if(!sealed)throw std::logic_error("Seal menu scene state before dispatch");memory.reconcile();const auto entry=memory.word(0xa8ce58);auto& start=static_cast<MenuPresentationServices&>(front);
 switch(entry){
 case 10:if(module([&]{return present_menu_start(game.menu,game.savedata,precision,start,replay_variant);})){api(MenuSceneApi::shutdown);return result(MenuSceneOutcomeKind::exit_requested,entry);}break;
 case 11:module([&]{present_menu_difficulty(game.menu,game.savedata,precision,start);});break;
 case 12:module([&]{present_menu_character_practice(game.menu,context.selection,game.resources,game.savedata,precision,front);});break;
 case 13:module([&]{present_menu_extra_confirm(game.menu,game.savedata,precision,start);});break;
 case 14:case 15:case 16:module([&]{present_menu_wrapper(entry==14?0x5ea8a0u:entry==15?0x5ea470u:0x5e9fa0u,game.menu,context.title,game.resources,context.assets,context.timing,precision,front,replay_variant,&context.keyboard_counts);});break;
 case 17:module([&]{present_menu_omniverse_confirm(game.menu,game.savedata,precision,start);});break;
 case 99:{
  Collector collector;
  bind_menu_scene_state(collector,game,context,replay_variant);
  frame.registers.eax=99;frame.registers.ecx=99;frame.registers.edx=9;game_start_entry_detail::compare_flags(frame.registers,99,102);
  const auto startup=module([&]{return enter_game_start_scene99(game,gaps,replay,replay_variant,precision,math,stage,frame,collector.entries);});refresh_allocations();auto out=result(startup.kind==GameStartEntryOutcomeKind::scene100?MenuSceneOutcomeKind::gameplay_ready:startup.kind==GameStartEntryOutcomeKind::original_failure_transfer?MenuSceneOutcomeKind::startup_transfer:MenuSceneOutcomeKind::memory_fault,entry);out.startup=startup;out.target=startup.target;out.fault_address=startup.fault_address;out.fault_bytes=startup.fault_bytes;out.write_fault=startup.write_fault;return out;
 }
 default:return result(MenuSceneOutcomeKind::unhandled_scene,entry);
 }
 return result(MenuSceneOutcomeKind::completed,entry);
}
inline MenuSceneOutcome MenuSceneRuntime::Impl::step(){
 if(!sealed)throw std::logic_error("Seal menu scene state before stepping");memory.reconcile();const auto entry=memory.word(0xa8ce58);if(!((entry>=10&&entry<=17)||entry==99))return result(MenuSceneOutcomeKind::unhandled_scene,entry);
 try{
  if(api(MenuSceneApi::process_message).eax!=0){api(MenuSceneApi::shutdown);return result(MenuSceneOutcomeKind::exit_requested,entry);}
  if(api(MenuSceneApi::clear_draw_screen,{0}).eax!=0){api(MenuSceneApi::shutdown);return result(MenuSceneOutcomeKind::exit_requested,entry);}
  const auto invoke_api=[&](const MenuSceneRequest& request){return invoke(request);};update_menu_scene_keyboard(memory,invoke_api);update_menu_scene_pad(memory,invoke_api);memory.clear(0xa73230,400);
  const auto actual=memory.word(0xa8ce58);memory.word(frame.registers.ebp-4,actual);frame.registers.eax=actual;frame.registers.ecx=actual;game_start_entry_detail::compare_flags(frame.registers,actual,102);
  auto dispatched=dispatch();if(dispatched.kind!=MenuSceneOutcomeKind::completed&&dispatched.kind!=MenuSceneOutcomeKind::gameplay_ready)return dispatched;
  if(actual>=10&&actual<=17)draw_menu_scene_footer(2,actual==10?2u:463u,false,replay_variant,invoke_api);
  play_menu_scene_sounds(memory,invoke_api);draw_menu_scene_footer(replay_variant?534u:512u,2,true,replay_variant,invoke_api);throttle();api(MenuSceneApi::screen_flip);
  const auto count=memory.word(0xa87f1c)+1u;memory.word(0xa87f1c,count);frame.registers.ecx=count;auto out=result(dispatched.kind,entry);out.startup=dispatched.startup;return out;
 }catch(const GameStartEntryAccessFault& fault){auto out=result(MenuSceneOutcomeKind::memory_fault,entry);out.fault_address=fault.address;out.fault_bytes=fault.bytes;out.write_fault=fault.writing;return out;}
 catch(const MenuSceneInputTransfer& transfer){auto out=result(MenuSceneOutcomeKind::input_transfer,entry);out.target=transfer.target;out.fault_address=transfer.binding;return out;}
 catch(const MenuSceneInputRegisterTransfer& transfer){auto out=result(MenuSceneOutcomeKind::input_register_transfer,entry);out.target=transfer.entry;out.input_register_transfer=transfer;return out;}
 catch(const MenuScenePlatformOutputFault& fault){auto out=result(MenuSceneOutcomeKind::platform_output_fault,entry);out.target=static_cast<U>(fault.api);out.fault_address=fault.index;out.fault_bytes=fault.required;return out;}
 catch(const MenuReplayFormatFault& fault){auto out=result(MenuSceneOutcomeKind::format_fault,entry);out.fault_address=fault.address;out.partial_stack_writes=fault.stack_writes;return out;}
 catch(const MenuPresentationAddressFault& fault){auto out=result(MenuSceneOutcomeKind::memory_fault,entry);out.fault_address=fault.address;out.fault_bytes=4;return out;}
}
inline MenuSceneRuntime::MenuSceneRuntime(GameStartEntryRefs game,GameStartEntryGaps& gaps,GameStartEntryReplayContext& replay,GameStartEntryMachineFrame& frame,MenuSceneRuntimeContext& menu,bool variant,unsigned precision,GameMath& math,MenuScenePlatform& platform):impl_(std::make_unique<Impl>(game,gaps,replay,frame,menu,variant,precision,math,platform)){}
inline MenuSceneRuntime::~MenuSceneRuntime()=default;
inline GameStartEntryMemory& MenuSceneRuntime::memory(){return impl_->memory;}
inline void MenuSceneRuntime::seal(){impl_->memory.seal();impl_->sealed=true;}
inline void MenuSceneRuntime::initialize_title(){impl_->initialize();}
inline void MenuSceneRuntime::initialize_title_background(){if(!impl_->sealed)throw std::logic_error("Seal menu scene state before initialization");impl_->title_initialize();}
inline void MenuSceneRuntime::return_title_at(std::uint32_t entry){if(entry!=0x5e9800&&entry!=0xb419a7)throw std::out_of_range("Menu title return entry");if(entry==0xb419a7&&!impl_->replay_variant)throw std::logic_error("Replay title entry requires Replay image");impl_->module([&]{return_menu_title(impl_->game.menu,impl_->context.title,impl_->game.resources,impl_->stage,entry==0xb419a7);});}
inline MenuSceneOutcome MenuSceneRuntime::dispatch_menu_scene(){impl_->memory.reconcile();const auto entry=impl_->memory.word(0xa8ce58);try{return impl_->dispatch();}
 catch(const MenuSceneInputRegisterTransfer& transfer){auto out=impl_->result(MenuSceneOutcomeKind::input_register_transfer,entry);out.target=transfer.entry;out.input_register_transfer=transfer;return out;}
 catch(const MenuSceneInputTransfer& transfer){auto out=impl_->result(MenuSceneOutcomeKind::input_transfer,entry);out.target=transfer.target;out.fault_address=transfer.binding;return out;}
 catch(const GameStartEntryAccessFault& fault){auto out=impl_->result(MenuSceneOutcomeKind::memory_fault,entry);out.fault_address=fault.address;out.fault_bytes=fault.bytes;out.write_fault=fault.writing;return out;}
 catch(const MenuPresentationAddressFault& fault){auto out=impl_->result(MenuSceneOutcomeKind::memory_fault,entry);out.fault_address=fault.address;out.fault_bytes=4;return out;}
 catch(const MenuReplayFormatFault& fault){auto out=impl_->result(MenuSceneOutcomeKind::format_fault,entry);out.fault_address=fault.address;out.partial_stack_writes=fault.stack_writes;return out;}
}
inline MenuSceneOutcome MenuSceneRuntime::step_menu_frame(){return impl_->step();}
}
