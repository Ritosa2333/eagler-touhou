// th16 菜单光标——原版 MenuCursor（Title +0x24 起，0xd8 字节）。
//   +0x00 index  +0x08 count  +0x0c.. 历史栈（0x402de0 压栈：+0x0c 索引 / +0x4c count，+0x8c 深度）
//   +0x90 禁用项表（最多 16）  +0xd0 wrap 标志  +0xd4 禁用项数
// 0x402e70 移动（wrap/clamp + 跳过禁用项）；0x418720 直接设置并跳过禁用项；0x440c00 禁用一项；0x402de0/0x402e20 压栈/出栈。
#pragma once
#include "Types.hpp"

namespace th16 {

struct MenuCursor {
    static constexpr i32 kMaxDisabled = 16;
    static constexpr i32 kHistoryDepth = 16;

    i32 index = 0;      // +0x00
    i32 count = 0;      // +0x08
    i32 history_index[kHistoryDepth] = {};  // +0x0c
    i32 history_count[kHistoryDepth] = {};  // +0x4c
    i32 history_depth = 0;                  // +0x8c
    i32 disabled[kMaxDisabled] = {};        // +0x90
    bool wrap = false;                      // +0xd0
    i32 disabled_count = 0;                 // +0xd4

    bool is_disabled(i32 i) const {
        for (i32 k = 0; k < disabled_count; ++k) if (disabled[k] == i) return true;
        return false;
    }

    // 0x402e70
    i32 move(i32 delta) {
        if (count <= 0) return index;
        do {
            index += delta;
            if (index >= count) {
                do {
                    index = wrap ? index - count : count - 1;
                } while (index >= count);
            }
            if (index < 0) {
                do {
                    if (!wrap) { index = 0; break; }
                    index += count;
                } while (index < 0);
            }
        } while (is_disabled(index));
        return index;
    }

    // 0x418720：clamp 后若落在禁用项上则向后找（到尾回 0）
    i32 set(i32 value) {
        if (count == 0) { index = value; return value; }
        if (value >= count) value = count - 1;
        else if (value < 0) value = 0;
        index = value;
        while (is_disabled(index)) {
            ++index;
            if (index >= count) index = 0;
        }
        return index;
    }

    // 0x440c00：加入禁用表；若当前正停在禁用项上则顺移
    void disable(i32 item) {
        if (disabled_count < kMaxDisabled) disabled[disabled_count++] = item;
        while (is_disabled(index)) {
            ++index;
            if (index >= count) index = 0;
        }
    }

    // 0x402de0：压栈当前 (index, count)，清禁用表
    void push() {
        history_index[history_depth] = index;
        history_count[history_depth] = count;
        ++history_depth;
        disabled_count = 0;
        if (history_depth > 15) history_depth = 15;
    }

    // 0x402e20：出栈恢复 (index, count)，清禁用表；栈空则 index = 0
    void pop() {
        --history_depth;
        if (history_depth < 0) {
            history_depth = 0;
            index = 0;
            disabled_count = 0;
            return;
        }
        index = history_index[history_depth];
        count = history_count[history_depth];
        disabled_count = 0;
    }
};

}  // namespace th16
