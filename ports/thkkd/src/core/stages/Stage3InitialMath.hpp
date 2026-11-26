// 原 67A9AB..67A9C6：FILD 有符号整数、FDIVR 原 binary64 常量，最后 FSTP 到 binary64。
#pragma once
#include "core/math/FiniteDivide.hpp"

namespace kaikeidou {
inline std::uint64_t stage3_initial_turn_rate(std::uint32_t denominator, unsigned precision) {
    if (!denominator) return 0x7ff0000000000000ull;
    using namespace binary_float;
    return to_binary64(divide(binary64(0x401921fb54442d28ull), integer(denominator), precision));
}
}
