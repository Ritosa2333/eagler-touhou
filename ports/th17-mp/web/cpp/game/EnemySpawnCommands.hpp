#pragma once
#include "EnemyState.hpp"
#include "EclContext.hpp"
namespace th17 {
class EnemySpawnCommands final:public EclCommands {
    EnemyState& enemy;EnemyWorldState& world;
public:
    EnemySpawnHost* spawning=nullptr;
    EnemySpawnCommands(EnemyState& e,EnemyWorldState& w):enemy(e),world(w){}
    int execute(EclContext&,u16 opcode)override;
};
}
