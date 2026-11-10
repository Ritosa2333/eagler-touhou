// th13-mp 浏览器宿主内部接口（P2 生产帧循环）。
// 帧体实现留在 BrowserRuntime.cpp（输入采样/应用状态是该 TU 的私有面）；
// MultiplayerExports.cpp 的 C 导出只转发到这里的稳定入口。
#pragma once

namespace th13::sdl {

// 一帧 rAF 风格驱动：装载挡板 → FrameCadence → 每 tick 分发（MP 会话
// Active 时走同 tick 重模拟 + 时间同步分支）。返回 false = 硬失败。
bool drive_one_frame(double delta_seconds);

// MP 手动泵导出入口：running/app 未就绪或 delta 非法返回 0。
int mp_run_frame(double delta_ms);

// 会话 Configure 成功后清零 MP 时间同步累积。
void reset_mp_accumulator();

}  // namespace th13::sdl
