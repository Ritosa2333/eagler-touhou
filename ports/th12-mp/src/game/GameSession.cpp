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
#include "../multiplayer/StageEntry.hpp"
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
  struct RetainedPilot { item_system::ResourceState resources; int graze; unsigned misses; bool ghost; int x,y; bool focused; };
  std::optional<RetainedPilot> retainedPilot;
  if(multiplayer::active&&carry)retainedPilot=RetainedPilot{pilot().resources,pilot().graze,pilot().misses,pilot().ghost,pilot().px,pilot().py,pilot().playerFocused};
  if (!carry) practice::resetTracker();
  pilot().adaptedMotion = {};
  gameSpeed = animationScene.runtime.delta = 1;
  sound_system::queue.reset();
  pilot().character = std::clamp(c, 0, 2);
  pilot().shot = std::clamp(s, 0, 1);
  difficulty = std::clamp(d, 0, 4);
  frame = backgroundFrame = 0;
  backgroundVisible = true;
  pilot().px = 0;
  pilot().py = 51200;
  pilot().resources = item_system::ResourceState::initial(difficulty);
  pilot().graze = 0;
  pilot().invuln = 120;
  pilot().deathWindow = pilot().bombTimer = 0;
  pilot().playerState = 1;
  pilot().playerStateTicks = 0;
  pilot().playerStateTime = pilot().playerTotalTime = 0;
  pilot().playerPreviousStateTicks = -1;
  pilot().playerInvulnerabilityTime = 120;
  pilot().playerFlags = 0;
  replayStageGlobals = {};
  pilot().playerFocused = false;
  pilot().playerFocusAnimation = 0;
  pilot().previousHeld = pilot().fireFrame = 0;
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
  messageAnimationOwners.clear();
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
  pilot().playerDamageSources.reset();
  pilot().playerBomb.reset();
  resetCameraEffects();
  pilot().playerMotion = {};
  pilot().shotSchedule = {};
  if (replayInitial) {
    // 43ae80/43c730 restore resources and RNG before Stage10 creates main;
    // 43c590 restores fixed position/focus before Replay11 reads input0.
    const auto* r = replayInitial;
    pilot().px = r[0];
    pilot().py = r[1];
    pilot().resources.power = std::clamp(r[2], pilot().resources.powerStep, pilot().resources.maxPower);
    pilot().resources.lives = r[3];
    pilot().resources.bombs = r[4];
    score = r[5];
    pilot().resources.pointValueStored = r[6];
    pilot().resources.lifeFragments = r[7];
    pilot().resources.bombFragments = r[8];
    pilot().resources.rank = r[12];
    for (int color : {r[9], r[10], r[11]})
      if (color)
        ufoManager.inventory.append(color);
    replayStageGlobals = {r[13], r[14], r[15], r[16]};
    pilot().graze = r[16]; // 43b311 restores CDC during independent stage selection.
  }
  if (carry) {
    pilot().resources = carry->resources;
    pilot().px = carry->x;
    pilot().py = carry->y;
    pilot().graze = carry->graze;
    replayStageGlobals = carry->nativeGlobals;
    random = {carry->seed, 0}; // 43c77b..43c78d: retain seed, new per-stage call counter.
    displayRandom = carry->displayRandom;
    ufoManager.inventory = carry->inventory;
    // The new presentation must bind the retained inventory; old queued VMs
    // belong to the previous scene and must not be replayed during its birth.
    ufoManager.inventory.hudEvents.clear();
    ufoManager.inventory.refresh();
  }
  pilot().playerMotion.updateOptions(loadouts[pilot().character * 2 + pilot().shot], pilot().resources.power, pilot().px, pilot().py, false, true);
  // 43c590 restores C598 after the initial option layout. Later body resets
  // must retain the live focus field rather than replaying this stage header.
  pilot().playerFocused = carry ? carry->focused : replayInitial && replayStageGlobals[2] != 0;
  // Both4385b0's initial construction and replay43add0 mark these reset flags.
  pilot().playerMotion.resetOptionPositions();
  bulletWorld.playerX = pilot().px / 128.f;
  bulletWorld.playerY = pilot().py / 128.f;
  nextEnemyId = nextProjectileId = 1;
  temporaryEnemyOwner = 0x7fffffffu;
  lastSpawnedEnemyId = 0;
  if (multiplayer::pendingStageEntry) {
    const auto& entry = *multiplayer::pendingStageEntry;
    displayRandom = {uint16_t(entry.displaySeed), entry.displayCalls};
    ufoManager.inventory = entry.inventory;
    replayStageGlobals = entry.nativeGlobals;
  }
  resetItemPresentation();
  if (multiplayer::active) multiplayer::recordStageEntry(seed,carry);
  consumeUfoPresentation();
  // 41d8ea..41d91e: the first stage's HUD creates front51, including
  // children49/50. Replay (GameScene+74) and Continue (4b0cc4) omit it.
  if (stageNumber == 1 && !replayInitial && !practice::replay() && replayStageGlobals[0] == 0)
    bindScreenAnimation(7, 51);
  initializeStageAnimations();
  if (!multiplayer::active) initializePlayerAnimation();
  // Restore selected token-side / real transition damage ownership before
  // synchronous main ECL can consume either state.
  practice::applyBirth();
  practice::damage::onBirth(stageNumber);
  if (multiplayer::active) {
    if(retainedPilot) {
      auto& first=multiplayer::pilots[0];
      first.resources=retainedPilot->resources; first.graze=retainedPilot->graze;
      first.misses=retainedPilot->misses; first.ghost=retainedPilot->ghost;
      first.px=retainedPilot->x;first.py=retainedPilot->y;first.playerFocused=retainedPilot->focused;
    }
    multiplayer::initializePilots(carry != nullptr);
    // The entry captures inventory before its presentation events are consumed.
    auto& entry = multiplayer::stageEntry;
    for (unsigned seat=0; seat<multiplayer::options.count; ++seat) {
      entry.resources[seat]=multiplayer::pilots[seat].resources;
      entry.graze[seat]=multiplayer::pilots[seat].graze;
      entry.misses[seat]=multiplayer::pilots[seat].misses;
      entry.x[seat]=multiplayer::pilots[seat].px;entry.y[seat]=multiplayer::pilots[seat].py;
      entry.focused[seat]=multiplayer::pilots[seat].playerFocused;
    }
  }
  spawn("main", 0, 0, 10000, 0, 0);
  // Native birth sets20000. Enemy18 of the input0 tick only clears it;
  // the first regular main update belongs to input1, without shifting Replay.
}
void setInitial(int x, int y, int p, int life, int bomb, int points) {
  pilot().px = x;
  pilot().py = y;
  pilot().resources.power = p;
  pilot().resources.lives = life;
  pilot().resources.bombs = bomb;
  score = points;
  bulletWorld.playerX = pilot().px / 128.f;
  bulletWorld.playerY = pilot().py / 128.f;
  pilot().playerMotion.updateOptions(loadouts[pilot().character * 2 + pilot().shot], pilot().resources.power, pilot().px, pilot().py, pilot().playerFocused, true);
}
void setReplayEconomy(int piv, int lifeFragments, int bombFragments, int red, int blue, int green,
                      int rank) {
  pilot().resources.pointValueStored = piv;
  pilot().resources.lifeFragments = lifeFragments;
  pilot().resources.bombFragments = bombFragments;
  pilot().resources.rank = std::clamp(rank, -1024, 1024);
  ufoManager.inventory.clearSlots();
  for (int color : {red, blue, green})
    if (color)
      ufoManager.inventory.append(color);
  ufoManager.inventory.refresh();
  consumeUfoPresentation();
}
} // namespace th12
