#pragma once
#include "EnemyManager.hpp"
namespace th10 {
struct EnemySpawnEnvironment {
    Enemy* const* boss;
    const Vec3* camera_origin;
    virtual void spawn(const char* subroutine,const EnemySpawnParameters& parameters)=0;
    virtual Vec3 project_world(const Vec3& position)=0;
};
}
