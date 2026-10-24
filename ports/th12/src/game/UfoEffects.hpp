// TH12 retail effect profiles2/1: summon pulse and its growing colored trails.
#pragma once
#include "AnmScene.hpp"
#include "CustomGeometry.hpp"

namespace th12 {
enum class UfoEffectKind : uint32_t { Trail = 1, Summon = 2 };
struct UfoEffectTimer {
  int previous = -1, current = 0;
  float value = 0;
  void advance(float delta);
};
// Native VM+478 points to a2c-byte summon payload or d8-byte trail payload.
// These are semantic values; no original pointers/layout are exposed here.
struct UfoEffectState {
  uint32_t nodeId = 0;
  UfoEffectKind kind = UfoEffectKind::Summon;
  std::array<float, 3> position{};
  UfoEffectTimer timer;
  float radius = 128, heading = 0;
  uint32_t centerColor = 0x40ff0000u, rimColor = 0x00ff00ffu;
  std::array<std::array<float, 2>, 16> points{};
  std::array<uint32_t, 16> colors{};
};
class UfoEffects {
  uint64_t serial_ = 1;
  std::unordered_map<uint32_t, std::shared_ptr<UfoEffectState>> states_;
  anm_logic::Node* bind(anm_logic::Scene&, std::shared_ptr<UfoEffectState>);
  void trail(anm_logic::Scene&, const std::array<float, 3>& screenPosition);
  bool tick(anm_logic::Scene&, UfoEffectState&);

public:
  // Reset after the owning Scene is reset, so no old callbacks remain alive.
  void reset();
  void summon(anm_logic::Scene&, float worldX, float worldY, float worldZ = 0);
  void defeat(anm_logic::Scene&, float worldX, float worldY, float worldZ = 0);
  bool owns(uint32_t nodeId) const;
  const UfoEffectState* state(uint32_t nodeId) const;
  custom_geometry::Mesh draw(const anm_logic::Node&) const;
};
extern UfoEffects ufoEffects;
} // namespace th12
