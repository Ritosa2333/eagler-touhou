#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace eagler::presentation {
// Existing owner samples are (previous, current). Weight 1 is the latest
// authoritative pose; 1 + phase predicts only the remaining display fraction.
// A late callback or a network wait never predicts beyond one logical tick.
inline float ExtrapolationWeight(double phase) noexcept {
    return std::isfinite(phase) ? 1.f + float(std::clamp(phase, 0., 1.)) : 1.f;
}

// Some multiplayer clocks consume a due slot even when no new native frame
// completes. Track the published endpoint so repeated waits cannot restart a
// prediction at phase zero every 60 Hz slot.
class ExtrapolationClock {
    double phase_ = 0;
    std::uint64_t frame_ = 0, revision_ = 0;
    bool valid_ = false;
public:
    void reset() noexcept { phase_ = 0; valid_ = false; }
    float observe(double delta, std::uint64_t frame, std::uint64_t revision,
                  double forward_phase) noexcept {
        if (!valid_ || revision != revision_ || frame < frame_) phase_ = 0;
        else if (frame != frame_) phase_ = std::isfinite(forward_phase)
            ? std::clamp(forward_phase, 0., 1.) : 0.;
        else if (std::isfinite(delta)) phase_ = std::clamp(phase_ + std::max(0., delta) * 60., 0., 1.);
        valid_ = true; frame_ = frame; revision_ = revision;
        return ExtrapolationWeight(phase_);
    }
};
}
