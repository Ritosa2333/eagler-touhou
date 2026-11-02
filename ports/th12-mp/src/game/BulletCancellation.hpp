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
// Original40cf60 uses BulletManager+44/+48/+50/+54, not the playfield.
// 409470 zeros the manager and409530 never initializes these four fields.
// Retail replay01 stage2 confirms the zero rectangle at the BossDead clear.
// Explicit rectangle arguments are useful for independently constructed probes.
inline bool viewportOverlaps(float x, float y, float width, float height,
                             float centerX = 0, float centerY = 0,
                             float viewportWidth = 0, float viewportHeight = 0) {
  const float left = float(double(x) - double(width) * .5),
              right = float(double(x) + double(width) * .5),
              top = float(double(y) - double(height) * .5),
              bottom = float(double(y) + double(height) * .5);
  const float viewportLeft = float(double(centerX) - double(viewportWidth) * .5),
              viewportRight = float(double(centerX) + double(viewportWidth) * .5),
              viewportTop = float(double(centerY) - double(viewportHeight) * .5),
              viewportBottom = float(double(centerY) + double(viewportHeight) * .5);
  return right >= viewportLeft && left <= viewportRight &&
         bottom >= viewportTop && top <= viewportBottom;
}
} // namespace bullet_cancellation
void cancelHostileBullets(bool respectCountdown = false);
void clearHostileViewport(bool convert);
void cancelHostileCircle(float x, float y, float radius, bool convert,
                         bool respectCountdown = false);
void cancelHostileRectangle(float x, float y, float width, float height, bool convert);
} // namespace th12
