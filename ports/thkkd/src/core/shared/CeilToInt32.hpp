// 原4F4040的SSE ceil再经6968C0转换；无效转换保持80000000位型。
#pragma once
#include "core/shared/FloorToInt32.hpp"
namespace kaikeidou {
inline std::uint32_t ceil_to_int32(std::uint64_t argument) noexcept {
    // 对称取整与无符号取负也保留int32端点、NaN和无穷输入的转换结果。
    return 0u-floor_to_int32(argument^0x8000000000000000ull);
}
}
