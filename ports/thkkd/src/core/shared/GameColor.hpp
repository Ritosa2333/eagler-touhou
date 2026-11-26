// 原 0x004049a0 在游戏所用 16/32 位显示格式中的颜色编码，参数只读取低八位。
#pragma once
#include <cstdint>

namespace kaikeidou {
inline std::uint32_t game_color(std::uint32_t red, std::uint32_t green, std::uint32_t blue, std::uint32_t display_depth) {
    red &= 255u; green &= 255u; blue &= 255u;
    if (display_depth == 16) return ((red >> 3) << 11) + ((green >> 2) << 5) + (blue >> 3);
    return 0xff000000u + (red << 16) + (green << 8) + blue;
}
}
