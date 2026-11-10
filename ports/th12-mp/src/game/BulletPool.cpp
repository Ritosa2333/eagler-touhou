#include "BulletPool.hpp"
namespace th12::bullet_pool {
int Slots::reserve() {
  for (size_t n = 0; n < capacity; ++n) {
    const size_t slot = (cursor + n) % capacity;
    if (phases[slot] == 0) {
      phases[slot] = 1;
      return int(slot);
    }
  }
  return -1;
}
} // namespace th12::bullet_pool
