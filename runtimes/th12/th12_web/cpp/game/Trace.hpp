#pragma once
#include "Types.hpp"
namespace th11 {
// Monotonic progress marker for the headless fixtures. A wasm trap poisons the
// instance, so callers read the exported address first and then inspect memory
// directly once the trap has fired. Costs one volatile store per step.
extern volatile i32 trace_point;
}
