#include "Interpolation.hpp"

#include <cmath>

namespace th13 {

float interpolation_weight(i32 mode, float elapsed, float duration) noexcept {
    if (duration == 0 || std::isnan(duration)) return 1;
    float t = static_cast<float>(elapsed / duration);
    auto power = [](double v, u32 exponent) {
        double r = v;
        for (u32 i = 1; i < exponent; i++)
            r = r * v;
        return r;
    };
    if (mode >= 1 && mode <= 3) return static_cast<float>(power(double(t), u32(mode + 1)));
    if (mode >= 4 && mode <= 6) return static_cast<float>(1 - power(1 - double(t), u32(mode - 2)));
    if (mode >= 9 && mode <= 14) {
        const u32 exponent = u32((mode - 9) % 3 + 2);
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
    if (mode == 19)
        return static_cast<float>(1 - sine(static_cast<float>(static_cast<float>(t * kPi) * .5f) + kHalfPi));
    if (mode == 20) {
        t = static_cast<float>(t * 2);
        const float s = sine(static_cast<float>(static_cast<float>(t * kPi) * .5f));
        return t < 1 ? static_cast<float>(s * .5f) : static_cast<float>(static_cast<float>(1 - s) * .5f + .5f);
    }
    if (mode == 21) {
        t = static_cast<float>(t * 2);
        if (t < 1)
            return static_cast<float>(
                static_cast<float>(1 - sine(static_cast<float>(static_cast<float>(t * kPi) * .5f) + kHalfPi)) * .5f);
        return static_cast<float>(
            .5f + static_cast<float>(sine(static_cast<float>(static_cast<float>(t - 1) * kPi) * .5f) * .5f));
    }
    if (mode >= 22 && mode <= 31) {
        constexpr float shift[] = {.25f, .300000011920928955078125f, .3499999940395355224609375f,
                                   .37999999523162841796875f, .4000000059604644775390625f};
        constexpr float divisor[] = {.5625f, .4899999797344207763671875f, .422499954700469970703125f,
                                     .38440001010894775390625f, .36000001430511474609375f};
        constexpr float subtract[] = {.111111111938953399658203125f, .18367348611354827880859375f,
                                      .28994083404541015625f, .375650346279144287109375f, .4444444477558135986328125f};
        constexpr float scale[] = {.888888895511627197265625f, .81632649898529052734375f, .71005916595458984375f,
                                   .624349653720855712890625f, .5555555820465087890625f};
        const u32 i = u32((mode - 22) % 5);
        double v = mode >= 27 ? double(static_cast<float>(1 - t)) : double(t);
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
    if (duration == 0) return end;
    if (mode == 7 || mode == 17) {
        start = static_cast<float>(start + (mode == 7 ? end : control2));
        current = start;
        if (mode == 17) control2 = static_cast<float>(control2 + end);
        return current;
    }
    if (mode == 8) {
        const long double t = static_cast<long double>(timer.fractional / static_cast<float>(duration));
        const long double reverse = static_cast<long double>(1) - t;
        const long double forward = t - static_cast<long double>(1);
        const long double square = t * t;
        const float h00 = static_cast<float>(forward * forward * (t + t + static_cast<long double>(1)));
        const float h11 = static_cast<float>(t * (forward * t));
        const float h01 = static_cast<float>((static_cast<long double>(3) - t - t) * square);
        const float h10 = static_cast<float>(reverse * reverse * t);
        const long double p = static_cast<long double>(start) * h00;
        const long double q = static_cast<long double>(end) * h01;
        const long double r = static_cast<long double>(control1) * h10;
        const long double s = static_cast<long double>(control2) * h11;
        current = static_cast<float>(q + p + r + s);
        return current;
    }
    const float weight = interpolation_weight(mode, timer.fractional, static_cast<float>(duration));
    current = static_cast<float>(static_cast<long double>(end - start) * weight + start);
    return current;
}

}
