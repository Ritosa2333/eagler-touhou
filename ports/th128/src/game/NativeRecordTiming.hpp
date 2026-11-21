#pragma once
#include <cmath>
#include <cstdint>
#include <optional>
#include <stdexcept>

namespace th128::record {

struct NativeFrameStatisticsState {
    double lastWindowSeconds = 0;
    std::uint32_t counter = 0;
    std::uint32_t overspeedWindows = 0;
    float fps = 0;
    double numerator = 0;
    double denominator = 0;
};

class NativeRecordTiming {
    NativeFrameStatisticsState state_;

public:
    void setRecordedCounters(double numerator, double denominator) {
        if (!std::isfinite(numerator) || !std::isfinite(denominator)
            || numerator < 0 || denominator < 0)
            throw std::runtime_error("timing record counters are outside the finite owned domain");
        state_.numerator = numerator;
        state_.denominator = denominator;
    }

    bool sample(double seconds, bool hasSession, std::uint32_t& sessionFlags,
                std::uint8_t originalCounterByte) {
        if (!std::isfinite(seconds) || seconds < 0)
            throw std::runtime_error("clock sample is outside the finite owned domain");
        if (seconds < state_.lastWindowSeconds) state_.lastWindowSeconds = seconds;
        const auto elapsed = seconds - state_.lastWindowSeconds;
        const bool advanced = elapsed >= 1.0;
        if (advanced) {
            state_.lastWindowSeconds += elapsed;
            state_.fps = static_cast<float>(static_cast<double>(state_.counter) / elapsed);
            if (state_.fps > 65.0f) ++state_.overspeedWindows;
            else state_.overspeedWindows = 0;
            if (hasSession) {
                if (!(sessionFlags & 0x14u)) {
                    state_.denominator += 60.0;
                    state_.numerator += state_.fps > 57.0f ? 60.0 : static_cast<double>(state_.fps);
                }
                sessionFlags &= ~0x80u;
            }
            state_.counter = 0;
        }
        state_.counter += 1u + originalCounterByte;
        return advanced;
    }

    const NativeFrameStatisticsState& state() const noexcept { return state_; }

    std::optional<float> slowdownPercentage() const {
        if (state_.denominator == 0) return std::nullopt;
        const auto ratio = static_cast<float>(state_.numerator / state_.denominator);
        const auto percentage = static_cast<float>(100.0 - 100.0 * static_cast<double>(ratio));
        if (!std::isfinite(percentage))
            throw std::runtime_error("timing percentage is outside the finite owned domain");
        return percentage;
    }

    static std::uint64_t time64FromFileTime(std::uint64_t filetime) noexcept {
        const auto shifted = filetime - std::uint64_t{116444736000000000};
        const auto seconds = shifted / std::uint64_t{10000000};
        return seconds <= std::uint64_t{0x793406fff} ? seconds : UINT64_MAX;
    }
};

}
