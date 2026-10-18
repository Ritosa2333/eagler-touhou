#pragma once
#include "GameMath.hpp"
namespace th10 {
struct Movement {
    Vec3 position;
    Vec3 velocity; // The orbit center while flags & 1 is set.
    float speed;
    float angle;
    float radius;
    float radial_velocity;
    u32 flags;
    void update_velocity() noexcept;
    void update() noexcept;
    void set_angle(float radians) noexcept {angle=normalize_angle(radians).to_float();}
};
static_assert(sizeof(Movement)==0x2c);
}
