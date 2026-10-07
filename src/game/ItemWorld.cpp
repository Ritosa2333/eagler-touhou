// TH12 1.00b portable C++ runtime. See NOTICE.md for rights.
#include "GameState.hpp"
#include "ItemPresentation.hpp"

namespace th12 {
ufo_system::EnemyHooks ufoHooks() {
  return {[](const ufo_system::EnemySpawn& request) -> u32 {
            auto* e = spawn(request.routine, request.position.x, request.position.y, request.health,
                            0, 0);
            if (e)
              e->isUfo = true;
            return e ? e->id : 0;
          },
          [](u32 id) -> ufo_system::EnemyView {
            for (const auto& p : enemies)
              if (p->id == id)
                return {p->active, p->id, {p->x, p->y}, p->life, p->maxLife};
            return {};
          },
          [](u32 id, int index, int value) {
            for (auto& p : enemies)
              if (p->id == id && index >= 0 && index < 4)
                p->variables[index] = u32(value);
          }};
}
void applyUfoPlan(const ufo_system::RewardPlan& plan) {
  for (const auto& effect : plan.effects)
    spawn(effect.routine, effect.position.x, effect.position.y, effect.health, 0, 0);
  itemPresentationPlan(plan);
  consumeUfoPresentation();
  for (const auto& request : plan.spawns)
    itemManager.spawn(request);
  if (plan.filledNow)
    eventBits |= 64;
}

void tickUfo() {
  const auto hooks = ufoHooks();
  const auto result = ufoManager.tick(
      {power, stageNumber, primaryBoss() != nullptr, dialogue > 0, 1, resources.powerStep}, hooks);
  for (const auto& action : result.actions)
    for (auto& p : enemies)
      if (p->id == action.enemyId && p->active) {
        if (action.kind == ufo_system::EnemyActionKind::RequestDeath) {
          p->lifeFlags |= u32(action.value);
          p->deathReason = action.reason;
        } else if (action.privateIndex >= 0 && action.privateIndex < 4)
          p->variables[action.privateIndex] = u32(action.value);
      }
  consumeUfoPresentation();
}

void tickItems(bool focus) {
  const auto hooks = ufoHooks();
  item_system::FrameView itemFrame;
  itemFrame.animate = itemPresentationTick;
  itemFrame.player =
      item_system::PlayerView::native(character, {px / 128.f, py / 128.f}, focus, playerState);
  itemFrame.ufo = ufoManager.view(hooks);
  itemFrame.dialogue = dialogue > 0;
  itemFrame.inventoryColors = &ufoManager.inventory.colors;
  itemFrame.inventoryCount = &ufoManager.inventory.count;
  itemManager.tick(itemFrame, [&](const item_system::Event& event) {
    itemPresentationEvent(event);
    using K = item_system::EventKind;
    switch (event.kind) {
    case K::CollectOrdinary:
      sound_system::queue.play(38, event.entity.position.x); // Common426da4 pickup tail.
      item_system::collectOrdinary(resources, event.entity, itemFrame.player);
      eventBits |= 64;
      break;
    case K::CollectAggregate:
      sound_system::queue.play(39, event.entity.position.x);
      sound_system::queue.play(38, event.entity.position.x);
      item_system::collectAggregate(resources, event.entity);
      eventBits |= 64;
      break;
    case K::CollectToken:
      sound_system::queue.play(38, event.entity.position.x);
      applyUfoPlan(ufoManager.collectToken({event.value, event.entity.position}, resources));
      eventBits |= 64;
      break;
    case K::Absorbed:
      sound_system::queue.play(38, event.entity.position.x); // Native42696d.
      applyUfoPlan(ufoManager.absorb(event.entity.type, stageNumber));
      break;
    case K::RankDelta:
      resources.addRank(event.value);
      break;
    default:
      break;
    }
  });
}

} // namespace th12
