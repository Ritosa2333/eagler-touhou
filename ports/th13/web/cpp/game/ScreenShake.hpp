#pragma once
#include "GameRandom.hpp"
#include "Timer.hpp"

namespace th13 {
// 45fb10(kind1) / 45f7c0. Each request has its own priority75 callback;
// it writes the shared background offset using the ANM random stream.
struct ScreenShake {
    i32 duration, start, end;
    Timer age;

    ScreenShake(i32 frames, i32 first, i32 last)
        : duration(frames), start(first), end(last) { age.set(0); }

    bool advance(GameRandom& random, float rate, Vec2& offset) {
        age.tick(&rate);
        if (age.current >= duration) return false;
        // x87 uses integer operands and three explicit f32 stores. Retain
        // integer precision before multiplication/division/addition.
        const i32 delta = i32(u32(end) - u32(start));
        const float product = float(double(delta) * double(age.fractional));
        const float quotient = float(double(product) / double(duration));
        const float magnitude = float(double(quotient) + double(start));
        const auto axis = [&]() {
            switch (random.next() % 3) {
            case 1: return magnitude;
            case 2: return -magnitude;
            default: return 0.f;
            }
        };
        offset.x = axis();
        offset.y = axis();
        // 45fc80 removes callbacks without zeroing the last offset.
        return true;
    }
};
} // namespace th13
