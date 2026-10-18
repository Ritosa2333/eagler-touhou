#pragma once
#include <cstdint>
#include <string>

namespace th12::practice {
struct Parameters {
  int32_t mode=1, stage=0, section=0, phase=0;
  int64_t score=0;
  int32_t life=8, life_fragment=0, bomb=8, bomb_fragment=0, power=400,
          value=10000, graze=0, ufo_side=0, ventra_1=0, ventra_2=0, ventra_3=0;
  bool dlg=false;
};
enum Tool : uint32_t {Invulnerable=1, InfiniteLives=2, InfiniteBombs=4, InfinitePower=8,
  TimeLock=16, AutoBomb=32, EverlastingBgm=64, AllClearBonus=128};

void setEnabled(bool enabled);
bool enabled();
// Fresh title/Practice owner reset. Optional clearTools belongs to profile OFF.
void reset(bool clearTools=false);
// Call once at selectStage before loading a new stage's unmodified ECL files.
void stageSelected();
// Version 1 is exactly 20 int32 words. See src/practice/bridge.mjs.
bool configure(const int32_t* words, int count);
const Parameters& parameters();
int start(int character, int shot, int difficulty, int seed);
bool prepareReplay(int difficulty);
// GameSession: after resetting managers/resources, immediately before main birth.
void applyBirth();
void setReplay(bool replay);
bool replay();
bool active();
void setTools(uint32_t tools);
uint32_t tools();
bool tool(Tool tool);
bool assisted();
void markAssisted();
void resetTracker();
void recordMiss();
void recordBomb();
void recordUfoCollected(int nativeType);
void recordUfoDropped(int nativeType);
void recordUfoSummoned(int nativeType);
// Original TH12 auto-bomb replaces only the deathbomb key test at436d9b.
void beforeTick(int& held, int& pressed);
void afterTick();
void setSpeed(int fps, int replaySlow, int replayFast, bool debugAcceleration);
double interval(bool replay, bool fast, bool slow, bool debug);
bool bossMusic();
bool everlastingBgm();
const std::string& lastError();
const std::string& snapshot();
int snapshotSize(); // Size of the last published snapshot; never rebuild it.
}
