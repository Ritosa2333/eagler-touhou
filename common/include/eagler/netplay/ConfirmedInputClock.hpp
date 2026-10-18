#pragma once
#include <cstdint>

namespace Netplay {
// Permission to sample a NEW logical slot while an exact-input phase waits.
// The adapter still owns frame numbers, immutable samples and transport sends.
// Combat keeps its ordinary capture frontier. A queued UI tail drains after
// resume instead of becoming a permanent extra gameplay input delay.
class ConfirmedInputClock {
public:
    static constexpr std::uint32_t MaxLead = 8;
    static constexpr std::uint64_t IntervalUs = 1'000'000 / 60;
    static constexpr std::uint64_t ClockToleranceUs = 250;

    void Reset() noexcept { due_ = 0; armed_ = false; }
    void BeginPhase(std::uint64_t nowUs) noexcept { due_ = nowUs + IntervalUs; armed_ = true; }
    bool CanCapture(std::uint32_t capture, std::uint32_t simulation,
                    bool confirmedPhase, std::uint64_t nowUs) const noexcept {
        if (capture == simulation) return true;
        if (capture < simulation || !confirmedPhase ||
            capture - simulation >= MaxLead || !armed_) return false;
        return nowUs >= due_ || due_ - nowUs <= ClockToleranceUs;
    }
    void Captured(std::uint64_t nowUs) noexcept {
        // Preserve fractional cadence across normal rAF jitter. A suspended
        // callback does not manufacture missed samples or duplicate edges.
        if (!armed_ || (nowUs < due_ && due_ - nowUs > ClockToleranceUs) ||
            (nowUs > due_ && nowUs - due_ >= IntervalUs)) due_ = nowUs;
        due_ += IntervalUs;
        armed_ = true;
    }
private:
    std::uint64_t due_ = 0;
    bool armed_ = false;
};
}
