#pragma once
#include "BulletEmitter.hpp"
#include "Rng.hpp"
namespace th11 {
struct Th16BulletEmitter;
struct BulletLaunch {
    Vec3 position;Vec2 velocity;float speed,angle;
};
bool bullet_th16_launch(const Th16BulletEmitter&,i32,i32,float,Rng&,const Vec3&,float,BulletLaunch&)noexcept;
// Computes one member of an emitter's pattern. A rejected bullet still consumes
// the original RNG draws and writes its position, speed and angle, but no velocity.
bool bullet_launch(const BulletEmitter&,i32 index,i32 layer,float aim_angle,
                   Rng&,const Vec3& player,float exclusion_squared,BulletLaunch&)noexcept;
struct Th14BulletEmitter;
bool bullet_th14_launch(const Th14BulletEmitter&,i32 index,i32 layer,float aim_angle,
                        Rng&,const Vec3& player,float exclusion_squared,BulletLaunch&)noexcept;
}
