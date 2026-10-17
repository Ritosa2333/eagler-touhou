// The real TH09 adapter and wire channel, with a deterministic component oracle.
// This is not a game-world, browser, real-device or performance acceptance test.
#define TH09_SESSION_FIXTURE_ONLY
#include "session.cpp"

static_assert(RollbackSession::History == 8, "Mobile prediction keeps the existing history bound");

void sameInput(const FrameInput& got, const FrameInput& want) {
    // Compare every field independently of FrameInput::operator==.
    CHECK(got.buttons == want.buttons);
    CHECK(got.analogMode == want.analogMode);
    CHECK(got.x == want.x && got.y == want.y);
    CHECK(got.unlimited == want.unlimited);
    CHECK(got.touchUsed == want.touchUsed);
    CHECK(got.touchBomb == want.touchBomb);
}
FrameInput heldInput(AnalogMode mode, bool unlimited = false) {
    FrameInput in(1 | 4);
    in.analogMode = mode;
    in.x = mode == AnalogMode::Joystick ? .75f : 125.25f;
    in.y = mode == AnalogMode::Joystick ? -.5f : 210.5f;
    in.unlimited = unlimited;
    in.touchUsed = true;
    return in;
}
FrameInput localInput(unsigned frame) {
    // Every captured local sample differs, so replay cannot silently resample
    // or substitute the current device value for a historical input.
    FrameInput in(std::uint16_t((frame & 1 ? 1 : 4) | (frame == 3 ? 2 : 0)));
    in.analogMode = AnalogMode::Joystick;
    in.x = float(frame + 1) / 16;
    in.y = -.25f;
    in.touchUsed = true;
    return in;
}

struct MobileFixture {
    Link a, b;
    RollbackSession session, peer;
    State state;
    std::array<State, 32> checkpoints{};
    std::array<std::array<FrameInput, MAX_PLAYERS>, 32> used{};
    std::array<FrameInput, 32> remote{};
    std::array<unsigned, 32> captures{};
    std::uint64_t now = 0;
    unsigned sequence = 0, remoteFrontier = 0;

    MobileFixture(): session(a), peer(b) {
        a.peer = &b; b.peer = &a;
        CHECK(session.Begin(config(0), 0, 0, AdonisMode::Rollback));
        CHECK(peer.Begin(config(1), 0, 0, AdonisMode::Rollback));
        for (unsigned i = 0; i < 32 && (!session.Ready() || !peer.Ready()); ++i) {
            CHECK(session.Pump(0)); CHECK(peer.Pump(0));
        }
        CHECK(session.Ready() && peer.Ready());
        CHECK(session.Mode() == AdonisMode::Rollback && session.InputDelay() == 0);
        a.pending.clear(); b.pending.clear();
    }
    void pump() {
        a.now = b.now = ++now;
        CHECK(session.Pump(now));
    }
    void receive(unsigned first, unsigned last) {
        CHECK(first <= last && last < remote.size());
        CHECK(last - first + 1 <= MAX_REDUNDANT_INPUTS);
        InputPacket packet;
        packet.sessionId = config(1).sessionId;
        packet.sequence = ++sequence;
        packet.senderPlayer = 1; packet.playerCount = 2;
        packet.firstInputFrame = first; packet.latestFrame = last;
        packet.senderFrame = remoteFrontier = std::max(remoteFrontier, last + 1);
        packet.inputCount = std::uint8_t(last - first + 1);
        for (unsigned i = 0; i < packet.inputCount; ++i) {
            CHECK(RollbackSession::ValidInput(remote[first + i]));
            packet.inputs[i] = remote[first + i];
        }
        Packet wire{now, {}};
        CHECK(EncodeInputPacket(packet, &wire.bytes));
        InputPacket decoded;
        CHECK(DecodeInputPacket(wire.bytes.data(), wire.bytes.size(), &decoded));
        CHECK(decoded.firstInputFrame == first && decoded.inputCount == packet.inputCount);
        for (unsigned i = 0; i < packet.inputCount; ++i)
            sameInput(decoded.inputs[i], remote[first + i]);
        a.pending.push_back(std::move(wire));
    }
    void capture() {
        const auto frame = session.Frame();
        CHECK(frame < captures.size() && session.NeedsCapture());
        CHECK(++captures[frame] == 1);
        CHECK(session.Capture(localInput(frame), now));
        CHECK(!session.NeedsCapture());
    }
    FrameDecision step() {
        const auto frame = session.Frame();
        CHECK(frame < used.size());
        const auto decision = session.Prepare();
        CHECK(decision.canAdvance);
        sameInput(decision.inputs[0], localInput(frame));
        checkpoints[frame] = state;
        used[frame] = decision.inputs;
        state.step(decision.inputs);
        CHECK(session.Complete(decision));
        return decision;
    }
    void seed(const FrameInput& in) {
        remote.fill(in);
        receive(0, 0); pump(); capture();
        CHECK(step().predictedMask == 0);
    }
    void predictThrough(unsigned end) {
        while (session.Frame() < end) {
            capture();
            const auto decision = step();
            CHECK(decision.predictedMask == 2);
        }
    }
    void restoreThrough(unsigned end) {
        const auto first = session.RollbackFrame();
        CHECK(first != INVALID_FRAME && first < end && session.Frame() == end);
        CHECK(end - first <= RollbackSession::History);
        const auto before = session.Captures();
        state = checkpoints[first];
        CHECK(session.Restored(first));
        CHECK(session.PredictionFrontier() == end);
        while (session.Frame() < end) {
            CHECK(!session.NeedsCapture());
            step();
            CHECK(session.Captures() == before);
        }
    }
    void verify(unsigned end) const {
        CHECK(session.Frame() == end && session.RollbackFrame() == INVALID_FRAME);
        CHECK(session.ConfirmedThrough() == end - 1);
        State reference;
        for (unsigned frame = 0; frame < end; ++frame) {
            const std::array<FrameInput, MAX_PLAYERS> exact{
                localInput(frame), remote[frame], {}};
            std::array<FrameInput, MAX_PLAYERS> confirmed;
            CHECK(session.ConfirmedInputs(frame, &confirmed));
            for (unsigned side = 0; side < MAX_PLAYERS; ++side) {
                sameInput(used[frame][side], exact[side]);
                sameInput(confirmed[side], exact[side]);
            }
            CHECK(captures[frame] == 1);
            reference.step(exact);
        }
        CHECK(state.x == reference.x && state.charge == reference.charge);
        CHECK(state.bombs == reference.bombs && state.menus == reference.menus);
        CHECK(state.previous == reference.previous && state.hash == reference.hash);
    }
};

void heldAndLimit(AnalogMode mode, bool unlimited = false) {
    MobileFixture f;
    const auto held = heldInput(mode, unlimited);
    f.seed(held);
    for (unsigned distance = 1; distance <= RollbackSession::History; ++distance) {
        f.capture();
        const auto decision = f.step();
        CHECK(decision.predictedMask == 2);
        sameInput(decision.inputs[1], held);
        CHECK(f.session.RollbackFrame() == INVALID_FRAME);
    }
    CHECK(f.session.Frame() == 9 && f.session.MissingRemoteInputs() == 8);
    // The due D0 input is captured once even while the ninth missing remote
    // frame is blocked. Receiving actual input must reuse that capture.
    f.capture();
    CHECK(!f.session.Prepare().canAdvance && !f.session.NeedsCapture());
    f.receive(1, 8); f.pump();
    CHECK(f.session.RollbackFrame() == INVALID_FRAME && f.session.Corrections() == 0);
    CHECK(f.session.Frame() == 9 && !f.session.NeedsCapture());
    sameInput(f.step().inputs[1], held);
    f.receive(9, 9); f.pump();
    CHECK(f.session.Captures() == 10 && f.session.Resimulated() == 0);
    f.verify(10);
}

unsigned changedCases = 0;
void lateChange(const char* name, const FrameInput& held, const FrameInput& changed) {
    CHECK(RollbackSession::ValidInput(held) && RollbackSession::ValidInput(changed));
    CHECK(held != changed);
    MobileFixture f;
    f.seed(held);
    for (unsigned frame = 2; frame < f.remote.size(); ++frame) f.remote[frame] = changed;
    f.predictThrough(6);
    // Match the production frontier capture before Restore. It belongs to
    // frame 6, not any of the historical frames that the correction revisits.
    f.capture();
    CHECK(f.session.Captures() == 7);
    f.receive(1, 5); f.pump();
    CHECK(f.session.RollbackFrame() == 2);
    CHECK(!f.session.Prepare().canAdvance);
    f.restoreThrough(6);
    CHECK(f.session.Corrections() == 1 && f.session.Resimulated() == 4);
    CHECK(f.session.Captures() == 7 && !f.session.NeedsCapture());
    f.receive(6, 6); f.pump();
    CHECK(f.step().predictedMask == 0);
    CHECK(f.session.NeedsCapture());
    f.verify(7);
    ++changedCases;
    std::printf("Mobile input full correction: %s PASS\n", name);
}
void changedInputs() {
    const auto touch = heldInput(AnalogMode::DirectTouch);
    const auto stick = heldInput(AnalogMode::Joystick);
    auto changed = touch; changed.x += .125f; lateChange("target-x", touch, changed);
    changed = touch; changed.y += .25f; lateChange("target-y", touch, changed);
    changed = stick; changed.x = -changed.x; lateChange("stick-turn", stick, changed);
    changed = stick; changed.y += .25f; lateChange("stick-y", stick, changed);
    changed = FrameInput(touch.buttons); lateChange("touch-release", touch, changed);
    changed = FrameInput(stick.buttons); lateChange("stick-release", stick, changed);
    changed = touch; changed.unlimited = true; lateChange("unlimited-on", touch, changed);
    lateChange("unlimited-off", changed, touch);
    changed = touch; changed.touchUsed = false; lateChange("touch-used", touch, changed);
    lateChange("target-to-stick", touch, stick);
    lateChange("stick-to-target", stick, touch);
    changed = touch; changed.buttons &= ~1u; lateChange("charge-release", touch, changed);
    changed = touch; changed.buttons &= ~4u; lateChange("focus-release", touch, changed);
    changed = touch; changed.buttons |= 64; lateChange("direction", touch, changed);
    changed = touch; changed.buttons |= 2; lateChange("bomb", touch, changed);
    changed = touch; changed.buttons |= 8; lateChange("menu", touch, changed);
}
void firstSampleAndButtons() {
    {
        MobileFixture f;
        f.remote.fill(heldInput(AnalogMode::DirectTouch));
        f.capture();
        const auto unknown = f.step();
        CHECK(unknown.predictedMask == 2);
        sameInput(unknown.inputs[1], FrameInput{});
        f.receive(0, 0); f.pump();
        CHECK(f.session.RollbackFrame() == 0);
        f.restoreThrough(1);
        CHECK(f.session.Captures() == 1 && f.session.Corrections() == 1);
        CHECK(f.session.Resimulated() == 1);
        f.verify(1);
    }
    {
        MobileFixture f;
        auto held = heldInput(AnalogMode::DirectTouch);
        held.buttons = 1 | 2 | 4 | 8 | 0xf0;
        f.seed(held);
        for (unsigned distance = 1; distance <= RollbackSession::History; ++distance) {
            f.capture();
            const auto decision = f.step();
            auto expected = held;
            expected.buttons = std::uint16_t(1 | 4 | (distance <= 3 ? 0xf0 : 0));
            CHECK(decision.predictedMask == 2);
            sameInput(decision.inputs[1], expected);
            CHECK(!(decision.inputs[1].buttons & (2 | 8)));
            CHECK(!decision.inputs[1].touchBomb);
        }
    }
}
void packetOrderAndConflict() {
    MobileFixture f;
    f.seed(heldInput(AnalogMode::DirectTouch));
    f.predictThrough(6);
    for (unsigned frame = 2; frame < f.remote.size(); ++frame) f.remote[frame].x += 1;
    f.receive(4, 5); f.pump();
    CHECK(f.session.RollbackFrame() == 4 && f.session.RemoteInputFrontier() == 1);
    f.receive(1, 3); f.pump();
    CHECK(f.session.RollbackFrame() == 2 && f.session.RemoteInputFrontier() == 6);
    f.restoreThrough(6);
    f.verify(6);
    const auto corrections = f.session.Corrections();
    f.receive(1, 5); f.pump();
    CHECK(f.session.RollbackFrame() == INVALID_FRAME && f.session.Corrections() == corrections);
    f.remote[2].x += 1;
    f.receive(2, 2);
    f.a.now = f.b.now = ++f.now;
    CHECK(!f.session.Pump(f.now) && f.session.Failed());
}
void invalidAndRepeatedCapture() {
    FrameInput value;
    CHECK(RollbackSession::ValidInput(value));
    value.x = 1; CHECK(!RollbackSession::ValidInput(value));
    value = {}; value.touchUsed = true; CHECK(!RollbackSession::ValidInput(value));
    value = {}; value.unlimited = true; CHECK(!RollbackSession::ValidInput(value));
    value = heldInput(AnalogMode::Joystick);
    value.x = 16; value.y = -16; CHECK(RollbackSession::ValidInput(value));
    value.x = 16.25f; CHECK(!RollbackSession::ValidInput(value));
    value = heldInput(AnalogMode::Joystick, true); CHECK(!RollbackSession::ValidInput(value));
    value = heldInput(AnalogMode::DirectTouch);
    value.x = 4096; value.y = -4096; CHECK(RollbackSession::ValidInput(value));
    value.x = 4096.25f; CHECK(!RollbackSession::ValidInput(value));
    value.x = std::numeric_limits<float>::infinity(); CHECK(!RollbackSession::ValidInput(value));
    value.x = std::numeric_limits<float>::quiet_NaN(); CHECK(!RollbackSession::ValidInput(value));
    value = heldInput(AnalogMode::DirectTouch); value.touchBomb = true;
    CHECK(!RollbackSession::ValidInput(value));
    value = heldInput(AnalogMode::DirectTouch); value.analogMode = AnalogMode::DirectTouchDelta;
    CHECK(!RollbackSession::ValidInput(value));
    value.analogMode = AnalogMode::DirectTouchBegin; CHECK(!RollbackSession::ValidInput(value));
    {
        MobileFixture f;
        f.capture();
        CHECK(!f.session.Capture(localInput(0), f.now));
        CHECK(f.session.Failed() && f.session.Captures() == 1);
    }
}
int main() {
    heldAndLimit(AnalogMode::DirectTouch);
    heldAndLimit(AnalogMode::DirectTouch, true);
    heldAndLimit(AnalogMode::Joystick);
    changedInputs();
    firstSampleAndButtons();
    packetOrderAndConflict();
    invalidAndRepeatedCapture();
    std::printf("TH09 mobile prediction component: held=3 changed=%u, D0/history8/full-input/once-only PASS\n", changedCases);
    std::puts("Scope: component input/rollback contract, not game-world, browser, mobile-device or performance acceptance");
}
