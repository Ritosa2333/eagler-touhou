#pragma once
#include "Types.hpp"
namespace th11 {
float normalize_angle(float)noexcept;
Vec2 polar(float angle,float length)noexcept;
Vec2 rotate(Vec2,float angle)noexcept;
struct Movement {
    Vec3 position,velocity;
    float speed=0,angle=0,radius=0,radial_velocity=0,ellipse_angle=0,ellipse_ratio=0;
    u32 flags=0;
    void update_velocity()noexcept;
    void update()noexcept;
    void refresh_position()noexcept;
    void snap_position()noexcept;
};
static_assert(sizeof(Movement)==0x34);
}
