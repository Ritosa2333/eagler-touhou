#pragma once
#include "Types.hpp"
namespace th09 {
struct FrameTiming {float rate=1;bool force_step=false;};
// TH09 stores the full floating time at +4 (not TH08's fractional remainder).
struct Timer {
    i32 previous=-999999;
    float time=0;
    i32 current=0;
    void reset(i32 value=0) noexcept;
    void advance(float amount,float rate=1.0f,u32 flags=0) noexcept;
    void set(i32 v) noexcept {reset(v);}
    i32 tick(const FrameTiming&) noexcept;
    void decrement(i32 n,const FrameTiming& timing) noexcept {advance(float(wrapping_sub(0,n)),timing.rate,timing.force_step?32:0);}
    float value() const noexcept {return time;}
};
static_assert(sizeof(Timer)==12);
}
