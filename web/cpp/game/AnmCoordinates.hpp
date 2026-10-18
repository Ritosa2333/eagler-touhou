#pragma once
#include "AnmVm.hpp"
namespace th15 {
bool anm_position(AnmVm&,Vec3& out,u32 depth=0)noexcept;
bool anm_adjust_position(AnmVm&,Vec3&,u32 depth=0)noexcept;
bool anm_quad_positions(AnmVm&,Vec3 (&out)[4])noexcept;
}
