#include "../th08_web/cpp/multiplayer/NetworkFrameSchedule.hpp"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <initializer_list>

namespace {
using Schedule=th08::multiplayer::NetworkFrameSchedule;
constexpr double tick=Schedule::interval;
unsigned checks=0;
void check(bool condition,const char* expression,int line){
    ++checks;
    if(!condition){std::fprintf(stderr,"network frame schedule:%d: %s\n",line,expression);std::exit(1);}
}
#define CHECK(value) check(bool(value),#value,__LINE__)
bool near(double left,double right){return std::abs(left-right)<1.e-9;}

void normal_cadence(){
    Schedule schedule;
    for(unsigned frame=0;frame<120;++frame){
        CHECK(schedule.advance(tick,true)==1);
        schedule.complete();
        CHECK(near(schedule.debt_seconds(),0));
    }
    for(unsigned refresh:{120u,144u}){
        schedule.reset();
        unsigned simulated=0;
        double previous_display=0;
        for(unsigned callback=0;callback<refresh*2;++callback){
            const unsigned due=schedule.advance(1./refresh,true);
            CHECK(due<=1);
            simulated+=due;
            if(due)schedule.complete();
            const double display=simulated+schedule.presentation_alpha();
            CHECK(display+1.e-9>=previous_display);
            previous_display=display;
        }
        CHECK(simulated==120);
        CHECK(near(schedule.debt_seconds(),0));
    }
}

void cpu_debt(){
    Schedule schedule;
    // Four ticks were offered; one completed before the CPU start budget.
    CHECK(schedule.advance(4.25*tick,true)==4);
    schedule.complete();
    schedule.defer(3);
    CHECK(near(schedule.debt_seconds(),3.25*tick));
    CHECK(schedule.presentation_alpha()==1.);
    CHECK(!schedule.retry_pending());
    CHECK(schedule.advance(0,true)==3);
    schedule.complete();
    CHECK(near(schedule.debt_seconds(),.25*tick));
    CHECK(near(schedule.presentation_alpha(),.25));
    CHECK(schedule.advance(.75*tick,true)==1);
    schedule.complete();
    CHECK(near(schedule.debt_seconds(),0));

    schedule.reset();
    CHECK(schedule.advance(60.,true)==Schedule::max_ticks);
    schedule.defer(Schedule::max_ticks-1);
    CHECK(schedule.advance(0,true)==Schedule::max_ticks-1);
    schedule.complete();
    schedule.defer(100);
    CHECK(near(schedule.debt_seconds(),Schedule::max_ticks*tick));
    CHECK(schedule.advance(0,true)==Schedule::max_ticks);
}

void network_wait(){
    Schedule schedule;
    CHECK(schedule.advance(3.375*tick,true)==3);
    schedule.blocked(true);
    CHECK(schedule.retry_pending());
    CHECK(near(schedule.debt_seconds(),.375*tick));
    CHECK(schedule.presentation_alpha()==1.);
    // Each blocked retry remains exactly one frame, including zero elapsed
    // time and a very long network wait. It never earns catch-up work.
    for(double elapsed:{0.,tick/2,1.,60.,0.}){
        CHECK(schedule.advance(elapsed,true)==1);
        CHECK(near(schedule.debt_seconds(),.375*tick));
        CHECK(schedule.presentation_alpha()==1.);
        schedule.blocked(true);
        CHECK(schedule.retry_pending());
    }
    schedule.defer(5); // A pending network wait cannot receive CPU debt.
    CHECK(near(schedule.debt_seconds(),.375*tick));
    CHECK(schedule.advance(0,true)==1);
    schedule.complete();
    CHECK(!schedule.retry_pending());
    CHECK(near(schedule.debt_seconds(),.375*tick));
    CHECK(near(schedule.presentation_alpha(),.375));
    CHECK(schedule.advance(0,true)==0);
    CHECK(schedule.advance(.625*tick,true)==1);
    schedule.complete();
    CHECK(near(schedule.debt_seconds(),0));

    // Discard retained whole CPU ticks once the actual blocker is input.
    CHECK(schedule.advance(2.625*tick,true)==2);
    schedule.defer(2);
    schedule.blocked(true);
    CHECK(near(schedule.debt_seconds(),.625*tick));
    CHECK(schedule.presentation_alpha()==1.);
    CHECK(schedule.advance(0,true)==1);
}

void correction_continuation(){
    Schedule schedule;
    CHECK(schedule.advance(2.25*tick,true)==2);
    // The first offered forward tick is still pending while its history is
    // corrected. Several long recovery callbacks must not create a six-tick
    // prediction burst after that single captured input finally advances.
    schedule.recovering();
    for(unsigned slice=0;slice<120;++slice){
        CHECK(schedule.advance((slice%3+1)*tick,true)==1);
        CHECK(schedule.retry_pending());
        CHECK(near(schedule.debt_seconds(),.25*tick));
        CHECK(schedule.presentation_alpha()==1.);
        schedule.recovering();
        schedule.defer(6);
    }
    CHECK(schedule.advance(0,true)==1);
    schedule.complete();
    CHECK(!schedule.retry_pending());
    CHECK(schedule.advance(0,true)==0);
    CHECK(schedule.advance(.75*tick,true)==1);
    schedule.complete();
    CHECK(near(schedule.debt_seconds(),0));
}

void scaled_time(){
    Schedule schedule;
    // The caller divides each new elapsed sample by its current pace.
    // A fixed 2x interval is 30 logical ticks per wall-clock second.
    unsigned simulated=0;
    for(unsigned callback=0;callback<120;++callback){
        const auto due=schedule.advance((1./120.)/2.,true);
        simulated+=due;
        if(due)schedule.complete();
    }
    CHECK(simulated==30);
    CHECK(near(schedule.debt_seconds(),0));

    schedule.reset();
    CHECK(schedule.advance((.5*tick)/1.,true)==0);
    CHECK(near(schedule.presentation_alpha(),.5));
    // Doubling pace leaves the already accrued half tick intact.
    CHECK(schedule.advance((.25*tick)/2.,true)==0);
    CHECK(near(schedule.presentation_alpha(),.625));
    CHECK(schedule.advance(0./2.,true)==0);
    CHECK(near(schedule.presentation_alpha(),.625));
    CHECK(schedule.advance((.25*tick)/1.,true)==0);
    CHECK(near(schedule.presentation_alpha(),.875));
    CHECK(schedule.advance((.25*tick)/2.,true)==1);
    schedule.complete();
    CHECK(near(schedule.debt_seconds(),0));
}

void reset_and_nonlive(){
    Schedule schedule;
    CHECK(schedule.advance(1.5*tick,true)==1);
    schedule.blocked(true);
    schedule.reset();
    CHECK(!schedule.retry_pending());
    CHECK(near(schedule.debt_seconds(),0));
    CHECK(near(schedule.presentation_alpha(),0));
    CHECK(schedule.advance(0,true)==0);

    CHECK(schedule.advance(1.5*tick,true)==1);
    schedule.blocked(false);
    CHECK(!schedule.retry_pending());
    CHECK(near(schedule.debt_seconds(),0));
    CHECK(schedule.advance(0,false)==0);

    CHECK(schedule.advance(1.5*tick,true)==1);
    schedule.blocked(true);
    CHECK(schedule.advance(0,false)==0);
    CHECK(!schedule.retry_pending());
    CHECK(near(schedule.debt_seconds(),.5*tick));
    CHECK(schedule.advance(.5*tick,false)==1);
}
}
int main(){
    normal_cadence();cpu_debt();network_wait();correction_continuation();scaled_time();reset_and_nonlive();
    std::printf("TH08 network frame schedule: %u checks PASS\n",checks);
}

