#pragma once
#include "GameTypes.hpp"

namespace th12 {
struct LaserSegment {
  float x = 0, y = 0, angle = 0, length = 0, width = 0, alpha = 1, u = 0, v = 0;
  u32 color = 0xffffffffu;
  int bank = 0, sprite = 0, layer = 0, blend = 0;
  float textureLength = 0, headOffset = 0;
  u32 owner = 0;
};
static_assert(sizeof(LaserSegment) == 64);
extern std::vector<LaserSegment> laserDrawState;
void captureLaserDraw();
} // namespace th12
