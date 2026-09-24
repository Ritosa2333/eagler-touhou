#include "../th10_web/cpp/multiplayer/InputLanes.hpp"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>

using namespace th10::multiplayer::InputLanes;

namespace {

Netplay::FrameInput input(std::uint16_t buttons, float x = 0.0f,
                          float y = 0.0f) {
    Netplay::FrameInput result(buttons);
    result.analogMode = Netplay::AnalogMode::Joystick;
    result.x = x;
    result.y = y;
    return result;
}

bool same(const State& left, const State& right) {
    return std::memcmp(&left, &right, sizeof(State)) == 0;
}

} // namespace

int main() {
    static_assert(std::is_trivially_copyable<State>::value);
    State state{};
    Netplay::FrameInput frame[] = {
        input(kShoot | kDirection, 0.25f, -0.5f),
        input(kBomb | 0x100u, -0.75f, 0.5f),
        input(kFocus, 1.0f, 0.0f),
    };

    assert(Commit(state, frame, 3));
    assert(state.inputs[0] == frame[0]);
    assert(state.inputs[1] == frame[1]);
    assert(state.seats[0].current == (kShoot | kDirection));
    assert(state.seats[1].current == (kBomb | 0x100u));
    assert(state.seats[0].pressed == (kShoot | kDirection));
    assert(state.seats[1].pressed == (kBomb | 0x100u));
    assert(state.seats[0].raw == frame[0].buttons);
    assert(state.seats[1].raw == frame[1].buttons);

    // P2 movement/fire is isolated from P1. The menu raw lane stays host raw.
    assert(HostControls(state).raw == frame[0].buttons);
    assert((HostControls(state).raw & 0x100u) == 0);

    const State before_invalid = state;
    frame[1].analogMode = static_cast<Netplay::AnalogMode>(5);
    assert(!Commit(state, frame, 3));
    assert(same(state, before_invalid));
    frame[1].analogMode = Netplay::AnalogMode::None;
    frame[1].x = std::numeric_limits<float>::quiet_NaN();
    assert(!Commit(state, frame, 3));
    assert(same(state, before_invalid));
    frame[1].x = 0.0f;
    frame[1].y = std::numeric_limits<float>::infinity();
    assert(!Commit(state, frame, 3));
    assert(same(state, before_invalid));

    // Every seat's new Pause is one menu event. A held P1 pause cannot hide a
    // fresh P2 edge because the event is merged from per-seat raw_pressed.
    Netplay::FrameInput pause_frame[] = {
        input(kPause), input(kPause), input(0),
    };
    assert(Commit(state, pause_frame, 3));
    assert((state.seats[0].raw_pressed & kPause) != 0);
    assert((state.seats[1].raw_pressed & kPause) != 0);
    assert((HostControls(state).raw_pressed & kPause) != 0);
    assert(Commit(state, pause_frame, 3));
    assert((HostControls(state).raw_pressed & kPause) == 0);
    Netplay::FrameInput p2_new_pause[] = {
        input(kPause), input(kPause), input(0),
    };
    // P1 remains held, P2 releases and presses again.
    p2_new_pause[1].buttons = 0;
    assert(Commit(state, p2_new_pause, 3));
    p2_new_pause[1].buttons = kPause;
    assert(Commit(state, p2_new_pause, 3));
    assert((HostControls(state).raw_pressed & kPause) != 0);

    State repeat_state{};
    for (int tick = 0; tick < 25; ++tick) {
        Netplay::FrameInput held[] = {input(kShoot), input(0), input(0)};
        assert(Commit(repeat_state, held, 3));
        assert((HostControls(repeat_state).raw_repeat & kShoot) == 0);
    }
    Netplay::FrameInput held[] = {input(kShoot), input(0), input(0)};
    assert(Commit(repeat_state, held, 3));
    assert((HostControls(repeat_state).raw_repeat & kShoot) != 0);

    // A two-seat frame clears the old third lane and can be restored/resimmed
    // byte-for-byte without sampling a device on the replay pass.
    State checkpoint = state;
    Netplay::FrameInput two_seat[] = {input(kShoot), input(kDirection)};
    assert(Commit(state, two_seat, 2));
    State resim = checkpoint;
    assert(Commit(resim, two_seat, 2));
    assert(same(state, resim));
    assert(resim.inputs[2].buttons == 0);
    assert(resim.seats[2].current == 0);

    // Local direct touch is captured only after rollback reconciliation. The
    // device owns an absolute target; the network frame owns a fresh
    // pre-timescale displacement, so prediction cannot repeat a browser event.
    LocalAnalogSample direct{};
    direct.kind = LocalAnalogSample::Kind::DirectTarget;
    direct.x = 14.0f;
    direct.y = 16.0f;
    direct.touchUsed = true;
    direct.touchBomb = true;
    Netplay::FrameInput logical{};
    assert(BuildLocalFrame(direct, kBomb, 1000, 2000, 2.0f, logical));
    assert(logical.buttons == kBomb);
    assert(logical.analogMode == Netplay::AnalogMode::DirectTouchDelta);
    assert(logical.x == 200.0f && logical.y == -200.0f);
    assert(logical.touchUsed && logical.touchBomb && !logical.unlimited);
    th10::i32 x = 0, y = 0;
    assert(ResolveMovement(logical, 150, x, y));
    assert(x == 106 && y == -106);
    logical.unlimited = true;
    assert(ResolveMovement(logical, 150, x, y));
    assert(x == 200 && y == -200);

    LocalAnalogSample joystick{};
    joystick.kind = LocalAnalogSample::Kind::Joystick;
    joystick.x = 1.0f;
    joystick.y = 1.0f;
    assert(BuildLocalFrame(joystick, kFocus, 0, 0, 1.0f, logical));
    assert(logical.analogMode == Netplay::AnalogMode::Joystick);
    assert(ResolveMovement(logical, 100, x, y));
    assert(x == 71 && y == 71);

    LocalAnalogSample neutral{};
    assert(BuildLocalFrame(neutral, kShoot, 0, 0, 1.0f, logical));
    assert(logical.analogMode == Netplay::AnalogMode::None);
    assert(!ResolveMovement(logical, 100, x, y));

    // Invalid count and null payload are also transactional.
    const State before_count = state;
    assert(!Commit(state, two_seat, 1));
    assert(same(state, before_count));
    assert(!Commit(state, nullptr, 2));
    assert(same(state, before_count));
    return 0;
}
