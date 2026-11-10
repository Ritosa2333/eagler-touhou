#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace th12::practice::damage {
// Semantic replacement for TH12's native reimuADmgSrcs[5], not a native struct
// blob. Word positions are explicit and independent of C++ padding/layout.
constexpr uint32_t Magic=0x32315344u; // ASCII DS12 in little endian.
constexpr int Version=1, HeaderWords=6, SourceWords=18, Capacity=128;
enum class EntryResult:int {Invalid=-1,NotApplicable=0,Recorded=1,Restored=2,
  PreservedTransition=3,Unavailable=4};

// A fresh run/retry replaces the old generation. `practice` includes original
// and custom stage practice; the original compatibility fix is full-run only.
bool begin(bool replay,bool practice,int character,int shot);
void reset();
// StageFlow, before startGame resets Player/Manager, for a genuine adjacent
// stage transition. It preserves the actual current portable source slots.
bool armTransition(int from,int to);
// GameSession, after Player/Manager reset, immediately before main ECL birth.
EntryResult onBirth(int stage);
// Import one recorded stage snapshot atomically, before its first main birth.
bool load(const int32_t* words,int count);
// Immutable recording-side snapshot at each stage's pre-main birth boundary.
const std::vector<int32_t>& stageWords(int stage);
const std::string& snapshot();
int snapshotSize(); // Matches snapshot().c_str() until the next capture.
const std::string& lastError();
}
