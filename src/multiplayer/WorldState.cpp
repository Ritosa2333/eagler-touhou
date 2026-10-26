#include "WorldState.hpp"
#include "../game/GameState.hpp"
#include "../game/StageFlow.hpp"
#include "../game/CameraEffects.hpp"
#include "../game/EnemyCallbacks.hpp"
#include "../game/HostilePool.hpp"
#include "../game/FriendlyPool.hpp"
#include "../game/LaserWorld.hpp"
#include "../game/ItemPresentation.hpp"
#include "../game/UfoEffects.hpp"
#include "../game/StageBackgroundEffect.hpp"
#include "../game/PresentationTracking.hpp"
#include <tuple>
namespace th12::multiplayer {
namespace {
auto stateReferences() {
  return std::tie(pilots, active, paused, finished, restartRequested,returnRequested,saveRequested,menuConfirm,menuInputReady,
    menuSelection,menuAction,menuPreviousHeld,menuAge,menuHoldAge,storyPreviousHeld,currentSeat, tickNumber, wipeFrames, lifeFragments,
    bombFragments, teamRank, damageRemainders, bulletGrazeMasks, random, displayRandom,
    nextDetachedAnimation, bullets, itemManager, ufoManager, messageVM, spellState,
    spellAnimationKeys, messageAnimationKeys, messageAnimationOwners, messageEvents,
    unsupported, animationUnsupported, referenceFaults, runtimeFaults, globals, phasePlayerCounters,
    frame, difficulty, stageNumber, backgroundFrame, gameSpeed, backgroundVisible, stageState, score,
    eventBits, dialogue, spell, ended, oracleFixtureFlags, replayStageGlobals, nextEnemyId,temporaryEnemyOwner,
    nextProjectileId, nextItemId, lastSpawnedEnemyId, backgroundEnemyOrigin, fullGameFlow,
    cameraEffects, bossHud, bossMusic, musicSerial, hostilePool, scorePopups, itemPresentation,
    stageBackgroundEffect, presentation::epoch, presentation::ownerRevisions, sound_system::queue,
    bulletWorld.playerX, bulletWorld.playerY);
}
template<class... T> auto copyValues(const std::tuple<T&...>& values) { return std::tuple<T...>(values); }
}
struct WorldState::Data {
  struct Pool;
  static std::shared_ptr<Pool> pool();
  decltype(copyValues(stateReferences())) values = copyValues(stateReferences());
  anm_logic::Scene::Snapshot scene = animationScene.snapshot();
  laser::Manager lasers = laserManager;
  UfoEffects effects = ufoEffects;
  std::vector<Enemy> enemies;
  Data() {
    enemies.reserve(th12::enemies.size());
    for (const auto& enemy : th12::enemies) enemies.push_back(*enemy);
  }
  void capture() {
    values = stateReferences();
    animationScene.snapshot(scene); lasers = laserManager; effects = ufoEffects;
    enemies.resize(th12::enemies.size());
    size_t index = 0;
    for (const auto& enemy : th12::enemies) enemies[index++] = *enemy;
  }
};
struct WorldState::Data::Pool {
  static constexpr size_t Capacity = 16;
  std::vector<std::unique_ptr<Data>> unused;
  Pool() { unused.reserve(Capacity); }
};
std::shared_ptr<WorldState::Data::Pool> WorldState::Data::pool() {
  // Outstanding checkpoint deleters retain the pool through process teardown.
  // The pool itself owns only released Data objects, so there is no cycle.
  static auto value = std::make_shared<Pool>();
  return value;
}
WorldState::WorldState() {
  const auto pool = Data::pool();
  std::unique_ptr<Data> saved;
  if (pool->unused.empty()) saved = std::make_unique<Data>();
  else { saved = std::move(pool->unused.back()); pool->unused.pop_back(); saved->capture(); }
  data_ = std::shared_ptr<const Data>(saved.release(), [pool](const Data* value) {
    // Only the last shared owner releases a checkpoint. Live/retained copies
    // never enter this cache and cannot be overwritten by a later frame.
    if (pool->unused.size() < Data::Pool::Capacity) pool->unused.emplace_back(const_cast<Data*>(value));
    else delete value;
  });
}
void WorldState::clearPool() { Data::pool()->unused.clear(); }
void WorldState::restore() const {
  stateReferences() = data_->values;
  animationScene.restore(data_->scene); laserManager = data_->lasers; ufoEffects = data_->effects;
  th12::enemies.clear();
  for (const auto& enemy : data_->enemies) th12::enemies.push_back(std::make_unique<Enemy>(enemy));
  refreshHostilePointers(); refreshFriendlyPointers();
}
}
