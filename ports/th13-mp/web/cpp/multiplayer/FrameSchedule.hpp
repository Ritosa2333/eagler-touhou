#pragma once
#include "../../../portable/sdl/FrameCadence.hpp"
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <eagler/netplay/PendingFrameSchedule.hpp>

namespace th13::multiplayer {
template<class Apply>
bool CompleteConfirmedStageHandoff(bool pending, unsigned committed,
                                   unsigned simulated, Apply apply) {
    return !pending || committed != simulated || apply();
}

class FrameClock {
    double previous_ = -1;
public:
    void reset() { previous_ = -1; }
    double elapsed(double now) {
        if (previous_ < 0) { previous_ = now; return 0; }
        if (now <= previous_) return 0;
        const double delta = (now - previous_) / 1000.;
        previous_ = now;
        return delta;
    }
};
class SlowdownGovernor {
public:
    static constexpr unsigned kWindow = 12;
    static constexpr unsigned kOnset = 8;
    static constexpr unsigned kFull = 11;
    static constexpr double kMaxStretch = 3.;
    static constexpr double kAttack = .35, kRelease = .08;
    static constexpr std::size_t kSpan = 6;

    void reset() { *this = SlowdownGovernor{}; }
    double stretch() const { return stretch_; }
    unsigned sustained_depth() const { return sustained_; }

    static unsigned depth_of(std::uint32_t next_frame, std::uint32_t confirmed) {
        return confirmed == 0xFFFFFFFFu || next_frame <= confirmed ? 0 : next_frame - confirmed;
    }

    double update(unsigned depth) {
        recent_[cursor_] = std::min(depth, kWindow);
        cursor_ = (cursor_ + 1) % kSpan;
        count_ = std::min(count_ + 1, kSpan);
        unsigned lowest = recent_[0];
        for (std::size_t i = 1; i < count_; ++i) lowest = std::min(lowest, recent_[i]);
        sustained_ = count_ == kSpan ? lowest : 0;
        const double pressure = sustained_ <= kOnset ? 0.
            : std::min(1., double(sustained_ - kOnset) / double(kFull - kOnset));
        const double target = 1. + pressure * (kMaxStretch - 1.);
        stretch_ += (target - stretch_) * (target > stretch_ ? kAttack : kRelease);
        if (stretch_ < 1.0005) stretch_ = 1.;
        if (sustained_ >= kWindow && stretch_ >= kMaxStretch - .05) ++pinned_;
        else pinned_ = 0;
        if (pinned_ > kSpan) {
            stretch_ = 1. + (stretch_ - 1.) * .5;
            pinned_ = 0;
        }
        return stretch_;
    }

private:
    std::array<unsigned, kSpan> recent_{};
    std::size_t cursor_ = 0, count_ = 0;
    unsigned sustained_ = 0, pinned_ = 0;
    double stretch_ = 1.;
};

class FrameSchedule {
    using Cadence = touhou::sdl::FrameCadence;
    Netplay::PendingFrameSchedule<Cadence> schedule_;
    unsigned offered_ = 0, completed_ = 0;
public:
    void reset() { schedule_.reset(); offered_ = 0; gap_cap_ = 0; }
    void align_phase() { schedule_.debt = Cadence::interval / 2; }
    double credit() const { return schedule_.debt; }
    unsigned take_completed() { const unsigned done = completed_; completed_ = 0; return done; }
    unsigned owed() const { return offered_; }
    bool retry_pending() const { return schedule_.retry_pending(); }
    bool has_work() const {
        return offered_ || retry_pending() || schedule_.debt + 1.e-9 >= Cadence::interval;
    }
    unsigned advance(double elapsed) {
        cpu_yield();
        offered_ = schedule_.advance(elapsed, true);
        return offered_;
    }
    static constexpr double kGapDebtCap = 2.;
    unsigned advance_gap(double elapsed) {
        gap_cap_ = kGapDebtCap;
        cpu_yield();
        const double seconds = std::clamp(elapsed, 0., kGapDebtCap);
        schedule_.debt = std::min(kGapDebtCap, schedule_.debt + seconds);
        if (schedule_.retry_pending()) schedule_.complete();
        offered_ = unsigned(std::floor((schedule_.debt + 1.e-9) / Cadence::interval));
        schedule_.debt = std::max(0., schedule_.debt - offered_ * Cadence::interval);
        return offered_;
    }
    void complete() { --offered_; ++completed_; schedule_.complete(); }
    void network_wait() {
        if (!offered_) return;
        if (gap_cap_ > 0) {
            schedule_.debt = std::min(gap_cap_, schedule_.debt + offered_ * Cadence::interval);
            offered_ = 0;
            return;
        }
        schedule_.blocked(true);
        offered_ = 0;
    }
    void cpu_yield() {
        if (!offered_) return;
        schedule_.complete();
        const double cap = gap_cap_ > 0 ? gap_cap_ : .1;
        schedule_.debt = std::min(cap, schedule_.debt + offered_ * Cadence::interval);
        offered_ = 0;
    }
private:
    double gap_cap_ = 0;
};
}
