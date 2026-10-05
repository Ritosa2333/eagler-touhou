#include "CooperativeRules.hpp"

#include <type_traits>

namespace th10::multiplayer {
namespace {

constexpr std::int32_t kMinX = -18400;
constexpr std::int32_t kMaxX = 18400;
constexpr std::int32_t kMinY = 3200;
constexpr std::int32_t kMaxY = 43200;
constexpr std::int16_t kMinNativeLives = -1;
constexpr std::int32_t kRescueRadiusSquared =
    kRescueRadiusHundredths * kRescueRadiusHundredths;

static_assert(std::is_trivially_copyable<State>::value,
              "cooperative state must remain rollback-copyable");

bool valid_life_state(LifeState state) noexcept {
    return state == LifeState::Alive || state == LifeState::Dying || state == LifeState::Spirit ||
           state == LifeState::Eliminated;
}

bool valid_seat(const State& state, std::uint8_t seat) noexcept {
    return state.seatCount >= kMinSeats && state.seatCount <= kMaxSeats &&
           seat < state.seatCount;
}

bool valid_position(const SeatFrameInput& input) noexcept {
    return input.x >= kMinX && input.x <= kMaxX && input.y >= kMinY &&
           input.y <= kMaxY;
}

std::int64_t distance_squared(const SeatFrameInput& left,
                              const SeatFrameInput& right) noexcept {
    const std::int64_t dx = static_cast<std::int64_t>(left.x) - right.x;
    const std::int64_t dy = static_cast<std::int64_t>(left.y) - right.y;
    return dx * dx + dy * dy;
}

void reset_rescue(SeatState& seat) noexcept {
    seat.rescueTicks = 0;
    seat.rescueTarget = -1;
}

std::uint8_t power_taps(const SeatState& seat) noexcept {
    if (seat.rescueTarget >= -1) return 0;
    const int value = -static_cast<int>(seat.rescueTarget) - 1;
    return value > 0 && value < kPowerTapCount ? static_cast<std::uint8_t>(value) : 0;
}

void set_power_gesture(SeatState& seat, std::uint8_t taps,
                       std::uint8_t window) noexcept {
    if (!taps || taps >= kPowerTapCount || !window) {
        reset_rescue(seat);
        return;
    }
    seat.rescueTarget = static_cast<std::int8_t>(-1 - static_cast<int>(taps));
    seat.rescueTicks = window;
}

void reset_wipe_if_anyone_alive(State& state) noexcept {
    for (std::uint8_t i = 0; i < state.seatCount; ++i) {
        if (state.seats[i].lifeState == LifeState::Alive ||
            state.seats[i].lifeState == LifeState::Dying) {
            state.wipeTicks = 0;
            state.retryPending = false;
            return;
        }
    }
}

std::int8_t select_receiver(const State& state, const FrameInput& input,
                            std::uint8_t giver) noexcept {
    std::int8_t best = -1;
    bool bestIsSpirit = false;
    for (std::uint8_t candidate = 0; candidate < state.seatCount; ++candidate) {
        if (candidate == giver || !valid_position(input.seats[giver]) ||
            !valid_position(input.seats[candidate]) ||
            distance_squared(input.seats[giver], input.seats[candidate]) >
                kRescueRadiusSquared) {
            continue;
        }

        const SeatState& target = state.seats[candidate];
        const bool isSpirit = target.lifeState == LifeState::Spirit;
        const bool canReceiveLife =
            target.lifeState == LifeState::Alive &&
            input.seats[candidate].canReceiveLifeTransfer &&
            target.lives < kMaxLives;
        if (!isSpirit && !canReceiveLife) {
            continue;
        }

        if (best < 0 || (isSpirit && !bestIsSpirit) ||
            (isSpirit == bestIsSpirit && target.lives < state.seats[best].lives) ||
            (isSpirit == bestIsSpirit && target.lives == state.seats[best].lives &&
             candidate < static_cast<std::uint8_t>(best))) {
            best = static_cast<std::int8_t>(candidate);
            bestIsSpirit = isSpirit;
        }
    }
    return best;
}

std::int8_t select_power_receiver(const State& state, const FrameInput& input,
                                  std::uint8_t giver) noexcept {
    std::int8_t best = -1;
    for (std::uint8_t candidate = 0; candidate < state.seatCount; ++candidate) {
        if (candidate == giver || !valid_position(input.seats[giver]) ||
            !valid_position(input.seats[candidate]) ||
            distance_squared(input.seats[giver], input.seats[candidate]) >
                kRescueRadiusSquared) {
            continue;
        }
        const SeatState& target = state.seats[candidate];
        if (target.lifeState != LifeState::Alive ||
            !input.seats[candidate].canReceivePowerTransfer ||
            target.power > kMaxPowerTransferRecipient) {
            continue;
        }
        if (best < 0 || target.power < state.seats[best].power ||
            (target.power == state.seats[best].power &&
             candidate < static_cast<std::uint8_t>(best))) {
            best = static_cast<std::int8_t>(candidate);
        }
    }
    return best;
}

void append_event(TickResult& result, EventKind kind, std::uint8_t seat,
                  std::int8_t target, std::int16_t giverLives,
                  std::int16_t targetLives) noexcept {
    if (result.eventCount >= kMaxSeats + 1) {
        return;
    }
    result.events[result.eventCount++] =
        Event{kind, seat, target, giverLives, targetLives};
}

} // namespace

std::uint8_t PowerTapProgress(const SeatState& seat) noexcept {
    return power_taps(seat);
}

bool Initialize(State& state, const SeatSetup* setup,
                std::uint8_t seatCount) noexcept {
    state = {};
    for (auto& seat : state.seats) {
        seat.rescueTarget = -1;
    }
    if (!setup || seatCount < kMinSeats || seatCount > kMaxSeats) {
        return false;
    }
    for (std::uint8_t i = 0; i < seatCount; ++i) {
        if (setup[i].character > 1 || setup[i].shot > 2 || setup[i].lives < 0 ||
            setup[i].lives > kMaxLives || setup[i].power < 0 ||
            setup[i].power > kMaxPower) {
            state = {};
            for (auto& seat : state.seats) {
                seat.rescueTarget = -1;
            }
            return false;
        }
    }
    state.seatCount = seatCount;
    for (std::uint8_t i = 0; i < seatCount; ++i) {
        state.seats[i].character = setup[i].character;
        state.seats[i].shot = setup[i].shot;
        state.seats[i].lives = setup[i].lives;
        state.seats[i].power = setup[i].power;
        state.seats[i].lifeState = LifeState::Alive;
    }
    return true;
}

bool ReportNativeSeatOutcome(State& state, std::uint8_t seat,
                             LifeState lifeState, std::int16_t lives,
                             std::int16_t power) noexcept {
    if (!valid_seat(state, seat) || !valid_life_state(lifeState) ||
        lives < kMinNativeLives || lives > kMaxLives ||
        (lifeState == LifeState::Alive && lives < 0) || power < 0 ||
        power > kMaxPower) {
        return false;
    }
    SeatState& current = state.seats[seat];
    current.lifeState = lifeState;
    current.lives = lives;
    current.power = power;
    if (lifeState != LifeState::Alive) {
        reset_rescue(current);
    }
    reset_wipe_if_anyone_alive(state);
    return true;
}

bool ApplyLifeAward(State& state, std::uint8_t seat, std::int16_t amount,
                    LifeState resultingState) noexcept {
    if (!valid_seat(state, seat) || amount <= 0 ||
        !valid_life_state(resultingState)) {
        return false;
    }
    SeatState& recipient = state.seats[seat];
    const std::int32_t awarded =
        static_cast<std::int32_t>(recipient.lives) + amount;
    recipient.lives = static_cast<std::int16_t>(
        awarded > kMaxLives ? kMaxLives : awarded);
    recipient.lifeState = resultingState;
    if (resultingState != LifeState::Alive) {
        reset_rescue(recipient);
    }
    reset_wipe_if_anyone_alive(state);
    return true;
}

bool BeginNextStage(State& state) noexcept {
    if (state.seatCount < kMinSeats || state.seatCount > kMaxSeats) {
        return false;
    }
    for (std::uint8_t seat = 0; seat < state.seatCount; ++seat) {
        SeatState& current = state.seats[seat];
        if (!valid_life_state(current.lifeState) ||
            current.character > 1 || current.shot > 2 ||
            current.lives < kMinNativeLives || current.lives > kMaxLives ||
            current.power < 0 || current.power > kMaxPower) {
            return false;
        }
        if (current.lives < 0) {
            current.lives = 0;
        }
        current.lifeState = LifeState::Alive;
        current.waitingForFocusRelease = false;
        reset_rescue(current);
    }
    state.wipeTicks = 0;
    state.retryPending = false;
    return true;
}

TickResult AdvanceOneTick(State& state, const FrameInput& input,
                          LifeItemAllocator allocator,
                          PowerItemAllocator powerAllocator) noexcept {
    TickResult result{};
    if (state.seatCount < kMinSeats || state.seatCount > kMaxSeats) {
        return result;
    }

    for (std::uint8_t giver = 0; giver < state.seatCount; ++giver) {
        SeatState& source = state.seats[giver];
        const SeatFrameInput& controls = input.seats[giver];

        // TH10's native big-P is exactly twenty units on its 0..100 scale.
        // Only the authoritative pressed edge advances this counter; a held
        // button, prediction retry, or packet retransmission cannot add taps.
        if (source.lifeState != LifeState::Alive ||
            !controls.canInitiateLifeTransfer || source.waitingForFocusRelease ||
            source.power < kPowerTransferAmount) {
            if (power_taps(source)) reset_rescue(source);
        } else {
            const std::int8_t powerTarget = select_power_receiver(state, input, giver);
            if (powerTarget < 0) {
                if (power_taps(source)) reset_rescue(source);
            } else {
                std::uint8_t taps = power_taps(source);
                std::uint8_t window = taps ? source.rescueTicks : 0;
                if (taps && window && --window == 0) {
                    taps = 0;
                    reset_rescue(source);
                } else if (taps) {
                    set_power_gesture(source, taps, window);
                }
                if (controls.shootPressed) {
                    taps = power_taps(source);
                    ++taps;
                    if (taps >= kPowerTapCount) {
                        reset_rescue(source);
                        if (powerAllocator.allocate &&
                            powerAllocator.allocate(powerAllocator.context, giver,
                                                    static_cast<std::uint8_t>(powerTarget))) {
                            source.power = static_cast<std::int16_t>(
                                source.power - kPowerTransferAmount);
                            append_event(result, EventKind::PowerItemTransferCommitted,
                                         giver, powerTarget, source.lives,
                                         state.seats[static_cast<std::uint8_t>(powerTarget)].lives);
                        }
                    } else {
                        set_power_gesture(source, taps, kPowerTapWindow);
                    }
                }
            }
        }

        if (source.waitingForFocusRelease) {
            reset_rescue(source);
            if (!controls.focus) {
                source.waitingForFocusRelease = false;
            }
            continue;
        }
        if (source.lifeState != LifeState::Alive ||
            !controls.canInitiateLifeTransfer || state.seatCount < 2) {
            reset_rescue(source);
            continue;
        }

        // Rapid-shot Power transfer and the continuous focus-hold life
        // transfer intentionally cannot accumulate in the same two fields.
        if (power_taps(source)) {
            continue;
        }

        const std::int8_t target = select_receiver(state, input, giver);
        if (target < 0) {
            reset_rescue(source);
            continue;
        }
        if (source.rescueTarget != target) {
            source.rescueTicks = 0;
            source.rescueTarget = target;
        }
        if (!controls.focus || controls.shoot) {
            reset_rescue(source);
            continue;
        }
        if (source.rescueTicks < kRescueTicks) {
            ++source.rescueTicks;
        }
        if (source.rescueTicks < kRescueTicks || source.lives <= 0) {
            continue;
        }

        SeatState& receiver = state.seats[static_cast<std::uint8_t>(target)];
        if (receiver.lifeState == LifeState::Spirit) {
            --source.lives;
            receiver.lifeState = LifeState::Alive;
            receiver.lives = 0;
            receiver.power = kMaxPower / 2;
            source.waitingForFocusRelease = true;
            reset_rescue(source);
            reset_rescue(receiver);
            append_event(result, EventKind::SpiritRevived, giver, target,
                         source.lives, receiver.lives);
        } else if (receiver.lifeState == LifeState::Alive &&
                   receiver.lives < kMaxLives) {
            reset_rescue(source);
            if (allocator.allocate &&
                allocator.allocate(allocator.context, giver,
                                   static_cast<std::uint8_t>(target))) {
                --source.lives;
                source.waitingForFocusRelease = true;
                append_event(result, EventKind::LifeItemTransferCommitted,
                             giver, target, source.lives, receiver.lives);
            }
        } else {
            reset_rescue(source);
        }
    }

    bool everyoneOut = true;
    for (std::uint8_t i = 0; i < state.seatCount; ++i) {
        if (state.seats[i].lifeState == LifeState::Alive ||
            state.seats[i].lifeState == LifeState::Dying) {
            everyoneOut = false;
            break;
        }
    }
    if (!everyoneOut) {
        state.wipeTicks = 0;
        state.retryPending = false;
    } else if (!state.retryPending) {
        if (state.wipeTicks < kWipeRetryTicks) {
            ++state.wipeTicks;
        }
        if (state.wipeTicks == kWipeRetryTicks) {
            state.retryPending = true;
            append_event(result, EventKind::WipeRetryRequested, 0, -1, 0, 0);
        }
    }
    return result;
}

} // namespace th10::multiplayer
