#pragma once
#include "GameRandom.hpp"
#include "Timer.hpp"

namespace th13 {
// 45fb10(kind1) / 45f7c0 and 45f8c0. Each request has its own
// priority16 callback; kind1 uses ANM RNG, kind8 uses game RNG.
struct ScreenShake {
    i32 duration, start, end;
    bool envelope = false;
    i32 amplitude = 0, rise = 0, hold = 0, fall = 0;
    Timer age;

    ScreenShake(i32 frames, i32 first, i32 last)
        : duration(frames), start(first), end(last) { age.set(0); }

    static ScreenShake bomb(i32 magnitude, i32 attack, i32 sustain, i32 decay) {
        ScreenShake result(0, 0, 0);
        result.envelope = true;
        result.amplitude = magnitude; result.rise = attack;
        result.hold = sustain; result.fall = decay;
        return result;
    }

    bool advance(GameRandom& random, float rate, Vec2& offset) {
        age.tick(&rate);
        float magnitude;
        if (envelope) {
            // 45f8c0 stores the envelope once, then multiplies it by the
            // integer amplitude and stores the product before either RNG call.
            float factor;
            if (age.current < rise) factor = float(double(age.fractional) / double(rise));
            else if (age.current < rise + hold) factor = 1.f;
            else {
                const i32 total = rise + hold + fall;
                if (age.current >= total) return false;
                const float remaining = float(double(total) - double(age.fractional));
                factor = float(double(remaining) / double(fall));
            }
            magnitude = float(double(amplitude) * double(factor));
        } else {
            if (age.current >= duration) return false;
            // 45f7c0 integer operands and three explicit f32 stores.
            const i32 delta = i32(u32(end) - u32(start));
            const float product = float(double(delta) * double(age.fractional));
            const float quotient = float(double(product) / double(duration));
            magnitude = float(double(quotient) + double(start));
        }
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
