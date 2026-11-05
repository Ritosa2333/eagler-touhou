#pragma once
#include "Player.hpp"
#include "AnmManager.hpp"
namespace th10 {
struct PlayerShootingEnvironment {
    const u32* input_keys;
    const float* default_rate;
    GameEconomy* economy;
    AnmManager* manager;
    AnmEnvironment* animations;
    AnmAllocationEnvironment* allocation;
    bool dialogue_active,enemy_manager_present;
    virtual void initialize_shot(Player& player,PlayerShot& shot,i32 frame)=0;
    virtual void update_shot(Player& player,PlayerShot& shot)=0;
    virtual void play_shot_sound(i32 sound,float horizontal_position)=0;
};
bool outside_playfield(const Vec3& position,float half_width,float half_height) noexcept;
}
