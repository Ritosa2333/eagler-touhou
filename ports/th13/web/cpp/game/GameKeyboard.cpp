#include "GameKeyboard.hpp"

namespace th13 {

u32 keyboard_keys(const bool* keys) noexcept {
    if (!keys) return 0;
    u32 value = 0;
    // 本轮接入位（ui-base.md §六）：射击/炸弹/低速/移动（含小键盘与斜向）/暂停/快进/确认。
    constexpr std::pair<u32, u32> bindings[] = {
        {90, 1},        // Z：射击/确认
        {88, 2},        // X：炸弹/取消
        {16, 8},        // Shift：低速移动（双 Shift 在 KeyboardMap 层归一为 vk16）
        {38, 16},       // ↑
        {104, 16},      // 小键盘 8
        {40, 32},       // ↓
        {98, 32},       // 小键盘 2
        {37, 64},       // ←
        {100, 64},      // 小键盘 4
        {39, 128},      // →
        {102, 128},     // 小键盘 6
        {97, 0x60},     // 小键盘 7：左上
        {99, 0xa0},     // 小键盘 1：左下
        {103, 0x50},    // 小键盘 9：右上
        {105, 0x90},    // 小键盘 3：右下
        {27, 0x100},    // Esc：暂停菜单
        {17, 0x200},    // Ctrl：对话快进（对话为桩时仅留位）
        {67, 0xa00},    // C：原版同时置 0x800 灵界与 0x200
        {81, 0x10000},  // Q
        {83, 0x20000},  // S
        {36, 0x40000},  // Home
        {80, 0x40000},  // P
        {13, 0x80000},  // Enter：菜单确认
        {68, 0x100000}, // D
        {82, 0x200000}, // R
        {121, 0x800000}, // F10
    };
    for (const auto& binding : bindings)
        if (keys[binding.first]) value |= binding.second;
    return value;
}

}  // namespace th13
