#pragma once
#include "GameTypes.hpp"

namespace th12 {
// Global resources survive native scene12. The incoming main routine must
// observe them on its synchronous birth, before input0/Enemy18 can run.
struct SessionCarry {
  item_system::ResourceState resources{};
  ufo_system::Inventory inventory{};
  std::array<int, 4> nativeGlobals{};
  Random displayRandom{};
  int x = 0, y = 51200, graze = 0;
  uint16_t seed = 0;
  bool focused = false;
};

enum class FullGameStatus { Disabled, Playing, StageClear, NextStage, Ending, Result, GameOver };
struct FullGameFlow {
  FullGameStatus status = FullGameStatus::Disabled;
  int stage = 1, nextStage = 0, clearAge = 0, clearDelay = 120, continueCount = 0;
  int64_t totalFrames = 0;
  bool practice = false, replay = false;
  bool recordHeaderReady = false;
  SessionCarry recordHeader{}; // Immutable pre-main-birth recording boundary.
  SessionCarry carry{};
  int entryFadeAge = 30;
};
extern FullGameFlow fullGameFlow;
extern std::string stageFlowSnapshotBuffer;

SessionCarry captureSessionCarry();
void resetFullGameFlow();
int startFullGame(int character, int shot, int difficulty, int seed, bool practice);
void setFullGameMode(bool practice);
bool fullGamePractice();
// Returns false only when legacy single-stage/replay dispatch owns the tick.
bool tickFullGame(int held, int pressed);
int startNextStage();
int continueFullGame();
bool canContinueFullGame();
const std::string& captureStageFlow();
} // namespace th12
