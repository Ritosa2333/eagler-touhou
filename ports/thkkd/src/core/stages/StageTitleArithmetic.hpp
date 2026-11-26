#pragma once
#include "core/stages/Stage5BossFinalArithmetic.hpp"
namespace kaikeidou {
inline std::int64_t stage_title_signed(std::uint32_t word){return word>>31?std::int64_t(word)-0x100000000ll:word;}
inline std::uint32_t stage_title_integer(binary_float::Number value){return stage5_boss_final_color_integer(value,true);}
}
