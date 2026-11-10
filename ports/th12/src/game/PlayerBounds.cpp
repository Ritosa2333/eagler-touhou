#include "PlayerBounds.hpp"
#include <cmath>

namespace th12::player_bounds {
std::array<Point, 4> vertices(const Quad& q) {
  if (q.mode == 0) {
    const auto edge = [](float p, float size, int anchor) {
      return anchor == 1   ? p
             : anchor == 2 ? float(double(p) - size)
                           : float(std::floor(float(double(p) - float(double(size) * .5))));
    };
    const float x = edge(q.x, q.width, q.anchorX), y = edge(q.y, q.height, q.anchorY);
    return {{{x, y, q.z},
             {float(double(x) + q.width), y, q.z},
             {x, float(double(y) + q.height), q.z},
             {float(double(x) + q.width), float(double(y) + q.height), q.z}}};
  }
  const float left = q.anchorX == 1 ? 0 : q.anchorX == 2 ? -q.width : -q.width * .5f;
  const float top = q.anchorY == 1 ? 0 : q.anchorY == 2 ? -q.height : -q.height * .5f;
  const float c = float(std::cos(double(q.angle))), s = float(std::sin(double(q.angle)));
  std::array<Point, 4> out{{{left, top, q.z},
                            {float(double(left) + q.width), top, q.z},
                            {left, float(double(top) + q.height), q.z},
                            {float(double(left) + q.width), float(double(top) + q.height), q.z}}};
  for (auto& p : out) {
    const float x = p.x, y = p.y;
    p.x = float(double(c) * x - double(s) * y + q.x);
    p.y = float(double(s) * x + double(c) * y + q.y);
  }
  return out;
}
bool survives(int type, int age, float x, float y, const std::array<Point, 4>& corners) {
  if (type == 2 || age < 10)
    return true;
  for (const auto& p : corners)
    if (p.x > 32 && p.x < 416 && p.y > 16 && p.y < 464)
      return true;
  return type == 3 && x > -224 && x < 224 && y >= -64;
}
bool hitRectangle(float x, float y, float width, float height, float ex, float ey, float ew,
                  float eh, bool laser) {
  const float el = float(double(ex) - double(ew) * .5), er = float(double(ex) + double(ew) * .5),
              et = float(double(ey) - double(eh) * .5), eb = float(double(ey) + double(eh) * .5);
  const float left = float(double(x) - double(width) * .5),
              right = float(double(x) + double(width) * .5),
              top = laser ? float(double(y) - height) : float(double(y) - double(height) * .5),
              bottom = laser ? y : float(double(y) + double(height) * .5);
  return eb >= 0 && !(right < el || bottom < et || left > er || top > eb);
}
} // namespace th12::player_bounds
