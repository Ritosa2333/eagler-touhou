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
#include <iterator>
#include <tuple>
namespace th12::multiplayer {
namespace {
auto stateReferences() {
  return std::tie(pilots, active, paused, finished, restartRequested,returnRequested,saveRequested,menuConfirm,
    menuSelection,menuAction,menuReadySeats,menuAge,menuHoldAge,menuInput,storyInput,currentSeat, tickNumber, wipeFrames, lifeFragments,
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
void copyEnemy(Enemy& destination, const Enemy& source, std::list<Context>& spare) {
  // Preserve list nodes and each script stack/call vector through short-lived
  // context count changes. Assignment still restores every semantic field.
  while (destination.contexts.size() > source.contexts.size()) {
    auto last = std::prev(destination.contexts.end());
    spare.splice(spare.end(), destination.contexts, last);
  }
  while (destination.contexts.size() < source.contexts.size() && !spare.empty())
    destination.contexts.splice(destination.contexts.end(), spare, spare.begin());
  destination = source;
  while (spare.size() > 128) spare.pop_back();
}
}
struct WorldState::Data {
  struct Pool;
  static std::shared_ptr<Pool> pool();
  decltype(copyValues(stateReferences())) values = copyValues(stateReferences());
  anm_logic::Scene::Snapshot scene = animationScene.snapshot();
  laser::Manager lasers = laserManager;
  UfoEffects effects = ufoEffects;
  std::vector<Enemy> enemies;
  size_t enemyCount = 0;
  explicit Data(Pool& storage);
  void capture(Pool& storage);
  void captureEnemies(Pool& storage);
};
struct WorldState::Data::Pool {
  static constexpr size_t Capacity = 16;
  std::vector<std::unique_ptr<Data>> unused;
  std::vector<std::unique_ptr<Enemy>> retiredEnemies;
  std::list<Context> spareContexts;
  Pool() { unused.reserve(Capacity); retiredEnemies.reserve(256); }
};
WorldState::Data::Data(Pool& storage) { captureEnemies(storage); }
void WorldState::Data::captureEnemies(Pool& storage) {
  enemyCount = th12::enemies.size();
  if (enemies.size() < enemyCount) enemies.resize(enemyCount);
  // Keep a bounded spare tail across small count fluctuations, without
  // retaining all script stacks from a much larger previous wave.
  else if (enemies.size() > enemyCount + 64) enemies.resize(enemyCount + 64);
  size_t index = 0;
  for (const auto& enemy : th12::enemies) copyEnemy(enemies[index++], *enemy, storage.spareContexts);
}
void WorldState::Data::capture(Pool& storage) {
  values = stateReferences();
  animationScene.snapshot(scene); lasers = laserManager; effects = ufoEffects;
  captureEnemies(storage);
}
std::shared_ptr<WorldState::Data::Pool> WorldState::Data::pool() {
  // Outstanding checkpoint deleters retain the pool through process teardown.
  // The pool itself owns only released Data objects, so there is no cycle.
  static auto value = std::make_shared<Pool>();
  return value;
}
WorldState::WorldState() {
  const auto pool = Data::pool();
  std::unique_ptr<Data> saved;
  if (pool->unused.empty()) saved = std::make_unique<Data>(*pool);
  else { saved = std::move(pool->unused.back()); pool->unused.pop_back(); saved->capture(*pool); }
  data_ = std::shared_ptr<const Data>(saved.release(), [pool](const Data* value) {
    // Only the last shared owner releases a checkpoint. Live/retained copies
    // never enter this cache and cannot be overwritten by a later frame.
    if (pool->unused.size() < Data::Pool::Capacity) pool->unused.emplace_back(const_cast<Data*>(value));
    else delete value;
  });
}
void WorldState::clearPool() {
  auto storage = Data::pool();
  storage->unused.clear(); storage->retiredEnemies.clear(); storage->spareContexts.clear();
}
void WorldState::restore() const {
  stateReferences() = data_->values;
  animationScene.restore(data_->scene); laserManager = data_->lasers; ufoEffects = data_->effects;
  auto storage = Data::pool();
  while (th12::enemies.size() > data_->enemyCount) {
    if (storage->retiredEnemies.size() < 256)
      storage->retiredEnemies.push_back(std::move(th12::enemies.back()));
    th12::enemies.pop_back();
  }
  while (th12::enemies.size() < data_->enemyCount) {
    std::unique_ptr<Enemy> enemy;
    if (storage->retiredEnemies.empty()) enemy = std::make_unique<Enemy>();
    else { enemy = std::move(storage->retiredEnemies.back()); storage->retiredEnemies.pop_back(); }
    th12::enemies.push_back(std::move(enemy));
  }
  for (size_t i = 0; i < data_->enemyCount; ++i)
    copyEnemy(*th12::enemies[i], data_->enemies[i], storage->spareContexts);
  refreshHostilePointers(); refreshFriendlyPointers();
}
}
