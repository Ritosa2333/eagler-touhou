#pragma once
#include "Player.hpp"
#include "AnmManager.hpp"
namespace th10 {
struct PlayerMovementEnvironment {
    const u32* input_keys;
    const i32* enemy_count;
    const float* default_rate;
    const bool* always_hitbox;
    GameEconomy* economy;
    AnmManager* manager;
    AnmFile* effect_file;
    AnmEnvironment* animations;
    AnmAllocationEnvironment* allocation;
    virtual void update_option(PlayerOption& option)=0;
    virtual bool movement(const Player&,i32,i32&,i32&){return false;}
};
}
