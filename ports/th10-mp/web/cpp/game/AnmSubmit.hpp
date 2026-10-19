#pragma once
#include "AnmRenderer.hpp"
namespace th10 {
// Signed-zero offsets are identities for finite normal values in every
// arithmetic mode. Equal signed zeros are also exact; mixed zeros, subnormals
// and nonfinite values retain the original Scalar path and its flags.
inline float anm_submission_add(float value,float offset) noexcept {
#if defined(TH_ENABLE_MULTIPLAYER_GAMEPLAY) && defined(__wasm__)
    u32 value_bits,offset_bits;
    std::memcpy(&value_bits,&value,sizeof(value_bits));std::memcpy(&offset_bits,&offset,sizeof(offset_bits));
    const u32 magnitude=value_bits&0x7fffffffu;
    if((offset_bits&0x7fffffffu)==0&&((magnitude>=0x00800000u&&magnitude<0x7f800000u)||value_bits==offset_bits))return value;
#endif
    return Scalar::add(value,offset);
}
inline bool anm_submission_outside(float max_x,float max_y,float min_x,float min_y,const RenderViewport& viewport) noexcept {
#if defined(TH_ENABLE_MULTIPLAYER_GAMEPLAY) && defined(__wasm__)
    const u32 right=viewport.x+viewport.width,bottom=viewport.y+viewport.height;
    const auto finite=[](float value){u32 bits;std::memcpy(&bits,&value,sizeof(bits));return (bits&0x7fffffffu)<0x7f800000u;};
    // These unsigned coordinates are exact f32 integers. For finite bounds,
    // conversion and ordered Extended comparison are exact and flag-free.
    // Preserve wrapped u32 endpoints and the original short-circuit order.
    if(viewport.x<=0x01000000u&&viewport.y<=0x01000000u&&right<=0x01000000u&&bottom<=0x01000000u&&finite(max_x)&&finite(max_y)&&finite(min_x)&&finite(min_y))
        return max_x<float(viewport.x)||max_y<float(viewport.y)||float(right)<min_x||float(bottom)<min_y;
#endif
    const auto unsigned_number=[](u32 bits){auto result=Extended::from_int(static_cast<i32>(bits));if(bits&0x80000000u)result=result+number(4294967296.0f);return result;};
    return number(max_x)<unsigned_number(viewport.x)||number(max_y)<unsigned_number(viewport.y)||unsigned_number(viewport.x+viewport.width)<number(min_x)||unsigned_number(viewport.y+viewport.height)<number(min_y);
}
}
