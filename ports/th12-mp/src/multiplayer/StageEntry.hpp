#pragma once
#include "../game/StageFlow.hpp"
#include "../game/Multiplayer.hpp"
#include <optional>
namespace th12::multiplayer {
struct StageEntry {
  uint32_t version = 2, stage = 1, seed = 1, displaySeed = 0x1234, displayCalls = 0;
  uint32_t ticks = 0, lifePieces = 0, bombPieces = 0;
  int score = 0, rank = 0;
  bool continuation = false;
  std::array<int,4> nativeGlobals{};
  std::array<item_system::ResourceState, 3> resources{};
  std::array<int, 3> graze{}, misses{};
  std::array<int, 3> x{},y{{51200,51200,51200}},focused{};
  ufo_system::Inventory inventory{};
};
extern std::optional<StageEntry> pendingStageEntry;
extern StageEntry stageEntry;
void recordStageEntry(int seed, const SessionCarry*);
std::vector<uint8_t> encodeStageEntry();
bool decodeStageEntry(const std::vector<uint8_t>&, StageEntry&);
bool restoreStageEntry(const StageEntry&);
}
