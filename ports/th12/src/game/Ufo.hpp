// Hand-restored TH12 1.00b UFO rules; not generated decompiler C.
#pragma once
#include "Items.hpp"
#include <functional>
#include <string>

namespace th12::ufo_system {
using item_system::ResourceState;
using item_system::RewardDelta;
using item_system::SpawnRequest;
using item_system::Vec2;
enum class Kind : int { Rainbow = -1, None = 0, Red = 1, Blue = 2, Green = 3 };

enum class HudTarget { Stock, PickupOverlay };
enum class HudOperation { Interrupt, SelectSprite };
struct InventoryHudEvent {
  uint64_t sequence = 0;
  HudTarget target = HudTarget::Stock;
  HudOperation operation = HudOperation::Interrupt;
  int logicalSlot = 0, physicalSlot = 1, script = 80, layer = 20, value = 0;
  uint32_t overlayId = 0; // The two front81 interrupts share one allocation.
  bool immediate = true;  // Original stock/overlay calls execute455630 synchronously.
};
struct Inventory {
  std::array<int, 3> colors{}; // Native encoding: 0empty,1red,2blue,3green.
  int count = 0, displayState = 0, gateTimer = 0, combination = 0;
  int rotation = 0; // HUD+6788: physical stock VM rotation, survives clearSlots.
  uint64_t nextHudSequence = 1;
  uint32_t nextOverlayId = 1;
  std::vector<InventoryHudEvent> hudEvents;
  int append(int color); // Logical append; mismatches also queue original9/7/8 rotation.
  void clearSlots();     // Does not reset combination, gateTimer or physical rotation.
  bool ready() const;
  int physicalSlot(int logicalSlot) const;
  void refresh(int newLogicalSlot = -1); // Original4214b0, front80 and optional front81.
  void blink(bool active);               // Original421690 selects color+63/color+59 sprites.
  std::vector<InventoryHudEvent> drainHudEvents();
};

struct EnemySpawn {
  std::string routine;
  Vec2 position{0, 128};
  int health = 1600;
};
struct EnemyView {
  bool active = false;
  uint32_t id = 0;
  Vec2 position{};
  int health = 0, maxHealth = 0;
};
struct EnemyHooks {
  std::function<uint32_t(const EnemySpawn&)> spawn;
  std::function<EnemyView(uint32_t)> query;
  // Legacy fixtures may keep this field; tick returns actions and never invokes it.
  std::function<void(uint32_t, int, int)> setPrivateInteger;
};
struct WorldView {
  int power = 100, stage = 1;
  bool bossPresent = false, dialogue = false;
  float rate = 1;
  int powerStep = 100;
};
enum class DeathReason { Defeated, Dialogue };
enum class RemovalReason { None, NaturalExit, ScriptDelete, Defeated, Dialogue, StageReset };
enum class EnemyActionKind { SetPrivateInteger, RequestDeath };
struct EnemyAction {
  EnemyActionKind kind = EnemyActionKind::SetPrivateInteger;
  uint32_t enemyId = 0;
  int privateIndex = 0,
      value = 0; // index0 = ECL-9985; RequestDeath value2 = native lifeFlags bit2.
  DeathReason reason = DeathReason::Defeated;
};
struct TickResult {
  std::vector<EnemyAction> actions;
  bool spawned = false, removed = false;
};
enum class VisualEventKind {
  Summoned,
  PoseUpdated,
  FillCompleted,
  DeathBurst,
  RewardDisplay,
  Removed,
  DirectReset
};
struct VisualEvent {
  uint64_t sequence = 0;
  VisualEventKind kind = VisualEventKind::PoseUpdated;
  uint32_t enemyId = 0;
  Vec2 position{};
  int ufoKind = 0, age = 0, remainingTicks = 0, health = 0, maxHealth = 0;
  float fill = 0, multiplier = 0;
  int scoreDisplayed = 0, interrupt = 0;
  // Original scene mapping, resource bank names are resolved by the host.
  int bulletScript = 0, noticeScript = 0, panelScript = 0, healthScript = 0, sound = 0;
};
struct TokenPickup {
  int color = 0;
  Vec2 position{};
}; // Color0-based matches original4270b0.
struct RewardPlan {
  // Current resource deltas versus deferred collectible items; no score at UFO death.
  // scoreStored = displayed score/10; pointValueStored = displayed PIV*100.
  RewardDelta immediate{};
  std::vector<SpawnRequest> spawns;
  std::vector<EnemySpawn> effects; // Death emits the real UFO_EtBreak2 ECL effect, HP3000.
  bool inventoryChanged = false, filledNow = false;
  DeathReason deathReason = DeathReason::Defeated;
  int tokenPickupScript = 0;
  Vec2 tokenPickupPosition{}; // bullet201..203, optional pickup trail.
};
enum class Phase { Inactive, Active, Finished };

class Manager {
public:
  Inventory inventory{};
  Phase phase = Phase::Inactive;
  uint32_t enemyId = 0, spawnFailures = 0, invalidEvents = 0;
  int kind = 0, age = 0, duration = 720, bucketA = 0, bucketB = 0;
  float elapsed = 0, fill = 0;
  Vec2 position{0, 128};
  bool rewardIssued = false, deathRequested = false;
  DeathReason pendingDeathReason = DeathReason::Defeated;
  RemovalReason lastRemovalReason = RemovalReason::None;
  uint32_t visualEnemyId = 0;
  bool visualsAttached = false;
  int health = 0, maxHealth = 0;
  int rewardDisplayFrames = 0, rewardDisplayScore = 0;
  float rewardDisplayMultiplier = 0;
  Vec2 rewardDisplayPosition{}; // Original screen-space UFO+54/+58, not current moving position.
  uint64_t nextVisualSequence = 1;
  std::vector<VisualEvent> visualEvents;

  void reset(); // Whole-session reset; distinct from native449f50 directReset.
  void
  directReset(); // Clear association/slots and stop loop; do not remove Enemy/visuals or reward.
  RewardPlan collectToken(int color, ResourceState& resources);
  RewardPlan collectToken(const TokenPickup& pickup, ResourceState& resources);
  // Host consumes actions immediately at UFO19. RequestDeath only marks lifeFlags;
  // Enemy18 must execute its own guarded death callback on the next effective update.
  TickResult tick(const WorldView& world, const EnemyHooks& hooks);
  void notifyEnemyRemoved(uint32_t id, RemovalReason reason);
  item_system::UfoView view() const;
  item_system::UfoView view(const EnemyHooks& hooks) const;
  RewardPlan absorb(int type, int stage = 1);
  float multiplier(const ResourceState& resources) const;
  RewardPlan finish(uint32_t killedId, const ResourceState& resources,
                    DeathReason reason = DeathReason::Defeated);
  std::vector<VisualEvent> drainVisualEvents();

private:
  int changingTokenType();
  void visual(VisualEventKind type, int interrupt = 0);
  void advanceClock(float rate);
  void releaseAssociation();
};
} // namespace th12::ufo_system
