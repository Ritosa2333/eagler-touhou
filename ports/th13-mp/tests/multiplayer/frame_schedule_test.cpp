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
    assert(stage_pending && stage_handoffs == 0);
    assert(CompleteConfirmedStageHandoff(stage_pending, 35, 35, handoff));
    assert(!stage_pending && stage_handoffs == 1);
    assert(CompleteConfirmedStageHandoff(stage_pending, 35, 35, handoff));
    assert(stage_handoffs == 1);
    assert(!CompleteConfirmedStageHandoff(true, 35, 35, [] { return false; }));
    FrameClock clock;
    assert(clock.elapsed(1000) == 0);
    assert(clock.elapsed(1016) == .016);
    for (unsigned packet = 0; packet < 100; ++packet) assert(clock.elapsed(1016) == 0);
    assert(clock.elapsed(1010) == 0);
    assert(clock.elapsed(1020) == .004);
    clock.reset();
    assert(clock.elapsed(2000) == 0);
    FrameSchedule shared;
    assert(shared.advance(clock.elapsed(2017)) == 1);
    shared.complete();
    assert(shared.advance(clock.elapsed(2017)) == 0);
    assert(shared.advance(clock.elapsed(2050)) == 2);
    shared.complete(); shared.complete();
    assert(shared.advance(clock.elapsed(2050)) == 0);
    constexpr double tick = 1. / 60.;
    FrameSchedule schedule;
    assert(schedule.advance(0) == 0);
    assert(!schedule.has_work());
    assert(schedule.advance(tick) == 1);
    for (unsigned replay = 0; replay < 12; ++replay) {
        schedule.cpu_yield();
        assert(schedule.advance(0) == 1);
    }
    schedule.complete();
    assert(schedule.advance(0) == 0);
    for (unsigned packet = 0; packet < 100; ++packet)
        assert(schedule.advance(0) == 0);
    assert(schedule.advance(tick) == 1);
    for (unsigned packet = 0; packet < 12; ++packet) {
        schedule.network_wait();
        assert(schedule.retry_pending());
        assert(schedule.advance(.1) == 1);
    }
    schedule.complete();
    assert(schedule.advance(0) == 0);
    assert(schedule.advance(tick * 2) == 2);
    schedule.complete();
    schedule.cpu_yield();
    assert(schedule.advance(0) == 1);
    schedule.complete();
    assert(schedule.advance(0) == 0);
    assert(schedule.advance(tick) == 1);
    schedule.network_wait();
    schedule.reset();
    assert(!schedule.retry_pending());
    assert(schedule.advance(0) == 0);

    using th13::multiplayer::SlowdownGovernor;
    assert(SlowdownGovernor::depth_of(10, 0xFFFFFFFFu) == 0);
    assert(SlowdownGovernor::depth_of(10, 9) == 1);
    assert(SlowdownGovernor::depth_of(10, 12) == 0);
    SlowdownGovernor governor;
    for (unsigned callback = 0; callback < 200; ++callback)
        assert(governor.update(SlowdownGovernor::kOnset) == 1.);
    for (unsigned callback = 0; callback < 40; ++callback)
        assert(governor.update(callback % 3 == 0 ? SlowdownGovernor::kWindow : 2) == 1.);
    double previous = 1.;
    for (unsigned callback = 0; callback < 200; ++callback) {
        const double stretch = governor.update(SlowdownGovernor::kFull);
        assert(stretch >= previous && stretch <= SlowdownGovernor::kMaxStretch);
        previous = stretch;
    }
    assert(previous > SlowdownGovernor::kMaxStretch - .01);
    unsigned released = 0;
    for (unsigned callback = 0; callback < 40; ++callback)
        if (governor.update(SlowdownGovernor::kWindow) < SlowdownGovernor::kMaxStretch - .4)
            ++released;
    assert(released > 0);
    governor.reset();
    const unsigned half = SlowdownGovernor::kOnset + 1;
    const double expected = 1. + (SlowdownGovernor::kMaxStretch - 1.) /
        double(SlowdownGovernor::kFull - SlowdownGovernor::kOnset);
    for (unsigned callback = 0; callback < 300; ++callback) governor.update(half);
    assert(governor.stretch() > expected - .01 && governor.stretch() < expected + .01);
    assert(expected > 1.2 && expected < SlowdownGovernor::kMaxStretch - .2);
    previous = governor.stretch();
    for (unsigned callback = 0; callback < 400; ++callback) {
        const double stretch = governor.update(0);
        assert(stretch <= previous);
        previous = stretch;
    }
    assert(governor.stretch() == 1.);
    governor.reset();
    for (unsigned callback = 0; callback < 300; ++callback) governor.update(SlowdownGovernor::kFull);
    FrameSchedule slowed;
    unsigned earned = 0;
    for (unsigned callback = 0; callback < 600; ++callback) {
        const unsigned owed = slowed.advance(tick / governor.stretch());
        for (unsigned i = 0; i < owed; ++i) { slowed.complete(); ++earned; }
    }
    assert(earned > 600 / SlowdownGovernor::kMaxStretch - 3 && earned < 600 / SlowdownGovernor::kMaxStretch + 3);
    FrameSchedule gap;
    assert(gap.advance_gap(0.5) == 30);
    gap.network_wait();
    assert(gap.owed() == 0);
    assert(gap.advance_gap(0) == 30);
    for (unsigned i = 0; i < 30; ++i) gap.complete();
    assert(gap.advance_gap(0) == 0);
    assert(gap.advance_gap(3.) == 120);
    gap.reset();
    assert(gap.advance_gap(0) == 0);
}
