// 连续探针的标题返回末帧使用与Outer相同的真实Replay寄存器桥。
#pragma once
#include "core/runtime/GameplayFrameRuntime.hpp"
namespace kaikeidou {
template<class Fixture,class Runtime>void advance_gameplay_probe_replay_tail(Fixture& fixture,Runtime& runtime,GameStartEntryMemory& memory,GameStartEntryMachineFrame& frame,std::uint32_t tick,std::uint32_t game_frame){
 if(fixture.advanced)throw std::runtime_error("Replay tail called twice");memory.reconcile();
 const auto entry_ecx=frame.registers.ecx,accumulator=memory.word(0xb43c04);
 const std::array<std::uint32_t,7> before{{tick,game_frame,entry_ecx,accumulator,memory.word(0xb43110),memory.word(0xb4310c),memory.word(0xb43108)}};
 for(unsigned i=0;i<before.size();++i)if(before[i]!=fixture.expected[i])throw std::runtime_error("Replay frame bridge entry differs");
 const auto result=runtime.replay_after_gameplay(memory,frame);
 if(result.kind!=OuterSceneCallKind::completed||result.target!=0x653444||frame.registers.eax!=game_frame||frame.registers.ecx!=entry_ecx)throw std::runtime_error("Replay frame bridge return differs");
 // 此真实尾帧的独立堆缓冲不重叠累计器；差值为原B42182观察点的加数ECX。
 const auto after_accumulator=memory.word(0xb43c04),addition_ecx=after_accumulator-accumulator;
 const auto cursor=memory.word(0xb43110),mode=memory.word(0xb4310c),buttons=memory.word(0xb43108);
 const std::array<std::uint32_t,5> after{{addition_ecx,after_accumulator,cursor,mode,buttons}};
 for(unsigned i=0;i<after.size();++i)if(after[i]!=fixture.expected[i+7])throw std::runtime_error("Replay frame bridge state differs");
 fixture.result={{tick,game_frame,entry_ecx,addition_ecx,cursor,mode,buttons,after_accumulator,static_cast<std::uint32_t>(fixture.buffer.size())}};fixture.advanced=true;
}
}
