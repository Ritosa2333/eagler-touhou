#pragma once
#include "EnemyState.hpp"
#include "BulletManager.hpp"
namespace th17 {
// These rules are selected by ECL IDs. No original function address is used
// for production dispatch.
bool enemy_update_rule(EnemyState&,BulletManager&,Rng&);
}
