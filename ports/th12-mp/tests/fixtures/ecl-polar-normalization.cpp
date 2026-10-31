#include "../../src/game/EclInterpreter.hpp"
#include "../../src/game/GameState.hpp"
using namespace th12;
static float output[2];
extern "C" const float* ecl_polar(float angle, float length) {
  std::array<u8, 52> bytes{};
  const u16 op = 81, size = 32, waitOp = 83, waitSize = 20;
  std::memcpy(bytes.data() + 4, &op, 2);
  std::memcpy(bytes.data() + 6, &size, 2);
  bytes[8] = 3; // Destinations are local references; angle/length are literals.
  bytes[10] = 63;
  bytes[11] = 4;
  const std::array<float, 4> operands{0, 4, angle, length};
  std::memcpy(bytes.data() + 16, operands.data(), 16);
  std::memcpy(bytes.data() + 36, &waitOp, 2);
  std::memcpy(bytes.data() + 38, &waitSize, 2);
  bytes[42] = 63;
  bytes[43] = 1;
  const float wait = 100000;
  std::memcpy(bytes.data() + 48, &wait, 4);
  Enemy enemy;
  enemy.active = true;
  enemy.contexts.emplace_back(bytes.data());
  auto& context = enemy.contexts.front();
  runContext(enemy, context);
  output[0] = asfloat(context.locals[0]);
  output[1] = asfloat(context.locals[1]);
  return output;
}
