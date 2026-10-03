#include "../th08_web/cpp/multiplayer/NetplayRuntime.hpp"
#include "../th08_web/cpp/multiplayer/SpectatorStream.hpp"
#include "../th08_web/cpp/multiplayer/InputSample.hpp"
#include "support/previous-rule-contract.hpp"
#include <cassert>
#include <cstdio>
#include <limits>

using th08::multiplayer::SessionSetup;
using th08::multiplayer::NetplayRuntime;
using Netplay::FrameInput;
using Netplay::RemoteInputResult;
using Netplay::SessionPhase;
using Wire=NetplayRuntime::WireResult;

static SessionSetup setup(unsigned count,unsigned local,std::uint64_t id=0x1020304055667788ull,
                          unsigned delay=0,unsigned prediction=8){
    const std::uint32_t words[]{4,count,local,1,1234,std::uint32_t(id),std::uint32_t(id>>32),
        0x11223344u,0x55667788u,0x99aabbccu,0xddeeff00u,0,0,7,0,count==3?11u:0u,0,delay,prediction};
    SessionSetup result;
    assert(th08::multiplayer::decode_session_setup(result,words,19));return result;
}
static void barrier(NetplayRuntime* peers,unsigned count){
    for(unsigned seat=0;seat<count;++seat)assert(!peers[seat].CanStart());
    for(unsigned seat=0;seat<count;++seat)for(unsigned remote=0;remote<count;++remote)if(seat!=remote){
        const auto packet=peers[remote].SessionPacket(SessionPhase::Hello);
        assert(peers[seat].ApplySession(packet)==Netplay::SessionPacketResult::Accepted);
    }
    for(unsigned seat=0;seat<count;++seat)assert(peers[seat].MarkReady());
    for(unsigned seat=0;seat<count;++seat)for(unsigned remote=0;remote<count;++remote)if(seat!=remote)
        assert(peers[seat].ApplySession(peers[remote].SessionPacket(SessionPhase::Ready))==Netplay::SessionPacketResult::Accepted);
    for(unsigned seat=0;seat<count;++seat)assert(peers[seat].CanStart());
}
static std::vector<std::uint8_t> wire(const Netplay::InputPacket& packet){
    std::vector<std::uint8_t> result;assert(Netplay::EncodeInputPacket(packet,&result));return result;
}
static Wire apply_wire(NetplayRuntime& runtime,const std::vector<std::uint8_t>& bytes){return runtime.ApplyWire(bytes.data(),bytes.size());}
static void decode_atomicity(){
    auto valid=setup(3,2),candidate=valid;
    const std::uint32_t invalid[]{3,3,2,1,1234,5,6,1,2,3,4,0,0,7,1,11,0};
    assert(!th08::multiplayer::decode_session_setup(candidate,invalid,17));
    assert(candidate.session_id==valid.session_id&&candidate.characters[1]==7);
    auto started=valid;started.started=true;
    assert(!th08::multiplayer::decode_session_setup(started,invalid,17)&&started.started);
    const std::uint32_t local[]{1,2,0,1,1234,0,0,7,0,0,0};
    SessionSetup legacy;assert(th08::multiplayer::decode_session_setup(legacy,local,11)&&!legacy.session_id);
    const std::uint32_t network_v3[]{3,2,0,1,1234,5,6,1,2,3,4,0,0,7,0,0,0};
    SessionSetup old_network;assert(th08::multiplayer::decode_session_setup(old_network,network_v3,17));
    assert(old_network.input_delay==0&&old_network.prediction_limit==8);
    NetplayRuntime runtime;assert(runtime.Reset(valid));
    assert(!runtime.Reset(legacy)&&runtime.Config().sessionId==valid.session_id);
    auto bad=valid;bad.characters[2]=12;
    assert(!runtime.Reset(bad)&&runtime.Config().sessionId==valid.session_id);
    assert(th08::multiplayer::gameplay_contract(setup(3,0))==th08::multiplayer::gameplay_contract(valid));
}
static void fixed_input_delay_is_applied_and_agreed(){
    const auto id=0x8899aabbccddeeffull;
    NetplayRuntime peers[2];
    assert(peers[0].Reset(setup(2,0,id,8,4))&&peers[1].Reset(setup(2,1,id,8,4)));
    assert(peers[0].Setup().input_delay==8&&peers[0].Setup().prediction_limit==4);
    NetplayRuntime zero;assert(zero.Reset(setup(2,1,id,0,8)));
    assert(peers[0].ApplySession(zero.SessionPacket(SessionPhase::Hello))==Netplay::SessionPacketResult::ContractMismatch);
    barrier(peers,2);
    const auto bootstrap=th08::multiplayer::RouteBootstrap({},2);
    assert(peers[0].CaptureLeadInBootstrap(bootstrap));
    assert(!peers[1].CaptureLeadInBootstrap(bootstrap));
    assert(peers[0].CaptureLocal(0,FrameInput(1)));
    assert(peers[1].CaptureLocal(0,FrameInput{}));
    std::vector<std::uint8_t> bytes;
    assert(peers[0].BuildInputWire(1,0,1,0,bytes));
    Netplay::InputPacket packet;assert(Netplay::DecodeInputPacket(bytes.data(),bytes.size(),&packet));
    assert(packet.firstInputFrame==0&&packet.latestFrame==8&&packet.inputCount==9);
    assert(packet.inputs[0]==bootstrap&&packet.inputs[1].buttons==0&&packet.inputs[2].buttons==0);
    assert(packet.inputs[8].buttons==1);
    assert(apply_wire(peers[1],bytes)==Wire::Accepted);
    assert(peers[1].BuildInputWire(0,0,1,0,bytes));
    assert(apply_wire(peers[0],bytes)==Wire::Accepted);
    for(unsigned frame=0;frame<=8;++frame){
        for(auto& peer:peers){
            const auto decision=peer.Prepare(frame);
            assert(decision.canAdvance&&!decision.predictedMask);
            assert(decision.inputs[0]==(frame==0?bootstrap:FrameInput(frame==8?1:0)));
            assert(peer.MarkSimulated(frame,decision));
            if(frame==0)assert(peer.SetWorldReady(true));
        }
    }
    assert(!peers[0].CaptureLeadInBootstrap(bootstrap));
}
static void previous_general_rules_are_rejected(){
    for(unsigned count:{2u,3u})for(unsigned version:{3u,4u})for(bool rules20261002:{false,true}){
        auto local=setup(count,0),remote=setup(count,1);local.version=remote.version=version;
        NetplayRuntime live,peer;assert(live.Reset(local)&&peer.Reset(remote));
        auto hello=peer.SessionPacket(SessionPhase::Hello);hello.gameplayAbi=previous_rule_contract(remote,rules20261002);
        assert(hello.gameplayAbi!=live.Config().gameplayAbi);
        assert(live.ApplySession(hello)==Netplay::SessionPacketResult::ContractMismatch&&!live.CanStart());
        NetplayRuntime observer;assert(observer.Reset(local)&&observer.BeginSpectator());
        Netplay::SpectatorFramePacket packet;packet.sessionId=observer.Config().sessionId;
        packet.playerCount=std::uint8_t(count);packet.frame=0;packet.gameplayAbi=hello.gameplayAbi;
        assert(!observer.FeedSpectator(packet)&&!observer.HasLocal(0));
        th08::multiplayer::SpectatorStream queue;assert(!queue.Append(packet,observer.Config()));
    }
}
static void session_and_inputs(unsigned count){
    NetplayRuntime peers[3];for(unsigned seat=0;seat<count;++seat)assert(peers[seat].Reset(setup(count,seat)));
    assert(!peers[0].MarkReady()&&!peers[0].CaptureLocal(0,FrameInput(1))&&!peers[0].Prepare(0).canAdvance);
    assert(peers[0].ApplySession(peers[1].SessionPacket(SessionPhase::Ready))==Netplay::SessionPacketResult::ReadyBeforeHello);
    auto wrong=peers[1].SessionPacket(SessionPhase::Hello);++wrong.seed;
    assert(peers[0].ApplySession(wrong)==Netplay::SessionPacketResult::ContractMismatch);
    auto other_build=setup(count,1);++other_build.build[0];NetplayRuntime mismatched;
    assert(mismatched.Reset(other_build));
    assert(peers[0].ApplySession(mismatched.SessionPacket(SessionPhase::Hello))==Netplay::SessionPacketResult::ContractMismatch);
    auto old=peers[1].SessionPacket(SessionPhase::Hello);
    const auto previous=setup(count,1);std::uint32_t legacy=2166136261u;
    const auto add=[&](std::uint32_t value){for(int i=0;i<4;++i){legacy^=(value>>(i*8))&255u;legacy*=16777619u;}};
    add(0x08000005u);add(previous.player_count);add(previous.difficulty);add(previous.seed);
    for(const auto character:previous.characters)add(character);
    old.gameplayAbi=legacy;
    assert(peers[0].ApplySession(old)==Netplay::SessionPacketResult::ContractMismatch);
    barrier(peers,count);
    assert(peers[0].CaptureLocal(0,FrameInput(1)));
    assert(!peers[0].Prepare(0).canAdvance); // no title journal => no prediction
    for(unsigned seat=1;seat<count;++seat)assert(peers[0].SubmitRemote(seat,0,FrameInput(64))==RemoteInputResult::Accepted);
    auto first=peers[0].Prepare(0);assert(first.canAdvance&&!first.predictedMask);
    auto tampered=first;tampered.inputs[0].buttons=128;assert(!peers[0].MarkSimulated(0,tampered));
    assert(peers[0].MarkSimulated(0,first));
    std::array<FrameInput,Netplay::MAX_PLAYERS> confirmed{};assert(peers[0].ConfirmedInputs(0,confirmed));
    assert(confirmed[0].buttons==1&&confirmed[1].buttons==64);
    assert(peers[0].SetWorldReady(true));
    for(unsigned frame=1;frame<=3;++frame){
        assert(peers[0].CaptureLocal(frame,FrameInput(1)));
        const auto predicted=peers[0].Prepare(frame);assert(predicted.canAdvance&&predicted.predictedMask);
        assert(predicted.inputs[1].buttons==(frame<=2?64:0));
        assert(peers[0].MarkSimulated(frame,predicted));
    }
    assert(!peers[0].SetWorldReady(false)&&!peers[0].CanRetire());
    for(unsigned seat=1;seat<count;++seat)for(unsigned frame=1;frame<=3;++frame)
        assert(peers[0].SubmitRemote(seat,frame,FrameInput(128))==RemoteInputResult::RollbackRequired);
    assert(peers[0].RollbackFrame()==1&&!peers[0].Prepare(4).canAdvance);
    assert(!peers[0].ConfirmedInputs(0,confirmed)); // do not publish while reconciling
    assert(!peers[0].BeginCorrection(2)&&peers[0].BeginCorrection(1));
    assert(!peers[0].CaptureLocal(1,FrameInput(0))&&!peers[0].EndCorrection());
    for(unsigned frame=1;frame<=3;++frame){
        const auto corrected=peers[0].Prepare(frame);assert(corrected.canAdvance&&!corrected.predictedMask);
        assert(corrected.inputs[1].buttons==128&&peers[0].MarkSimulated(frame,corrected));
    }
    assert(peers[0].EndCorrection()&&peers[0].NextFrame()==4&&peers[0].ConfirmedInputs(3,confirmed));
    assert(peers[0].CanRetire()&&peers[0].Retire()&&!peers[0].CanStart());
    const auto old_id=setup(count,0).session_id;SessionSetup next;
    assert(peers[0].BeginNextRun(next,4321)&&next.session_id!=old_id&&peers[0].Generation()==1);
    assert(peers[0].NextFrame()==0&&peers[0].LastFrame()==Netplay::INVALID_FRAME&&!peers[0].CanStart());
    std::vector<std::uint8_t> stale;assert(Netplay::EncodeSessionPacket(peers[1].SessionPacket(SessionPhase::Hello),&stale));
    assert(apply_wire(peers[0],stale)==Wire::IgnoredSession);
}
static void confirmation_gap_is_bounded(){
    NetplayRuntime peers[2];for(unsigned seat=0;seat<2;++seat)assert(peers[seat].Reset(setup(2,seat)));
    barrier(peers,2);auto& r=peers[0];assert(r.SetWorldReady(true));
    for(unsigned frame=0;frame<NetplayRuntime::MaxRollbackFrames;++frame){
        assert(r.CaptureLocal(frame,FrameInput(0)));
        if(frame)assert(r.SubmitRemote(1,frame,FrameInput(0))==RemoteInputResult::Accepted);
        const auto decision=r.Prepare(frame);assert(decision.canAdvance&&r.MarkSimulated(frame,decision));
    }
    const unsigned frame=NetplayRuntime::MaxRollbackFrames;
    assert(r.CaptureLocal(frame,FrameInput(0))&&r.SubmitRemote(1,frame,FrameInput(0))==RemoteInputResult::Accepted);
    assert(!r.Prepare(frame).canAdvance); // exact current input cannot bypass frame-zero hole
    assert(r.SubmitRemote(1,0,FrameInput(0))==RemoteInputResult::PredictionCorrect);
    assert(r.Prepare(frame).canAdvance);
    assert(r.SubmitRemote(1,100000,FrameInput(0))==RemoteInputResult::InvalidPlayer);
}
static SessionSetup delayed_setup(unsigned local,unsigned delay=4,unsigned limit=2){
    const std::uint32_t words[]{4,2,local,1,1234,0x55667788u,0x10203040u,
        0x11223344u,0x55667788u,0x99aabbccu,0xddeeff00u,
        0,0,7,0,0,0,delay,limit};
    SessionSetup result;assert(th08::multiplayer::decode_session_setup(result,words,19));return result;
}
static void delayed_capture_and_wait(){
    NetplayRuntime p0,p1;
    assert(p0.Reset(delayed_setup(0))&&p1.Reset(delayed_setup(1)));
    auto mismatch=delayed_setup(1,3,2);NetplayRuntime wrong;
    assert(wrong.Reset(mismatch));
    assert(p0.ApplySession(wrong.SessionPacket(SessionPhase::Hello))==Netplay::SessionPacketResult::ContractMismatch);
    NetplayRuntime peers[2];
    assert(peers[0].Reset(delayed_setup(0))&&peers[1].Reset(delayed_setup(1)));
    barrier(peers,2);
    auto& r=peers[0];assert(r.SetWorldReady(true));
    FrameInput drag(64);drag.analogMode=Netplay::AnalogMode::DirectTouchDelta;
    drag.x=3;drag.y=-2;drag.touchUsed=true;
    assert(r.CaptureLocal(0,drag,2));
    assert(r.NextCaptureFrame()==1&&r.HasCapture(0));
    for(unsigned frame=0;frame<4;++frame){
        assert(r.HasLocalFrame(frame));
        if(frame==0){std::uint8_t route=255;
            auto bootstrap=r.Prepare(0); // remote is still absent
            assert(bootstrap.canAdvance&&th08::multiplayer::DecodeRouteBootstrap(bootstrap.inputs[0],route)&&route==2);
        }
    }
    std::vector<std::uint8_t> bytes;assert(r.BuildInputWire(1,0,1,0,bytes));
    Netplay::InputPacket packet;assert(Netplay::DecodeInputPacket(bytes.data(),bytes.size(),&packet));
    assert(packet.firstInputFrame==0&&packet.latestFrame==4&&packet.inputs[4]==drag);
    assert(r.SubmitRemote(1,0,FrameInput{})==RemoteInputResult::Accepted);
    for(unsigned frame=0;frame<3;++frame){
        if(frame)assert(r.CaptureLocal(frame,FrameInput(16)));
        auto decision=r.Prepare(frame);assert(decision.canAdvance&&r.MarkSimulated(frame,decision));
    }
    assert(!r.Prepare(3).canAdvance); // two predicted frames are the soft budget
    assert(r.CanCapture()&&r.CaptureLocal(3,FrameInput(32)));
    // Waiting must not enqueue another eight frames in front of the declared
    // delay. Repeated callbacks keep the already-sent physical sample intact.
    for(unsigned wait=0;wait<30;++wait){
        assert(!r.CanCapture()&&!r.CaptureLocal(4,FrameInput(32)));
        assert(r.NextFrame()==3&&r.NextCaptureFrame()==4);
    }
    for(unsigned frame=1;frame<=2;++frame)
        assert(r.SubmitRemote(1,frame,FrameInput{})==RemoteInputResult::PredictionCorrect);
    assert(r.Prepare(3).canAdvance);
    auto wrongLimit=delayed_setup(1,4,3);NetplayRuntime limit;
    assert(limit.Reset(wrongLimit));
    assert(r.ApplySession(limit.SessionPacket(SessionPhase::Hello))==Netplay::SessionPacketResult::ContractMismatch);
    NetplayRuntime observer;assert(observer.Reset(delayed_setup(0))&&observer.BeginSpectator());
    Netplay::SpectatorFramePacket exact{};
    exact.sessionId=observer.Config().sessionId;exact.gameplayAbi=observer.Config().gameplayAbi;
    exact.playerCount=2;exact.frame=0;
    exact.inputs[0]=th08::multiplayer::RouteBootstrap(FrameInput{},1);
    assert(observer.FeedSpectator(exact));
    const auto observed=observer.Prepare(0);
    assert(observed.canAdvance&&!observed.predictedMask&&observed.inputs[0]==exact.inputs[0]);
}
static void packet_transaction_and_negative_inputs(){
    NetplayRuntime peers[2];for(unsigned seat=0;seat<2;++seat)assert(peers[seat].Reset(setup(2,seat)));
    barrier(peers,2);auto& r=peers[0];
    assert(r.SubmitRemote(1,1,FrameInput(64))==RemoteInputResult::Accepted);
    Netplay::InputPacket packet;packet.sessionId=r.Config().sessionId;packet.senderPlayer=1;packet.playerCount=2;
    packet.sequence=1;packet.firstInputFrame=0;packet.latestFrame=1;packet.inputCount=2;
    packet.inputs[0]=FrameInput(1);packet.inputs[1]=FrameInput(128);
    assert(apply_wire(r,wire(packet))==Wire::Rejected&&r.ConfirmedThrough()==Netplay::INVALID_FRAME);
    // Frame zero must remain unmodified when the later redundant row conflicts.
    assert(r.SubmitRemote(1,0,FrameInput(2))==RemoteInputResult::Accepted&&r.ConfirmedThrough()==1);
    packet.inputCount=0;packet.firstInputFrame=Netplay::INVALID_FRAME;packet.latestFrame=Netplay::INVALID_FRAME;
    assert(apply_wire(r,wire(packet))==Wire::Accepted);
    packet.sessionId^=1;assert(apply_wire(r,wire(packet))==Wire::IgnoredSession);
    const std::uint8_t junk[]{0,1,2};assert(r.ApplyWire(junk,sizeof(junk))==Wire::Malformed);
    FrameInput bad(0);bad.buttons=0x8000;assert(!NetplayRuntime::ValidInput(bad));
    bad=FrameInput(0);bad.analogMode=Netplay::AnalogMode::DirectTouchDelta;assert(NetplayRuntime::ValidInput(bad));
    bad.analogMode=Netplay::AnalogMode::DirectTouchBegin;assert(NetplayRuntime::ValidInput(bad));
    bad.x=9000;assert(!NetplayRuntime::ValidInput(bad));
    FrameInput touch(2);touch.analogMode=Netplay::AnalogMode::DirectTouch;touch.touchUsed=touch.touchBomb=true;
    touch.x=12.5f;touch.y=-8.25f;assert(NetplayRuntime::ValidInput(touch));
    touch.x=9000;assert(!NetplayRuntime::ValidInput(touch));
    touch.x=0.5f;touch.y=-0.25f;touch.analogMode=Netplay::AnalogMode::Joystick;
    assert(NetplayRuntime::ValidInput(touch));touch.unlimited=true;assert(!NetplayRuntime::ValidInput(touch));
    bad=FrameInput(0);bad.x=std::numeric_limits<float>::quiet_NaN();assert(!NetplayRuntime::ValidInput(bad));
    bad=FrameInput(0);bad.touchBomb=true;assert(!r.CaptureLocal(0,bad));
    assert(r.CaptureLocal(0,FrameInput(1))&&r.CaptureLocal(0,FrameInput(1))&&!r.CaptureLocal(0,FrameInput(2)));
    const auto d=r.Prepare(0);assert(r.MarkSimulated(0,d));
    assert(!r.Reset(setup(2,0))&&r.NextFrame()==1); // active history cannot be silently replaced
}
static void spectator_is_exact_read_only_and_bounded(){
    using th08::multiplayer::SpectatorStream;
    for(const unsigned count:{2u,3u}){
        NetplayRuntime r;assert(r.Reset(setup(count,0))&&r.BeginSpectator());
        assert(r.Spectator()&&r.CanStart()&&!r.Ready()&&!r.MarkReady());
        assert(!r.SetWorldReady(true)&&!r.CaptureLocal(0,FrameInput(1)));
        assert(r.SubmitRemote(1,0,FrameInput(1))==RemoteInputResult::InvalidPlayer);
        assert(!r.Prepare(0).canAdvance&&!r.Reset(setup(count,0)));
        Netplay::SpectatorFramePacket p;p.sessionId=r.Config().sessionId;p.gameplayAbi=r.Config().gameplayAbi;
        p.playerCount=static_cast<std::uint8_t>(count);p.frame=0;
        p.inputs[0]=FrameInput(1);p.inputs[1]=FrameInput(2);
        p.inputs[1].analogMode=Netplay::AnalogMode::DirectTouch;
        p.inputs[1].x=1.25f;p.inputs[1].y=-2.5f;p.inputs[1].touchUsed=p.inputs[1].touchBomb=true;
        auto wrong=p;wrong.gameplayAbi^=1;assert(!r.FeedSpectator(wrong)&&!r.HasLocal(0));
        wrong=p;wrong.inputs[1].x=std::numeric_limits<float>::quiet_NaN();
        assert(!r.FeedSpectator(wrong)&&!r.HasLocal(0));
        SpectatorStream queue;assert(queue.Append(p,r.Config())&&queue.Size()==1);
        assert(r.FeedSpectator(*queue.Front()));queue.Pop();assert(!r.FeedSpectator(p));
        auto decision=r.Prepare(0);assert(decision.canAdvance&&!decision.predictedMask);
        assert(r.MarkSimulated(0,decision));
        std::array<FrameInput,Netplay::MAX_PLAYERS> inputs{};assert(r.ConfirmedInputs(0,inputs)&&inputs[1]==p.inputs[1]);
        assert(!r.Prepare(1).canAdvance); // A stalled observer never predicts.
        std::vector<std::uint8_t> bytes;assert(!r.BuildInputWire(1,0,1,0,bytes));
        assert(r.ApplyWire(nullptr,0)==Wire::IgnoredSession);
        assert(!r.BeginCorrection(0));
        assert(r.CanRetire()&&r.Retire());SessionSetup next;assert(!r.BeginNextRun(next,1234));
        const auto config=r.Config();
        r.Clear();assert(!r.Spectator());
        queue.Clear();p.frame=1;assert(!queue.Append(p,config)&&queue.Failed());
        p.frame=0;assert(!queue.Append(p,config)); // Failure remains latched.
        queue.Clear();assert(queue.Append(p,config));assert(!queue.Append(p,config));
        queue.Clear();p.playerCount=4;auto badconfig=config;badconfig.playerCount=4;
        assert(!queue.Append(p,badconfig));
        queue.Clear();p.playerCount=static_cast<std::uint8_t>(count);
        for(std::uint32_t frame=0;frame<SpectatorStream::Capacity;++frame){p.frame=frame;assert(queue.Append(p,config));}
        p.frame=SpectatorStream::Capacity;assert(!queue.Append(p,config));
        assert(queue.Size()==SpectatorStream::Capacity&&queue.Failed());
    }
}
static SessionSetup adonis_setup(unsigned count,unsigned local,unsigned delay,unsigned mode){
    const std::uint32_t words[]{5,count,local,1,1234,0x55667788u,0x10203040u,
        0x11223344u,0x55667788u,0x99aabbccu,0xddeeff00u,0,0,7,0,count==3?11u:0u,0,delay,8,mode};
    SessionSetup result;assert(th08::multiplayer::decode_session_setup(result,words,20));return result;
}
static FrameInput adonis_input(unsigned seat,unsigned frame){
    FrameInput in(std::uint16_t(1|((frame+seat)%2?16:64)));
    in.analogMode=Netplay::AnalogMode::DirectTouchDelta;
    in.x=float(int((frame+seat)%7)-3)*.25f;in.y=float(int(frame%5)-2)*.5f;
    in.touchUsed=true;in.touchBomb=frame%11==seat;if(in.touchBomb)in.buttons|=2;return in;
}
static void adonis_exact_input_and_generation(unsigned count,unsigned delay){
    NetplayRuntime peers[3];
    for(unsigned p=0;p<count;++p)assert(peers[p].Reset(adonis_setup(count,p,delay,1)));
    barrier(peers,count);
    std::vector<std::uint8_t> old_wire;
    for(unsigned f=0;f<48;++f){
        for(unsigned p=0;p<count;++p){
            auto& r=peers[p];assert(!r.AllowsRollback()&&!r.WorldReady()&&!r.SetWorldReady(true));
            assert(r.CanCapture()&&r.CaptureLocal(f,adonis_input(p,f)));
            assert(!r.CanCapture()&&r.NextCaptureFrame()==f+1);
            assert(r.CaptureLocal(f,adonis_input(p,f)));
            assert(!r.CaptureLocal(f,FrameInput{})&&!r.CaptureLocal(f+1,FrameInput{}));
        }
        for(unsigned p=0;p<count;++p)for(unsigned other=0;other<count;++other)if(p!=other){
            assert(peers[p].BuildInputWire(other,f,f+1,0,old_wire));
            assert(apply_wire(peers[other],old_wire)==Wire::Accepted);
        }
        for(auto* r=peers;r!=peers+count;++r){
            auto decision=r->Prepare(f);assert(decision.canAdvance&&!decision.predictedMask);
            for(unsigned p=0;p<count;++p)assert(decision.inputs[p]==(f<delay?FrameInput{}:adonis_input(p,f-delay)));
            auto forged=decision;forged.predictedMask=2;
            assert(!r->MarkSimulated(f,forged)&&r->NextFrame()==f);
            assert(r->MarkSimulated(f,decision)&&r->RollbackFrame()==Netplay::INVALID_FRAME&&!r->BeginCorrection(f));
            std::array<FrameInput,Netplay::MAX_PLAYERS> exact;
            assert(r->ConfirmedInputs(f,exact)&&exact==decision.inputs);
        }
    }
    for(unsigned p=0;p<count;++p){
        auto& r=peers[p];assert(r.CanRetire()&&r.Retire());SessionSetup next;
        assert(r.BeginNextRun(next,4321)&&r.Generation()==1);
        assert(next.version==5&&next.adonis_mode==1&&next.input_delay==delay);
        assert(r.NextFrame()==0&&r.NextCaptureFrame()==0&&!r.WorldReady());
        assert(apply_wire(r,old_wire)==Wire::IgnoredSession);
    }
}
static void adonis_hybrid_correction(unsigned count){
    NetplayRuntime peers[3];for(unsigned p=0;p<count;++p)assert(peers[p].Reset(adonis_setup(count,p,2,2)));
    barrier(peers,count);auto& r=peers[0];assert(r.SetWorldReady(true));
    std::array<std::uint64_t,7> states{};
    const auto step=[](std::uint64_t prior,const Netplay::FrameDecision& d,unsigned n){
        for(unsigned p=0;p<n;++p)prior=prior*131+std::uint64_t(d.inputs[p].buttons)*(p+1);
        return prior;
    };
    for(unsigned f=0;f<6;++f){
        assert(r.CaptureLocal(f,FrameInput(std::uint16_t(16+f))));
        if(!f)for(unsigned p=1;p<count;++p)assert(r.SubmitRemote(p,0,FrameInput{})==RemoteInputResult::Accepted);
        const auto d=r.Prepare(f);assert(d.canAdvance);
        if(f)assert(d.predictedMask);
        states[f+1]=step(states[f],d,count);assert(r.MarkSimulated(f,d));
    }
    for(unsigned f=1;f<6;++f)for(unsigned p=1;p<count;++p)
        assert(r.SubmitRemote(p,f,FrameInput(std::uint16_t((f+p)*16)))==RemoteInputResult::RollbackRequired);
    assert(r.RollbackFrame()==1&&r.BeginCorrection(1));
    std::uint64_t reference=0;
    for(unsigned f=0;f<6;++f){
        Netplay::FrameDecision exact;exact.inputs[0]=FrameInput(f<2?0:std::uint16_t(16+f-2));
        for(unsigned p=1;p<count;++p)exact.inputs[p]=FrameInput(f?std::uint16_t((f+p)*16):0);
        reference=step(reference,exact,count);
        if(f){const auto d=r.Prepare(f);assert(d.canAdvance&&!d.predictedMask&&d.inputs==exact.inputs);
            assert(!r.CanCapture()&&r.NextCaptureFrame()==6);
            states[f+1]=step(states[f],d,count);assert(r.MarkSimulated(f,d));}
        assert(states[f+1]==reference);
    }
    assert(r.EndCorrection()&&r.NextFrame()==6&&r.NextCaptureFrame()==6);
}
static void adonis_mismatch_and_gap(){
    NetplayRuntime a,b;const auto configured=adonis_setup(2,0,1,1);assert(a.Reset(configured));
    for(auto other:{adonis_setup(2,1,1,2),adonis_setup(2,1,2,1),setup(2,1)}){
        b.Clear();assert(b.Reset(other));
        assert(a.ApplySession(b.SessionPacket(SessionPhase::Hello))==Netplay::SessionPacketResult::ContractMismatch);
    }
    auto invalid=configured;invalid.adonis_mode=3;assert(!a.Reset(invalid)&&a.Setup().adonis_mode==1);
    invalid=configured;invalid.input_delay=10;assert(!a.Reset(invalid)&&a.InputDelay()==1);
    std::uint32_t zero=0;assert(!th08::multiplayer::decode_session_setup(invalid,&zero,0));
    NetplayRuntime peers[2];for(unsigned p=0;p<2;++p)assert(peers[p].Reset(adonis_setup(2,p,1,1)));
    barrier(peers,2);auto& r=peers[0];assert(r.CaptureLocal(0,FrameInput(1)));
    assert(r.SubmitRemote(1,1,FrameInput(16))==RemoteInputResult::Accepted);
    assert(!r.Prepare(0).canAdvance);
    assert(r.SubmitRemote(1,0,FrameInput{})==RemoteInputResult::Accepted);
    const auto exact=r.Prepare(0);assert(exact.canAdvance&&!exact.predictedMask&&r.MarkSimulated(0,exact));
    NetplayRuntime observer;assert(observer.Reset(adonis_setup(2,0,9,1))&&observer.BeginSpectator());
    Netplay::SpectatorFramePacket packet;packet.sessionId=observer.Config().sessionId;
    packet.gameplayAbi=observer.Config().gameplayAbi;packet.playerCount=2;packet.frame=0;
    packet.inputs[0]=adonis_input(0,17);packet.inputs[1]=adonis_input(1,17);
    assert(observer.FeedSpectator(packet));const auto d=observer.Prepare(0);
    assert(d.canAdvance&&d.inputs==packet.inputs&&!d.predictedMask);
}
int main(){
    previous_general_rules_are_rejected();
    for(unsigned count:{2u,3u})for(unsigned mode:{1u,2u}){
        auto measured=setup(count,0);measured.version=6;measured.adonis_mode=mode;measured.input_delay_auto=true;
        NetplayRuntime n;assert(n.Reset(measured)&&n.PreparingWorld()&&!n.CanCapture());
        assert(n.ApplyMeasuredTiming(1,mode==2?2:0)&&n.Setup().prediction_limit==8);
        auto incompatible=n.Setup();incompatible.measured_prediction=0;
        if(mode==2)assert(th08::multiplayer::gameplay_contract(incompatible)!=n.Config().gameplayAbi);
        auto remote=n.Setup();remote.local_player=1;
        NetplayRuntime peer;assert(peer.Reset(remote));
        auto hello=peer.SessionPacket(SessionPhase::Hello);
        hello.gameplayAbi=previous_adonis_contract(remote);
        assert(n.ApplySession(hello)==Netplay::SessionPacketResult::ContractMismatch&&!n.CanStart());
    }
    decode_atomicity();fixed_input_delay_is_applied_and_agreed();session_and_inputs(2);session_and_inputs(3);
    delayed_capture_and_wait();
    confirmation_gap_is_bounded();packet_transaction_and_negative_inputs();
    spectator_is_exact_read_only_and_bounded();
    for(unsigned count:{2u,3u}){for(unsigned delay:{0u,1u,9u})adonis_exact_input_and_generation(count,delay);adonis_hybrid_correction(count);}
    adonis_mismatch_and_gap();
    std::puts("TH08 frame-zero admission, corrected frontiers, atomic packets and generation fences: PASS");
}
