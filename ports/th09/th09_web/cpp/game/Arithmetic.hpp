#pragma once
#include "Types.hpp"
namespace th09 {
// x87 24-bit arithmetic expressed with ordinary C++ float, with explicit
// float stores at the original storage boundaries. No Extended runtime.
inline float number(float value) noexcept {return value;}
inline i32 truncate(double value) noexcept {return value>=-0x1p63&&value<0x1p63?signed_bits(u32(i64(value))):0;}
struct Scalar {
    static float add(float a,float b) noexcept {return a+b;}
    static float sub(float a,float b) noexcept {return a-b;}
    static float mul(float a,float b) noexcept {return a*b;}
    static float div(float a,float b) noexcept {return a/b;}
    static i32 truncate(float value) noexcept {return th09::truncate(value);}
};
}
