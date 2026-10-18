#pragma once
#include "Types.hpp"
namespace th09 {
// TH09 1.50a: 0x42ae20 / 0x42ae50 / 0x42ae70 / 0x42aea0.
struct Rng {
    u16 seed=0, reserved=0;
    u32 calls=0;
    u16 next16() noexcept;
    u32 next32() noexcept;
    u16 below(u16 maximum) noexcept {return maximum?next16()%maximum:0;}
    u32 bounded32(u32 maximum) noexcept {return maximum?next32()%maximum:0;}
    double range(float maximum) noexcept {return float(unit())*maximum;}
    double unit() noexcept {return float(next32())*0x1p-32f;}
    double signed_unit() noexcept {return float(next32())*0x1p-31f-1.0f;}
};
static_assert(sizeof(Rng)==8);
}
