// TH12 1.00b portable C++ runtime. See NOTICE.md for rights.
#include "GameState.hpp"

namespace th12 {
float aim(float x, float y) {
  return float(std::atan2(double(py / 128.f - y), double(px / 128.f - x)));
}
float normalize(float angle) {
  while (angle > pi)
    angle -= tau;
  while (angle < -pi)
    angle += tau;
  return angle;
}
float mirrorAngle(float angle) {
  return normalize(float(double(pi / 2) - normalize(float(double(angle) - pi / 2))));
}
void combinePosition(Enemy& e) {
  const float x = float(double(e.ax) + e.rx), y = float(double(e.ay) + e.ry);
  const float vx = float(double(x) - e.x), vy = float(double(y) - e.y);
  e.x = enemy_motion::quantize(float(double(e.x) + vx));
  e.y = enemy_motion::quantize(float(double(e.y) + vy));
}
float movementCoordinate(double v) {
  return enemy_motion::quantize(float(v));
}
void circularPosition(Enemy& e, bool relative) {
  auto& m = relative ? e.relativeCircle : e.absoluteCircle;
  const float angle = relative ? e.relativeAngle : e.angle;
  (relative ? e.rx : e.ax) =
      movementCoordinate(double(m.centerX) + float(std::cos(double(angle)) * m.radius));
  (relative ? e.ry : e.ay) =
      movementCoordinate(double(m.centerY) + float(std::sin(double(angle)) * m.radius));
}
Enemy* primaryBoss() {
  for (auto& p : enemies)
    if (p->active && p->boss && p->bossSlot == 0)
      return p.get();
  return nullptr;
}
float easing(float value, int mode) {
  const double t = std::clamp(value, 0.f, 1.f);
  if (mode >= 1 && mode <= 3)
    return float(std::pow(t, mode + 1));
  if (mode >= 4 && mode <= 6)
    return float(1 - std::pow(1 - t, mode - 2));
  if (mode >= 9 && mode <= 11) {
    const int n = mode - 7;
    return float(t < .5 ? std::pow(2 * t, n) * .5 : 1 - std::pow(2 - 2 * t, n) * .5);
  }
  if (mode >= 12 && mode <= 14) {
    const int n = mode - 10;
    return float(t < .5 ? (1 - std::pow(1 - 2 * t, n)) * .5 : (std::pow(2 * t - 1, n) + 1) * .5);
  }
  return mode == 15 ? 0.f : mode == 16 ? 1.f : float(t);
}

} // namespace th12
