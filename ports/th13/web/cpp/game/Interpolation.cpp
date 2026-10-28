// th13 缓动权重与插值通道步进（行为规格见 Interpolation.hpp 文件头）。
#include "Interpolation.hpp"

#include <cmath>

namespace th13 {

float interpolation_weight(i32 mode, float elapsed, float duration) noexcept {
    if (duration == 0 || std::isnan(duration)) return 1;
    float t = static_cast<float>(elapsed / duration);
    // 0x473a40 精度链（draw_compare f159 帧位级实证）：除 t 入口一次落
    // f32 后，幂/二次曲线族的全部中间量在 x87 80 位寄存器链上流转，
    // 仅最终结果一次落 f32（逐分支 fstps 唯一）。逐步 f32 舍入会在
    // (2-(2-t2)²)×0.5 等链上漂 1-16 ULP（f159 神灵 world[12] 实测
    // c0c0a8d0 vs oracle c0c0a8e0，double 链复现精确命中）。
    // ⟹ 中间量走 double，最终单次落 float。
    auto power = [](double v, u32 exponent) {
        double r = v;
        for (u32 i = 1; i < exponent; i++) r = r * v;
        return r;
    };
    if (mode >= 1 && mode <= 3) return static_cast<float>(power(double(t), u32(mode + 1)));
    if (mode >= 4 && mode <= 6) return static_cast<float>(1 - power(1 - double(t), u32(mode - 2)));
    if (mode >= 9 && mode <= 14) {
        const u32 exponent = u32((mode - 9) % 3 + 2);
        // mode9 族加倍点 t2 落 f32（0x473b65 fstps 实证）——保持单次舍入。
        const double t2 = static_cast<float>(t * 2);
        if (mode <= 11)
            return t2 < 1 ? static_cast<float>(power(t2, exponent) * .5)
                          : static_cast<float>((2 - power(2 - t2, exponent)) * .5);
        return t2 < 1 ? static_cast<float>(.5 - power(1 - t2, exponent) * .5)
                      : static_cast<float>(power(t2 - 1, exponent) * .5 + .5);
    }
    if (mode == 15) return 0;
    if (mode == 16) return 1;
    auto sine = [](float angle) { return static_cast<float>(std::sin(double(angle))); };
    constexpr float kHalfPi = 1.57079637050628662109375f;
    constexpr float kPi = 3.1415927410125732421875f;
    if (mode == 18) return sine(static_cast<float>(static_cast<float>(t * kPi) * .5f));
    if (mode == 19) return static_cast<float>(1 - sine(static_cast<float>(static_cast<float>(t * kPi) * .5f) + kHalfPi));
    if (mode == 20) {
        t = static_cast<float>(t * 2);
        const float s = sine(static_cast<float>(static_cast<float>(t * kPi) * .5f));
        return t < 1 ? static_cast<float>(s * .5f) : static_cast<float>(static_cast<float>(1 - s) * .5f + .5f);
    }
    if (mode == 21) {
        t = static_cast<float>(t * 2);
        if (t < 1)
            return static_cast<float>(static_cast<float>(1 - sine(static_cast<float>(static_cast<float>(t * kPi) * .5f) + kHalfPi)) * .5f);
        return static_cast<float>(.5f + static_cast<float>(sine(static_cast<float>(static_cast<float>(t - 1) * kPi) * .5f) * .5f));
    }
    if (mode >= 22 && mode <= 31) {
        constexpr float shift[] = {.25f, .300000011920928955078125f, .3499999940395355224609375f, .37999999523162841796875f, .4000000059604644775390625f};
        constexpr float divisor[] = {.5625f, .4899999797344207763671875f, .422499954700469970703125f, .38440001010894775390625f, .36000001430511474609375f};
        constexpr float subtract[] = {.111111111938953399658203125f, .18367348611354827880859375f, .28994083404541015625f, .375650346279144287109375f, .4444444477558135986328125f};
        constexpr float scale[] = {.888888895511627197265625f, .81632649898529052734375f, .71005916595458984375f, .624349653720855712890625f, .5555555820465087890625f};
        const u32 i = u32((mode - 22) % 5);
        // 同 0x473a40 单次落位链：二次曲线族中间量 double。
        double v = mode >= 27 ? 1 - double(t) : double(t);
        v = v - shift[i];
        v = v * v;
        v = v / divisor[i];
        v = v - subtract[i];
        v = v / scale[i];
        return mode >= 27 ? static_cast<float>(1 - v) : static_cast<float>(v);
    }
    return t;
}

float EclInterpolation::step(float rate) noexcept {
    if (duration > 0) {
        timer.tick(&rate);
        if (timer.current >= duration) {
            timer.set(duration);
            duration = 0;
        }
    }
    return evaluate();
}

float EclInterpolation::evaluate() noexcept {
    if (duration == 0) return mode == 7 || mode == 17 ? start : end;
    if (mode == 7 || mode == 17) {
        start = static_cast<float>(start + (mode == 7 ? end : control2));
        current = start;
        if (mode == 17) control2 = static_cast<float>(control2 + end);
        return current;
    }
    if (mode == 8) {  // Hermite：指令 92 的控制点在参数 6/7。
        const float t = static_cast<float>(timer.fractional / static_cast<float>(duration));
        const float twice = static_cast<float>(t * 2);
        const float a = static_cast<float>(t - 1);
        const float b = static_cast<float>(1 - t);
        const float h00 = static_cast<float>(static_cast<float>(a * a) * static_cast<float>(twice + 1));
        const float h11 = static_cast<float>(static_cast<float>(a * t) * t);
        const float h01 = static_cast<float>(static_cast<float>(3 - twice) * static_cast<float>(t * t));
        const float h10 = static_cast<float>(b * b * t);
        const float p = static_cast<float>(start * h00);
        const float q = static_cast<float>(end * h01);
        const float r = static_cast<float>(control1 * h10);
        const float s = static_cast<float>(control2 * h11);
        current = static_cast<float>(static_cast<float>(q + p) + static_cast<float>(r + s));
        return current;
    }
    const float weight = interpolation_weight(mode, timer.fractional, static_cast<float>(duration));
    current = static_cast<float>(start + static_cast<float>(static_cast<float>(end - start) * weight));
    return current;
}

}  // namespace th13
