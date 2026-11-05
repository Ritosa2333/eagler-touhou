#include "HostilePool.hpp"
#include "GameState.hpp"
namespace th12 {
bullet_pool::Slots hostilePool;
std::array<Projectile*, bullet_pool::Slots::capacity> hostileSlots{};
namespace {
struct SlotReference { size_t slot; bool epoch; };
std::unordered_map<u32, SlotReference> slotsById;
bool indexEpoch = false;
}
void resetHostilePool() {
  hostilePool.reset();
  hostileSlots = {};
  slotsById.clear();
  indexEpoch = false;
}
void attachHostileSlot(Projectile& b) {
  if (b.physicalSlot < 0 || b.physicalSlot >= int(hostileSlots.size()))
    return;
  hostileSlots[b.physicalSlot] = &b;
  slotsById[b.entityId] = {size_t(b.physicalSlot), indexEpoch};
}
void releaseHostileBullet(u32 id) {
  const auto found = slotsById.find(id);
  if (found == slotsById.end())
    return;
  const auto slot = found->second.slot;
  if (const auto* bullet = hostileSlots[slot])
    hostilePool.cancelEffects[slot] = bullet->enemy.cancelEffect;
  hostilePool.release(slot);
  hostileSlots[slot] = nullptr;
  slotsById.erase(found);
}
void refreshHostilePointers() {
  // deque compaction moves values. Rebuild pointers after it, preserving the
  // physical pool identity/cursor; synchronous pushes preserve references.
  hostileSlots = {};
  // Every retained entry belonged to the previous pass. Mark live IDs with
  // the new epoch and retire only missing IDs, preserving hash-node storage.
  indexEpoch = !indexEpoch;
  hostilePool.phases = {};
  for (auto& b : bullets)
    if (!b.friendly && b.active && b.enemy.phase != eb::Phase::Inactive && b.physicalSlot >= 0) {
      attachHostileSlot(b);
      hostilePool.phases[b.physicalSlot] = 1;
    }
  std::erase_if(slotsById, [](const auto& entry) { return entry.second.epoch != indexEpoch; });
}
} // namespace th12
