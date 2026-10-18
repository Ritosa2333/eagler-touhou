#pragma once
#include "Arithmetic.hpp"
namespace th08 {
// TH08 1.00d 0x43ecc0..0x43edad; the backup seed does not include the call count.
struct Rng {
    u16 seed=0, backup=0;
    u32 calls=0;
    u16 next16() noexcept;
    u32 next32() noexcept;
    u16 bounded16(u16 limit) noexcept {return limit?next16()%limit:0;}
    u32 bounded32(u32 limit) noexcept {return limit?next32()%limit:0;}
    Extended unit() noexcept;
    Extended signed_unit() noexcept;
    Extended range(float limit) noexcept {return unit()*number(limit);}
    Extended signed_range(float limit) noexcept {return signed_unit()*number(limit);}
    void save() noexcept {backup=seed;}
    void restore() noexcept {seed=backup;}
};
static_assert(sizeof(Rng)==8);
}
