#pragma once
#include "GameTypes.hpp"

namespace th12 {
extern laser::Manager laserManager;
extern laser::World& laserWorld;
uint64_t laserAnimationKey(u32 serial, laser::Role role);
int spawnLaser(Enemy&, Shooter&, laser::Kind, int lookupId = 0);
void tickLasers();
} // namespace th12
