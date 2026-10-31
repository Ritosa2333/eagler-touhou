#pragma once
#include "BulletPool.hpp"
#include "GameTypes.hpp"
namespace th12 {
extern bullet_pool::Slots hostilePool;
extern std::array<Projectile*, bullet_pool::Slots::capacity> hostileSlots;
void resetHostilePool();
void attachHostileSlot(Projectile&);
void releaseHostileBullet(u32 id);
void refreshHostilePointers();
} // namespace th12
