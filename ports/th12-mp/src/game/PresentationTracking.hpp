#pragma once
#include <cstdint>
#include <unordered_map>

// Presentation sidecar only. These values never drive a game decision,
// consume RNG, or enter Replay/save state.
namespace th12::presentation {
inline uint32_t epoch = 1;
inline std::unordered_map<uint32_t, uint32_t> ownerRevisions;
inline void reset() { ++epoch; ownerRevisions.clear(); }
inline void discontinuity(uint32_t owner) { ++ownerRevisions[owner]; }
inline uint32_t revision(uint32_t owner) {
  const auto at = ownerRevisions.find(owner);
  return at == ownerRevisions.end() ? 0 : at->second;
}
} // namespace th12::presentation
