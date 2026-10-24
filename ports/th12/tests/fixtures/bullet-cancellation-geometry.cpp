#include "../../src/game/BulletCancellation.hpp"
extern "C" {
int point_visible(float x, float y) {
  return th12::bullet_cancellation::pointVisible(x, y);
}
int circle_contains(float x, float y, float ox, float oy, float radius, float hitbox) {
  return th12::bullet_cancellation::circleContains(x, y, ox, oy, radius, hitbox);
}
}
