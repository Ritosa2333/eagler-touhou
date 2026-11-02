#include "InputLanes.hpp"
#include <cmath>
namespace th12::multiplayer {
bool InputLanes::Capture(uint32_t held, bool pause, int mode, float x, float y, Netplay::FrameInput& out) {
  if ((held & ~uint32_t(HeldMask)) || mode < 0 || mode > 3 || !std::isfinite(x) || !std::isfinite(y)) return false;
  Netplay::FrameInput next(uint16_t(held) | (pause ? Pause : 0));
  if (mode == 3) {
    if (std::abs(x) > 1 || std::abs(y) > 1) return false;
    next.analogMode = Netplay::AnalogMode::Joystick; next.x = x; next.y = y;
  } else if (mode) {
    if (x < -184 || x > 184 || y < 32 || y > 432) return false;
    next.analogMode = Netplay::AnalogMode::DirectTouch; next.x = x; next.y = y; next.unlimited = mode == 2;
  }
  next.touchUsed = mode != 0;
  if (!Netplay::IsValidFrameInput(next)) return false;
  out = next; return true;
}
bool InputLanes::Decode(const Netplay::FrameInput* frames, unsigned count, std::array<Input, 3>& out) {
  if (!frames || count < 2 || count > 3) return false;
  std::array<Input, 3> next{};
  for (unsigned seat = 0; seat < count; ++seat) {
    const auto& f = frames[seat]; auto& p = next[seat];
    if (!Netplay::IsValidFrameInput(f) || f.touchBomb || (f.buttons & ~(HeldMask | Pause))) return false;
    p.held = f.buttons & HeldMask; p.pause = (f.buttons & Pause) != 0;
    if (f.analogMode == Netplay::AnalogMode::Joystick) {
      p.motion = 3; p.x = int(std::lround(f.x * 32767)); p.y = int(std::lround(f.y * 32767));
    } else if (f.analogMode == Netplay::AnalogMode::DirectTouch) {
      if (f.x < -184 || f.x > 184 || f.y < 32 || f.y > 432) return false;
      p.motion = f.unlimited ? 2 : 1; p.x = int(std::lround(f.x * 128)); p.y = int(std::lround(f.y * 128));
    } else if (f.analogMode != Netplay::AnalogMode::None) return false;
  }
  out = next; return true;
}
}
