#pragma once
#include "EclVm.hpp"
namespace th08 {
bool intersects_playfield(const Vec3& position,float width,float height)noexcept;
// Original 42cc13..42ce11: an enemy must enter the playfield before ordinary
// off-screen retirement applies. The last trail point can keep it alive.
bool update_enemy_visibility(EclVm&)noexcept;
}
