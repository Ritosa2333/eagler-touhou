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
void recordBossCountdown(int limit, int phaseAge) {
  //41a7d3 writes HUD values during Enemy18's interrupt check, before the
  //phase clock tail. HUD25 reads those fields, including a timeout's0.00.
  const int ticks = limit - phaseAge;
  const int seconds = ticks / 60;
  bossHud.timerSeconds = std::min(seconds, 99);
  bossHud.timerHundredths = seconds > 99 ? 99 : (ticks % 60) * 100 / 60;
}
void tickBossHud() {
  const auto* boss = primaryBoss();
  const bool visible = boss && !dialogue;
  // HUD+4b50/+5004 are direct embedded ascii3/4 VMs, ticked by HUD25.
  for (int i = 0; i < 2; ++i) {
    const auto key = 0x7200000000000000ull + i;
    if (!animationScene.find(key) && animationScene.registry.bank(11)) {
      animationScene.bind(key, 0xfffe0100u + i, 11, 3 + i,
                          anm_logic::Membership::Embedded, 0, 0, {}, false, nullptr, 0, 0, 23);
      bossHud.timerDigits[i] = key;
    }
    animationScene.tickEmbedded(key);
  }
  bossHud.timerVisible = visible && bossHud.timerSeconds >= 0;
  if (bossHud.timerVisible && bossHud.timerSeconds != bossHud.previousTimerSeconds) {
    // Native41e2a8 only sounds on a decreasing second, below10.
    if (bossHud.timerSeconds < bossHud.previousTimerSeconds && bossHud.timerSeconds < 10)
      sound_system::queue.play(bossHud.timerSeconds < 5 ? 12 : 11, 0, false);
    const int irq = bossHud.timerSeconds < 5 ? 9 : bossHud.timerSeconds < 10 ? 8 : 7;
    for (int i = 0; i < 2; ++i) {
      animationScene.interrupt(bossHud.timerDigits[i], irq);
      if (auto* node = animationScene.find(bossHud.timerDigits[i])) {
        auto& state = node->vm.control();
        state.sprite = 243 + (i == 0 ? bossHud.timerSeconds / 10 : bossHud.timerSeconds % 10);
      }
    }
    bossHud.previousTimerSeconds = bossHud.timerSeconds;
  }
  for (const auto key : bossHud.timerDigits)
    animationScene.drawEnabled(key, bossHud.timerVisible);
  if (const auto* node = animationScene.find(bossHud.timerDigits[0]))
    bossHud.timerColor = node->vm.state().primary;
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
  out << "],\"timer\":{\"visible\":" << bossHud.timerVisible
      << ",\"seconds\":" << bossHud.timerSeconds << ",\"hundredths\":" << bossHud.timerHundredths
      << ",\"colorARGB\":" << bossHud.timerColor << "}}";
  bossHudSnapshotBuffer = out.str();
  return bossHudSnapshotBuffer;
}
} // namespace th12
