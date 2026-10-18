#pragma once
#include "Arithmetic.hpp"
namespace th08 {
struct FrameTiming {
    float rate=1;
    bool force_step=false; // Supervisor flags bit 5; applied by increment/decrement only.
};
struct Timer {
    i32 previous=-999;
    float fraction=0;
    i32 current=0;
    void set(i32 value) noexcept {previous=-999;fraction=0;current=value;}
    i32 tick(const FrameTiming& timing) noexcept;
    void increment(i32 amount,const FrameTiming& timing) noexcept;
    void decrement(i32 amount,const FrameTiming& timing) noexcept;
    Extended value() const noexcept {return Extended::from_int(current)+number(fraction);}
    bool changed() const noexcept {return previous!=current;}
};
static_assert(sizeof(Timer)==12);
}
