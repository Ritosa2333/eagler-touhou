#include "netplay/NetplayCore.hpp"
#include "netplay/NetplaySession.hpp"

#include <cassert>
#include <cstdint>
#include <iostream>
#include <vector>

using namespace Netplay;

static void TestProtocolRoundTrip()
{
    InputPacket packet;
    packet.sessionId = 0x123456789abcdef0ull;
    packet.sequence = 99;
    packet.ackSequence = 96;
    packet.senderPlayer = 1;
    packet.playerCount = 3;
    packet.latestFrame = 12;
    packet.ackFrame = 10;
    packet.firstInputFrame = 9;
    packet.senderFrame = 14;
    packet.frameAdvantage = -3;
    packet.inputCount = 4;
    packet.inputs[0] = 1;
    packet.inputs[1] = 2;
    packet.inputs[1].analogMode = AnalogMode::DirectTouch;
    packet.inputs[1].x = 12.5f;
    packet.inputs[1].y = -3.25f;
    packet.inputs[1].unlimited = true;
    packet.inputs[1].touchUsed = true;
    packet.inputs[1].touchBomb = true;
    packet.inputs[2] = 3;
    packet.inputs[3] = 4;

    std::vector<std::uint8_t> wire;
    assert(EncodeInputPacket(packet, &wire));
    assert(wire.size() >= 4 && wire[0] == 'E' && wire[1] == '6' && wire[2] == 'N' && wire[3] == 'P');
    InputPacket decoded;
    assert(DecodeInputPacket(wire.data(), wire.size(), &decoded));
    assert(decoded.sessionId == packet.sessionId);
    assert(decoded.sequence == 99 && decoded.ackSequence == 96);
    assert(decoded.senderPlayer == 1 && decoded.playerCount == 3);
    assert(decoded.firstInputFrame == 9 && decoded.latestFrame == 12);
    assert(decoded.senderFrame == 14 && decoded.frameAdvantage == -3);
    assert(decoded.inputCount == 4 && decoded.inputs[3] == 4);
    assert(decoded.inputs[1] == packet.inputs[1]);
    wire[4]++;
    assert(!DecodeInputPacket(wire.data(), wire.size(), &decoded));

    assert(!DecodeInputPacket(wire.data(), 10, &decoded));

    InputPacket overflow = packet;
    overflow.firstInputFrame = INVALID_FRAME - 1;
    overflow.latestFrame = 1;
    overflow.inputCount = 4;
    assert(!EncodeInputPacket(overflow, &wire));

    SessionPacket hello;
    hello.sessionId = 0x1111222233334444ull;
    hello.seed = 0x4a3d;
    hello.gameplayAbi = 1;
    hello.gameId = 6;
    hello.senderPlayer = 1;
    hello.playerCount = 2;
    hello.phase = SessionPhase::Hello;
    assert(EncodeSessionPacket(hello, &wire));
    PacketType type;
    assert(PeekPacketType(wire.data(), wire.size(), &type));
    assert(type == PacketType::Session);
    SessionPacket decodedHello;
    assert(DecodeSessionPacket(wire.data(), wire.size(), &decodedHello));
    assert(decodedHello.sessionId == hello.sessionId);
    assert(decodedHello.seed == hello.seed && decodedHello.gameplayAbi == 1);
    assert(decodedHello.gameId == 6 && decodedHello.senderPlayer == 1);
    assert(decodedHello.phase == SessionPhase::Hello);

    SpectatorFramePacket spectator;
    spectator.sessionId = packet.sessionId;
    spectator.frame = 42;
    spectator.gameplayAbi = 3;
    spectator.playerCount = 3;
    spectator.inputs[0] = packet.inputs[0];
    spectator.inputs[1] = packet.inputs[1];
    spectator.inputs[2] = packet.inputs[2];
    assert(EncodeSpectatorFramePacket(spectator, &wire));
    assert(PeekPacketType(wire.data(), wire.size(), &type));
    assert(type == PacketType::SpectatorFrame);
    SpectatorFramePacket decodedSpectator;
    assert(DecodeSpectatorFramePacket(wire.data(), wire.size(), &decodedSpectator));
    assert(decodedSpectator.sessionId == spectator.sessionId);
    assert(decodedSpectator.gameplayAbi == spectator.gameplayAbi);
    assert(decodedSpectator.frame == 42 && decodedSpectator.playerCount == 3);
    assert(decodedSpectator.inputs[1] == spectator.inputs[1]);
    wire.push_back(0);
    assert(!DecodeSpectatorFramePacket(wire.data(), wire.size(), &decodedSpectator));
}

static SessionConfig MakeSessionConfig(std::uint8_t localPlayer)
{
    SessionConfig config;
    config.sessionId = 0x123456789abcdef0ull;
    config.seed = 0x4a3d;
    config.gameplayAbi = 1;
    config.gameId = 6;
    config.playerCount = 2;
    config.localPlayer = localPlayer;
    return config;
}

static void TestSessionGate()
{
    SessionGate a;
    SessionGate b;
    assert(a.Reset(MakeSessionConfig(0)));
    assert(b.Reset(MakeSessionConfig(1)));
    assert(!a.CanStart() && !b.CanStart());

    // READY can overtake HELLO on an unordered/future transport. It is ignored
    // until HELLO arrives; periodic READY retransmission then completes safely.
    SessionPacket prematureReady = b.BuildPacket(SessionPhase::Ready);
    assert(a.Apply(prematureReady) == SessionPacketResult::ReadyBeforeHello);
    assert(!a.PeerReady(1));

    assert(a.Apply(b.BuildPacket(SessionPhase::Hello)) == SessionPacketResult::Accepted);
    assert(b.Apply(a.BuildPacket(SessionPhase::Hello)) == SessionPacketResult::Accepted);
    assert(a.CanSendReady() && b.CanSendReady());
    a.MarkLocalReady();
    b.MarkLocalReady();
    assert(a.LocalReady() && b.LocalReady());
    assert(a.Apply(b.BuildPacket(SessionPhase::Ready)) == SessionPacketResult::Accepted);
    assert(b.Apply(a.BuildPacket(SessionPhase::Ready)) == SessionPacketResult::Accepted);
    assert(a.CanStart() && b.CanStart());

    SessionGate fresh;
    assert(fresh.Reset(MakeSessionConfig(0)));
    SessionPacket mismatch = b.BuildPacket(SessionPhase::Hello);
    mismatch.gameplayAbi++;
    assert(fresh.Apply(mismatch) == SessionPacketResult::ContractMismatch);

}

static void TestPredictionAndRollback()
{
    RollbackCore core;
    CoreConfig cfg;
    cfg.sessionId = 7;
    cfg.playerCount = 2;
    cfg.localPlayer = 0;
    cfg.maxRollbackFrames = 8;
    assert(core.Reset(cfg));

    assert(core.ScheduleLocalInput(0, 0x10));
    assert(core.SubmitRemoteInput(1, 0, 0x20) == RemoteInputResult::Accepted);
    FrameDecision f0 = core.PrepareFrame(0);
    assert(f0.canAdvance && f0.predictedMask == 0);
    assert(core.MarkSimulated(0, f0));

    assert(core.ScheduleLocalInput(1, 0x11));
    FrameDecision f1 = core.PrepareFrame(1);
    assert(f1.canAdvance && (f1.predictedMask & 0x02) != 0);
    assert(f1.inputs[1] == 0x20);
    assert(core.MarkSimulated(1, f1));

    assert(core.SubmitRemoteInput(1, 1, 0x21) == RemoteInputResult::RollbackRequired);
    assert(core.HasRollbackRequest() && core.RollbackFrame() == 1);
    core.ClearRollbackRequest();

    FrameDecision replay = core.PrepareFrame(1);
    assert(replay.canAdvance && replay.predictedMask == 0 && replay.inputs[1] == 0x21);
    assert(core.MarkSimulated(1, replay));
}

static void TestAnalogPredictionAndRollback()
{
    RollbackCore core;
    CoreConfig cfg;
    cfg.sessionId = 17;
    cfg.playerCount = 2;
    cfg.localPlayer = 0;
    cfg.maxRollbackFrames = 8;
    assert(core.Reset(cfg));

    FrameInput initial;
    initial.buttons = 0x10;
    initial.analogMode = AnalogMode::DirectTouch;
    initial.x = 4.0f;
    initial.y = -2.0f;
    initial.touchUsed = true;
    assert(core.ScheduleLocalInput(0, FrameInput{}));
    assert(core.SubmitRemoteInput(1, 0, initial) == RemoteInputResult::Accepted);
    auto frame0 = core.PrepareFrame(0);
    assert(frame0.canAdvance && core.MarkSimulated(0, frame0));

    assert(core.ScheduleLocalInput(1, FrameInput{}));
    auto predicted = core.PrepareFrame(1);
    assert(predicted.canAdvance);
    assert(predicted.inputs[1].analogMode == AnalogMode::DirectTouch);
    assert(predicted.inputs[1].x == initial.x && predicted.inputs[1].y == initial.y);
    assert(predicted.inputs[1].buttons == 0x10 && predicted.inputs[1].touchUsed);
    assert(core.MarkSimulated(1, predicted));

    assert(core.ScheduleLocalInput(2, FrameInput{}));
    auto secondMissing = core.PrepareFrame(2);
    assert(secondMissing.canAdvance);
    assert(secondMissing.inputs[1].x == 0.0f && secondMissing.inputs[1].y == 0.0f);

    FrameInput changed = initial;
    changed.x = 7.5f;
    assert(core.SubmitRemoteInput(1, 1, changed) == RemoteInputResult::RollbackRequired);
    auto corrected = core.PrepareFrame(1);
    assert(corrected.canAdvance && corrected.inputs[1] == changed);
}

static void TestPredictionFiltersEdgeInputs()
{
    RollbackCore core;
    CoreConfig cfg;
    cfg.sessionId = 18;
    cfg.playerCount = 2;
    cfg.localPlayer = 0;
    cfg.maxRollbackFrames = 8;
    cfg.predictableButtons = 0x01f5; // TH07 direction/focus/shoot/skip
    assert(core.Reset(cfg));

    FrameInput initial;
    initial.buttons = 0x01ff; // includes Bomb and Menu edge bits
    initial.analogMode = AnalogMode::DirectTouch;
    initial.x = 3.0f;
    initial.y = -1.0f;
    initial.touchUsed = true;
    initial.touchBomb = true;
    assert(core.ScheduleLocalInput(0, FrameInput{}));
    assert(core.SubmitRemoteInput(1, 0, initial) == RemoteInputResult::Accepted);
    const auto exact = core.PrepareFrame(0);
    assert(exact.canAdvance && core.MarkSimulated(0, exact));

    assert(core.ScheduleLocalInput(1, FrameInput{}));
    const auto predicted = core.PrepareFrame(1);
    assert(predicted.canAdvance);
    assert(predicted.inputs[1].buttons == (initial.buttons & cfg.predictableButtons));
    assert(!predicted.inputs[1].touchBomb);
    assert(predicted.inputs[1].touchUsed);
    assert(predicted.inputs[1].x == initial.x && predicted.inputs[1].y == initial.y);
}

static void TestDirectionPredictionHasIndependentHorizon()
{
    RollbackCore core;
    CoreConfig cfg;
    cfg.sessionId = 19;
    cfg.playerCount = 2;
    cfg.localPlayer = 0;
    cfg.maxRollbackFrames = 8;
    cfg.predictableButtons = 0x01f5;
    cfg.directionButtons = 0x000f;
    cfg.maxDirectionPredictionFrames = 3;
    assert(core.Reset(cfg));

    FrameInput held;
    held.buttons = 0x0111; // direction + Focus + Shoot
    assert(core.ScheduleLocalInput(0, FrameInput{}));
    assert(core.SubmitRemoteInput(1, 0, held) == RemoteInputResult::Accepted);
    const auto exact = core.PrepareFrame(0);
    assert(exact.canAdvance && core.MarkSimulated(0, exact));

    for (std::uint32_t frame = 1; frame <= 3; ++frame)
    {
        assert(core.ScheduleLocalInput(frame, FrameInput{}));
        const auto predicted = core.PrepareFrame(frame);
        assert(predicted.canAdvance);
        assert((predicted.inputs[1].buttons & cfg.directionButtons) ==
               (held.buttons & cfg.directionButtons));
        assert(core.MarkSimulated(frame, predicted));
    }

    assert(core.ScheduleLocalInput(4, FrameInput{}));
    const auto capped = core.PrepareFrame(4);
    assert(capped.canAdvance);
    assert((capped.inputs[1].buttons & cfg.directionButtons) == 0);
    assert((capped.inputs[1].buttons & 0x0110) == 0x0110);

    // The rollback horizon remains eight frames; only predicted movement is
    // neutral after frame three.
    assert(core.MarkSimulated(4, capped));
    for (std::uint32_t frame = 5; frame <= 8; ++frame)
    {
        assert(core.ScheduleLocalInput(frame, FrameInput{}));
        const auto predicted = core.PrepareFrame(frame);
        assert(predicted.canAdvance);
        assert((predicted.inputs[1].buttons & cfg.directionButtons) == 0);
        assert(core.MarkSimulated(frame, predicted));
    }
    assert(core.ScheduleLocalInput(9, FrameInput{}));
    assert(!core.PrepareFrame(9).canAdvance);
}

static void TestCorrectPredictionDoesNotRollback()
{
    RollbackCore core;
    CoreConfig cfg;
    cfg.sessionId = 8;
    cfg.playerCount = 2;
    cfg.localPlayer = 0;
    cfg.maxRollbackFrames = 8;
    assert(core.Reset(cfg));
    assert(core.ScheduleLocalInput(0, 1));
    assert(core.SubmitRemoteInput(1, 0, 2) == RemoteInputResult::Accepted);
    auto f0 = core.PrepareFrame(0);
    assert(f0.canAdvance && core.MarkSimulated(0, f0));
    assert(core.ScheduleLocalInput(1, 1));
    auto f1 = core.PrepareFrame(1);
    assert(f1.canAdvance && core.MarkSimulated(1, f1));
    assert(core.SubmitRemoteInput(1, 1, 2) == RemoteInputResult::PredictionCorrect);
    assert(!core.HasRollbackRequest());
}

static void TestRollbackBudgetStalls()
{
    RollbackCore core;
    CoreConfig cfg;
    cfg.sessionId = 9;
    cfg.playerCount = 2;
    cfg.localPlayer = 0;
    cfg.maxRollbackFrames = 3;
    assert(core.Reset(cfg));
    assert(core.SubmitRemoteInput(1, 0, 0) == RemoteInputResult::Accepted);
    for (std::uint32_t frame = 0; frame < 5; ++frame)
    {
        assert(core.ScheduleLocalInput(frame, 0));
        auto decision = core.PrepareFrame(frame);
        if (frame <= 3)
        {
            assert(decision.canAdvance);
            assert(core.MarkSimulated(frame, decision));
        }
        else
            assert(!decision.canAdvance);
    }
}

static void TestInputDelayAndPacketRedundancy()
{
    RollbackCore a;
    RollbackCore b;
    CoreConfig ca;
    ca.sessionId = 10;
    ca.playerCount = 2;
    ca.localPlayer = 0;
    ca.inputDelay = 2;
    CoreConfig cb = ca;
    cb.localPlayer = 1;
    assert(a.Reset(ca) && b.Reset(cb));
    assert(a.ScheduleLocalInput(0, 0x31)); // simulation frame 2
    assert(a.ScheduleLocalInput(1, 0x32)); // simulation frame 3
    assert(a.ScheduleLocalInput(2, 0x33)); // simulation frame 4

    InputPacket p = a.BuildInputPacket(1, 4, 5, 4);
    assert(p.firstInputFrame == 0 && p.inputCount == 5);
    std::vector<std::uint8_t> wire;
    assert(EncodeInputPacket(p, &wire));
    InputPacket decoded;
    assert(DecodeInputPacket(wire.data(), wire.size(), &decoded));
    RemoteInputResult result;
    assert(b.ApplyInputPacket(decoded, &result));
    assert(b.ConfirmedThrough(0) == 4);
}

static void TestThreePlayerIndependentPrediction()
{
    RollbackCore core;
    CoreConfig cfg;
    cfg.sessionId = 11;
    cfg.playerCount = 3;
    cfg.localPlayer = 0;
    cfg.maxRollbackFrames = 8;
    assert(core.Reset(cfg));
    assert(core.ScheduleLocalInput(0, 1));
    assert(core.SubmitRemoteInput(1, 0, 2) == RemoteInputResult::Accepted);
    assert(core.SubmitRemoteInput(2, 0, 4) == RemoteInputResult::Accepted);
    auto f0 = core.PrepareFrame(0);
    assert(f0.canAdvance && core.MarkSimulated(0, f0));

    assert(core.ScheduleLocalInput(1, 1));
    assert(core.SubmitRemoteInput(1, 1, 3) == RemoteInputResult::Accepted);
    auto f1 = core.PrepareFrame(1);
    assert(f1.canAdvance && f1.inputs[1] == 3 && f1.inputs[2] == 4);
    assert((f1.predictedMask & (1u << 1)) == 0);
    assert((f1.predictedMask & (1u << 2)) != 0);
    assert(core.MarkSimulated(1, f1));
    assert(core.SubmitRemoteInput(2, 1, 5) == RemoteInputResult::RollbackRequired);
    assert(core.RollbackFrame() == 1);
}

static void TestLossReorderDuplicateAndAckShrink()
{
    RollbackCore a;
    RollbackCore b;
    CoreConfig ca;
    ca.sessionId = 12;
    ca.playerCount = 2;
    ca.localPlayer = 0;
    CoreConfig cb = ca;
    cb.localPlayer = 1;
    assert(a.Reset(ca) && b.Reset(cb));

    for (std::uint32_t frame = 0; frame <= 7; ++frame)
    {
        assert(a.ScheduleLocalInput(frame, static_cast<std::uint16_t>(0x100 + frame)));
        assert(b.ScheduleLocalInput(frame, static_cast<std::uint16_t>(0x200 + frame)));
    }

    // Pretend a's frame-0..4 packet was lost entirely. Since b has never
    // acknowledged a frame, the later packet still carries the whole missing
    // range and reconstructs it without transport-level retransmission.
    const InputPacket lost = a.BuildInputPacket(1, 4, 1, 0);
    assert(lost.firstInputFrame == 0 && lost.inputCount == 5);
    const InputPacket recovery = a.BuildInputPacket(1, 7, 2, 0);
    assert(recovery.firstInputFrame == 0 && recovery.inputCount == 8);
    RemoteInputResult worst;
    assert(b.ApplyInputPacket(recovery, &worst));
    assert(b.ConfirmedThrough(0) == 7);

    // A stale/reordered older packet is now entirely duplicate and must not
    // disturb the confirmed timeline.
    assert(b.ApplyInputPacket(lost, &worst));
    assert(b.ConfirmedThrough(0) == 7);

    // Deliver the exact recovery packet again: duplicate packets are harmless.
    assert(b.ApplyInputPacket(recovery, &worst));
    assert(b.ConfirmedThrough(0) == 7);

    // b's reply acknowledges a through frame 7. Once a consumes that ACK,
    // its next packet starts at frame 8 instead of repeating old history.
    const InputPacket reply = b.BuildInputPacket(0, 7, 3, 2);
    assert(reply.ackFrame == 7);
    assert(a.ApplyInputPacket(reply, &worst));
    assert(a.ScheduleLocalInput(8, 0x108));
    const InputPacket shrunk = a.BuildInputPacket(1, 8, 4, 3);
    assert(shrunk.firstInputFrame == 8 && shrunk.inputCount == 1);
}

static void TestFinalUpstreamRedundancyWindow()
{
    RollbackCore core;
    CoreConfig cfg;
    cfg.sessionId = 19;
    cfg.playerCount = 2;
    cfg.localPlayer = 0;
    assert(core.Reset(cfg));
    for (std::uint32_t frame = 0; frame < 40; ++frame)
        assert(core.ScheduleLocalInput(frame, static_cast<std::uint16_t>(frame)));

    const InputPacket packet = core.BuildInputPacket(1, 39, 1, 0);
    assert(MAX_REDUNDANT_INPUTS == 32);
    assert(packet.firstInputFrame == 8);
    assert(packet.latestFrame == 39);
    assert(packet.inputCount == 32);
}

int main()
{
    TestProtocolRoundTrip();
    TestSessionGate();
    TestPredictionAndRollback();
    TestAnalogPredictionAndRollback();
    TestPredictionFiltersEdgeInputs();
    TestDirectionPredictionHasIndependentHorizon();
    TestCorrectPredictionDoesNotRollback();
    TestRollbackBudgetStalls();
    TestInputDelayAndPacketRedundancy();
    TestThreePlayerIndependentPrediction();
    TestLossReorderDuplicateAndAckShrink();
    TestFinalUpstreamRedundancyWindow();
    std::cout << "TH06 netplay core: PASS\n";
    return 0;
}
