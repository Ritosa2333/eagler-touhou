// TH12 1.00b portable C++ runtime. See NOTICE.md for rights.
#include "../bullet_visuals.hpp"
#include "BulletPool.hpp"
#include "GameState.hpp"
#include "UfoEffects.hpp"

namespace th12 {
std::vector<Sprite> drawState;
std::array<float, 41> hud;
std::vector<AnimationPose> animationDrawState;
std::vector<AnimationDrawSchedule> animationScheduleState;
namespace {
// 45dfa0 installs wrappers that invoke4611d0 for each registered layer.
constexpr int LayerPriority[32] = {4,  6,  8,  9,  11, 13, 16, 17, 18, 19, 20, 23, 25, 26, 28, 30,
                                   34, 36, 38, 41, 42, 50, 51, 52, 63, 64, -1, -1, 14, 39, 66, 65};
int drawPriority(const anm_logic::Node& node) {
  if (node.membership == anm_logic::Membership::Embedded) {
    if (node.owner == 0xffff0000u)
      return 24; // 435be3 registers437670;437600 draws Player+14 directly.
    if ((node.owner & 0xffff0000u) == 0xfffe0000u)
      return 44; // 41f450 draws stock VMs inside HUD's41f040 callback.
    if ((node.owner & 0xf0000000u) == 0x10000000u)
      return 31; // 4095f9 registers40a220, irrespective of Bullet VM.layer.
    if ((node.owner & 0xf0000000u) == 0x20000000u || (node.owner & 0xf0000000u) == 0x30000000u)
      return 27; // 4259d4 registers4273c0; bodies/offscreen arrows.
  }
  if (node.vm.state().flags & 0x10000000u)
    return -1; // 4611d0 skips registered VMs owned by a custom renderer.
  if (node.membership == anm_logic::Membership::Secondary)
    return node.vm.state().layer == 31 ? 65 : 66; // 4610d0 rebuilds only30/31.
  const int layer = node.vm.state().layer;
  return layer >= 0 && layer < 32 ? LayerPriority[layer] : -1;
}
} // namespace
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
  animationScheduleState.clear();
  std::vector<const anm_logic::Node*> nodes;
  // 460fe0 rebuilds layer lists in registered chain order, including head
  // insertion. A node id sort would silently discard that ordering.
  for (const auto membership : {anm_logic::Membership::Primary, anm_logic::Membership::Secondary,
                                anm_logic::Membership::Embedded}) {
    const auto chain = animationScene.chain(membership);
    nodes.insert(nodes.end(), chain.begin(), chain.end());
  }
  u32 order = 0;
  std::unordered_map<u32, u32> bulletOrder;
  std::unordered_map<u32, const Projectile*> hostileDraw;
  // 40a020 appends surviving physical slots in ascending order to six
  // appearance groups;40a220 draws group0..5, not allocation/id order.
  for (const auto& bullet : bullets)
    if (!bullet.friendly && bullet.active && bullet.physicalSlot >= 0 && bullet.enemy.type >= 0 &&
        bullet.enemy.type < int(bullet_visuals::appearances.size())) {
      hostileDraw[0x10000000u + bullet.entityId] = &bullet;
      bulletOrder[0x10000000u + bullet.entityId] =
          u32(bullet_visuals::appearances[bullet.enemy.type].group * bullet_pool::Slots::capacity +
              bullet.physicalSlot);
    }
  for (const auto* pointer : nodes) {
    const auto& node = *pointer;
    if (node.customGeometry && !ufoEffects.owns(node.id))
      continue;
    const u32 id = node.id;
    const auto& s = node.vm.state();
    const int priority = drawPriority(node);
    u32 drawOrder = order++;
    if (priority == 31)
      if (const auto at = bulletOrder.find(node.owner); at != bulletOrder.end())
        drawOrder = at->second;
    // 460c90 selects camera2 at layer4; the battle callbacks and HUD44
    // inherit it. Layer21 switches to full1; secondary31 uses camera0.
    const u32 clip = node.membership == anm_logic::Membership::Secondary && s.layer == 31 ? 1u
                     : priority < 43 || priority == 44                                    ? 2u
                                                                                          : 0u;
    animationScheduleState.push_back({id, priority, drawOrder, clip});
    if (node.customGeometry)
      continue;
    if (s.mode == 9 || s.mode == 13)
      continue;
    auto transform = resolveAnimationTransform(node);
    if (priority == 31 && (s.flags & 0x20000000u))
      if (const auto at = hostileDraw.find(node.owner); at != hostileDraw.end()) {
        // 40a1a2 computes the embedded VM heading at draw time. Opcode82
        // supplies this flag even when no ETEX rotation command was present.
        transform.rotation =
            anm_logic::normalized(float(double(at->second->enemy.angle) + 1.5707963705062866));
      }
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
    auto& output = animationDrawState.back();
    output.reserved[0] = asbits(s.spriteWidth);
    output.reserved[1] = asbits(s.spriteHeight);
    output.reserved[2] = 1;
  }
}

} // namespace th12
