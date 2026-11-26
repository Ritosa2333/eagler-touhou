// 游戏体与Replay末钩子的原指令分界；寄存器位掩码只声明实际已恢复的值。
#pragma once
#include "core/runtime/GameStartEntryMemory.hpp"
namespace kaikeidou {
enum class OuterSceneCallKind {completed,transfer};
enum OuterSceneRegister:std::uint32_t {
 outer_eax=1,outer_ecx=2,outer_edx=4,outer_ebx=8,outer_esp=16,
 outer_ebp=32,outer_esi=64,outer_edi=128,outer_eflags=256
};
struct OuterSceneCallOutcome {
 OuterSceneCallKind kind=OuterSceneCallKind::completed;
 std::uint32_t source=0,target=0,exact_registers=0;
};
struct OuterSceneGameplayServices {
 virtual ~OuterSceneGameplayServices()=default;
 // 6533F9..65343E：完成14个原调用，正常source65343A、target65343F。
 virtual OuterSceneCallOutcome gameplay_body(GameStartEntryMemory& memory,GameStartEntryMachineFrame& frame)=0;
 // B4214D末钩子：正常sourceB42183、target653444，EAX返回原game_frame。
 virtual OuterSceneCallOutcome replay_after_gameplay(GameStartEntryMemory& memory,GameStartEntryMachineFrame& frame)=0;
};
}
