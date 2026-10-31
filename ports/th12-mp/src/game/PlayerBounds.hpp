#pragma once
#include <array>

namespace th12::player_bounds {
struct Point {
  float x = 0, y = 0, z = 0;
};
struct Quad {
  float x = 0, y = 0, z = 0, width = 0, height = 0, angle = 0;
  int mode = 0, anchorX = 0, anchorY = 0;
};
std::array<Point, 4> vertices(const Quad&);
bool survives(int type, int age, float x, float y, const std::array<Point, 4>&);
bool hitRectangle(float x, float y, float width, float height, float ex, float ey, float ew,
                  float eh, bool laser);
} // namespace th12::player_bounds
