#pragma once
#include "EclVm.hpp"
namespace th08 {
void cancel_enemy_async(EclVm&)noexcept;
bool retire_enemy_effects(EclVm&)noexcept;
// Original 0042bcf0 cleanup, including GUI ownership and player references.
bool retire_enemy(EclVm&,EclGlobals&,u16& replay_flags);
}
