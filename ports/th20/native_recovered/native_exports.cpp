#include "native_core.hpp"
using namespace th20::recovered;
#ifdef _WIN32
#define API extern "C" __declspec(dllexport)
#else
#define API extern "C"
#endif
API std::uint32_t th20_lcg_step(std::uint32_t x) { return lcg_step(x); }
API std::uint32_t th20_lcg_next(std::uint32_t* x) { return lcg_next(*x); }
API void th20_timer_reset(Timer* t) { timer_reset(*t); }
API void th20_timer_mode(Timer* t, std::uint32_t m) { timer_mode(*t, m); }
API void th20_timer_set(Timer* t, std::int32_t x) { timer_set(*t, x); }
API void th20_timer_add(Timer* t, float x, const float* r) { timer_add(*t, x, r); }
API std::int32_t th20_timer_tick(Timer* t, const float* r) { return timer_tick(*t, r); }
// Validation-only adapters: preserve raw IEEE754 payloads across ctypes and
// explicitly align the host SSE environment with the emulated original code.
API std::uint32_t th20_get_mxcsr() { return _mm_getcsr(); }
API void th20_set_mxcsr(std::uint32_t value) { _mm_setcsr(value); }
API void th20_timer_add_bits(Timer* t, std::uint32_t delta_bits, const float* r) {
    float delta;
    std::memcpy(&delta, &delta_bits, sizeof(delta));
    timer_add(*t, delta, r);
}
