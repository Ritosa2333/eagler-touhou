#pragma once
#include "CustomGeometry.hpp"

namespace th12 {
// Original Stage+35dc..35f4 and40ff10 screen-space grid. The retail stage05
// stream selects mode1, a17x7 wave. This state never captures a render target.
class StageBackgroundEffect {
public:
  int mode = 0, columns = 0, rows = 0;
  float targetRadius = 0, radius = 0, angleX = 0, angleY = 0;
  uint32_t color = 0;
  std::vector<custom_geometry::Vertex> vertices;
  std::array<uint64_t, 16> keys{};
  void reset();
  bool begin(int kind);
  void tick(bool activeSpell);
  custom_geometry::Mesh draw(int column, const anm_logic::State&) const;
  bool owns(uint64_t key) const;
};
extern StageBackgroundEffect stageBackgroundEffect;
} // namespace th12
