#pragma once
#include "EnemyState.hpp"
#include "EclContext.hpp"
#include "EnemyVisualCommands.hpp"
namespace th17 {
class EnemyMovementCommands final:public EclCommands {
    EnemyState& enemy;EnemyWorldState& world;
public:
    EnemyVisualCommands visuals;
    EnemyMovementCommands(EnemyState& enemy,EnemyWorldState& world):enemy(enemy),world(world),visuals(enemy){}
    int execute(EclContext&,u16 opcode)override;
};
}
