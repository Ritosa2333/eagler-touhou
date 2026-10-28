#pragma once
#include "AnmGeometry.hpp"
namespace th13 {
// Enemy+40cc..40e0: ECL629, 415ae0 grid and41a3d0 deformation.
struct EnemyFog {
    static constexpr size_t side=17;
    float target=0,radius=16,angle_x=0,angle_y=0;
    u32 color=0;
    std::array<AnmGeometryVertex,side*side> vertices{};
    void update(const Vec3& center,float rate=1);
};
}
