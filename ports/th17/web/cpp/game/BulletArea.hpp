#pragma once
#include "BulletState.hpp"
namespace th17 {
bool bullet_circle_area(const BulletState&,const Vec3& center,float radius,bool honor_protection)noexcept;
struct BulletRectangleArea {
    Vec3 center;float half_x,half_y,sine,cosine;
    BulletRectangleArea(const Vec3&,const Vec2&,float angle)noexcept;
    bool contains(const BulletState&)const noexcept;
};
bool bullet_rectangle_area(const BulletState&,const Vec3& center,const Vec2& size,float angle)noexcept;
}
