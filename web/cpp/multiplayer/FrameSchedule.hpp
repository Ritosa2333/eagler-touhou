#pragma once
#include "../../../portable/sdl/FrameCadence.hpp"
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
// Only elapsed clock time earns new frames. Both display and packet callbacks
// can spend due credit; replaying history never spends a fresh frame's credit.
class FrameSchedule {
    using Cadence = touhou::sdl::FrameCadence;
    Netplay::PendingFrameSchedule<Cadence> schedule_;
    unsigned offered_ = 0;
public:
    void reset() { schedule_.reset(); offered_ = 0; }
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
    void complete() { --offered_; schedule_.complete(); }
    void network_wait() {
        if (!offered_) return;
        schedule_.blocked(true);
        offered_ = 0;
    }
    void cpu_yield() {
        if (!offered_) return;
        schedule_.complete();
        schedule_.debt = std::min(.1, schedule_.debt + offered_ * Cadence::interval);
        offered_ = 0;
    }
};
}
