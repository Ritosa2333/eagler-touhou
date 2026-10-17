#pragma once
#include "EnemyState.hpp"
#include "EclContext.hpp"
namespace th15 {
class BulletCommands {
public:
    EnemyState& enemy;EnemyWorldState& world;BulletEmissionHost* host=nullptr;
    BulletCommands(EnemyState& enemy,EnemyWorldState& world):enemy(enemy),world(world){}
    int execute(EclContext& context,u16 opcode);
};
}
