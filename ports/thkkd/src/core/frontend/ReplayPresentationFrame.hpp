// B4214D末钩子保留原PUSHAD/POPAD、完整ECX累计及返回game_frame的ABI。
#pragma once
#include "core/runtime/GameStartEntryMemory.hpp"
namespace kaikeidou {
enum class ReplayPresentationFrameKind {returned,memory_fault};
struct ReplayPresentationFrameOutcome {
 ReplayPresentationFrameKind kind=ReplayPresentationFrameKind::returned;
 std::uint32_t source=0xb42183,target=0x653444,fault_address=0,fault_bytes=0;
 bool writing=false;
};
namespace replay_presentation_frame_detail {
inline void add_flags(GameStartEntryRegisters& r,std::uint32_t a,std::uint32_t b,bool preserve_carry=false){
 const auto wide=std::uint64_t(a)+b;const auto v=static_cast<std::uint32_t>(wide);unsigned parity=0;
 for(unsigned i=0;i<8;++i)parity+=(v>>i)&1u;
 const auto carry=preserve_carry?(r.eflags&1u):(wide>>32?1u:0u);
 r.eflags=(r.eflags&~0x8d5u)|carry|((parity&1u)?0u:4u)|((a^b^v)&16u)|(v==0?64u:0u)|((v>>24)&128u)|(((~(a^b)&(a^v))>>20)&0x800u);
}
inline void zero_compare_flags(GameStartEntryRegisters& r,std::uint32_t value){
 unsigned parity=0;for(unsigned i=0;i<8;++i)parity+=(value>>i)&1u;
 r.eflags=(r.eflags&~0x8d5u)|((parity&1u)?0u:4u)|(value==0?64u:0u)|((value>>24)&128u);
}
}
template<class Memory>ReplayPresentationFrameOutcome execute_replay_presentation_frame(Memory& memory,GameStartEntryMachineFrame& frame){
 using U=std::uint32_t;auto& r=frame.registers;U pc=0xb4214d;
 const auto read=[&](U at,U address){pc=at;return memory.word(address);};
 const auto write=[&](U at,U address,U value){pc=at;memory.word(address,value);};
 try{
  r.eax=read(0xb4214d,0xb3e2e8);const auto mode=read(0xb42152,0xb4310c);replay_presentation_frame_detail::zero_compare_flags(r,mode);
  const auto saved=r;const std::array<U,8> values{{saved.eax,saved.ecx,saved.edx,saved.ebx,saved.esp,saved.ebp,saved.esi,saved.edi}};
  for(U i=0;i<8;++i)write(mode?0xb421a7:0xb4215f,saved.esp-(i+1u)*4u,values[i]);r.esp-=32u;
  if(mode==0){
   pc=0xb42160;r.ecx=(r.ecx&0xffffff00u)|memory.byte(0xb43108);r.edx=read(0xb42166,0xb43110);r.edi=read(0xb4216c,0xb43100);
   pc=0xb42172;memory.byte(r.edx+r.edi,static_cast<std::uint8_t>(r.ecx));
   replay_presentation_frame_detail::add_flags(r,r.edx,1,true);++r.edx;write(0xb42176,0xb43110,r.edx);
  }else{
   r.edx=read(0xb421a8,0xb43110);replay_presentation_frame_detail::add_flags(r,r.edx,1,true);++r.edx;write(0xb421af,0xb43110,r.edx);
  }
  const auto accumulator=read(0xb4217c,0xb43c04);write(0xb4217c,0xb43c04,accumulator+r.ecx);replay_presentation_frame_detail::add_flags(r,accumulator,r.ecx);
  // POPAD跳过保存的ESP槽，最终ESP回到入口；原栈字节保留。
  const auto pop=[&](){const auto value=read(0xb42182,r.esp);r.esp+=4u;return value;};
  r.edi=pop();r.esi=pop();r.ebp=pop();r.esp+=4u;r.ebx=pop();r.edx=pop();r.ecx=pop();r.eax=pop();
  return {};
 }catch(const GameStartEntryAccessFault& fault){return {ReplayPresentationFrameKind::memory_fault,pc,pc,fault.address,fault.bytes,fault.writing};}
}
}
