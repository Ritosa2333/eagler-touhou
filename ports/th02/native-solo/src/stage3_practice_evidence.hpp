#pragma once
#include <array>
namespace soew {
// Static Azinn OP/MAIN evidence only; these entries are NOT enabled practice
// targets until the stage-specific initializer and scene are implemented.
struct Stage3PracticeEvidence {int id;const char *label;unsigned progress,measure;};
inline constexpr std::array<Stage3PracticeEvidence,8> stage3_practice_evidence={{{0x00,"CHAPTER 1",0,0},{0x03,"CHAPTER 2",53,17},{0x0f,"MIDBOSS",103,34},{0x33,"CHAPTER 3",104,34},{0x34,"CHAPTER 4",150,50},{0x35,"CHAPTER 5",182,60},{0x36,"CHAPTER 6",240,80},{0x37,"PRE BOSS",265,88}}};
// Boss Start, Inner Pair, Outer Pair, Boss Phase 1, 2, 3 respectively.
inline constexpr std::array<int,6> stage3_boss_entry_ids={0x10,0x17,0x18,0x27,0x48,0x49};
}
