// 键盘位掩码：浏览器物理键（KeyboardMap.inc 的 vk）→ 原版 32 位输入状态位。
// 位值源自原版 GetKeyboardState 过滤器（th15 已对照 0x401f50；th13 侧待
// oracle 录像逐帧确认，接入位见 docs/plans/ui-base.md §六）。
#pragma once

#include "Types.hpp"
#include <utility>

namespace th13 {

// keys 为按 vk 索引的按下表（与原版 256 键位数组同构）。
u32 keyboard_keys(const bool* virtual_keys) noexcept;

}  // namespace th13
