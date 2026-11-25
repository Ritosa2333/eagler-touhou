#pragma once

#include "Timer.hpp"
#include "Types.hpp"

#include <array>

namespace th13 {

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

float interpolation_weight(i32 mode, float elapsed, float duration) noexcept;

template <u32 N, bool Angle = false> struct FloatInterpolation {
    std::array<float, N> start{}, end{}, control1{}, control2{}, current{};
    Timer timer{0, 0, 0, 0, 0};
    i32 duration = 0, mode = 0;

    void begin(i32 frames, i32 easing, const std::array<float, N>& from, const std::array<float, N>& to) noexcept {
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
                    control2[i] = static_cast<float>((Angle ? end[i] : control2[i]) + (Angle ? control2[i] : end[i]));
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
                            normalize_angle(r))) +
                        normalize_angle(s)));
                else if constexpr (N == 3)
                    current[i] = i ? static_cast<float>(s + static_cast<float>(r + static_cast<float>(p + q)))
                                   : static_cast<float>(static_cast<float>(r + static_cast<float>(p + q)) + s);
                else if constexpr (N == 1)
                    current[i] = static_cast<float>(static_cast<float>(static_cast<float>(q + p) + r) + s);
                else
                    current[i] = static_cast<float>(static_cast<float>(static_cast<float>(p + q) + r) + s);
            }
        } else {
            const float weight = interpolation_weight(mode, timer.fractional, static_cast<float>(duration));
            for (u32 i = 0; i < N; i++) {
                float difference = static_cast<float>(end[i] - start[i]);
                if constexpr (Angle) {
                    if (difference > 3.1415927410125732421875f)
                        difference =
                            static_cast<float>(end[i] - static_cast<float>(start[i] + 6.283185482025146484375f));
                    else if (static_cast<float>(start[i] - end[i]) > 3.1415927410125732421875f)
                        difference =
                            static_cast<float>(end[i] - static_cast<float>(start[i] - 6.283185482025146484375f));
                    difference = normalize_angle(difference);
                    current[i] = normalize_angle(
                        static_cast<float>(start[i] + normalize_angle(static_cast<float>(difference * weight))));
                } else {
                    current[i] = static_cast<float>(static_cast<long double>(end[i] - start[i]) * weight + start[i]);
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

template <u32 N> struct IntegerInterpolation {
    std::array<i32, N> start{}, end{}, control1{}, control2{}, current{};
    Timer timer{0, 0, 0, 0, 0};
    i32 duration = 0, mode = 0;

    void begin(i32 frames, i32 easing, const std::array<i32, N>& from, const std::array<i32, N>& to) noexcept {
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
                    current[i] =
                        truncate_int(static_cast<float>(static_cast<float>(static_cast<float>(q + p) + r) + s));
                else
                    current[i] = wrapping_add(
                        wrapping_add(wrapping_add(truncate_int(p), truncate_int(q)), truncate_int(r)), truncate_int(s));
            }
        } else {
            const float weight = interpolation_weight(mode, timer.fractional, static_cast<float>(duration));
            for (u32 i = 0; i < N; i++) {
                if constexpr (N == 1) {
                    const double delta =
                        static_cast<double>(wrapping_sub(end[i], start[i])) * static_cast<double>(weight);
                    current[i] = truncate_int(static_cast<double>(start[i]) + delta);
                } else {
                    const float delta = static_cast<float>(static_cast<float>(wrapping_sub(end[i], start[i])) * weight);
                    current[i] = wrapping_add(start[i], truncate_int(delta));
                }
            }
        }
        return current;
    }
};

using AlphaInterpolation = IntegerInterpolation<1>;
using ColorInterpolation = IntegerInterpolation<3>;

}
