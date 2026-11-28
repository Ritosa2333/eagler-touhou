#pragma once

#include "Types.hpp"

#include <array>

namespace th13 {

struct BulletAppearance {
    i32 script;
    struct Color {
        i32 sprite, hit, spawn, cancel;
    };
    std::array<Color, 16> colors;
    float scale;
    i32 hit_kind, cancel_kind, spawn_script;
};

const BulletAppearance* bullet_appearance(i32 type) noexcept;
inline i32 bullet_cancel_script(i32 type, i32 color) noexcept {
    const auto* entry = bullet_appearance(type);
    if (!entry || color < 0 || color >= 16) return -1;
    switch (entry->cancel_kind) {
    case 0:
        return color * 2 + 4;
    case 1: {
        constexpr i32 scripts[] = {4, 8, 12, 16, 20, 24, 28, 34};
        return color < 8 ? scripts[color] : -1;
    }
    case 2:
        return -1;
    case 3:
        return 16;
    case 4:
        return 6;
    case 5:
        return 12;
    case 6:
        return entry->colors[u32(color)].cancel;
    case 7:
        return 197;
    case 8:
        return 199;
    case 9:
        return 201;
    case 10:
        return 207;
    default:
        return -1;
    }
}

inline i32 bullet_animation_parameter(i32 type, i32 color, i32 parameter) noexcept {
    const auto* entry = bullet_appearance(type);
    if (!entry || color < 0 || color >= 16 || entry->colors[0].sprite < 0) return parameter;
    const auto& value = entry->colors[u32(color)];
    switch (parameter) {
    case 0:
        return value.sprite;
    case 1:
        return value.hit;
    case 2:
        return value.spawn;
    case 3:
        return value.cancel;
    default:
        return parameter;
    }
}

}
