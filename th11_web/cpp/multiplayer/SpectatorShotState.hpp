#pragma once
#ifndef TH11_MULTIPLAYER
#error TH11 spectator state belongs to the multiplayer source set
#endif
#include "../game/Types.hpp"
#include <vector>
namespace th11 {
class PlayerFrame;
namespace multiplayer {
struct SpectatorShotAccess {
    static bool Capture(const PlayerFrame&,std::vector<u8>&);
    static bool Apply(PlayerFrame&,const std::vector<u8>&);
    // Last title-owned codec rejection, used by the existing spectator diagnostics.
    static u32 Failure();
};
}}
