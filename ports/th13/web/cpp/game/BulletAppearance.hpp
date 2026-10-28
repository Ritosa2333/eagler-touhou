// th13 弹幕外观（类型 → 脚本/精灵表）。表体见 BulletAppearance.cpp 文件头：
// 数据逐字节取自原版 .data，弹体精灵解析与 0x4120b0 回调语义一致。
#pragma once

#include "Types.hpp"

#include <array>

namespace th13 {

struct BulletAppearance {
    i32 script;  // 弹体 ANM 脚本（bullet.anm 内编号）
    struct Color {
        i32 sprite, hit, spawn, cancel;  // 逻辑参数 0..3 → 精灵
    };
    std::array<Color, 16> colors;
    float scale;  // 碰撞半径推导用缩放（+0x104）
    i32 hit_kind, cancel_kind, spawn_script;
};

const BulletAppearance* bullet_appearance(i32 type) noexcept;
// 40e840/40f6a4 initialize bullet+bb0 from type and color, for411df0's
// separate cancel animation (not the bullet VM's interrupt1 sprite).
inline i32 bullet_cancel_script(i32 type,i32 color) noexcept {
    const auto* entry=bullet_appearance(type);
    if(!entry||color<0||color>=16)return -1;
    switch(entry->cancel_kind){
    case 0:return color*2+4;
    case 1:{constexpr i32 scripts[]={4,8,12,16,20,24,28,34};return color<8?scripts[color]:-1;}
    case 2:return -1;
    case 3:return 16;
    case 4:return 6;
    case 6:return entry->colors[u32(color)].cancel;
    case 7:return 197;
    case 8:return 199;
    case 9:return 201;
    case 10:return 207;
    default:return -1;
    }
}

// 0x4120b0：类型未映射（首色弹体精灵 <0）时原样返回参数。
inline i32 bullet_animation_parameter(i32 type, i32 color, i32 parameter) noexcept {
    const auto* entry = bullet_appearance(type);
    if (!entry || color < 0 || color >= 16 || entry->colors[0].sprite < 0) return parameter;
    const auto& value = entry->colors[u32(color)];
    switch (parameter) {
    case 0: return value.sprite;
    case 1: return value.hit;
    case 2: return value.spawn;
    case 3: return value.cancel;
    default: return parameter;
    }
}

}  // namespace th13
