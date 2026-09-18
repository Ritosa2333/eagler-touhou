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

    packet.inputs[0].analogMode = static_cast<AnalogMode>(3);
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
} // namespace

int main()
{
    TestProtocolCapability();
    TestCoreBehavior();
    TestSessionGate();
    TestNativeWebSocketStub();
    std::cout << "eagler-common netplay base: PASS\n";
    return 0;
}
