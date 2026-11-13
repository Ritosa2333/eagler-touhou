#pragma once
#include "Types.hpp"
#include "RateScalarTh14.hpp"
#include <cmath>
namespace th11 {
inline i32 truncate_int(double value) noexcept {
    if(!std::isfinite(value)||value>=2147483648.0||value<-2147483648.0)return INT32_MIN;
    return static_cast<i32>(value);
}
struct Timer {
    i32 previous=-999999,current=0;
    float fractional=0;
    const float* rate=nullptr;
    u32 flags=0;
    void set(i32 frame,const float* default_rate) noexcept {
        if(!(flags&1)){rate=default_rate;flags|=1;}
        previous=wrapping_add(frame,-1);current=frame;fractional=float(frame);
    }
    void tick() noexcept {
        previous=current;
        // Retail408690 and the inline Player clocks treat a null rate as1.
        const float speed=kPortTitle==14&&!rate?1.f:th14_rate_load(rate);
        if(speed>0.99f&&speed<1.01f){current=wrapping_add(current,1);fractional=float(double(fractional)+1);}
        else {fractional=float(double(fractional)+speed);current=truncate_int(fractional);}
    }
    void advance(float frames) noexcept {
        previous=current;const float speed=th14_rate_load(rate);
        const float delta=speed>0.99f&&speed<1.01f?frames:float(double(speed)*frames);
        fractional=float(double(fractional)+delta);current=truncate_int(fractional);
    }
};
}
