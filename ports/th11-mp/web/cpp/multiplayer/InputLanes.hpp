#pragma once
#ifndef TH11_MULTIPLAYER
#error TH11 Multiplayer must remain outside the ordinary source set
#endif
#include <eagler/netplay/NetplayProtocol.hpp>
#include "../game/Multiplayer.hpp"
#include <array>
namespace th11::multiplayer {
class InputLanes {
public:
    // Native keyboard buttons occupy the low bits; Pause has its own wire bit
    // so Enter and Escape never become the same gameplay action.
    static constexpr std::uint16_t Pause=0x8000u;
    static constexpr std::uint16_t Restart=0x4000u;
    static bool Capture(std::uint32_t held,bool pause,int motion,float x,float y,Netplay::FrameInput&);
    static bool Decode(const Netplay::FrameInput*,unsigned,std::array<MultiplayerInput,3>&);
};
}
