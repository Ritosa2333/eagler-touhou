#pragma once
#include "Types.hpp"
namespace th15 {
constexpr bool valid_multiplayer_chapter_threshold(u32 value)noexcept{return value>=500000&&value<=5000000&&value%250000==0;}
constexpr i32 multiplayer_chapter_threshold(unsigned seats,i32 configured=0)noexcept{return configured?configured:seats==3?1500000:seats==2?1250000:1000000;}
}
