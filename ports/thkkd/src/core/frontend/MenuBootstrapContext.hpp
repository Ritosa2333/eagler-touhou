// 冷启动新增的可观察数据；资源缺口在已有引用绑定后按原区间单独持有。
#pragma once
#include <array>
#include <cstdint>
#include <vector>
namespace kaikeidou {
struct MenuBootstrapContext {
 std::array<std::uint32_t,1024> random_table{}; // A77558..A78557。
 std::uint32_t scalar_a74e90=0;
};
struct MenuBootstrapGaps {
 struct Block {std::uint32_t address=0;std::vector<std::uint8_t> bytes;};
 std::vector<Block> blocks;
};
struct MenuBootstrapRegisters {std::uint32_t eax=0,ecx=0,edx=0;};
struct MenuBootstrapPairRegisters {std::uint32_t eax=0,ecx=0,known=0;};
struct MenuBootstrapFault {std::uint32_t pc=0,address=0,bytes=0;bool writing=false;};
enum class MenuBootstrapOutcomeKind {completed,memory_fault,platform_output_fault};
struct MenuBootstrapOutcome {
 MenuBootstrapOutcomeKind kind=MenuBootstrapOutcomeKind::completed;
 std::uint32_t target=0x65345a,fault_pc=0,fault_address=0,fault_bytes=0;
 bool write_fault=false;MenuBootstrapRegisters registers;
};
}
