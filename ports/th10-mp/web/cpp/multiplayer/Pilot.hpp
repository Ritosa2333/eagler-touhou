#pragma once
#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#error Multiplayer pilots must not enter an ordinary build.
#endif
#include "../game/GameEconomy.hpp"
#include "../game/Player.hpp"
#include "../game/Bomb.hpp"

namespace th10::multiplayer {
// A permanent seat binding. Native Player and Bomb callbacks carry their own
// object pointers; none of these fields are installed into a global current seat.
struct Pilot {
    GameEconomy& game;
    const u32 seat;
    Player* player=nullptr;
    Bomb* bomb=nullptr;
    PlayerProfile* cached_profile=nullptr;
    u32 input_keys=0;
    struct Presentation {Vec3 position{};i32 state=0;bool valid=false;} presentation;
    Pilot(GameEconomy& game,u32 seat):game(game),seat(seat){}
};
}
