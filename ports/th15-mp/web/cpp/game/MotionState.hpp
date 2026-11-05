#pragma once
#include "Types.hpp"
namespace th15 {
// TH15's shared motion modes are used by enemies and projectiles. Position is
// quantized to hundredths after each update, including negative coordinates.
struct MotionState {
    Vec3 position{},origin{};
    float speed=0,angle=0,radius=0,angular_velocity=0,axis_angle=0,axis_scale=0,phase=0;
    Vec3 velocity{};u32 flags=0;
    void set_angle(float value)noexcept;
    void integrate(float rate=1)noexcept;
    void advance(float rate=1)noexcept;
};
static_assert(sizeof(MotionState)==68);
}
