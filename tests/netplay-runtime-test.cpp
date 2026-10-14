#include "../th10_web/cpp/multiplayer/NetplayRuntime.hpp"
#include "../th10_web/cpp/multiplayer/InputLanes.hpp"

#include <cassert>
#include <limits>

using namespace th10::multiplayer;

namespace {
SessionSetup tail_setup(std::uint32_t delay=0){
    SessionSetup setup{};
    setup.version=5;setup.build[0]=1;setup.playerCount=2;
    setup.difficulty=2;setup.seed=1234;setup.input_delay=delay;
    setup.loadouts[0]={0,0};setup.loadouts[1]={1,2};setup.configured=true;
    return setup;
}
void tail_handshake(NetplayRuntime& a,NetplayRuntime& b){
    assert(a.ApplySession(b.Hello())==Netplay::SessionPacketResult::Accepted);
    assert(b.ApplySession(a.Hello())==Netplay::SessionPacketResult::Accepted);
    a.MarkLocalReady();b.MarkLocalReady();
    assert(a.ApplySession(b.Ready())==Netplay::SessionPacketResult::Accepted);
    assert(b.ApplySession(a.Ready())==Netplay::SessionPacketResult::Accepted);
    assert(a.CanStart()&&b.CanStart());
}
void assert_tail_choices(const NetplayRuntime& runtime,bool pacing,bool presentation,bool locked){
    assert(runtime.NetworkGapTailEnabled()==pacing);
    assert(runtime.NetworkTailPresentationEnabled()==presentation);
    assert(runtime.NetworkGapTailLocked()==locked&&runtime.NetworkTailPresentationLocked()==locked);
}
void test_network_gap_tail_startup_lock(){
    for(bool enabled:{false,true})for(bool extra_draw:{false,true})for(std::uint32_t delay:{0u,3u}){
        auto setup=tail_setup(delay),other=setup;other.localPlayer=1;
        NetplayRuntime host,peer;
        assert_tail_choices(host,false,false,false);
        assert(!host.SetNetworkGapTailEnabled(enabled));
        assert(!host.SetNetworkTailPresentationEnabled(extra_draw));
        assert(host.Reset(setup,91)&&peer.Reset(other,91));
        assert_tail_choices(host,true,false,false);
        assert_tail_choices(peer,true,false,false);
        assert(host.InputGapScale()==1.&&host.IntervalScale()==peer.IntervalScale());
        // The input-delay neutral prefix is not a physical capture at D.
        assert(!host.HasLocalCapture(0));
        // Change both in either order. Neither choice may overwrite the other.
        assert(host.SetNetworkTailPresentationEnabled(!extra_draw));
        assert_tail_choices(host,true,!extra_draw,false);
        assert(host.SetNetworkGapTailEnabled(!enabled));
        assert_tail_choices(host,!enabled,!extra_draw,false);
        const double startup_scale=host.InputGapScale(),startup_effective=host.IntervalScale();
        assert(host.SetNetworkTailPresentationEnabled(extra_draw));
        assert_tail_choices(host,!enabled,extra_draw,false);
        assert(host.InputGapScale()==startup_scale&&host.IntervalScale()==startup_effective);
        assert(host.SetNetworkGapTailEnabled(enabled));
        assert_tail_choices(host,enabled,extra_draw,false);
        // A peer may use the opposite choices without changing the wire ABI
        // or the logical frame prepared from the same authoritative inputs.
        assert(peer.SetNetworkGapTailEnabled(!enabled));
        assert(peer.SetNetworkTailPresentationEnabled(!extra_draw));
        assert_tail_choices(peer,!enabled,!extra_draw,false);
        assert(host.Config().gameplayAbi==peer.Config().gameplayAbi);
        assert(!host.CaptureLocal(0,Netplay::FrameInput{}));
        Netplay::FrameDecision forged{};forged.canAdvance=true;
        assert(!host.MarkSimulated(0,forged));
        assert_tail_choices(host,enabled,extra_draw,false);
        assert(host.SetNetworkGapTailEnabled(enabled));
        assert(host.SetNetworkTailPresentationEnabled(extra_draw));
        tail_handshake(host,peer);

        Netplay::FrameInput local{};local.analogMode=Netplay::AnalogMode::DirectTouch;
        local.x=12300;local.y=36500;local.unlimited=true;local.touchUsed=true;
        auto invalid=local;invalid.x=std::numeric_limits<float>::quiet_NaN();
        assert(!host.CaptureLocal(0,invalid));
        assert(!host.CaptureLocal(Netplay::INVALID_FRAME,local));
        assert_tail_choices(host,enabled,extra_draw,false);
        assert(host.SetNetworkGapTailEnabled(enabled));
        assert(host.SetNetworkTailPresentationEnabled(extra_draw));
        assert(host.CaptureLocal(0,local));
        assert_tail_choices(host,enabled,extra_draw,true);
        const double scale=host.InputGapScale(),effective=host.IntervalScale();
        assert(!host.SetNetworkGapTailEnabled(!enabled));
        assert(!host.SetNetworkGapTailEnabled(enabled));
        assert(!host.SetNetworkTailPresentationEnabled(!extra_draw));
        assert(!host.SetNetworkTailPresentationEnabled(extra_draw));
        assert_tail_choices(host,enabled,extra_draw,true);
        assert(host.InputGapScale()==scale&&host.IntervalScale()==effective);
        auto replacement=local;replacement.x+=100;
        assert(!host.CaptureLocal(0,replacement)&&host.CaptureLocal(0,local));
        assert(host.SubmitRemote(1,0,Netplay::FrameInput{})==Netplay::RemoteInputResult::Accepted);
        auto first=host.Prepare(0);assert(first.canAdvance&&!first.predictedMask);
        assert(first.inputs[0]==(delay?Netplay::FrameInput{}:local));
        assert(peer.CaptureLocal(0,Netplay::FrameInput{}));
        assert(peer.SubmitRemote(0,0,delay?Netplay::FrameInput{}:local)==Netplay::RemoteInputResult::Accepted);
        const auto peer_first=peer.Prepare(0);
        assert(peer_first.canAdvance&&peer_first.predictedMask==first.predictedMask);
        for(unsigned seat=0;seat<setup.playerCount;++seat)assert(peer_first.inputs[seat]==first.inputs[seat]);
        assert(peer.MarkSimulated(0,peer_first));
        assert_tail_choices(peer,!enabled,!extra_draw,true);
        assert(host.MarkSimulated(0,first)&&host.RewindSimulationTo(0));
        // Rewinding the cursor back to "never simulated" must not reopen it.
        assert(host.LastSimulatedFrame()==Netplay::INVALID_FRAME);
        assert_tail_choices(host,enabled,extra_draw,true);
        assert(!host.SetNetworkGapTailEnabled(!enabled));
        assert(!host.SetNetworkTailPresentationEnabled(!extra_draw));
        assert(host.MarkSimulated(0,host.Prepare(0))&&host.CanRetireRun());
        assert(host.RetireRun());
        assert_tail_choices(host,enabled,extra_draw,true);
        assert(!host.SetNetworkGapTailEnabled(!enabled));
        assert(!host.SetNetworkTailPresentationEnabled(!extra_draw));
        SessionSetup next{};assert(host.BeginNextRun(next,4321));
        assert(host.Generation()==1);
        assert_tail_choices(host,enabled,extra_draw,true);
        assert(!host.HasLocalCapture(0)&&host.LastSimulatedFrame()==Netplay::INVALID_FRAME);
        assert(host.InputGapScale()==1.&&!host.SetNetworkGapTailEnabled(!enabled));
        assert(!host.SetNetworkTailPresentationEnabled(!extra_draw));
        host.Clear();
        assert_tail_choices(host,false,false,false);
        assert(host.InputGapScale()==1.&&!host.SetNetworkGapTailEnabled(true));
        assert(!host.SetNetworkTailPresentationEnabled(true));
        assert(host.Reset(setup,92));
        assert_tail_choices(host,true,false,false);
        assert(host.SetNetworkGapTailEnabled(false)&&host.SetNetworkTailPresentationEnabled(true));
        assert(host.Reset(setup,93));
        assert_tail_choices(host,true,false,false);
        auto rejected=setup;rejected.configured=false;
        assert(!host.Reset(rejected,93));
        assert_tail_choices(host,false,false,false);
    }
    // A diagnostic adapter can commit a neutral lead-in before capturing.
    // The commit itself must latch the same permanent startup guard.
    {
        auto setup=tail_setup(3),other=setup;other.localPlayer=1;
        NetplayRuntime host,peer;assert(host.Reset(setup,94)&&peer.Reset(other,94));
        assert(host.SetNetworkGapTailEnabled(true));
        assert(host.SetNetworkTailPresentationEnabled(true));tail_handshake(host,peer);
        assert(host.SubmitRemote(1,0,Netplay::FrameInput{})==Netplay::RemoteInputResult::Accepted);
        assert(!host.HasLocalCapture(0)&&host.MarkSimulated(0,host.Prepare(0)));
        assert(host.RewindSimulationTo(0)&&!host.HasLocalCapture(0));
        assert_tail_choices(host,true,true,true);
        assert(!host.SetNetworkGapTailEnabled(false));
        assert(!host.SetNetworkTailPresentationEnabled(false));
    }
    // A nonconsecutive diagnostic capture also latches both choices, even if
    // frame zero is absent and no simulation cursor has advanced yet.
    {
        auto setup=tail_setup(),other=setup;other.localPlayer=1;
        NetplayRuntime host,peer;assert(host.Reset(setup,97)&&peer.Reset(other,97));
        assert(host.SetNetworkGapTailEnabled(false));
        assert(host.SetNetworkTailPresentationEnabled(true));tail_handshake(host,peer);
        assert(host.CaptureLocal(7,Netplay::FrameInput{}));
        assert(!host.HasLocalCapture(0)&&host.LastSimulatedFrame()==Netplay::INVALID_FRAME);
        assert_tail_choices(host,false,true,true);
        assert(!host.SetNetworkGapTailEnabled(true));
        assert(!host.SetNetworkTailPresentationEnabled(false));
    }
    for(unsigned mode:{1u,2u}){
        auto setup=tail_setup();setup.adonis_mode=mode;
        NetplayRuntime runtime;assert(runtime.Reset(setup,95));
        assert_tail_choices(runtime,false,false,false);
        assert(!runtime.SetNetworkGapTailEnabled(true));
        assert(!runtime.SetNetworkTailPresentationEnabled(true));
        assert(runtime.SetNetworkGapTailEnabled(false));
        assert(runtime.SetNetworkTailPresentationEnabled(false));
        assert_tail_choices(runtime,false,false,false);
        assert(runtime.Reset(tail_setup(),98));
        assert_tail_choices(runtime,true,false,false);
    }
    auto setup=tail_setup();NetplayRuntime playback;
    assert(playback.Reset(setup,99)&&playback.SetNetworkTailPresentationEnabled(true));
    assert(playback.BeginPlayback(setup));
    assert_tail_choices(playback,false,false,false);
    assert(!playback.SetNetworkGapTailEnabled(true)&&!playback.SetNetworkGapTailEnabled(false));
    assert(!playback.SetNetworkTailPresentationEnabled(true)&&!playback.SetNetworkTailPresentationEnabled(false));
    Netplay::FrameInput playback_inputs[2]{};
    assert(playback.FeedPlayback(0,playback_inputs,2));
    assert(playback.MarkSimulated(0,playback.Prepare(0)));
    assert_tail_choices(playback,false,false,true);

    // The production WASI transport cannot open a spectator connection. Check
    // failed attempts preserve startup choices; successful Web-only role entry
    // must additionally be observed through the real browser export.
    NetplayRuntime spectator;
    assert(spectator.Reset(tail_setup(),100));
    assert(spectator.SetNetworkTailPresentationEnabled(true));
    assert(!spectator.ConnectSpectator(nullptr,"spectator"));
    assert(!spectator.ConnectSpectator("ws://127.0.0.1/",""));
    assert_tail_choices(spectator,true,true,false);
}
void test_network_gap_tail_preserved_frontier(){
    auto setup=tail_setup(),other=setup;other.localPlayer=1;
    NetplayRuntime host,peer;assert(host.Reset(setup,96)&&peer.Reset(other,96));
    assert(host.SetNetworkGapTailEnabled(true));tail_handshake(host,peer);
    Netplay::FrameInput target{};target.analogMode=Netplay::AnalogMode::DirectTouch;
    target.x=8300;target.y=36500;target.unlimited=true;target.touchUsed=true;
    assert(host.SubmitRemote(1,0,target)==Netplay::RemoteInputResult::Accepted);
    for(std::uint32_t frame=0;frame<=12;++frame){
        auto local=target;local.x+=float(frame);
        assert(host.CaptureLocal(frame,local));
        const auto decision=host.Prepare(frame);
        assert(decision.canAdvance&&decision.inputs[0]==local&&decision.inputs[1]==target);
        assert(host.MarkSimulated(frame,decision));
    }
    assert(host.MissingRemoteInputs()==12&&host.PredictionFrontier()==13);
    host.BeginCorrection(13);assert(host.RewindSimulationTo(5));
    assert(host.NextFrame()==5&&host.MissingRemoteInputs()==12&&!host.CanCapture());
    NetworkInputGapPacing pacing;pacing.Reset(0);
    pacing.Update(350,host.MissingRemoteInputs(),true);assert(pacing.Scale()==8.);
    for(std::uint32_t frame=5;frame<=12;++frame){
        assert(host.HasLocalCapture(frame));
        assert(host.MarkSimulated(frame,host.Prepare(frame)));
        assert(host.MissingRemoteInputs()==12);
    }
    host.EndCorrection();assert(host.CanCapture());
    assert(host.CaptureLocal(13,target));
    // An exact newest slot cannot fill the older hole or extend H=12.
    assert(host.SubmitRemote(1,13,target)==Netplay::RemoteInputResult::Accepted);
    assert(host.InputPresent(1,13)&&!host.Prepare(13).canAdvance);
    for(std::uint32_t frame=1;frame<=12;++frame)
        assert(host.SubmitRemote(1,frame,target)==Netplay::RemoteInputResult::PredictionCorrect);
    assert(host.MissingRemoteInputs()==0&&!host.HasRollbackRequest());
    assert(host.Prepare(13).canAdvance&&host.MarkSimulated(13,host.Prepare(13)));
    assert(!host.SetNetworkGapTailEnabled(false));
    // Deep recovery does not keep a latch: the free region eventually clears
    // the brake without touching any captured input, confirmation or H limit.
    pacing.Update(725,host.MissingRemoteInputs(),true);assert(pacing.Scale()==1.);
}
}

int main() {
    test_network_gap_tail_startup_lock();
    test_network_gap_tail_preserved_frontier();
    for(unsigned mode:{1u,2u}){
        const std::uint32_t words[]{4,2,0,1,1234,77,0,0,0,0,1,1,0,0,mode,1,2,1,2,3,4};
        SessionSetup setup;assert(DecodeSessionSetup(setup,words,21));setup.input_delay=1;setup.measured_prediction=mode==2?2:0;
        NetplayRuntime a,b;assert(a.Reset(setup,77)&&a.PreparingWorld()&&a.AllowsRollback()==(mode==2));
        auto peer=setup;peer.localPlayer=1;assert(b.Reset(peer,77));a.ApplySession(b.Hello());b.ApplySession(a.Hello());
        a.MarkLocalReady();b.MarkLocalReady();a.ApplySession(b.Ready());b.ApplySession(a.Ready());assert(a.CanStart());
        assert(a.CaptureLocal(0,Netplay::FrameInput{}));assert(a.CaptureLocal(1,Netplay::FrameInput{}));
        assert(a.SubmitRemote(1,0,Netplay::FrameInput{})==Netplay::RemoteInputResult::Accepted);
        auto first=a.Prepare(0);assert(first.canAdvance&&!first.predictedMask&&a.MarkSimulated(0,first));
        const auto next=a.Prepare(1);assert(mode==2?next.canAdvance&&next.predictedMask:!next.canAdvance);
        assert(a.SubmitRemote(1,1,Netplay::FrameInput{})==Netplay::RemoteInputResult::Accepted);
        auto exact=a.Prepare(1);assert(exact.canAdvance&&!exact.predictedMask);
        auto changed=peer;changed.build[0]^=1;NetplayRuntime mismatch;assert(mismatch.Reset(changed,77));
        assert(a.ApplySession(mismatch.Hello())==Netplay::SessionPacketResult::ContractMismatch);
    }
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
