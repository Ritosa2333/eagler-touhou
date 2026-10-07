#include "GameSession.hpp"
#include "EnemyCallbacks.hpp"
#include "GameState.hpp"
#include "HostilePool.hpp"
#include "ItemPresentation.hpp"
#include "LaserWorld.hpp"
namespace th12 {
void selectStage(int number) {
  laserManager.reset(&laserWorld);
  resetItemPresentation();
  ended = 0;
  enemies.clear();
  bullets.clear();
  resetHostilePool();
  program = Program{};
  messageVM.reset();
  messageFiles = {};
  messageEvents.clear();
  scorePopups.clear();
  stageNumber = std::clamp(number, 1, 7);
  stageProgram = {};
  stageState = {};
  backgroundEnemyOrigin = {};
}
void startGame(int c, int s, int d, int seed) {
  sound_system::queue.reset();
  character = std::clamp(c, 0, 2);
  shot = std::clamp(s, 0, 1);
  difficulty = std::clamp(d, 0, 4);
  frame = backgroundFrame = 0;
  backgroundVisible = true;
  px = 0;
  py = 51200;
  resources = item_system::ResourceState::initial(difficulty);
  graze = 0;
  invuln = 120;
  deathWindow = bombTimer = 0;
  playerState = 1;
  playerStateTicks = 0;
  playerFlags = 0;
  playerFocusAnimation = 0;
  previousHeld = fireFrame = 0;
  dialogue = spell = ended = 0;
  oracleFixtureFlags = 0;
  messageVM.reset();
  messageEvents.clear();
  spellState.reset();
  scorePopups.clear();
  spellAnimationKeys = {};
  messageAnimationKeys = {};
  laserManager.reset(&laserWorld);
  animationScene.reset();
  animationUnsupported.clear();
  configureAnimations();
  configurePlayerAnimations();
  animationScene.runtime.unsupported = [](int bank, int script, int opcode, const char*) {
    ++unsupported[987];
    ++animationUnsupported[(uint32_t(bank) << 16) | uint16_t(opcode)];
  };
  displayRandom = {0x1234, 0};
  nextDetachedAnimation = 1;
  random = {u16(seed), 0};
  unsupported = {};
  referenceFaults.clear();
  runtimeFaults.clear();
  globals.clear();
  backgroundEnemyOrigin = {};
  enemies.clear();
  bullets.clear();
  itemManager.reset();
  resetHostilePool();
  ufoManager.reset();
  playerDamageSources.reset();
  playerBomb.reset();
  playerMotion = {};
  shotSchedule = {};
  playerMotion.updateOptions(loadouts[character * 2 + shot], power, px, py, false, true);
  bulletWorld.playerX = px / 128.f;
  bulletWorld.playerY = py / 128.f;
  nextEnemyId = nextProjectileId = 1;
  resetItemPresentation();
  initializeStageAnimations();
  initializePlayerAnimation();
  spawn("main", 0, 0, 10000, 0, 0);
  for (auto& e : enemies)
    e->flags &= ~0x20000u;
}
void setInitial(int x, int y, int p, int life, int bomb, int points) {
  px = x;
  py = y;
  power = p;
  lives = life;
  bombs = bomb;
  score = points;
  bulletWorld.playerX = px / 128.f;
  bulletWorld.playerY = py / 128.f;
  playerMotion.updateOptions(loadouts[character * 2 + shot], power, px, py, previousHeld & 8, true);
}
void setReplayEconomy(int piv, int lifeFragments, int bombFragments, int red, int blue, int green,
                      int rank) {
  resources.pointValueStored = piv;
  resources.lifeFragments = lifeFragments;
  resources.bombFragments = bombFragments;
  resources.rank = std::clamp(rank, -1024, 1024);
  ufoManager.inventory.clearSlots();
  for (int color : {red, blue, green})
    if (color)
      ufoManager.inventory.append(color);
  ufoManager.inventory.refresh();
  consumeUfoPresentation();
}
} // namespace th12
