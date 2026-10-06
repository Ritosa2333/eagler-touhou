#include "LaserCollision.hpp"
#include <cmath>
namespace th12::laser {
LineCollisionResult collidePlayerLine(const Vec3& origin, float angle, float transverseSize,
                                      float longitudinalSize, const PlayerCollision& p) {
  const float dx = float(double(p.position.x) - origin.x),
              dy = float(double(p.position.y) - origin.y);
  const float cosine = float(std::cos(double(-angle))), sine = float(std::sin(double(-angle)));
  const float x = float(double(cosine) * dx - double(sine) * dy),
              y = float(double(sine) * dx + double(cosine) * dy);
  const auto intersects = [&](float hw, float hh) {
    const float x1 = float(double(x) - hw), x2 = float(double(x) + hw), y1 = float(double(y) - hh),
                y2 = float(double(y) + hh);
    return double(x1) <= longitudinalSize && double(y1) <= double(transverseSize) * .5 &&
           double(x2) >= 0 && double(y2) >= -double(transverseSize) * .5;
  };
  if (!intersects(float(double(p.hitWidth) * 16), float(double(p.hitHeight) * 16)))
    return {};
  if (!intersects(p.hitWidth, p.hitHeight))
    return {Collision::Graze, false, false};
  // Invulnerable line intersection returns0, unlike ordinary bullet rectangle
  // 437810's return1; therefore it must not trigger laser clearing.
  if (p.dialogue || p.state == 2 || p.state == 3 || p.state == 4 || (p.flags & 2) ||
      p.invulnerability > 0)
    return {};
  return {Collision::Hit, true, true};
}
} // namespace th12::laser
