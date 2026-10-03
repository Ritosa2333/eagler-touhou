#include "../th10_web/cpp/multiplayer/CooperativeRules.hpp"

#include <cassert>
#include <cstdint>
#include <initializer_list>
#include <type_traits>

using namespace th10::multiplayer;

namespace {

struct ItemProbe {
    bool succeeds;
    std::uint8_t callCount;
    std::uint8_t givers[kMaxSeats];
    std::uint8_t targets[kMaxSeats];
};

bool allocate_item(void* context, std::uint8_t giver,
                   std::uint8_t target) noexcept {
    ItemProbe& probe = *static_cast<ItemProbe*>(context);
    const std::uint8_t index = probe.callCount++;
    if (index < kMaxSeats) {
        probe.givers[index] = giver;
        probe.targets[index] = target;
    }
    return probe.succeeds;
}

State make_state(std::uint8_t count) {
    State state{};
    SeatSetup setup[kMaxSeats]{{0, 0, 2, 0}, {1, 2, 2, 40}, {0, 1, 8, 60}};
    assert(Initialize(state, setup, count));
    return state;
}

FrameInput input_at(std::int32_t x0 = 0, std::int32_t x1 = 1000,
                    std::int32_t x2 = 0) {
    FrameInput input{};
    input.seats[0] = {x0, 10000, true, true, true, true, false, false};
    input.seats[1] = {x1, 10000, true, true, true, true, false, false};
    input.seats[2] = {x2, 10000, true, true, true, true, false, false};
    return input;
}

void spirit(State& state, std::uint8_t seat, std::int16_t lives = -1) {
    assert(ReportNativeSeatOutcome(state, seat, LifeState::Spirit, lives,
                                   state.seats[seat].power));
}

void no_teammate_in_range_resets_progress() {
    State state = make_state(2);
    spirit(state, 1);
    FrameInput input = input_at(0, 2001);
    for (int tick = 0; tick < 100; ++tick) {
        const TickResult result = AdvanceOneTick(state, input);
        assert(result.eventCount == 0);
    }
    assert(state.seats[0].rescueTicks == 0);
    assert(state.seats[0].lifeState == LifeState::Alive);
    assert(state.seats[1].lifeState == LifeState::Spirit);
}

void radius_is_inclusive_at_twenty_pixels() {
    State state = make_state(2);
    spirit(state, 1);
    FrameInput input = input_at(0, kRescueRadiusHundredths);
    TickResult result{};
    for (int tick = 0; tick < kRescueTicks; ++tick) {
        result = AdvanceOneTick(state, input);
    }
    assert(result.eventCount == 1);
    assert(result.events[0].kind == EventKind::SpiritRevived);
    assert(result.events[0].seat == 0 && result.events[0].targetSeat == 1);
    assert(state.seats[0].lives == 1);
    assert(state.seats[1].lives == 0);
    assert(state.seats[1].lifeState == LifeState::Alive);
    assert(state.seats[1].power == kMaxPower / 2);
    assert(state.seats[0].waitingForFocusRelease);
}

void repeated_inputs_advance_logical_ticks_and_focus_release_gates_next_use() {
    State state = make_state(2);
    spirit(state, 1);
    FrameInput input = input_at();
    for (int tick = 0; tick < kRescueTicks - 1; ++tick) {
        assert(AdvanceOneTick(state, input).eventCount == 0);
    }
    assert(state.seats[0].rescueTicks == kRescueTicks - 1);
    assert(AdvanceOneTick(state, input).events[0].kind ==
           EventKind::SpiritRevived);
    assert(AdvanceOneTick(state, input).eventCount == 0);
    assert(state.seats[0].waitingForFocusRelease);
    input.seats[0].focus = false;
    AdvanceOneTick(state, input);
    assert(!state.seats[0].waitingForFocusRelease);
    assert(state.seats[0].rescueTicks == 0);
}

void shoot_breaks_continuous_rescue_progress() {
    State state = make_state(2);
    spirit(state, 1);
    FrameInput input = input_at();
    for (int tick = 0; tick < 40; ++tick) {
        AdvanceOneTick(state, input);
    }
    input.seats[0].shoot = true;
    AdvanceOneTick(state, input);
    assert(state.seats[0].rescueTicks == 0);
    assert(state.seats[0].rescueTarget == -1);
}

void five_rapid_shoot_presses_transfer_one_native_big_power_item() {
    State state = make_state(2);
    assert(ReportNativeSeatOutcome(state, 0, LifeState::Alive, 2, 40));
    assert(ReportNativeSeatOutcome(state, 1, LifeState::Alive, 2, 0));
    FrameInput input = input_at();
    ItemProbe probe{true, 0, {}, {}};
    TickResult result{};
    for (std::uint8_t tap = 1; tap <= kPowerTapCount; ++tap) {
        input.seats[0].shoot = true;
        input.seats[0].shootPressed = true;
        result = AdvanceOneTick(state, input, {}, {&probe, allocate_item});
        if (tap < kPowerTapCount) {
            assert(result.eventCount == 0);
            assert(PowerTapProgress(state.seats[0]) == tap);
        }
        input.seats[0].shoot = false;
        input.seats[0].shootPressed = false;
        if (tap < kPowerTapCount) AdvanceOneTick(state, input, {}, {&probe, allocate_item});
    }
    assert(result.eventCount == 1);
    assert(result.events[0].kind == EventKind::PowerItemTransferCommitted);
    assert(result.events[0].seat == 0 && result.events[0].targetSeat == 1);
    assert(probe.callCount == 1 && probe.givers[0] == 0 && probe.targets[0] == 1);
    assert(state.seats[0].power == 20);
    assert(state.seats[1].power == 0);
    assert(PowerTapProgress(state.seats[0]) == 0);
}

void power_taps_expire_and_never_count_a_held_shot_twice() {
    State state = make_state(2);
    assert(ReportNativeSeatOutcome(state, 0, LifeState::Alive, 2, 40));
    assert(ReportNativeSeatOutcome(state, 1, LifeState::Alive, 2, 0));
    FrameInput input = input_at();
    input.seats[0].shoot = true;
    input.seats[0].shootPressed = true;
    assert(AdvanceOneTick(state, input).eventCount == 0);
    assert(PowerTapProgress(state.seats[0]) == 1);
    input.seats[0].shootPressed = false;
    for (int tick = 0; tick < 10; ++tick) AdvanceOneTick(state, input);
    assert(PowerTapProgress(state.seats[0]) == 1);
    input.seats[0].shoot = false;
    for (int tick = 10; tick < kPowerTapWindow; ++tick) AdvanceOneTick(state, input);
    assert(PowerTapProgress(state.seats[0]) == 0);
}

void power_transfer_rejects_full_or_spirit_recipients_and_failed_allocation() {
    State state = make_state(2);
    assert(ReportNativeSeatOutcome(state, 0, LifeState::Alive, 2, 40));
    assert(ReportNativeSeatOutcome(state, 1, LifeState::Alive, 2, kMaxPower));
    FrameInput input = input_at();
    input.seats[0].shoot = true;
    input.seats[0].shootPressed = true;
    AdvanceOneTick(state, input);
    assert(PowerTapProgress(state.seats[0]) == 0);

    // A transfer is exactly +1.00 Power.  Do not debit the donor when the
    // recipient has less than a full 1.00 of room before the native 5.00 cap.
    assert(ReportNativeSeatOutcome(state, 1, LifeState::Alive, 2,
                                   kMaxPowerTransferRecipient + 1));
    AdvanceOneTick(state, input);
    assert(PowerTapProgress(state.seats[0]) == 0);

    assert(ReportNativeSeatOutcome(state, 1, LifeState::Alive, 2, 0));
    ItemProbe probe{false, 0, {}, {}};
    for (int tap = 0; tap < kPowerTapCount; ++tap) {
        input.seats[0].shoot = true;
        input.seats[0].shootPressed = true;
        AdvanceOneTick(state, input, {}, {&probe, allocate_item});
        input.seats[0].shoot = false;
        input.seats[0].shootPressed = false;
        if (tap + 1 < kPowerTapCount) AdvanceOneTick(state, input, {}, {&probe, allocate_item});
    }
    assert(probe.callCount == 1 && state.seats[0].power == 40);

    spirit(state, 1);
    input.seats[0].shoot = true;
    input.seats[0].shootPressed = true;
    AdvanceOneTick(state, input, {}, {&probe, allocate_item});
    assert(PowerTapProgress(state.seats[0]) == 0);
}

void three_seat_life_items_commit_synchronously_and_independently() {
    State state = make_state(3);
    assert(ReportNativeSeatOutcome(state, 2, LifeState::Alive, 0, 60));
    FrameInput input = input_at(0, 1000, 500);
    input.seats[2].canInitiateLifeTransfer = false;
    input.seats[2].focus = false;
    ItemProbe probe{true, 0, {}, {}};
    const LifeItemAllocator allocator{&probe, allocate_item};
    TickResult result{};
    for (int tick = 0; tick < kRescueTicks; ++tick) {
        result = AdvanceOneTick(state, input, allocator);
    }
    assert(result.eventCount == 2);
    assert(result.events[0].kind == EventKind::LifeItemTransferCommitted);
    assert(result.events[0].seat == 0 && result.events[0].targetSeat == 2);
    assert(result.events[1].kind == EventKind::LifeItemTransferCommitted);
    assert(result.events[1].seat == 1 && result.events[1].targetSeat == 2);
    assert(probe.callCount == 2);
    assert(probe.givers[0] == 0 && probe.targets[0] == 2);
    assert(probe.givers[1] == 1 && probe.targets[1] == 2);
    assert(state.seats[0].lives == 1 && state.seats[1].lives == 1);
    assert(state.seats[0].waitingForFocusRelease &&
           state.seats[1].waitingForFocusRelease);
}

void failed_item_allocation_costs_no_life_and_retries_after_full_progress() {
    State state = make_state(3);
    FrameInput input = input_at(0, 3000, 1000);
    input.seats[1].canInitiateLifeTransfer = false;
    input.seats[1].focus = false;
    input.seats[2].canInitiateLifeTransfer = false;
    input.seats[2].focus = false;
    ItemProbe probe{false, 0, {}, {}};
    const LifeItemAllocator allocator{&probe, allocate_item};
    TickResult result{};
    for (int tick = 0; tick < kRescueTicks; ++tick) {
        result = AdvanceOneTick(state, input, allocator);
    }
    assert(result.eventCount == 0);
    assert(probe.callCount == 1);
    assert(state.seats[0].lives == 2);
    assert(!state.seats[0].waitingForFocusRelease);
    assert(state.seats[0].rescueTicks == 0);
    probe.succeeds = true;
    for (int tick = 0; tick < kRescueTicks - 1; ++tick) {
        assert(AdvanceOneTick(state, input, allocator).eventCount == 0);
    }
    result = AdvanceOneTick(state, input, allocator);
    assert(result.eventCount == 1);
    assert(result.events[0].kind == EventKind::LifeItemTransferCommitted);
    assert(state.seats[0].lives == 1);
    assert(probe.callCount == 2);
}

void each_seat_sees_prior_life_debits_when_selecting_a_recipient() {
    State state = make_state(3);
    assert(ReportNativeSeatOutcome(state, 2, LifeState::Alive, 2, 60));
    FrameInput input = input_at(0, 500, 1000);
    ItemProbe probe{true, 0, {}, {}};
    TickResult result{};
    for (int tick = 0; tick < kRescueTicks; ++tick) {
        result = AdvanceOneTick(state, input, {&probe, allocate_item});
    }
    assert(result.eventCount == 1);
    assert(probe.callCount == 1);
    assert(probe.givers[0] == 0 && probe.targets[0] == 2);
    assert(state.seats[0].lives == 1);
    assert(state.seats[1].lives == 2 && state.seats[2].lives == 2);
    // A prior donor's debit changes the following seats' selected recipient.
    // Changing recipients restarts their continuous 90-tick hold.
    assert(state.seats[1].rescueTarget == 0 && state.seats[1].rescueTicks == 1);
    assert(state.seats[2].rescueTarget == 0 && state.seats[2].rescueTicks == 1);
    for (int tick = 0; tick < kRescueTicks - 1; ++tick)
        result = AdvanceOneTick(state, input, {&probe, allocate_item});
    assert(result.eventCount == 1 && probe.callCount == 2);
    assert(probe.givers[1] == 1 && probe.targets[1] == 0);
    assert(state.seats[2].rescueTarget == 1 && state.seats[2].rescueTicks == 1);
}

void sequential_givers_keep_th07_rescue_frame_semantics() {
    State state = make_state(3);
    spirit(state, 2);
    FrameInput input = input_at(0, 500, 1000);
    TickResult result{};
    for (int tick = 0; tick < kRescueTicks - 1; ++tick) {
        result = AdvanceOneTick(state, input);
    }
    assert(state.seats[0].rescueTicks == kRescueTicks - 1);
    assert(state.seats[1].rescueTicks == kRescueTicks - 1);
    ItemProbe probe{true, 0, {}, {}};
    result = AdvanceOneTick(state, input, {&probe, allocate_item});
    assert(result.eventCount == 2);
    assert(result.events[0].kind == EventKind::SpiritRevived);
    assert(result.events[0].seat == 0 && result.events[0].targetSeat == 2);
    assert(result.events[1].kind == EventKind::LifeItemTransferCommitted);
    assert(result.events[1].seat == 1 && result.events[1].targetSeat == 2);
    assert(probe.callCount == 1);
    assert(probe.givers[0] == 1 && probe.targets[0] == 2);
    assert(state.seats[1].lives == 1);
}

void life_awards_require_explicit_status_and_clamp_to_th10_cap() {
    State state = make_state(2);
    assert(ReportNativeSeatOutcome(state, 1, LifeState::Eliminated, -1, 10));
    assert(ApplyLifeAward(state, 1, 1, LifeState::Eliminated));
    assert(state.seats[1].lives == 0);
    assert(state.seats[1].lifeState == LifeState::Eliminated);
    assert(ApplyLifeAward(state, 1, 1, LifeState::Alive));
    assert(state.seats[1].lives == 1);
    assert(state.seats[1].lifeState == LifeState::Alive);
    assert(ApplyLifeAward(state, 1, 20, LifeState::Alive));
    assert(state.seats[1].lives == kMaxLives);
}

void team_extend_can_bank_a_spirit_life_without_reviving_it() {
    State state = make_state(2);
    spirit(state, 1);
    assert(state.seats[1].lives == -1);
    assert(ApplyLifeAward(state, 1, 1, state.seats[1].lifeState));
    assert(state.seats[1].lives == 0);
    assert(state.seats[1].lifeState == LifeState::Spirit);
    assert(state.seats[1].rescueTicks == 0);
    assert(state.seats[1].rescueTarget == -1);
}

void next_stage_revives_every_seat_and_drops_transient_coop_state() {
    State state = make_state(3);
    assert(ReportNativeSeatOutcome(state, 0, LifeState::Dying, -1, 17));
    assert(ReportNativeSeatOutcome(state, 1, LifeState::Spirit, -1, 40));
    assert(ReportNativeSeatOutcome(state, 2, LifeState::Eliminated, 3, 60));
    state.seats[0].rescueTicks = 42;
    state.seats[0].rescueTarget = 1;
    state.seats[1].waitingForFocusRelease = true;
    // The Power gesture shares the rescue transient bytes using a negative
    // target token.  A stage boundary must clear this too before Replay takes
    // its stage-entry checkpoint.
    state.seats[2].rescueTicks = kPowerTapWindow;
    state.seats[2].rescueTarget = -4;
    state.wipeTicks = 99;
    state.retryPending = true;
    assert(BeginNextStage(state));
    assert(state.wipeTicks == 0 && !state.retryPending);
    assert(state.seats[0].lifeState == LifeState::Alive && state.seats[0].lives == 0);
    assert(state.seats[1].lifeState == LifeState::Alive && state.seats[1].lives == 0);
    assert(state.seats[2].lifeState == LifeState::Alive && state.seats[2].lives == 3);
    assert(state.seats[0].power == 17 && state.seats[1].power == 40 &&
           state.seats[2].power == 60);
    for (std::uint8_t seat = 0; seat < state.seatCount; ++seat) {
        assert(state.seats[seat].rescueTicks == 0);
        assert(state.seats[seat].rescueTarget == -1);
        assert(!state.seats[seat].waitingForFocusRelease);
    }
}

void spirit_alive_spirit_fullwipe_resets_then_retries_at_exactly_180_ticks() {
    State state = make_state(2);
    spirit(state, 0);
    assert(ReportNativeSeatOutcome(state, 1, LifeState::Eliminated, -1, 0));
    FrameInput input = input_at();
    for (int tick = 0; tick < 100; ++tick) {
        assert(AdvanceOneTick(state, input).eventCount == 0);
    }
    assert(state.wipeTicks == 100);

    assert(ReportNativeSeatOutcome(state, 0, LifeState::Alive, 0, 0));
    assert(state.wipeTicks == 0 && !state.retryPending);
    spirit(state, 0);
    for (int tick = 0; tick < kWipeRetryTicks - 1; ++tick) {
        assert(AdvanceOneTick(state, input).eventCount == 0);
    }
    assert(state.wipeTicks == kWipeRetryTicks - 1);
    TickResult result = AdvanceOneTick(state, input);
    assert(result.eventCount == 1);
    assert(result.events[0].kind == EventKind::WipeRetryRequested);
    assert(state.wipeTicks == kWipeRetryTicks && state.retryPending);
    assert(AdvanceOneTick(state, input).eventCount == 0);
    assert(state.wipeTicks == kWipeRetryTicks && state.retryPending);
}

void final_death_waits_for_native_transition_and_can_receive_a_team_extend() {
    State state = make_state(2);
    FrameInput input = input_at();
    assert(ReportNativeSeatOutcome(state, 1, LifeState::Dying, -1, 0));
    for (int tick = 0; tick < kRescueTicks; ++tick) {
        assert(AdvanceOneTick(state, input).eventCount == 0);
    }
    assert(state.seats[0].rescueTicks == 0);
    assert(state.seats[0].lives == 2);
    spirit(state, 0);
    for (int tick = 0; tick < 30; ++tick) {
        assert(AdvanceOneTick(state, input).eventCount == 0);
        assert(state.wipeTicks == 0);
    }
    assert(ApplyLifeAward(state, 1, 1, LifeState::Dying));
    assert(state.seats[1].lives == 0);
    assert(state.seats[1].lifeState == LifeState::Dying);
    assert(ReportNativeSeatOutcome(state, 1, LifeState::Alive, 0, 0));
    assert(state.wipeTicks == 0 && !state.retryPending);
    assert(ReportNativeSeatOutcome(state, 1, LifeState::Dying, -1, 0));
    spirit(state, 1);
    for (int tick = 0; tick < kWipeRetryTicks - 1; ++tick) {
        assert(AdvanceOneTick(state, input).eventCount == 0);
    }
    assert(AdvanceOneTick(state, input).events[0].kind == EventKind::WipeRetryRequested);
}

void invalid_rosters_and_loadouts_are_rejected() {
    State state{};
    const SeatSetup setup[kMaxSeats]{{0, 0, 2, 0}, {1, 2, 2, 100},
                                    {0, 1, 2, 100}};
    assert(!Initialize(state, setup, 1));
    assert(state.seatCount == 0);
    assert(!Initialize(state, setup, 4));
    assert(!Initialize(state, nullptr, 2));
    SeatSetup bad[kMaxSeats]{{2, 0, 2, 0}, {1, 2, 2, 0}, {0, 1, 2, 0}};
    assert(!Initialize(state, bad, 2));
    bad[0] = {0, 3, 2, 0};
    assert(!Initialize(state, bad, 2));
    assert(Initialize(state, setup, 3));
    assert(!ReportNativeSeatOutcome(state, 3, LifeState::Alive, 0, 0));
    assert(!ReportNativeSeatOutcome(state, 0, LifeState::Alive, -2, 0));
    assert(!ReportNativeSeatOutcome(state, 0, LifeState::Alive, -1, 0));
    assert(!ApplyLifeAward(state, 0, 0, LifeState::Alive));
}

} // namespace

void rescue_uses_fixed_resources_without_charging_donor_power() {
    for (std::uint8_t count : {std::uint8_t(2), std::uint8_t(3)})
    for (int banked : {-1, 0, 2, 9})
    for (int power = 0; power <= 100; ++power) {
        State state = make_state(count);
        state.seats[0].power = power;
        spirit(state, 1, banked);
        FrameInput input = input_at();
        input.seats[1].canInitiateLifeTransfer = false;
        input.seats[2].canInitiateLifeTransfer = false;
        ItemProbe probe{false, 0, {}, {}};
        TickResult result{};
        for (int tick = 0; tick < kRescueTicks; ++tick)
            result = AdvanceOneTick(state, input, {&probe, allocate_item});
        assert(result.eventCount == 1 && probe.callCount == 0);
        assert(result.events[0].kind == EventKind::SpiritRevived);
        assert(state.seats[0].lives == 1 && state.seats[0].power == power);
        assert(state.seats[1].lives == 0 && state.seats[1].power == kMaxPower / 2);
    }
}

int main() {
    static_assert(std::is_trivially_copyable<State>::value,
                  "state must be safe to copy into a rollback journal");
    no_teammate_in_range_resets_progress();
    radius_is_inclusive_at_twenty_pixels();
    repeated_inputs_advance_logical_ticks_and_focus_release_gates_next_use();
    shoot_breaks_continuous_rescue_progress();
    five_rapid_shoot_presses_transfer_one_native_big_power_item();
    rescue_uses_fixed_resources_without_charging_donor_power();
    power_taps_expire_and_never_count_a_held_shot_twice();
    power_transfer_rejects_full_or_spirit_recipients_and_failed_allocation();
    three_seat_life_items_commit_synchronously_and_independently();
    failed_item_allocation_costs_no_life_and_retries_after_full_progress();
    each_seat_sees_prior_life_debits_when_selecting_a_recipient();
    sequential_givers_keep_th07_rescue_frame_semantics();
    life_awards_require_explicit_status_and_clamp_to_th10_cap();
    team_extend_can_bank_a_spirit_life_without_reviving_it();
    next_stage_revives_every_seat_and_drops_transient_coop_state();
    final_death_waits_for_native_transition_and_can_receive_a_team_extend();
    spirit_alive_spirit_fullwipe_resets_then_retries_at_exactly_180_ticks();
    invalid_rosters_and_loadouts_are_rejected();
    return 0;
}
