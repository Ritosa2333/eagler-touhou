#pragma once
#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#error Cooperative resources must not enter an ordinary build
#endif
#include "PlayerResources.hpp"
#include "../game/Arithmetic.hpp"

namespace th08::multiplayer {
inline i32 base_life_bombs(float native_bombs) noexcept {
    const i32 reduced=i32(native_bombs)-1;
    return reduced<1?1:reduced;
}
// Shared by a fresh session and the native team reset. Callers retain control
// of run/stage statistics; beginning a life only establishes its resources.
inline void begin_base_life(PilotResources& bank,float lives,float power,float native_bombs) noexcept {
    bank.lives=lives;bank.power=power;bank.bombs=float(base_life_bombs(native_bombs));
}
// The Netherworld team (3) and its solo loadouts Youmu (10) / Yuyuko (11)
// carry the original stage bonus; no other loadout does.
inline bool netherworld_stage_bonus(u8 character) noexcept {
    return character==3||character==10||character==11;
}
inline void begin_next_stage_life(PilotResources& bank,bool was_spirit,float native_bombs,bool ghost_refill=false) noexcept {
    if(was_spirit)bank.bombs=float(base_life_bombs(native_bombs));
    // The Netherworld team's stage bonus mirrors the native stage-results rule
    // per seat: entering a stage below three bombs grants exactly one.
    if(ghost_refill&&Scalar::truncate(bank.bombs)<3)bank.bombs+=1.f;
}
}
