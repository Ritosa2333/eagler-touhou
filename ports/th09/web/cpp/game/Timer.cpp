#include "Timer.hpp"
#include <limits>
namespace th09 {
void Timer::reset(i32 value) noexcept {
    previous=-999999;time=float(value);current=value;
}
void Timer::advance(float amount,float rate,u32 flags) noexcept {
    previous=current;
    // The original game uses x87 single-precision, round-to-nearest arithmetic.
    const float delta=rate>0.99f?float(amount):float(rate)*float(amount);
    time=float(delta+float(time));
    // MSVC's original _ftol returns a 64-bit integer; the timer keeps EAX.
    // Large finite times therefore wrap at 32 bits. Invalid FISTP64 yields
    // 0x8000000000000000 and consequently a zero low word.
    current=time>=-0x1p63 && time<0x1p63 ? signed_bits(u32(i64(time))):0;
    if(flags&0x20){previous=-999999;time=float(float(current)+float(time));}
}
i32 Timer::tick(const FrameTiming& timing) noexcept {
    previous=current;
    if(timing.rate<=0.99f){
        const float next=float(time)+float(timing.rate);time=float(next);
        // FST retains the unrounded value for the following _ftol.
        current=next>=-0x1p63&&next<0x1p63?signed_bits(u32(i64(next))):0;
    }else {current=wrapping_add(current,1);time=float(float(time)+1.0f);}
    return current;
}
}
