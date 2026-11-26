// 暂停页新增字体；共享场景与计数沿原地址视图持有。
#pragma once
#include <cstdint>
namespace kaikeidou {
struct PauseGameoverContext {std::uint32_t font=0;}; // A74BDC。
// 只保存本体实际消费/改变的ABI寄存器；其他caller-save属于显示/平台边界。
struct PauseGameoverAbi {std::uint32_t eax=0,edi=0;std::uint32_t instruction=0;};
}
