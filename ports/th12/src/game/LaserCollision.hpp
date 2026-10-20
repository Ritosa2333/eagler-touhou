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
  // Native 437980 reads SHT+4, independently of Player+9e4/+9e8 used by
  // the line query. Appended to preserve existing aggregate initializers.
  float circleHitRadius = 2;
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
// Native 437980. Its invulnerable inner hit returns1 without438370; its
// outer graze precedes all Player state/dialogue gates. clearLaser is false:
// this primitive does not decide whether the calling object is removed.
LineCollisionResult collidePlayerCircle(const Vec3& center, float radius,
                                        const PlayerCollision& player);
} // namespace th12::laser
