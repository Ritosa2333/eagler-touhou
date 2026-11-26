#pragma once
#include "core/math/BinaryFloat.hpp"
namespace kaikeidou {
namespace stage3_first_detail {
inline std::int64_t signed_word(std::uint32_t v){return v>>31?std::int64_t(v)-0x100000000ll:std::int64_t(v);}
// 原floor/ceil输入仅为固定局部循环索引0..30的一半，精确整数结果保持原调用位置。
inline binary_float::Number floor_half(std::uint32_t i){return binary_float::integer(i/2);}
inline binary_float::Number ceil_half(std::uint32_t i){return binary_float::integer((i+1)/2);}
}
}
