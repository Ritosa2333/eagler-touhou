// TH12 1.00b ANM v7 logical interpreter. Native entry points: reset 0x402520,
// bind 0x454d10, tick 0x455630, variable readers 0x4551b0/0x4553f0.
// This module owns no rendering or RNG seed. Birth-time binding executes one
// tick synchronously. The caller owns scheduling, including newly registered
// children (primary tick 27, secondary tick 8); a draw/snapshot must
// never call tick(). The value 0x17 passed to 0x4615a0 is a draw layer.
#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <functional>
#include <limits>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace th12::anm_logic {
constexpr float Pi = 3.1415927410125732421875f, Tau = 6.283185482025146484375f;
inline float asFloat(uint32_t v) {
  float r;
  std::memcpy(&r, &v, 4);
  return r;
}
inline uint32_t asWord(float v) {
  uint32_t r;
  std::memcpy(&r, &v, 4);
  return r;
}
inline int32_t nativeInt(double v) {
  return std::isfinite(v) && v >= -2147483648. && v < 2147483648. ? int32_t(v) : INT32_MIN;
}
inline float normalized(float v) {
  if (!std::isfinite(v))
    return v;
  while (v > Pi)
    v -= Tau;
  while (v < -Pi)
    v += Tau;
  return v;
}
inline float curve(float t, int mode) {
  t = std::clamp(t, 0.f, 1.f);
  if (mode >= 1 && mode <= 3)
    return float(std::pow(double(t), mode + 1));
  if (mode >= 4 && mode <= 6)
    return float(1. - std::pow(1. - double(t), mode - 2));
  if (mode >= 9 && mode <= 11) {
    int p = mode - 7;
    return float(t < .5f ? std::pow(2. * t, p) / 2. : 1. - std::pow(2. - 2. * t, p) / 2.);
  }
  if (mode >= 12 && mode <= 14) {
    int p = mode - 10;
    return float(t < .5f ? (1. - std::pow(1. - 2. * t, p)) / 2.
                         : (std::pow(2. * t - 1., p) + 1.) / 2.);
  }
  return mode == 15 ? 0.f : mode == 16 ? 1.f : t;
}
struct Command {
  int16_t op = 0, time = 0;
  uint16_t mask = 0;
  uint32_t offset = 0;
  // Unmasked destinations really point into the shared native script. Keep
  // mutable words in the registry so literal-destination writes are shared.
  std::vector<uint32_t> words;
};
struct Script {
  int sourceId = 0;
  std::vector<Command> commands;
  std::unordered_map<uint32_t, size_t> offsets;
};
struct SpriteInfo {
  float width = 0, height = 0;
};
struct Bank {
  std::vector<Script> scripts;
  std::vector<SpriteInfo> sprites;
};

struct State {
  std::array<int32_t, 6> integers{}; // +3fc..408, +41c/+420
  std::array<float, 4> floats{};     // +40c..418 (int0/float0 are independent)
  float x = 0, y = 0, z = 0, rx = 0, ry = 0, rotation = 0, sx = 1, sy = 1;
  float engineX = 0, engineY = 0, engineZ = 0, u = 0, v = 0, uvScaleX = 1, uvScaleY = 1;
  // Native +43c/+440/+444: an additional script offset, independently added
  // to +424/+428/+42c and engine +430/+434/+438 by the draw helpers.
  float offsetX = 0, offsetY = 0, offsetZ = 0;
  float geometryRadius = 0, geometryWidth = 0;
  int geometryCount = 0;
  uint32_t primary = 0xffffffffu, secondary = 0, flags = 6, flags2 = 0;
  int sprite = -1, spriteBank = -1, layer = 0, mode = 0, anchorX = 0, anchorY = 0, blend = 0;
  int bank = -1, script = -1, clock = 0, ticks = 0;
  bool visible = false, held = false, ended = true, deleteRequested = false, displayDomain = false;
};
struct ChildRequest {
  int bank = 0, script = 0, opcode = 0, registrationFlags = 0;
  bool detached = false;
  float x = 0, y = 0, z = 0;
  bool hasOffset = false;
  float offsetX = 0, offsetY = 0;
  const State* parent = nullptr;
  // The scheduler fills this during synchronous construction, allowing
  // op96/97 to resolve their extra operands AFTER the child's birth tick.
  mutable State* createdState = nullptr;
};
struct Runtime {
  std::function<uint32_t()> gameRandom32, displayRandom32;
  // Must bind the requested VM before returning: native child creation calls
  // 0x454d10 synchronously. Subsequent scheduler membership is caller-owned.
  std::function<void(const ChildRequest&)> spawnChild;
  std::function<void(int bank, int script, int opcode, const char* reason)> unsupported;
  std::function<int(int sprite)> remapSprite; // hostile bullet color callback (+4ac)
  // Native negative sprite resolves through FontManager+18fb4 to ascii264.
  // Bank numbers are host resource identities; the default matches TH12's
  // explicit loader mapping, not the script's bank.
  int whiteSpriteBank = 11, whiteSprite = 264;
  std::array<float, 3> cameraPosition{}, cameraEyeOffset{};
  float delta = 1;
  size_t instructionBudget = 4096;
};
struct DiagnosticInterpolation {
  bool active = false;
  int mode = 0;
  float time = 0, duration = 0;
  std::vector<int> fields;
  std::vector<float> from, to, a, b;
};
struct Diagnostic {
  size_t pc = 0, savedPC = 0;
  uint32_t byteOffset = 0, savedByteOffset = 0;
  float time = 0, savedTimer = 0;
  int pendingInterrupt = 0;
  bool hasSavedTimer = false;
  std::array<float, 3> rotationVelocity{};
  std::array<float, 2> scaleVelocity{}, scrollVelocity{};
  std::vector<DiagnosticInterpolation> interpolations;
};
} // namespace th12::anm_logic
