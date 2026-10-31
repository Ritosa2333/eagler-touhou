#include "FriendlyPool.hpp"
#include "GameState.hpp"
namespace th12 {
std::array<Projectile*, FriendlyCapacity> friendlySlots{};
void resetFriendlyPool() {
  friendlySlots = {};
}
int reserveFriendlySlot() {
  // Original439660 always scans the fixed array from slot0, without a cursor.
  for (int slot = int(multiplayer::currentSeat) * FriendlyPerSeat; slot < int(multiplayer::currentSeat + 1) * FriendlyPerSeat; ++slot)
    if (!friendlySlots[slot] || !friendlySlots[slot]->active)
      return slot;
  return -1;
}
void attachFriendlySlot(Projectile& bullet) {
  if (bullet.physicalSlot >= 0 && bullet.physicalSlot < FriendlyCapacity)
    friendlySlots[bullet.physicalSlot] = &bullet;
}
void refreshFriendlyPointers() {
  // deque compaction moves values; pushes during a query preserve references.
  resetFriendlyPool();
  for (auto& bullet : bullets)
    if (bullet.friendly && bullet.active)
      attachFriendlySlot(bullet);
}
} // namespace th12
