// Integration probe for the production EnemyManager movement path. The native
// fixture executes the original EXE; this harness calls the restored manager.
#include "../../src/game/GameState.hpp"
#include "../../src/game/EclInterpreter.hpp"
using namespace th12;
static float values[30];
static Enemy& enemy() { return *enemies.front(); }
extern "C" {
void setup(int relative, float angle, float radius, float delta, float speed,
           float targetSpeed, float targetRadius, float targetDelta,
           int duration, int mode, int clamp) {
  enemies.clear();
  animationScene.reset();
  oracleFixtureFlags = 3; // This fixture compares only movement, without hurt.
  auto pointer = std::make_unique<Enemy>();
  auto& e = *pointer;
  e.id = 1;
  e.hidden = true;
  e.flags = 2 | 16 | (clamp ? 0x10000u : 0);
  e.ay = e.y = 128;
  e.clampY = 128;
  e.clampWidth = 280;
  e.clampHeight = 128;
  auto& circle = relative ? e.relativeCircle : e.absoluteCircle;
  circle.enabled = true;
  circle.centerY = relative ? 0 : 128;
  circle.radius = radius;
  circle.radialSpeed = delta;
  (relative ? e.relativeAngle : e.angle) = normalize(angle);
  (relative ? e.relativeSpeed : e.speed) = speed;
  circularPosition(e, relative);
  combinePosition(e);
  circle.ticks = duration;
  circle.mode = mode;
  circle.fromOmega = speed;
  circle.fromRadius = radius;
  circle.fromRadial = delta;
  circle.toOmega = targetSpeed;
  circle.toRadius = targetRadius;
  circle.toRadial = targetDelta;
  enemies.push_back(std::move(pointer));
}
void restore_position(float x, float y, float ax, float ay, float rx, float ry,
                      float angle, float relativeAngle, float radius) {
  auto& e = enemy();
  e.x = x; e.y = y; e.ax = ax; e.ay = ay; e.rx = rx; e.ry = ry;
  e.angle = angle; e.relativeAngle = relativeAngle;
  e.relativeCircle.radius = radius;
}
void tick() { tickEnemies(); }
void combine() { combinePosition(enemy()); }
void bounds(float x, float y, float width, float height) {
  auto& e = enemy();
  e.clampX = x; e.clampY = y; e.clampWidth = width; e.clampHeight = height;
}
void set_position(int opcode, float x, float y) {
  std::array<u8, 24> data{};
  const u16 op = u16(opcode), size = data.size();
  std::memcpy(data.data() + 4, &op, 2);
  std::memcpy(data.data() + 6, &size, 2);
  data[10] = 63; data[11] = 2;
  std::memcpy(data.data() + 16, &x, 4);
  std::memcpy(data.data() + 20, &y, 4);
  Context context(data.data());
  command(enemy(), context, opcode);
}
float quantized(float value) { return enemy_motion::quantize(value); }
const float* snapshot() {
  const auto& e = enemy();
  int index = 0;
  for (int relative = 0; relative < 2; ++relative) {
    const auto& circle = relative ? e.relativeCircle : e.absoluteCircle;
    for (float v : {relative ? e.rx : e.ax, relative ? e.ry : e.ay,
                    circle.centerX, circle.centerY,
                    relative ? e.relativeSpeed : e.speed,
                    relative ? e.relativeAngle : e.angle,
                    circle.radius, circle.radialSpeed, float(circle.enabled)})
      values[index++] = v;
  }
  values[18] = e.x;
  values[19] = e.y;
  values[27] = e.absoluteCircle.ticks;
  values[28] = e.relativeCircle.ticks;
  values[29] = e.active;
  return values;
}
}
