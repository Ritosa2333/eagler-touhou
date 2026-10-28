// 临时探针（对局窗 -96px 水平偏移排查专用；任务闭环后整体移除）
#pragma once

namespace th13::probe {
// 置位后按 budget 打印相机/顶点取证；budget 递减归零即静默。
extern bool enabled;
extern unsigned budget;
// 相机行独立预算（避免顶点洪水吞掉相机行）。
void probe_cam_arm(unsigned n);
}  // namespace th13::probe
