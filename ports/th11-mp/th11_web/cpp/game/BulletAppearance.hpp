#pragma once
#include "Types.hpp"
#include <array>
namespace th11 {
struct BulletAppearance {i32 script,sprites[48];float hitbox;i32 group,effect_mode;};
static_assert(sizeof(BulletAppearance)==208);
extern const std::array<BulletAppearance,29> bullet_appearances;
extern const std::array<i32,8> bullet_cancel_scripts;
}
