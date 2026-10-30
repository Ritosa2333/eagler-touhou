#pragma once
#include "Types.hpp"
namespace th17 {
// Original 1.00b: 0x402740 / 0x4027b0 / 0x402840 / 0x402880.
// next32 combines the two words before rotating them into the next seed.
struct Rng {
    u16 seed=0,reserved=0;
    u32 calls=0;
    u16 next16() noexcept {
        const u16 mixed=u16((seed^0x9630u)-0x6553u);
        seed=u16((mixed<<2)|(mixed>>14));
        ++calls;
        return seed;
    }
    u32 next32() noexcept {
        const u16 high=u16((seed^0x9630u)-0x6553u);
        const u16 middle=u16((high<<2)|(high>>14));
        const u16 low=u16((middle^0x9630u)-0x6553u);
        seed=u16((low<<2)|(low>>14));
        calls+=2;
        return (u32(high)<<16)|low;
    }
    float unit() noexcept {return float(float(next32())*0x1p-32f);}
    float signed_unit() noexcept {return float(float(float(next32())*0x1p-31f)-1.f);}
};
static_assert(sizeof(Rng)==8);
}
