#pragma once
#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#error HintOwnerPool is multiplayer-only
#endif

#include "../game/StageHints.hpp"
#include "RollbackPool.hpp"

namespace th10::multiplayer {
// A World keeps one hints manager across destroy/recreate cycles. Its list
// sentinels and UpdateChain entries carry the manager's actual address.
using HintOwnerPool = RollbackPool<sizeof(StageHints),1>;
}
