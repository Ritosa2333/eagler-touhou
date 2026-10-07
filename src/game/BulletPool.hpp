#pragma once
#include <array>
#include <cstddef>
#include <cstdint>

namespace th12::bullet_pool {
class Slots {
public:
  static constexpr size_t capacity = 2000;
  std::array<uint16_t, capacity> phases{};
  size_t cursor = 0;
  void reset() {
    phases = {};
    cursor = 0;
  }
  // 40a250 searches from the saved pointer, wrapping at its phase5 sentinel.
  // Reserve before synchronous birth; commit the cursor AFTER birth/ANM.
  int reserve();
  void commit(size_t slot) { cursor = (slot + 1) % capacity; }
  void release(size_t slot) {
    if (slot < capacity)
      phases[slot] = 0;
  }
};
} // namespace th12::bullet_pool
