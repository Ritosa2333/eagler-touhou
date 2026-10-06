// TH12 1.00b portable C++ runtime. See NOTICE.md for rights.
#include "GameState.hpp"

namespace th12 {
uint64_t stagePrimitiveKey(int slot) {
  return 0xc00000000ull + u32(slot);
}
uint64_t stageEmbeddedKey(int slot) {
  return 0xd00000000ull + u32(slot);
}
struct StageWorld : stage_logic::World {
  void unsupported(int opcode, const char*) override { ++th12::unsupported[982]; }
  void tickObjects() override {
    for (const auto& q : stageProgram.primitives)
      animationScene.tickEmbedded(stagePrimitiveKey(q.slot));
  }
  void bindEmbedded(int slot, int script) override {
    if (slot < 0 || slot >= 8) {
      ++th12::unsupported[982];
      return;
    }
    if (script < 0) {
      animationScene.remove(stageEmbeddedKey(slot));
      return;
    }
    animationScene.bind(stageEmbeddedKey(slot), 0xc0000000u + slot, 10, script,
                        anm_logic::Membership::Embedded, 0, 0, {}, false, nullptr, 0, 0);
  }
  void tickEmbedded(int slot) override { animationScene.tickEmbedded(stageEmbeddedKey(slot)); }
  void publish(const stage_logic::Camera& camera) override {
    animationScene.runtime.cameraPosition = camera.position;
    animationScene.runtime.cameraEyeOffset = camera.eyeOffset;
  }
} stageWorld;
void initializeStageAnimations() {
  stageState.reset(stageProgram);
  stageWorld.publish(stageState.camera);
  // 402a40 installs the stage view before STD can override it.
  stageState.camera.position = {0, 0, -600};
  stageState.camera.direction = {0, 300, 600};
  stageState.camera.up = {0, 1, 0};
  stageWorld.publish(stageState.camera);
  for (const auto& q : stageProgram.primitives)
    if (auto* node =
            animationScene.bind(stagePrimitiveKey(q.slot), 0xc1000000u + q.slot, 10, q.script,
                                anm_logic::Membership::Embedded, 0, 0, {}, false, nullptr, 0, 0, 0))
      node->customGeometry = true;
}

void tickStage() {
  if (stageState.program && !stageProgram.commands.empty()) {
    if (!stageState.tick(stageWorld))
      ++unsupported[982];
    backgroundFrame = stageState.callbackCount;
  } else
    ++backgroundFrame;
}

} // namespace th12
