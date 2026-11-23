// th16 输入采样——原版 0x401d50（键盘位收集）+ 0x4018e0（手柄合并）+ 0x405580（边沿与连发）。
//
// 位布局（0x401d50 的 GetKeyboardState 位移链逐项反推）：
//   bit0  Z / 手柄按钮        bit1  X        bit3  Shift（低速）
//   bit4  ↑/小键盘8  bit5 ↓/2  bit6 ←/4  bit7 →/6（小键盘 7/9/1/3 = 对角组合 0x50/0x90/0x60/0xa0）
//   bit8  Esc        bit9  Ctrl / C（跳过对话）
//   bit16 Q  bit17 S  bit18 P / Home  bit19 Enter  bit20 D  bit21 R  bit24 F10
// 菜单口径（0x44b5f0）：确定 = 0x80001（Z|Enter），取消 = 0x102（X|Esc）。
// DAT_004a50b0 = held，+4 = previous，+8 = repeat（DAT_004a50b8），+0xc = pressed（DAT_004a50bc），+0x10 = released。
#pragma once
#include "Types.hpp"

namespace th16 {

struct InputState {
    static constexpr u32 kShot = 1u << 0;
    static constexpr u32 kBomb = 1u << 1;
    static constexpr u32 kSlow = 1u << 3;
    static constexpr u32 kUp = 1u << 4;
    static constexpr u32 kDown = 1u << 5;
    static constexpr u32 kLeft = 1u << 6;
    static constexpr u32 kRight = 1u << 7;
    static constexpr u32 kEscape = 1u << 8;
    static constexpr u32 kSkip = 1u << 9;
    static constexpr u32 kRelease = 1u << 11;   // 季节解放（C 键）
    static constexpr u32 kQ = 1u << 16;
    static constexpr u32 kS = 1u << 17;
    static constexpr u32 kPause = 1u << 18;
    static constexpr u32 kEnter = 1u << 19;
    static constexpr u32 kD = 1u << 20;
    static constexpr u32 kR = 1u << 21;
    static constexpr u32 kF10 = 1u << 24;

    static constexpr u32 kConfirmMask = kShot | kEnter;   // 0x80001
    static constexpr u32 kCancelMask = kBomb | kEscape;   // 0x102

    u32 held = 0;       // DAT_004a50b0
    u32 previous = 0;   // DAT_004a50b4
    u32 repeat = 0;     // DAT_004a50b8：长按连发
    u32 pressed = 0;    // DAT_004a50bc
    u32 released = 0;   // DAT_004a50c0
    u32 held_over7 = 0; // +0x22c：按住超过 7 帧的位
    u32 hold_frames[32] = {};   // +0x14 起：每位连续按下帧数（连发后 −8）
    u32 total_frames[32] = {};  // +0x114 起：累计按住帧数

    // 0x405580
    void commit() {
        repeat = 0;
        held_over7 = 0;
        for (int i = 0; i < 32; ++i) {
            const u32 bit = 1u << i;
            if ((held & bit) == 0) {
                hold_frames[i] = 0;
                total_frames[i] = 0;
                continue;
            }
            ++hold_frames[i];
            ++total_frames[i];
            if (hold_frames[i] > 7u) held_over7 |= bit;
            if (hold_frames[i] > 0x19u) {
                repeat |= bit;
                hold_frames[i] -= 8;
            }
        }
        pressed = (previous ^ held) & held;
        released = ~held & (previous ^ held);
        previous = held;
    }
};

}  // namespace th16
