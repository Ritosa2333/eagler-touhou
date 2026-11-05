#pragma once
#include "Types.hpp"
#include <array>
namespace th15 {
struct BulletColor {i32 sprite=0,hit=0,spawn=0,cancel=0;};
struct BulletAppearance {i32 script=0;std::array<BulletColor,16> colors{};float radius=0;i32 item=0,cancel_kind=0,overlay=0;};
extern const std::array<BulletAppearance,44> bullet_appearances;
const BulletAppearance* bullet_appearance(i32 type)noexcept;
i32 bullet_cancellation(const BulletAppearance&,i32 color,i32 previous,bool replacement)noexcept;
i32 bullet_animation_parameter(i32 type,i32 color,i32 parameter)noexcept;
}
