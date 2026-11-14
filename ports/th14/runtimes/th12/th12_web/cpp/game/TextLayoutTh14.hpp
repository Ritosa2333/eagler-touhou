#pragma once
#include "TextRaster.hpp"
#include "Timer.hpp"
namespace th11 {
// Retail47db50/47dc40/47dd70 prepare physical scratch coordinates. Right and
// centred offsets may be odd, so they cannot be represented as half-size units.
inline TextStyle th14_text_style(TextStyle style,float width,u32 bytes,u32 flags,bool right,bool centered) noexcept {
    const u8 height=u8(style.height);style.height=height?height:17;
    style.plain=(flags&0x800)!=0;style.offset_in_pixels=true;
    style.spacing*=2;
    style.offset=wrapping_add(style.offset,style.offset);
    const u32 extent=u32(style.height*2-1)*bytes;
    if(right)style.offset=truncate_int(float(width-float(extent>>1)));
    if(centered)style.offset=signed_bits(u32(truncate_int(width)/2)-(extent>>2));
    if(style.plain)style.outline=0;
    return style;
}
}
