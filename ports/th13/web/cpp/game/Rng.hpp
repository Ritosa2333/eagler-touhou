#pragma once

#include <cstdint>

namespace th13 {

class Rng {
public:
    explicit Rng(uint32_t seed = 1) : state_(seed) {}

    void reseed(uint32_t seed) { state_ = seed; }

    int next() {
        state_ = state_ * kMultiplier + kIncrement;
        return static_cast<int>((state_ >> 16) & 0x7FFF);
    }

    uint32_t state() const { return state_; }

    static constexpr uint32_t kMultiplier = 0x343FDu;
    static constexpr uint32_t kIncrement = 0x269EC3u;

private:
    uint32_t state_;
};

extern bool rng_probe_on;

class GameRandom {
public:
    inline static uint32_t probe[20] = {};
    static void tag(int i) {
        if (rng_probe_on && i >= 0 && i < 20) ++probe[i];
    }
    using DrawObserver = void (*)(uint32_t before, uint32_t after);
    inline static DrawObserver draw_observer = nullptr;
    using InstanceDrawObserver = void (*)(const GameRandom*, uint32_t, uint32_t);
    inline static InstanceDrawObserver instance_draw_observer = nullptr;
    explicit GameRandom(uint16_t seed = 0) : seed_(seed) {}

    void reseed(uint16_t seed) { seed_ = seed; }

    void reseed_restore(uint16_t seed) {
        seed_ = seed;
        index_ = 0;
    }

    uint32_t next() {
        if (draw_observer) draw_observer(index_, index_ + 2);
        if (instance_draw_observer) instance_draw_observer(this, index_, index_ + 2);
        const uint16_t a = static_cast<uint16_t>((seed_ ^ 0x9630u) - 0x6553u);
        const uint16_t step = static_cast<uint16_t>((a >> 14) + a * 4);
        const uint16_t b = static_cast<uint16_t>((step ^ 0x9630u) - 0x6553u);
        seed_ = static_cast<uint16_t>((b >> 14) + b * 4);
        index_ += 2;
        return (static_cast<uint32_t>(a) << 16) | b;
    }

    uint16_t step16() {
        if (draw_observer) draw_observer(index_, index_ + 1);
        if (instance_draw_observer) instance_draw_observer(this, index_, index_ + 1);
        const uint16_t a = static_cast<uint16_t>((seed_ ^ 0x9630u) - 0x6553u);
        seed_ = static_cast<uint16_t>((a >> 14) + a * 4);
        index_ += 1;
        return seed_;
    }

    float signed_unit() {
        const double raw = static_cast<double>(next());
        return static_cast<float>(raw * (1.0 / 2147483648.0) - 1.0);
    }

    uint32_t next32() { return next(); }
    float unit() {
        const double raw = static_cast<double>(next());
        return static_cast<float>(raw * (1.0 / 4294967296.0));
    }

    uint32_t index() const { return index_; }
    uint16_t seed() const { return seed_; }

private:
    uint16_t seed_;
    uint32_t index_ = 0;
};

}
