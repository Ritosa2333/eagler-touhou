#include "../../src/game/GameState.hpp"
#include "../../src/game/HostilePool.hpp"
using namespace th12;
extern "C" {
void morph_bullet_tick(int teleport) {
  if (teleport)
    if (auto* b = hostileSlots[0]) {
      b->enemy.x = b->x = 20;
      b->enemy.y = b->y = -71;
    }
  tickBullets();
}
const int32_t* morph_bullet_state() {
  static int32_t result[15];
  const auto* b = hostileSlots[0];
  if (!b || !b->active) {
    result[0] = 0;
    result[13] = th12::random.seed;
    result[14] = th12::random.calls;
    return result;
  }
  const auto* node = animationScene.find(0x500000000ull + b->entityId);
  const auto& p = node->vm.state();
  result[0] = int(b->enemy.phase);
  result[1] = b->type;
  result[2] = b->color;
  result[3] = b->age;
  result[4] = p.clock;
  result[5] = p.script;
  result[6] = p.sprite;
  for (int i = 0; i < 6; ++i)
    result[7 + i] = p.integers[i];
  result[13] = th12::random.seed;
  result[14] = th12::random.calls;
  return result;
}
}
