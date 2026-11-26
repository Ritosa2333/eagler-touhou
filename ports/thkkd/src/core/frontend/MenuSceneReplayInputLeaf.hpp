// B4241D的完整x86叶ABI；包含原错位POPAD和非法绑定的真实CALL转移。
#pragma once
#include "core/runtime/GameStartEntryContext.hpp"
namespace kaikeidou {
enum class MenuSceneInputLeafKind {returned,data_transfer};
struct MenuSceneInputLeafOutcome {MenuSceneInputLeafKind kind=MenuSceneInputLeafKind::returned;std::uint32_t target=0,binding=0,value=0,source=0;};
namespace menu_scene_input_leaf_detail {
inline void compare(GameStartEntryRegisters& r,std::uint32_t a,std::uint32_t b){const auto v=a-b;unsigned parity=0;for(unsigned i=0;i<8;++i)parity+=(v>>i)&1u;r.eflags=(r.eflags&~0x8d5u)|(a<b?1u:0u)|((parity&1u)?0u:4u)|((a^b^v)&16u)|(v==0?64u:0u)|((v>>24)&128u)|((((a^b)&(a^v))>>20)&0x800u);}
}
template<class Memory>MenuSceneInputLeafOutcome execute_menu_scene_replay_input_leaf(Memory& memory,GameStartEntryMachineFrame& frame){
 auto& r=frame.registers;const auto push=[&](std::uint32_t value){r.esp-=4u;memory.word(r.esp,value);};const auto pop=[&](){const auto value=memory.word(r.esp);r.esp+=4u;return value;};
 push(r.ebp);r.ebp=r.esp;const auto mode=memory.word(0xb4310c);menu_scene_input_leaf_detail::compare(r,mode,0);const auto binding=memory.word(r.ebp+8u);
 if(mode==0){menu_scene_input_leaf_detail::compare(r,binding,16);if(binding>=16){push(0x6cf720);push(0xb4244d);return {MenuSceneInputLeafKind::data_transfer,0x924726,binding,r.eax,0xb42448};}r.eax=memory.word(0xa72208u+binding*4u);}
 else{
  const auto saved=r;push(saved.eax);push(saved.ecx);push(saved.edx);push(saved.ebx);push(saved.esp);push(saved.ebp);push(saved.esi);push(saved.edi);
  r.ecx=memory.word(0xb43108);r.edx=memory.word(r.ebp+8u);if(r.edx==11)r.edx=7;
  // 原逐次SHL在第32次后恒为0，后续无内存访问，最终EDI等于EDX。
  r.eax=r.edx<32?1u<<r.edx:0u;r.edi=r.edx;r.ecx&=r.eax;r.esp+=4u;menu_scene_input_leaf_detail::compare(r,r.ecx,0);memory.word(r.ebp-4u,r.ecx?1u:0u);
  r.edi=pop();r.esi=pop();r.ebp=pop();r.esp+=4u;r.ebx=pop();r.edx=pop();r.ecx=pop();r.eax=pop();r.eax=memory.word(r.ebp-4u);r.esp=r.ebp;
 }
 r.ebp=pop();const auto target=pop();return {MenuSceneInputLeafKind::returned,target,binding,r.eax,0xb424a5};
}
}
