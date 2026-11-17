// 换关装载钟与关首数值重置的纯策略函数（生产 Application.cpp 与单测共用）。
//
// 原版证据（th13.exe v1.00c，反编译对读）：
//   0x42b990:45  `or dword [stage_ctx+0x60], 4` 装载入口置 bit2；
//   0x42b990:231 装载成功（FUN_0043b5a0 之后）`and dword [..+0x60], ~4` 清 bit2。
//   0x42c550:248-261 `(scene_flags & 4)==0` 才放行 FUN_0042a370（分数滚动）。
//   0x45f430（45ef50 为 kind0 淡入，无此门）只在 GameInf+0x60 bits0/2 全清时
//     推进计时：`((flags>>2 | flags) & 1) == 0`，即 bit0|bit2 都为 0。
//   0x409270（0x42c550 装载门 state0 旧 STD 分支）创建 30 帧退场淡出：
//     update 45f430 priority16 / draw 45f4c0 priority11，kind2、duration 0x1e、
//     RGB 0；同一处 0x409380 启动新 STD 的 60 tick 进入状态。
// 因此 bit2 的生命周期必须止于「装载完成帧」（port 的 start_transition 帧，
// 0x42b990:75-78 把 be810 追平 be7c0 的同一拍），而不是 0x42c550 激活分支
// （port 的 activate_next_stage/begin_run）。拖到激活会让 0x409270 的淡出
// 计时器在整个 30 帧门内冻结在 time=0/alpha=0，用户侧的换面淡出动画消失。
#pragma once

#include "../game/Types.hpp"

namespace th13::sdl {

// 装载入口置 stage ctx +0x60 bit2（0x42b990:45）。分数滚动门与 45f430 计时门
// 共用该位。
inline constexpr u32 kStageLoaderSceneFlag = 4u;

// 装载成功帧清 bit2（0x42b990:231），其余 scene 位（0x10 覆盖层、0x4000 通关
// 演出等）原样保留。
inline u32 stage_loader_completed_scene_flags(u32 scene_flags) noexcept {
    return scene_flags & ~kStageLoaderSceneFlag;
}

// 0x45f430 的计时门：kind5（port 的退场淡出；对应原版 0x409270 的 kind2，
// 两者共用 45f430 更新体）在 GameInf+0x60 bit0/bit2 置位时冻结；kind0
// （45ef50 淡入）不受影响。
inline bool stage_loading_holds_game_fade(i32 kind, u32 scene_flags) noexcept {
    return kind == 5 && (scene_flags & 5u) != 0;
}

// 0x42b990:103-110（dcc30!=0，新局/Retry）重置 be7c0/be7d0/be7dc/be7e4；
// 0xc 换关（:54-78，dcc30==0）全部跨关保留。表：初值 0x4c0ab0、封顶 0x4c0ac8。
inline constexpr i32 kRunPointInitTable[6] = {5000, 10000, 15000, 20000, 20000, 100000};
inline constexpr i32 kRunPointCapTable[6] = {100000, 250000, 500000, 500000, 500000, 1000000};

// continuing=true（0xc 换关）时不做任何写入；false（新局/Retry/回放冷启动）
// 才执行 0x42b990:103-110 的四项重置。
inline void run_entry_reset_points(bool continuing, i32 difficulty, i32& score,
                                   i32& max_point, i32& max_point_cap,
                                   i32& graze) noexcept {
    if (continuing) return;
    score = 0;
    max_point = kRunPointInitTable[difficulty] * 100;
    max_point_cap = kRunPointCapTable[difficulty] * 100;
    graze = 0;
}

}  // namespace th13::sdl
