#include "../../src/player_weapons.hpp"
th12::pw::Manager manager;
float output[6];
extern "C" {
void source_begin(float x, float y, float radius, float growth, int ticks) {
  manager.reset();
  manager.circle(x, y, radius, growth, ticks, 4);
}
void source_tick() { manager.tick(); }
int source_damage(float x, float y) { return manager.damage(x, y, 1, 1); }
const float* source_state() {
  const auto& s = manager.sources[0];
  output[0] = s.x; output[1] = s.y; output[2] = s.radius;
  output[3] = s.previousTicks; output[4] = s.ticks; output[5] = s.flags;
  return output;
}
}
