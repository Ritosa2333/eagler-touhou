#pragma once
#include "../../../portable/sdl/FrameCadence.hpp"
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <eagler/netplay/PendingFrameSchedule.hpp>

namespace th13::multiplayer {
// A corrected clear can precede the saved forward frontier. Apply its
// confirmed stage door before the next replayed frame, without presenting
// historical images or replacing resources while records still retain them.
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
// Frame-rate-drop handling for rollback play, the netplay counterpart of the
// original loop's slowdown: when the remote seats cannot keep up, the local
// clock stretches in proportion to the sustained prediction depth, so the game
// slows smoothly and rollbacks stay short instead of the prediction window
// filling and the game stalling. Wall-clock advice only; inputs, prediction,
// rollback and world state never depend on it.
class SlowdownGovernor {
public:
    static constexpr unsigned kWindow = 12;      // SessionSetup::MaxRollbackFrames
    static constexpr unsigned kOnset = 8;        // sustained depth where stretching starts
    static constexpr unsigned kFull = 11;        // sustained depth at the full stretch
    static constexpr double kMaxStretch = 3.;    // clock speed floor 20 Hz
    static constexpr double kAttack = .35, kRelease = .08;
    static constexpr std::size_t kSpan = 6;      // callbacks in the sustained-depth envelope

    void reset() { *this = SlowdownGovernor{}; }
    double stretch() const { return stretch_; }
    unsigned sustained_depth() const { return sustained_; }

    // depth = RollbackCore::PrepareFrame's distance for the next frame: how far
    // it lies beyond the newest input confirmed by every remote.
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
        // Depth parked on the prediction window while the clock is already at
        // its slowest is symmetric pressure: both peers are display-bound, and
        // slowing both does not close the gap. Release so the original catch-up
        // can spend the display gap. A peer that is genuinely ahead sees its
        // depth fall once it slows, and never takes this branch.
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

// Only elapsed clock time earns new frames. Both display and packet callbacks
// can spend due credit; replaying history never spends a fresh frame's credit.
class FrameSchedule {
    using Cadence = touhou::sdl::FrameCadence;
    Netplay::PendingFrameSchedule<Cadence> schedule_;
    unsigned offered_ = 0;
public:
    void reset() { schedule_.reset(); offered_ = 0; gap_cap_ = 0; }
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
    // Credit the whole display gap, the way the original 1/60 catch-up loop does.
    // A gap longer than kGapDebtCap becomes slowdown rather than a catch-up
    // spiral; frames the prediction window cannot run yet stay credited.
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
    void complete() { --offered_; schedule_.complete(); }
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
