#include "../../src/game/GameState.hpp"
#include "../../src/game/HostilePool.hpp"
using namespace th12;
namespace {
struct ApproachWorld : eb::World {
  int unsupportedCount = 0;
  void emit(const eb::Emission&) override {}
  void unsupported(uint32_t) override { ++unsupportedCount; }
  std::array<float, 2> spriteDimensions(const eb::State&) const override { return {16, 16}; }
} approachWorld;
eb::State approachState;
eb::Program approachProgram;
float initialAngle, initialSpeed;
float motionFloats[6], worldFloats[6];
uint32_t motionWords[10], worldWords[19];
}
extern "C" {
void approach_reset(float angle, float speed, float playerX, float playerY) {
  approachWorld = {};
  approachWorld.playerX = playerX;
  approachWorld.playerY = playerY;
  approachState = {};
  approachProgram = {};
  initialAngle = angle;
  initialSpeed = speed;
}
void approach_set(int slot, int concurrent, uint32_t kind, int c, int d, float a, float b) {
  eb::set(approachProgram, slot, concurrent != 0, kind, c, d, a, b);
}
void approach_initialize() {
  eb::initialize(approachState, 1, approachProgram, 3, 0, 0, 120,
                 initialAngle, initialSpeed, approachWorld);
  approachState.offscreenGrace = 1000;
}
void approach_tick() { eb::tick(approachState, approachWorld); }
const float* approach_floats() {
  const auto& s = approachState;
  motionFloats[0] = s.x;
  motionFloats[1] = s.y;
  motionFloats[2] = s.vx;
  motionFloats[3] = s.vy;
  motionFloats[4] = s.speed;
  motionFloats[5] = s.angle;
  return motionFloats;
}
const uint32_t* approach_words() {
  const auto& s = approachState;
  const auto& m = s.speedApproach;
  motionWords[0] = s.active;
  motionWords[1] = s.cursor;
  motionWords[2] = m.timer;
  motionWords[3] = m.duration;
  motionWords[4] = asbits(m.a);
  motionWords[5] = asbits(m.b);
  motionWords[6] = asbits(m.vx);
  motionWords[7] = asbits(m.vy);
  motionWords[8] = 0; // Original scalar/vector helper also writes Z=0.
  motionWords[9] = approachWorld.unsupportedCount;
  return motionWords;
}
void approach_world_tick() { tickBullets(); }
const float* approach_world_floats() {
  const auto* b = hostileSlots[0];
  if (!b)
    return worldFloats;
  const auto& s = b->enemy;
  worldFloats[0] = s.x;
  worldFloats[1] = s.y;
  worldFloats[2] = s.vx;
  worldFloats[3] = s.vy;
  worldFloats[4] = s.speed;
  worldFloats[5] = s.angle;
  return worldFloats;
}
const uint32_t* approach_world_words() {
  const auto* b = hostileSlots[0];
  if (!b || !b->active) {
    worldWords[0] = 0;
    worldWords[16] = th12::random.seed;
    worldWords[17] = th12::random.calls;
    return worldWords;
  }
  const auto* node = animationScene.find(0x500000000ull + b->entityId);
  const auto& p = node->vm.state();
  worldWords[0] = int(b->enemy.phase);
  worldWords[1] = b->type;
  worldWords[2] = b->color;
  worldWords[3] = b->age;
  worldWords[4] = b->enemy.active;
  worldWords[5] = b->enemy.cursor;
  worldWords[6] = b->enemy.speedApproach.timer;
  worldWords[7] = p.clock;
  worldWords[8] = p.script;
  worldWords[9] = p.sprite;
  for (int i = 0; i < 6; ++i)
    worldWords[10 + i] = p.integers[i];
  worldWords[16] = th12::random.seed;
  worldWords[17] = th12::random.calls;
  worldWords[18] = unsupported[0];
  return worldWords;
}
}
