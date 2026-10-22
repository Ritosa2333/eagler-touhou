#include "GameSession.hpp"
#include "AdaptedInput.hpp"
#include "CameraEffects.hpp"
#include "EnemyCallbacks.hpp"
#include "FriendlyPool.hpp"
#include "GameState.hpp"
#include "HostilePool.hpp"
#include "ItemPresentation.hpp"
#include "LaserWorld.hpp"
#include "StageFlow.hpp"
#include "Practice.hpp"
#include "PracticeDamageSources.hpp"
namespace th12 {
void selectStage(int number) {
  practice::stageSelected();
  gameSpeed = animationScene.runtime.delta = 1;
  resetCameraEffects();
  laserManager.reset(&laserWorld);
  resetItemPresentation();
  ended = 0;
  bossHud = {};
  bossMusic = false;
  musicSerial = 0;
  enemies.clear();
  bullets.clear();
  resetHostilePool();
  resetFriendlyPool();
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
void startGame(int c, int s, int d, int seed, const int* replayInitial, const SessionCarry* carry) {
  if (!carry) practice::resetTracker();
  adaptedMotion = {};
  gameSpeed = animationScene.runtime.delta = 1;
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
  playerStateTime = playerTotalTime = 0;
  playerPreviousStateTicks = -1;
  playerInvulnerabilityTime = 120;
  playerFlags = 0;
  replayStageGlobals = {};
  playerFocused = false;
  playerFocusAnimation = 0;
  previousHeld = fireFrame = 0;
  dialogue = spell = ended = 0;
  bossHud = {};
  bossMusic = practice::bossMusic();
  musicSerial = 0;
  oracleFixtureFlags = 0;
  messageVM.reset();
  messageEvents.clear();
  spellState.reset();
  spellState.replayWallClock = replayInitial != nullptr;
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
  phasePlayerCounters = {};
  backgroundEnemyOrigin = {};
  enemies.clear();
  bullets.clear();
  itemManager.reset();
  resetHostilePool();
  resetFriendlyPool();
  ufoManager.reset();
  playerDamageSources.reset();
  playerBomb.reset();
  resetCameraEffects();
  playerMotion = {};
  shotSchedule = {};
  if (replayInitial) {
    // 43ae80/43c730 restore resources and RNG before Stage10 creates main;
    // 43c590 restores fixed position/focus before Replay11 reads input0.
    const auto* r = replayInitial;
    px = r[0];
    py = r[1];
    power = std::clamp(r[2], resources.powerStep, resources.maxPower);
    lives = r[3];
    bombs = r[4];
    score = r[5];
    resources.pointValueStored = r[6];
    resources.lifeFragments = r[7];
    resources.bombFragments = r[8];
    resources.rank = r[12];
    for (int color : {r[9], r[10], r[11]})
      if (color)
        ufoManager.inventory.append(color);
    replayStageGlobals = {r[13], r[14], r[15], r[16]};
    graze = r[16]; // 43b311 restores CDC during independent stage selection.
  }
  if (carry) {
    resources = carry->resources;
    px = carry->x;
    py = carry->y;
    graze = carry->graze;
    replayStageGlobals = carry->nativeGlobals;
    random = {carry->seed, 0}; // 43c77b..43c78d: retain seed, new per-stage call counter.
    displayRandom = carry->displayRandom;
    ufoManager.inventory = carry->inventory;
    // The new presentation must bind the retained inventory; old queued VMs
    // belong to the previous scene and must not be replayed during its birth.
    ufoManager.inventory.hudEvents.clear();
    ufoManager.inventory.refresh();
  }
  playerMotion.updateOptions(loadouts[character * 2 + shot], power, px, py, false, true);
  // 43c590 restores C598 after the initial option layout. Later body resets
  // must retain the live focus field rather than replaying this stage header.
  playerFocused = carry ? carry->focused : replayInitial && replayStageGlobals[2] != 0;
  // Both4385b0's initial construction and replay43add0 mark these reset flags.
  playerMotion.resetOptionPositions();
  bulletWorld.playerX = px / 128.f;
  bulletWorld.playerY = py / 128.f;
  nextEnemyId = nextProjectileId = 1;
  lastSpawnedEnemyId = 0;
  resetItemPresentation();
  consumeUfoPresentation();
  // 41d8ea..41d91e: the first stage's HUD creates front51, including
  // children49/50. Replay (GameScene+74) and Continue (4b0cc4) omit it.
  if (stageNumber == 1 && !replayInitial && !practice::replay() && replayStageGlobals[0] == 0)
    bindScreenAnimation(7, 51);
  initializeStageAnimations();
  initializePlayerAnimation();
  // Restore selected token-side / real transition damage ownership before
  // synchronous main ECL can consume either state.
  practice::applyBirth();
  practice::damage::onBirth(stageNumber);
  spawn("main", 0, 0, 10000, 0, 0);
  // Native birth sets20000. Enemy18 of the input0 tick only clears it;
  // the first regular main update belongs to input1, without shifting Replay.
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
  playerMotion.updateOptions(loadouts[character * 2 + shot], power, px, py, playerFocused, true);
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
