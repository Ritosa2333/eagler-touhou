#pragma once
#include "Types.hpp"
#include "../../../portable/numeric/ScalarMath.hpp"

namespace th10 {
// Numeric compatibility for the original game's extended intermediate values.
// This is a number type, not an x86 register, instruction or execution engine.
// The platform adapter chooses precision and rounding for the current context.
struct Extended {
    u64 significand = 0;
    u16 exponent = 0;
    u16 reserved16 = 0;
    u32 reserved32 = 0;
    static Extended from_float(float value) noexcept;
    static Extended from_double(double value) noexcept;
    static Extended from_int(i32 value) noexcept;
    static Extended from_int64(i64 value) noexcept;
    float to_float() const noexcept;
    double to_double() const noexcept;
    i32 truncate_int() const noexcept;
    Extended round_to_integer() const noexcept;
    Extended square_root() const noexcept;
    bool is_nan() const noexcept;
    Extended operator-() const noexcept;
    friend Extended operator+(Extended a, Extended b) noexcept;
    friend Extended operator-(Extended a, Extended b) noexcept;
    friend Extended operator*(Extended a, Extended b) noexcept;
    friend Extended operator/(Extended a, Extended b) noexcept;
    friend bool operator<(Extended a, Extended b) noexcept;
    friend bool operator==(Extended a, Extended b) noexcept;
};
static_assert(sizeof(Extended) == 16);
enum class Precision : u8 { Single = 32, Double = 64, Extended = 80 };
enum class Rounding : u8 { NearestEven = 0, TowardZero = 1, Down = 2, Up = 3 };
void arithmetic_mode(Precision precision, Rounding rounding) noexcept;
bool single_precision_nearest() noexcept;
using Scalar=touhou::numeric::ScalarMath<Extended,single_precision_nearest>;
inline Extended number(float value) noexcept { return Extended::from_float(value); }
}
