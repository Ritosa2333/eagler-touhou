#include "../../src/game/EclInterpreter.hpp"
#include "../../src/game/GameState.hpp"
using namespace th12;
static Enemy target;
extern "C" {
const uint32_t* enemy_hit_pose(uint32_t flags, int phaseLife, uint32_t spellFlags, int phaseAge,
                               int flash, int hitSound, int missing) {
  target = Enemy{};
  target.id = 1;
  target.x = 25;
  target.y = 100;
  target.flags = flags;
  target.phaseLife = phaseLife;
  target.phaseAge = phaseAge;
  target.hitFlashTicks = flash;
  target.hitSound = hitSound;
  spellState.flags = spellFlags;
  animationScene.reset();
  sound_system::queue.reset();
  auto* node = missing ? nullptr : animationScene.bind(enemyAnimationKey(1, 0), 1, 2, 7);
  if (node) {
    node->vm.control().flags = 0x10001u;
    node->vm.control().secondary = 0x12345678u;
  }
  synchronizeEnemyHitPresentation(target);
  static uint32_t result[5];
  result[0] = target.hitFlashTicks;
  result[1] = node ? node->vm.state().flags : 0;
  result[2] = node ? node->vm.state().secondary : 0;
  const auto sounds = sound_system::queue.events();
  result[3] = sounds.size();
  result[4] = sounds.empty() ? 0 : sounds[0].id;
  return result;
}
const int* enemy_hit_property(int value, int useCommands) {
  target = Enemy{};
  if (useCommands) {
    std::array<u8, 20> bytes{};
    const u16 size = bytes.size();
    std::memcpy(bytes.data() + 6, &size, 2);
    std::memcpy(bytes.data() + 16, &value, 4);
    Context context(bytes.data());
    command(target, context, 453);
    command(target, context, 449);
  }
  static int result[2];
  result[0] = target.hitSound;
  result[1] = bool(target.flags & 0x40000000u);
  return result;
}
}
