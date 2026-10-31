// Native CameraEffect profiles1/8:4526e0/4527f0 updates at priority14.
#pragma once
#include "GameTypes.hpp"

namespace th12 {
struct CameraShakeEffect {
  int amplitude = 0, rise = 0, hold = 0, fall = 0, age = 0;
  float elapsed = 0;
  int profile = 8, endAmplitude = 0;
};
struct CameraEffects {
  std::vector<CameraShakeEffect> shakes;
  std::array<float, 2> offsets{}; // Original4cee04/4cee08, read by stage drawing.
  void shake(int amplitude, int rise, int hold, int fall);
  void pulse(int duration, int startAmplitude, int endAmplitude);
  void tick(Random&, float rate = 1, uint32_t applicationFlags = 0, bool stopping = false);
  void reset();
};
extern CameraEffects cameraEffects;
void tickCameraEffects();
void resetCameraEffects();
} // namespace th12
