// th16 帧计时——原版 0x45a8a0 入口：
//   若 DAT_004d9d1c & 0x40（可变帧率追帧），while (logical(+0x2088) < target(+0x2078)) logical += 1/60；
//   之后跑一遍更新链 0x401460；绘制链 0x4015a0 受跳帧计数 DAT_004c12c4 节制。
// 原版每次调用只跑一个逻辑步，由主循环按 QueryPerformanceCounter 节拍重复调用。
// 浏览器只有 rAF 一个节拍（60/120/144Hz 不定），因此在这里把墙钟差折算成逻辑步数，
// 由 Application::step_at 逐步调用 logic_step，保证 60 逻辑帧/秒与原版一致。
#pragma once
#include "Types.hpp"

namespace th16 {

struct FrameClock {
    static constexpr double kStep = 1.0 / 60.0;
    static constexpr double kResyncGap = 0.5;  // 超过半秒（标签页挂起等）直接对齐，不补帧
    static constexpr u32 kMaxSteps = 4;        // 单个宿主帧最多追 4 步（原版靠跳帧 DAT_004c12c4）

    double now = 0;          // 墙钟秒（宿主写入；对应 +0x2078）
    double logical = 0;      // 已消费的逻辑时间（+0x2088）
    bool started = false;
    bool catch_up = true;    // DAT_004d9d1c & 0x40
    u32 subframes = 0;       // 本宿主帧应执行的逻辑步数

    u32 begin_frame(double wall_seconds) {
        now = wall_seconds;
        if (!started || !catch_up) {
            started = true;
            logical = now;
            subframes = 1;
            return subframes;
        }
        if (now - logical > kResyncGap || now < logical - kResyncGap) {
            logical = now;
            subframes = 1;
            return subframes;
        }
        // 四舍五入到最近的步数：60Hz rAF 抖动 ±数 ms 时稳定 1 步/帧，120Hz 交替 0/1，30Hz 固定 2；
        // 显示器 59.9Hz 之类的慢漂移会周期性补 1 步，保持 60 逻辑帧/秒（与原版 QPC 节拍一致）。
        const double behind = now - logical;
        u32 steps = behind <= 0 ? 0 : u32(behind / kStep + 0.5);
        if (steps > kMaxSteps) {
            steps = kMaxSteps;
            logical = now;  // 追不上就丢弃余量（原版靠跳帧）
        } else {
            logical += steps * kStep;
        }
        subframes = steps;
        return subframes;
    }
};

}  // namespace th16
