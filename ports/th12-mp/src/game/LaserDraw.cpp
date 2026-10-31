#include "LaserDraw.hpp"
#include "GameState.hpp"
#include "LaserWorld.hpp"

namespace th12 {
std::vector<LaserSegment> laserDrawState;
void captureLaserDraw() {
  laserDrawState.clear();
  for (const auto* object : laserManager.ordered()) {
    const auto& s = object->state;
    const auto* node = animationScene.find(laserAnimationKey(s.serialId, laser::Role::Main));
    if (!node)
      continue;
    const auto& pose = node->vm.state();
    if (!(pose.flags & 1u) || s.width <= 0)
      continue;
    const auto color = pose.flags & 0x10000u ? pose.secondary : pose.primary;
    const auto segment = [&](const laser::Vec3& p, float angle, float length) {
      if (length <= 0)
        return;
      laserDrawState.push_back({p.x + 224, p.y + 16, angle, length, s.width, 1, pose.u, pose.v,
                                color, pose.spriteBank, pose.sprite, pose.layer, pose.blend,
                                s.parameters.maxLength, s.headOffset, 0xb0000000u + s.serialId});
    };
    if (s.kind != laser::Kind::Curve)
      segment(s.position, s.angle, s.length);
    else
      for (size_t i = 1; i < s.nodes.size(); ++i) {
        const auto& from = s.nodes[i].position;
        const auto& to = s.nodes[i - 1].position;
        const double x = double(to.x) - from.x, y = double(to.y) - from.y;
        segment(from, float(std::atan2(y, x)), float(std::hypot(x, y)));
      }
  }
}
} // namespace th12
