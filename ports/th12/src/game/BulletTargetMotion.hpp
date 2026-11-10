// Native 40b269/40c230 extension2000000. This is a movement callback;
// the caller owns ordinary position integration and animation scheduling.
#pragma once
namespace th12::eb {
struct State;
struct Record;
struct World;
struct TargetMotion {
  float fromX = 0, fromY = 0, targetX = 0, targetY = 0, savedSpeed = 0;
  float elapsedValue = 0, interpolationValue = 0;
  int elapsed = 0, interpolation = 0, duration = 0, mode = 0;
};
// Mode9 is the original mode used by all literal retail ECL records and the
// laser callback. Other scalar modes0..6/9..16 are supported and sampled.
// Native special vector modes7/8/17 have a different interpolation structure.
bool beginTargetMotion(TargetMotion&, State&, const Record&, World&);
// Returns1 upon completion, for the 4099e0 same-tick extension-start retry.
// Before completion, only velocity and angle change. At completion position
// snaps to target, speed restores, and active2000000 clears before the retry.
bool tickTargetMotion(TargetMotion&, State&, float gameRate = 1.f);
} // namespace th12::eb
