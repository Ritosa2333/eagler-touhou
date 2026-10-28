// th13 基础数值类型与位级助手（对齐 th15 web/cpp/game/Types.hpp 的拆分约定）。
// 全部为纯函数，无平台依赖，可进 wasm/SDL3 双目标。
#pragma once

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace th13 {

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

// 位重现（bit-cast）助手：ECL 栈条目以原始 32 位存储，int/float 视图切换必须走位复制。
inline i32 signed_bits(u32 bits) noexcept {
    i32 result;
    std::memcpy(&result, &bits, 4);
    return result;
}

inline float float_from_bits(u32 bits) noexcept {
    float result;
    std::memcpy(&result, &bits, 4);
    return result;
}

inline u32 float_to_bits(float value) noexcept {
    u32 result;
    std::memcpy(&result, &value, 4);
    return result;
}

// 环绕算术：脚本整数运算不做未定义行为，按原版 32 位补码环绕。
inline i32 wrapping_add(i32 a, i32 b) noexcept { return signed_bits(u32(a) + u32(b)); }
inline i32 wrapping_sub(i32 a, i32 b) noexcept { return signed_bits(u32(a) - u32(b)); }
inline i32 wrapping_mul(i32 a, i32 b) noexcept { return signed_bits(u32(a) * u32(b)); }

// 几何载体：ANM 可视状态与世界坐标共用（对齐 th15 web/cpp/game/Types.hpp）。
struct Vec2 {
    float x = 0, y = 0;
};

struct Vec3 {
    float x = 0, y = 0, z = 0;
};

struct Matrix4 {
    float m[16]{};
    void identity() noexcept {
        std::memset(m, 0, sizeof m);
        m[0] = m[5] = m[10] = m[15] = 1;
    }
};

// 向零截断（Timer 等浮点帧计数用；对齐 th15 Timer 的 truncate_int）。
inline i32 truncate_int(float value) noexcept {
    if (!std::isfinite(value) || value >= 2147483648.f || value < -2147483648.f) return INT32_MIN;
    return static_cast<i32>(value);
}

// 就近取整（banker's rounding，x87 fistp 默认舍入模式）。
// 依据：th13.exe 0x4971f0 为 MSVC _ftol 家族（ROUND + 负数修正），
// ECL 取参在读到 'f' 型表达式栈条目转 int 时调用它（0x478e60/0x477030 内 call 0x4971f0）。
inline i32 nearest_int(float value) noexcept {
    if (!std::isfinite(value) || value >= 2147483648.f || value < -2147483648.f) return INT32_MIN;
    return static_cast<i32>(std::nearbyint(value));
}

// 角度归一到 (-π, π]（原版 0x497... 家族；th15 normalize_angle 同款行为）。
inline float normalize_angle(float angle) noexcept {
    constexpr float kPi = 3.1415927410125732421875f;
    constexpr float kTwoPi = 6.283185482025146484375f;
    angle = static_cast<float>(std::fmod(double(angle), double(kTwoPi)));
    if (angle > kPi) angle = static_cast<float>(angle - kTwoPi);
    else if (angle < -kPi) angle = static_cast<float>(angle + kTwoPi);
    return angle;
}

}  // namespace th13
