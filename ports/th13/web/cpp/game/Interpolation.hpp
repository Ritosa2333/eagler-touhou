// th13 ECL 插值通道与缓动权重。
// 通道对象固定 0x30 字节：原版线程状态块（§2.1，docs/plans/ecl-vm.md）在 +0x1020 起
// 内联 8×0x30 通道（0x417450 初始化 8 组；解释器 0x4772a0 case 0x5b/0x5c 挂接），
// 通道内部字节布局未逐字段定域，此处为行为等价的紧凑布局，供 EclFile 快照落盘。
// 缓动模式族沿用 th13 世代 ZUN 共享表（th15 Interpolation.cpp 已对齐的同族实现，
// 待 oracle 对照轮逐模式验证）。
#pragma once

#include "Types.hpp"
#include "Timer.hpp"

#include <array>

namespace th13 {

// 标量插值通道（ECL 指令 91/92 挂接；0x30 字节见文件头说明）。
// 通道字段 28 字节 + Timer 20 字节 = 0x30，凑满原版线程状态块内联布局。
struct EclInterpolation {
    i32 duration = 0;
    i32 mode = 0;
    float start = 0, end = 0, control1 = 0, control2 = 0, current = 0;
    Timer timer{};

    void begin(i32 frames, i32 easing, float from, float to) noexcept {
        start = from;
        end = to;
        duration = frames;
        mode = easing;
        timer.set(0);
    }

    float step(float rate) noexcept;
    float evaluate() noexcept;
};
static_assert(sizeof(EclInterpolation) == 0x30);

// 缓动权重：mode 决定 [0,1] 进度的映射。
float interpolation_weight(i32 mode, float elapsed, float duration) noexcept;

// ANM 插值通道族（th13 与 th15 同代的 ZUN 共享实现；算术顺序保持原版，
// 插值改变的是碰撞/重放可见状态，不只是视效）。
template <u32 N, bool Angle = false>
struct FloatInterpolation {
    std::array<float, N> start{}, end{}, control1{}, control2{}, current{};
    Timer timer{0, 0, 0, 0, 0};
    i32 duration = 0, mode = 0;

    void begin(i32 frames, i32 easing, const std::array<float, N>& from,
               const std::array<float, N>& to) noexcept {
        start = from;
        end = to;
        duration = frames;
        mode = easing;
        timer.set(0);
    }

    std::array<float, N> step(float rate = 1) noexcept {
        if (duration > 0) {
            timer.tick(&rate);
            if (timer.current >= duration) {
                timer.set(duration);
                duration = 0;
            }
        }
        return evaluate();
    }

    std::array<float, N> evaluate() noexcept {
        if (duration == 0) return mode == 7 || mode == 17 ? start : end;
        if (mode == 7 || mode == 17) {
            for (u32 i = 0; i < N; i++) {
                start[i] = static_cast<float>(start[i] + (mode == 7 ? end[i] : control2[i]));
                if constexpr (Angle) start[i] = normalize_angle(start[i]);
                current[i] = start[i];
                if (mode == 17) {
                    control2[i] = static_cast<float>((Angle ? end[i] : control2[i]) +
                                                     (Angle ? control2[i] : end[i]));
                    if constexpr (Angle) control2[i] = normalize_angle(control2[i]);
                }
            }
            return current;
        }
        if (mode == 8) {
            const float t = static_cast<float>(timer.fractional / static_cast<float>(duration));
            const float twice = static_cast<float>(t * 2);
            const float a = static_cast<float>(t - 1);
            const float b = static_cast<float>(1 - t);
            const float h00 = static_cast<float>(static_cast<float>(a * a) * static_cast<float>(twice + 1));
            const float h11 = static_cast<float>(static_cast<float>(a * t) * t);
            const float h01 = static_cast<float>(static_cast<float>(3 - twice) * static_cast<float>(t * t));
            const float h10 = static_cast<float>(b * b * t);
            for (u32 i = 0; i < N; i++) {
                const float p = static_cast<float>(start[i] * h00);
                const float q = static_cast<float>(end[i] * h01);
                const float r = static_cast<float>(control1[i] * h10);
                const float s = static_cast<float>(control2[i] * h11);
                if constexpr (Angle)
                    current[i] = normalize_angle(static_cast<float>(
                        normalize_angle(static_cast<float>(
                            normalize_angle(static_cast<float>(normalize_angle(p) + normalize_angle(q))) +
                            normalize_angle(r))) + normalize_angle(s)));
                else if constexpr (N == 3)
                    current[i] = i ? static_cast<float>(s + static_cast<float>(r + static_cast<float>(p + q)))
                                   : static_cast<float>(static_cast<float>(r + static_cast<float>(p + q)) + s);
                else if constexpr (N == 1)
                    current[i] = static_cast<float>(static_cast<float>(static_cast<float>(q + p) + r) + s);
                else
                    current[i] = static_cast<float>(static_cast<float>(static_cast<float>(p + q) + r) + s);
            }
        } else {
            // [wf300 数值纪律·ANM 专属] oracle flip_probe 逐帧位级实证（wf289-302
            // 14/14 帧命中）：ANM 插值（00462120 族）权重为**逐步 f32 链**——
            // t→f32、u=1−t→f32、w=1−u·u→f32、diff×w→f32、+start→f32；与 ECL
            // 0x473a40 的 double 链（f159 实证）不同源，不得共用。实测差异点
            // 在 u=f32(1−t) 的先舍入（double 链漏 age12/13/8/2 四帧）。
            const float t = static_cast<float>(timer.fractional / static_cast<float>(duration));
            float weight;
            if constexpr (N == 2) {
                // 466812 calls473a40 for scale/UV-scale. Its polynomial
                // intermediates remain in x87 registers until the final
                // weight store; this differs from the vector3 path below.
                weight = interpolation_weight(mode, timer.fractional, static_cast<float>(duration));
            } else if (mode >= 4 && mode <= 6) {
                const float u = static_cast<float>(1 - t);
                float w = u;
                for (u32 k = 1; k < u32(mode - 2); k++) w = static_cast<float>(w * u);
                weight = static_cast<float>(1 - w);
            } else if (mode >= 1 && mode <= 3) {
                weight = t;
                for (u32 k = 1; k < u32(mode + 1); k++) weight = static_cast<float>(weight * t);
            } else {
                weight = interpolation_weight(mode, timer.fractional, static_cast<float>(duration));
            }
            for (u32 i = 0; i < N; i++) {
                float difference = static_cast<float>(end[i] - start[i]);
                if constexpr (Angle) {
                    if (difference > 3.1415927410125732421875f)
                        difference = static_cast<float>(end[i] - static_cast<float>(start[i] + 6.283185482025146484375f));
                    else if (static_cast<float>(start[i] - end[i]) > 3.1415927410125732421875f)
                        difference = static_cast<float>(end[i] - static_cast<float>(start[i] - 6.283185482025146484375f));
                    difference = normalize_angle(difference);
                    current[i] = normalize_angle(static_cast<float>(
                        start[i] + normalize_angle(static_cast<float>(difference * weight))));
                } else {
                    current[i] = static_cast<float>(start[i] + static_cast<float>(difference * weight));
                }
            }
        }
        return current;
    }
};

using ScalarInterpolation = FloatInterpolation<1>;
using Vec2Interpolation = FloatInterpolation<2>;
using Vec3Interpolation = FloatInterpolation<3>;
using AngleInterpolation = FloatInterpolation<1, true>;

template <u32 N>
struct IntegerInterpolation {
    std::array<i32, N> start{}, end{}, control1{}, control2{}, current{};
    Timer timer{0, 0, 0, 0, 0};
    i32 duration = 0, mode = 0;

    void begin(i32 frames, i32 easing, const std::array<i32, N>& from,
               const std::array<i32, N>& to) noexcept {
        start = from;
        end = to;
        duration = frames;
        mode = easing;
        timer.set(0);
    }

    std::array<i32, N> step(float rate = 1) noexcept {
        if (duration > 0) {
            timer.tick(&rate);
            if (timer.current >= duration) {
                timer.set(duration);
                duration = 0;
            }
        }
        if (duration == 0) return mode == 7 || mode == 17 ? start : end;
        if (mode == 7 || mode == 17) {
            for (u32 i = 0; i < N; i++) {
                start[i] = wrapping_add(start[i], mode == 7 ? end[i] : control2[i]);
                current[i] = start[i];
                if (mode == 17) control2[i] = wrapping_add(control2[i], end[i]);
            }
            return current;
        }
        if (mode == 8) {
            const float t = static_cast<float>(timer.fractional / static_cast<float>(duration));
            const float twice = static_cast<float>(t * 2);
            const float a = static_cast<float>(t - 1);
            const float b = static_cast<float>(1 - t);
            const float h00 = static_cast<float>(static_cast<float>(twice + 1) * static_cast<float>(a * a));
            const float h11 = static_cast<float>(static_cast<float>(a * t) * t);
            const float h01 = static_cast<float>(static_cast<float>(3 - twice) * static_cast<float>(t * t));
            const float h10 = static_cast<float>(b * b * t);
            for (u32 i = 0; i < N; i++) {
                const float p = static_cast<float>(static_cast<float>(start[i]) * h00);
                const float q = static_cast<float>(static_cast<float>(end[i]) * h01);
                const float r = static_cast<float>(static_cast<float>(control1[i]) * h10);
                const float s = static_cast<float>(static_cast<float>(control2[i]) * h11);
                if constexpr (N == 1)
                    current[i] = truncate_int(static_cast<float>(
                        static_cast<float>(static_cast<float>(q + p) + r) + s));
                else
                    current[i] = wrapping_add(wrapping_add(wrapping_add(truncate_int(p), truncate_int(q)),
                                                          truncate_int(r)),
                                              truncate_int(s));
            }
        } else {
            const float weight = interpolation_weight(mode, timer.fractional, static_cast<float>(duration));
            for (u32 i = 0; i < N; i++) {
                const float delta =
                    static_cast<float>(static_cast<float>(wrapping_sub(end[i], start[i])) * weight);
                if constexpr (N == 1)
                    current[i] = truncate_int(static_cast<float>(delta + static_cast<float>(start[i])));
                else
                    current[i] = wrapping_add(start[i], truncate_int(delta));
            }
        }
        return current;
    }
};

using AlphaInterpolation = IntegerInterpolation<1>;
using ColorInterpolation = IntegerInterpolation<3>;

}  // namespace th13
