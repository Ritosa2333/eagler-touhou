#include "PresentationGeometry.hpp"
#include "EnemyCallbacks.hpp"
#include "GameState.hpp"
#include "LaserWorld.hpp"
#include "TextGlyphs.hpp"
#include <charconv>

namespace th12 {
std::vector<MeshRecord> meshDrawState;
std::vector<custom_geometry::Vertex> meshVertexState;
namespace {
bool region(const anm_logic::Node& node, custom_geometry::SpriteRegion& output) {
  const auto& state = node.vm.state();
  const auto bank = animationScene.registry.bank(state.spriteBank);
  if (!bank || state.sprite < 0 || state.sprite >= int(bank->sprites.size()))
    return false;
  output = custom_geometry::spriteRegion(state, bank->sprites[state.sprite]);
  return true;
}
void append(custom_geometry::Mesh mesh, const anm_logic::Node& node) {
  if (mesh.vertices.empty())
    return;
  meshDrawState.push_back({mesh.bank, mesh.sprite, mesh.textureEntry, mesh.layer, mesh.blend,
                           uint32_t(mesh.primitive), uint32_t(mesh.pass), mesh.drawPriority,
                           node.owner, node.id, uint32_t(meshVertexState.size()),
                           uint32_t(mesh.vertices.size()), uint32_t(mesh.filter),
                           uint32_t(mesh.wrapU) | (uint32_t(mesh.wrapV) << 1)});
  meshVertexState.insert(meshVertexState.end(), mesh.vertices.begin(), mesh.vertices.end());
}
} // namespace
void captureCustomGeometry() {
  meshDrawState.clear();
  meshVertexState.clear();
  std::vector<u32> ids;
  for (const auto& [id, node] : animationScene.nodes())
    if (node->alive && !node->customGeometry && node->vm.state().mode == 13)
      ids.push_back(id);
  std::sort(ids.begin(), ids.end());
  for (const auto id : ids) {
    const auto& node = *animationScene.nodes().at(id);
    custom_geometry::SpriteRegion sprite;
    if (!region(node, sprite))
      continue;
    const auto p = resolveAnimationTransform(node);
    append(custom_geometry::ufoFillRing(
               node.vm.state(), sprite,
               {float(double(p.x) + node.viewportX), float(double(p.y) + node.viewportY), p.z}),
           node);
  }
  for (const auto* object : laserManager.ordered()) {
    const auto& state = object->state;
    if (const auto* main =
            animationScene.find(laserAnimationKey(state.serialId, laser::Role::Main))) {
      custom_geometry::SpriteRegion sprite;
      if (region(*main, sprite))
        append(state.kind == laser::Kind::Curve
                   ? custom_geometry::curveLaser(state, main->vm.state(), sprite)
                   : custom_geometry::straightLaser(state, main->vm.state(), sprite),
               *main);
    }
    const bool capVisible = state.kind == laser::Kind::Curve
                                ? custom_geometry::curveCapVisible(state)
                                : custom_geometry::straightCapVisible(state);
    if (capVisible)
      if (const auto* cap =
              animationScene.find(laserAnimationKey(state.serialId, laser::Role::Cap))) {
        custom_geometry::SpriteRegion sprite;
        if (!region(*cap, sprite))
          continue;
        const auto p = resolveAnimationTransform(*cap);
        auto mesh = custom_geometry::flatQuad(
            cap->vm.state(), sprite,
            {float(double(p.x) + cap->viewportX), float(double(p.y) + cap->viewportY), p.z},
            p.rotation, p.sx, p.sy);
        mesh.pass = custom_geometry::DrawPass::LaserManager;
        mesh.drawPriority = 29;
        append(std::move(mesh), *cap);
      }
  }
  if (const auto bank = animationScene.registry.bank(11)) {
    u32 order = 0;
    for (const auto& popup : scorePopups) {
      char buffer[16];
      const auto number = std::to_chars(buffer, buffer + sizeof(buffer), popup.displayedValue);
      std::vector<ascii_text::Glyph> glyphs;
      if (!ascii_text::font5({buffer, size_t(number.ptr - buffer)}, popup.screenPosition.x,
                             popup.screenPosition.y, popup.screenPosition.z, popup.scaleX,
                             popup.scaleY, popup.horizontalAlignment, popup.verticalAlignment,
                             popup.color, glyphs))
        continue;
      for (const auto& glyph : glyphs) {
        if (glyph.sprite < 0 || glyph.sprite >= int(bank->sprites.size()))
          continue;
        anm_logic::State state;
        state.flags = 3;
        state.flags2 = 2;
        state.spriteBank = 11;
        state.sprite = glyph.sprite;
        state.primary = glyph.argb;
        state.sx = glyph.sx;
        state.sy = glyph.sy;
        state.anchorX = state.anchorY = 1;
        auto mesh = custom_geometry::flatQuad(
            state, custom_geometry::spriteRegion(state, bank->sprites[glyph.sprite]),
            {glyph.x, glyph.y, glyph.z}, 0, glyph.sx, glyph.sy);
        mesh.pass = custom_geometry::DrawPass::AsciiOverlay;
        mesh.drawPriority = 45;
        anm_logic::Node descriptor;
        descriptor.id = order++;
        append(std::move(mesh), descriptor);
      }
    }
  }
}
} // namespace th12
