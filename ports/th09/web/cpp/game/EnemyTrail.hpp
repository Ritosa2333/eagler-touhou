#pragma once
#include "AnmLayout.hpp"
#include "SpriteGeometry.hpp"
#include <array>
namespace th09 {
struct EclVm;struct EclInstruction;
struct EnemyTrailPoint {Vec3 position,velocity;float direction=0;};
using EnemyTrailVertex=AttackTextureVertex;
struct EnemyTrail {
    std::array<EnemyTrailPoint,96> history;
    std::array<EnemyTrailVertex,194> vertices;
    u8 flags=0;i16 length=0,collision_length=0,interval=0;
    bool command(EclVm&,const EclInstruction&);
    bool prepare_uv(const AnmVm&,i32 vertex_count);
};
}
