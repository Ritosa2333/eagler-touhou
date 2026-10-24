#include "StageFlow.hpp"
#include "CameraEffects.hpp"
#include "EnemyCallbacks.hpp"
#include "FriendlyPool.hpp"
#include "GameSession.hpp"
#include "GameState.hpp"
#include "HostilePool.hpp"
#include "ItemPresentation.hpp"
#include "LaserWorld.hpp"
#include "Practice.hpp"
#include "PracticeDamageSources.hpp"

namespace th12 {
FullGameFlow fullGameFlow{};
std::string stageFlowSnapshotBuffer;

SessionCarry captureSessionCarry() {
  SessionCarry carry;
  carry.resources = resources;
  carry.inventory = ufoManager.inventory;
  carry.nativeGlobals = replayStageGlobals;
  carry.nativeGlobals[2] = playerFocused ? 1 : 0;
  carry.nativeGlobals[3] = graze;
  carry.displayRandom = displayRandom;
  carry.x = px;
  carry.y = py;
  carry.graze = graze;
  carry.seed = random.seed;
  carry.focused = playerFocused;
  return carry;
}

void resetFullGameFlow() { fullGameFlow = {}; }

void setFullGameMode(bool practice) {
  fullGameFlow = {};
  fullGameFlow.status = FullGameStatus::Playing;
  fullGameFlow.stage = stageNumber;
  fullGameFlow.practice = practice;
  fullGameFlow.replay = spellState.replayWallClock;
  fullGameFlow.continueCount = std::clamp(replayStageGlobals[0], 0, 9);
}

bool fullGamePractice() {
  return fullGameFlow.status != FullGameStatus::Disabled && fullGameFlow.practice;
}

int startFullGame(int c, int s, int d, int seed, bool practice) {
  if (c < 0 || c > 2 || s < 0 || s > 1 || d < 0 || d > 4 || seed < 0 || seed > 65535 ||
      (stageNumber == 7) != (d == 4))
    return 0;
  th12::practice::resetTracker();
  SessionCarry initial;
  initial.resources = item_system::ResourceState::initial(d);
  initial.seed = uint16_t(seed);
  initial.displayRandom = {0x1234, 0};
  if (practice) {
    // Native421e89..421ef0: practice has nine spare lives and stages2..6
    // start at2.00 power. A normal full run starts with the normal resources.
    initial.resources.lives = 9;
    if (stageNumber > 1 && stageNumber < 7)
      initial.resources.power = 200;
  }
  startGame(c, s, d, seed, nullptr, &initial);
  setFullGameMode(practice);
  fullGameFlow.recordHeader = initial;
  fullGameFlow.recordHeaderReady = true;
  return 1;
}

static void beginClear() {
  auto& flow = fullGameFlow;
  flow.status = FullGameStatus::StageClear;
  flow.clearAge = 0;
  // Native4227dc waits120 clear ticks before starting the next stage.
  // Native41e09a waits380 ticks before the main-game ending scene.
  flow.clearDelay = flow.stage == 6 && !flow.practice ? 380 : 120;
  flow.nextStage = !flow.practice && flow.stage < 6 ? flow.stage + 1 : 0;
  gameSpeed = animationScene.runtime.delta = 1;
  // MSG21 schedules scene12.4227dc waits for HUD's120-tick clear timer;
  // player16, enemy18, bullet21, item22 and HUD/MSG25 remain registered.
  // The outgoing managers are retired when the incoming stage is constructed.
}

bool tickFullGame(int held, int pressed) {
  auto& flow = fullGameFlow;
  if (flow.status == FullGameStatus::Disabled)
    return false;
  if (flow.status == FullGameStatus::Playing) {
    tick(held, pressed);
    flow.entryFadeAge = std::min(30, flow.entryFadeAge + 1);
    ++flow.totalFrames;
    if (ended == 1)
      beginClear();
    else if (ended == 2)
      flow.status = FullGameStatus::GameOver;
    return true;
  }
  if (flow.status != FullGameStatus::StageClear)
    return true;
  //4227dc first holds the clear reward for120 ticks.422839 then starts
  //4057e0's30-tick field fade;42296c creates the incoming stage afterwards.
  if (flow.nextStage && flow.clearAge == flow.clearDelay) {
    stageState.flags |= 4;
    for (const auto* node : animationScene.chain(anm_logic::Membership::Primary))
      if (node->vm.state().bank == 7 && node->vm.state().script == 85)
        animationScene.interruptNode(node->id, 1);
  }
  stageState.transitionAge = std::max(0, flow.clearAge - flow.clearDelay);
  tickGameplay(held, pressed);
  ++flow.clearAge;
  ++flow.totalFrames;
  if (flow.clearAge >= flow.clearDelay + (flow.nextStage ? 30 : 0)) {
    flow.carry = captureSessionCarry();
    if (flow.nextStage)
      flow.status = FullGameStatus::NextStage;
    else if (flow.stage == 6 && !flow.practice && !flow.replay)
      flow.status = FullGameStatus::Ending;
    else
      flow.status = FullGameStatus::Result;
  }
  return true;
}

int startNextStage() {
  auto& flow = fullGameFlow;
  if (flow.status != FullGameStatus::NextStage || !flow.nextStage || flow.replay ||
      stageNumber != flow.nextStage)
    return 0;
  const auto carry = flow.carry;
  const auto total = flow.totalFrames;
  if (!practice::damage::armTransition(flow.stage, stageNumber))
    return 0;
  startGame(character, shot, difficulty, carry.seed, nullptr, &carry);
  setFullGameMode(false);
  fullGameFlow.entryFadeAge = 0;
  fullGameFlow.totalFrames = total;
  fullGameFlow.recordHeader = carry;
  fullGameFlow.recordHeaderReady = true;
  return 1;
}

bool canContinueFullGame() {
  const auto& flow = fullGameFlow;
  return flow.status == FullGameStatus::GameOver && !flow.practice && !flow.replay &&
         flow.stage < 7 && stageNumber == flow.stage;
}

int continueFullGame() {
  if (!canContinueFullGame())
    return 0;
  const auto count = std::min(9, fullGameFlow.continueCount + 1);
  const auto total = fullGameFlow.totalFrames;
  SessionCarry initial;
  initial.resources = item_system::ResourceState::initial(difficulty);
  // Native4230a0 increments the low score digit;421dde..421f87 starts
  // a fresh instance of the same stage, with empty inventory/fragments,
  // two spare lives/bombs, fresh PIV and the continue rank penalty.
  initial.resources.rank = -512;
  if (stageNumber > 1 && stageNumber < 7)
    initial.resources.power = 400;
  initial.seed = random.seed;
  initial.displayRandom = displayRandom;
  initial.nativeGlobals[0] = count;
  startGame(character, shot, difficulty, initial.seed, nullptr, &initial);
  setFullGameMode(false);
  fullGameFlow.totalFrames = total;
  fullGameFlow.recordHeader = initial;
  fullGameFlow.recordHeaderReady = true;
  return 1;
}

const std::string& captureStageFlow() {
  static constexpr const char* names[] = {"disabled", "playing", "stage-clear", "next-stage",
                                          "ending", "result", "game-over"};
  const auto& f = fullGameFlow;
  std::ostringstream out;
  out << "{\"status\":\"" << names[int(f.status)] << "\",\"stage\":" << f.stage
      << ",\"nextStage\":" << f.nextStage << ",\"clearAge\":" << f.clearAge
      << ",\"clearDelay\":" << f.clearDelay << ",\"continueCount\":" << f.continueCount
      << ",\"totalFrames\":" << f.totalFrames << ",\"practice\":" << (f.practice ? "true" : "false")
      << ",\"replay\":" << (f.replay ? "true" : "false")
      << ",\"canContinue\":" << (canContinueFullGame() ? "true" : "false")
      << ",\"recordHeader\":";
  if (!f.recordHeaderReady) {
    out << "null";
  } else {
    const auto& h = f.recordHeader;
    const auto& r = h.resources;
    out << "{\"seed\":" << h.seed << ",\"x\":" << h.x << ",\"y\":" << h.y
        << ",\"power\":" << r.power << ",\"lives\":" << r.lives << ",\"bombs\":" << r.bombs
        << ",\"scoreStored\":" << r.scoreStored << ",\"pointValueStored\":" << r.pointValueStored
        << ",\"lifeFragments\":" << r.lifeFragments << ",\"bombFragments\":" << r.bombFragments
        << ",\"ufoColors\":[" << h.inventory.colors[0] << ',' << h.inventory.colors[1] << ','
        << h.inventory.colors[2] << "],\"rank\":" << r.rank << ",\"nativeGlobals\":["
        << h.nativeGlobals[0] << ',' << h.nativeGlobals[1] << ',' << h.nativeGlobals[2] << ','
        << h.nativeGlobals[3] << "]}";
  }
  out << '}';
  stageFlowSnapshotBuffer = out.str();
  return stageFlowSnapshotBuffer;
}
} // namespace th12
