#pragma once
#include "EnemyFrame.hpp"
#include "EnemyProjectileEnvironment.hpp"
#include "EnemyAnimationEnvironment.hpp"
#include "EnemySpawnEnvironment.hpp"
#include "EnemySceneEnvironment.hpp"
namespace th10 {
struct EnemyCommandEnvironment {
    EnemyFrameEnvironment& frame;
    EnemyProjectileEnvironment& projectiles;
    EnemyAnimationEnvironment& animations;
    EnemySpawnEnvironment& spawning;
    EnemySceneEnvironment& scene;
};
}
