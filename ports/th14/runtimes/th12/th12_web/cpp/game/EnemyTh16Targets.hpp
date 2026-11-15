#pragma once
#include "Types.hpp"
namespace th11 {
struct Enemy;struct EnemyLink;
u32 enemy_th16_nearest(EnemyLink*,Vec3,float)noexcept;
bool enemy_th16_valid(EnemyLink*,u32)noexcept;
Enemy* enemy_th16_lookup(EnemyLink*,u32)noexcept;
}
