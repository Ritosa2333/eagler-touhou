#include "../../src/bullet.hpp"
using namespace th12::eb;
struct SoundWorld : World {
  uint32_t calls = 0, last = 0, missing = 0;
  void emit(const Emission&) override {}
  void unsupported(uint32_t) override { ++missing; }
  void sound(int id, float) override { ++calls; last = id; }
};
extern "C" const uint32_t* approach_sound_case(int prefix, int sound, int duration) {
  static uint32_t result[5];
  State state;
  SoundWorld world;
  state.phase = Phase::Alive;
  state.speed = state.vx = 1;
  state.y = 120;
  state.program.transformSound = sound;
  if (prefix)
    set(state.program, 0, false, 0x80000000, 0, 0, 0, 0);
  set(state.program, prefix, false, 0x20000000, duration, 0, .06f, -999999);
  start(state, world);
  result[0] = state.active;
  result[1] = state.cursor;
  result[2] = world.calls;
  result[3] = world.last;
  result[4] = world.missing;
  return result;
}
