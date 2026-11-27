// 浏览器运行时导出：RNG 对照接口（oracle 直调）。
// WASI harness 目标下编译为空实现（TH16_WASI_HARNESS），保持双目标同源。
#include "game/Rng.hpp"

#ifndef TH16_WASI_HARNESS
#include <emscripten/emscripten.h>

extern "C" {

EMSCRIPTEN_KEEPALIVE void th16RngSeed(th16::Rng* rng, uint32_t seed) { rng->seed = uint16_t(seed); rng->calls = 0; }
EMSCRIPTEN_KEEPALIVE uint32_t th16RngNext32(th16::Rng* rng) { return rng->next32(); }
EMSCRIPTEN_KEEPALIVE uint32_t th16RngNext16(th16::Rng* rng) { return rng->next16(); }
EMSCRIPTEN_KEEPALIVE uint32_t th16RngState(const th16::Rng* rng) { return rng->seed; }

}  // extern "C"
#endif
