#include "Arithmetic.hpp"
#include "../../../portable/numeric/ExactFloat.hpp"

namespace th10 {
namespace {
struct FloatBits32 { u32 value; };
struct FloatBits64 { u64 value; };
extern "C" {
extern u8 softfloat_roundingMode, extF80_roundingPrecision;
void f32_to_extF80M(FloatBits32, Extended*);
void f64_to_extF80M(FloatBits64, Extended*);
void i32_to_extF80M(i32, Extended*);
void i64_to_extF80M(i64, Extended*);
FloatBits32 extF80M_to_f32(const Extended*);
FloatBits64 extF80M_to_f64(const Extended*);
i32 extF80M_to_i32(const Extended*, u8, bool);
i64 extF80M_to_i64(const Extended*, u8, bool);
void extF80M_add(const Extended*, const Extended*, Extended*);
void extF80M_sub(const Extended*, const Extended*, Extended*);
void extF80M_mul(const Extended*, const Extended*, Extended*);
void extF80M_div(const Extended*, const Extended*, Extended*);
void extF80M_roundToInt(const Extended*, u8, bool, Extended*);
void extF80M_sqrt(const Extended*, Extended*);
bool extF80M_lt_quiet(const Extended*, const Extended*);
bool extF80M_eq(const Extended*, const Extended*);
}
}
void arithmetic_mode(Precision precision, Rounding rounding) noexcept {
    extF80_roundingPrecision = static_cast<u8>(precision);
    softfloat_roundingMode = static_cast<u8>(rounding);
}
bool single_precision_nearest() noexcept {
    return extF80_roundingPrecision == 32 && softfloat_roundingMode == 0;
}
Extended Extended::from_float(float value) noexcept {
    Extended fast;if(touhou::numeric::from_float(value,fast))return fast;
    u32 bits; std::memcpy(&bits, &value, sizeof(bits));
    Extended result; f32_to_extF80M({bits}, &result); return result;
}
Extended Extended::from_double(double value) noexcept {
    Extended fast;if(touhou::numeric::from_double(value,fast))return fast;
    u64 bits; std::memcpy(&bits, &value, sizeof(bits));
    Extended result; f64_to_extF80M({bits}, &result); return result;
}
Extended Extended::from_int(i32 value) noexcept {
    return touhou::numeric::from_integer<Extended>(value);
}
Extended Extended::from_int64(i64 value) noexcept {
    return touhou::numeric::from_integer<Extended>(value);
}
float Extended::to_float() const noexcept {
    float fast;if(touhou::numeric::to_float(*this,fast))return fast;
    const u32 bits = extF80M_to_f32(this).value;
    float result; std::memcpy(&result, &bits, sizeof(result)); return result;
}
double Extended::to_double() const noexcept {
    double fast;if(touhou::numeric::to_double(*this,fast))return fast;
    const u64 bits = extF80M_to_f64(this).value;
    double result; std::memcpy(&result, &bits, sizeof(result)); return result;
}
i32 Extended::truncate_int() const noexcept {
    i32 fast;if(touhou::numeric::truncate_low_word(*this,fast))return fast;
    // The original MSVC helper converts to signed 64 bits, then uses EAX.
    // This also preserves its low-word result for out-of-range/NaN inputs.
    const u32 bits = static_cast<u32>(extF80M_to_i64(this, 1, false));
    i32 result; std::memcpy(&result, &bits, sizeof(result)); return result;
}
bool Extended::is_nan() const noexcept {
    return (exponent & 0x7fff) == 0x7fff && (significand & 0x7fffffffffffffffull);
}
Extended Extended::round_to_integer() const noexcept {Extended result;extF80M_roundToInt(this,softfloat_roundingMode,false,&result);return result;}
Extended Extended::square_root() const noexcept {Extended result;if(extF80_roundingPrecision==32&&softfloat_roundingMode==0&&touhou::numeric::square_root(*this,result))return result;extF80M_sqrt(this,&result);return result;}
Extended Extended::operator-() const noexcept { auto result = *this; result.exponent ^= 0x8000; return result; }
Extended operator+(Extended a, Extended b) noexcept { Extended result;if(extF80_roundingPrecision==32&&softfloat_roundingMode==0&&touhou::numeric::binary(a,b,result,0))return result;extF80M_add(&a,&b,&result);return result; }
Extended operator-(Extended a, Extended b) noexcept { Extended result;if(extF80_roundingPrecision==32&&softfloat_roundingMode==0&&touhou::numeric::binary(a,b,result,1))return result;extF80M_sub(&a,&b,&result);return result; }
Extended operator*(Extended a, Extended b) noexcept { Extended result;if(extF80_roundingPrecision==32&&softfloat_roundingMode==0&&touhou::numeric::binary(a,b,result,2))return result;extF80M_mul(&a,&b,&result);return result; }
Extended operator/(Extended a, Extended b) noexcept { Extended result;if(extF80_roundingPrecision==32&&softfloat_roundingMode==0&&touhou::numeric::binary(a,b,result,3))return result;extF80M_div(&a,&b,&result);return result; }
bool operator<(Extended a, Extended b) noexcept { float x,y;if(touhou::numeric::to_float(a,x)&&touhou::numeric::to_float(b,y))return x<y;return extF80M_lt_quiet(&a,&b); }
bool operator==(Extended a, Extended b) noexcept { float x,y;if(touhou::numeric::to_float(a,x)&&touhou::numeric::to_float(b,y))return x==y;return extF80M_eq(&a,&b); }
}
