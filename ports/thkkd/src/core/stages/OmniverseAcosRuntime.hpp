#pragma once
#include "core/stages/OmniverseAcos.hpp"
namespace kaikeidou {
// MXCSR低六位是运算累积状态，环境检查只比较其控制位。
inline std::uint64_t checked_omniverse_acos_bits(std::uint64_t argument,std::uint32_t sse2_flag,std::uint32_t mxcsr,std::uint32_t x87_control){check_omniverse_acos_environment(sse2_flag,mxcsr&~63u,x87_control);return omniverse_acos_bits(argument);}
}
