#pragma once
#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#error SceneOwnerPools is multiplayer-only
#endif

#include "../game/Bomb.hpp"
#include "../game/GameObjectResources.hpp"
#include "../game/GameSession.hpp"
#include "../game/LaserManager.hpp"
#include "CooperativeRules.hpp"
#include "RollbackPool.hpp"

namespace th10::multiplayer {
// Native scene callbacks retain these owner addresses. Pools belong to World,
// across scene teardown/reconstruction; generation reset only clears history.
using LaserOwnerPool=RollbackPool<sizeof(LaserManager),1>;
using BombOwnerPool=RollbackPool<sizeof(Bomb),kMaxSeats>;
using SessionOwnerPool=RollbackPool<sizeof(GameSession),1>;
using GameEffectOwnerPool=RollbackPool<sizeof(GameEffects),1>;
}
