#pragma once
#include "BulletShooter.hpp"
#include "Rng.hpp"
namespace th15 {
struct BulletInitialMotion {Vec3 position{},velocity{};float speed=0,angle=0;};
float bullet_aim(const Vec3& position,const Vec3& player)noexcept;
bool form_bullet(const BulletShooter& shooter,i32 column,i32 row,float aim,Rng& random,const Vec3& player,float minimum_distance_squared,BulletInitialMotion& out)noexcept;
}
