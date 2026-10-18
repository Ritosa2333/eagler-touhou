#pragma once
#include "Types.hpp"
namespace th11 {
// TH11 1.00a: 0x458bc0 / 0x458c30. next32 packs pre-rotation words.
struct Rng {
    u16 seed=0,reserved=0;u32 calls=0;
    u16 next16() noexcept {const u16 mixed=u16((seed^0x9630u)-0x6553u);seed=u16((mixed<<2)|(mixed>>14));++calls;return seed;}
    u32 next32() noexcept {const u16 high=u16((seed^0x9630u)-0x6553u);const u16 middle=u16((high<<2)|(high>>14));const u16 low=u16((middle^0x9630u)-0x6553u);seed=u16((low<<2)|(low>>14));calls+=2;return (u32(high)<<16)|low;}
    u32 next_four() noexcept {const u16 lo=next16(),hi=next16();next16();next16();return (u32(hi)<<16)|lo;}
    float unit() noexcept {return float(double(next32())*0x1p-32);}
    float signed_unit() noexcept {return float(double(next32())*0x1p-31-1);}
};
static_assert(sizeof(Rng)==8);
}
