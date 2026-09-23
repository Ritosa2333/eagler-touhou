#pragma once
#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#error Multiplayer balance must not enter an ordinary build
#endif
#include "../game/GameMath.hpp"
namespace th08::multiplayer {
// TH07's generic cooperative scaling, applied after each title's native
// damage and spell rules. TH08's Last Spell/time-orb math remains native.
inline i32 boss_damage(i32 damage,u32 count){
    const float factor=count>=3?2.f/3.f:count==2?.75f:1.f;
    return Scalar::truncate(Scalar::mul(Extended::from_int(damage).to_float(),factor));
}
inline i32 bomb_damage(i32 damage,u32 count){
    return count>=3?Scalar::truncate(Scalar::mul(Extended::from_int(damage).to_float(),2.f/3.f)):damage;
}
inline i32 rank_penalty(i32 amount,u32 count){return count>1?amount/i32(count):amount;}
}
