#pragma once

namespace th12 {
namespace bullet_cancellation {
// Original40d560 receives2 for both half-extents when spawning item9.
inline bool pointVisible(float x, float y) {
  return double(x) + 2 > -192 && double(x) - 2 < 192 && double(y) + 2 > 0 && double(y) - 2 < 448;
}
// Original40cb11..40cb62 stores each delta, the expanded radius and the
// summed squared distance as float, before comparing against radius squared.
inline bool circleContains(float x, float y, float originX, float originY, float radius,
                           float hitbox) {
  const float dx = float(double(x) - originX), dy = float(double(y) - originY);
  const float reach = float(double(radius) + double(hitbox) * .5);
  const float distance = float(double(dx) * dx + double(dy) * dy);
  return distance <= double(reach) * reach;
}
inline bool rectangleContains(float x, float y, float originX, float originY, float width,
                              float height) {
  return double(x) >= double(originX) - double(width) * .5 &&
         double(x) <= double(originX) + double(width) * .5 &&
         double(y) >= double(originY) - double(height) * .5 &&
         double(y) <= double(originY) + double(height) * .5;
}
// Original40cf60 uses each bullet's hit rectangle against the manager's
// 384x448 viewport, including equality at an edge.
inline bool viewportOverlaps(float x, float y, float width, float height) {
  const float left = float(double(x) - double(width) * .5),
              right = float(double(x) + double(width) * .5),
              top = float(double(y) - double(height) * .5),
              bottom = float(double(y) + double(height) * .5);
  return right >= -192 && left <= 192 && bottom >= 0 && top <= 448;
}
} // namespace bullet_cancellation
void cancelHostileBullets();
void clearHostileViewport(bool convert);
void cancelHostileCircle(float x, float y, float radius, bool convert,
                         bool respectCountdown = false);
void cancelHostileRectangle(float x, float y, float width, float height, bool convert);
} // namespace th12
