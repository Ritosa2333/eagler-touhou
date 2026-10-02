// Reuse the exact baseline wire fault injector and deterministic input oracle.
// This is a component model, not a TH09 game-world or performance benchmark.
#define TH09_SESSION_FIXTURE_ONLY
#include "session.cpp"

struct AdonisDriver {
    RollbackSession session;
    State state;
    std::array<State,RollbackSession::History> checkpoints{};
    std::array<unsigned,600> captured{};
    std::array<std::uint64_t,600> hashes{};
    unsigned published=0,waits=0,snapshots=0;
    unsigned side,delay;
    AdonisMode mode;
    double due=0;
    AdonisDriver(Link& link,unsigned player,unsigned d,AdonisMode m):session(link),side(player),delay(d),mode(m) {
        CHECK(session.Begin(config(side),0,static_cast<std::uint8_t>(d),m));
    }
    FrameInput expected(unsigned player,unsigned frame) const {return frame<delay?FrameInput{}:input(player,frame-delay);}
    bool step() {
        auto d=session.Prepare();if(!d.canAdvance)return false;
        const auto f=session.Frame();CHECK(f<600);CHECK(d.inputs[side]==expected(side,f));
        if(mode==AdonisMode::Delay){CHECK(!d.predictedMask);CHECK(session.InputsReconciledBeforeNext());}
        else if(d.predictedMask || !session.InputsReconciledBeforeNext())++snapshots;
        checkpoints[f%RollbackSession::History]=state;
        state.step(d.inputs);hashes[f]=state.hash;CHECK(session.Complete(d));return true;
    }
    void tick(std::uint64_t now) {
        CHECK(session.Pump(now,session.Frame()<600));if(!session.Ready())return;
        // Transfer phase delay to a forward-only deadline; no simulation tick
        // is dropped and no replay/capture clocks enter the world checkpoint.
        due+=1-session.PacedElapsedMs(1);
        if(now>=due && session.Frame()<600){
            auto f=session.Frame();
            if(session.NeedsCapture()){CHECK(++captured[f]==1);CHECK(session.Capture(input(side,f),now));}
        }
        if(session.RollbackFrame()!=INVALID_FRAME){
            CHECK(mode!=AdonisMode::Delay);
            const auto f=session.RollbackFrame(),end=session.Frame();
            CHECK(end-f<=RollbackSession::History);state=checkpoints[f%RollbackSession::History];
            CHECK(session.Restored(f));while(session.Frame()<end)CHECK(step());
        }
        if(now>=due && session.Frame()<600) {
            if(step())due=std::max(due+1000.0/60,double(now));else ++waits;
        }
        const auto confirmed=session.ConfirmedThrough();
        while(confirmed!=INVALID_FRAME && published<=confirmed) {
            std::array<FrameInput,MAX_PLAYERS> exact;CHECK(session.ConfirmedInputs(published,&exact));
            for(unsigned p=0;p<2;++p)CHECK(exact[p]==expected(p,published));
            ++published;
        }
    }
};
struct Counts {unsigned resim,waits,snapshots;};
Counts runAdonis(AdonisMode mode,unsigned delay,unsigned impairment) {
    Link a,b;a.peer=&b;b.peer=&a;a.mode=b.mode=impairment;
    AdonisDriver left(a,0,delay,mode),right(b,1,delay,mode);std::uint64_t now=0;
    for(;now<150000;++now){a.now=b.now=now;left.tick(now);right.tick(now);
        if(left.published==600&&right.published==600&&left.session.CanRetire()&&right.session.CanRetire())break;}
    CHECK(now<150000);State reference;
    for(unsigned f=0;f<600;++f){
        const std::array<FrameInput,MAX_PLAYERS> exact{left.expected(0,f),left.expected(1,f),{}};
        reference.step(exact);CHECK(left.hashes[f]==reference.hash&&right.hashes[f]==reference.hash);
        CHECK(left.captured[f]==1&&right.captured[f]==1);
    }
    CHECK(left.session.Frame()==600&&right.session.Frame()==600);
    if(mode==AdonisMode::Delay){
        CHECK(!left.session.Corrections()&&!right.session.Corrections());
        CHECK(!left.session.Resimulated()&&!right.session.Resimulated());
        CHECK(!left.snapshots&&!right.snapshots);
    }
    const Counts c{left.session.Resimulated()+right.session.Resimulated(),left.waits+right.waits,left.snapshots+right.snapshots};
    std::printf("AD model mode=%u D=%u network=%u frames=600 elapsed=%llu corrections=%u/%u resim=%u waits=%u snapshots=%u phase=%u/%u PASS\n",
        unsigned(mode),delay,impairment,(unsigned long long)now,left.session.Corrections(),right.session.Corrections(),c.resim,c.waits,c.snapshots,
        left.session.Channel().AdonisStatistics().Adjustments(),right.session.Channel().AdonisStatistics().Adjustments());
    CHECK(left.session.Retire(now));CHECK(right.session.Retire(now));return c;
}
void mismatch(AdonisMode aMode,unsigned aDelay,AdonisMode bMode,unsigned bDelay) {
    Link a,b;a.peer=&b;b.peer=&a;RollbackSession left(a),right(b);
    CHECK(left.Begin(config(0),0,aDelay,aMode));CHECK(right.Begin(config(1),0,bDelay,bMode));
    bool failed=false;
    for(unsigned now=0;now<1000&&!failed;++now){a.now=b.now=now;failed=!left.Pump(now)||!right.Pump(now);}
    CHECK(failed);CHECK(!left.Ready()&&!right.Ready());
}
int main(){
    mismatch(AdonisMode::Delay,3,AdonisMode::Hybrid,3);
    mismatch(AdonisMode::Delay,3,AdonisMode::Rollback,3);
    mismatch(AdonisMode::Hybrid,2,AdonisMode::Hybrid,3);
    mismatch(AdonisMode::Delay,3,AdonisMode::Delay,4);
    for(auto delay:{0u,1u,3u,9u})for(unsigned network=0;network<5;++network)runAdonis(AdonisMode::Delay,delay,network);
    for(unsigned network=1;network<5;++network){
        const auto baseline=runAdonis(AdonisMode::Rollback,0,network);
        const auto hybrid=runAdonis(AdonisMode::Hybrid,2,network);
        if(network==1)CHECK(hybrid.resim<baseline.resim);
    }
    puts("TH09 Adonis model correctness, once-only input, retirement, mode/delay mismatch and hybrid comparison PASS");
}
