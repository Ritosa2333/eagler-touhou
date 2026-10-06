// Native 437a80's rotated rectangle/graze query and its Player state gate.
#pragma once
#include "Laser.hpp"
namespace th12::laser {
struct PlayerCollision {
  Vec3 position{};
  // Native Player+9e4/+9e8: collision half extents, not sprite dimensions.
  float hitWidth = 2, hitHeight = 2;
  int state = 1, invulnerability = 0;
  uint32_t flags = 0;
  bool dialogue = false;
};
struct LineCollisionResult {
  Collision code = Collision::None;
  // Native return1 calls438370 immediately, opening the deathbomb window.
  // It neither confirms a Miss nor directly subtracts a life.
  bool beginMiss = false;
  bool clearLaser = false;
};
// These are precisely native stack F0/F1/F2. Native halves transverseSize
// internally, uses longitudinalSize as [0,L], and expands graze by16×Player
// hit extents. The renderer's visible width and a fixed24 graze radius differ.
LineCollisionResult collidePlayerLine(const Vec3& origin, float angle, float transverseSize,
                                      float longitudinalSize, const PlayerCollision& player);
} // namespace th12::laser
