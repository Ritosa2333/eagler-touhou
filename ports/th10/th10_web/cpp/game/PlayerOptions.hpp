#pragma once
#include "Player.hpp"
#include "AnmManager.hpp"
namespace th10 {
struct PlayerOptionsEnvironment {
    GameEconomy* economy;
    AnmManager* manager;
    AnmEnvironment* animations;
    AnmAllocationEnvironment* allocation;
    Player* global_player;
    u32 trailing_callback,anchored_callback;
};
}
