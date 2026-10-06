// TH12 1.00b portable C++ runtime. See NOTICE.md for rights.
#include "GameState.hpp"

namespace th12 {
std::vector<Sprite> drawState;
std::array<float, 41> hud;
std::vector<AnimationPose> animationDrawState;
AnimationTransform resolveAnimationTransform(const anm_logic::Node& node, int depth) {
  const auto& s = node.vm.state();
  AnimationTransform result{s.x + s.engineX + s.offsetX,
                            s.y + s.engineY + s.offsetY,
                            s.z + s.engineZ + s.offsetZ,
                            s.sx,
                            s.sy,
                            s.rotation};
  if (node.parent && depth < 16) {
    auto found = animationScene.nodes().find(node.parent);
    if (found != animationScene.nodes().end()) {
      const auto parent = resolveAnimationTransform(*found->second, depth + 1);
      result.x += parent.x;
      result.y += parent.y;
      result.z += parent.z;
      const auto& immediate = found->second->vm.state();
      result.sx *= immediate.sx;
      result.sy *= immediate.sy;
      if (s.mode == 1 || s.mode == 3 || s.mode >= 14)
        result.rotation += immediate.rotation;
    }
  }
  return result;
}
void captureAnimationDraw() {
  animationDrawState.clear();
  std::vector<u32> ids;
  for (const auto& [id, node] : animationScene.nodes())
    if (node->alive && !node->customGeometry)
      ids.push_back(id);
  std::sort(ids.begin(), ids.end());
  for (u32 id : ids) {
    const auto& node = *animationScene.nodes().at(id);
    const auto& s = node.vm.state();
    const auto transform = resolveAnimationTransform(node);
    animationDrawState.push_back({transform.x + node.viewportX,
                                  transform.y + node.viewportY,
                                  transform.z,
                                  s.rx,
                                  s.ry,
                                  transform.rotation,
                                  transform.sx,
                                  transform.sy,
                                  s.u,
                                  s.v,
                                  s.uvScaleX,
                                  s.uvScaleY,
                                  s.geometryRadius,
                                  s.geometryWidth,
                                  s.primary,
                                  s.secondary,
                                  s.spriteBank,
                                  s.sprite,
                                  s.layer,
                                  s.mode,
                                  s.anchorX,
                                  s.anchorY,
                                  s.blend,
                                  s.geometryCount,
                                  s.flags,
                                  s.flags2,
                                  id,
                                  node.owner,
                                  1});
  }
}

} // namespace th12
