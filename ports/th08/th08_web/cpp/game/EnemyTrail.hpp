#pragma once
#include "AnmRenderer.hpp"
namespace th08 {
struct EclVm;struct EclInstruction;
struct EnemyTrailPoint {Vec3 position,velocity;float angle=0;};
struct EnemyTrail {
    EnemyTrailPoint points[96];SpriteVertex vertices[194];
    u8 flags=0,padding=0;i16 length=0,collision_length=0,step=0;
    EnemyTrail(){std::memset(this,0,sizeof(*this));}
};
static_assert(sizeof(EnemyTrailPoint)==28&&sizeof(EnemyTrail)==0x1fc0);
bool configure_enemy_trail(EclVm&,const EclInstruction&);
bool update_enemy_trail(EclVm&)noexcept;
}
