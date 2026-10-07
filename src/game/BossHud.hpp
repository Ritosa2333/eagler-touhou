#pragma once
#include <array>
#include <cstdint>
#include <string>
namespace th12 {
struct BossHud {
  float fill = 0, target = 0;
  int remaining = 0, checkpoint = 0;
  bool shifted = false;
  std::array<float, 4> markers{};
  std::array<uint32_t, 4> colors{};
  uint64_t name = 0;
  std::array<uint64_t, 10> stars{};
};
extern BossHud bossHud;
extern bool bossMusic;
extern uint32_t musicSerial;
extern std::string bossHudSnapshotBuffer;
void tickBossHud();
void clearBossMarkers();
const std::string& captureBossHud();
} // namespace th12
