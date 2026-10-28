// stage1 运动数学原语（oracle 对拍口径，锚点见 docs/plans/event-contract.md §5）。
// 全部按原版 0x472f30/0x474f00/0x473a40/0x497670 的运算结构复刻：
// 中间量以 double 计算、落点舍回 float，对齐 x87 long-double 中间精度。
#pragma once

#include "Types.hpp"

#include <cmath>

namespace th13 {

// 0x472f30/0x472e90：角度归一到 [-π, π]。原版为逐次 ±2π 的浮点循环，
// 常量取 float π 的 double 视图（3.1415927410125732 / 6.2831854820251465）；
// 每步减/加 2π 后落 f32 再继续（x87 (float) 舍入——逐帧对拍实证必需）。
// 循环退出比较位于本次减法 f32 落盘之后（0x472e90/0x472f30 的
// mov eax,ecx; inc ecx; fstp; cmp eax,0x20; jg——s5-drift-20261028 §2.2/§2.3
// 位级复核）：正方向每次调用最多 34 次减法、第 34 次也落盘后才退出；
// 负方向 do-while 对称（同为 34 次）。超范围角（ECL 累积自旋角
// 780.3973388671875 rad）两条 wrap 链各耗尽 34 次减法，结果 =
// 353.14031982421875（33 次链得 365.7066650390625，恰差 2×2π）。
inline float wrap_angle(float value) {
    const double kPi = 3.1415927410125732;
    const double kTwoPi = 6.2831854820251465;
    double v = value;
    int up = 0;
    while (kPi < v) {
        v = static_cast<float>(v - kTwoPi);
        // 0x472e90/0x472f30 正方向：计数比较在减法落盘之后——第 34 次
        // （迭代序号 0x21）减法也落盘并以此退出；减前 break 只做 33 次。
        if (0x20 < up) break;
        ++up;
    }
    if (-kPi <= v) return static_cast<float>(v);
    int down = 0;
    do {
        v = static_cast<float>(v + kTwoPi);
        if (0x20 < down) return static_cast<float>(v);
        ++down;
    } while (v < -kPi);
    return static_cast<float>(v);
}

// 镜像翻转（0x41c137/0x41c21f 处理块）：a → wrap(π/2 − wrap(a − π/2))。
inline float mirror_angle(float value) {
    return wrap_angle(static_cast<float>(1.5707963705062866 -
                                         wrap_angle(static_cast<float>(value - 1.5707963705062866))));
}

// 0x474f00：极坐标 → xy（cos/sin 以 double 计算，落点 float）。
inline void polar_xy(float angle, float radius, float& out_x, float& out_y) {
    out_x = static_cast<float>(std::cos(static_cast<double>(angle)) * static_cast<double>(radius));
    out_y = static_cast<float>(std::sin(static_cast<double>(angle)) * static_cast<double>(radius));
}

// 0x497690 + 0x473780 尾部：坐标量化到 0.01（floor，向负无穷）。
// 原版序列（0x4739c0..0x4739f0）：f32 值 ×100.0 落 f32 → 转 double →
// floor → /100.0 落 f32。实测 -163.783264 → -163.79（round 会给 -163.78）。
inline float round001(float value) {
    const double scaled = static_cast<double>(value * 100.0f);
    return static_cast<float>(std::floor(scaled)) / 100.0f;
}

// 0x473a40 缓动表（mode = 脚本插值模式；t = elapsed/duration，f32 除法）。
// 逐 case 对照反编译；7/17/8 由插值器特判，不经此函数。
inline float ease_interp(i32 mode, float elapsed, float duration) {
    const float t = elapsed / duration;  // f32 除法（原版 fVar1 = param_2 / param_3）
    const double x = t;
    switch (mode) {
    case 1: return static_cast<float>(x * x);
    case 2: return static_cast<float>(x * x * x);
    case 3: return static_cast<float>(x * x * x * x);
    case 4: return static_cast<float>(1.0 - (1.0 - x) * (1.0 - x));
    case 5: { const double y = 1.0 - x; return static_cast<float>(1.0 - y * y * y); }
    case 6: { const double y = 1.0 - x; return static_cast<float>(1.0 - y * y * y * y); }
    case 0xa: {
        const double y = x * 2.0;
        if (y < 1.0) return static_cast<float>(y * y * y * 0.5);
        const double z = 2.0 - y;
        return static_cast<float>((2.0 - z * z * z) * 0.5);
    }
    case 0xb: {
        const double y = x * 2.0;
        if (y < 1.0) return static_cast<float>(y * y * y * y * 0.5);
        const double z = 2.0 - y;
        return static_cast<float>((2.0 - z * z * z * z) * 0.5);
    }
    case 0xc: {
        const double y = x + x;
        if (y < 1.0) return static_cast<float>(0.5 - (1.0 - y) * (1.0 - y) * 0.5);
        return static_cast<float>((y - 1.0) * (y - 1.0) * 0.5 + 0.5);
    }
    case 0xd: {
        const double y = x + x;
        if (y < 1.0) { const double z = 1.0 - y; return static_cast<float>(0.5 - z * z * z * 0.5); }
        const double z = y - 1.0;
        return static_cast<float>(z * z * z * 0.5 + 0.5);
    }
    case 0xe: {  // 反编译首分支与 0xd 同式（三次），第二分支四次，照录
        const double y = x + x;
        if (y < 1.0) { const double z = 1.0 - y; return static_cast<float>(0.5 - z * z * z * 0.5); }
        const double z = y - 1.0;
        return static_cast<float>(z * z * z * z * 0.5 + 0.5);
    }
    case 0xf: return 0.0f;
    case 0x10: return 1.0f;
    case 18: // 473d4c: sine-out
        return float(std::sin(double(float(x*double(3.1415927f)*.5))));
    case 19: { // 473d77: sine-in
        const float s=float(std::sin(double(float(x*double(3.1415927f)*.5+double(1.5707964f)))));
        return float(1.0-double(s));
    }
    case 20: { // 473db4: out/in, argument continues through pi
        const float twice=float(t*2);
        const float s=float(std::sin(double(float(double(twice)*double(3.1415927f)*.5))));
        return twice<1?float(double(s)*.5):float((1.0-double(s))*.5+.5);
    }
    case 21: { // 473e35: in/out, second half restarts the sine at zero
        const float twice=float(t*2);
        const float a=twice<1?float(double(twice)*double(3.1415927f)*.5+double(1.5707964f)):
                             float((double(twice)-1)*double(3.1415927f)*.5);
        const float s=float(std::sin(double(a)));
        return twice<1?float((1.0-double(s))*.5):float(double(s)*.5+.5);
    }
    case 0x16: return static_cast<float>((((x - 0.25) * (x - 0.25)) / 0.5625 - 0.11111111) / 0.8888889);
    case 0x17: return static_cast<float>((((x - 0.3) * (x - 0.3)) / 0.48999998 - 0.18367349) / 0.8163265);
    case 0x18: return static_cast<float>((((x - 0.35) * (x - 0.35)) / 0.42249995 - 0.28994083) / 0.71005917);
    case 0x19: return static_cast<float>((((x - 0.38) * (x - 0.38)) / 0.3844 - 0.37565035) / 0.62434965);
    case 0x1a: return static_cast<float>((((x - 0.4) * (x - 0.4)) / 0.36 - 0.44444445) / 0.5555556);
    case 0x1b: { const double y = (1.0 - x) - 0.25; return static_cast<float>(1.0 - ((y * y) / 0.5625 - 0.11111111) / 0.8888889); }
    case 0x1c: { const double y = (1.0 - x) - 0.3; return static_cast<float>(1.0 - ((y * y) / 0.48999998 - 0.18367349) / 0.8163265); }
    case 0x1d: { const double y = (1.0 - x) - 0.35; return static_cast<float>(1.0 - ((y * y) / 0.42249995 - 0.28994083) / 0.71005917); }
    case 0x1e: { const double y = (1.0 - x) - 0.38; return static_cast<float>(1.0 - ((y * y) / 0.3844 - 0.37565035) / 0.62434965); }
    case 0x1f: { const double y = (1.0 - x) - 0.4; return static_cast<float>(1.0 - ((y * y) / 0.36 - 0.44444445) / 0.5555556); }
    default: break;
    }
    // case 9 及 default（含 mode 0）：mode 9 = in-out quad，default = 线性。
    if (mode != 9) return t;
    const double y = x * 2.0;
    if (y < 1.0) return static_cast<float>(y * y * 0.5);
    return static_cast<float>((2.0 - (2.0 - y) * (2.0 - y)) * 0.5);
}

}  // namespace th13
