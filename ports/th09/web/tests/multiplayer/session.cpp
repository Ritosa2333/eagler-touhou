#include "RollbackSession.hpp"
#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <deque>
#include <limits>
#include <vector>

using namespace Netplay;
using th09::multiplayer::RollbackSession;
#define CHECK(x) do { if (!(x)) { std::fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x); std::abort(); } } while (0)

struct Packet { std::uint64_t due; std::vector<std::uint8_t> bytes; };
struct Link : PeerTransport {
    Link* peer = nullptr;
    std::deque<Packet> pending;
    std::uint64_t now = 0;
    unsigned mode = 0, sent = 0, impaired = 0, dropCalibrationKind = 0;
    bool recovering=false,disconnected=false,suspended=false;
    bool IsOpen() const override { return !disconnected; }
    bool Failed() const override { return false; }
    bool Recovering()const override{return recovering;}
    bool Disconnected()const override{return disconnected;}
    bool CalibrationSuspended()const override{return suspended;}
    std::size_t BufferedAmount() const override { return 0; }
    bool Send(const std::uint8_t* bytes, std::size_t size, bool reliable) {
        if(AdonisStartup::IsPacket(bytes,size)){
            if(dropCalibrationKind&&bytes[4]==dropCalibrationKind){dropCalibrationKind=0;return true;}
            const bool probe=bytes[4]==2||bytes[4]==3;
            if(mode==3&&probe){++impaired;return true;}
            const auto delay=probe&&mode?25+(bytes[28]*17)%37:0;
            peer->pending.push_back({now+delay,{bytes,bytes+size}});return true;
        }
        AdonisPhaseSample phase;
        if(DecodeAdonisPhaseSample(bytes,size,&phase)){
            peer->pending.push_back({now+25,{bytes,bytes+size}});return true;
        }
        PacketType type; CHECK(PeekPacketType(bytes,size,&type));
        std::uint64_t delay = 0;
        if (type == PacketType::Input) {
            ++sent;
            if (mode) delay = 25 + (sent * 17) % 37;
            if (mode == 2 && now >= 1400 && now < 2200) { delay += 800; ++impaired; }
            if (mode == 3 && !reliable) { ++impaired; return true; }
            if (mode == 4 && !reliable && sent % 5 == 0) { ++impaired; return true; }
        }
        peer->pending.push_back({now+delay,{bytes,bytes+size}});
        if (mode == 4 && sent % 7 == 0) peer->pending.push_back({now+delay+75,{bytes,bytes+size}});
        return true;
    }
    bool SendTo(std::uint8_t, const std::uint8_t* p,std::size_t n) override { return Send(p,n,false); }
    bool SendRepairTo(std::uint8_t, const std::uint8_t* p,std::size_t n) override { return Send(p,n,true); }
    bool SendControl(const std::uint8_t* p,std::size_t n) override { return Send(p,n,true); }
    bool Poll(std::vector<std::uint8_t>* out) override {
        // Deliberately allow new low-latency packets to overtake delayed tails.
        auto it=std::find_if(pending.begin(),pending.end(),[&](const auto& p){return p.due<=now;});
        if(it==pending.end())return false;
        *out=std::move(it->bytes);pending.erase(it);return true;
    }
};
SessionConfig config(unsigned side, unsigned abi=1) {
    SessionConfig c; c.sessionId=0x901234; c.seed=123; c.gameplayAbi=abi; c.gameId=9;
    c.localPlayer=side; c.playerCount=2; return c;
}
FrameInput input(unsigned seat,unsigned frame) {
    FrameInput in;
    in.buttons=((frame/13+seat)%2?64:128) | (frame%19<14?1:0) | (frame%43==0?2:0) | (frame%71==0?8:0);
    if(frame%17<9){in.analogMode=AnalogMode::DirectTouch;in.touchUsed=true;in.x=float(int(frame%201)-100);in.y=float(80+seat*9);}
    return in;
}
// An independent deterministic state machine, intentionally sensitive to
// action edges and pointer coordinates. This verifies adapter/protocol
// behavior; it is not a TH09 world or a browser performance benchmark.
struct State {
    std::array<std::int64_t,2> x{},charge{},bombs{},menus{};
    std::array<unsigned,2> previous{};
    std::uint64_t hash=123;
    void step(const std::array<FrameInput,MAX_PLAYERS>& in) {
        for(unsigned p=0;p<2;++p){
            const auto& a=in[p];x[p]+=(a.buttons&128?1:0)-(a.buttons&64?1:0);
            if(a.analogMode==AnalogMode::DirectTouch)x[p]=std::int64_t(a.x);
            if(a.buttons&1)++charge[p];else if(previous[p]&1){hash^=charge[p]+17;charge[p]=0;}
            if((a.buttons&2)&&!(previous[p]&2))++bombs[p];
            if((a.buttons&8)&&!(previous[p]&8))++menus[p];
            previous[p]=a.buttons;hash=hash*1099511628211ull+std::uint64_t(x[p])+bombs[p]*11+menus[p]*7;
        }
    }
};
struct Driver {
    RollbackSession session;
    State state;
    std::array<State,RollbackSession::History> checkpoints{};
    std::vector<std::uint64_t> hashes;
    std::vector<unsigned> captureCount;
    unsigned published=0, waits=0, localImmediate=0;
    bool deferInputs=false;
    explicit Driver(Link& link,bool defer=false):session(link),hashes(800),captureCount(800),deferInputs(defer){}
    void tick(unsigned side,std::uint64_t now,unsigned limit) {
        const bool pumped=session.Pump(now,session.Frame()<limit,0,deferInputs);
        if(!pumped){CHECK(session.FlushDeferredInputs(now));CHECK(pumped);}
        if(!session.Ready()){CHECK(session.FlushDeferredInputs(now));return;}
        if(session.NeedsCapture()&&session.Frame()<limit){
            auto f=session.Frame();
            CHECK(session.Channel().LatestCapture()==INVALID_FRAME||session.Channel().LatestCapture()+1==f);
            CHECK(++captureCount[f]==1);CHECK(session.Capture(input(side,f),now));
        }
        CHECK(session.FlushDeferredInputs(now));
        auto rollback=session.RollbackFrame();
        if(rollback!=INVALID_FRAME){
            const auto end=session.Frame();CHECK(end-rollback<=RollbackSession::History);
            state=checkpoints[rollback%RollbackSession::History];CHECK(session.Restored(rollback));
            while(session.Frame()<end)CHECK(step(side));
        }
        if(session.Frame()<limit&&!step(side))++waits;
        const auto confirmed=session.ConfirmedThrough();
        while(confirmed!=INVALID_FRAME&&published<=confirmed){
            std::array<FrameInput,MAX_PLAYERS> exact;
            CHECK(session.ConfirmedInputs(published,&exact));
            for(unsigned p=0;p<2;++p)CHECK(exact[p]==input(p,published));
            ++published;
        }
    }
    bool step(unsigned side){
        const auto f=session.Frame();auto d=session.Prepare();if(!d.canAdvance)return false;
        CHECK(d.inputs[side]==input(side,f));++localImmediate;
        for(unsigned p=0;p<2;++p)if(d.predictedMask&(1u<<p)){
            CHECK(!(d.inputs[p].buttons&(2|8))&&!d.inputs[p].touchBomb);
            CHECK(RollbackSession::ValidInput(d.inputs[p]));
        }
        checkpoints[f%RollbackSession::History]=state;state.step(d.inputs);hashes[f]=state.hash;
        CHECK(session.Complete(d));return true;
    }
};
void simulation(unsigned mode,bool deferInputs=false) {
    Link a,b;a.peer=&b;b.peer=&a;a.mode=b.mode=mode;
    Driver left(a,deferInputs),right(b,deferInputs);CHECK(left.session.Begin(config(0),0));CHECK(right.session.Begin(config(1),0));
    constexpr unsigned frames=600;std::uint64_t now=0;
    for(;now<50000;++now){
        a.now=b.now=now;
        if(now%16==0){left.tick(0,now,frames);right.tick(1,now,frames);}
        if(left.published==frames&&right.published==frames&&left.session.CanRetire()&&right.session.CanRetire())break;
    }
    CHECK(now<50000);CHECK(left.session.Frame()==frames&&right.session.Frame()==frames);
    State reference;
    for(unsigned f=0;f<frames;++f){
        std::array<FrameInput,MAX_PLAYERS> exact{input(0,f),input(1,f),{}};reference.step(exact);
        CHECK(left.hashes[f]==reference.hash);CHECK(right.hashes[f]==reference.hash);
        CHECK(left.captureCount[f]==1&&right.captureCount[f]==1);
    }
    CHECK(left.session.Captures()==frames&&right.session.Captures()==frames);
    CHECK(left.session.Corrections()+right.session.Corrections()>0);
    if(mode)CHECK(left.session.Corrections()>0&&right.session.Corrections()>0);
    if(mode==2||mode==3)CHECK(a.impaired>0&&left.waits>0);
    if(mode==3)CHECK(left.session.Channel().RepairsSent()>0&&right.session.Channel().RepairsSent()>0);
    CHECK(left.session.Retire(now));CHECK(right.session.Retire(now));
    CHECK(!left.session.Ready()&&!right.session.Ready());
    std::printf("mode=%u deferInputs=%u frames=%u corrections=%u/%u resim=%u/%u waits=%u/%u elapsed=%llu ms PASS\n",mode,unsigned(deferInputs),frames,
        left.session.Corrections(),right.session.Corrections(),left.session.Resimulated(),right.session.Resimulated(),
        left.waits,right.waits,(unsigned long long)now);
}
void contracts() {
    Link a,b;a.peer=&b;b.peer=&a;RollbackSession left(a),right(b);
    CHECK(left.Begin(config(0),0));CHECK(right.Begin(config(1,2),0));
    bool failed=false;
    for(unsigned now=0;now<1000;++now){a.now=b.now=now;failed=!left.Pump(now)||!right.Pump(now);if(failed)break;}
    CHECK(failed);CHECK(!left.Ready()&&!right.Ready());
    CHECK(left.Begin(config(0),1000,2));CHECK(right.Begin(config(1),1000,3));a.pending.clear();b.pending.clear();
    failed=false;
    for(unsigned now=1000;now<2000;++now){a.now=b.now=now;failed=!left.Pump(now)||!right.Pump(now);if(failed)break;}
    CHECK(failed);CHECK(!left.Ready()&&!right.Ready());
    CHECK(left.Begin(config(0),1000));CHECK(right.Begin(config(1),1000));a.pending.clear();b.pending.clear();
    for(unsigned now=1000;now<2000&&(!left.Ready()||!right.Ready());++now){a.now=b.now=now;CHECK(left.Pump(now));CHECK(right.Pump(now));}
    CHECK(left.Ready()&&right.Ready());CHECK(left.NeedsCapture());
    CHECK(left.Capture(input(0,0),2000));auto d=left.Prepare();CHECK(d.canAdvance);CHECK(d.inputs[0]==input(0,0));
    CHECK(!left.NeedsCapture());CHECK(!left.Capture(input(0,0),2000));
    FrameInput invalid;invalid.analogMode=AnalogMode::DirectTouch;invalid.x=std::numeric_limits<float>::infinity();
    CHECK(!RollbackSession::ValidInput(invalid));invalid.x=4097;CHECK(!RollbackSession::ValidInput(invalid));
    CHECK(left.Begin(config(0),3000,3));CHECK(right.Begin(config(1),3000,3));a.pending.clear();b.pending.clear();
    for(unsigned now=3000;now<4000&&(!left.Ready()||!right.Ready());++now){a.now=b.now=now;CHECK(left.Pump(now));CHECK(right.Pump(now));}
    CHECK(left.Ready()&&right.Ready());
    for(unsigned capture=0;capture<4;++capture){
        a.now=b.now=4000+capture;
        CHECK(left.NeedsCapture());CHECK(left.Capture(input(0,capture),a.now));
        auto delayed=left.Prepare();CHECK(delayed.canAdvance);
        CHECK(delayed.inputs[0]==(capture<3?FrameInput{}:input(0,0)));
        CHECK(left.Complete(delayed));
    }
    std::puts("ABI gate, zero-frame local input, once-only capture, input bounds PASS");
}
#ifndef TH09_SESSION_FIXTURE_ONLY
struct PacingFixture {
    Link a,b;
    Driver left,right;
    std::uint64_t now=0;
    unsigned sequence=0,remoteFrame=0;
    explicit PacingFixture(AdonisMode mode=AdonisMode::Rollback):left(a),right(b) {
        a.peer=&b;b.peer=&a;
        CHECK(left.session.Begin(config(0),0,0,mode));
        CHECK(right.session.Begin(config(1),0,0,mode));
        // A real handshake at time zero catches clock-origin sentinel bugs.
        for(unsigned i=0;i<32&&(!left.session.Ready()||!right.session.Ready());++i){
            CHECK(left.session.Pump(0));CHECK(right.session.Pump(0));
            CHECK(left.session.IntervalScale()==left.session.Channel().IntervalScale());
        }
        CHECK(left.session.Ready()&&right.session.Ready());
        CHECK(left.session.InputDelay()==0);
        a.pending.clear();b.pending.clear();
    }
    void pump(std::uint64_t at,bool expectsInput=true) {
        a.now=b.now=now=at;CHECK(left.session.Pump(now,expectsInput));
    }
    void capture() {
        const auto f=left.session.Frame();CHECK(left.session.NeedsCapture());
        CHECK(++left.captureCount[f]==1);CHECK(left.session.Capture(input(0,f),now));
    }
    void advance(unsigned count) {
        for(unsigned i=0;i<count;++i){capture();CHECK(left.step(0));}
    }
    void actual(unsigned first,unsigned last) {
        // Deliver a valid slice of the peer tape, permitting an earlier hole
        // to remain missing while later actual inputs arrive through Link.
        CHECK(first<=last&&last-first+1<=MAX_REDUNDANT_INPUTS);
        InputPacket packet;packet.sessionId=config(1).sessionId;packet.sequence=++sequence;
        packet.senderPlayer=1;packet.playerCount=2;packet.firstInputFrame=first;
        packet.latestFrame=last;packet.inputCount=std::uint8_t(last-first+1);
        packet.senderFrame=remoteFrame=std::max(remoteFrame,last+1);
        for(unsigned i=0;i<packet.inputCount;++i)packet.inputs[i]=input(1,first+i);
        Packet wire{now,{}};CHECK(EncodeInputPacket(packet,&wire.bytes));
        a.pending.push_back(std::move(wire));
    }
    void restore() {
        const auto f=left.session.RollbackFrame();CHECK(f!=INVALID_FRAME);
        CHECK(left.session.Frame()-f<=RollbackSession::History);
        left.state=left.checkpoints[f%RollbackSession::History];CHECK(left.session.Restored(f));
    }
};
void input_batch_scopes() {
    {
        PacingFixture f;f.advance(8);f.actual(0,7);f.pump(20);
        CHECK(f.left.session.RollbackFrame()==0);f.restore();
        CHECK(f.left.session.Frame()==0&&!f.left.session.NeedsCapture());
        const auto sent=f.a.sent;
        f.a.now=f.b.now=f.now=60;
        CHECK(f.left.session.Pump(60,true,0,true));
        // An unfinished replay cannot postpone an ACK for a future capture,
        // even if its caller supplies the opt-in hint on every callback.
        CHECK(f.a.sent==sent+1&&f.left.session.Captures()==8);
        CHECK(f.left.session.FlushDeferredInputs(60));CHECK(f.a.sent==sent+1);
        CHECK(f.left.step(0)&&!f.left.session.NeedsCapture());
    }
    {
        PacingFixture f;f.advance(1);f.actual(0,0);
        f.a.now=f.b.now=f.now=1;const auto sent=f.a.sent;
        CHECK(f.left.session.Pump(1,true,0,true));CHECK(f.a.sent==sent);
        FrameInput invalid;invalid.analogMode=AnalogMode::DirectTouch;
        invalid.x=std::numeric_limits<float>::infinity();
        CHECK(!f.left.session.Capture(invalid,1));
        CHECK(f.left.session.Failed()&&f.left.session.Captures()==1);
        CHECK(f.left.session.FlushDeferredInputs(1));CHECK(f.a.sent==sent+1);
        InputPacket old;const auto& wire=f.b.pending.back().bytes;
        CHECK(DecodeInputPacket(wire.data(),wire.size(),&old));
        CHECK(old.sequence==2&&old.ackFrame==0&&old.latestFrame==0);
        CHECK(old.inputCount==1&&old.inputs[0]==input(0,0));
        CHECK(f.left.session.Failed());
    }
    std::puts("Input send batch: partial replay and rejected capture flush PASS");
}
void network_pacing() {
    {
        PacingFixture f;
        for(unsigned count:{4u,1u,2u,1u}){
            f.advance(count);f.pump(f.now+16);
            CHECK(f.left.session.MissingRemoteInputs()==f.left.session.Frame());
            CHECK(f.left.session.NetworkIntervalScale()==1);
            CHECK(f.left.session.IntervalScale()==f.left.session.Channel().IntervalScale());
        }
        f.capture();CHECK(!f.left.session.Prepare().canAdvance);
        const auto captures=f.left.session.Captures();
        for(unsigned i=0;i<8;++i)f.pump(100);
        CHECK(f.left.session.Captures()==captures);
        CHECK(f.left.session.Frame()==RollbackSession::History);
        CHECK(f.left.session.IntervalScale()==1);
        // Preserve the channel's existing ClockReversed failure contract.
        f.a.now=f.b.now=99;CHECK(!f.left.session.Pump(99));CHECK(f.left.session.Failed());
        CHECK(!f.left.session.Ready());
        CHECK(f.left.session.IntervalScale()==f.left.session.Channel().IntervalScale());
        f.left.session.Clear();CHECK(f.left.session.IntervalScale()==1);
        CHECK(f.left.session.Begin(config(0),0));CHECK(f.left.session.IntervalScale()==1);
    }
    {
        // One slow callback can have a complete remote tape waiting in Link.
        // Pending correction and an unfinished eight-frame replay are CPU
        // work, not missing network information.
        PacingFixture f;f.advance(8);f.actual(0,7);f.pump(1000);
        CHECK(f.left.session.RollbackFrame()==0);
        CHECK(f.left.session.ConfirmedThrough()==INVALID_FRAME);
        CHECK(f.left.session.IntervalScale()==f.left.session.Channel().IntervalScale());
        f.restore();f.pump(1200);CHECK(f.left.session.Frame()==0);
        CHECK(f.left.session.ConfirmedThrough()==INVALID_FRAME);
        CHECK(f.left.session.IntervalScale()==f.left.session.Channel().IntervalScale());
    }
    {
        // Removing pre-emptive slowdown must not admit a ninth prediction,
        // forget an earlier input hole, or resample a held local capture.
        PacingFixture f;f.advance(8);f.capture();
        CHECK(!f.left.session.Prepare().canAdvance); // The ninth prediction stays forbidden.
        f.pump(82);f.actual(1,7);f.pump(98);
        CHECK(f.left.session.RollbackFrame()==1);
        CHECK(f.left.session.ConfirmedThrough()==INVALID_FRAME);
        CHECK(f.left.session.MissingRemoteInputs()==8); // Frame zero is still missing.
        f.restore();CHECK(f.left.session.Frame()==1);f.pump(114);
        CHECK(f.left.session.MissingRemoteInputs()==8); // Rewind did not create room.
        CHECK(f.left.step(0));f.actual(0,0);f.pump(130);
        CHECK(f.left.session.RollbackFrame()==0);
        CHECK(f.left.session.NetworkIntervalScale()==1);
        f.restore();CHECK(f.left.session.ConfirmedThrough()==INVALID_FRAME);
        f.pump(130);f.pump(180);CHECK(f.left.step(0));
        f.pump(230);CHECK(f.left.step(0));
        f.pump(280);CHECK(f.left.session.Frame()<8);
        CHECK(f.left.session.ConfirmedThrough()==INVALID_FRAME);
        CHECK(f.left.session.IntervalScale()==f.left.session.Channel().IntervalScale());
        while(f.left.session.Frame()<8)CHECK(f.left.step(0));
        State reference;
        for(unsigned frame=0;frame<8;++frame){
            std::array<FrameInput,MAX_PLAYERS> exact{input(0,frame),input(1,frame),{}};
            reference.step(exact);CHECK(f.left.hashes[frame]==reference.hash);
            CHECK(f.left.captureCount[frame]==1);
        }
        CHECK(f.left.captureCount[8]==1&&!f.left.session.NeedsCapture());
        const auto next=f.left.session.Prepare();CHECK(next.canAdvance);
        CHECK(next.inputs[0]==input(0,8));CHECK(f.left.session.InputDelay()==0);
    }
    {
        PacingFixture f;f.advance(7);f.pump(50);
        f.pump(50,false);CHECK(f.left.session.IntervalScale()==f.left.session.Channel().IntervalScale());
        f.pump(1000,false);f.pump(1000);
        CHECK(f.left.session.IntervalScale()==f.left.session.Channel().IntervalScale());
        f.pump(1010);CHECK(f.left.session.IntervalScale()==f.left.session.Channel().IntervalScale());
    }
    for(auto mode:{AdonisMode::Delay,AdonisMode::Hybrid}){
        PacingFixture f(mode);
        if(mode==AdonisMode::Hybrid)f.advance(7);
        else{f.capture();CHECK(!f.left.session.Prepare().canAdvance);}
        f.pump(1000);CHECK(f.left.session.IntervalScale()==f.left.session.Channel().IntervalScale());
    }
    std::puts("No shortage slowdown; hard horizon, input holes, exact replay, capture ownership and clock/mode contracts PASS");
}
void after_present_recovery(){
    PacingFixture f;f.advance(3);f.actual(0,2);f.pump(20);
    CHECK(f.left.session.RollbackFrame()==0);CHECK(!f.left.session.Prepare().canAdvance);
    f.capture();const auto visible=f.left.session.Prepare(true);CHECK(visible.canAdvance);
    f.left.checkpoints[3]=f.left.state;f.left.state.step(visible.inputs);
    CHECK(f.left.session.Complete(visible,true));CHECK(f.left.session.Frame()==4);
    CHECK(f.left.session.RollbackFrame()==0&&f.left.session.ConfirmedThrough()==INVALID_FRAME);
    // Correct after the visible tick. Replaying must reuse all four captures.
    f.actual(3,3);f.pump(21);f.restore();CHECK(!f.left.session.Prepare(true).canAdvance);
    while(f.left.session.Frame()<4)CHECK(f.left.step(0));
    State reference;for(unsigned n=0;n<4;++n){std::array<FrameInput,MAX_PLAYERS> exact{input(0,n),input(1,n),{}};reference.step(exact);CHECK(f.left.hashes[n]==reference.hash);CHECK(f.left.captureCount[n]==1);}
    CHECK(f.left.session.ConfirmedThrough()==3);
    // The additional visible tick must not extend an eight-frame old error.
    PacingFixture bound;bound.advance(8);bound.actual(0,7);bound.pump(20);bound.capture();
    CHECK(!bound.left.session.Prepare(true).canAdvance);CHECK(bound.left.session.Frame()==8);
    std::puts("After-present visible tick: exact reference, capture once, confirmation fence and history bound PASS");
}
int main(){
    contracts();network_pacing();input_batch_scopes();after_present_recovery();
    for(bool deferInputs:{false,true})for(unsigned mode=0;mode<5;++mode)simulation(mode,deferInputs);
    return 0;
}
#endif
