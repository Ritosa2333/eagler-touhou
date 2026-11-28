#pragma once
#include "Types.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>

namespace th13 {
inline i32 encode_spell_elapsed_time(double elapsed) noexcept {
    if (!std::isfinite(elapsed) || elapsed < 0) return 0;
    const double remainder = std::fmod(elapsed, 0.0167);
    double rounded = elapsed - remainder;
    if (remainder >= 0.00835) rounded += 0.0167;
    const double whole = std::floor(rounded);
    const i32 seconds = i32(std::min(999.0, whole));
    const i32 centiseconds = i32((rounded - whole) * 100.0);
    return (((seconds + 22 + centiseconds) * 1000 + (seconds + 66) % 1000) * 100 + (centiseconds + 33) % 100);
}

inline bool spell_time_packed_valid(i32 packed) noexcept {
    return packed / 100000 - 0x16 == ((packed / 100) % 1000 + 0x3a6) % 1000 + (packed % 100 + 0x43) % 100;
}

struct SpellTimeClock {
    struct End {
        i32 index = 0;
        double seconds = 0;
    };
    void begin(std::chrono::steady_clock::time_point now) noexcept {
        if (active_) return;
        start_ = now;
        active_ = true;
    }
    bool end(std::chrono::steady_clock::time_point now, End& out) noexcept {
        if (!active_) return false;
        active_ = false;
        out.index = index_++;
        out.seconds = std::chrono::duration<double>(now - start_).count();
        return true;
    }
    void reset() noexcept {
        active_ = false;
        index_ = 0;
    }
    i32 index() const noexcept { return index_; }
    bool active() const noexcept { return active_; }

private:
    std::chrono::steady_clock::time_point start_;
    bool active_ = false;
    i32 index_ = 0;
};
}
