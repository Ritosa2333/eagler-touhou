#pragma once
#include "PracticeConfig.hpp"
namespace th08 {
class GameplayScene;struct GameplaySession;
bool apply_practice(GameplayScene&,GameplaySession&);
void update_practice(GameplayScene&,GameplaySession&);
// Upstream th08_name_fix pins the boss name for direct boss warps (stage 5
// boss, FinalB, Extra boss); the game's own stage-progress name update would
// otherwise restore the midboss name afterwards. Returns the front.anm enemy
// name row to display, or 0 when the warp does not pin one.
i32 practice_boss_name_override(u32 stage, i32 section);
}
