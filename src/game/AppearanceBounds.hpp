#pragma once
#include "EnemyRules.hpp"
#include "animation/AnmTypes.hpp"
#include <optional>

namespace th128::enemy {
std::optional<Bounds> scaledSpriteBounds(float width,float height,float scaleX,float scaleY) noexcept;
std::optional<Bounds> constantSpriteBounds(const anm::Bank& bank,int script);
}
