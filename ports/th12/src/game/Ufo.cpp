#include "Ufo.hpp"
#include <algorithm>
#include <utility>

namespace th12::ufo_system {
namespace {
InventoryHudEvent stockEvent(Inventory& inventory, int logical, HudOperation op, int value) {
  InventoryHudEvent e;
  e.sequence = inventory.nextHudSequence++;
  e.logicalSlot = logical;
  e.physicalSlot = inventory.physicalSlot(logical);
  e.operation = op;
  e.value = value;
  return e;
}
bool validKind(int kind) {
  return kind == -1 || kind == 1 || kind == 2 || kind == 3;
}
} // namespace
int Inventory::physicalSlot(int logical) const {
  return (rotation + logical) % 3 + 1;
}
int Inventory::append(int color) {
  if (color <= 0 || color > 3)
    return 0;
  gateTimer = 0;
  if (colors[2])
    return 0;
  if (!colors[0]) {
    colors[0] = color;
    count = displayState = 1;
    return 0;
  }
  if (!colors[1]) {
    colors[1] = color;
    count = displayState = 2;
    return 0;
  }
  colors[2] = color;
  count = displayState = 3;
  if (colors[0] == color && colors[1] == color) {
    combination = color;
    return color;
  }
  if (colors[0] != color && colors[1] != color && colors[0] != colors[1]) {
    combination = -1;
    return -1;
  }
  colors = {colors[1], color, 0};
  count = 2;
  displayState = 4;
  // Native421a60: signal the old physical sequence before incrementing rotation.
  for (int i = 0; i < 3; i++)
    hudEvents.push_back(stockEvent(*this, i, HudOperation::Interrupt, i == 0 ? 9 : i == 1 ? 7 : 8));
  rotation = (rotation + 1) % 3;
  return 0;
}
void Inventory::clearSlots() {
  colors = {};
  count = displayState = 0;
}
bool Inventory::ready() const {
  return displayState == 3 && gateTimer >= 0;
}
void Inventory::refresh(int newSlot) {
  for (int i = 0; i < 3; i++) {
    hudEvents.push_back(stockEvent(*this, i, HudOperation::Interrupt, 10 + colors[i]));
    if (i == newSlot)
      hudEvents.push_back(stockEvent(*this, i, HudOperation::Interrupt, 17));
  }
  if (newSlot < 0 || newSlot > 2)
    return;
  const uint32_t overlay = nextOverlayId++;
  for (int value : {7 + newSlot, 10 + colors[newSlot]}) {
    InventoryHudEvent e;
    e.sequence = nextHudSequence++;
    e.target = HudTarget::PickupOverlay;
    e.logicalSlot = newSlot;
    e.physicalSlot = physicalSlot(newSlot);
    e.script = 81;
    e.layer = 21;
    e.value = value;
    e.overlayId = overlay;
    hudEvents.push_back(e);
  }
}
void Inventory::blink(bool active) {
  for (int i = 0; i < 3; i++)
    hudEvents.push_back(
        stockEvent(*this, i, HudOperation::SelectSprite, (active ? 63 : 59) + colors[i]));
}
std::vector<InventoryHudEvent> Inventory::drainHudEvents() {
  auto result = std::move(hudEvents);
  hudEvents.clear();
  return result;
}

void Manager::reset() {
  *this = Manager{};
}
void Manager::directReset() {
  // Native449f50 does not reset age/kind/buckets/fill, delete Enemy or refresh stock VMs.
  visual(VisualEventKind::DirectReset);
  phase = Phase::Inactive;
  enemyId = 0;
  inventory.clearSlots();
  deathRequested = false;
  lastRemovalReason = RemovalReason::StageReset;
}
RewardPlan Manager::collectToken(int color, ResourceState& resources) {
  return collectToken(TokenPickup{color, {}}, resources);
}
RewardPlan Manager::collectToken(const TokenPickup& pickup, ResourceState& resources) {
  RewardPlan plan;
  if (pickup.color < 0 || pickup.color > 2) {
    ++invalidEvents;
    return plan;
  }
  if (inventory.count >= 3) {
    const auto before = resources;
    resources.addPointValue(100000);
    plan.immediate = item_system::difference(before, resources);
    return plan;
  }
  int selected = inventory.count;
  inventory.append(pickup.color + 1);
  if (inventory.displayState == 4)
    --selected;
  inventory.refresh(selected);
  plan.inventoryChanged = true;
  plan.tokenPickupScript = 201 + pickup.color;
  plan.tokenPickupPosition = pickup.position;
  return plan;
}
void Manager::visual(VisualEventKind type, int interrupt) {
  VisualEvent event;
  event.sequence = nextVisualSequence++;
  event.kind = type;
  event.enemyId = visualEnemyId ? visualEnemyId : enemyId;
  event.position = position;
  event.ufoKind = kind;
  event.age = age;
  event.remainingTicks = duration - age;
  event.health = health;
  event.maxHealth = maxHealth;
  event.fill = fill;
  event.multiplier = rewardDisplayMultiplier;
  event.scoreDisplayed = rewardDisplayScore;
  event.interrupt = interrupt;
  if (type == VisualEventKind::Summoned) {
    event.bulletScript = 207;
    event.noticeScript = 82;
    event.panelScript = 134;
    event.healthScript = 136;
    event.sound = 54;
  } else if (type == VisualEventKind::FillCompleted) {
    event.bulletScript = 207;
    event.sound = 48;
  } else if (type == VisualEventKind::DeathBurst) {
    event.bulletScript = 208;
    event.sound = 6;
  } else if (type == VisualEventKind::Removed) {
    event.bulletScript = 207;
    event.panelScript = 134;
    event.healthScript = 136;
    event.sound = 54;
  } else if (type == VisualEventKind::DirectReset)
    event.sound = 54;
  else if (type == VisualEventKind::RewardDisplay)
    event.position = rewardDisplayPosition;
  visualEvents.push_back(event);
}
void Manager::advanceClock(float rate) {
  elapsed = item_system::add(elapsed, rate);
  age = int(elapsed);
}
void Manager::releaseAssociation() {
  inventory.clearSlots();
  inventory.refresh(-1); // Native44a357 precedes scene deletion/queued exits.
  visual(VisualEventKind::Removed, 1);
  visualsAttached = false;
  visualEnemyId = 0;
  if (lastRemovalReason == RemovalReason::None)
    lastRemovalReason =
        rewardIssued ? (pendingDeathReason == DeathReason::Dialogue ? RemovalReason::Dialogue
                                                                    : RemovalReason::Defeated)
                     : RemovalReason::NaturalExit;
  phase = Phase::Inactive;
  enemyId = 0;
  deathRequested = false;
}
TickResult Manager::tick(const WorldView& world, const EnemyHooks& hooks) {
  TickResult result;
  if (enemyId) {
    // Native blink occurs before lookup, even on the frame the Enemy disappeared.
    if (age % 16 == 1)
      inventory.blink(true);
    else if (age % 16 == 9)
      inventory.blink(false);
    const EnemyView enemy = hooks.query ? hooks.query(enemyId) : EnemyView{};
    if (!enemy.active) {
      releaseAssociation();
      result.removed = true;
      advanceClock(world.rate);
      // Native cleanup returns early: a freshly issued120tick reward display stays120.
      return result;
    }
    position = enemy.position;
    health = enemy.health;
    if (enemy.maxHealth > 0)
      maxHealth = enemy.maxHealth;
    if (world.dialogue) {
      deathRequested = true;
      pendingDeathReason = DeathReason::Dialogue;
      result.actions.push_back(
          {EnemyActionKind::RequestDeath, enemyId, 0, 2, DeathReason::Dialogue});
    }
    if (age == duration - 300 || world.bossPresent)
      result.actions.push_back({EnemyActionKind::SetPrivateInteger, enemyId, 0, 999});
    else if (age < duration - 300)
      result.actions.push_back({EnemyActionKind::SetPrivateInteger, enemyId, 0, -999});
    visual(VisualEventKind::PoseUpdated);
  } else if (inventory.ready()) {
    const int selected = inventory.combination;
    if (!validKind(selected)) {
      ++invalidEvents;
      return result;
    }
    static const char* names[4] = {"UFO_Red", "UFO_Blue", "UFO_Green", "UFO_Rainbow"};
    constexpr int healthTable[5] = {1600, 1900, 2100, 2300, 2500};
    EnemySpawn request;
    request.routine = names[selected == -1 ? 3 : selected - 1];
    request.health = healthTable[std::clamp(world.power / std::max(1, world.powerStep), 0, 4)];
    const uint32_t id = hooks.spawn ? hooks.spawn(request) : 0;
    if (!id) {
      ++spawnFailures;
      return result;
    }
    enemyId = visualEnemyId = id;
    visualsAttached = true;
    kind = selected;
    phase = Phase::Active;
    position = request.position;
    health = maxHealth = request.health;
    age = 0;
    elapsed = 0;
    fill = 0;
    bucketA = bucketB = 0;
    rewardIssued = deathRequested = false;
    pendingDeathReason = DeathReason::Defeated;
    lastRemovalReason = RemovalReason::None;
    inventory.refresh(-1);
    visual(VisualEventKind::Summoned, selected == -1 ? 10 : selected + 6);
    result.spawned = true;
  }
  if (rewardDisplayFrames > 0)
    --rewardDisplayFrames;
  advanceClock(world.rate);
  return result;
}
void Manager::notifyEnemyRemoved(uint32_t id, RemovalReason reason) {
  if (id == enemyId)
    lastRemovalReason = reason; // Cleanup stays in UFO19, not the caller's Enemy18.
}
item_system::UfoView Manager::view() const {
  return {phase == Phase::Active, kind, age, position};
}
item_system::UfoView Manager::view(const EnemyHooks& hooks) const {
  auto result = view();
  if (result.active && hooks.query) {
    const auto enemy = hooks.query(enemyId);
    result.active = enemy.active;
    result.position = enemy.position;
  }
  return result;
}
RewardPlan Manager::absorb(int type, int stage) {
  RewardPlan plan;
  if (phase != Phase::Active) {
    ++invalidEvents;
    return plan;
  }
  if (type == 1) {
    if (kind == -1)
      ++bucketB;
    else
      ++bucketA;
  } else if (type == 2) {
    if (kind == -1)
      ++bucketA;
    else
      ++bucketB;
  } else if (type < 10 || type > 12) {
    ++invalidEvents;
    return plan;
  }
  if (fill >= 1)
    return plan;
  const double increment = (type == 1 || type == 2)
                               ? 0.03200000151991844 - 0.0020000000949949026 * stage
                               : 0.10000000149011612;
  fill = float(double(fill) + increment);
  if (fill < 1)
    return plan;
  fill = 1;
  plan.filledNow = true;
  visual(VisualEventKind::FillCompleted, 2);
  const int item = kind == 1 ? 4 : kind == 3 ? 7 : kind == -1 ? changingTokenType() : 0;
  if (item)
    plan.spawns.push_back(SpawnRequest{item, position});
  return plan;
}
float Manager::multiplier(const ResourceState& resources) const {
  switch (kind) {
  case 1:
    return resources.power >= resources.maxPower ? 2.f : 1.f;
  case 2:
    return fill >= 1 ? 8.f : fill >= .5f ? 6.f : float(2 + double(fill) * 8);
  case 3:
    return 2.f;
  case -1:
    return fill >= 1 ? 4.f : fill >= .5f ? 3.f : float(2 + double(fill) * 2);
  default:
    return 1.f;
  }
}
RewardPlan Manager::finish(uint32_t killedId, const ResourceState& resources, DeathReason reason) {
  RewardPlan plan;
  if (killedId != enemyId || phase != Phase::Active || rewardIssued)
    return plan;
  if (deathRequested)
    reason = pendingDeathReason;
  pendingDeathReason = reason;
  plan.deathReason = reason;
  rewardIssued = true;
  phase = Phase::Finished;
  plan.effects.push_back(EnemySpawn{"UFO_EtBreak2", position, 3000});
  visual(VisualEventKind::DeathBurst);
  const float factor = multiplier(resources);
  if (bucketA || bucketB) {
    const int type = bucketA && bucketB ? 18 : bucketA ? 16 : 17;
    SpawnRequest aggregate{type, position, -item_system::pi / 2, 0};
    aggregate.aggregateKind = kind;
    aggregate.bucketA = bucketA;
    aggregate.bucketB = bucketB;
    aggregate.multiplier = factor;
    plan.spawns.push_back(aggregate);
    rewardDisplayFrames = 120;
    rewardDisplayMultiplier = factor;
    const int pointValue = resources.pointValue();
    rewardDisplayScore = kind == 1
                             ? int(double(pointValue * bucketA) * factor) + pointValue * bucketB
                             : int(double(pointValue * bucketB) * factor);
    rewardDisplayPosition = {position.x + 224, position.y + 16};
    visual(VisualEventKind::RewardDisplay);
  }
  if (kind == 1) {
    plan.spawns.push_back(SpawnRequest{4, position});
    plan.spawns.push_back(SpawnRequest{13, position});
  } else if (kind == 2)
    plan.spawns.push_back(SpawnRequest{14, position});
  else if (kind == 3) {
    plan.spawns.push_back(SpawnRequest{5, position});
    plan.spawns.push_back(SpawnRequest{15, position});
  } else if (kind == -1) {
    const int token = changingTokenType();
    if (token)
      plan.spawns.push_back(SpawnRequest{token, position});
  }
  return plan;
}
int Manager::changingTokenType() {
  const int color = inventory.colors[2];
  if (color < 1 || color > 3) {
    ++invalidEvents;
    return 0;
  }
  return color + 12;
}
std::vector<VisualEvent> Manager::drainVisualEvents() {
  auto result = std::move(visualEvents);
  visualEvents.clear();
  return result;
}
} // namespace th12::ufo_system
