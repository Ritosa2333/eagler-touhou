// th16 基础类型：与原版 32 位 MSVC 布局一致的定宽整数与向量。
#pragma once
#include <cstdint>
#include <cstring>
#include <vector>
#include <string>

namespace th16 {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

struct Vec2 { float x = 0, y = 0; };
struct Vec3 { float x = 0, y = 0, z = 0; };

inline u16 read_u16(const u8* p) { return u16(p[0] | (p[1] << 8)); }
inline u32 read_u32(const u8* p) { return u32(p[0]) | (u32(p[1]) << 8) | (u32(p[2]) << 16) | (u32(p[3]) << 24); }
inline i32 read_i32(const u8* p) { return i32(read_u32(p)); }
inline float read_f32(const u8* p) { float f; std::memcpy(&f, p, 4); return f; }
inline i32 signed_bits(u32 bits) noexcept { i32 r; std::memcpy(&r, &bits, 4); return r; }
inline float float_from_bits(u32 bits) noexcept { float r; std::memcpy(&r, &bits, 4); return r; }
inline u32 float_to_bits(float v) noexcept { u32 r; std::memcpy(&r, &v, 4); return r; }
inline i32 wrapping_add(i32 a, i32 b) noexcept { return signed_bits(u32(a) + u32(b)); }
inline i32 wrapping_sub(i32 a, i32 b) noexcept { return signed_bits(u32(a) - u32(b)); }
inline i32 truncate_int(float v) noexcept { return i32(v); }  // CVTTSS2SI

}  // namespace th16
