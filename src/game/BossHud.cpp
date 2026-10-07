#include "BossHud.hpp"
#include "GameState.hpp"
#include <iomanip>
#include <sstream>
namespace th12 {
BossHud bossHud;
bool bossMusic = false;
uint32_t musicSerial = 0;
std::string bossHudSnapshotBuffer;
void clearBossMarkers() {
  bossHud.markers = {};
  bossHud.colors = {};
}
void tickBossHud() {
  const auto* boss = primaryBoss();
  const bool visible = boss && !dialogue;
  if (visible && boss->maxLife > 0) {
    // Native41e463..41e4c7: fill rises by .025, but loss is immediate.
    bossHud.target = float(double(boss->life) / boss->maxLife);
    if (bossHud.fill < bossHud.target)
      bossHud.fill = float(double(bossHud.fill) + .02500000037252903);
    if (bossHud.fill > bossHud.target)
      bossHud.fill = bossHud.target;
    const bool moved =
        bossHud.shifted ? (py / 128.f >= 80 || px > 0) : (py / 128.f <= 64 && px / 128.f < -64);
    if (moved) {
      const int interrupt = bossHud.shifted ? 2 : 3;
      for (int i = 0; i < bossHud.remaining && i < 10; ++i)
        animationScene.interrupt(bossHud.stars[i], interrupt);
      animationScene.interrupt(bossHud.name, interrupt);
      bossHud.shifted = !bossHud.shifted;
    }
    if (!animationScene.find(bossHud.name)) {
      int script = -1;
      if (bossHud.checkpoint >= 24)
        script = 118 + stageNumber - 1;
      else if (stageNumber == 5)
        script = 118;
      else if (stageNumber == 7)
        script = 119;
      if (script >= 0)
        bossHud.name = bindScreenAnimation(7, script);
    }
    for (int i = 0; i < 10; ++i) {
      if (i < bossHud.remaining) {
        if (!animationScene.find(bossHud.stars[i]))
          bossHud.stars[i] = bindScreenAnimation(7, 53 + i);
      } else if (bossHud.stars[i]) {
        animationScene.interrupt(bossHud.stars[i], 1);
        bossHud.stars[i] = 0;
      }
    }
  } else {
    animationScene.interrupt(bossHud.name, 1);
    bossHud.name = 0;
    bossHud.fill = 0;
    bossHud.markers = {};
  }
}
const std::string& captureBossHud() {
  std::ostringstream out;
  out << std::setprecision(9) << "{\"fill\":" << bossHud.fill << ",\"target\":" << bossHud.target
      << ",\"remaining\":" << bossHud.remaining << ",\"checkpoint\":" << bossHud.checkpoint
      << ",\"markers\":[";
  for (int i = 0; i < 4; ++i) {
    if (i)
      out << ',';
    out << '[' << bossHud.markers[i] << ',' << bossHud.colors[i] << ']';
  }
  out << "]}";
  bossHudSnapshotBuffer = out.str();
  return bossHudSnapshotBuffer;
}
} // namespace th12
