#pragma once
#include <cstdint>
#include <cstring>
#include <immintrin.h>

// Manually recovered from the user's th20.exe (embedded version string 1.00c).
// VA comments refer to image base 0x00400000. See reports/native_validation.json.
// These are engine components, not a complete playable engine.
namespace th20::recovered {
inline std::int32_t signed_bits(std::uint32_t x) noexcept {
    std::int32_t r; std::memcpy(&r, &x, 4); return r;
}
inline float add32(float a, float b) noexcept {
    return _mm_cvtss_f32(_mm_add_ss(_mm_set_ss(a), _mm_set_ss(b)));
}
inline float mul32(float a, float b) noexcept {
    return _mm_cvtss_f32(_mm_mul_ss(_mm_set_ss(a), _mm_set_ss(b)));
}
inline float int_float(std::int32_t x) noexcept {
    return _mm_cvtss_f32(_mm_cvtsi32_ss(_mm_setzero_ps(), x));
}
inline std::int32_t truncate32(float x) noexcept {
    // CVTTSS2SI produces INT_MIN for NaN/out-of-range; a C++ cast would be UB.
    return _mm_cvttss_si32(_mm_set_ss(x));
}

// 0x00422cb0, including its behavior for raw states outside the normal seed range.
inline std::uint32_t lcg_step(std::uint32_t state) noexcept {
    const std::uint64_t product = std::uint64_t{state} * 48271u;
    std::uint64_t folded = (product >> 31) + (product & 0x7fffffffu);
    if (folded >= 0x7fffffffu) folded -= 0x7fffffffu;
    return static_cast<std::uint32_t>(folded);
}
// 0x004235f0. Higher RNG wrappers also lock and apply a configurable modulus.
inline std::uint32_t lcg_next(std::uint32_t& state) noexcept {
    state = lcg_step(state); return state;
}

struct Timer {
    std::int32_t previous;
    std::int32_t current;
    float current_f;
    std::uint32_t flags;
};
static_assert(sizeof(Timer) == 16);

// 0x00423f50: does not modify flags.
inline void timer_reset(Timer& t) noexcept {
    t.current = 0; t.previous = -999999; t.current_f = 0.0f;
}
// 0x00423fe0.
inline void timer_mode(Timer& t, std::uint32_t mode) noexcept {
    t.flags = (t.flags & ~6u) | ((mode & 3u) << 1);
}
inline void timer_initialize_if_needed(Timer& t) noexcept {
    if (!(t.flags & 1u)) { timer_reset(t); timer_mode(t, 0); t.flags |= 1u; }
}
// 0x00423f80 (0x00423520 forwards here).
inline void timer_set(Timer& t, std::int32_t value) noexcept {
    timer_initialize_if_needed(t);
    t.current = value;
    t.current_f = int_float(value);
    t.previous = signed_bits(static_cast<std::uint32_t>(value) - 1u);
}
inline bool near_one(float rate) noexcept {
    return rate > 0.99f && rate < 1.01f;
}
// 0x004530f0. rate is the float at the global clock pointer 0x005aefe0,
// or nullptr when that pointer is null. Clock storage ownership is not recovered.
inline void timer_add(Timer& t, float delta, const float* rate) noexcept {
    timer_initialize_if_needed(t);
    t.flags &= ~6u;
    t.previous = t.current;
    const float step = !rate || near_one(*rate) ? delta : mul32(*rate, delta);
    t.current_f = add32(t.current_f, step);
    t.current = truncate32(t.current_f);
}
// 0x004533b0. The near-one/null paths increment the integer independently.
inline std::int32_t timer_tick(Timer& t, const float* rate) noexcept {
    timer_initialize_if_needed(t);
    t.flags &= ~6u;
    t.previous = t.current;
    if (!rate || near_one(*rate)) {
        t.current = signed_bits(static_cast<std::uint32_t>(t.current) + 1u);
        t.current_f = add32(t.current_f, 1.0f);
    } else {
        t.current_f = add32(t.current_f, *rate);
        t.current = truncate32(t.current_f);
    }
    return t.current;
}
}
