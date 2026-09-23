#pragma once
#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#error Multiplayer balance must not enter an ordinary build
#endif
#include "../game/Arithmetic.hpp"
namespace th10::multiplayer {
// TH07's generic cooperative damage balance. TH07-specific chained cards
// and Cherry formulas have no counterpart here.
inline i32 boss_damage(i32 damage,u32 players){
    const float factor=players>=3?2.0f/3.0f:players==2?.75f:1.f;
    return Scalar::truncate(Scalar::mul(Extended::from_int(damage).to_float(),factor));
}
inline i32 bomb_damage(i32 damage,u32 players){
    return players>=3?Scalar::truncate(Scalar::mul(Extended::from_int(damage).to_float(),2.0f/3.0f)):damage;
}
inline i32 rank_penalty(i32 value,u32 players){return players>1?value/i32(players):value;}
}
