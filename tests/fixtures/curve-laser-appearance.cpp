#include "../../src/game/GameState.hpp"
#include "../../src/game/GameSession.hpp"
#include "../../src/game/LaserWorld.hpp"
#include "../../src/game/PresentationGeometry.hpp"
using namespace th12;
extern "C" {
void curve_appearance_setup(int color) {
  startGame(0, 0, 3, 4660);
  // Match the declared isolated native host presentation seed immediately
  // before this factory. This is fixture setup, never a replay substitution.
  displayRandom = {0xbeef, 0};
  laser::Parameters parameters;
  parameters.type = 0;
  parameters.color = color;
  parameters.width = 16;
  parameters.speed = 3;
  parameters.nodeCount = 42;
  parameters.flags = 1;
  parameters.sound = 22;
  parameters.transformSound = 40;
  laserManager.spawn(laser::Kind::Curve, parameters, laserWorld);
  tickLasers(); // Same single original Laser20 visit before the captured draw.
}
const uint32_t* curve_appearance_state() {
  static uint32_t values[16];
  const auto objects = laserManager.ordered();
  const auto* node = animationScene.find(laserAnimationKey(objects.front()->state.serialId,
                                                          laser::Role::Main));
  const auto& state = node->vm.state();
  captureCustomGeometry();
  const auto mesh = std::find_if(meshDrawState.begin(), meshDrawState.end(),
                               [](const auto& value) { return value.pass == 1 && value.primitive == 5; });
  values[0] = state.script;
  values[1] = state.sprite;
  values[2] = state.layer;
  values[3] = state.blend;
  values[4] = mesh->sprite;
  values[5] = mesh->textureEntry;
  values[6] = mesh->vertexOffset;
  values[7] = mesh->vertexCount;
  values[8] = state.mode;
  values[9] = state.anchorX;
  values[10] = state.anchorY;
  const auto* cap = animationScene.find(laserAnimationKey(objects.front()->state.serialId,
                                                         laser::Role::Cap));
  values[11] = cap->vm.state().blend;
  values[12] = cap->vm.state().mode;
  values[13] = cap->vm.state().anchorY;
  const auto capMesh = std::find_if(meshDrawState.begin(), meshDrawState.end(),
                                  [](const auto& value) { return value.pass == 1 && value.primitive == 4; });
  values[14] = capMesh->vertexOffset;
  values[15] = capMesh->vertexCount;
  return values;
}
const void* curve_appearance_vertices() { return meshVertexState.data(); }
}
