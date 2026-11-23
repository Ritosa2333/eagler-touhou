// th16 ReplayManager 快进 / FPS 文字——原版 0x448e40 / 0x4482f0（由 0x448e90 绘制链 thunk 转入）。
//
// 0x448e40（更新链优先级 0x22）：GameManager 存在、非装载（+0x88 bit2）、ReplayManager+0xc==1、
// DAT_004a50b0 & 0x201（Z|Skip）时，按 +0x20c 的有符号 %8：非 0 → Restart(6)，否则 Continue(1)。
// 计数器由记录链 0x447fd0 / 回放链 0x448130 递增；本函数只读。
//
// 0x4482f0：回放模式在 (383,450) 以 "%3d" 画 +0x208；颜色 <30 红、<50 粉、否则白。
#pragma once
#include "Chain.hpp"
#include "Types.hpp"

namespace th16 {

// 0x448e40：返回 ChainResult::Restart / Continue（值为 6 / 1）
inline ChainResult replay_fast_forward_result(bool game_manager_live, bool loading, i32 replay_mode,
                                             u32 held, i32 frame_counter) {
    if (!game_manager_live || loading || replay_mode != 1 || (held & 0x201u) == 0)
        return ChainResult::Continue;
    // MSVC 有符号 %8：与 `and eax,0x80000007; jns …` 同口径；非负时等价 counter&7
    if ((frame_counter % 8) != 0) return ChainResult::Restart;
    return ChainResult::Continue;
}

// 0x4482f0 颜色选择（DAT_004a6d98+0x1920c）
inline u32 replay_fps_color(u8 fps_byte) {
    const float fps = float(fps_byte);
    if (fps < 30.0f) return 0xff5050ffu;
    if (fps < 50.0f) return 0xffa0a0ffu;
    return 0xffffffffu;
}

// 0x4263e0 全局 "%2.1ffps" 颜色：阈值 30 / 40（FrameTimer+0x30）
inline u32 global_fps_color(float fps) {
    if (fps < 30.0f) return 0xff5050ffu;
    if (fps < 40.0f) return 0xffa0a0ffu;
    return 0xffffffffu;
}

}  // namespace th16
