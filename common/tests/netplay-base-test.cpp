#include <eagler/netplay/NetplaySession.hpp>
#include <eagler/netplay/NetplayCore.hpp>
#include <eagler/netplay/NetplayProtocol.hpp>
#include <eagler/netplay/WebSocketTransport.hpp>

#include <cassert>
#include <cstdint>
#include <iostream>
#include <vector>

using namespace Netplay;

namespace
{
void TestProtocolCapability()
{
    InputPacket packet;
    packet.sessionId = 0x1234;
    packet.senderPlayer = 0;
    packet.playerCount = 2;
    packet.latestFrame = 0;
    packet.firstInputFrame = 0;
    packet.inputCount = 1;
    packet.inputs[0].analogMode = AnalogMode::DirectTouch;
    packet.inputs[0].x = 1.5f;
    packet.inputs[0].y = -2.0f;

    std::vector<std::uint8_t> wire;
    assert(EncodeInputPacket(packet, &wire));
    assert(wire.size() >= 4 && wire[0] == 'E' && wire[1] == 'T' &&
           wire[2] == 'N' && wire[3] == 'P');
    InputPacket decoded;
    assert(DecodeInputPacket(wire.data(), wire.size(), &decoded));
    assert(decoded.inputs[0] == packet.inputs[0]);

    packet.inputs[0].analogMode = AnalogMode::DirectTouchDelta;
    assert(EncodeInputPacket(packet, &wire));
    assert(DecodeInputPacket(wire.data(), wire.size(), &decoded));
    assert(decoded.inputs[0] == packet.inputs[0]);
    packet.inputs[0].analogMode = AnalogMode::DirectTouchBegin;
    assert(EncodeInputPacket(packet, &wire));
    assert(DecodeInputPacket(wire.data(), wire.size(), &decoded));
    assert(decoded.inputs[0] == packet.inputs[0]);

    packet.inputs[0].analogMode = static_cast<AnalogMode>(5);
    assert(!EncodeInputPacket(packet, &wire));
}

void TestCoreBehavior()
{
    RollbackCore core;
    CoreConfig config;
    config.sessionId = 77;
    config.playerCount = 2;
    config.localPlayer = 0;
    config.inputDelay = 2;
    config.maxRollbackFrames = 8;
    assert(core.Reset(config));
    assert(core.ScheduleLocalInput(0, FrameInput(0x10)));

    assert(core.SubmitRemoteInput(1, 0, FrameInput(0x20)) ==
           RemoteInputResult::Accepted);
    auto frame0 = core.PrepareFrame(0);
    assert(frame0.canAdvance);
    assert(core.MarkSimulated(0, frame0));
}

void TestAbsoluteTargetPrediction()
{
    for (bool absolute : {false, true})
    {
        RollbackCore core;
        CoreConfig config{};
        config.sessionId = 0x123456;
        config.localPlayer = 0;
        config.maxRollbackFrames = 12;
        config.predictableButtons = 1;
        config.directTouchIsAbsolute = absolute;
        assert(core.Reset(config));
        FrameInput target(3);
        target.analogMode = AnalogMode::DirectTouch;
        target.x = 8300; target.y = 36500;
        target.unlimited = target.touchUsed = target.touchBomb = true;
        assert(core.SubmitRemoteInput(1, 0, target) == RemoteInputResult::Accepted);
        for (std::uint32_t frame = 0; frame <= 10; ++frame)
        {
            assert(core.ScheduleLocalInput(frame, FrameInput{}));
            const auto decision = core.PrepareFrame(frame);
            assert(decision.canAdvance);
            const auto& predicted = decision.inputs[1];
            assert(predicted.analogMode == AnalogMode::DirectTouch);
            assert(predicted.x == ((absolute || frame <= 1) ? target.x : 0));
            assert(predicted.y == ((absolute || frame <= 1) ? target.y : 0));
            assert(frame == 0 || (!predicted.touchBomb && predicted.buttons == 1));
            assert(core.MarkSimulated(frame, decision));
        }
        assert(core.SubmitRemoteInput(1, 5, FrameInput{}) == RemoteInputResult::RollbackRequired);
        assert(core.RollbackFrame() == 5);
    }
}

void TestEquivalentPredictionConfirmationDoesNotRollback()
{
    RollbackCore core;
    CoreConfig config;
    config.sessionId = 88;
    config.playerCount = 2;
    config.localPlayer = 0;
    config.maxRollbackFrames = 8;
    assert(core.Reset(config));

    assert(core.ScheduleLocalInput(0, FrameInput(1)));
    assert(core.SubmitRemoteInput(1, 0, FrameInput(2)) == RemoteInputResult::Accepted);
    const auto frame0 = core.PrepareFrame(0);
    assert(frame0.canAdvance && core.MarkSimulated(0, frame0));

    assert(core.ScheduleLocalInput(1, FrameInput(1)));
    const auto predicted = core.PrepareFrame(1);
    assert(predicted.canAdvance && (predicted.predictedMask & (1u << 1)) != 0);
    assert(core.MarkSimulated(1, predicted));
    assert(!core.InputPresent(1, 1));

    FrameInput used{};
    bool wasPredicted = false;
    assert(core.UsedInput(1, 1, &used, &wasPredicted));
    assert(wasPredicted && used.buttons == 2);

    assert(core.SubmitEquivalentRemoteInput(1, 2, FrameInput(3)) ==
           EquivalentRemoteInputResult::NotPredicted);
    assert(core.SubmitEquivalentRemoteInput(0, 1, FrameInput(3)) ==
           EquivalentRemoteInputResult::InvalidPlayer);
    assert(core.SubmitEquivalentRemoteInput(1, 1, FrameInput(3)) ==
           EquivalentRemoteInputResult::Confirmed);
    assert(!core.HasRollbackRequest());
    assert(core.InputPresent(1, 1));
    assert(core.ConfirmedThrough(1) == 1);
    const auto authoritative = core.PrepareFrame(1);
    assert(authoritative.canAdvance && authoritative.predictedMask == 0 &&
           authoritative.inputs[1].buttons == 3);

    assert(core.SubmitEquivalentRemoteInput(1, 1, FrameInput(3)) ==
           EquivalentRemoteInputResult::Duplicate);
    assert(core.SubmitEquivalentRemoteInput(1, 1, FrameInput(4)) ==
           EquivalentRemoteInputResult::ConflictingConfirmedInput);
}

void TestAggregateConfirmedRemoteFrontier()
{
    RollbackCore core;
    CoreConfig config;
    config.sessionId = 99;
    config.playerCount = 3;
    config.localPlayer = 0;
    config.maxRollbackFrames = 8;
    assert(core.Reset(config));

    assert(core.ConfirmedThroughAllRemotes() == INVALID_FRAME);
    assert(core.SubmitRemoteInput(1, 0, FrameInput(1)) == RemoteInputResult::Accepted);
    assert(core.ConfirmedThroughAllRemotes() == INVALID_FRAME);
    assert(core.SubmitRemoteInput(2, 0, FrameInput(2)) == RemoteInputResult::Accepted);
    assert(core.ConfirmedThroughAllRemotes() == 0);

    assert(core.SubmitRemoteInput(1, 1, FrameInput(1)) == RemoteInputResult::Accepted);
    assert(core.ConfirmedThroughAllRemotes() == 0);
    assert(core.SubmitRemoteInput(2, 1, FrameInput(2)) == RemoteInputResult::Accepted);
    assert(core.ConfirmedThroughAllRemotes() == 1);
}

SessionPacket PeerPacket(const SessionConfig &config, std::uint8_t player, SessionPhase phase)
{
    SessionPacket packet;
    packet.sessionId = config.sessionId;
    packet.seed = config.seed;
    packet.gameplayAbi = config.gameplayAbi;
    packet.gameId = config.gameId;
    packet.senderPlayer = player;
    packet.playerCount = config.playerCount;
    packet.phase = phase;
    return packet;
}

void TestSessionGate()
{
    SessionConfig config;
    config.sessionId = 0x12345678u;
    config.seed = 0xabcdefu;
    config.gameplayAbi = 5;
    config.gameId = 7;
    config.playerCount = 3;
    config.localPlayer = 0;

    SessionGate gate;
    assert(gate.Reset(config));
    assert(!gate.CanSendReady());
    assert(!gate.CanStart());

    assert(gate.Apply(PeerPacket(config, 1, SessionPhase::Ready)) ==
           SessionPacketResult::ReadyBeforeHello);
    assert(gate.Apply(PeerPacket(config, 1, SessionPhase::Hello)) ==
           SessionPacketResult::Accepted);
    assert(gate.Apply(PeerPacket(config, 2, SessionPhase::Hello)) ==
           SessionPacketResult::Accepted);
    assert(gate.CanSendReady());

    gate.MarkLocalReady();
    assert(gate.LocalReady());
    assert(gate.Apply(PeerPacket(config, 1, SessionPhase::Ready)) ==
           SessionPacketResult::Accepted);
    assert(gate.Apply(PeerPacket(config, 2, SessionPhase::Ready)) ==
           SessionPacketResult::Accepted);
    assert(gate.CanStart());

    SessionPacket mismatch = PeerPacket(config, 1, SessionPhase::Hello);
    mismatch.gameplayAbi++;
    assert(gate.Apply(mismatch) == SessionPacketResult::ContractMismatch);
}

void TestNativeWebSocketStub()
{
#ifndef __EMSCRIPTEN__
    WebSocketTransport transport;
    assert(!transport.Connect("ws://127.0.0.1/unused"));
    assert(transport.Failed());
    assert(!transport.IsOpen());
    assert(transport.LastError() == "WebSocket transport is only implemented for Web/Emscripten");

    const std::uint8_t byte = 1;
    assert(!transport.Send(&byte, 1));
    std::vector<std::uint8_t> packet;
    assert(!transport.Poll(&packet));
    assert(transport.BufferedAmount() == 0);
#endif
}

void TestConfirmedHoldThroughReorderedInput()
{
    RollbackCore core;
    CoreConfig config;
    config.playerCount = 3;
    config.maxRollbackFrames = 8;
    config.predictableButtons = 0xf1;
    config.directionButtons = 0xf0;
    config.maxDirectionPredictionFrames = 2; // Legacy cutoff must not apply.
    config.prediction = InputPrediction::ConfirmedHold;
    assert(core.Reset(config));
    for (unsigned f = 0; f <= 9; ++f) assert(core.ScheduleLocalInput(f, 0));

    assert(core.SubmitRemoteInput(1, 0, 0x10) == RemoteInputResult::Accepted);
    assert(core.SubmitRemoteInput(1, 2, 0x20) == RemoteInputResult::Accepted);
    assert(core.SubmitRemoteInput(2, 0, 0x40) == RemoteInputResult::Accepted);
    assert(core.SubmitRemoteInput(2, 5, 0x80) == RemoteInputResult::Accepted);
    for (unsigned f = 0; f <= 8; ++f)
    {
        const auto d = core.PrepareFrame(f);
        assert(d.canAdvance);
        assert(d.inputs[1].buttons == (f == 2 ? 0x20 : 0x10));
        assert(d.inputs[2].buttons == (f == 5 ? 0x80 : 0x40));
        assert(core.MarkSimulated(f, d));
    }
    assert(!core.PrepareFrame(9).canAdvance);
    // Frame 1 matches and frame 2 was simulated exactly. The new confirmed
    // direction still changes frame 3 onward, only for the affected seat.
    assert(core.SubmitRemoteInput(1, 1, 0x10) == RemoteInputResult::RollbackRequired);
    assert(core.RollbackFrame() == 3 && core.RewindSimulationTo(3));
    for (unsigned f = 3; f <= 8; ++f)
    {
        const auto d = core.PrepareFrame(f);
        assert(d.inputs[1].buttons == 0x20);
        assert(d.inputs[2].buttons == (f == 5 ? 0x80 : 0x40));
        assert(core.MarkSimulated(f, d));
    }
    for (unsigned f = 1; f <= 8; ++f)
    {
        if (f >= 3) assert(core.SubmitRemoteInput(1, f, 0x20) == RemoteInputResult::PredictionCorrect);
        if (f != 5) core.SubmitRemoteInput(2, f, f < 5 ? 0x40 : 0x80);
    }
    assert(core.RollbackFrame() == 6 && core.RewindSimulationTo(6));
    for (unsigned f = 6; f <= 8; ++f)
        assert(core.MarkSimulated(f, core.PrepareFrame(f)));
    for (unsigned f = 0; f <= 8; ++f)
    {
        std::array<FrameInput, MAX_PLAYERS> exact;
        assert(core.ConfirmedInputs(f, &exact));
        assert(exact[1].buttons == (f < 2 ? 0x10 : 0x20));
        assert(exact[2].buttons == (f < 5 ? 0x40 : 0x80));
    }
}

void TestConfirmedHoldTouchSemantics()
{
    for (const auto mode : {AnalogMode::Joystick, AnalogMode::DirectTouch,
                            AnalogMode::DirectTouchBegin, AnalogMode::DirectTouchDelta})
        for (const bool absolute : {false, true})
        {
            RollbackCore core;
            CoreConfig config;
            config.prediction = InputPrediction::ConfirmedHold;
            config.predictableButtons = 0xf1;
            config.directTouchIsAbsolute = absolute;
            config.maxDirectTouchDeltaPredictionFrames = 6;
            assert(core.Reset(config));
            assert(core.ScheduleLocalInput(0, 0) && core.ScheduleLocalInput(1, 0));
            auto sample = FrameInput(0x15);
            sample.analogMode = mode;
            sample.x = 0.75f; sample.y = -0.5f;
            sample.touchUsed = sample.touchBomb = true;
            assert(core.SubmitRemoteInput(1, 0, sample) == RemoteInputResult::Accepted);
            const auto exact = core.PrepareFrame(0);
            assert(exact.inputs[1] == sample && core.MarkSimulated(0, exact));
            const auto d = core.PrepareFrame(1);
            assert(d.canAdvance && d.predictedMask == 2);
            const auto& held = d.inputs[1];
            const bool holdAxes = mode == AnalogMode::Joystick || (mode == AnalogMode::DirectTouch && absolute);
            assert(held.x == (holdAxes ? sample.x : 0) && held.y == (holdAxes ? sample.y : 0));
            assert(held.buttons == 0x11 && held.touchUsed && !held.touchBomb);
            assert(held.analogMode == (mode == AnalogMode::DirectTouchBegin ? AnalogMode::DirectTouchDelta : mode));
        }
}

void TestAbsoluteTouchTrajectory()
{
    RollbackCore core;
    CoreConfig config;
    config.prediction = InputPrediction::ConfirmedHold;
    config.directTouchIsAbsolute = true;
    config.absoluteTouchForecastFrames = 6;
    assert(core.Reset(config));
    const auto touch = [](float x) {
        FrameInput sample(1);
        sample.analogMode = AnalogMode::DirectTouch;
        sample.unlimited = true;
        sample.x = x;
        sample.y = 350;
        return sample;
    };
    for (unsigned frame=0; frame<=12; ++frame)
        assert(core.ScheduleLocalInput(frame,0));
    for (unsigned frame=0; frame<5; ++frame) {
        assert(core.SubmitRemoteInput(1,frame,touch(float(frame)*2)) == RemoteInputResult::Accepted);
        assert(core.MarkSimulated(frame,core.PrepareFrame(frame)));
    }
    assert(core.PrepareFrame(5).inputs[1].x == 10);
    assert(core.PrepareFrame(10).inputs[1].x == 20);
    assert(core.PrepareFrame(12).inputs[1].x == 20);
    // A later packet cannot affect the forecast across a gap.
    assert(core.SubmitRemoteInput(1,6,touch(6)) == RemoteInputResult::Accepted);
    assert(core.PrepareFrame(7).inputs[1].x == 14);
    assert(core.MarkSimulated(5,core.PrepareFrame(5)));
    assert(core.MarkSimulated(6,core.PrepareFrame(6)));
    assert(core.MarkSimulated(7,core.PrepareFrame(7)));
    // Closing the gap confirms the prediction at frame 5, but exposes a
    // turn in the contiguous trajectory and invalidates the frame 7 tail.
    assert(core.SubmitRemoteInput(1,5,touch(10)) == RemoteInputResult::RollbackRequired);
    assert(core.RollbackFrame() == 7);
    assert(core.PrepareFrame(7).inputs[1].x == 6);
    assert(core.RewindSimulationTo(7));
    assert(core.MarkSimulated(7,core.PrepareFrame(7)));
    assert(core.SubmitRemoteInput(1,7,touch(60)) == RemoteInputResult::RollbackRequired);
    assert(core.RollbackFrame() == 7);
    assert(core.PrepareFrame(8).inputs[1].x == 60); // discontinuous drag
    assert(core.SubmitRemoteInput(1,8,FrameInput(0)) == RemoteInputResult::Accepted);
    assert(core.PrepareFrame(9).inputs[1].analogMode == AnalogMode::None);
}

void TestConfirmedTapPatternReturnsToHold()
{
    RollbackCore core;
    CoreConfig config;
    config.prediction = InputPrediction::ConfirmedHold;
    config.directionButtons = 0xf0;
    config.predictableButtons = 0xf1;
    assert(core.Reset(config));
    constexpr std::uint16_t words[]{16,16,0,32,32,0,16,16,0,17};
    for (unsigned f = 0; f <= 13; ++f)
    {
        assert(core.ScheduleLocalInput(f, 0));
        if (f < std::size(words)) core.SubmitRemoteInput(1, f, words[f]);
        const auto d = core.PrepareFrame(f);
        assert(d.canAdvance && core.MarkSimulated(f, d));
        if (f >= std::size(words)) assert(d.inputs[1].buttons == (f < 12 ? 17 : 1));
    }
    for (unsigned f = 10; f < 12; ++f)
        assert(core.SubmitRemoteInput(1, f, 17) == RemoteInputResult::PredictionCorrect);
    assert(core.SubmitRemoteInput(1, 12, 17) == RemoteInputResult::RollbackRequired);
    assert(core.RollbackFrame() == 12 && core.RewindSimulationTo(12));
    assert(core.MarkSimulated(12, core.PrepareFrame(12)));
    const auto repaired = core.PrepareFrame(13);
    assert(repaired.canAdvance && repaired.inputs[1].buttons == 17);
}

void TestConfirmedHoldIgnoresIntermediatePacketAnchors()
{
    RollbackCore core;
    CoreConfig config;
    config.sessionId = 44;
    config.prediction = InputPrediction::ConfirmedHold;
    assert(core.Reset(config));
    core.SubmitRemoteInput(1, 0, 16);
    core.SubmitRemoteInput(1, 2, 32);
    for (unsigned f = 0; f <= 3; ++f)
    {
        assert(core.ScheduleLocalInput(f, 0));
        assert(core.MarkSimulated(f, core.PrepareFrame(f)));
    }
    InputPacket packet;
    packet.sessionId = 44;
    packet.senderPlayer = 1;
    packet.playerCount = 2;
    packet.firstInputFrame = 1;
    packet.latestFrame = 3;
    packet.inputCount = 3;
    packet.inputs[0] = FrameInput(16);
    packet.inputs[1] = FrameInput(32);
    packet.inputs[2] = FrameInput(16);
    assert(core.ApplyInputPacket(packet) && !core.HasRollbackRequest());
    std::array<FrameInput, MAX_PLAYERS> confirmed;
    assert(core.ConfirmedInputs(3, &confirmed) && confirmed[1].buttons == 16);
}

void TestForecastRewindCanBeSupersededAcrossPackets()
{
    for (bool consultBetweenPackets : {false, true})
        for (bool matches : {false, true})
        {
            RollbackCore core;
            CoreConfig config;
            config.sessionId = 44;
            config.playerCount = 3;
            config.prediction = InputPrediction::ConfirmedHold;
            assert(core.Reset(config));
            core.SubmitRemoteInput(1, 0, 16);
            core.SubmitRemoteInput(1, 2, 32);
            for (unsigned f = 0; f <= 3; ++f)
            {
                assert(core.ScheduleLocalInput(f, 0));
                core.SubmitRemoteInput(2, f, 0);
                assert(core.MarkSimulated(f, core.PrepareFrame(f)));
            }
            auto packet = [&](unsigned frame, std::uint16_t buttons) {
                InputPacket p;
                p.sessionId = 44; p.senderPlayer = 1; p.playerCount = 3;
                p.firstInputFrame = p.latestFrame = frame;
                p.inputCount = 1; p.inputs[0] = FrameInput(buttons);
                return p;
            };
            assert(core.ApplyInputPacket(packet(1, 16)));
            if (consultBetweenPackets) assert(core.RollbackFrame() == 3);
            assert(core.ApplyInputPacket(packet(3, matches ? 16 : 32)));
            // A diagnostic read between packets must not promote a forecast
            // to a hard mismatch or block the confirmed replay stream.
            assert(core.RollbackFrame() == (matches ? INVALID_FRAME : 3));
            std::array<FrameInput, MAX_PLAYERS> confirmed;
            assert(core.ConfirmedInputs(3, &confirmed) == matches);
            if (!matches)
            {
                assert(core.RewindSimulationTo(3));
                assert(core.MarkSimulated(3, core.PrepareFrame(3)));
                assert(core.ConfirmedInputs(3, &confirmed) && confirmed[1].buttons == 32);
            }
        }
}
} // namespace

int main()
{
    TestProtocolCapability();
    TestCoreBehavior();
    TestAbsoluteTargetPrediction();
    TestEquivalentPredictionConfirmationDoesNotRollback();
    TestAggregateConfirmedRemoteFrontier();
    TestConfirmedHoldThroughReorderedInput();
    TestConfirmedHoldTouchSemantics();
    TestAbsoluteTouchTrajectory();
    TestConfirmedTapPatternReturnsToHold();
    TestConfirmedHoldIgnoresIntermediatePacketAnchors();
    TestForecastRewindCanBeSupersededAcrossPackets();
    TestSessionGate();
    TestNativeWebSocketStub();
    std::cout << "eagler-common netplay base: PASS\n";
    return 0;
}
