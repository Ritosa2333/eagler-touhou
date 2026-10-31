// TH12 1.00b portable C++ runtime. See NOTICE.md for rights.
#include "CameraEffects.hpp"
#include "AdaptedInput.hpp"
#include "EnemyCallbacks.hpp"
#include "FriendlyPool.hpp"
#include "GameState.hpp"
#include "HostilePool.hpp"
#include "ItemPresentation.hpp"
#include "LaserWorld.hpp"
#include "Practice.hpp"
#include "PresentationMotion.hpp"

namespace th12 {
void tick(int held, int pressed) {
  if (ended)
    return;
  tickGameplay(held, pressed);
}
void tickGameplay(int held, int pressed) {
  eventBits = 0;
  sound_system::queue.reset();
  scorePopups.clear();
  // Original update priorities: ANM8, Stage12, Camera14, Player16, Bomb17, Enemy18,
  // UFO19, Laser20, Bullet21, Item22, Spell23, HUD/MSG25, primary ANM27.
  animationScene.tickSecondary();
  tickStage();
  tickCameraEffects();
  practice::beforeTick(held, pressed);
  if (multiplayer::active) multiplayer::tickPlayers(); else tickPlayer(held, pressed);
  pilot().adaptedMotion.finish();
  if (multiplayer::active) multiplayer::tickBombs(); else tickBomb();
  tickEnemies();
  tickUfo();
  tickLasers();
  tickBullets();
  tickItems(held & 8);
  tickSpell();
  if (multiplayer::active) multiplayer::tickRules();
  bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](auto& b) { return !b.active; }),
                bullets.end());
  refreshHostilePointers();
  refreshFriendlyPointers();
  tickBossHud();
  tickMessage(held, pressed);
  syncItemPresentation();
  for (const auto& e : enemies)
    if (!e->active) {
      if (e->extraBombShieldAnimation)
        animationScene.remove(e->extraBombShieldAnimation);
      animationScene.removeOwner(e->id);
    }
  animationScene.collect();
  animationScene.tickPrimary();
  animationScene.collect();
  for (unsigned seat=0;seat<(multiplayer::active?multiplayer::options.count:1);++seat) {
    multiplayer::Scope owner(seat);
    if (animationScene.registry.bank(0)) buildPlayerBombTrails(); else tickPlayerBombEffects();
  }
  enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](auto& e) { return !e->active; }),
                enemies.end());
  ++frame;
  if (frame%120==0) collectPresentationRevisions();
  practice::afterTick();
}

} // namespace th12
