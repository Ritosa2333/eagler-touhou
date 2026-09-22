#pragma once
#include "Arithmetic.hpp"
namespace th10 {
struct Timer {
    i32 previous;
    i32 current;
    float fractional;
    const float* rate;
    void reset() noexcept;
    void initialize(i32 previous_value) noexcept;
    i32 tick() noexcept;
    void advance(float frames) noexcept;
    // F4 time lock. 0x40e5b0 nops the unscaled branch's `inc edx`, so
    // `previous` and `fractional` still advance while `current` is held.
    i32 tick_time_locked() noexcept;
};
static_assert(offsetof(Timer, fractional) == 8 && offsetof(Timer, rate) == 12);
}
