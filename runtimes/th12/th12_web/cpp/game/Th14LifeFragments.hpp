#pragma once
#include "Types.hpp"
namespace th11 {
// Retail 4d57a0: 49 entries of three, followed by the 99,999,999 sentinel.
// Indices outside 0..49 are not valid progression states.
inline i32 th14_life_fragment_requirement(i32 awards)noexcept{return awards<49?3:99999999;}
}
