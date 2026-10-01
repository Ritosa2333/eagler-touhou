#include "../th10_web/cpp/multiplayer/NetplayRuntime.hpp"
#include "../th10_web/cpp/multiplayer/InputLanes.hpp"

#include <cassert>

using namespace th10::multiplayer;

int main() {
    SessionSetup setup{};
    setup.playerCount = 2;
    setup.localPlayer = 0;
    setup.difficulty = 2;
    setup.seed = 1234;
    setup.loadouts[0] = {0, 0};
    setup.loadouts[1] = {1, 2};
    setup.configured = true;

    NetplayRuntime host, peer;
    constexpr std::uint64_t sessionId = 0x1020304050607080ull;
    assert(host.Reset(setup, sessionId));
    setup.localPlayer = 1;
    assert(peer.Reset(setup, sessionId));
    assert(!host.LocalReady()&&!host.CaptureLocal(0,Netplay::FrameInput(1)));
    assert(!host.Prepare(0).canAdvance);
    Netplay::FrameDecision forged{};forged.canAdvance=true;
    assert(!host.MarkSimulated(0,forged));

    assert(host.ApplySession(peer.Hello()) == Netplay::SessionPacketResult::Accepted);
    assert(peer.ApplySession(host.Hello()) == Netplay::SessionPacketResult::Accepted);
    assert(host.CanSendReady() && peer.CanSendReady());
    host.MarkLocalReady();
    peer.MarkLocalReady();
    assert(host.ApplySession(peer.Ready()) == Netplay::SessionPacketResult::Accepted);
    assert(peer.ApplySession(host.Ready()) == Netplay::SessionPacketResult::Accepted);
    assert(host.CanStart() && peer.CanStart());

    Netplay::FrameInput local{};
    local.buttons = InputLanes::kShoot | 0x10;
    assert(host.CaptureLocal(0, local));
    auto predicted = host.Prepare(0);
    assert(predicted.canAdvance);
    assert((predicted.predictedMask & (1u << 1)) != 0);
    assert(host.MarkSimulated(0, predicted));

    // Direction/shoot prediction differs when the exact remote sample arrives,
    // so the common core must request correction from the first divergent frame.
    Netplay::FrameInput remote{};
    remote.buttons = InputLanes::kFocus | 0x20;
    assert(host.SubmitRemote(1, 0, remote) == Netplay::RemoteInputResult::RollbackRequired);
    assert(host.HasRollbackRequest());
    assert(host.RollbackFrame() == 0);

    host.ClearRollbackRequest();
    assert(!host.HasRollbackRequest());

    // Test the title's real config, not just an opt-in common-core fixture.
    Netplay::FrameInput target{};
    target.analogMode=Netplay::AnalogMode::DirectTouch;
    target.x=8300;target.y=36500;target.unlimited=true;target.touchUsed=true;
    assert(host.CaptureLocal(1,local));
    assert(host.SubmitRemote(1,1,target)==Netplay::RemoteInputResult::Accepted);
    auto exact=host.Prepare(1);assert(exact.canAdvance&&host.MarkSimulated(1,exact));
    for(std::uint32_t frame=2;frame<10;++frame){
        assert(host.CaptureLocal(frame,local));
        auto decision=host.Prepare(frame);
        assert(decision.canAdvance&&decision.inputs[1].analogMode==Netplay::AnalogMode::DirectTouch);
        assert(decision.inputs[1].x==target.x&&decision.inputs[1].y==target.y);
        assert(host.MarkSimulated(frame,decision));
    }

    SessionSetup delayedSetup=setup;
    delayedSetup.localPlayer=0;
    delayedSetup.input_delay=3;
    NetplayRuntime delayedHost,delayedPeer;
    assert(delayedHost.Reset(delayedSetup,sessionId+1));
    delayedSetup.localPlayer=1;
    assert(delayedPeer.Reset(delayedSetup,sessionId+1));
    assert(delayedHost.ApplySession(delayedPeer.Hello())==Netplay::SessionPacketResult::Accepted);
    assert(delayedPeer.ApplySession(delayedHost.Hello())==Netplay::SessionPacketResult::Accepted);
    delayedHost.MarkLocalReady();delayedPeer.MarkLocalReady();
    assert(delayedHost.ApplySession(delayedPeer.Ready())==Netplay::SessionPacketResult::Accepted);
    assert(delayedPeer.ApplySession(delayedHost.Ready())==Netplay::SessionPacketResult::Accepted);
    assert(delayedHost.CaptureLocal(0,local));
    auto leadIn=delayedHost.Prepare(0);assert(leadIn.canAdvance);assert(leadIn.inputs[0]==Netplay::FrameInput{});
    std::vector<std::uint8_t> wire;
    assert(delayedHost.BuildInputWire(1,0,1,0,wire));
    Netplay::InputPacket packet{};assert(Netplay::DecodeInputPacket(wire.data(),wire.size(),&packet));
    assert(packet.latestFrame==3);

    SessionSetup mismatched=delayedSetup;mismatched.localPlayer=0;mismatched.input_delay=2;
    NetplayRuntime mismatchHost;assert(mismatchHost.Reset(mismatched,sessionId+2));
    mismatched.localPlayer=1;mismatched.input_delay=3;
    NetplayRuntime mismatchPeer;assert(mismatchPeer.Reset(mismatched,sessionId+2));
    assert(mismatchHost.ApplySession(mismatchPeer.Hello())==Netplay::SessionPacketResult::ContractMismatch);
    return 0;
}
