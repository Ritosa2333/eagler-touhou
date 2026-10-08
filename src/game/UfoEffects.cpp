#include "UfoEffects.hpp"
#include <algorithm>
#include <cmath>

namespace th12 {
UfoEffects ufoEffects;
namespace {
constexpr uint64_t EffectKeyBase = 0x7900000000000000ull;
constexpr float Pi = 3.1415927410125732f, Tau = 6.2831854820251465f;
float normalize(float angle) {
  while (angle > Pi)
    angle = float(double(angle) - Tau);
  while (angle < -Pi)
    angle = float(double(angle) + Tau);
  return angle;
}
uint32_t random32(anm_logic::Scene& scene) {
  if (scene.runtime.gameRandom32)
    return scene.runtime.gameRandom32();
  if (scene.runtime.unsupported)
    scene.runtime.unsupported(0, 0, -2, "summon effect needs the game RNG provider");
  return 0;
}
float signedUnit(anm_logic::Scene& scene) {
  // 410309/4104bc rounds the signed unit value before multiplying by Pi.
  return float(double(random32(scene)) / 2147483648.0 - 1);
}
custom_geometry::Vertex vertex(float x, float y, uint32_t color) {
  return {x, y, 0, 1, color, 0, 0}; // Original FVF44 has no texture coordinates.
}
} // namespace

void UfoEffectTimer::advance(float delta) {
  previous = current;
  // Retail timer fast path uses one tick for a rate approximately equal to1.
  if (double(delta) >= .99 && delta <= 1.01f) {
    ++current;
    value = float(double(value) + 1);
  } else {
    value = float(double(value) + delta);
    current = int(value);
  }
}
void UfoEffects::reset() {
  states_.clear();
  serial_ = 1;
}
bool UfoEffects::owns(uint32_t id) const {
  return states_.contains(id);
}
const UfoEffectState* UfoEffects::state(uint32_t id) const {
  const auto at = states_.find(id);
  return at == states_.end() ? nullptr : at->second.get();
}
anm_logic::Node* UfoEffects::bind(anm_logic::Scene& scene,
                                std::shared_ptr<UfoEffectState> effect) {
  const auto serial = serial_++;
  // Native40fbe0 first binds script0 at layer23, then the initializer writes
  // payload/engine position and layer13, then installs its callbacks.
  auto* node = scene.bind(EffectKeyBase + serial, 0xee000000u + uint32_t(serial), 0, 0,
                          anm_logic::Membership::Primary, 0, 0, {}, false, nullptr, 0, 0, 23);
  if (!node)
    return nullptr;
  auto& pose = node->vm.control();
  pose.engineX = effect->position[0];
  pose.engineY = effect->position[1];
  pose.engineZ = effect->position[2];
  pose.layer = 13;
  node->customGeometry = true;
  effect->nodeId = node->id;
  states_[node->id] = effect;
  node->beforeTick = [this, &scene, effect] {
    const bool ended = tick(scene, *effect);
    if (ended)
      states_.erase(effect->nodeId);
    return ended;
  };
  return node;
}
void UfoEffects::summon(anm_logic::Scene& scene, float x, float y, float z) {
  auto effect = std::make_shared<UfoEffectState>();
  effect->kind = UfoEffectKind::Summon;
  effect->position = {float(double(x) + 224), float(double(y) + 16), z};
  bind(scene, std::move(effect));
}
void UfoEffects::defeat(anm_logic::Scene& scene, float x, float y, float z) {
  const std::array<float, 3> position{float(double(x) + 224), float(double(y) + 16), z};
  // 44b086..44b09a follows the bullet208 bind with ten actual effect1
  // births. Each initializer consumes RNG; their Primary27 updates retain
  // the ordinary trail motion, colors, lifetime and later RNG consumption.
  for (int i = 0; i < 10; ++i)
    trail(scene, position);
}
void UfoEffects::trail(anm_logic::Scene& scene, const std::array<float, 3>& position) {
  auto effect = std::make_shared<UfoEffectState>();
  effect->kind = UfoEffectKind::Trail;
  effect->position = position;
  effect->points[0] = {position[0], position[1]};
  effect->timer = {0, 1, 1};
  for (int i = 0; i < 16; ++i) {
    effect->colors[i] = 0xff0040ffu;
    if (i < 8)
      effect->colors[i] = (effect->colors[i] & ~0x00ff0000u) |
                          (uint32_t(255 - (i << 5)) << 16);
    else
      effect->colors[i] = (effect->colors[i] & ~0xff000000u) |
                          (uint32_t(255 - ((i - 8) << 4)) << 24);
  }
  // The factory performs ANM bind before initializer RNG. Script0 has no RNG.
  // Keep that ordering even if the resource's own bind later changes.
  if (auto* node = bind(scene, effect)) {
    (void)node;
    effect->heading = float(double(signedUnit(scene)) * Pi);
  }
}
bool UfoEffects::tick(anm_logic::Scene& scene, UfoEffectState& effect) {
  const float delta = scene.runtime.delta;
  if (effect.kind == UfoEffectKind::Summon) {
    if (effect.timer.current >= 50)
      return true;
    if (effect.timer.current != effect.timer.previous) {
      trail(scene, effect.position);
      trail(scene, effect.position);
    }
    effect.rimColor = (effect.rimColor & 0xffffffu) |
                       (uint32_t(uint8_t((effect.rimColor >> 24) + 3)) << 24);
    effect.radius = float(double(effect.radius) - 1.5);
    effect.timer.advance(delta);
    return false;
  }
  const int count = effect.timer.current;
  if (count >= 16)
    return true;
  if (count != effect.timer.previous) {
    for (int i = 0; i < count; ++i) {
      const int alpha = int(effect.colors[i] >> 24);
      effect.colors[i] = (effect.colors[i] & 0xffffffu) |
                         (uint32_t(std::max(0, alpha - 16)) << 24);
    }
    const float unit = float(double(random32(scene)) / 4294967296.0);
    const float length = float(double(unit) * 8 + 8);
    const float dx = float(std::cos(double(effect.heading)) * length);
    const float dy = float(std::sin(double(effect.heading)) * length);
    effect.points[count] = {float(double(effect.points[count - 1][0]) + dx),
                            float(double(effect.points[count - 1][1]) + dy)};
    const float randomAngle = float(double(signedUnit(scene)) * Pi);
    const float turn = float(double(randomAngle) / 10);
    effect.heading = normalize(float(double(effect.heading) + turn));
  }
  effect.timer.advance(delta);
  return false;
}
custom_geometry::Mesh UfoEffects::draw(const anm_logic::Node& node) const {
  custom_geometry::Mesh mesh;
  mesh.bank = mesh.sprite = -1; // Native SELECTARG2=DIFFUSE, not a sprite texture.
  mesh.layer = 13;
  mesh.drawPriority = 26;
  mesh.blend = int((node.vm.state().flags >> 5) & 7u);
  mesh.owner = node.owner;
  mesh.nodeId = node.id;
  const auto* effect = state(node.id);
  if (!effect || !node.alive)
    return mesh;
  if (effect->kind == UfoEffectKind::Trail) {
    mesh.primitive = custom_geometry::Primitive::LineStrip;
    for (int i = 0; i < std::clamp(effect->timer.current, 0, 16); ++i)
      mesh.vertices.push_back(vertex(effect->points[i][0], effect->points[i][1], effect->colors[i]));
    return mesh;
  }
  mesh.primitive = custom_geometry::Primitive::TriangleList;
  std::array<custom_geometry::Vertex, 33> outer;
  const float step = float(double(Tau) / 32);
  float angle = 0;
  for (auto& point : outer) {
    const float dx = float(std::cos(double(angle)) * effect->radius);
    const float dy = float(std::sin(double(angle)) * effect->radius);
    point = vertex(float(double(dx) + effect->position[0]),
                   float(double(dy) + effect->position[1]), effect->rimColor);
    angle = normalize(float(double(angle) + step));
  }
  const auto center = vertex(effect->position[0], effect->position[1], effect->centerColor);
  for (int i = 0; i < 32; ++i) {
    mesh.vertices.push_back(center);
    mesh.vertices.push_back(outer[i]);
    mesh.vertices.push_back(outer[i + 1]);
  }
  return mesh;
}
} // namespace th12
