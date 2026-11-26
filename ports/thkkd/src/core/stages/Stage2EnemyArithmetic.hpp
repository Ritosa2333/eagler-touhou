// 二面首段局部选择器与原 x87 中间落点；不改变共享数值实现。
#pragma once
#include "core/stages/Stage2EnemyConstants.hpp"
#include "core/math/FiniteDivide.hpp"
#include "core/shared/CeilToInt32.hpp"
#include <array>
namespace kaikeidou {
inline std::uint32_t stage2_enemy_pick_integer(std::uint32_t difficulty,const std::array<std::uint32_t,6>& values){return values[difficulty<6?difficulty:3];}
inline std::uint64_t stage2_enemy_pick_real(std::uint32_t difficulty,const std::array<std::uint64_t,6>& values){return values[difficulty<6?difficulty:3];}
inline std::uint64_t stage2_enemy_radians(std::uint64_t value,unsigned precision){using namespace binary_float;return to_binary64(divide(multiply(binary64(value),binary64(stage2_enemy_detail::constant(0x6d1860)),precision),binary64(stage2_enemy_detail::constant(0x6d1858)),precision));}
inline std::uint32_t stage2_enemy_ceil_integer(std::uint64_t value){return ceil_to_int32(value);}
namespace stage2_enemy_detail {
inline std::int64_t signed_word(std::uint32_t value){return value>>31?std::int64_t(value)-0x100000000ll:value;}
inline binary_float::Number subtract(binary_float::Number a,binary_float::Number b,unsigned bits){b.negative=!b.negative;return binary_float::add(a,b,bits);}
}
}
