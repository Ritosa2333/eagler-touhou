#pragma once
#include <cstdint>
namespace th12::enemy_life {
// Native412050 stores accumulated damage in seven-times life during a spell.
// Keep the remainder: dividing each hit by seven would discard small hits.
inline int32_t expanded(int32_t life) {
  return int32_t(uint32_t(life) * 7u);
}
inline int32_t reduced(int32_t raw, int32_t threshold) {
  const int32_t remainder = int32_t(uint32_t(raw) - uint32_t(threshold) * 7u);
  return int32_t(uint32_t(remainder / 7) + uint32_t(threshold));
}
inline void hurt(int32_t& life, int32_t& raw, int32_t threshold, uint32_t flags, int32_t damage) {
  if (flags & 1u) {
    raw = int32_t(uint32_t(raw) - uint32_t(damage));
    life = reduced(raw, threshold);
  } else {
    life = int32_t(uint32_t(life) - uint32_t(damage));
  }
}
} // namespace th12::enemy_life
