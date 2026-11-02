// TH12 1.00b portable C++ runtime. See NOTICE.md for rights.
#include "GameState.hpp"
#include "../native_easing.hpp"

namespace th12 {
float aim(float x, float y) {
  const auto p=multiplayer::target(x,y);
  return float(std::atan2(double(p[1]-y),double(p[0]-x)));
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
  // Native413700 also applies this when called synchronously by ECL setters.
  // With flag10000 it rewrites the absolute component even inside the bounds:
  // the combined coordinate's floor residue belongs to absolute, not relative.
  if (e.flags & 0x10000u) {
    e.x = std::clamp(e.x, e.clampX - e.clampWidth * .5f, e.clampX + e.clampWidth * .5f);
    e.y = std::clamp(e.y, e.clampY - e.clampHeight * .5f, e.clampY + e.clampHeight * .5f);
    e.ax = float(double(e.x) - e.rx);
    e.ay = float(double(e.y) - e.ry);
  }
}
float movementCoordinate(double v) {
  return enemy_motion::quantize(float(v));
}
void circularPosition(Enemy& e, bool relative) {
  auto& m = relative ? e.relativeCircle : e.absoluteCircle;
  const float angle = relative ? e.relativeAngle : e.angle;
  if (m.ellipse) {
    double difference = double(angle) - m.ellipseAngle;
    if (difference > pi)
      difference -= tau;
    else if (difference < -pi)
      difference += tau;
    // Native465280 stores its angle difference, followed by two4646e0
    // normalizations. 465390's polar pair is stored before scaling local X.
    const float localAngle = normalize(normalize(float(difference)));
    const float x = float(double(float(std::cos(double(localAngle)) * m.radius)) * m.ellipseRatio),
                y = float(std::sin(double(localAngle)) * m.radius),
                cosine = float(std::cos(double(m.ellipseAngle))),
                sine = float(std::sin(double(m.ellipseAngle)));
    const float rotatedX = float(double(x) * cosine - double(y) * sine),
                rotatedY = float(double(x) * sine + double(y) * cosine);
    (relative ? e.rx : e.ax) = movementCoordinate(double(m.centerX) + rotatedX);
    (relative ? e.ry : e.ay) = movementCoordinate(double(m.centerY) + rotatedY);
    return;
  }
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
  return native_easing::sample(value, mode);
}

} // namespace th12
