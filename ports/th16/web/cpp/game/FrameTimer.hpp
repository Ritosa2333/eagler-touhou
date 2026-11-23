// th16 FrameTimer——原版 DAT_004a6dc8（0x4262f0 测速；绘制链 0x4b 画 "%2.1ffps"）。
//
// 锚点：
//   0x4262f0  QPC 秒 − 起点；满 1s 用本窗口逻辑帧数估 FPS 写入 +0x30；游戏未暂停时累加 +0x20/+0x28
//            （慢放率 = 100 − (+0x20)/(+0x28)*100，供 0x448400 回放头）
//   0x4263e0  颜色阈值：fps <30 红 / <40 粉 / 白（见 Replay.hpp global_fps_color）
//
// harness 无墙钟时不调用 tick，fps 保持 60、slow_rate 为 0，避免绘制 / digest 黄金漂移。
#pragma once
#include "Types.hpp"

namespace th16 {

struct FrameTimer {
    double origin = 0;          // +0x10：窗口起点（首次 QPC；之后每次满秒推进）
    i32 over65_streak = 0;      // +0x18：连续 >65fps 计数（原版写但本端口只镜像）
    i32 window_frames = 0;      // +0x1c：本秒累计逻辑帧（含跳帧 DAT_004c12c4）
    double elapsed_good = 0;    // +0x20：计入慢放分子（fps≤57 用实测，否则 +60）
    double elapsed_total = 0;   // +0x28：分母（每秒 +60）
    float fps = 60.0f;          // +0x30

    void reset();
    // now = QPC 秒；skip_frames = DAT_004c12c4；paused = GameManager flags & 0x14（装载/暂停）
    void tick(double now, i32 skip_frames, bool game_live, bool paused);
    // 0x448400：100 − elapsed_good/elapsed_total×100；分母为 0 时 0
    float slow_rate() const;
};

}  // namespace th16
