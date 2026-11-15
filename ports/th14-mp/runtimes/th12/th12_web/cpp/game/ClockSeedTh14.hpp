#pragma once
#include "Rng.hpp"
namespace th11 {
// Retail444a24/444a2a writes the low word of timeGetTime into both
// streams. Counts and reserved words are not changed by these stores.
inline void th14_seed_clock(u32 ticks,Rng& visual,Rng& gameplay)noexcept{
    gameplay.set_seed(u16(ticks));visual.set_seed(u16(ticks));
}
}
