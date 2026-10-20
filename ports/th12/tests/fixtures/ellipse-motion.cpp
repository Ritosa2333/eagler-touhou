#include "../../src/game/EclInterpreter.hpp"
#include "../../src/game/GameState.hpp"
using namespace th12;
static float output[30];
extern "C" {
void ellipse_setup() {
  enemies.clear();
  animationScene.reset();
  oracleFixtureFlags = 3;
  auto p = std::make_unique<Enemy>();
  p->id = 1;
  p->hidden = true;
  p->flags = 2 | 4 | 8 | 16;
  p->ax = 10;
  p->ay = 100;
  p->rx = 3;
  p->ry = -7;
  p->x = 13;
  p->y = 93;
  enemies.push_back(std::move(p));
}
void ellipse_command(int opcode, int duration, int mode, float a, float b, float c, float d,
                     float f, float g) {
  std::array<u8, 44> bytes{};
  const u16 size = bytes.size();
  std::memcpy(bytes.data() + 6, &size, 2);
  auto* payload = bytes.data() + 16;
  if (opcode == 301 || opcode == 303 || opcode == 305 || opcode == 307 || opcode == 309 ||
      opcode == 311 || opcode == 321 || opcode == 323) {
    std::memcpy(payload, &duration, 4);
    std::memcpy(payload + 4, &mode, 4);
    const std::array<float, 5> values{a, b, c, d, f};
    std::memcpy(payload + 8, values.data(), 20);
  } else if (opcode == 325 || opcode == 326) {
    std::memcpy(payload, &duration, 4);
    const std::array<float, 6> values{a, b, c, d, f, g};
    std::memcpy(payload + 4, values.data(), 24);
  } else {
    const std::array<float, 6> values{a, b, c, d, f, g};
    std::memcpy(payload, values.data(), 24);
  }
  Context context(bytes.data());
  command(*enemies.front(), context, opcode);
}
void ellipse_tick() {
  tickEnemies();
}
const float* ellipse_state() {
  const auto& e = *enemies.front();
  int cursor = 0;
  for (int relative = 0; relative < 2; ++relative) {
    const auto& m = relative ? e.relativeCircle : e.absoluteCircle;
    for (float value : {relative ? e.rx : e.ax, relative ? e.ry : e.ay, m.centerX, m.centerY,
                        relative ? e.relativeSpeed : e.speed, relative ? e.relativeAngle : e.angle,
                        m.radius, m.radialSpeed, m.ellipseAngle, m.ellipseRatio,
                        float(int(m.enabled) | (int(m.ellipse) << 1))})
      output[cursor++] = value;
  }
  output[22] = e.x;
  output[23] = e.y;
  output[24] = e.absoluteCircle.enabled ? e.absoluteCircle.ticks : e.polarTicks;
  output[25] = e.absoluteCircle.ticks;
  output[26] = e.absoluteCircle.ellipseTicks;
  output[27] = e.relativeCircle.enabled ? e.relativeCircle.ticks : e.relativePolarTicks;
  output[28] = e.relativeCircle.ticks;
  output[29] = e.relativeCircle.ellipseTicks;
  return output;
}
}
