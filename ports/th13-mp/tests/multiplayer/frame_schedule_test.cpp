#include "../../web/cpp/multiplayer/FrameSchedule.hpp"
#include <cassert>

int main() {
    using th13::multiplayer::FrameSchedule;
    using th13::multiplayer::FrameClock;
    using th13::multiplayer::CompleteConfirmedStageHandoff;
    bool stage_pending = true;
    unsigned stage_handoffs = 0;
    const auto handoff = [&] { stage_pending = false; ++stage_handoffs; return true; };
    assert(CompleteConfirmedStageHandoff(false, 35, 35, handoff));
    assert(CompleteConfirmedStageHandoff(stage_pending, 34, 35, handoff));
    assert(stage_pending && stage_handoffs == 0); // Retained records keep old resources.
    assert(CompleteConfirmedStageHandoff(stage_pending, 35, 35, handoff));
    assert(!stage_pending && stage_handoffs == 1); // Confirmed clear applies in correction.
    assert(CompleteConfirmedStageHandoff(stage_pending, 35, 35, handoff));
    assert(stage_handoffs == 1); // Packet/RAF retries cannot repeat the stage door.
    assert(!CompleteConfirmedStageHandoff(true, 35, 35, [] { return false; }));
    FrameClock clock;
    assert(clock.elapsed(1000) == 0);
    assert(clock.elapsed(1016) == .016);
    for (unsigned packet = 0; packet < 100; ++packet) assert(clock.elapsed(1016) == 0);
    assert(clock.elapsed(1010) == 0); // Older RAF timestamp cannot rewind time.
    assert(clock.elapsed(1020) == .004);
    clock.reset();
    assert(clock.elapsed(2000) == 0); // New generation consumes no old credit.
    FrameSchedule shared;
    assert(shared.advance(clock.elapsed(2017)) == 1); // Wake samples before RAF.
    shared.complete();
    assert(shared.advance(clock.elapsed(2017)) == 0); // Same-time RAF does not resample.
    assert(shared.advance(clock.elapsed(2050)) == 2); // Replay wall time earns real credit.
    shared.complete(); shared.complete();
    assert(shared.advance(clock.elapsed(2050)) == 0);
    constexpr double tick = 1. / 60.;
    FrameSchedule schedule;
    assert(schedule.advance(0) == 0);
    assert(!schedule.has_work());
    assert(schedule.advance(tick) == 1);
    // A correction can yield repeatedly; the due NEW frame is still owed.
    for (unsigned replay = 0; replay < 12; ++replay) {
        schedule.cpu_yield();
        assert(schedule.advance(0) == 1);
    }
    schedule.complete();
    assert(schedule.advance(0) == 0);
    // Receive bursts without elapsed time cannot manufacture logic frames.
    for (unsigned packet = 0; packet < 100; ++packet)
        assert(schedule.advance(0) == 0);
    // A missing remote input retains exactly one retry, including during a
    // long network wait. Once received it can run immediately, without RAF.
    assert(schedule.advance(tick) == 1);
    for (unsigned packet = 0; packet < 12; ++packet) {
        schedule.network_wait();
        assert(schedule.retry_pending());
        assert(schedule.advance(.1) == 1);
    }
    schedule.complete();
    assert(schedule.advance(0) == 0);
    // CPU budgeting preserves all already offered work, with the normal
    // title's bounded debt. It must not silently discard a second due input.
    assert(schedule.advance(tick * 2) == 2);
    schedule.complete();
    schedule.cpu_yield();
    assert(schedule.advance(0) == 1);
    schedule.complete();
    assert(schedule.advance(0) == 0);
    assert(schedule.advance(tick) == 1);
    schedule.network_wait();
    schedule.reset(); // Exit/reconfigure discards work owned by the old run.
    assert(!schedule.retry_pending());
    assert(schedule.advance(0) == 0);

    // Frame-rate-drop handling: stretch follows the sustained prediction depth.
    using th13::multiplayer::SlowdownGovernor;
    assert(SlowdownGovernor::depth_of(10, 0xFFFFFFFFu) == 0);  // no remote input yet
    assert(SlowdownGovernor::depth_of(10, 9) == 1);
    assert(SlowdownGovernor::depth_of(10, 12) == 0);
    SlowdownGovernor governor;
    for (unsigned callback = 0; callback < 200; ++callback)
        assert(governor.update(SlowdownGovernor::kOnset) == 1.);  // ordinary latency never slows
    // A burst of depth that a single callback clears is not pressure.
    for (unsigned callback = 0; callback < 40; ++callback)
        assert(governor.update(callback % 3 == 0 ? SlowdownGovernor::kWindow : 2) == 1.);
    // Sustained depth just short of the window slows the clock to the cap.
    double previous = 1.;
    for (unsigned callback = 0; callback < 200; ++callback) {
        const double stretch = governor.update(SlowdownGovernor::kFull);
        assert(stretch >= previous && stretch <= SlowdownGovernor::kMaxStretch);
        previous = stretch;
    }
    assert(previous > SlowdownGovernor::kMaxStretch - .01);
    // Depth pinned on the window while already fully slowed is both peers
    // display-bound. The stretch releases instead of holding the cap.
    unsigned released = 0;
    for (unsigned callback = 0; callback < 40; ++callback)
        if (governor.update(SlowdownGovernor::kWindow) < SlowdownGovernor::kMaxStretch - .4)
            ++released;
    assert(released > 0);
    // Partial pressure converges to a proportional stretch, not the cap.
    governor.reset();
    const unsigned half = SlowdownGovernor::kOnset + 1;
    const double expected = 1. + (SlowdownGovernor::kMaxStretch - 1.) /
        double(SlowdownGovernor::kFull - SlowdownGovernor::kOnset);
    for (unsigned callback = 0; callback < 300; ++callback) governor.update(half);
    assert(governor.stretch() > expected - .01 && governor.stretch() < expected + .01);
    assert(expected > 1.2 && expected < SlowdownGovernor::kMaxStretch - .2);
    // Releasing pressure returns smoothly to the true clock.
    previous = governor.stretch();
    for (unsigned callback = 0; callback < 400; ++callback) {
        const double stretch = governor.update(0);
        assert(stretch <= previous);
        previous = stretch;
    }
    assert(governor.stretch() == 1.);
    // A stretched clock earns proportionally fewer frames and never freezes.
    governor.reset();
    for (unsigned callback = 0; callback < 300; ++callback) governor.update(SlowdownGovernor::kFull);
    FrameSchedule slowed;
    unsigned earned = 0;
    for (unsigned callback = 0; callback < 600; ++callback) {
        const unsigned owed = slowed.advance(tick / governor.stretch());
        for (unsigned i = 0; i < owed; ++i) { slowed.complete(); ++earned; }
    }
    assert(earned > 600 / SlowdownGovernor::kMaxStretch - 3 && earned < 600 / SlowdownGovernor::kMaxStretch + 3);
    // A long display gap is credited in full, and frames the callback does not
    // run remain available. Time beyond two seconds becomes slowdown.
    FrameSchedule gap;
    assert(gap.advance_gap(0.5) == 30);
    gap.network_wait();
    assert(gap.owed() == 0);
    assert(gap.advance_gap(0) == 30); // The unrun frames are still owed.
    for (unsigned i = 0; i < 30; ++i) gap.complete();
    assert(gap.advance_gap(0) == 0);
    assert(gap.advance_gap(3.) == 120); // Two seconds of credit, not the whole hitch.
    gap.reset();
    assert(gap.advance_gap(0) == 0);
}
