#pragma once

#include <cstdint>

namespace th10::multiplayer {

constexpr std::uint8_t kMaxSeats = 3;
constexpr std::uint8_t kMinSeats = 2;
constexpr std::int16_t kMaxLives = 9;
constexpr std::int16_t kMaxPower = 100;
constexpr std::uint8_t kRescueTicks = 90;
constexpr std::uint16_t kWipeRetryTicks = 180;
constexpr std::int32_t kRescueRadiusHundredths = 2000;
constexpr std::int16_t kPowerTransferAmount = 20;
constexpr std::int16_t kMaxPowerTransferRecipient = kMaxPower - 1;
constexpr std::uint8_t kPowerTapCount = 5;
constexpr std::uint8_t kPowerTapWindow = 24;

enum class LifeState : std::uint8_t {
    Alive,
    // Final native death animation has not yet reached its terminal state.
    Dying,
    Spirit,
    Eliminated,
};

struct SeatSetup {
    std::uint8_t character;
    std::uint8_t shot;
    std::int16_t lives;
    std::int16_t power;
};

struct SeatState {
    std::int16_t lives;
    std::int16_t power;
    std::uint8_t character;
    std::uint8_t shot;
    LifeState lifeState;
    std::uint8_t rescueTicks;
    std::int8_t rescueTarget;
    bool waitingForFocusRelease;
};

// Life-transfer progress and the rapid-shot Power gesture are mutually
// exclusive. To preserve the already-published rollback/canonical state
// layout, an in-progress Power gesture uses rescueTarget -2..-8 for taps 1..7
// and rescueTicks for its expiry window. Stage checkpoints only admit the
// clean -1/0 state, so this transient encoding does not change Replay format.
std::uint8_t PowerTapProgress(const SeatState& seat) noexcept;

// This is the complete rollback-owned policy state. It contains no pointers,
// clocks, platform handles, or native game objects.
struct State {
    std::uint8_t seatCount;
    std::uint16_t wipeTicks;
    bool retryPending;
    SeatState seats[kMaxSeats];
};

struct SeatFrameInput {
    // TH10 fixed_position coordinates: hundredths of a pixel.
    std::int32_t x;
    std::int32_t y;
    bool canInitiateLifeTransfer;
    bool canReceiveLifeTransfer;
    bool canReceivePowerTransfer;
    bool focus;
    bool shoot;
    bool shootPressed;
};

struct FrameInput {
    SeatFrameInput seats[kMaxSeats];
};

enum class EventKind : std::uint8_t {
    SpiritRevived,
    LifeItemTransferCommitted,
    PowerItemTransferCommitted,
    WipeRetryRequested,
};

struct Event {
    EventKind kind;
    std::uint8_t seat;
    std::int8_t targetSeat;
    std::int16_t giverLivesAfter;
    std::int16_t targetLivesAfter;
};

struct TickResult {
    Event events[kMaxSeats + 1];
    std::uint8_t eventCount;
};

using AllocateTargetedLifeItem = bool (*)(void* context, std::uint8_t giver,
                                          std::uint8_t target) noexcept;

struct LifeItemAllocator {
    void* context;
    AllocateTargetedLifeItem allocate;
};
using AllocateTargetedPowerItem = bool (*)(void* context, std::uint8_t giver,
                                           std::uint8_t target) noexcept;
struct PowerItemAllocator {
    void* context;
    AllocateTargetedPowerItem allocate;
};

// Invalid setup leaves state cleared and returns false. A successful setup
// creates a fixed 2- or 3-seat roster with TH10 character/shot bounds.
bool Initialize(State& state, const SeatSetup* seats, std::uint8_t seatCount) noexcept;

// The adapter calls this after native life/power/death processing has finished.
// The policy records that authoritative outcome; it never repeats native
// penalties. TH10's native life range is -1..9 (negative means game over).
bool ReportNativeSeatOutcome(State& state, std::uint8_t seat, LifeState lifeState,
                             std::int16_t lives, std::int16_t power) noexcept;

// Life awards always specify the resulting life state. Pass the existing state
// for an ordinary pickup; a title adapter must explicitly select any recovery
// transition and mirror the returned policy snapshot to its native player.
bool ApplyLifeAward(State& state, std::uint8_t seat, std::int16_t amount,
                    LifeState resultingState) noexcept;

// A real next-stage transition is a clean cooperative boundary. Every seat
// becomes playable again; a final-death -1 is promoted to the minimum playable
// zero-life state, while surviving life/power values are retained. Rescue and
// wipe gestures never leak across stages.
bool BeginNextStage(State& state) noexcept;

// One call equals one logical gameplay tick. Repeated identical inputs count
// as repeated ticks; the caller is responsible for invoking this once per tick.
// The optional allocator is called synchronously in seat order. A missing or
// failed allocator leaves the giver's life untouched and permits a later retry.
TickResult AdvanceOneTick(State& state, const FrameInput& input,
                          LifeItemAllocator allocator = {},
                          PowerItemAllocator powerAllocator = {},
                          PowerItemAllocator rescueAllocator = {}) noexcept;

} // namespace th10::multiplayer
