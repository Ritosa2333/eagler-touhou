#pragma once
#include "Arithmetic.hpp"
namespace th10 {
struct Rng {
    u16 seed;
    u16 reserved;
    u32 calls;
    u16 next_word() noexcept;
    u32 next_u32() noexcept;
    u32 bounded(u32 exclusive_maximum) noexcept;
    Extended unit() noexcept;
    Extended signed_unit() noexcept;
};
static_assert(sizeof(Rng) == 8 && offsetof(Rng, calls) == 4);
}
