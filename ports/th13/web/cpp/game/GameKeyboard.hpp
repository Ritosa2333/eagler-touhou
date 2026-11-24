// 键盘位掩码：浏览器物理键（KeyboardMap.inc 的 vk）→ 原版 32 位输入状态（0x4e49fc）。
// 菜单位与原版掩码逐项一致（方向 0x10/0x20/0x40/0x80、取消 0x102、确认 0x80001，
// 见 0x4525b0）；对局位沿 th15 同族映射。接入位见 docs/plans/ui-base.md §六。
#pragma once

#include "Types.hpp"
#include <utility>

namespace th13 {

// keys 为按 vk 索引的按下表（与原版 256 键位数组同构）。
u32 keyboard_keys(const bool* virtual_keys) noexcept;

}  // namespace th13
