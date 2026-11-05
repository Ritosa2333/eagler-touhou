#pragma once
#include "enemy-frame-fixture.hpp"
#include "../../cpp/game/EnemyManager.hpp"
namespace th11::test {
struct EnemyManagerFixture:EnemyFrameFixture {
    EnemyManager enemies;
    explicit EnemyManagerFixture(EclProgram& p):enemies(p,enemy.world,enemy.commands,frame_world){}
};
}
