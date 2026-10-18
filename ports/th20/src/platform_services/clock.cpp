#include "clock.hpp"
#include <cstring>
#include <emmintrin.h>
#include <float.h>

namespace th20::source::platform {
namespace {
double add(double a, double b) noexcept { return _mm_cvtsd_f64(_mm_add_sd(_mm_set_sd(a), _mm_set_sd(b))); }
double sub(double a, double b) noexcept { return _mm_cvtsd_f64(_mm_sub_sd(_mm_set_sd(a), _mm_set_sd(b))); }
double mul(double a, double b) noexcept { return _mm_cvtsd_f64(_mm_mul_sd(_mm_set_sd(a), _mm_set_sd(b))); }
double div(double a, double b) noexcept { return _mm_cvtsd_f64(_mm_div_sd(_mm_set_sd(a), _mm_set_sd(b))); }
double signed32(std::uint32_t value) noexcept {
    std::int32_t bits; std::memcpy(&bits, &value, 4);
    return _mm_cvtsd_f64(_mm_cvtsi32_sd(_mm_setzero_pd(), bits));
}
}
double signed_counter_to_double(std::uint64_t value) noexcept {
    const auto low = static_cast<std::uint32_t>(value);
    const auto high = static_cast<std::uint32_t>(value >> 32);
    // MOVD/ORPS constructs 2^52 + low, then SUBSD subtracts 2^52.
    const std::uint64_t bits = 0x4330000000000000ull | low;
    double low_double; std::memcpy(&low_double, &bits, 8);
    low_double = sub(low_double, 4503599627370496.0);
    return high ? add(low_double, mul(signed32(high), 4294967296.0)) : low_double;
}
double performance_clock(std::uint64_t sample, std::uint64_t origin,
                         std::uint64_t frequency, double& offset) noexcept {
    const double now = div(signed_counter_to_double(sample - origin), signed_counter_to_double(frequency));
    if (offset > now) offset = now;
    return sub(now, offset);
}
double multimedia_clock(std::uint32_t milliseconds, double& offset) noexcept {
    const double now = add(signed32(milliseconds), (milliseconds >> 31) ? 4294967296.0 : 0.0);
    if (offset > now) offset = now;
    // Keep the original mixed-unit comparison and MULSD/SUBSD/DIVSD sequence.
    return div(sub(now, mul(offset, 1000.0)), 1000.0);
}
bool set_rounding_mode(std::uint32_t mode) noexcept {
    if ((mode & 0x300u) != mode) return true;
    // The original CRT changes the rounding bits of BOTH floating-point units
    // and preserves their separately configured exception/precision settings.
    unsigned x87 = 0, sse = 0;
    __control87_2(mode, _MCW_RC, &x87, &sse);
    return (x87 & _MCW_RC) != mode || (sse & _MCW_RC) != mode;
}
}
