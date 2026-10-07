#pragma once
namespace th12::eb {
// Native40c480's timer-only400 profile (Record.d==0). Unlike1000 it
// decrements before testing completion, and protects offscreen lifetime.
struct BulletGate {
  int remaining = 0;
  float value = 0;
  void begin(int duration) {
    remaining = duration;
    value = float(duration);
  }
  bool tick(float gameRate = 1) {
    value = float(double(value) - gameRate);
    remaining = int(value);
    return remaining <= 0;
  }
};
} // namespace th12::eb
