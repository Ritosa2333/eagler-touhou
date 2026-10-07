#include "HostilePool.hpp"
#include "GameState.hpp"
namespace th12 {
bullet_pool::Slots hostilePool;
std::array<Projectile*, bullet_pool::Slots::capacity> hostileSlots{};
namespace {
std::unordered_map<u32, size_t> slotsById;
}
void resetHostilePool() {
  hostilePool.reset();
  hostileSlots = {};
  slotsById.clear();
}
void attachHostileSlot(Projectile& b) {
  if (b.physicalSlot < 0 || b.physicalSlot >= int(hostileSlots.size()))
    return;
  hostileSlots[b.physicalSlot] = &b;
  slotsById[b.entityId] = b.physicalSlot;
}
void releaseHostileBullet(u32 id) {
  const auto found = slotsById.find(id);
  if (found == slotsById.end())
    return;
  hostilePool.release(found->second);
  hostileSlots[found->second] = nullptr;
  slotsById.erase(found);
}
void refreshHostilePointers() {
  // deque compaction moves values. Rebuild pointers after it, preserving the
  // physical pool identity/cursor; synchronous pushes preserve references.
  hostileSlots = {};
  slotsById.clear();
  hostilePool.phases = {};
  for (auto& b : bullets)
    if (!b.friendly && b.active && b.enemy.phase != eb::Phase::Inactive && b.physicalSlot >= 0) {
      attachHostileSlot(b);
      hostilePool.phases[b.physicalSlot] = 1;
    }
}
} // namespace th12
