#include "../../src/game/GameSession.hpp"
#include "../../src/game/GameState.hpp"
#include "../../src/game/CameraEffects.hpp"
using namespace th12;
namespace {
float values[17];
uint64_t mainKey = 0;
}
extern "C" {
void bomb_prepare(int c, int s) {
  startGame(c, s, 1, 42);
  px = 20 * 128;
  py = 300 * 128;
  th12::random = {42, 0};
  mainKey = 0;
}
void bomb_phase(int start) {
  tickCameraEffects();
  // Production Player16 synchronization precedes Bomb17. It must not revive
  // a primary VM retired by the preceding ANM27 pass.
  syncBombAnimations();
  if (start) {
    startPlayerBomb();
    mainKey = playerBomb.primaryVisuals.empty() ? 0 :
              0x700000000ull + playerBomb.primaryVisuals.front().id;
    if (playerBomb.loadout == 0)
      mainKey = 0x600000000ull + playerBomb.reimuAEmitter.id;
    else if (playerBomb.loadout == 3)
      mainKey = 0x700000000ull + playerBomb.marisaB.id;
  }
  tickBomb();
  animationScene.tickPrimary();
  animationScene.collect();
  buildPlayerBombTrails();
  ++frame;
}
const float* bomb_snapshot() {
  const auto* main = animationScene.find(mainKey);
  const auto* backdrop = animationScene.find(playerBomb.backgroundAnimation);
  values[0] = playerBomb.active;
  values[1] = float(playerBomb.age);
  values[2] = main != nullptr;
  values[3] = main ? float(main->vm.state().script) : -1;
  values[4] = main ? float(main->vm.state().clock) : -1;
  values[5] = backdrop != nullptr;
  values[6] = backdrop ? float(backdrop->vm.state().script) : -1;
  values[7] = backdrop ? float(backdrop->vm.state().clock) : -1;
  values[8] = float(th12::random.seed);
  values[9] = float(th12::random.calls);
  values[10] = float(cameraEffects.shakes.size());
  values[11] = cameraEffects.offsets[0];
  values[12] = cameraEffects.offsets[1];
  values[13] = main ? float(main->vm.state().primary >> 24) : -1;
  values[14] = main ? float(main->vm.state().secondary >> 24) : -1;
  values[15] = backdrop ? float(backdrop->vm.state().primary >> 24) : -1;
  values[16] = backdrop ? float(backdrop->vm.state().secondary >> 24) : -1;
  return values;
}
void bomb_remove_main() { animationScene.remove(mainKey); }
}
