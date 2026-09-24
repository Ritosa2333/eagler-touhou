#include "InputLanes.hpp"

#include <algorithm>
#include <cmath>

namespace th10::multiplayer::InputLanes {
namespace {

bool valid_input(const Netplay::FrameInput& input) noexcept {
    // Keep this gate aligned with the common decoder: the enum is bounded and
    // both axes must be finite. Coordinates are transport payload, not local
    // motion/configuration state, so no device-specific range is imposed here.
    const auto mode = static_cast<unsigned>(input.analogMode);
    return mode <= 4u && std::isfinite(input.x) && std::isfinite(input.y);
}

constexpr float kVelocityLimit = 10000000.0f;

void limit_vector(float& x, float& y, float speed) noexcept {
    speed = std::abs(speed);
    const float square = x * x + y * y;
    if (square > speed * speed && square > 0.0f) {
        const float scale = speed / std::sqrt(square);
        x *= scale;
        y *= scale;
    }
}

void update_host_lane(State& state, u16 raw_repeat) noexcept {
    state.host = state.seats[0];
    // GameInput::update_edges clears raw_repeat. The title's native update
    // chain samples raw input before edge processing, so retain that captured
    // host repeat in the menu view without changing the rollback seat lanes.
    state.host.raw_repeat = raw_repeat;
    // Keep host current/previous/pressed/released native. Pause is a menu
    // event and is intentionally merged only into the raw edge channel. This
    // lets a fresh P2/P3 pause trigger while another seat still holds pause.
    state.host.raw_pressed = static_cast<u16>(state.host.raw_pressed |
                                              state.pause_rising);
}

} // namespace

bool Commit(State& state, const Netplay::FrameInput* inputs, u32 count) noexcept {
    if (!inputs || count < 2 || count > kSeatCount) {
        return false;
    }
    for (u32 seat = 0; seat < count; ++seat) {
        if (!valid_input(inputs[seat])) {
            return false;
        }
    }

    // Validate everything before touching the caller's owner. The candidate
    // also makes aliasing inputs == state.inputs safe for the transaction.
    State next = state;
    next.pause_rising = 0;
    u16 host_raw_repeat = 0;
    for (u32 seat = 0; seat < kSeatCount; ++seat) {
        const Netplay::FrameInput value = seat < count ? inputs[seat]
                                                       : Netplay::FrameInput{};
        auto& lane = next.seats[seat];
        next.inputs[seat] = value;

        lane.update_raw(value.buttons);
        if (seat == 0) {
            host_raw_repeat = lane.raw_repeat;
        }
        lane.previous = lane.current;
        lane.current = static_cast<u16>(value.buttons & kNativeCurrentMask);
        lane.update_edges();

        next.pause_rising = static_cast<u16>(next.pause_rising |
                                             (lane.raw_pressed & kPause));
    }
    update_host_lane(next, host_raw_repeat);
    state = next;
    return true;
}

bool BuildLocalFrame(const LocalAnalogSample& sample, u16 buttons,
                     i32 fixedX, i32 fixedY, float rate,
                     Netplay::FrameInput& out) noexcept {
    if (!std::isfinite(sample.x) || !std::isfinite(sample.y) ||
        !std::isfinite(rate)) {
        return false;
    }

    Netplay::FrameInput next{};
    next.buttons = buttons;
    next.touchUsed = sample.touchUsed;
    next.touchBomb = sample.touchBomb;
    switch (sample.kind) {
    case LocalAnalogSample::Kind::None:
        break;
    case LocalAnalogSample::Kind::Joystick:
        next.analogMode = Netplay::AnalogMode::Joystick;
        next.x = std::clamp(sample.x, -1.0f, 1.0f);
        next.y = std::clamp(sample.y, -1.0f, 1.0f);
        break;
    case LocalAnalogSample::Kind::DirectTarget:
        next.analogMode = Netplay::AnalogMode::DirectTouchDelta;
        next.unlimited = sample.unlimited;
        if (rate != 0.0f) {
            next.x = (sample.x * 100.0f - static_cast<float>(fixedX)) / rate;
            next.y = (sample.y * 100.0f - static_cast<float>(fixedY)) / rate;
        }
        if (!std::isfinite(next.x) || !std::isfinite(next.y) ||
            std::abs(next.x) > kVelocityLimit || std::abs(next.y) > kVelocityLimit) {
            return false;
        }
        break;
    }
    if (!Netplay::IsValidFrameInput(next)) return false;
    out = next;
    return true;
}

bool ResolveMovement(const Netplay::FrameInput& input, i32 speed,
                     i32& x, i32& y) noexcept {
    if (!valid_input(input) || input.analogMode == Netplay::AnalogMode::None) {
        return false;
    }

    float dx = input.x, dy = input.y;
    if (input.analogMode == Netplay::AnalogMode::Joystick) {
        dx = std::clamp(dx, -1.0f, 1.0f) * static_cast<float>(speed);
        dy = std::clamp(dy, -1.0f, 1.0f) * static_cast<float>(speed);
        limit_vector(dx, dy, static_cast<float>(speed));
    } else if (input.analogMode == Netplay::AnalogMode::DirectTouch ||
               input.analogMode == Netplay::AnalogMode::DirectTouchDelta ||
               input.analogMode == Netplay::AnalogMode::DirectTouchBegin) {
        if (!input.unlimited) limit_vector(dx, dy, static_cast<float>(speed));
        dx = std::clamp(dx, -kVelocityLimit, kVelocityLimit);
        dy = std::clamp(dy, -kVelocityLimit, kVelocityLimit);
    } else {
        return false;
    }

    x = static_cast<i32>(std::round(dx));
    y = static_cast<i32>(std::round(dy));
    return true;
}

const GameInput& HostControls(State& state) noexcept {
    return state.host;
}

const GameInput& HostControls(const State& state) noexcept {
    return state.host;
}

} // namespace th10::multiplayer::InputLanes
