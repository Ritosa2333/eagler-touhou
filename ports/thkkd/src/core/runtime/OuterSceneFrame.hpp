// 653290..653493的单帧控制流；场景分派与游戏体均回到原公共尾边界。
#pragma once
#include "core/runtime/OuterSceneServices.hpp"
#include "core/frontend/MenuSceneInput.hpp"
#include "core/frontend/MenuScenePeripheral.hpp"
namespace kaikeidou {
enum class OuterSceneOutcomeKind {completed,exit_requested,transfer,register_transfer,memory_fault,input_transfer,input_register_transfer,platform_output_fault,format_fault};
struct OuterSceneOutcome {
 OuterSceneOutcomeKind kind=OuterSceneOutcomeKind::completed;
 std::uint32_t entry_scene=0,dispatched_scene=0,scene=0,outer_frame=0,game_frame=0;
 std::uint32_t source=0,target=0,exact_registers=0,missing_registers=0,fault_address=0,fault_bytes=0;
 bool writing=false,run_gameplay=false;
};
struct OuterSceneFrameServices {
 virtual ~OuterSceneFrameServices()=default;
 virtual MenuScenePlatformResult platform(const MenuSceneRequest& request)=0;
 virtual OuterSceneOutcome dispatch_scene(std::uint32_t scene,GameStartEntryMachineFrame& frame)=0;
 virtual OuterSceneCallOutcome gameplay_body(GameStartEntryMachineFrame& frame)=0;
 virtual OuterSceneCallOutcome replay_after_gameplay(GameStartEntryMachineFrame& frame)=0;
 virtual void throttle(GameStartEntryMachineFrame& frame)=0;
};
namespace outer_scene_detail {
inline void add_flags(GameStartEntryRegisters& r,std::uint32_t a,std::uint32_t b){const auto wide=std::uint64_t(a)+b;const auto value=static_cast<std::uint32_t>(wide);unsigned ones=0;for(unsigned i=0;i<8;++i)ones+=(value>>i)&1u;r.eflags=(r.eflags&~0x8d5u)|static_cast<std::uint32_t>(wide>>32)|((ones&1u)?0u:4u)|((a^b^value)&16u)|(value==0?64u:0u)|((value>>24)&128u)|(((~(a^b)&(a^value))>>20)&0x800u);}
inline void compare_flags(GameStartEntryRegisters& r,std::uint32_t a,std::uint32_t b){const auto value=a-b;unsigned ones=0;for(unsigned i=0;i<8;++i)ones+=(value>>i)&1u;r.eflags=(r.eflags&~0x8d5u)|(a<b?1u:0u)|((ones&1u)?0u:4u)|((a^b^value)&16u)|(value==0?64u:0u)|((value>>24)&128u)|((((a^b)&(a^value))>>20)&0x800u);}
inline std::uint32_t dispatch_index(std::uint32_t scene){if(scene==0)return 0;if(scene>=10&&scene<=17)return scene==17?8u:scene-9u;if(scene>=99&&scene<=102)return scene-90u;return 13;}
}
template<class Memory>OuterSceneOutcome step_outer_scene_frame(Memory& memory,GameStartEntryMachineFrame& frame,OuterSceneFrameServices& services,bool replay_variant){
 using U=std::uint32_t;OuterSceneOutcome out;memory.reconcile();out.entry_scene=memory.word(0xa8ce58);auto& r=frame.registers;
 const auto finish=[&](){memory.reconcile();out.scene=memory.word(0xa8ce58);out.outer_frame=memory.word(0xa87f1c);out.game_frame=memory.word(0xb3e2e8);return out;};
 const auto invoke=[&](const MenuSceneRequest& request){memory.reconcile();auto result=services.platform(request);memory.reconcile();for(const auto& write:result.writes)memory.write(write.address,write.bytes);memory.reconcile();r.eax=result.eax;r.ecx=result.ecx;r.edx=result.edx;return result;};
 const auto stop_call=[&](const OuterSceneCallOutcome& called){out.kind=OuterSceneOutcomeKind::transfer;out.source=called.source;out.target=called.target;out.exact_registers=called.exact_registers;};
 try{
  out.source=0x6531f3;if(invoke({MenuSceneApi::process_message,{},{},{}}).eax!=0){invoke({MenuSceneApi::shutdown,{},{},{}});r.eax=0;outer_scene_detail::compare_flags(r,0,0);out.kind=OuterSceneOutcomeKind::exit_requested;out.target=0x653493;return finish();}
  out.source=0x653203;if(invoke({MenuSceneApi::clear_draw_screen,{0},{},{}}).eax!=0){invoke({MenuSceneApi::shutdown,{},{},{}});r.eax=0;outer_scene_detail::compare_flags(r,0,0);out.kind=OuterSceneOutcomeKind::exit_requested;out.target=0x653493;return finish();}
  out.source=0x653214;update_menu_scene_keyboard(memory,invoke);out.source=0x653219;update_menu_scene_pad(memory,invoke);
  out.source=0x652f6f;memory.clear(0xa73230,400);
  out.source=0x6532a2;const auto actual=memory.word(0xa8ce58);out.dispatched_scene=actual;memory.word(frame.registers.ebp-4u,actual);r.eax=actual;outer_scene_detail::compare_flags(r,actual,102);
  if(actual<=102){r.ecx=actual;r.edx=outer_scene_detail::dispatch_index(actual);}
  auto dispatched=services.dispatch_scene(actual,frame);memory.reconcile();const auto initial=out.entry_scene;out=dispatched;out.entry_scene=initial;out.dispatched_scene=actual;
  if(out.kind==OuterSceneOutcomeKind::exit_requested){r.eax=0;outer_scene_detail::compare_flags(r,0,0);out.target=0x653493;return finish();}
  if(out.kind!=OuterSceneOutcomeKind::completed)return finish();
  if(out.run_gameplay){
   memory.reconcile();auto body=services.gameplay_body(frame);memory.reconcile();
   if(body.kind!=OuterSceneCallKind::completed||body.target!=0x65343f){stop_call(body);return finish();}
   if(replay_variant){
    if(!(body.exact_registers&outer_ecx)){out.kind=OuterSceneOutcomeKind::register_transfer;out.source=body.source;out.target=0xb4214d;out.exact_registers=body.exact_registers;out.missing_registers=outer_ecx;return finish();}
    auto bridge=services.replay_after_gameplay(frame);memory.reconcile();
    if(bridge.kind!=OuterSceneCallKind::completed||bridge.target!=0x653444){stop_call(bridge);return finish();}
    if(!(bridge.exact_registers&outer_eax)){out.kind=OuterSceneOutcomeKind::register_transfer;out.source=bridge.source;out.target=0x653444;out.exact_registers=bridge.exact_registers;out.missing_registers=outer_eax;return finish();}
   }else r.eax=memory.word(0xb3e2e8);
   out.source=0x653444;const auto old=r.eax;outer_scene_detail::add_flags(r,old,1);r.eax=old+1u;memory.word(0xb3e2e8,r.eax);
  }
  if(actual>=10&&actual<=17){constexpr std::array<U,8> sites{{0x65331c,0x653335,0x65334e,0x653367,0x653399,0x6533b2,0x6533cb,0x653380}};out.source=sites[actual-10];draw_menu_scene_footer(2,actual==10?2u:463u,false,replay_variant,invoke);}
  out.source=0x65345a;play_menu_scene_sounds(memory,invoke);out.source=0x653466;draw_menu_scene_footer(replay_variant?534u:512u,2,true,replay_variant,invoke);
  out.source=0x65346e;memory.reconcile();services.throttle(frame);memory.reconcile();out.source=0x653473;invoke({MenuSceneApi::screen_flip,{},{},{}});
  out.source=0x65347e;const U old=memory.word(0xa87f1c);outer_scene_detail::add_flags(r,old,1);r.ecx=old+1u;memory.word(0xa87f1c,r.ecx);out.exact_registers=outer_ecx|outer_eflags;out.target=0x653290;return finish();
 }catch(const GameStartEntryAccessFault& f){out.kind=OuterSceneOutcomeKind::memory_fault;out.fault_address=f.address;out.fault_bytes=f.bytes;out.writing=f.writing;return finish();}
 catch(const MenuScenePlatformOutputFault& f){out.kind=OuterSceneOutcomeKind::platform_output_fault;out.target=static_cast<U>(f.api);out.fault_address=f.index;out.fault_bytes=f.required;return finish();}
}
}
