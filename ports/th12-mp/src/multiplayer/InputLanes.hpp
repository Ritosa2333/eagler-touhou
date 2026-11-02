#pragma once
#include <eagler/netplay/NetplayProtocol.hpp>
#include "../game/Multiplayer.hpp"
namespace th12::multiplayer {
class InputLanes {
public:
  static constexpr uint16_t Pause = 0x8000;
  static constexpr uint16_t Confirm = 0x1000, Restart = 0x2000, HeldMask = 0x32ff;
  static bool Capture(uint32_t held, bool pause, int mode, float x, float y, Netplay::FrameInput&);
  static bool Decode(const Netplay::FrameInput*, unsigned count, std::array<Input, 3>&);
};
}
