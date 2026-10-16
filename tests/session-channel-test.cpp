#include <eagler/netplay/SessionChannel.hpp>
#include <eagler/netplay/InputRepairBudget.hpp>
#include <cstdio>
#include <cstdlib>
#include <deque>
#include <functional>
#include <memory>
#include <vector>

using namespace Netplay;
#define CHECK(test) do { if (!(test)) { std::fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #test); std::abort(); } } while (false)

namespace {
struct Mesh;
struct Link final : PeerTransport {
    Mesh& mesh; std::uint8_t seat;
    bool open=true,broken=false,recovering=false,disconnected=false,rejectInput=false,rejectRepair=false; std::size_t buffered=0;
    std::deque<std::vector<std::uint8_t>> inbox;
    Link(Mesh& owner,std::uint8_t player):mesh(owner),seat(player){}
    bool IsOpen()const override{return open;}
    bool Failed()const override{return broken;}
    bool Recovering()const override{return recovering;}
    bool Disconnected()const override{return disconnected;}
    bool SendTo(std::uint8_t peer,const std::uint8_t* bytes,std::size_t size)override;
    bool SendRepairTo(std::uint8_t peer,const std::uint8_t* bytes,std::size_t size)override;
    bool SendControl(const std::uint8_t* bytes,std::size_t size)override;
    bool Poll(std::vector<std::uint8_t>* out)override {
        if(inbox.empty())return false;
        out->assign(inbox.front().begin(),inbox.front().end());inbox.pop_front();return true;
    }
    std::size_t BufferedAmount()const override{return buffered;}
};
struct Peer {
    Link link; SessionGate gate; RollbackCore core; SessionChannel channel;
    unsigned captures=0;
    Peer(Mesh& owner,std::uint8_t seat):link(owner,seat),channel(link){}
};
struct Mesh {
    std::vector<std::unique_ptr<Peer>> peers;
    std::uint64_t now=0,routed=0;
    bool exactPhase=false;
    // True drops delivery while preserving a successful send, as loss does.
    std::function<bool(std::uint8_t,std::uint8_t,bool,const std::vector<std::uint8_t>&)> drop;
    explicit Mesh(std::uint8_t count,bool phase=false):exactPhase(phase){
        for(std::uint8_t seat=0;seat<count;++seat)peers.emplace_back(new Peer(*this,seat));
        for(std::uint8_t seat=0;seat<count;++seat)begin(seat,100);
    }
    void begin(std::uint8_t seat,std::uint64_t id){
        auto& peer=*peers[seat];SessionConfig session{};
        session.sessionId=id;session.gameId=10;session.gameplayAbi=0x543210;session.seed=1234;
        session.playerCount=static_cast<std::uint8_t>(peers.size());session.localPlayer=seat;
        CoreConfig core{};core.sessionId=id;core.playerCount=session.playerCount;core.localPlayer=seat;
        core.maxRollbackFrames=12;core.predictableButtons=0xf5;core.directionButtons=0xf0;core.maxDirectionPredictionFrames=3;
        core.allowPrediction=!exactPhase;
        SessionChannelConfig policy;policy.adonisPhase=exactPhase;
        CHECK(peer.gate.Reset(session));CHECK(peer.core.Reset(core));CHECK(peer.channel.BeginSession(session,now,policy));
    }
    bool route(std::uint8_t from,std::uint8_t to,bool reliable,const std::uint8_t* bytes,std::size_t size){
        CHECK(from!=to&&to<peers.size());if(!peers[from]->link.open)return false;
        std::vector<std::uint8_t> wire(bytes,bytes+size);++routed;
        if(!drop||!drop(from,to,reliable,wire))peers[to]->link.inbox.push_back(std::move(wire));
        return true;
    }
    void pump(bool expects=false,std::uint64_t advance=10){
        now+=advance;
        for(auto& peer:peers){
            const bool ok=peer->channel.Pump(peer->gate,peer->core,now,expects);
            if(!ok)std::fprintf(stderr,"peer=%u failure=%s now=%llu\n",peer->link.seat,peer->channel.ErrorText(),static_cast<unsigned long long>(now));
            CHECK(ok);
        }
    }
    bool ready()const{for(const auto& peer:peers)if(!peer->gate.CanStart())return false;return true;}
    void barrier(){for(int i=0;i<1000&&!ready();++i)pump();CHECK(ready());}
};
bool Link::SendTo(std::uint8_t peer,const std::uint8_t* bytes,std::size_t size){return !rejectInput&&mesh.route(seat,peer,false,bytes,size);}
bool Link::SendRepairTo(std::uint8_t peer,const std::uint8_t* bytes,std::size_t size){return !rejectRepair&&mesh.route(seat,peer,true,bytes,size);}
bool Link::SendControl(const std::uint8_t* bytes,std::size_t size){
    bool sent=true;for(std::uint8_t peer=0;peer<mesh.peers.size();++peer)if(peer!=seat)sent=mesh.route(seat,peer,true,bytes,size)&&sent;return sent;
}
FrameInput sample(std::uint8_t seat,std::uint32_t frame){
    return FrameInput(static_cast<std::uint16_t>(1|((frame/9+seat)%2?0x80:0x40)|(frame%31==seat?2:0)));
}
std::uint32_t next(const RollbackCore& core){const auto last=core.LastSimulatedFrame();return last==INVALID_FRAME?0:last+1;}
void capture(Peer& peer,std::uint32_t frame,std::uint64_t now){
    if(peer.core.HasLocalCapture(frame))return;
    CHECK(peer.core.ScheduleLocalInput(frame,sample(peer.link.seat,frame)));
    CHECK(peer.channel.LocalCaptured(peer.core,frame,now));++peer.captures;
}
void reconcile(Peer& peer){
    if(!peer.core.HasRollbackRequest())return;
    const auto first=peer.core.RollbackFrame(),last=peer.core.LastSimulatedFrame();
    CHECK(peer.core.RewindSimulationTo(first));
    for(auto frame=first;frame<=last;++frame){const auto d=peer.core.PrepareFrame(frame);CHECK(d.canAdvance);CHECK(peer.core.MarkSimulated(frame,d));}
}
bool atLeast(std::uint32_t frame,std::uint32_t target){return frame!=INVALID_FRAME&&frame>=target;}

void future_capture_tail_remains_deliverable(){
    for(unsigned count:{2u,3u})for(bool phase:{false,true}){
        Mesh mesh(static_cast<std::uint8_t>(count),phase);mesh.barrier();
        // Exact UI may keep its physical capture frontier moving while the
        // authoritative simulation remains at frame zero. Use real channel
        // encode/ACK/admission, not direct core input injection.
        for(unsigned frame=0;frame<8;++frame){
            for(auto& peer:mesh.peers)capture(*peer,frame,mesh.now);
            mesh.pump(false,17);
            for(auto& peer:mesh.peers)CHECK(next(peer->core)==0);
        }
        for(auto& peer:mesh.peers)for(unsigned frame=0;frame<8;++frame){
            const auto d=peer->core.PrepareFrame(frame);CHECK(d.canAdvance&&!d.predictedMask);
            for(unsigned seat=0;seat<count;++seat)CHECK(d.inputs[seat]==sample(seat,frame));
            CHECK(peer->core.MarkSimulated(frame,d));
        }
        for(unsigned retry=0;retry<30;++retry)mesh.pump(false,17);
        for(auto& peer:mesh.peers){CHECK(peer->captures==8);CHECK(peer->channel.CanRetire(peer->core,7));}
    }
}

void lossy_three_peer_stream(){
    Mesh mesh(3);bool droppedReady=false;unsigned fast=0;
    constexpr std::uint32_t frames=1800; // exercise several input-history wraps
    mesh.drop=[&](std::uint8_t from,std::uint8_t to,bool reliable,const std::vector<std::uint8_t>& wire){
        SessionPacket session;
        if(!droppedReady&&from==0&&to==1&&DecodeSessionPacket(wire.data(),wire.size(),&session)&&session.phase==SessionPhase::Ready){droppedReady=true;return true;}
        return !reliable&&++fast%3==0;
    };
    mesh.barrier();CHECK(droppedReady);
    for(int step=0;step<5000;++step){
        bool finished=true;
        for(auto& owner:mesh.peers){
            auto& peer=*owner;reconcile(peer);const auto frame=next(peer.core);
            if(frame<frames){finished=false;capture(peer,frame,mesh.now);const auto d=peer.core.PrepareFrame(frame);if(d.canAdvance)CHECK(peer.core.MarkSimulated(frame,d));}
            if(!atLeast(peer.core.ConfirmedThroughAllRemotes(),frames-1)||!atLeast(peer.core.AcknowledgedLocalThroughAllRemotes(),frames-1))finished=false;
        }
        mesh.pump(true,5);if(finished)break;
    }
    for(const auto& owner:mesh.peers){
        auto& peer=*owner;reconcile(peer);CHECK(peer.core.LastSimulatedFrame()==frames-1);CHECK(peer.captures==frames);CHECK(peer.channel.CanRetire(peer.core,frames-1));
        for(std::uint8_t seat=0;seat<3;++seat)for(std::uint32_t frame=frames-32;frame<frames;++frame){
            FrameInput used;CHECK(peer.core.UsedInput(seat,frame,&used));CHECK(used==sample(seat,frame));
        }
    }
    // A lost READY may solicit replies, not an infinite HELLO echo after start.
    unsigned controls=0;
    mesh.drop=[&](std::uint8_t,std::uint8_t,bool,const std::vector<std::uint8_t>& wire){
        SessionPacket packet;if(DecodeSessionPacket(wire.data(),wire.size(),&packet))++controls;return false;
    };
    for(int step=0;step<100;++step)mesh.pump();CHECK(controls<=12);
}

void retired_final_ack_echo(){
    Mesh mesh(2);mesh.barrier();auto& fast=*mesh.peers[0];auto& slow=*mesh.peers[1];bool dropReliableAck=true;
    mesh.drop=[&](std::uint8_t from,std::uint8_t to,bool reliable,const std::vector<std::uint8_t>& wire){
        InputPacket p;
        if(from!=0||to!=1||!DecodeInputPacket(wire.data(),wire.size(),&p)||p.sessionId!=100||p.ackFrame!=0)return false;
        if(!reliable)return true;if(dropReliableAck){dropReliableAck=false;return true;}return false;
    };
    for(auto& peer:mesh.peers)capture(*peer,0,mesh.now);
    for(int i=0;i<20;++i)mesh.pump();
    for(auto& peer:mesh.peers){const auto d=peer->core.PrepareFrame(0);CHECK(d.canAdvance&&d.predictedMask==0);CHECK(peer->core.MarkSimulated(0,d));}
    CHECK(fast.channel.CanRetire(fast.core,0));CHECK(!slow.channel.CanRetire(slow.core,0));
    CHECK(fast.channel.Retire(fast.core,0,mesh.now));fast.gate.Clear();fast.core.Clear();mesh.begin(0,200);CHECK(!fast.gate.CanStart());
    for(int i=0;i<100&&!slow.channel.CanRetire(slow.core,0);++i)mesh.pump();
    CHECK(slow.channel.CanRetire(slow.core,0));CHECK(fast.channel.RepairsSent()>=2);
    CHECK(slow.channel.Retire(slow.core,0,mesh.now));slow.gate.Clear();slow.core.Clear();mesh.begin(1,200);mesh.barrier();
    for(int i=0;i<10;++i)mesh.pump();CHECK(!fast.channel.Retiring()&&!slow.channel.Retiring());
    CHECK(fast.core.LastSimulatedFrame()==INVALID_FRAME&&slow.core.LastSimulatedFrame()==INVALID_FRAME);
    CHECK(fast.core.ConfirmedThroughAllRemotes()==INVALID_FRAME&&slow.core.ConfirmedThroughAllRemotes()==INVALID_FRAME);
    InputPacket stale;stale.sessionId=100;stale.senderPlayer=0;stale.playerCount=2;stale.ackFrame=stale.latestFrame=0;
    std::vector<std::uint8_t> wire;CHECK(EncodeInputPacket(stale,&wire));slow.link.inbox.push_back(wire);mesh.pump();
    CHECK(slow.core.AcknowledgedLocalThroughAllRemotes()==INVALID_FRAME);CHECK(slow.channel.PacketsIgnored()!=0);
}

void retirement_pump_retries_refused_ack_without_draining_calibration(){
    Mesh mesh(2);mesh.barrier();auto& fast=*mesh.peers[0];auto& slow=*mesh.peers[1];
    mesh.drop=[](std::uint8_t from,std::uint8_t to,bool reliable,const std::vector<std::uint8_t>& wire){
        InputPacket packet;
        return from==0&&to==1&&!reliable&&DecodeInputPacket(wire.data(),wire.size(),&packet)&&
            packet.sessionId==100&&packet.ackFrame==0;
    };
    for(auto& peer:mesh.peers)capture(*peer,0,mesh.now);
    for(unsigned step=0;step<3;++step)mesh.pump(false,1);
    for(auto& peer:mesh.peers){
        const auto decision=peer->core.PrepareFrame(0);
        CHECK(decision.canAdvance&&!decision.predictedMask);
        CHECK(peer->core.MarkSimulated(0,decision));
    }
    CHECK(fast.channel.CanRetire(fast.core,0)&&!slow.channel.CanRetire(slow.core,0));
    const auto sent=fast.channel.PacketsSent(),repairs=fast.channel.RepairsSent();
    fast.link.rejectRepair=fast.link.rejectInput=true;
    CHECK(fast.channel.Retire(fast.core,0,mesh.now));
    CHECK(fast.channel.Retiring()&&!fast.channel.Active());
    CHECK(fast.channel.PacketsSent()==sent&&fast.channel.RepairsSent()==repairs);

    // A next-generation calibration owner may already have replaced the old
    // core. Its queued bytes must remain opaque to this retirement-only pump.
    fast.gate.Clear();fast.core.Clear();
    fast.link.inbox.push_back({0xca,0x1b,0x00});
    const auto queued=fast.link.inbox;
    const auto received=fast.channel.PacketsReceived(),ignored=fast.channel.PacketsIgnored();
    fast.link.rejectRepair=fast.link.rejectInput=false;
    mesh.now+=SessionChannelConfig{}.inputResendMs-1;
    CHECK(fast.channel.PumpRetirement(mesh.now));
    CHECK(fast.channel.PacketsSent()==sent);
    ++mesh.now;
    CHECK(fast.channel.PumpRetirement(mesh.now));
    CHECK(fast.channel.PacketsSent()==sent+1&&fast.channel.RepairsSent()==repairs+1);
    CHECK(fast.link.inbox==queued&&fast.channel.PacketsReceived()==received&&
          fast.channel.PacketsIgnored()==ignored);
    CHECK(!fast.gate.CanStart()&&fast.core.LastSimulatedFrame()==INVALID_FRAME&&
          !fast.core.HasLocalCapture(0)&&fast.captures==1);
    CHECK(slow.channel.Pump(slow.gate,slow.core,mesh.now,false));
    CHECK(slow.channel.CanRetire(slow.core,0));

    // Calibration consumes its own queue; BeginSession must preserve the old
    // cache until the slow peer also reaches the new generation's HELLO.
    fast.link.inbox.clear();mesh.begin(0,200);
    CHECK(fast.channel.Retiring());
    CHECK(slow.channel.Retire(slow.core,0,mesh.now));
    slow.gate.Clear();slow.core.Clear();mesh.begin(1,200);
    mesh.barrier();for(unsigned step=0;step<4;++step)mesh.pump();
    CHECK(!fast.channel.Retiring()&&!slow.channel.Retiring());
    CHECK(fast.core.LastSimulatedFrame()==INVALID_FRAME&&slow.core.LastSimulatedFrame()==INVALID_FRAME);
    CHECK(fast.captures==1&&slow.captures==1);
}

void retirement_pump_rejects_active_session_and_bad_clock(){
    {
        Mesh mesh(2);auto& peer=*mesh.peers[0];
        CHECK(!peer.channel.PumpRetirement(mesh.now));
        CHECK(peer.channel.Error()==SessionChannel::Failure::InvalidRetirement);
    }
    {
        Mesh mesh(2);auto& peer=*mesh.peers[0];peer.channel.Clear();
        CHECK(peer.channel.PumpRetirement(100));
        CHECK(!peer.channel.PumpRetirement(99));
        CHECK(peer.channel.Error()==SessionChannel::Failure::ClockReversed);
    }
    {
        Mesh mesh(2);auto& peer=*mesh.peers[0];peer.channel.Clear();peer.link.broken=true;
        CHECK(!peer.channel.PumpRetirement(mesh.now));
        CHECK(peer.channel.Error()==SessionChannel::Failure::Transport);
    }
}

void reliable_tail_repairs_blackout(){
    Mesh mesh(2);mesh.barrier();
    mesh.drop=[](std::uint8_t,std::uint8_t,bool reliable,const std::vector<std::uint8_t>&){return !reliable;};
    for(auto& peer:mesh.peers)capture(*peer,0,mesh.now);
    for(int i=0;i<20;++i)mesh.pump(true);CHECK(mesh.peers[0]->core.ConfirmedThroughAllRemotes()==INVALID_FRAME);
    // The all-fast-loss path needs a reliable input round trip and then a
    // reliable ACK round trip; allow the two bounded repair intervals.
    for(int i=0;i<100&&(!atLeast(mesh.peers[0]->core.AcknowledgedLocalThroughAllRemotes(),0)||
                        !atLeast(mesh.peers[1]->core.AcknowledgedLocalThroughAllRemotes(),0));++i)
        mesh.pump(true);
    for(const auto& peer:mesh.peers){
        CHECK(peer->core.ConfirmedThroughAllRemotes()==0);CHECK(peer->channel.RepairsSent()!=0);
        CHECK(peer->core.AcknowledgedLocalThroughAllRemotes()==0);
    }
}

void explicit_retirement_fence_recovers_before_repair_timer(bool backpressured=false){
    Mesh mesh(2);mesh.barrier();
    mesh.drop=[](std::uint8_t,std::uint8_t,bool reliable,const std::vector<std::uint8_t>&){return !reliable;};
    for(auto& owner:mesh.peers){
        auto& peer=*owner;
        if(backpressured)peer.link.buffered=SessionChannelConfig{}.bufferedLimit+1;
        CHECK(peer.core.ScheduleLocalInput(0,FrameInput{}));
        CHECK(peer.channel.LocalCaptured(peer.core,0,mesh.now));
        const auto d=peer.core.PrepareFrame(0);CHECK(d.canAdvance);CHECK(peer.core.MarkSimulated(0,d));
    }
    CHECK(mesh.peers[0]->core.ConfirmedThroughAllRemotes()==INVALID_FRAME);
    CHECK(mesh.peers[1]->core.ConfirmedThroughAllRemotes()==INVALID_FRAME);
    for(auto& peer:mesh.peers)CHECK(peer->channel.FlushRetirementFence(peer->core,0,mesh.now));
    mesh.pump(false,32);
    for(const auto& peer:mesh.peers){
        CHECK(peer->core.ConfirmedThroughAllRemotes()==0);
        CHECK(peer->core.AcknowledgedLocalThroughAllRemotes()==INVALID_FRAME);
    }
    for(auto& peer:mesh.peers)CHECK(peer->channel.FlushRetirementFence(peer->core,0,mesh.now));
    mesh.pump(false,1);
    for(const auto& peer:mesh.peers){
        CHECK(peer->core.AcknowledgedLocalThroughAllRemotes()==0);
        CHECK(peer->channel.CanRetire(peer->core,0));
        CHECK(peer->channel.RepairsSent()>=2);
    }
}

void reliable_repairs_survive_fast_lane_backpressure(){
    for(const auto interval:{std::uint64_t(250),Netplay::InputRepairBudget::StalledMs})for(const std::uint8_t count:{2,3}){
        Mesh mesh(count);SessionChannelConfig policy;policy.repairIntervalMs=interval;
        for(auto& peer:mesh.peers){peer->channel.Clear();CHECK(peer->channel.BeginSession(peer->gate.Config(),mesh.now,policy));}
        mesh.barrier();unsigned fastSends=0;const auto start=mesh.now;
        std::uint64_t lastRepair[MAX_PLAYERS][MAX_PLAYERS]{};
        mesh.drop=[&](std::uint8_t from,std::uint8_t to,bool reliable,const std::vector<std::uint8_t>& wire){
            InputPacket packet;
            if(reliable&&DecodeInputPacket(wire.data(),wire.size(),&packet)){
                if(lastRepair[from][to])CHECK(mesh.now-lastRepair[from][to]>=interval);
                lastRepair[from][to]=mesh.now;
            }
            if(!reliable)++fastSends;return !reliable;
        };
        // The unreliable input lane can stay queued while the independent
        // reliable control lane still works. Its own bounded SendRepairTo
        // admission, not the aggregate fast-lane queue, governs repairs.
        for(auto& peer:mesh.peers){
            peer->link.buffered=SessionChannelConfig{}.bufferedLimit+1;
            capture(*peer,0,mesh.now);
        }
        for(int step=0;step<100;++step)mesh.pump(true);
        CHECK(fastSends==0);
        for(auto& peer:mesh.peers){
            CHECK(peer->channel.Error()==SessionChannel::Failure::None);
            CHECK(peer->core.ConfirmedThroughAllRemotes()==0);
            CHECK(peer->core.AcknowledgedLocalThroughAllRemotes()==0);
            CHECK(peer->channel.RepairsSent()>0&&peer->channel.RepairsSent()<=(1+(mesh.now-start)/interval)*(count-1));
            CHECK(peer->captures==1);
            const auto d=peer->core.PrepareFrame(0);CHECK(d.canAdvance&&!d.predictedMask);
            for(std::uint8_t seat=0;seat<count;++seat)CHECK(d.inputs[seat]==sample(seat,0));
            CHECK(peer->core.MarkSimulated(0,d));CHECK(peer->channel.CanRetire(peer->core,0));
            peer->link.buffered=0;
        }
        // Draining the input queue restores the ordinary input path without
        // reconnecting, resampling frame zero, or changing its captured data.
        mesh.drop=nullptr;
        for(auto& peer:mesh.peers)capture(*peer,1,mesh.now);
        for(int step=0;step<100;++step)mesh.pump(true);
        for(auto& peer:mesh.peers){
            CHECK(peer->core.ConfirmedThroughAllRemotes()==1);
            CHECK(peer->core.AcknowledgedLocalThroughAllRemotes()==1);
            CHECK(peer->captures==2);
        }
    }
}

void backpressured_total_outage_still_times_out(){
    Mesh mesh(2);mesh.barrier();
    mesh.drop=[](std::uint8_t,std::uint8_t,bool,const std::vector<std::uint8_t>&){return true;};
    for(auto& peer:mesh.peers){
        peer->link.buffered=SessionChannelConfig{}.bufferedLimit+1;
        capture(*peer,0,mesh.now);
        CHECK(peer->channel.Pump(peer->gate,peer->core,mesh.now,true));
    }
    mesh.now+=ConfirmedInputWatchdog::DefaultTimeoutMs;
    for(auto& peer:mesh.peers){
        CHECK(!peer->channel.Pump(peer->gate,peer->core,mesh.now,true));
        CHECK(peer->channel.Error()==SessionChannel::Failure::ConfirmedTimeout);
        CHECK(peer->core.ConfirmedThroughAllRemotes()==INVALID_FRAME);
        CHECK(peer->core.LastSimulatedFrame()==INVALID_FRAME);
        CHECK(peer->captures==1);
    }
}

void timeouts_and_invalid_ack(){
    {
        Mesh mesh(2);mesh.drop=[](std::uint8_t,std::uint8_t,bool,const std::vector<std::uint8_t>&){return true;};auto& p=*mesh.peers[0];
        CHECK(p.channel.Pump(p.gate,p.core,1,false));CHECK(!p.channel.Pump(p.gate,p.core,45'000,false));CHECK(p.channel.Error()==SessionChannel::Failure::HandshakeTimeout);
    }
    {
        Mesh mesh(2);mesh.barrier();auto& p=*mesh.peers[0];capture(p,0,mesh.now);
        CHECK(p.channel.Pump(p.gate,p.core,mesh.now,true));CHECK(!p.channel.Pump(p.gate,p.core,mesh.now+15'000,true));CHECK(p.channel.Error()==SessionChannel::Failure::ConfirmedTimeout);
    }
    {
        Mesh mesh(2);mesh.barrier();auto& p=*mesh.peers[0];capture(p,0,mesh.now);
        InputPacket packet{};packet.sessionId=100;packet.playerCount=2;packet.senderPlayer=1;packet.ackFrame=5;
        std::vector<std::uint8_t> wire;CHECK(EncodeInputPacket(packet,&wire));p.link.inbox.push_back(wire);
        CHECK(!p.channel.Pump(p.gate,p.core,mesh.now,false));CHECK(p.channel.Error()==SessionChannel::Failure::MalformedPacket);
        CHECK(p.core.AcknowledgedLocalThrough(1)==INVALID_FRAME);CHECK(p.core.ConfirmedThrough(1)==INVALID_FRAME);
    }
    {
        Mesh mesh(2);mesh.barrier();auto& p=*mesh.peers[0];p.link.broken=true;
        CHECK(!p.channel.Pump(p.gate,p.core,mesh.now,false));CHECK(p.channel.Error()==SessionChannel::Failure::Transport);
    }
}
void pacing_validated_packets_and_reset(){
    Mesh mesh(2);mesh.barrier();auto& p=*mesh.peers[0];
    const auto send=[&](std::uint32_t sequence,std::uint32_t frame,std::int16_t lead,std::uint64_t session=100){
        InputPacket packet{};packet.sessionId=session;packet.playerCount=2;packet.senderPlayer=1;
        packet.sequence=sequence;packet.senderFrame=frame;packet.frameAdvantage=lead;
        std::vector<std::uint8_t> wire;CHECK(EncodeInputPacket(packet,&wire));p.link.inbox.push_back(wire);
        CHECK(p.channel.Pump(p.gate,p.core,mesh.now,false));
    };
    for(unsigned frame=1;frame<=40;++frame)send(frame,frame,-50);
    CHECK(p.channel.IntervalScale()>1.005&&p.channel.IntervalScale()<=1.02);
    const auto scale=p.channel.IntervalScale(),lead=p.channel.FrameLead();
    for(unsigned i=0;i<40;++i){send(100+i,40,50);send(1,41,50);send(200+i,41,50,999);}
    CHECK(p.channel.IntervalScale()==scale&&p.channel.FrameLead()==lead);
    // Pacing observations neither admit input nor move the simulation frontier.
    CHECK(p.core.LastSimulatedFrame()==INVALID_FRAME&&p.core.ConfirmedThroughAllRemotes()==INVALID_FRAME);
    p.channel.Clear();mesh.begin(0,200);
    CHECK(p.channel.IntervalScale()==1&&p.channel.FrameLead()==0);
}
// Reused from TH09's independently tested input-batch work (512353c).
struct InputSendTrace {
    struct Send {bool reliable;std::uint64_t at;InputPacket packet;std::vector<std::uint8_t> wire;};
    Mesh& mesh;std::vector<Send> sends;unsigned controls=0;
    explicit InputSendTrace(Mesh& owner):mesh(owner){
        mesh.drop=[this](std::uint8_t from,std::uint8_t,bool reliable,const std::vector<std::uint8_t>& wire){
            if(from==0){
                InputPacket input;SessionPacket control;
                if(DecodeInputPacket(wire.data(),wire.size(),&input))sends.push_back({reliable,mesh.now,input,wire});
                else if(DecodeSessionPacket(wire.data(),wire.size(),&control))++controls;
            }
            return false;
        };
    }
    ~InputSendTrace(){mesh.drop=nullptr;}
};
void capture_send_coalesces_without_sequence_holes(){
    Mesh mesh(2);mesh.barrier();InputSendTrace trace(mesh);
    auto& p=*mesh.peers[0];auto& remote=*mesh.peers[1];
    capture(p,0,mesh.now);capture(remote,0,mesh.now);
    CHECK(trace.sends.size()==1&&trace.sends[0].packet.sequence==1);
    ++mesh.now;
    CHECK(p.channel.Pump(p.gate,p.core,mesh.now,true,true));
    CHECK(p.core.ConfirmedThrough(1)==0&&trace.sends.size()==1);
    capture(p,1,mesh.now);
    CHECK(p.channel.FlushDeferredInputs(p.core,mesh.now));
    CHECK(trace.sends.size()==2);
    const auto& merged=trace.sends.back().packet;
    CHECK(merged.sequence==2&&merged.ackSequence==1&&merged.ackFrame==0);
    CHECK(merged.firstInputFrame==0&&merged.inputCount==2&&merged.latestFrame==1);
    CHECK(merged.inputs[0]==sample(0,0)&&merged.inputs[1]==sample(0,1));
    CHECK(p.core.LastSimulatedFrame()==INVALID_FRAME&&p.captures==2);
    // The independent/default Pump still sends the owed tail immediately.
    mesh.now+=SessionChannelConfig{}.inputResendMs;
    CHECK(p.channel.Pump(p.gate,p.core,mesh.now,true));
    CHECK(trace.sends.size()==3&&trace.sends.back().packet.sequence==3);
    capture(p,2,mesh.now);
    CHECK(p.channel.FlushDeferredInputs(p.core,mesh.now));
    CHECK(trace.sends.size()==4&&trace.sends.back().packet.sequence==4&&p.captures==3);
}
void deferred_send_flushes_without_fresh_capture(){
    for(unsigned outcome=0;outcome<4;++outcome){
        Mesh mesh(2);mesh.barrier();InputSendTrace trace(mesh);
        auto& p=*mesh.peers[0];capture(p,0,mesh.now);capture(*mesh.peers[1],0,mesh.now);
        ++mesh.now;CHECK(p.channel.Pump(p.gate,p.core,mesh.now,true,true));
        CHECK(trace.sends.size()==1);
        if(outcome==0)CHECK(!p.core.PrepareFrame(1).canAdvance);
        if(outcome==1){
            CHECK(!p.channel.LocalCaptured(p.core,2,mesh.now));
            CHECK(p.channel.Error()==SessionChannel::Failure::InvalidCapture);
        }
        if(outcome==2)CHECK(p.channel.LocalCaptured(p.core,0,mesh.now));
        if(outcome==3)p.link.open=false;
        CHECK(p.channel.FlushDeferredInputs(p.core,mesh.now));
        if(outcome==3){
            CHECK(trace.sends.size()==1);
            p.link.open=true;CHECK(p.channel.Pump(p.gate,p.core,mesh.now,true));
        }
        CHECK(trace.sends.size()==2&&trace.sends.back().packet.sequence==2);
        const auto& sent=trace.sends.back().packet;
        CHECK(sent.ackFrame==0&&sent.latestFrame==0&&sent.inputCount==1&&sent.inputs[0]==sample(0,0));
        CHECK(p.channel.FlushDeferredInputs(p.core,mesh.now));
        CHECK(trace.sends.size()==2&&p.captures==1);
        if(outcome==1)CHECK(p.channel.Error()==SessionChannel::Failure::InvalidCapture);
    }
}
void deferred_send_keeps_old_redundant_coverage(){
    {
        Mesh mesh(2);mesh.barrier();InputSendTrace trace(mesh);auto& p=*mesh.peers[0];
        capture(p,0,mesh.now);capture(*mesh.peers[1],0,mesh.now);
        ++mesh.now;CHECK(p.channel.Pump(p.gate,p.core,mesh.now,true,true));
        CHECK(trace.sends.size()==1);
        // A caller leaving the consecutive-capture contract must flush BEFORE
        // scheduling: this jump overwrites frame zero's physical ring slot.
        CHECK(p.channel.FlushDeferredInputs(p.core,mesh.now));
        CHECK(trace.sends.size()==2&&trace.sends[1].packet.inputs[0]==sample(0,0));
        capture(p,static_cast<std::uint32_t>(INPUT_HISTORY_SIZE),mesh.now);
        CHECK(!p.core.HasLocalCapture(0));
        CHECK(trace.sends.size()==3&&trace.sends[2].packet.sequence==3);
        CHECK(p.channel.FlushDeferredInputs(p.core,mesh.now));CHECK(trace.sends.size()==3);
    }
    {
        Mesh mesh(2);mesh.barrier();InputSendTrace trace(mesh);auto& p=*mesh.peers[0];
        capture(p,0,mesh.now);capture(*mesh.peers[1],0,mesh.now);
        ++mesh.now;CHECK(p.channel.Pump(p.gate,p.core,mesh.now,true,true));
        CHECK(p.core.ScheduleLocalInput(1,sample(0,1)));
        capture(p,2,mesh.now); // short jump fallback, with the old slots still intact
        CHECK(p.channel.FlushDeferredInputs(p.core,mesh.now));CHECK(trace.sends.size()==3);
        CHECK(trace.sends[1].packet.sequence==2&&trace.sends[1].packet.latestFrame==0);
        CHECK(trace.sends[2].packet.sequence==3&&trace.sends[2].packet.inputCount==3);
        for(unsigned i=0;i<3;++i)CHECK(trace.sends[2].packet.inputs[i]==sample(0,i));
    }
    {
        Mesh mesh(2);mesh.barrier();InputSendTrace trace(mesh);auto& p=*mesh.peers[0];
        for(std::uint32_t frame=0;frame<31;++frame)capture(p,frame,mesh.now);
        trace.sends.clear();mesh.now+=SessionChannelConfig{}.inputResendMs;
        CHECK(p.channel.Pump(p.gate,p.core,mesh.now,true,true));
        CHECK(trace.sends.empty());capture(p,31,mesh.now);
        CHECK(p.channel.FlushDeferredInputs(p.core,mesh.now));
        CHECK(trace.sends.size()==1&&trace.sends[0].packet.sequence==32);
        CHECK(trace.sends[0].packet.firstInputFrame==0&&trace.sends[0].packet.inputCount==32);
        mesh.now+=SessionChannelConfig{}.inputResendMs;
        CHECK(p.channel.Pump(p.gate,p.core,mesh.now,true,true));
        CHECK(trace.sends.size()==2&&trace.sends[1].packet.sequence==33);
        CHECK(trace.sends[1].packet.firstInputFrame==0&&trace.sends[1].packet.latestFrame==31);
        capture(p,32,mesh.now);CHECK(p.channel.FlushDeferredInputs(p.core,mesh.now));
        CHECK(trace.sends.size()==3&&trace.sends[2].packet.sequence==34);
        CHECK(trace.sends[2].packet.firstInputFrame==1&&trace.sends[2].packet.latestFrame==32);
        for(unsigned i=0;i<32;++i)CHECK(trace.sends[1].packet.inputs[i]==sample(0,i));
    }
}
void deferred_send_retries_transport_rejection(){
    for(bool backpressured:{false,true}){
        Mesh mesh(2);mesh.barrier();InputSendTrace trace(mesh);auto& p=*mesh.peers[0];
        capture(p,0,mesh.now);capture(*mesh.peers[1],0,mesh.now);
        ++mesh.now;CHECK(p.channel.Pump(p.gate,p.core,mesh.now,true,true));
        if(backpressured)p.link.buffered=SessionChannelConfig{}.bufferedLimit+1;
        else p.link.rejectInput=true;
        capture(p,1,mesh.now);
        CHECK(p.channel.FlushDeferredInputs(p.core,mesh.now));CHECK(trace.sends.size()==1);
        CHECK(p.channel.Error()==SessionChannel::Failure::None&&p.captures==2);
        p.link.buffered=0;p.link.rejectInput=false;
        mesh.now+=SessionChannelConfig{}.inputResendMs;
        CHECK(p.channel.Pump(p.gate,p.core,mesh.now,true));
        CHECK(trace.sends.size()==2);
        // Sequence 2 was a real, refused ordinary attempt, as on the original
        // path. Coalescing itself never allocates a cancelled sequence.
        const auto& retry=trace.sends.back().packet;
        CHECK(retry.sequence==3&&retry.ackFrame==0&&retry.firstInputFrame==0&&retry.inputCount==2);
        CHECK(retry.inputs[0]==sample(0,0)&&retry.inputs[1]==sample(0,1));
        CHECK(p.channel.FlushDeferredInputs(p.core,mesh.now));CHECK(trace.sends.size()==2);
    }
}
void deferred_send_keeps_repair_order_and_clock(){
    for(bool backpressured:{false,true}){
        Mesh mesh(2);SessionChannelConfig policy;policy.repairIntervalMs=InputRepairBudget::StalledMs;
        for(auto& peer:mesh.peers){peer->channel.Clear();CHECK(peer->channel.BeginSession(peer->gate.Config(),mesh.now,policy));}
        mesh.barrier();InputSendTrace trace(mesh);auto& p=*mesh.peers[0];
        capture(p,0,mesh.now);
        if(backpressured)p.link.buffered=policy.bufferedLimit+1;
        mesh.now+=policy.repairIntervalMs;
        CHECK(p.channel.Pump(p.gate,p.core,mesh.now,true,true));
        CHECK(trace.sends.size()==(backpressured?2u:3u));
        const auto& repair=trace.sends.back();
        CHECK(repair.reliable&&repair.packet.sequence==2&&repair.packet.inputs[0]==sample(0,0));
        if(!backpressured)CHECK(!trace.sends[1].reliable&&trace.sends[1].wire==repair.wire);
        const auto count=trace.sends.size();
        CHECK(p.channel.FlushDeferredInputs(p.core,mesh.now));CHECK(trace.sends.size()==count);
    }
    for(bool fresh:{false,true}){
        Mesh mesh(2);SessionChannelConfig policy;policy.repairIntervalMs=InputRepairBudget::StalledMs;
        for(auto& peer:mesh.peers){peer->channel.Clear();CHECK(peer->channel.BeginSession(peer->gate.Config(),mesh.now,policy));}
        mesh.barrier();InputSendTrace trace(mesh);
        auto& p=*mesh.peers[0];auto& remote=*mesh.peers[1];const auto start=mesh.now;
        capture(p,0,mesh.now);
        CHECK(remote.channel.Pump(remote.gate,remote.core,mesh.now,true));
        capture(remote,0,mesh.now); // ACK 0 plus an input still needing our ACK
        mesh.now=start+policy.repairIntervalMs-1;
        CHECK(p.channel.Pump(p.gate,p.core,mesh.now,true,true));
        CHECK(trace.sends.size()==1&&p.core.AcknowledgedLocalThrough(1)==0);
        ++mesh.now;
        if(fresh)capture(p,1,mesh.now);
        CHECK(p.channel.FlushDeferredInputs(p.core,mesh.now));CHECK(trace.sends.size()==2);
        // Receipt, not the later capture/flush, anchors the next repair.
        mesh.now=start+2*policy.repairIntervalMs-1;
        CHECK(p.channel.Pump(p.gate,p.core,mesh.now,true,true));
        CHECK(trace.sends.size()==4&&p.channel.RepairsSent()==1);
        CHECK(!trace.sends[2].reliable&&trace.sends[3].reliable&&trace.sends[2].wire==trace.sends[3].wire);
        CHECK(trace.sends[3].packet.sequence==3&&trace.sends[3].packet.ackFrame==0);
        CHECK(trace.sends[3].packet.inputCount==(fresh?1:0));
        CHECK(p.channel.FlushDeferredInputs(p.core,mesh.now));CHECK(trace.sends.size()==4);
    }
}
void deferred_send_preserves_control_and_epoch(){
    Mesh mesh(2);mesh.barrier();InputSendTrace trace(mesh);
    auto& p=*mesh.peers[0];auto& remote=*mesh.peers[1];
    capture(p,0,mesh.now);CHECK(remote.channel.Pump(remote.gate,remote.core,mesh.now,true));
    capture(remote,0,mesh.now);
    std::vector<std::uint8_t> hello;CHECK(EncodeSessionPacket(remote.gate.BuildPacket(SessionPhase::Hello),&hello));
    p.link.inbox.push_back(hello);mesh.now+=SessionChannelConfig{}.controlResendMs;
    const auto controls=trace.controls;
    CHECK(p.channel.Pump(p.gate,p.core,mesh.now,true,true));
    CHECK(trace.controls>controls&&trace.sends.size()==1);
    p.channel.Clear();mesh.begin(0,200);
    CHECK(p.channel.FlushDeferredInputs(p.core,mesh.now));
    CHECK(trace.sends.size()==1&&p.channel.LatestCapture()==INVALID_FRAME);
}
void adonis_pump_does_not_defer(){
    Mesh mesh(2);SessionChannelConfig policy;policy.adonisPhase=true;
    for(auto& peer:mesh.peers){peer->channel.Clear();CHECK(peer->channel.BeginSession(peer->gate.Config(),mesh.now,policy));}
    mesh.barrier();InputSendTrace trace(mesh);auto& p=*mesh.peers[0];
    capture(p,0,mesh.now);capture(*mesh.peers[1],0,mesh.now);
    ++mesh.now;CHECK(p.channel.Pump(p.gate,p.core,mesh.now,true,true));
    CHECK(trace.sends.size()==2&&trace.sends.back().packet.sequence==2);
    capture(p,1,mesh.now);CHECK(p.channel.FlushDeferredInputs(p.core,mesh.now));
    CHECK(trace.sends.size()==3&&trace.sends.back().packet.sequence==3);
}
void short_repair_policy(){
    // Compare the complete input + ACK exchange under all-fast-lane loss.
    // The test clock includes receive/pump cadence; no sleeping or game ticks.
    const auto recover=[](std::uint64_t interval){
        Mesh mesh(2);SessionChannelConfig policy;policy.repairIntervalMs=interval;
        for(auto& owner:mesh.peers){auto& p=*owner;p.channel.Clear();CHECK(p.channel.BeginSession(p.gate.Config(),mesh.now,policy));}
        mesh.barrier();mesh.drop=[](std::uint8_t,std::uint8_t,bool reliable,const std::vector<std::uint8_t>&){return !reliable;};
        const auto start=mesh.now;for(auto& p:mesh.peers)capture(*p,0,mesh.now);
        for(unsigned step=0;step<200;++step){
            mesh.pump(true,5);
            if(mesh.peers[0]->core.AcknowledgedLocalThroughAllRemotes()==0&&mesh.peers[1]->core.AcknowledgedLocalThroughAllRemotes()==0){
                CHECK(mesh.peers[0]->captures==1&&mesh.peers[1]->captures==1);
                CHECK(mesh.peers[0]->channel.RepairsSent()<=3&&mesh.peers[1]->channel.RepairsSent()<=3);
                return mesh.now-start;
            }
        }
        CHECK(false);return std::uint64_t(0);
    };
    const auto previous=recover(250),current=recover(InputRepairBudget::StalledMs);
    CHECK(current<=200&&previous>=500&&current<previous);
    std::printf("tail repair (input+ACK, all fast packets lost): %llu -> %llu ms\n",
        static_cast<unsigned long long>(previous),static_cast<unsigned long long>(current));
}
}
static void connection_recovery_keeps_captured_input(){
    Mesh mesh(2);for(unsigned i=0;i<50;++i)mesh.pump();
    for(auto& peer:mesh.peers){CHECK(peer->gate.CanStart());capture(*peer,0,mesh.now);}
    for(auto& peer:mesh.peers){peer->link.open=false;peer->link.recovering=true;}
    for(unsigned i=0;i<2000;++i)mesh.pump(true);
    for(auto& peer:mesh.peers){CHECK(!peer->gate.CanStart());CHECK(peer->channel.Error()==SessionChannel::Failure::None);CHECK(peer->core.HasLocalCapture(0));peer->link.open=true;peer->link.recovering=false;}
    for(unsigned i=0;i<50;++i)mesh.pump(true);
    for(auto& peer:mesh.peers){CHECK(peer->gate.CanStart());CHECK(peer->core.ConfirmedThroughAllRemotes()==0);CHECK(peer->captures==1);peer->link.open=false;peer->link.disconnected=true;}
    for(unsigned i=0;i<2000;++i)mesh.pump(true);
    for(auto& peer:mesh.peers){CHECK(!peer->gate.CanStart());CHECK(peer->channel.Error()==SessionChannel::Failure::None);}
}
int main(){
    future_capture_tail_remains_deliverable();
    connection_recovery_keeps_captured_input();
    lossy_three_peer_stream();retired_final_ack_echo();reliable_tail_repairs_blackout();
    retirement_pump_retries_refused_ack_without_draining_calibration();
    retirement_pump_rejects_active_session_and_bad_clock();
    explicit_retirement_fence_recovers_before_repair_timer();timeouts_and_invalid_ack();
    explicit_retirement_fence_recovers_before_repair_timer(true);
    pacing_validated_packets_and_reset();
    short_repair_policy();
    reliable_repairs_survive_fast_lane_backpressure();
    backpressured_total_outage_still_times_out();
    capture_send_coalesces_without_sequence_holes();
    deferred_send_flushes_without_fresh_capture();
    deferred_send_keeps_old_redundant_coverage();
    deferred_send_retries_transport_rejection();
    deferred_send_keeps_repair_order_and_clock();
    deferred_send_preserves_control_and_epoch();
    adonis_pump_does_not_defer();
    std::puts("eagler-common session channel: PASS");
}
