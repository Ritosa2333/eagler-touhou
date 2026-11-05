#include "../web/cpp/multiplayer/CooperativeRules.hpp"
#include "../web/cpp/multiplayer/InputLanes.hpp"
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
constexpr std::uint32_t correctionFrame = 15;
constexpr std::uint32_t arrivalFrame = 22;
constexpr std::uint32_t totalFrames = 36;

struct Model {
    State rules;
    InputLanes::State lanes{};
};

std::uint16_t buttons(std::uint32_t frame) {
    return frame == 0 || frame == 3 || frame == 6 || frame == 9 ||
           frame == correctionFrame ? focus : 0;
}

State initial_state() {
    State state{};
    const SeatSetup seats[]{{0, 0, 2, 0}, {1, 2, 2, 40}};
    assert(Initialize(state, seats, 2));
    assert(ReportNativeSeatOutcome(state, 0, LifeState::Spirit, -1, 0));
    return state;
}

FrameInput rule_input(InputLanes::State& lanes, const Netplay::FrameInput* captured) {
    assert(InputLanes::Commit(lanes, captured, 2));
    const auto& remote = lanes.seats[1];
    FrameInput input{};
    input.seats[0] = {0, 10000, false, false, false, false, false, false, false};
    input.seats[1] = {1000, 10000, true, true, true,
                     (remote.current & focus) != 0, false, false,
                     (remote.pressed & focus) != 0};
    return input;
}

auto seat_values(const SeatState& seat) {
    return std::make_tuple(seat.lives, seat.power, seat.character, seat.shot,
                          seat.lifeState, seat.rescueTicks, seat.rescueTarget,
                          seat.waitingForFocusRelease);
}

void compare(const Model& expectedModel, const Model& actualModel, std::uint32_t frame) {
    const auto& expected = expectedModel.rules;
    const auto& actual = actualModel.rules;
    bool equal = expected.seatCount == actual.seatCount &&
                 expected.wipeTicks == actual.wipeTicks &&
                 expected.retryPending == actual.retryPending;
    for (std::uint8_t seat = 0; seat < kMaxSeats; ++seat) {
        equal = equal && seat_values(expected.seats[seat]) == seat_values(actual.seats[seat]);
        const auto& left = expectedModel.lanes.seats[seat];
        const auto& right = actualModel.lanes.seats[seat];
        equal = equal && left.current == right.current && left.previous == right.previous &&
                left.pressed == right.pressed && left.released == right.released &&
                left.raw == right.raw && left.raw_pressed == right.raw_pressed;
    }
    if (!equal) {
        std::fprintf(stderr, "cooperative owner diverged after frame %u\n", frame);
        std::abort();
    }
}
}

int main() {
    // Four confirmed Focus taps precede a missing fifth press. Prediction
    // keeps the previous released input, so rescue waits for the correction.
    // Replay restores both the policy state and the real per-seat edge owner.
    std::array<Model, totalFrames> expected{};
    Model reference{initial_state()};
    for (std::uint32_t frame = 0; frame < totalFrames; ++frame) {
        std::array<Netplay::FrameInput, 2> captured{};
        captured[1] = Netplay::FrameInput(buttons(frame));
        AdvanceOneTick(reference.rules, rule_input(reference.lanes, captured.data()));
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
    assert(journal.Reset({8, sizeof(Model), 1}));
    Model actual{initial_state()};
    const auto simulate = [&](std::uint32_t frame) {
        const auto decision = core.PrepareFrame(frame);
        assert(decision.canAdvance);
        assert(journal.BeginFrame(frame));
        assert(journal.Touch(&actual, sizeof(actual)));
        AdvanceOneTick(actual.rules, rule_input(actual.lanes, decision.inputs.data()));
        assert(journal.EndFrame());
        assert(core.MarkSimulated(frame, decision));
    };

    for (std::uint32_t frame = 0; frame < totalFrames; ++frame) {
        assert(core.ScheduleLocalInput(frame, std::uint16_t{0}));
        if (frame != correctionFrame)
            assert(core.SubmitRemoteInput(1, frame, buttons(frame)) == Netplay::RemoteInputResult::Accepted);
        simulate(frame);
        if (frame == correctionFrame) {
            assert(actual.rules.seats[0].lifeState == LifeState::Spirit);
            assert(actual.rules.seats[1].lives == 2);
            assert(expected[frame].rules.seats[0].lifeState == LifeState::Alive);
            assert(expected[frame].rules.seats[1].lives == 1);
        }
        if (frame == arrivalFrame) {
            assert(core.SubmitRemoteInput(1, correctionFrame, focus) ==
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
    std::puts("TH10 five-Focus rescue + real input-edge owner + late-input rollback: PASS");
}
