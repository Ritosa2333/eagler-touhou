#pragma once
#include <cmath>

namespace Netplay {
// TH09's pending-frame rule, parameterized by the title's ordinary clock.
// A network wait retries one already-due frame without new wall-time credit.
// Input capture and logical frame ownership remain with the game adapter.
template<class Cadence>
class PendingFrameSchedule : public Cadence {
    bool retry_ = false;
public:
    void reset() { Cadence::reset(); retry_ = false; }
    bool retry_pending() const { return retry_; }
    unsigned advance(double seconds, bool live = false) {
        if (!live) retry_ = false;
        return retry_ ? 1 : Cadence::advance(seconds);
    }
    void blocked(bool live) {
        if (!live) { reset(); return; }
        retry_ = true;
        this->debt = std::fmod(this->debt, Cadence::interval);
    }
    void complete() { retry_ = false; }
};
}
