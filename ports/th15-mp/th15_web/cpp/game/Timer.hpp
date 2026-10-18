#pragma once
#include "Types.hpp"
#include <cmath>
namespace th15 {
inline i32 truncate_int(float value)noexcept{if(!std::isfinite(value)||value>=2147483648.f||value<-2147483648.f)return INT32_MIN;return static_cast<i32>(value);}
// TH15 uses SSE single precision and a rate-table index rather than a pointer.
struct Timer {
    i32 previous=-999999,current=0;float fractional=0;u32 rate_index=0,flags=0;
    void set(i32 frame)noexcept{if(!(flags&1)){current=0;previous=-999999;fractional=0;rate_index=0;flags|=1;}current=frame;previous=wrapping_add(frame,-1);fractional=float(frame);}
    void tick(const float* rate)noexcept{rate_index=0;previous=current;const float speed=rate?*rate:1.f;if(speed>.99f&&speed<1.01f){current=wrapping_add(current,1);fractional=float(fractional+1.f);}else{fractional=float(fractional+speed);current=truncate_int(fractional);}}
    void decrement(const float* rate)noexcept{rate_index=0;previous=current;const float speed=rate?*rate:1.f;fractional=float(fractional-(speed>.99f&&speed<1.01f?1.f:speed));current=truncate_int(fractional);}
};
static_assert(sizeof(Timer)==20);
}
