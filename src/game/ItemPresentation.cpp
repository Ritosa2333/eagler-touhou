#include "ItemPresentation.hpp"
#include "GameState.hpp"
#include "UfoEffects.hpp"
#include <unordered_set>

namespace th12 {
namespace {
constexpr uint64_t KeyBase = 0x7100000000000000ull;
uint64_t itemKey(uint32_t id, int slot) {
  return KeyBase + (uint64_t(slot + 1) << 40) + id;
}
void enable(anm_logic::Node* node, bool shown) {
  if (!node)
    return;
  auto& state = node->vm.control();
  state.flags = shown ? state.flags | 2u : state.flags & ~2u;
}
} // namespace
ItemPresentation itemPresentation;

uint64_t ItemPresentation::transientKey() {
  return KeyBase + (uint64_t(16) << 40) + serial_++;
}

anm_logic::Node* ItemPresentation::bindScreen(anm_logic::Scene& scene, int bank, int script) {
  const auto key = transientKey();
  return scene.bind(key, 0xeb000000u + serial_, bank, script, anm_logic::Membership::Primary, 0, 0,
                    {}, false, nullptr, 0, 0, 23);
}

void ItemPresentation::reset(anm_logic::Scene& scene, const ufo_system::Inventory& inventory) {
  ufoEffects.reset();
  items_.clear();
  stock_ = {};
  overlays_.clear();
  ring_ = panel_ = health_ = 0;
  serial_ = 1;
  lastStockFrame_ = -1;
  hasUfoPose_ = false;
  lastUfoPose_ = {};
  // Native41d66a..41d70a binds all three VMs before their position interrupts.
  for (int i = 0; i < 3; i++) {
    stock_[i] = KeyBase + uint64_t(i + 1);
    scene.bind(stock_[i], StockOwner + uint32_t(i + 1), 7, 80, anm_logic::Membership::Embedded, 0,
               0, {}, false, nullptr, 0, 0, 20);
  }
  for (int i = 0; i < 3; i++)
    scene.interrupt(stock_[i], 7 + i, true);
  for (int i = 0; i < 3; i++) {
    const int physical = inventory.physicalSlot(i) - 1;
    scene.interrupt(stock_[physical], 10 + inventory.colors[i], true);
  }
}

ItemPresentation::ItemBinding* ItemPresentation::ensureItem(anm_logic::Scene& scene,
                                                            const item_system::Entity& entity) {
  if (!entity.id || entity.state == item_system::State::DelayedPoint || !scene.registry.bank(0))
    return nullptr;
  auto [at, created] = items_.try_emplace(entity.id);
  auto& record = at->second;
  const int script = entity.visualScript ? entity.visualScript : entity.type + 165;
  if (created || !record.body) {
    record.body = itemKey(entity.id, 0);
    record.script = script;
    record.aggregate = entity.type >= 16;
    if (record.aggregate) {
      // 42771f..427724 uses ECX=0. The first surviving Item22 tail writes
      // engine position; a same-frame pickup deliberately skips that tail.
      scene.bind(record.body, 0x20000000u + entity.id, 0, script, anm_logic::Membership::Primary, 0,
                 0, {}, false, nullptr, 0, 0, 23);
    } else {
      scene.bind(record.body, 0x20000000u + entity.id, 0, script, anm_logic::Membership::Embedded,
                 entity.position.x, entity.position.y);
      // Types9/10..18 have no embedded offscreen-arrow binding.
      if (entity.type >= 1 && entity.type <= 8) {
        record.arrow = itemKey(entity.id, 1);
        scene.bind(record.arrow, 0x30000000u + entity.id, 0, entity.type + 185,
                   anm_logic::Membership::Embedded, entity.position.x, 8);
      }
    }
    record.lastVisualAge = 0;
    record.interruptVersion = 0;
  }
  return &record;
}

void ItemPresentation::updateItemPose(anm_logic::Scene& scene, ItemBinding& record,
                                      const item_system::Entity& entity) {
  if (record.aggregate) {
    scene.position(record.body, entity.position.x + 224, entity.position.y + 16);
  } else {
    scene.position(record.body, entity.position.x, entity.position.y);
    enable(scene.find(record.body), entity.type >= 9 || entity.position.y >= -8);
    if (record.arrow) {
      scene.position(record.arrow, entity.position.x, 8);
      auto* arrow = scene.find(record.arrow);
      enable(arrow, entity.position.y < -8);
      if (arrow) {
        // Original draw truncates, then writes a byte; preserve wrap at
        // y<-40 rather than adding an unverified alpha clamp.
        const uint32_t alpha =
            uint32_t(anm_logic::nativeInt(float(double(entity.position.y + 8) * 255 / 32))) & 255u;
        auto& state = arrow->vm.control();
        state.primary = (state.primary & 0xffffffu) | (alpha << 24);
      }
    }
  }
}

void ItemPresentation::releaseItem(anm_logic::Scene& scene, ItemBinding& record) {
  if (record.aggregate) {
    if (!record.bodyReleased)
      scene.interrupt(record.body, 1);
    record.bodyReleased = true;
  } else {
    scene.remove(record.body);
    if (record.arrow)
      scene.remove(record.arrow);
  }
}

void ItemPresentation::inventoryEvent(anm_logic::Scene& scene,
                                      const ufo_system::InventoryHudEvent& event) {
  uint64_t key = 0;
  if (event.target == ufo_system::HudTarget::Stock) {
    if (event.physicalSlot < 1 || event.physicalSlot > 3)
      return;
    key = stock_[event.physicalSlot - 1];
  } else {
    auto at = overlays_.find(event.overlayId);
    if (at == overlays_.end()) {
      auto* node = bindScreen(scene, 7, event.script);
      if (!node)
        return;
      key = node->key;
      overlays_[event.overlayId] = key;
    } else
      key = at->second;
  }
  if (event.operation == ufo_system::HudOperation::SelectSprite) {
    if (auto* node = scene.find(key)) {
      auto& state = node->vm.control();
      state.sprite = event.value;
      state.spriteBank = 7;
    }
  } else
    scene.interrupt(key, event.value, event.immediate);
}

void ItemPresentation::visualEvent(anm_logic::Scene& scene, const ufo_system::VisualEvent& event) {
  using Visual = ufo_system::VisualEventKind;
  if (event.kind == Visual::Summoned || event.kind == Visual::FillCompleted ||
      event.kind == Visual::DeathBurst)
    sound_system::queue.play(event.sound, event.position.x);
  else if (event.kind == Visual::Removed || event.kind == Visual::DirectReset)
    sound_system::queue.stop(event.sound);
  using K = ufo_system::VisualEventKind;
  switch (event.kind) {
  case K::Summoned: {
    // 44a942 calls40fbe0(type2) before front82. Its Primary27 callback
    // owns the summon flash and subsequent random trail births.
    ufoEffects.summon(scene, event.position.x, event.position.y);
    if (auto* notice = bindScreen(scene, 7, event.noticeScript))
      scene.interrupt(notice->key, event.interrupt);
    ring_ = transientKey();
    scene.bind(ring_, 0xfffd0001u, 0, event.bulletScript, anm_logic::Membership::Primary,
               event.position.x, event.position.y, {}, false, nullptr, 224, 16, 23);
    if (auto* node = bindScreen(scene, 7, event.panelScript))
      panel_ = node->key;
    health_ = transientKey();
    scene.bind(health_, 0xfffd0003u, 7, event.healthScript, anm_logic::Membership::Primary,
               event.position.x, event.position.y, {}, false, nullptr, 224, 16, 23);
    lastUfoPose_ = event;
    hasUfoPose_ = true;
    break;
  }
  case K::PoseUpdated:
    lastUfoPose_ = event;
    hasUfoPose_ = true;
    break;
  case K::FillCompleted:
    scene.interrupt(ring_, event.interrupt);
    break;
  case K::DeathBurst: {
    const auto key = transientKey();
    scene.bind(key, 0xeb000000u + serial_, 0, event.bulletScript, anm_logic::Membership::Primary,
               event.position.x, event.position.y, {}, false, nullptr, 224, 16, 23);
    ufoEffects.defeat(scene, event.position.x, event.position.y);
    break;
  }
  case K::Removed:
    scene.remove(ring_);
    scene.interrupt(panel_, event.interrupt);
    scene.interrupt(health_, event.interrupt);
    ring_ = panel_ = health_ = 0;
    hasUfoPose_ = false;
    break;
  case K::DirectReset:
    // Native449f50 drops the association without deleting the existing VMs.
    hasUfoPose_ = false;
    break;
  case K::RewardDisplay:
    // FontManager draws multiplier/score from Manager's retained display
    // fields; no invented ANM text root or display-side timer is created.
    break;
  }
}

void ItemPresentation::consume(anm_logic::Scene& scene, ufo_system::Manager& manager) {
  for (const auto& event : manager.inventory.drainHudEvents())
    inventoryEvent(scene, event);
  for (const auto& event : manager.drainVisualEvents())
    visualEvent(scene, event);
  updateUfoPose(scene, manager);
}

void ItemPresentation::plan(anm_logic::Scene& scene, const ufo_system::RewardPlan& plan) {
  if (plan.tokenPickupScript) {
    const auto key = transientKey();
    scene.bind(key, 0xeb000000u + serial_, 0, plan.tokenPickupScript,
               anm_logic::Membership::Primary, plan.tokenPickupPosition.x,
               plan.tokenPickupPosition.y, {}, false, nullptr, 224, 16, 23);
  }
}

void ItemPresentation::event(anm_logic::Scene& scene, const item_system::Event& event) {
  using K = item_system::EventKind;
  const auto& entity = event.entity;
  switch (event.kind) {
  case K::AnimationInterrupt:
    if (auto* record = ensureItem(scene, entity)) {
      scene.interrupt(record->body, event.value);
      record->interruptVersion = entity.visualInterruptVersion;
    }
    break;
  case K::AnimationRebind: {
    const auto old = items_.find(entity.id);
    const bool alreadyBound = old != items_.end() && scene.find(old->second.body);
    if (auto* record = ensureItem(scene, entity)) {
      // A delayed point is discovered through its first-bind event. Execute
      // that bind once; color changes of an existing token reset its old VM.
      if (alreadyBound || record->script != event.value)
        scene.rebind(record->body, 0, event.value, false);
      record->script = event.value;
      record->lastVisualAge = 0;
      record->interruptVersion = entity.visualInterruptVersion;
    }
    break;
  }
  case K::HintCreated:
    if (auto* record = ensureItem(scene, entity)) {
      record->hint = transientKey();
      record->hintGeneration = entity.hintGeneration;
      // ECX=0 at original factory; only a surviving tail supplies position.
      scene.bind(record->hint, 0x40000000u + entity.id, 0, entity.hintScript,
                 anm_logic::Membership::Primary, 0, 0, {}, false, nullptr, 0, 0, 23);
      scene.interrupt(record->hint, event.value);
    }
    break;
  case K::HintRemoved:
    if (auto at = items_.find(entity.id); at != items_.end()) {
      scene.remove(at->second.hint);
      at->second.hint = 0;
    }
    break;
  case K::HintCollected:
    if (auto at = items_.find(entity.id); at != items_.end()) {
      scene.interrupt(at->second.hint, event.value);
      at->second.hint = 0;
    }
    break;
  case K::CollectOrdinary:
  case K::CollectToken:
  case K::CollectAggregate:
  case K::Absorbed:
    if (auto* record = ensureItem(scene, entity))
      releaseItem(scene, *record);
    break;
  default:
    break;
  }
}

void ItemPresentation::itemTail(anm_logic::Scene& scene, const item_system::Entity& entity) {
  auto* record = ensureItem(scene, entity);
  if (!record)
    return;
  if (!record->aggregate) {
    if (auto* node = scene.find(record->body); node && (node->vm.state().flags & 1))
      scene.tickEmbedded(record->body);
    if (record->arrow)
      if (auto* node = scene.find(record->arrow); node && (node->vm.state().flags & 1))
        scene.tickEmbedded(record->arrow);
  }
  record->lastVisualAge = entity.visualAge + 1;
  updateItemPose(scene, *record, entity);
  if (record->hint)
    scene.position(record->hint, entity.position.x + 224, entity.position.y + 16);
}

void ItemPresentation::updateUfoPose(anm_logic::Scene& scene, const ufo_system::Manager& manager) {
  if (!hasUfoPose_)
    return;
  const auto& event = lastUfoPose_;
  scene.position(ring_, event.position.x, event.position.y);
  if (auto* root = scene.find(ring_))
    for (const auto* child : scene.children(root->id))
      if (child->vm.state().script == 206) {
        auto& state = scene.nodes().at(child->id)->vm.control();
        state.rx = float(double(event.fill) * anm_logic::Tau);
        state.flags |= 4;
      }
  if (manager.age >= 60 && event.maxHealth > 0) {
    if (auto* node = scene.find(health_)) {
      auto& state = node->vm.control();
      state.sx = float(double(event.health) / event.maxHealth);
      state.flags |= 8;
    }
    scene.position(health_, event.position.x, event.position.y);
  }
}

void ItemPresentation::sync(anm_logic::Scene& scene, const item_system::Manager& manager,
                            const ufo_system::Manager& ufo, int frame) {
  if (lastStockFrame_ != frame) {
    for (auto key : stock_)
      scene.tickEmbedded(key);
    lastStockFrame_ = frame;
  }
  std::unordered_set<uint32_t> active;
  for (const auto& entity : manager.entities)
    if (entity.active()) {
      active.insert(entity.id);
      auto* record = ensureItem(scene, entity);
      if (!record)
        continue;
      // A host without Item22's precise tail hook can catch up by the original
      // successful-tail counter. This never advances a newly reused slot0.
      if (!record->aggregate)
        while (record->lastVisualAge < entity.visualAge) {
          scene.tickEmbedded(record->body);
          if (record->arrow)
            scene.tickEmbedded(record->arrow);
          ++record->lastVisualAge;
        }
      if (!record->aggregate || record->lastVisualAge > 0 || entity.visualAge > 0)
        updateItemPose(scene, *record, entity);
    }
  for (auto at = items_.begin(); at != items_.end();)
    if (!active.contains(at->first)) {
      auto& record = at->second;
      if (!record.aggregate) {
        scene.remove(record.body);
        if (record.arrow)
          scene.remove(record.arrow);
      }
      if (record.hint)
        scene.remove(record.hint);
      at = items_.erase(at);
    } else
      ++at;
  for (auto at = overlays_.begin(); at != overlays_.end();)
    if (!scene.find(at->second))
      at = overlays_.erase(at);
    else
      ++at;
  updateUfoPose(scene, ufo);
}

void resetItemPresentation() {
  itemPresentation.reset(animationScene, ufoManager.inventory);
}
void consumeUfoPresentation() {
  itemPresentation.consume(animationScene, ufoManager);
}
void itemPresentationPlan(const ufo_system::RewardPlan& plan) {
  itemPresentation.plan(animationScene, plan);
}
void itemPresentationEvent(const item_system::Event& event) {
  itemPresentation.event(animationScene, event);
}
void itemPresentationTick(const item_system::Entity& entity) {
  itemPresentation.itemTail(animationScene, entity);
}
void syncItemPresentation() {
  itemPresentation.sync(animationScene, itemManager, ufoManager, frame);
}
} // namespace th12
