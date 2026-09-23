#include "../th10_web/cpp/multiplayer/CooperativeRules.hpp"
#include <eagler/netplay/NetplayCore.hpp>
#include <eagler/netplay/RollbackJournal.hpp>

#include <array>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <tuple>

using namespace th10::multiplayer;

namespace {
constexpr std::uint16_t focus = 4;
constexpr std::uint32_t correctionFrame = 89;
constexpr std::uint32_t arrivalFrame = 96;
constexpr std::uint32_t totalFrames = 110;

State initial_state() {
    State state{};
    const SeatSetup seats[]{{0, 0, 2, 0}, {1, 2, 2, 40}};
    assert(Initialize(state, seats, 2));
    assert(ReportNativeSeatOutcome(state, 0, LifeState::Spirit, -1, 0));
    return state;
}

FrameInput rule_input(std::uint16_t remoteButtons) {
    FrameInput input{};
    input.seats[0] = {0, 10000, false, false, false, false};
    input.seats[1] = {1000, 10000, true, true, (remoteButtons & focus) != 0, false};
    return input;
}

auto seat_values(const SeatState& seat) {
    return std::make_tuple(seat.lives, seat.power, seat.character, seat.shot,
                          seat.lifeState, seat.rescueTicks, seat.rescueTarget,
                          seat.waitingForFocusRelease);
}

void compare(const State& expected, const State& actual, std::uint32_t frame) {
    bool equal = expected.seatCount == actual.seatCount &&
                 expected.wipeTicks == actual.wipeTicks &&
                 expected.retryPending == actual.retryPending;
    for (std::uint8_t seat = 0; seat < kMaxSeats; ++seat)
        equal = equal && seat_values(expected.seats[seat]) == seat_values(actual.seats[seat]);
    if (!equal) {
        std::fprintf(stderr, "cooperative owner diverged after frame %u\n", frame);
        std::abort();
    }
}
}

int main() {
    // A missing release on the 90th rescue tick predicts a rescue that never
    // happened. Correcting it must restore the donor life, recipient state,
    // release latch and progress, not merely their displayed positions.
    std::array<State, totalFrames> expected{};
    State reference = initial_state();
    for (std::uint32_t frame = 0; frame < totalFrames; ++frame) {
        AdvanceOneTick(reference, rule_input(frame == correctionFrame ? 0 : focus));
        expected[frame] = reference;
    }

    Netplay::RollbackCore core;
    Netplay::CoreConfig config;
    config.sessionId = 42;
    config.playerCount = 2;
    config.localPlayer = 0;
    config.maxRollbackFrames = 8;
    config.predictableButtons = focus;
    assert(core.Reset(config));
    Netplay::RollbackJournal journal;
    assert(journal.Reset({8, sizeof(State), 1}));
    State actual = initial_state();
    const auto simulate = [&](std::uint32_t frame) {
        const auto decision = core.PrepareFrame(frame);
        assert(decision.canAdvance);
        assert(journal.BeginFrame(frame));
        assert(journal.Touch(&actual, sizeof(actual)));
        AdvanceOneTick(actual, rule_input(decision.inputs[1].buttons));
        assert(journal.EndFrame());
        assert(core.MarkSimulated(frame, decision));
    };

    for (std::uint32_t frame = 0; frame < totalFrames; ++frame) {
        assert(core.ScheduleLocalInput(frame, std::uint16_t{0}));
        if (frame != correctionFrame)
            assert(core.SubmitRemoteInput(1, frame, focus) == Netplay::RemoteInputResult::Accepted);
        simulate(frame);
        if (frame == correctionFrame) {
            assert(actual.seats[0].lifeState == LifeState::Alive);
            assert(actual.seats[1].lives == 1);
            assert(expected[frame].seats[0].lifeState == LifeState::Spirit);
        }
        if (frame == arrivalFrame) {
            assert(core.SubmitRemoteInput(1, correctionFrame, std::uint16_t{0}) ==
                   Netplay::RemoteInputResult::RollbackRequired);
            assert(core.RollbackFrame() == correctionFrame);
            std::uint32_t restoredFrame = Netplay::INVALID_FRAME;
            assert(journal.UndoTo(core.RollbackFrame(), &restoredFrame));
            assert(restoredFrame == correctionFrame);
            core.ClearRollbackRequest();
            for (std::uint32_t replay = restoredFrame; replay <= frame; ++replay) {
                // No ScheduleLocalInput or device sampling during replay.
                simulate(replay);
                compare(expected[replay], actual, replay);
            }
        }
        if (frame < correctionFrame || frame >= arrivalFrame)
            compare(expected[frame], actual, frame);
    }
    assert(!journal.Failed());
    assert(core.ConfirmedThroughAllRemotes() == totalFrames - 1);
    std::puts("TH10 cooperative owner + eagler-common late-input rollback: PASS");
}
