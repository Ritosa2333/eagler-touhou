#include "CameraEffects.hpp"
#include "GameState.hpp"

namespace th12 {
CameraEffects cameraEffects;
void CameraEffects::shake(int amplitude, int rise, int hold, int fall) {
  // 462380 inserts a new registration before equal priority registrations.
  shakes.insert(shakes.begin(), {amplitude, rise, hold, fall});
}
void CameraEffects::pulse(int duration, int startAmplitude, int endAmplitude) {
  CameraShakeEffect effect;
  effect.profile = 1;
  effect.amplitude = startAmplitude;
  effect.rise = duration;
  effect.endAmplitude = endAmplitude;
  shakes.insert(shakes.begin(), effect);
}
void CameraEffects::tick(Random& rng, float rate, uint32_t flags, bool stopping) {
  if (stopping) {
    shakes.clear();
    return;
  }
  for (auto at = shakes.begin(); at != shakes.end();) {
    auto& effect = *at;
    // 4526e0 profile1 has no application pause gate;4527f0 profile8 does.
    if (effect.profile == 8 && (((flags | (flags >> 2)) & 1) || (flags & 0x72))) {
      ++at;
      continue;
    }
    // Actual464a80 snaps rates strictly inside(.99,1.01) to one timer tick.
    const bool wholeTick = double(rate) > .9900000095367432 && rate < 1.0099999904632568f;
    effect.elapsed = float(double(effect.elapsed) + (wholeTick ? 1.f : rate));
    effect.age = wholeTick ? effect.age + 1 : anm_logic::nativeInt(effect.elapsed);
    const int total = effect.profile == 1 ? effect.rise : effect.rise + effect.hold + effect.fall;
    if (effect.age >= total) {
      // Return7 invokes452960/452f00. It unregisters the object without
      // changing offsets or drawing new random values.
      at = shakes.erase(at);
      continue;
    }
    float amplitude;
    if (effect.profile == 1) {
      // 45271f..45274d rounds product and quotient independently before adding
      //  the starting integer amplitude. Native SUB wraps its signed operand.
      const i32 delta = i32(u32(effect.endAmplitude) - u32(effect.amplitude));
      const float scaled = float(double(delta) * effect.elapsed);
      const float normalized = float(double(scaled) / effect.rise);
      amplitude = float(double(normalized) + effect.amplitude);
    } else {
      float envelope;
      if (effect.age < effect.rise)
        envelope = float(double(effect.elapsed) / effect.rise);
      else if (effect.age < effect.rise + effect.hold)
        envelope = 1;
      else {
        const float remaining = float(double(total) - effect.elapsed);
        envelope = float(double(remaining) / effect.fall);
      }
      amplitude = float(double(effect.amplitude) * envelope);
    }
    for (float& axis : offsets) {
      const auto choice = rng.next32() % 3;
      axis = choice == 0 ? 0.f : choice == 1 ? amplitude : -amplitude;
    }
    ++at;
  }
}
void CameraEffects::reset() {
  shakes.clear();
  offsets = {};
}
void tickCameraEffects() {
  // Draw42's460e40..460e62 clears these globals after the preceding frame's
  // stage/gameplay draws. Keep that frame seam even when no GPU draws occur.
  cameraEffects.offsets = {};
  cameraEffects.tick(random);
}
void resetCameraEffects() {
  cameraEffects.reset();
}
} // namespace th12
