#pragma once
#include "PlayerMotion.hpp"
namespace th09 {
bool intersects_box(const Bounds& hit,Vec2 center,Vec2 extent) noexcept;
bool intersects_circle(Vec2 player,float player_radius,Vec2 center,float radius) noexcept;
bool collects_item(i32 life,const Bounds& pickup,Vec2 center,Vec2 extent) noexcept;
u32 intersects_rotated_box(Vec2 player,Vec2 hit_extent,Vec2 center,Vec2 extent,Vec2 pivot,float angle,bool near) noexcept;
}
