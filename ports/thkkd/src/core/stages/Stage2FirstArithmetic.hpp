// 二面首段的度转弧度边界；原5E3400返回后先落binary64。
#pragma once
#include "core/math/FiniteDivide.hpp"
#include "core/stages/Stage2FirstConstants.hpp"
namespace kaikeidou {
inline std::uint64_t stage2_first_degrees(std::uint64_t angle,unsigned precision){
    using namespace binary_float;using stage2_first_detail::constant;
    return to_binary64(divide(multiply(binary64(angle),binary64(constant(0x6d1860)),precision),binary64(constant(0x6d1858)),precision));
}
}
