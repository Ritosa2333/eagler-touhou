#include "../../src/game/Laser.hpp"
#include <cmath>
using namespace th12::laser;
namespace {
struct Probe : Laser {
  void initialize(World&) override {}
  bool update(Manager& manager, World& world) override { return extensions(manager, world); }
} probe;
struct ProbeWorld : World {
  float rate = 1;
  int missing = 0;
  float gameRate() const override { return rate; }
  void unsupported(uint32_t code) override {
    // Native Moving/Timed400 is ignored; their documented unsupported marker
    // is outside the scoped Curve restoration and does not alter gameplay.
    if (code != 0x400 || probe.state.kind == Kind::Curve) ++missing;
  }
} world;
Manager manager;
float output[1024];
void gate(const State& s, float* out) {
  out[0] = s.offscreenGate.previous; out[1] = s.offscreenGate.current;
  out[2] = s.offscreenGate.value; out[3] = s.offscreenGateFlags;
}
void motion(const State& s, float* out) {
  out[0] = s.activeExtensions; out[1] = s.cursor; gate(s, out + 2);
  out[6] = s.polarAcceleration.timer.previous; out[7] = s.polarAcceleration.timer.current;
  out[8] = s.polarAcceleration.timer.value; out[9] = s.angle; out[10] = s.speed;
  out[11] = s.velocity.x; out[12] = s.velocity.y; out[13] = s.velocity.z;
}
}
extern "C" {
void offscreen_callback_begin(int kind, int duration, float rate, int flags, int concurrent) {
  manager.reset(); world.rate = rate; world.missing = 0; probe.state = {};
  auto& s = probe.state; s.kind = Kind(kind); s.phase = Phase::Active;
  s.angle = .33f; s.speed = 6;
  s.velocity = {float(std::cos(double(s.angle)) * 6), float(std::sin(double(s.angle)) * 6), 0};
  s.parameters.program.records[0] = {-999999, -999999, duration, flags, 0x400, false};
  if (kind == 2) {
    s.parameters.program.records[1] = {-.083333336f, .05235988f, 60, -999999, 8, concurrent != 0};
    s.parameters.program.records[2] = {.083333336f, 0, 60, -999999, 8, true};
  }
}
void offscreen_callback_tick() { probe.update(manager, world); }
const float* offscreen_callback_snapshot() { motion(probe.state, output); return output; }
int offscreen_missing() { return world.missing; }
void offscreen_lifecycle_begin(int duration) {
  manager.reset(); world.rate = 1; world.missing = 0;
  // This fixture excludes ANM state/RNG: the native lifecycle runs real ANM,
  // while this World supplies only its irrelevant drawing/audio boundaries.
  Parameters p; p.position = {190, 200, 0}; p.angle = 0; p.speed = 6;
  p.width = 16; p.nodeCount = 42; p.type = 0; p.color = 13;
  int index = 0;
  if (duration >= 0) p.program.records[index++] = {-999999, -999999, duration, 0, 0x400, false};
  p.program.records[index++] = {-.083333336f, .05235988f, 60, -999999, 8, true};
  p.program.records[index] = {.083333336f, 0, 60, -999999, 8, true};
  manager.spawn(Kind::Curve, p, world);
}
void offscreen_lifecycle_tick() { manager.tick(world); }
const float* offscreen_lifecycle_snapshot() {
  const auto objects = manager.ordered(); output[0] = objects.size();
  if (objects.empty()) return output;
  const auto& s = objects[0]->state;
  motion(s, output + 1); output[15] = int(s.phase);
  output[16] = s.age.previous; output[17] = s.age.current; output[18] = s.age.value;
  output[19] = s.position.x; output[20] = s.position.y; output[21] = s.position.z;
  output[22] = s.offscreenGrace.previous; output[23] = s.offscreenGrace.current;
  output[24] = s.offscreenGrace.value; output[25] = s.nodes.size();
  auto* out = output + 26;
  for (const auto& node : s.nodes) {
    *out++ = node.position.x; *out++ = node.position.y; *out++ = node.position.z;
    *out++ = node.angle; *out++ = node.speed;
  }
  return output;
}
}
