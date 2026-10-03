// Reuse the exact baseline wire fault injector and deterministic input oracle.
// This is a component model, not a TH09 game-world or performance benchmark.
#define TH09_SESSION_FIXTURE_ONLY
#include "session.cpp"

struct AdonisDriver {
    using Trace=FrameInput(*)(unsigned,unsigned);
    RollbackSession session;
    State state;
    std::array<State,RollbackSession::History> checkpoints{};
    std::array<unsigned,600> captured{};
    std::array<std::uint64_t,600> hashes{};
    unsigned published=0,waits=0,snapshots=0;
    unsigned side,delay;
    AdonisMode mode;
    Trace trace;
    double due=0;
    AdonisDriver(Link& link,unsigned player,unsigned d,AdonisMode m,Trace source=input,bool measured=false,unsigned reserve=2):session(link),side(player),delay(d),mode(m),trace(source) {
        CHECK(measured?session.BeginMeasured(config(side),0,d,m,reserve):session.Begin(config(side),0,static_cast<std::uint8_t>(d),m));
    }
    FrameInput expected(unsigned player,unsigned frame) const {return frame<delay?FrameInput{}:trace(player,frame-delay);}
    bool step() {
        auto d=session.Prepare();if(!d.canAdvance)return false;
        const auto f=session.Frame();CHECK(f<600);CHECK(d.inputs[side]==expected(side,f));
        if(mode==AdonisMode::Delay){CHECK(!d.predictedMask);CHECK(session.InputsReconciledBeforeNext());}
        else if(d.predictedMask || !session.InputsReconciledBeforeNext())++snapshots;
        checkpoints[f%RollbackSession::History]=state;
        state.step(d.inputs);hashes[f]=state.hash;CHECK(session.Complete(d));return true;
    }
    void tick(std::uint64_t now) {
        CHECK(session.Pump(now,session.Frame()<600));if(!session.Ready()){CHECK(!session.Captures());return;}
        delay=session.InputDelay();
        // Transfer phase delay to a forward-only deadline; no simulation tick
        // is dropped and no replay/capture clocks enter the world checkpoint.
        due+=1-session.PacedElapsedMs(1);
        if(now>=due && session.Frame()<600){
            auto f=session.Frame();
            if(session.NeedsCapture()){CHECK(++captured[f]==1);CHECK(session.Capture(trace(side,f),now));}
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
Counts runAdonis(AdonisMode mode,unsigned delay,unsigned impairment,AdonisDriver::Trace trace=input,bool measured=false,unsigned reserve=2,bool lostCommit=false) {
    Link a,b;a.peer=&b;b.peer=&a;a.mode=b.mode=impairment;
    if(lostCommit){a.dropCalibrationKind=7;b.dropCalibrationKind=8;}
    AdonisDriver left(a,0,delay,mode,trace,measured,reserve),right(b,1,delay,mode,trace,measured,reserve);std::uint64_t now=0;
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
    if(measured){
        CHECK(left.session.Startup().Ready()&&right.session.Startup().Ready());
        CHECK(left.delay==right.delay);
        const auto choice=left.session.Startup().Selected();
        CHECK(choice.delay==left.delay);
        CHECK(delay==AdonisStartup::Automatic?choice.delay+choice.prediction==choice.fullDelay:choice.delay==delay);
        CHECK(left.session.Channel().AdonisStatistics().PredictionAllowanceUs()==
              (choice.prediction*1000000u+59)/60);
        std::printf("  measured B=%u D=%u prediction=%u, input lane samples=%u/%u PASS\n",choice.fullDelay,choice.delay,choice.prediction,
            left.session.Startup().Local().received,right.session.Startup().Local().received);
    }
    CHECK(left.session.Retire(now));CHECK(right.session.Retire(now));return c;
}
void mismatch(AdonisMode aMode,unsigned aDelay,AdonisMode bMode,unsigned bDelay) {
    Link a,b;a.peer=&b;b.peer=&a;RollbackSession left(a),right(b);
    CHECK(left.Begin(config(0),0,aDelay,aMode));CHECK(right.Begin(config(1),0,bDelay,bMode));
    bool failed=false;
    for(unsigned now=0;now<1000&&!failed;++now){a.now=b.now=now;failed=!left.Pump(now)||!right.Pump(now);}
    CHECK(failed);CHECK(!left.Ready()&&!right.Ready());
}
FrameInput heldMotion(unsigned side,unsigned frame){
    FrameInput value;value.buttons=1|(frame%121==0?2:0)|(frame%137==0?8:0);
    const auto phase=(frame/40)%5;
    if(phase<3){value.analogMode=AnalogMode::DirectTouch;value.x=float(20+side*15+phase*8);value.y=80;value.touchUsed=true;value.unlimited=phase==2;}
    else if(phase==3){value.analogMode=AnalogMode::Joystick;value.x=.75f;value.y=-.5f;value.touchUsed=true;}
    return value; // Last phase is a real release, which must be reconciled.
}
void hybridHeldPrediction(){
    for(auto mode:{AdonisMode::Rollback,AdonisMode::Delay,AdonisMode::Hybrid})for(auto analog:{AnalogMode::DirectTouch,AnalogMode::Joystick}){
        Link a,b;a.peer=&b;b.peer=&a;RollbackSession left(a),right(b);
        CHECK(left.Begin(config(0),0,0,mode)&&right.Begin(config(1),0,0,mode));
        for(unsigned now=0;now<1000&&(!left.Ready()||!right.Ready());++now){a.now=b.now=now;CHECK(left.Pump(now)&&right.Pump(now));}
        CHECK(left.Ready()&&right.Ready());
        FrameInput held(1|2|4|8);held.analogMode=analog;held.x=analog==AnalogMode::Joystick?.75f:125.f;held.y=analog==AnalogMode::Joystick?-.5f:210.f;held.touchUsed=true;
        CHECK(left.Capture({},1000)&&right.Capture(held,1000));a.now=b.now=1000;CHECK(left.Pump(1000)&&right.Pump(1000));
        CHECK(left.Complete(left.Prepare()));
        for(unsigned frame=1;frame<4;++frame){
            CHECK(left.Capture({},1000+frame));const auto predicted=left.Prepare();
            if(mode==AdonisMode::Delay){CHECK(!predicted.canAdvance);break;}
            CHECK(predicted.canAdvance&&(predicted.predictedMask&2));
            CHECK(!(predicted.inputs[1].buttons&(2|8))&&!predicted.inputs[1].touchBomb);
            if(mode==AdonisMode::Hybrid){CHECK(predicted.inputs[1].analogMode==analog&&predicted.inputs[1].x==held.x&&predicted.inputs[1].y==held.y);}
            else CHECK(predicted.inputs[1].analogMode==AnalogMode::None);
            CHECK(left.Complete(predicted));
        }
    }
}
int main(){
    runAdonis(AdonisMode::Hybrid,AdonisStartup::Automatic,0,input,true);
    for(auto m:{AdonisMode::Delay,AdonisMode::Hybrid})for(unsigned d:{AdonisStartup::Automatic,0u,1u,9u})runAdonis(m,d,1,input,true);
    runAdonis(AdonisMode::Hybrid,AdonisStartup::Automatic,4,input,true,1);
    runAdonis(AdonisMode::Hybrid,AdonisStartup::Automatic,1,input,true,2,true);
    {
        Link a,b;a.peer=&b;b.peer=&a;a.mode=b.mode=3;RollbackSession l(a),r(b);
        CHECK(l.BeginMeasured(config(0),0,AdonisStartup::Automatic,AdonisMode::Hybrid));
        CHECK(r.BeginMeasured(config(1),0,AdonisStartup::Automatic,AdonisMode::Hybrid));
        bool failed=false;
        for(unsigned t=0;t<11000&&!failed;++t){a.now=b.now=t;failed=!l.Pump(t)||!r.Pump(t);}
        CHECK(failed&&!l.Captures()&&!r.Captures()&&!l.Ready()&&!r.Ready());
    }
    hybridHeldPrediction();
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
    for(unsigned network=1;network<5;++network){
        for(unsigned delay:{0u,2u,9u})runAdonis(AdonisMode::Hybrid,delay,network,heldMotion);
    }
    puts("TH09 Adonis model correctness, once-only input, retirement, mode/delay mismatch and hybrid comparison PASS");
}
