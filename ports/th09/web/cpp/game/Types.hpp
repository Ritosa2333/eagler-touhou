#pragma once
#include <cstdint>
#include <cstring>
#include <cstddef>
namespace th09 {
using u8=std::uint8_t; using u16=std::uint16_t; using u32=std::uint32_t; using u64=std::uint64_t;
using i8=std::int8_t; using i16=std::int16_t; using i32=std::int32_t; using i64=std::int64_t;
struct Vec2 {float x=0, y=0;};
struct Vec3 {float x=0, y=0, z=0;};
inline i32 signed_bits(u32 bits) noexcept {i32 result;std::memcpy(&result,&bits,4);return result;}
inline i32 wrapping_add(i32 a,i32 b) noexcept {return signed_bits(u32(a)+u32(b));}
inline i32 wrapping_sub(i32 a,i32 b) noexcept {return signed_bits(u32(a)-u32(b));}
}
