#include "ChargeGauge.hpp"
namespace th09 {
namespace {i32 original_int(float v){return v>=-0x1p63&&v<0x1p63?signed_bits(u32(i64(v))):0;}}
i32 ChargeGauge::add(float amount,u32 character) noexcept {
    const i32 before=original_int(value)/100;
    const float sum=float(amount)+float(value);
    value=float(character==1?float(amount)*float(0.15f)+sum:sum);
    if(value>=400.0f)value=400.0f;
    const i32 after=original_int(value)/100;
    return before==after?-1:after;
}
}
