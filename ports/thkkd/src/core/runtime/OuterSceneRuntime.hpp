// 已恢复场景模块的共同帧；宿主只提供平台和实际游戏体两个边界。
#pragma once
#include "core/runtime/OuterSceneFrame.hpp"
#include "core/runtime/OuterSceneBindings.hpp"
#include "core/frontend/MenuSceneRuntime.hpp"
#include "core/frontend/MenuBootstrapRuntime.hpp"
#include "core/frontend/ReplayFailureContinuation.hpp"
namespace kaikeidou {
struct OuterSceneHost {
 MenuScenePlatform& menu;MenuBootstrapPlatform& bootstrap;PauseGameoverHost pause;
 EndingCreditsPlatform& ending;BossPhaseServices& failure;OuterSceneGameplayServices& gameplay;
};
class OuterSceneRuntime:private OuterSceneFrameServices {
 using U=std::uint32_t;using Q=std::uint64_t;
 GameStartEntryRefs game_;GameStartEntryGaps& gaps_;GameStartEntryReplayContext& replay_;GameStartEntryMachineFrame& frame_;
 MenuSceneRuntimeContext& context_;CollisionContext& collision_;EndingCreditsAssets& ending_;MenuBootstrapContext& bootstrap_;MenuBootstrapGaps& bootstrap_gaps_;PauseGameoverContext& pause_;
 bool variant_,sealed_=false;unsigned precision_;GameMath& math_;OuterSceneHost host_;MenuSceneRuntime menu_;GameStartEntryMemoryExtensions extensions_;ReplayFailureContinuationPolicy policy_;
 MenuSceneOutcome menu_outcome_;PauseGameoverOutcome pause_outcome_;MenuBootstrapOutcome bootstrap_outcome_;ReplayFailureContinuationOutcome failure_outcome_;
 GameStartEntryMemory& view(){return menu_.memory();}
 MenuScenePlatformResult invoke(const MenuSceneRequest& request){view().reconcile();auto out=host_.menu.invoke(request);view().reconcile();for(const auto& write:out.writes)view().write(write.address,write.bytes);view().reconcile();frame_.registers.eax=out.eax;frame_.registers.ecx=out.ecx;frame_.registers.edx=out.edx;return out;}
 struct Front:MenuPresentationServices {
  OuterSceneRuntime& r;explicit Front(OuterSceneRuntime& owner):r(owner){}
  MenuScenePlatformResult api(MenuSceneApi a,std::vector<U> words={},std::vector<std::string_view> texts={}){return r.invoke({a,std::move(words),std::move(texts),{}});}
  std::int32_t input_count(U b)override{return static_cast<std::int32_t>(read_menu_scene_input(r.view(),b,r.variant_,[&](const MenuSceneRequest& q){return r.invoke(q);}));}
  U color(U a,U b,U c)override{return api(MenuSceneApi::color,{a,b,c}).eax;}void blend(U a,U b)override{api(MenuSceneApi::blend,{a,b});}
  void draw_font_text(std::int32_t x,std::int32_t y,std::string_view t,U ink,U font,U edge,U vertical)override{api(MenuSceneApi::draw_font_text,{U(x),U(y),ink,font,edge,vertical},{t});}
  void draw_rotated_sprite(U x,U y,Q z,Q a,U g,U t,U turn)override{api(MenuSceneApi::rotated_sprite,{x,y,U(z),U(z>>32),U(a),U(a>>32),g,t,turn});}
  void change_stage_resources(std::int32_t)override{throw std::logic_error("Outer throttle unexpected resource dispatch");}
  void present_component(MenuStartComponent)override{throw std::logic_error("Outer throttle unexpected menu component");}
  void draw_help(std::string_view,std::string_view)override{throw std::logic_error("Outer throttle unexpected help");}
  void move_cursor(U,U)override{throw std::logic_error("Outer throttle unexpected cursor");}void present_cursor()override{throw std::logic_error("Outer throttle recursion");}
  void brightness(U a,U b,U c)override{api(MenuSceneApi::brightness,{a,b,c});}void draw_graph(U x,U y,U g,U t)override{api(MenuSceneApi::draw_graph,{x,y,g,t});}
  U platform_random(U n)override{return api(MenuSceneApi::platform_random,{n}).eax;}U text_width(U f,std::string_view t)override{return api(MenuSceneApi::text_width_font,{f},{t}).eax;}
  MenuPresentationRegisters draw_formatted(U x,U y,U ink,std::string_view text,bool number,Q value)override{std::vector<U> words{x,y,ink};if(number){words.push_back(U(value));words.push_back(U(value>>32));}auto out=api(MenuSceneApi::draw_formatted,std::move(words),{text});return {out.eax,out.ecx};}
  U now_count(U n)override{return api(MenuSceneApi::now_count,{n}).eax;}void sleep_ms(U n)override{api(MenuSceneApi::sleep_ms,{n});}
  void menu_body(U)override{throw std::logic_error("Outer throttle unexpected menu body");}void replay_ui_route()override{throw std::logic_error("Outer throttle unexpected Replay UI");}
 }front_;
 struct Ending:EndingCreditsPlatform {
  OuterSceneRuntime& owner;explicit Ending(OuterSceneRuntime& r):owner(r){}
  EndingCreditsPlatformResult invoke(const EndingCreditsPlatformRequest& request)override{
   if(request.api==0x656c70){auto& r=owner.frame_.registers;const auto value=read_menu_scene_input(owner.view(),request.words.at(0),false,[&](const MenuSceneRequest& q){return owner.invoke(q);});r.eax=value;return {r.eax,r.ecx,r.edx,{}};}
   auto out=owner.host_.ending.invoke(request);auto& r=owner.frame_.registers;r.eax=out.eax;r.ecx=out.ecx;r.edx=out.edx;return out;
  }
  GameStartEntryMachineFrame& replay_input_frame(U caller,U binding)override{
   auto& frame=owner.frame_;auto& r=frame.registers;r.eax=binding;r.esp-=4;owner.view().word(r.esp,binding);r.esp-=4;owner.view().word(r.esp,caller+5u);return frame;
  }
  void observe_input_leaf(U caller,const GameStartEntryMachineFrame& frame,const MenuSceneInputLeafOutcome& out)override{
   owner.host_.ending.observe_input_leaf(caller,frame,out);
   if(out.kind==MenuSceneInputLeafKind::returned){auto& r=owner.frame_.registers;outer_scene_detail::add_flags(r,r.esp,4);r.esp+=4;}
  }
  void observe_text_copy(U destination,U source)override{owner.host_.ending.observe_text_copy(destination,source);}
 }ending_api_;
 MenuScenePlatformResult platform(const MenuSceneRequest& request)override{return host_.menu.invoke(request);}
 OuterSceneCallOutcome gameplay_body(GameStartEntryMachineFrame& f)override{return host_.gameplay.gameplay_body(view(),f);}
 OuterSceneCallOutcome replay_after_gameplay(GameStartEntryMachineFrame& f)override{return host_.gameplay.replay_after_gameplay(view(),f);}
 void throttle(GameStartEntryMachineFrame& f)override{f.registers.ecx=throttle_menu_frame(context_.timing,precision_,front_,&context_.keyboard_counts);}
 OuterSceneOutcome menu_result(){
  OuterSceneOutcome out;out.target=menu_outcome_.target;out.fault_address=menu_outcome_.fault_address;out.fault_bytes=menu_outcome_.fault_bytes;out.writing=menu_outcome_.write_fault;
  switch(menu_outcome_.kind){
   case MenuSceneOutcomeKind::completed:case MenuSceneOutcomeKind::gameplay_ready:case MenuSceneOutcomeKind::unhandled_scene:break;
   case MenuSceneOutcomeKind::exit_requested:out.kind=OuterSceneOutcomeKind::exit_requested;out.source=0x65330c;break;
   case MenuSceneOutcomeKind::memory_fault:out.kind=OuterSceneOutcomeKind::memory_fault;break;
   case MenuSceneOutcomeKind::input_transfer:out.kind=OuterSceneOutcomeKind::input_transfer;break;
   case MenuSceneOutcomeKind::input_register_transfer:out.kind=OuterSceneOutcomeKind::input_register_transfer;out.source=menu_outcome_.input_register_transfer.return_instruction;break;
   case MenuSceneOutcomeKind::platform_output_fault:out.kind=OuterSceneOutcomeKind::platform_output_fault;break;
   case MenuSceneOutcomeKind::format_fault:out.kind=OuterSceneOutcomeKind::format_fault;break;
   case MenuSceneOutcomeKind::startup_transfer:out.kind=OuterSceneOutcomeKind::transfer;break;
  }return out;
 }
 OuterSceneOutcome dispatch_scene(U scene,GameStartEntryMachineFrame& f)override{
  OuterSceneOutcome out;view().reconcile();
  if(scene==0){
   MenuBootstrapRuntime bootstrap(game_,gaps_,replay_,context_,collision_,ending_,bootstrap_,bootstrap_gaps_,variant_,precision_,host_.bootstrap,extensions_);bootstrap.seal();bootstrap_outcome_=bootstrap.run_scene_zero();bootstrap.memory().reconcile();view().reconcile();f.registers.eax=bootstrap_outcome_.registers.eax;f.registers.ecx=bootstrap_outcome_.registers.ecx;f.registers.edx=bootstrap_outcome_.registers.edx;
   if(bootstrap_outcome_.kind!=MenuBootstrapOutcomeKind::completed){out.kind=bootstrap_outcome_.kind==MenuBootstrapOutcomeKind::memory_fault?OuterSceneOutcomeKind::memory_fault:OuterSceneOutcomeKind::platform_output_fault;out.source=bootstrap_outcome_.fault_pc;out.fault_address=bootstrap_outcome_.fault_address;out.fault_bytes=bootstrap_outcome_.fault_bytes;out.writing=bootstrap_outcome_.write_fault;}return out;
  }
  if((scene>=10&&scene<=17)||scene==99){
   menu_outcome_=menu_.dispatch_menu_scene();view().reconcile();out=menu_result();
   if(menu_outcome_.kind==MenuSceneOutcomeKind::startup_transfer&&menu_outcome_.target==0x64423d){
    auto policy=policy_;policy.open_replay_handles=menu_outcome_.startup.open_handles;
    GameStartEntryMemoryExtensions failure_extras;for(auto x:extensions_)if(!(x.address==f.stack_address&&x.data==f.stack.data()&&x.size==f.stack.size()))failure_extras.push_back(x);
    failure_outcome_=continue_replay_failure(game_,gaps_,replay_,f,precision_,math_,host_.failure,host_.menu.savedata_io(),policy,failure_extras);view().reconcile();out.source=failure_outcome_.pc;out.target=failure_outcome_.target;out.exact_registers=failure_outcome_.exact_registers;
    if(failure_outcome_.kind==ReplayFailureContinuationKind::returned_target&&failure_outcome_.target==0x65345a)out.kind=OuterSceneOutcomeKind::completed;
    else if(failure_outcome_.kind==ReplayFailureContinuationKind::read_fault||failure_outcome_.kind==ReplayFailureContinuationKind::write_fault){out.kind=OuterSceneOutcomeKind::memory_fault;out.fault_address=failure_outcome_.address;out.fault_bytes=failure_outcome_.bytes;out.writing=failure_outcome_.kind==ReplayFailureContinuationKind::write_fault;}
    else out.kind=OuterSceneOutcomeKind::transfer;
   }return out;
  }
  if(scene==100){
   PauseGameoverRuntime pause(game_,gaps_,replay_,context_.title,pause_,host_.pause,math_,precision_,variant_,extensions_);pause_outcome_=pause.update({f.registers.eax,f.registers.edi,0});pause.memory().reconcile();view().reconcile();f.registers.eax=pause_outcome_.memory_fault?pause_outcome_.abi.eax:pause_outcome_.result;f.registers.edi=pause_outcome_.abi.edi;
   if(pause_outcome_.memory_fault){out.kind=OuterSceneOutcomeKind::memory_fault;out.source=pause_outcome_.abi.instruction;out.fault_address=pause_outcome_.fault_address;out.fault_bytes=pause_outcome_.fault_bytes;out.writing=pause_outcome_.writing;}else out.run_gameplay=pause_outcome_.result==0;return out;
  }
  if(scene==101||scene==102){
   auto& r=f.registers;const U return_pc=scene==101?0x653453u:0x65345au;U pc=scene==101?0x65344eu:0x653455u;
   try{
    r.esp-=4;view().word(r.esp,return_pc);pc=scene==101?0x669dd0u:0x66c820u;r.esp-=4;view().word(r.esp,r.ebp);r.ebp=r.esp;const U locals=scene==101?0x1cu:0x28u;outer_scene_detail::compare_flags(r,r.esp,locals);r.esp-=locals;
    EndingCreditsRuntime ending(view(),game_,context_.title,ending_api_,variant_,precision_);if(scene==101)ending.ending();else ending.credits();
    pc=scene==101?0x66c5adu:0x66cd21u;r.esp=r.ebp;r.ebp=view().word(r.esp);r.esp+=4;pc=scene==101?0x66c5b0u:0x66cd24u;const auto target=view().word(r.esp);r.esp+=4;
    if(target!=return_pc){out.kind=OuterSceneOutcomeKind::transfer;out.source=pc;out.target=target;}
   }catch(const MenuSceneInputLeafOutcome& leaf){out.kind=OuterSceneOutcomeKind::input_transfer;out.source=leaf.source;out.target=leaf.target;out.fault_address=leaf.binding;}
   catch(const GameStartEntryAccessFault& fault){out.kind=OuterSceneOutcomeKind::memory_fault;out.source=pc;out.fault_address=fault.address;out.fault_bytes=fault.bytes;out.writing=fault.writing;}
   view().reconcile();
  }
  return out;
 }
public:
 OuterSceneRuntime(GameStartEntryRefs game,GameStartEntryGaps& gaps,GameStartEntryReplayContext& replay,GameStartEntryMachineFrame& frame,MenuSceneRuntimeContext& menu,
  CollisionContext& collision,EndingCreditsAssets& ending,MenuBootstrapContext& bootstrap,MenuBootstrapGaps& bootstrap_gaps,PauseGameoverContext& pause,
  bool replay_variant,unsigned precision,GameMath& math,OuterSceneHost host,const GameStartEntryMemoryExtensions& extra={},ReplayFailureContinuationPolicy policy={}):
  game_(game),gaps_(gaps),replay_(replay),frame_(frame),context_(menu),collision_(collision),ending_(ending),bootstrap_(bootstrap),bootstrap_gaps_(bootstrap_gaps),pause_(pause),variant_(replay_variant),precision_(precision),math_(math),host_(host),menu_(game,gaps,replay,frame,menu,replay_variant,precision,math,host.menu),policy_(std::move(policy)),front_(*this),ending_api_(*this){
   OuterSceneBindingCollector existing;bind_menu_scene_state(existing,game_,context_,variant_);existing.bind_bytes(frame_.stack_address,frame_.stack.data(),frame_.stack.size());
   auto first=outer_scene_extensions(game_,context_,collision_,ending_,bootstrap_,bootstrap_gaps_,pause_,frame_,variant_,extra);for(const auto& x:first){const auto duplicate=std::find_if(existing.entries.begin(),existing.entries.end(),[&](const auto& y){return x.address==y.address&&x.data==y.data&&x.size==y.size;});if(duplicate==existing.entries.end())view().bind_bytes(x.address,x.data,x.size);}bind_menu_bootstrap_gaps(view(),bootstrap_gaps_);extensions_=outer_scene_extensions(game_,context_,collision_,ending_,bootstrap_,bootstrap_gaps_,pause_,frame_,variant_,extra);
  }
 GameStartEntryMemory& memory(){return view();}
 void seal(){menu_.seal();sealed_=true;}
 OuterSceneOutcome step(){if(!sealed_)throw std::logic_error("Seal outer scene initial state before stepping");return step_outer_scene_frame(view(),frame_,*this,variant_);}
 const MenuSceneOutcome& menu_outcome()const{return menu_outcome_;}
 const PauseGameoverOutcome& pause_outcome()const{return pause_outcome_;}
 const MenuBootstrapOutcome& bootstrap_outcome()const{return bootstrap_outcome_;}
 const ReplayFailureContinuationOutcome& continuation_outcome()const{return failure_outcome_;}
};
}
