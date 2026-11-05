#pragma once
#include "Arithmetic.hpp"
namespace th10 {
// The caller passes the rate sampled at animate entry: later VM writes may
// alias AnmEnvironment::rate. Do not re-read the environment for this guard.
inline bool anm_stationary_uv_mode(float rate) noexcept {
#if defined(TH_ENABLE_MULTIPLAYER_GAMEPLAY) && defined(__wasm__)
    u32 bits;std::memcpy(&bits,&rate,sizeof(bits));
    return bits==0x3f800000u&&single_precision_nearest();
#else
    (void)rate;return false;
#endif
}
// Under the admitted rate/mode, signed-zero velocity times +1 and its sum
// with +0 or a positive normal f32 in [0,1) are exact and flag-free. The
// unchanged result cannot take either wrap branch. Reject -0 offsets,
// subnormals, nonfinite values and every nonzero velocity. No VM state is cached.
inline bool anm_stationary_uv(float value,float velocity,bool mode) noexcept {
    u32 value_bits,velocity_bits;std::memcpy(&value_bits,&value,sizeof(value_bits));std::memcpy(&velocity_bits,&velocity,sizeof(velocity_bits));
    return mode&&(velocity_bits&0x7fffffffu)==0&&(value_bits==0||(value_bits>=0x00800000u&&value_bits<0x3f800000u));
}
}
