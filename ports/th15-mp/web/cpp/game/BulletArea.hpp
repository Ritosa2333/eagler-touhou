#pragma once
#include "BulletState.hpp"
namespace th15 {
bool bullet_circle_area(const BulletState&,const Vec3& center,float radius,bool honor_protection)noexcept;
bool bullet_rectangle_area(const BulletState&,const Vec3& center,const Vec2& size,float angle)noexcept;
}
