// 损坏Replay续接的有界出口；寄存器有效位明确区别生产可消费值与依赖内部volatile。
#pragma once
#include "core/runtime/GameStartEntryContext.hpp"
namespace kaikeidou {
enum class ReplayFailureContinuationKind {returned_target,read_fault,write_fault,fetch_transfer};
enum ReplayFailureRegister : std::uint32_t {failure_eax=1,failure_ecx=2,failure_edx=4,failure_ebx=8,failure_esp=16,failure_ebp=32,failure_esi=64,failure_edi=128,failure_eflags=256};
struct ReplayFailureContinuationPolicy {
 std::vector<std::uint32_t> registered_return_targets;
 std::vector<std::uint32_t> open_replay_handles;
};
struct ReplayFailureContinuationOutcome {
 ReplayFailureContinuationKind kind=ReplayFailureContinuationKind::fetch_transfer;
 std::uint32_t pc=0x64423d,address=0,bytes=0,target=0;
 GameStartEntryRegisters registers;
 std::uint32_t exact_registers=failure_ebx|failure_esp|failure_ebp|failure_esi|failure_edi;
 std::vector<std::uint32_t> open_replay_handles;
};
}
