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
}
