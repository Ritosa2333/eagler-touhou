// TH12 1.00b portable C++ runtime. See NOTICE.md for rights.
#include "GameState.hpp"

namespace th12 {
void tickSpell() {
  auto* spellBoss = primaryBoss();
  const auto spellTick = spellState.tick(
      playerBomb.active, py / 128.f, spellBoss ? spellBoss->x : 0, spellBoss ? spellBoss->y : 0);
  if (!spellTick.valid)
    ++unsupported[986];
  spell = spellState.active();
  if (spellState.active()) {
    animationScene.position(spellAnimationKeys[3], spellState.ringX + 224, spellState.ringY + 16);
    if (spellTick.bannerInterrupt)
      for (int i = 0; i < 3; i++)
        animationScene.interrupt(spellAnimationKeys[i], spellTick.bannerInterrupt);
  }
  if (spellTick.openedGameplay) {
    backgroundVisible = false;
    stageState.flags &= ~1u;
  }
}

} // namespace th12
