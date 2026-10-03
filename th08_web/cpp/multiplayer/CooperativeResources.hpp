#pragma once
#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#error Cooperative resources must not enter an ordinary build
#endif
#include "PlayerResources.hpp"

namespace th08::multiplayer {
constexpr i32 base_life_bombs=1;
// Shared by a fresh session and the native team reset. Callers retain control
// of run/stage statistics; beginning a life only establishes its resources.
inline void begin_base_life(PilotResources& bank,float lives,float power) noexcept {
    bank.lives=lives;bank.power=power;bank.bombs=float(base_life_bombs);
}
inline void begin_next_stage_life(PilotResources& bank,bool was_spirit) noexcept {
    if(was_spirit)bank.bombs=float(base_life_bombs);
}
}
