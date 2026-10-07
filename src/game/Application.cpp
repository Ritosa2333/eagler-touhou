// TH12 1.00b portable C++ runtime. See NOTICE.md for rights.
#include "EnemyCallbacks.hpp"
#include "GameState.hpp"
#include "HostilePool.hpp"
#include "ItemPresentation.hpp"
#include "LaserWorld.hpp"

namespace th12 {
void tick(int held, int pressed) {
  if (ended)
    return;
  eventBits = 0;
  sound_system::queue.reset();
  scorePopups.clear();
  // Original update priorities: ANM8, Stage12, Player16, Bomb17, Enemy18,
  // UFO19, Laser20, Bullet21, Item22, Spell23, HUD/MSG25, primary ANM27.
  animationScene.tickSecondary();
  tickStage();
  tickPlayer(held, pressed);
  tickBomb();
  clearBombLasers();
  tickEnemies();
  tickUfo();
  tickLasers();
  tickBullets();
  tickItems(held & 8);
  tickSpell();
  bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](auto& b) { return !b.active; }),
                bullets.end());
  refreshHostilePointers();
  tickBossHud();
  tickMessage(held, pressed);
  syncItemPresentation();
  for (const auto& e : enemies)
    if (!e->active)
      animationScene.removeOwner(e->id);
  animationScene.collect();
  animationScene.tickPrimary();
  animationScene.collect();
  if (animationScene.registry.bank(0))
    buildPlayerBombTrails();
  else
    tickPlayerBombEffects();
  enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](auto& e) { return !e->active; }),
                enemies.end());
  ++frame;
}

} // namespace th12
