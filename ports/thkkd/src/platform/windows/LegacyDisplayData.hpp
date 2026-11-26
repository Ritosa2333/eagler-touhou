#pragma once
#include "render/DisplayState.hpp"

namespace kaikeidou {
inline void install_legacy_display_data(DisplayState& display) {
    // 原程序代码段内的只读索引，用于选择弹型、机体武器与 bomb 的显示分支。
    constexpr std::uint8_t projectile_styles[]{
        0,1,14,14,14,14,14,14,14,14,14,14,14,2,14,14,14,
        3,4,5,6,7,8,6,9,14,10,11,12,14,14,14,13
    };
    constexpr std::uint8_t weapon_styles[]{
        0,1,2,9,9,9,9,9,9,9,3,4,5,9,9,9,9,9,9,9,6,7,8
    };
    for(unsigned i=0;i<sizeof(projectile_styles);++i)display.store<1>(0x615774+i,projectile_styles[i]);
    for(unsigned i=0;i<sizeof(weapon_styles);++i){
        display.store<1>(0x619a68+i,weapon_styles[i]);
        display.store<1>(0x65d458+i,weapon_styles[i]);
    }
}
}
