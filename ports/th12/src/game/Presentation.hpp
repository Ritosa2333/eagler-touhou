#pragma once
#include "GameTypes.hpp"

namespace th12 {
struct Sprite {
  float x, y, rotation, scale;
  int bank, animation, kind, color, age;
  u32 entityId = 0;
  int interrupt = 0;
  float alpha = 1;
};
struct AnimationPose {
  float x = 0, y = 0, z = 0, rx = 0, ry = 0, rotation = 0, sx = 1, sy = 1, u = 0, v = 0,
        uvScaleX = 1, uvScaleY = 1, radius = 0, width = 0;
  u32 primary = 0, secondary = 0;
  int bank = 0, sprite = 0, layer = 0, mode = 0, anchorX = 0, anchorY = 0, blend = 0, segments = 0;
  u32 flags = 0, flags2 = 0, id = 0, owner = 0;
  float alpha = 1;
  u32 reserved[3]{};
};
static_assert(sizeof(AnimationPose) == 128);
// Kept separate from the 128-byte pose ABI: native managers can draw an
// embedded VM at a scheduler priority unrelated to its ANM layer.
struct AnimationDrawSchedule {
  u32 id = 0;
  int priority = 0;
  u32 order = 0, clip = 2; // 0full640x480,1playfield384x448,2stage422x480.
};
static_assert(sizeof(AnimationDrawSchedule) == 16);
struct AnimationTransform {
  float x = 0, y = 0, z = 0, sx = 1, sy = 1, rotation = 0;
};
extern std::vector<Sprite> drawState;
extern std::vector<AnimationPose> animationDrawState;
extern std::vector<AnimationDrawSchedule> animationScheduleState;
extern std::array<float, 41> hud;
AnimationTransform resolveAnimationTransform(const anm_logic::Node& node, int depth = 0);
void captureAnimationDraw();
} // namespace th12
