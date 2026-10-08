#include "../../src/game/AnmLogic.hpp"
using namespace th12::anm_logic;
namespace {
Registry registry;
Runtime runtime;
VM machine;
float values[9];
int missing;
}
extern "C" {
int external_load(const uint8_t* data, int size) {
  std::string error;
  return registry.loadBank(0, data, size, error);
}
int external_begin(int script) {
  machine = {};
  runtime = {};
  missing = 0;
  runtime.unsupported = [](int, int, int, const char*) { ++missing; };
  return machine.bind(registry, 0, script, runtime);
}
void external_interrupt(int code) { machine.interrupt(code); }
void external_scale(float x, float y) {
  auto& state = machine.control();
  state.sx = x;
  state.sy = y;
  state.flags |= 8;
}
void external_tick() { machine.tick(runtime); }
const float* external_snapshot() {
  const auto& state = machine.state();
  values[0] = state.sx;
  values[1] = state.sy;
  values[2] = float(state.clock);
  values[3] = float(state.layer);
  values[4] = float(state.primary >> 24);
  values[5] = float(state.ended);
  values[6] = float(missing);
  values[7] = 0;
  for (const auto& interpolation : machine.diagnostics().interpolations)
    if (interpolation.active)
      ++values[7];
  values[8] = state.blend;
  return values;
}
}
