#include "game/Rng.hpp"

#ifndef TH13_WASI_HARNESS
#include <emscripten/emscripten.h>

extern "C" {

EMSCRIPTEN_KEEPALIVE
void th13RngSeed(th13::Rng* rng, uint32_t seed) { rng->reseed(seed); }

EMSCRIPTEN_KEEPALIVE
uint32_t th13RngNext(th13::Rng* rng) { return rng->next(); }

EMSCRIPTEN_KEEPALIVE
uint32_t th13RngState(const th13::Rng* rng) { return rng->state(); }
}
#endif
