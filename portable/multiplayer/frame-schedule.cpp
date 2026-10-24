// Real TH11 simulation cadence/common PendingFrameSchedule oracle, no wall-clock benchmark.
#include "../../web/cpp/multiplayer/FrameSchedule.hpp"
#include "../sdl/FrameCadence.hpp"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <limits>
namespace {
using Clock=th11::multiplayer::FrameSchedule;
constexpr double step=Clock::interval;
unsigned groups=0,checks=0;
void check(bool value,const char* why){++checks;if(!value){std::fprintf(stderr,"FAIL schedule group %u check %u: %s\n",groups,checks,why);std::exit(1);}}
void near(double actual,double expected,const char* why){check(std::abs(actual-expected)<1.e-8,why);}
void group(const char* name){++groups;std::printf("GROUP %u %s\n",groups,name);}
void consume(Clock& clock,unsigned count){for(unsigned n=0;n<count;++n)check(clock.complete(),"consume exactly one newly completed forward");check(clock.owed()==0,"all offered forward obligations consumed");}
void ordinary_equivalence(){
    {touhou::sdl::FrameCadence display;th11::multiplayer::SimulationCadence simulation;
     check(display.advance(3*step)==1&&simulation.advance(3*step)==3,"display deadlines do not change network catch-up");}
    group("unchanged original cadence when every offered tick completes at high refresh rates");
    for(unsigned hz:{60u,90u,120u,144u,165u,240u}){
        Clock clock;th11::multiplayer::SimulationCadence original;unsigned count=0;
        for(unsigned callback=0;callback<5*hz;++callback){
            const double elapsed=1./hz;const auto due=clock.advance(elapsed);
            check(due==original.advance(elapsed),"same due count as independent simulation helper");
            check(due<=4&&clock.owed()==due,"same four-tick cap and offered ledger");
            near(clock.phase(),original.debt,"exact nominal phase preserved");
            consume(clock,due);count+=due;check(!clock.retry_pending(),"ordinary progress creates no retry");
        }
        check(count==300,"five nominal seconds complete 300 logical ticks");
    }
    Clock clock;th11::multiplayer::SimulationCadence original;
    for(double elapsed:{0.,.001,.009,.03,.052,.2,0.,.004,.05}){
        const auto due=clock.advance(elapsed);check(due==original.advance(elapsed),"irregular finite cadence retains the network cap");
        near(clock.phase(),original.debt,"irregular phase matches original");consume(clock,due);
    }
}
void cpu_refund(){
    group("CPU return once, preserve fractional phase, and never consume history as forward");
    Clock clock;check(clock.advance(3.75*step)==3,"three offered forward ticks");near(clock.phase(),.75*step,"initial phase");
    check(clock.complete(),"first forward complete");check(clock.owed()==2,"two forward remain");
    clock.cpu_yield();check(clock.owed()==0&&!clock.retry_pending(),"CPU settlement closes current offer");
    near(clock.phase(),2.75*step,"all uncompleted ticks plus exact old fraction retained");
    clock.cpu_yield();near(clock.phase(),2.75*step,"duplicate CPU settlement is a no-op");
    check(!clock.complete(),"already settled offer cannot consume phantom forward");
    check(clock.advance(0)==2,"owed ticks retry with no new wall-time credit");consume(clock,2);
    near(clock.phase(),.75*step,"completing an expensive callback preserves fraction");
    check(clock.advance(.25*step)==1,"remaining quarter phase completes next due tick");consume(clock,1);near(clock.phase(),0,"no duplicate phase");
    check(clock.advance(2.5*step)==2,"two offered ticks before history restoration");
    // Historical work is performed by driver without complete(). Simulate two
    // whole history transactions here, then yielding before any new forward.
    check(clock.owed()==2,"historical transactions do not own new forward debt");
    clock.cpu_yield();near(clock.phase(),2.5*step,"CPU restoration yield returns every offered forward tick");
    check(clock.advance(.5*step)==3,"next callback accumulates only newly elapsed CPU time");consume(clock,3);
}
void exact_loop_ledger(){
    group("exact playback and task continuation spend only previously offered CPU debt");
    Clock fixed;th11::multiplayer::SimulationCadence old;unsigned before=0,after=0;
    for(unsigned cycle=0;cycle<100;++cycle){
        check(old.advance(3*step)==3&&fixed.advance(3*step)==3,"same delayed three-tick offer");
        ++before;++after;check(fixed.complete(),"one real forward frame completed");fixed.cpu_yield();
        before+=old.advance(0);const auto debt=fixed.advance(0);
        check(debt==2,"a task retry receives exactly the two unstarted frames");
        consume(fixed,debt);after+=debt;
        check(fixed.advance(0)==0,"a second zero-elapsed task cannot create work");
    }
    check(before==100&&after==300,"unconditional deduction loses 200 real logical frames");
    fixed.advance(3*step);fixed.reset();fixed.cpu_yield();
    check(fixed.advance(0)==0,"replacing the generation cancels its pending offer");
}
void network_wait(){
    group("actual information wait retains one due tick and never accrues waiting wall time");
    Clock clock;check(clock.advance(3.75*step)==3,"multiple offered ticks before actual input gap");
    check(clock.complete(),"one forward precedes gap");clock.network_wait();
    check(clock.retry_pending()&&clock.owed()==0,"network hold closes offered ledger and owns one retry");near(clock.phase(),.75*step,"network hold keeps fractional phase");
    clock.cpu_yield();near(clock.phase(),.75*step,"tail CPU settlement cannot duplicate network obligation");
    for(double elapsed:{0.,.001,.5,10.,100.,0.}){
        check(clock.advance(elapsed)==1&&clock.owed()==1,"network retry is immediate and singular");
        near(clock.phase(),.75*step,"waiting time discarded");
        clock.network_wait();clock.network_wait();clock.cpu_yield();
        near(clock.phase(),.75*step,"repeated settlement preserves one pending and original phase");
    }
    check(clock.advance(100)==1,"arrival callback does not backfill previous network stall");consume(clock,1);
    check(!clock.retry_pending()&&clock.advance(0)==0,"completed retry never becomes duplicate forward");
    near(clock.phase(),.75*step,"retry completion preserves original fractional remainder");
    check(clock.advance(.25*step)==1,"ordinary cadence resumes from saved phase");consume(clock,1);
    clock.network_wait();check(!clock.retry_pending(),"no due obligation cannot invent network pending");
    check(clock.advance(100)==4,"long interruption bounded by old cap");clock.network_wait();
    near(clock.phase(),0,"actual network wait drops whole residual catch-up debt");check(clock.advance(100)==1,"only one actual network retry remains");consume(clock,1);
}
void transitions(){
    group("network to CPU recovery never restores discarded network time");
    Clock clock;check(clock.advance(1.25*step)==1,"one due plus quarter phase");clock.network_wait();
    for(double elapsed:{.5,10.,100.}){check(clock.advance(elapsed)==1,"single missing input retry");clock.network_wait();}
    check(clock.advance(100)==1,"late packet callback ignores prior wait time");
    clock.cpu_yield();check(!clock.retry_pending(),"late correction switches to CPU ownership");
    near(clock.phase(),1.25*step,"only one pending obligation converted to bounded CPU debt");
    clock.cpu_yield();near(clock.phase(),1.25*step,"no duplicate conversion in tail");
    check(clock.advance(.75*step)==2,"new CPU time accrues after the transition");consume(clock,2);near(clock.phase(),0,"no invented network debt");
    check(clock.advance(2.5*step)==2,"CPU work before a later network gap");clock.cpu_yield();
    check(clock.advance(.5*step)==3,"three new offered after CPU resume");check(clock.complete(),"first completes");clock.network_wait();
    check(clock.advance(100)==1,"CPU to actual network stops accumulation again");consume(clock,1);check(clock.advance(0)==0,"dropped extra network debt does not reappear");
}
void bounded_and_defensive(){
    group("final history transaction has a bounded completion opportunity, never new forward credit");
    for(unsigned completed=0;completed<12;++completed)for(unsigned remaining=1;remaining<=12;++remaining){
        check(Clock::can_replay_next(completed,remaining,7.999),"old 8ms gate retains complete-tick admission");
        check(Clock::can_replay_next(completed,remaining,8.,true)==(!completed||remaining==1),"only final history tick may start at 8ms");
        check(Clock::can_replay_next(completed,remaining,11.999,true)==(!completed||remaining==1),"tail opportunity ends before 12ms");
        check(Clock::can_replay_next(completed,remaining,8.)==!completed,"unproven tail opportunity is disabled by default");
        check(Clock::can_replay_next(completed,remaining,11.999)==!completed,"default cannot silently enable tail gate");
        check(Clock::can_replay_next(completed,remaining,12.)==!completed,"12ms is an admission boundary, not a reset clock");
        check(Clock::can_replay_next(completed,remaining,100.)==!completed,"first complete transaction preserved without admitting a second");
        check(!Clock::can_replay_next(completed,0,0.),"no historical work when already complete");
    }
    group("bounded long stalls, defensive unfinished offers and invalid elapsed");
    Clock clock;
    for(unsigned i=0;i<50;++i){
        check(clock.advance(100)==4,"every long CPU interruption keeps four-tick offer cap");
        check(clock.complete(),"one forward progresses");clock.cpu_yield();
        check(clock.phase()<=Clock::max_debt+1.e-9&&clock.phase()>=0,"returned CPU debt remains at most 100 ms");
    }
    clock.reset();check(clock.advance(100)==4,"100 ms retained before deduction");near(clock.phase(),2*step,"two more ticks remain after offering four");
    clock.cpu_yield();near(clock.phase(),6*step,"all six bounded ticks retained, not unbounded stall time");
    check(clock.advance(0)==4,"first catchup batch of four");consume(clock,4);
    check(clock.advance(0)==2,"second bounded catchup batch of two");consume(clock,2);check(clock.advance(0)==0,"no second hidden backlog");
    check(clock.advance(2.25*step)==2&&clock.complete(),"one unclosed forward remains");
    check(clock.advance(.75*step)==2,"next callback defensively returns omitted CPU offer once");consume(clock,2);near(clock.phase(),0,"defensive return preserves phase exactly");
    check(clock.advance(1.5*step)==1,"retry setup");clock.network_wait();check(clock.advance(100)==1,"offer retry");
    check(clock.advance(100)==1&&clock.retry_pending(),"unsettled known retry remains network wait");
    near(clock.phase(),.5*step,"defensive retry does not turn elapsed into CPU credit");consume(clock,1);
    clock.reset();
    for(double elapsed:{-1.,std::numeric_limits<double>::infinity(),-std::numeric_limits<double>::infinity(),std::numeric_limits<double>::quiet_NaN()}){
        check(clock.advance(elapsed)==0,"invalid elapsed contributes zero credit");near(clock.phase(),0,"invalid elapsed cannot poison phase");
    }
}
void reset(){
    group("generation reset clears offered, retry and fractional credit");
    Clock clock;check(clock.advance(2.75*step)==2,"nonempty offer before reset");clock.reset();
    check(clock.owed()==0&&!clock.retry_pending()&&!clock.complete(),"reset clears current offer");
    near(clock.phase(),0,"reset phase");check(clock.advance(0)==0,"no previous generation credit");
    check(clock.advance(1.25*step)==1,"network reset fixture");clock.network_wait();clock.reset();
    check(!clock.retry_pending()&&clock.advance(0)==0,"reset clears network pending");near(clock.phase(),0,"reset clears wait phase");
    check(clock.advance(1.25*step)==1,"CPU reset fixture");clock.cpu_yield();clock.reset();
    check(clock.advance(0)==0,"reset clears CPU owed debt");
}
void budget_and_forward(){
    group("8 ms additional-start gate preserves first forward after complete historical recovery");
    check(Clock::max_ticks==4&&Clock::start_budget_ns==8000000,"original four-tick and 8 ms constants");
    check(Clock::can_start(0,0)&&Clock::can_start(0,8000000)&&Clock::can_start(0,30000000),"already-due first forward always gets one attempt");
    check(Clock::can_start(1,7999999)&&!Clock::can_start(1,8000000),"8 ms is a strict additional-start boundary");
    check(Clock::can_start(3,0)&&!Clock::can_start(4,0),"clock cannot adopt common six-tick cap");
    Clock clock;check(clock.advance(3.75*step)==3,"three due forward before restoration");
    check(clock.owed()==3,"whole restored history leaves all new-forward obligations");
    unsigned completed_forward=0;
    check(Clock::can_start(completed_forward,9000000),"finished history cannot consume first-forward admission");
    check(clock.complete(),"driver attempts owed forward after finishing recovery");++completed_forward;
    check(!Clock::can_start(completed_forward,10000000),"budget prevents launching a second new forward");
    clock.cpu_yield();near(clock.phase(),2.75*step,"tail returns exactly remaining forward obligations");
    check(clock.advance(0)==2,"next callback owes exactly two");consume(clock,2);
}
void sustained_cpu(){
    group("deterministic high-refresh CPU yields preserve total logical progress");
    for(unsigned hz:{60u,90u,120u,144u,165u,240u}){
        Clock clock;unsigned completed=0,yields=0;bool resuming=false;
        for(unsigned callback=1;callback<=5*hz;++callback){
            const auto due=clock.advance(1./hz);if(!due)continue;
            if(!resuming&&callback%7==0){clock.cpu_yield();++yields;resuming=true;}
            else{for(unsigned tick=0;tick<due;++tick){check(clock.complete(),"sustained forward consumes one due");++completed;}resuming=false;}
        }
        clock.cpu_yield();
        for(unsigned pass=0;pass<3;++pass){const auto due=clock.advance(0);for(unsigned n=0;n<due;++n){check(clock.complete(),"drain bounded remaining forward");++completed;}}
        check(yields>0&&completed==300,"CPU yield changes callback placement without silently slowing nominal 60 Hz");
        check(clock.owed()==0&&!clock.retry_pending(),"drained CPU sequence has no synthetic network wait");near(clock.phase(),0,"full five-second credit accounted once");
    }
}
}
int main(){
    check(Clock::can_start(1,15'999'999,true),"owed second forward can amortize render under 16ms");
    check(!Clock::can_start(1,16'000'000,true),"forward pair has an exact bounded admission deadline");
    check(!Clock::can_start(2,8'000'000,true),"no third expensive forward admitted by pair option");
    check(!Clock::can_start(1,8'000'000,false),"post-history callback retains original forward admission");
    ordinary_equivalence();cpu_refund();exact_loop_ledger();network_wait();transitions();bounded_and_defensive();reset();budget_and_forward();sustained_cpu();
    std::printf("PASS TH11 frame schedule: %u groups, %u checks\n",groups,checks);return 0;
}
