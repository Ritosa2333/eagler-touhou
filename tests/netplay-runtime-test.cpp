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
    return 0;
}
