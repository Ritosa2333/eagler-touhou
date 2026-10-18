#pragma once
#include "Enemy.hpp"
namespace th10 {
// Shared fields owned by the HUD, item-value and spell-card systems.
struct EnemyPhaseState {
    const float* default_rate;
    i32* countdown;
    i32* item_value;
    u32* spell_flags;
    i32* spell_elapsed;
    i32* spell_bonus;
    u32* spell_animation_flags[7];
};
}
