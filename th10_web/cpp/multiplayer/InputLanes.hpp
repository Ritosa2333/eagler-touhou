#pragma once

#include "../game/GameInput.hpp"

#include <cstddef>
#include <type_traits>

#include <eagler/netplay/NetplayProtocol.hpp>

namespace th10::multiplayer::InputLanes {

constexpr std::size_t kSeatCount = Netplay::MAX_PLAYERS;
constexpr u16 kNativeCurrentMask = 0x1f7;
constexpr u16 kShoot = 1u << 0;
constexpr u16 kBomb = 1u << 1;
constexpr u16 kFocus = 1u << 2;
constexpr u16 kPause = 1u << 3;
constexpr u16 kDirection = 0xf0;

// This is the complete title-owned input owner for a rollback frame. It has
// no native object, platform handle, clock, or pointer, so a snapshot can
// copy it as one value. `host` is the menu/dialogue view derived from seat 0;
// it is kept in the owner so replay and resimulation do not sample anything.
struct State {
    GameInput seats[kSeatCount]{};
    Netplay::FrameInput inputs[kSeatCount]{};
    GameInput host{};
    u16 pause_rising = 0;
};

static_assert(std::is_trivially_copyable<State>::value,
              "input lanes must remain rollback-copyable");

// Validate and install one already-captured logical frame. `count` is the
// session player count (two or three). Missing seats are committed as neutral
// lanes, including their edge transition, so stale input cannot leak across a
// session boundary. No device or local configuration is read here.
bool Commit(State& state, const Netplay::FrameInput* inputs, u32 count) noexcept;

// Return the menu/dialogue lane. `raw` and raw edge fields come from the host
// seat; raw_pressed additionally carries one Pause event when any seat newly
// pressed Pause during the committed frame. Native current/pressed fields stay
// host-only for title dialogue and gameplay code.
const GameInput& HostControls(State& state) noexcept;
const GameInput& HostControls(const State& state) noexcept;

} // namespace th10::multiplayer::InputLanes
