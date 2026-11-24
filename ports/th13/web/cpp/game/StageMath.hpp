#pragma once

#include "Types.hpp"

#include <cmath>

namespace th13 {

inline float wrap_angle(float value) {
    const double kPi = 3.1415927410125732;
    const double kTwoPi = 6.2831854820251465;
    double v = value;
    int up = 0;
    while (kPi < v) {
        v = static_cast<float>(v - kTwoPi);
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

inline float mirror_angle(float value) {
    return wrap_angle(
        static_cast<float>(1.5707963705062866 - wrap_angle(static_cast<float>(value - 1.5707963705062866))));
}

inline void polar_xy(float angle, float radius, float& out_x, float& out_y) {
    out_x = static_cast<float>(std::cos(static_cast<double>(angle)) * static_cast<double>(radius));
    out_y = static_cast<float>(std::sin(static_cast<double>(angle)) * static_cast<double>(radius));
}

inline float round001(float value) {
    const double scaled = static_cast<double>(value * 100.0f);
    return static_cast<float>(std::floor(scaled)) / 100.0f;
}

inline float ease_interp(i32 mode, float elapsed, float duration) {
    const float t = elapsed / duration;
    const double x = t;
    const auto quad = [](double v, double shift, double divisor, double subtract, double scale) {
        return static_cast<float>(((v - shift) * (v - shift) / divisor - subtract) / scale);
    };
    switch (mode) {
    case 1:
        return static_cast<float>(x * x);
    case 2:
        return static_cast<float>(x * x * x);
    case 3:
        return static_cast<float>(x * x * x * x);
    case 4:
        return static_cast<float>(1.0 - (1.0 - x) * (1.0 - x));
    case 5: {
        const double y = 1.0 - x;
        return static_cast<float>(1.0 - y * y * y);
    }
    case 6: {
        const double y = 1.0 - x;
        return static_cast<float>(1.0 - y * y * y * y);
    }
    case 0xa: {
        const double y = double(t * 2.0f);
        if (y < 1.0) return static_cast<float>(y * y * y * 0.5);
        const double z = 2.0 - y;
        return static_cast<float>((2.0 - z * z * z) * 0.5);
    }
    case 0xb: {
        const double y = double(t * 2.0f);
        if (y < 1.0) return static_cast<float>(y * y * y * y * 0.5);
        const double z = 2.0 - y;
        return static_cast<float>((2.0 - z * z * z * z) * 0.5);
    }
    case 0xc: {
        const double y = double(t + t);
        if (y < 1.0) return static_cast<float>(0.5 - (1.0 - y) * (1.0 - y) * 0.5);
        return static_cast<float>((y - 1.0) * (y - 1.0) * 0.5 + 0.5);
    }
    case 0xd: {
        const double y = double(t + t);
        if (y < 1.0) {
            const double z = 1.0 - y;
            return static_cast<float>(0.5 - z * z * z * 0.5);
        }
        const double z = y - 1.0;
        return static_cast<float>(z * z * z * 0.5 + 0.5);
    }
    case 0xe: {
        const double y = double(t + t);
        if (y < 1.0) {
            const double z = 1.0 - y;
            return static_cast<float>(0.5 - z * z * z * 0.5);
        }
        const double z = y - 1.0;
        return static_cast<float>(z * z * z * z * 0.5 + 0.5);
    }
    case 0xf:
        return 0.0f;
    case 0x10:
        return 1.0f;
    case 18:
        return float(std::sin(double(float(x * double(3.1415927f) * .5))));
    case 19: {
        const float s = float(std::sin(double(float(x * double(3.1415927f) * .5 + double(1.5707964f)))));
        return float(1.0 - double(s));
    }
    case 20: {
        const float twice = float(t * 2);
        const float s = float(std::sin(double(float(double(twice) * double(3.1415927f) * .5))));
        return twice < 1 ? float(double(s) * .5) : float((1.0 - double(s)) * .5 + .5);
    }
    case 21: {
        const float twice = float(t * 2);
        const float a = twice < 1 ? float(double(twice) * double(3.1415927f) * .5 + double(1.5707964f))
                                  : float((double(twice) - 1) * double(3.1415927f) * .5);
        const float s = float(std::sin(double(a)));
        return twice < 1 ? float((1.0 - double(s)) * .5) : float(double(s) * .5 + .5);
    }
    case 0x16:
        return quad(x, 0.25, 0.5625, 0.11111111f, 0.8888889f);
    case 0x17:
        return quad(x, 0.3f, 0.48999998f, 0.18367349f, 0.8163265f);
    case 0x18:
        return quad(x, 0.35f, 0.42249995f, 0.28994083f, 0.71005917f);
    case 0x19:
        return quad(x, 0.38f, 0.3844f, 0.37565035f, 0.62434965f);
    case 0x1a:
        return quad(x, 0.4f, 0.36f, 0.44444445f, 0.5555556f);
    case 0x1b: {
        const double u = double(1.0f - t);
        return static_cast<float>(1.0 - (((u - 0.25) * (u - 0.25)) / 0.5625 - 0.11111111f) / 0.8888889f);
    }
    case 0x1c: {
        const double u = double(1.0f - t);
        return static_cast<float>(1.0 - (((u - 0.3f) * (u - 0.3f)) / 0.48999998f - 0.18367349f) / 0.8163265f);
    }
    case 0x1d: {
        const double u = double(1.0f - t);
        return static_cast<float>(1.0 - (((u - 0.35f) * (u - 0.35f)) / 0.42249995f - 0.28994083f) / 0.71005917f);
    }
    case 0x1e: {
        const double u = double(1.0f - t);
        return static_cast<float>(1.0 - (((u - 0.38f) * (u - 0.38f)) / 0.3844f - 0.37565035f) / 0.62434965f);
    }
    case 0x1f: {
        const double u = double(1.0f - t);
        return static_cast<float>(1.0 - (((u - 0.4f) * (u - 0.4f)) / 0.36f - 0.44444445f) / 0.5555556f);
    }
    default:
        break;
    }
    if (mode != 9) return t;
    const double y = double(t * 2.0f);
    if (y < 1.0) return static_cast<float>(y * y * 0.5);
    return static_cast<float>((2.0 - (2.0 - y) * (2.0 - y)) * 0.5);
}

}
