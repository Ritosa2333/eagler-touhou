// th16 实体运动状态（0x44 字节）：自机弹 +0x48、敌弹、道具等共用。
//   0x402ff0  按模式算速度分量（mode 0：极坐标 → velocity；mode 2/3：角速度/半径推进；mode 4：轨道）
//   0x403110  按模式推进位置，再把 x/y 截到 1/100（×100 → floor → ÷100，单精度乘除、CRT floor）
// 本阶段只实现自机弹用到的 mode 0 与 mode 2；mode 3/4 由敌弹（0x411dd0）移植时随其 oracle 补齐。
#pragma once
#include "Types.hpp"

namespace th16 {

// 0x402d90：把角度折回 (-π, π]（两段循环共用计数器，各至多 0x22 次）
float wrap_angle(float a) noexcept;
// 0x405510 / 0x4054f0：CVTSS2SD → CRT sin/cos(double) → CVTSD2SS
float sin_f(float a) noexcept;
float cos_f(float a) noexcept;
// 0x405260：CVTSS2SD → CRT floor → CVTSD2SS
float floor_f(float a) noexcept;
// 0x4054d0 / 0x406470 / 0x417510 / 0x4173a0 / 0x426260：x87 fcos/fsin（扩展精度）× 长度后才舍入到单精度
Vec3 polar(float angle, float length) noexcept;
// 0x4052a0 → 0x487aaa：CRT atan2(double, double) → float
float atan2_f(float y, float x) noexcept;
// D3DXVec3Normalize（oracle 桩口径）：len = sqrt(f32(f32(x²+y²)+z²))，len == 0 时原样返回
Vec3 normalize3(const Vec3& v) noexcept;
// 0x403d30：以 (rect_x, rect_y) 为中心、宽高 (width, height)、绕中心转 angle 的矩形是否与圆 (cx, cy, r) 相交。
// 自机伤害源对敌机（0x445a30）与炸弹矩形消弹（0x416e20）共用
bool rect_hits_circle(float rect_x, float rect_y, float width, float height, float angle,
                      float cx, float cy, float r) noexcept;

struct MotionState {
    Vec3 pos;              // +0x00
    Vec3 pos2;             // +0x0c（mode 4：积分位置）
    float speed = 0;       // +0x18（mode 2/3：角速度）
    float angle = 0;       // +0x1c
    float radius = 0;      // +0x20（mode 2/3）
    float radius_speed = 0;// +0x24
    float angle2 = 0;      // +0x28（mode 4）
    float f2c = 0;         // +0x2c
    float f30 = 0;         // +0x30
    Vec3 velocity;         // +0x34（mode 2：圆心）
    u32 mode = 0;          // +0x40 低 4 位为模式

    // 0x402ff0
    void update_velocity(float game_speed) noexcept;
    // 0x403110
    void integrate(float game_speed) noexcept;
};
static_assert(sizeof(MotionState) == 0x44, "MotionState 为 0x44 字节");

}  // namespace th16
