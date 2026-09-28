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
    unsigned mode = 0, sent = 0, impaired = 0;
    bool IsOpen() const override { return true; }
    bool Failed() const override { return false; }
    std::size_t BufferedAmount() const override { return 0; }
    bool Send(const std::uint8_t* bytes, std::size_t size, bool reliable) {
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
    explicit Driver(Link& link):session(link),hashes(800),captureCount(800){}
    void tick(unsigned side,std::uint64_t now,unsigned limit) {
        CHECK(session.Pump(now,session.Frame()<limit));
        if(!session.Ready())return;
        if(session.NeedsCapture()&&session.Frame()<limit){
            auto f=session.Frame();CHECK(++captureCount[f]==1);CHECK(session.Capture(input(side,f),now));
        }
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
            CHECK(!(d.inputs[p].buttons&(2|8)));CHECK(d.inputs[p].analogMode==AnalogMode::None);
        }
        checkpoints[f%RollbackSession::History]=state;state.step(d.inputs);hashes[f]=state.hash;
        CHECK(session.Complete(d));return true;
    }
};
void simulation(unsigned mode) {
    Link a,b;a.peer=&b;b.peer=&a;a.mode=b.mode=mode;
    Driver left(a),right(b);CHECK(left.session.Begin(config(0),0));CHECK(right.session.Begin(config(1),0));
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
    std::printf("mode=%u frames=%u corrections=%u/%u resim=%u/%u waits=%u/%u elapsed=%llu ms PASS\n",mode,frames,
        left.session.Corrections(),right.session.Corrections(),left.session.Resimulated(),right.session.Resimulated(),
        left.waits,right.waits,(unsigned long long)now);
}
void contracts() {
    Link a,b;a.peer=&b;b.peer=&a;RollbackSession left(a),right(b);
    CHECK(left.Begin(config(0),0));CHECK(right.Begin(config(1,2),0));
    bool failed=false;
    for(unsigned now=0;now<1000;++now){a.now=b.now=now;failed=!left.Pump(now)||!right.Pump(now);if(failed)break;}
    CHECK(failed);CHECK(!left.Ready()&&!right.Ready());
    CHECK(left.Begin(config(0),1000));CHECK(right.Begin(config(1),1000));a.pending.clear();b.pending.clear();
    for(unsigned now=1000;now<2000&&(!left.Ready()||!right.Ready());++now){a.now=b.now=now;CHECK(left.Pump(now));CHECK(right.Pump(now));}
    CHECK(left.Ready()&&right.Ready());CHECK(left.NeedsCapture());
    CHECK(left.Capture(input(0,0),2000));auto d=left.Prepare();CHECK(d.canAdvance);CHECK(d.inputs[0]==input(0,0));
    CHECK(!left.NeedsCapture());CHECK(!left.Capture(input(0,0),2000));
    FrameInput invalid;invalid.analogMode=AnalogMode::DirectTouch;invalid.x=std::numeric_limits<float>::infinity();
    CHECK(!RollbackSession::ValidInput(invalid));invalid.x=4097;CHECK(!RollbackSession::ValidInput(invalid));
    std::puts("ABI gate, zero-frame local input, once-only capture, input bounds PASS");
}
int main(){contracts();for(unsigned mode=0;mode<5;++mode)simulation(mode);return 0;}
