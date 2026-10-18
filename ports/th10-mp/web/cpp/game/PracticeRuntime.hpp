#pragma once
#include "PracticeConfig.hpp"
namespace th10 {
namespace browser { struct World; struct GameState; }
// One-shot practice setup. Mirrors thprac_th10.cpp th10_patch_main.
bool apply_practice(browser::World&,browser::GameState&);
// Per-frame cheats. Mirrors thprac_th10.cpp's hook updates.
void update_practice(browser::World&,browser::GameState&);
// Guard predicates for the session's hard constraints. Locks prevent a
// decrement and never rewrite values, so Phase D consumers check these at the
// original decrement/hit-test sites instead of this module editing arrays.
bool practice_invincible(const PracticeState&);      // F1: skip the player hit test
bool practice_infinite_lives(const PracticeState&);  // F2: block life loss
bool practice_infinite_power(const PracticeState&);  // F3: block power loss
bool practice_time_lock(const PracticeState&);       // F4: block timeout decrements
bool practice_auto_bomb(const PracticeState&);       // F5: inject the bomb bit
bool practice_no_faith_loss(const PracticeState&);   // F6: block faith decay
// thprac_th10.cpp:2314 th10_logo. True when a custom practice start at a
// section suppresses the stage-entry logo/title card. Stage-warp portion 1 is a
// normal stage start and keeps the title.
bool practice_skip_stage_logo(const PracticeState&);
// thprac_th10.cpp:1752 THBGMTest / :2308 th10_bgm. True when the selected
// custom practice section plays the boss theme instead of the stage theme.
bool practice_boss_bgm(const PracticeState&);
}
