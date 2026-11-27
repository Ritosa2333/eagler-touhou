// th16 ANM 几何——VM 位置到屏幕坐标的变换与四角计算，绘制（AnmRenderer）与逻辑判定（自机弹出界、
// 对话文字跟随）共用的一组纯函数。
//
// 锚点：
//   0x406a70  位置变换：scale mode（ins_313）乘窗口缩放 → 沿 root 链累加（未分离时，ins_314 随父 Z 旋转）
//             或加 layer group 偏移（0x40000 场地偏移 DAT_004d9d50/54，其它 DAT_004d9d48/4c）
//   0x406c40  pos + entity_offset + pos2 → 0x406a70
//   0x465c40  轴对齐四角（锚点 × 精灵尺寸 − anchor_offset，× 窗口缩放，× scale·scale2（含父））+ 位置
//   0x4660b0  带 Z 旋转的四角（0x464dd0 世界旋转）
//   0x466390  mode 4：世界坐标经当前相机投影成屏幕点，按"一单位 right 向量投影后的像素长"缩放的旋转四角
//   0x468c00  VM 四角的屏幕坐标（自机子弹出界判定）
//
// 屏幕几何常量来自 Supervisor 每帧写入的全局：DAT_004d9d34 窗口缩放（0x45b280，默认 1.0）、
// DAT_004d9d50/54 场地偏移 = (w/2, (h-448)/2)、DAT_004d9d48/4c = (w/2, 16×缩放)（0x43cb10 / 0x43d140）。
#pragma once
#include "AnmVm.hpp"
#include "Camera.hpp"
#include "Types.hpp"

namespace th16 {

struct AnmGeometry {
    float window_scale = 1.0f;    // DAT_004d9d34
    float field_offset_x = 320;   // DAT_004d9d50（layer group 0x40000）
    float field_offset_y = 16;    // DAT_004d9d54
    float screen_offset_x = 320;  // DAT_004d9d48（其它非零 layer group）
    float screen_offset_y = 16;   // DAT_004d9d4c

    // 0x465c40 / 0x406a70：ins_313 的 scale mode → 窗口缩放因子（1 / 3 整倍，2 / 4 半倍，其余 1）
    static float scale_factor(const AnmVm& vm, float window_scale) noexcept;
    // 0x406a70
    Vec3 transform_position(const AnmVm& vm, Vec3 p) const;
    // 0x406c40 + 0x406a70
    Vec3 world_position(const AnmVm& vm) const;
    // 0x465c40 的四角偏移（不含位置）：TL, TR, BL, BR
    void axis_corners(const AnmVm& vm, float out[4][2]) const;
    // 0x4660b0 的四角偏移（不含位置）
    void rotated_corners(const AnmVm& vm, float out[4][2]) const;
    // 0x466390：mode 4 的屏幕四角（已含位置）与投影深度；投影深度不在 [0, 1] 内返回 false（原版返回 -1 不画）
    bool projected_corners(const AnmVm& vm, const Camera& camera, float out[4][2], float& depth) const;
    // 0x468c00：mode 0/2/3 轴对齐、mode 1 带旋转；其它模式不写并返回 false
    bool vm_corners(const AnmVm& vm, float out[4][3]) const;
};

}  // namespace th16
