#pragma once
#include "BulletEmitter.hpp"
#include "Rng.hpp"
namespace th11 {
struct BulletLaunch {
    Vec3 position;Vec2 velocity;float speed,angle;
};
// Computes one member of an emitter's pattern. A rejected bullet still consumes
// the original RNG draws and writes its position, speed and angle, but no velocity.
bool bullet_launch(const BulletEmitter&,i32 index,i32 layer,float aim_angle,
                   Rng&,const Vec3& player,float exclusion_squared,BulletLaunch&)noexcept;
}
