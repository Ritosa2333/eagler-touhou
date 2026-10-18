#include "../web/cpp/sdl/RafDeadlineGuard.hpp"
#include "../web/cpp/multiplayer/NetworkFrameSchedule.hpp"
#include "sdl/PresentationCadence.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <initializer_list>
#include <limits>

using Guard=th10::host::RafDeadlineGuard;
namespace {
unsigned cases=0,checks=0,failures=0;
const char* current_case="";
const char* case_names[32]{};
void Begin(const char* name){current_case=name;case_names[cases++]=name;}
void Check(bool value,const char* expression,int line){
    ++checks;
    if(!value){++failures;std::fprintf(stderr,"%s:%d: %s\n",current_case,line,expression);}
}
#define CHECK(value) Check(bool(value),#value,__LINE__)
bool Near(double a,double b){return std::fabs(a-b)<1.e-8;}
double Status(const Guard& guard,Guard::StatusField field){
    double out[Guard::StatusCount]{};guard.Status(out);return out[field];
}
Guard::Token Prime(Guard& guard){
    CHECK(guard.Configure(true)==1);guard.Start(1);
    const auto first=guard.Request(99.,true);
    CHECK(first!=0);CHECK(!guard.timer_armed()); // No cold-start compensation.
    const auto frame=guard.Raf(first,100.,100.);
    CHECK(frame.service);CHECK(frame.sampleRaf);CHECK(Near(frame.realRafDeltaMs,0.));
    CHECK(guard.Complete(frame.ticket,103.2));
    const auto request=guard.Request(103.8,true);
    CHECK(request!=0);CHECK(guard.timer_armed());CHECK(Near(guard.deadline_ms(),128.2));
    return request;
}
// Exercise the actual production debt helper with the host's accepted time.
// This is not a browser/SDL test and does not claim physical presentation.
struct Clock {
    th10::multiplayer::NetworkFrameSchedule schedule;
    double last_seconds=-1,credited_seconds=0,audio_remainder_ms=0;
    unsigned audio_ms=0,steps=0,last_offered=0,last_completed=0;
    void Pause(){last_seconds=-1;schedule.reset();}
    void RejectReadiness(const Guard::Dispatch& dispatch){
        CHECK(dispatch.service);
        last_seconds=dispatch.acceptedTimestampMs/1000.;schedule.reset();
    }
    void Consume(const Guard::Dispatch& dispatch,bool inputReady=true,
                 unsigned completionLimit=6,double scale=1.){
        if(!dispatch.service)return;
        const double now=dispatch.acceptedTimestampMs/1000.;
        CHECK(last_seconds<0||now>=last_seconds);
        const double delta=last_seconds<0?0:std::max(0.,now-last_seconds);
        last_seconds=now;credited_seconds+=delta;audio_remainder_ms+=delta*1000.;
        const auto milliseconds=unsigned(std::floor(audio_remainder_ms));
        audio_remainder_ms-=milliseconds;audio_ms+=milliseconds;
        last_offered=schedule.advance(delta/scale,true);last_completed=0;
        bool blocked=false;
        while(last_completed<last_offered&&last_completed<completionLimit){
            if(!inputReady){schedule.blocked(true);blocked=true;break;}
            schedule.complete();++last_completed;++steps;
        }
        if(!blocked&&last_completed<last_offered)schedule.defer(last_offered-last_completed);
    }
};
void Sample(touhou::sdl::PresentationCadence& cadence,const Guard::Dispatch& dispatch){
    if(dispatch.sampleRaf)cadence.advance(dispatch.realRafDeltaMs/1000.);
}
bool DisplayEligible(const Guard& guard,const touhou::sdl::PresentationCadence& cadence){
    return cadence.high_refresh&&touhou::sdl::PresentationCadence::fast_sample(
        guard.last_real_raf_delta_ms()/1000.);
}

void DefaultAndLock(){
    Begin("default-off-and-module-lifetime-lock");
    Guard guard;
    CHECK(Status(guard,Guard::Version)==2);
    CHECK(Status(guard,Guard::Enabled)==0);CHECK(Status(guard,Guard::Locked)==0);
    CHECK(guard.Configure(false)==0);guard.Start(1);
    CHECK(guard.Request(0,true)==0);CHECK(guard.Configure(true)==-1);
    guard.Stop(2);CHECK(guard.Configure(true)==-1);
    CHECK(Status(guard,Guard::Running)==0);CHECK(Status(guard,Guard::Locked)==1);
    guard.Start(3);CHECK(guard.Request(10,true)==0);
    CHECK(Status(guard,Guard::RafRequests)==0);
    Guard fresh;CHECK(fresh.Configure(true)==1);fresh.Start(1);
    fresh.Stop(2);CHECK(fresh.Configure(false)==-1);
}
void NormalCadence(){
    Begin("normal-real-raf-cadence-and-no-timer-high-refresh");
    for(const unsigned hz:{120u,60u,30u,15u}){
        Guard guard;CHECK(guard.Configure(true)==1);guard.Start(1);
        touhou::sdl::PresentationCadence display;Clock clock;
        auto request=guard.Request(0,false);
        constexpr unsigned count=120;
        for(unsigned i=0;i<count;++i){
            const double timestamp=10.+double(i)*1000./hz;
            const auto frame=guard.Raf(request,timestamp,timestamp+.5);
            CHECK(frame.service);Sample(display,frame);clock.Consume(frame);
            CHECK(guard.Complete(frame.ticket,timestamp+2.));
            request=guard.Request(timestamp+2.1,DisplayEligible(guard,display));
            CHECK(request!=0);
            if(hz<=60)CHECK(!guard.timer_armed());
        }
        CHECK(display.high_refresh==(hz==120));
        CHECK(Near(clock.credited_seconds,double(count-1)/hz));
        CHECK(Near(clock.audio_ms+clock.audio_remainder_ms,clock.credited_seconds*1000.));
        CHECK(Status(guard,Guard::TimerServices)==0);
        CHECK(Status(guard,Guard::RafServices)==count);
    }
}
void DelayedRaf(){
    Begin("raf-delay-with-timer-dispatch-opportunity");
    Guard guard;const auto request=Prime(guard);Clock clock;
    // Establish the same initial native clock as Prime's completed RAF.
    Guard::Dispatch initial;initial.service=true;initial.acceptedTimestampMs=100.;
    clock.Consume(initial);
    const auto timer=guard.Timer(guard.timer_serial(),132.8,true);
    CHECK(timer.service);CHECK(!timer.sampleRaf);CHECK(guard.pending_raf());
    clock.Consume(timer);CHECK(guard.Complete(timer.ticket,142.9));
    CHECK(!guard.timer_armed());CHECK(guard.request_serial()==request);
    const auto raf=guard.Raf(request,154.1,154.593);
    CHECK(raf.service);CHECK(raf.sampleRaf);clock.Consume(raf);
    CHECK(Near(raf.realRafDeltaMs,54.1));
    CHECK(guard.Complete(raf.ticket,164.7));
    CHECK(Near(clock.credited_seconds,.054593));
    CHECK(Near(clock.audio_ms+clock.audio_remainder_ms,54.593));
    CHECK(Status(guard,Guard::TimerServices)==1);
    CHECK(Near(Status(guard,Guard::LastTimerLatenessMs),4.6));
}
void NoImplicitRearm(){
    Begin("completed-timer-needs-explicit-host-rearm");
    Guard guard;Prime(guard);
    const auto timer=guard.Timer(guard.timer_serial(),128.2,true);
    CHECK(timer.service);CHECK(guard.Complete(timer.ticket,138.3));
    const auto before=Status(guard,Guard::CallbackSequence);
    for(unsigned i=0;i<20;++i){
        CHECK(!guard.Timer(guard.timer_serial(),200.+100.*i,true).service);
        CHECK(guard.Request(200.+100.*i,true)==0);
    }
    CHECK(guard.pending_raf());CHECK(!guard.timer_armed());
    CHECK(Status(guard,Guard::TimerServices)==1);
    CHECK(Status(guard,Guard::CallbackSequence)==before);
}
void BothArrivalOrders(){
    Begin("raf-and-timer-arrival-order-and-four-ms-boundary");
    {
        Guard guard;const auto request=Prime(guard);
        const auto raf=guard.Raf(request,128.,128.2);
        CHECK(raf.service);CHECK(!guard.timer_armed());
        CHECK(!guard.Timer(guard.timer_serial(),128.3,true).service);
        CHECK(guard.Complete(raf.ticket,138.));
        CHECK(Status(guard,Guard::TimerServices)==0);
    }
    {
        Guard guard;const auto request=Prime(guard);
        const auto timer=guard.Timer(guard.timer_serial(),128.2,true);
        CHECK(timer.service);CHECK(guard.Complete(timer.ticket,138.3));
        const auto near=guard.Raf(request,139.,139.2);
        CHECK(near.current);CHECK(!near.service);CHECK(near.sampleRaf);
        CHECK(Status(guard,Guard::NearSkips)==1);
        CHECK(Near(Status(guard,Guard::LastAcceptedTimestampMs),128.2));
        const auto next=guard.Request(139.4,false);
        const auto raf=guard.Raf(next,147.,147.2);
        CHECK(raf.service);CHECK(guard.Complete(raf.ticket,149.));
        CHECK(Near(Status(guard,Guard::LastAcceptedTimestampMs),147.2));
    }
    {
        Guard guard;const auto request=Prime(guard);
        const auto timer=guard.Timer(guard.timer_serial(),128.2,true);
        CHECK(guard.Complete(timer.ticket,138.)); // Exact binary boundary.
        const auto raf=guard.Raf(request,141.5,142.);
        CHECK(raf.service);CHECK(Status(guard,Guard::NearSkips)==0);
        CHECK(guard.Complete(raf.ticket,144.));
    }
}
void BackwardTimestamps(){
    Begin("timer-covered-and-backward-raf-timestamps");
    Guard guard;const auto request=Prime(guard);Clock clock;
    Guard::Dispatch initial;initial.service=true;initial.acceptedTimestampMs=100.;
    clock.Consume(initial);
    const auto timer=guard.Timer(guard.timer_serial(),128.2,true);
    clock.Consume(timer);CHECK(guard.Complete(timer.ticket,138.3));
    const auto covered=guard.Raf(request,120.,139.);
    CHECK(!covered.service);CHECK(covered.sampleRaf);
    CHECK(Near(covered.realRafDeltaMs,20.));
    CHECK(Near(Status(guard,Guard::LastAcceptedTimestampMs),128.2));
    auto next=guard.Request(139.2,false);
    const auto backward=guard.Raf(next,119.,144.);
    CHECK(!backward.service);CHECK(Near(backward.realRafDeltaMs,0.));
    CHECK(Near(Status(guard,Guard::LastAcceptedTimestampMs),128.2));
    next=guard.Request(144.2,false);
    const auto resumed=guard.Raf(next,146.,146.5);
    CHECK(resumed.service);CHECK(Near(resumed.realRafDeltaMs,26.));
    clock.Consume(resumed);CHECK(guard.Complete(resumed.ticket,149.));
    CHECK(Near(clock.credited_seconds,.0465));
    CHECK(Status(guard,Guard::TimestampSkips)==2);
    CHECK(Status(guard,Guard::SkippedRafSamples)==2);
}
void RetiredTimeout(){
    Begin("already-queued-timeout-after-stop-and-restart");
    Guard guard;const auto old=Prime(guard),old_timer=guard.timer_serial();guard.Stop(2);
    CHECK(!guard.Timer(old_timer,200.,true).service);
    CHECK(!guard.Raf(old,199.,200.).service);
    guard.Start(3);const auto replacement=guard.Request(300.,false);
    CHECK(replacement!=old);CHECK(guard.pending_raf());
    CHECK(!guard.Timer(old_timer,301.,true).service);
    CHECK(guard.request_serial()==replacement);CHECK(guard.pending_raf());
    CHECK(!guard.Raf(old,302.,302.5).service);
    CHECK(guard.request_serial()==replacement);CHECK(guard.pending_raf());
    const auto raf=guard.Raf(replacement,303.,303.5);
    CHECK(raf.service);CHECK(Near(raf.realRafDeltaMs,0.));
    CHECK(guard.Complete(raf.ticket,305.));
    const auto following=guard.Request(305.2,true),fresh_timer=guard.timer_serial();
    CHECK(following!=0);CHECK(fresh_timer!=old_timer);CHECK(guard.timer_current(fresh_timer));
    CHECK(!guard.Timer(old_timer,340.,true).service);
    CHECK(guard.timer_current(fresh_timer));
}
void RestartInsideCallback(){
    Begin("stop-restart-inside-prepare-or-finish-keeps-new-request");
    for(const bool timerSource:{false,true}){
        Guard guard;const auto old=Prime(guard),old_timer=guard.timer_serial();
        const auto inFlight=timerSource?guard.Timer(old_timer,130.,true):guard.Raf(old,130.,130.2);
        CHECK(inFlight.service);CHECK(guard.callback_active());
        // Models either runtimePrepare or runtimeFinish replacing the loop.
        guard.Stop(2);guard.Start(3);
        const auto replacement=guard.Request(140.,false);
        const auto sequence=Status(guard,Guard::CallbackSequence);
        CHECK(!guard.Complete(inFlight.ticket,145.));
        CHECK(guard.Rearm(inFlight.ticket,true)==0);
        CHECK(guard.request_serial()==replacement);CHECK(guard.pending_raf());
        CHECK(!guard.callback_active());CHECK(Status(guard,Guard::CallbackSequence)==sequence);
        const auto fresh=guard.Raf(replacement,150.,150.2);
        CHECK(fresh.service);CHECK(guard.callback_active());
        CHECK(!guard.Complete(inFlight.ticket,151.));
        CHECK(guard.callback_active());CHECK(guard.Complete(fresh.ticket,153.));
        const auto second=guard.Request(153.2,true);
        CHECK(second!=0);CHECK(guard.timer_armed());
        const auto replacement_timer=guard.timer_serial();
        CHECK(guard.Rearm(inFlight.ticket,true)==0);
        CHECK(!guard.Timer(old_timer,180.,true).service);
        CHECK(guard.timer_current(replacement_timer));
    }
}
void PauseAndVisibility(){
    Begin("visibility-loss-normal-pause-and-resume-reanchors-time");
    Guard guard;const auto request=Prime(guard);Clock clock;
    Guard::Dispatch initial;initial.service=true;initial.acceptedTimestampMs=100.;
    clock.Consume(initial);
    // Dispatch-time eligibility changes after arming (visibility/loading/etc).
    const auto hidden=guard.Timer(guard.timer_serial(),130.,false);
    CHECK(hidden.current);CHECK(!hidden.service);CHECK(guard.pending_raf());
    CHECK(Status(guard,Guard::EligibilitySkips)==1);
    guard.ResetAfterPause();clock.Pause();
    CHECK(!guard.timer_armed());
    CHECK(!guard.Timer(guard.timer_serial(),140.,true).service); // Already queued old timer.
    guard.ResetAfterPause();clock.Pause(); // The resume path resets too.
    const auto resumed=guard.Raf(request,100000.,100000.2);
    CHECK(resumed.service);CHECK(Near(resumed.realRafDeltaMs,0.));
    clock.Consume(resumed);CHECK(Near(clock.credited_seconds,0.));
    CHECK(guard.Complete(resumed.ticket,100002.));
    const auto next=guard.Request(100002.2,false);
    CHECK(next!=0);CHECK(!guard.timer_armed());
    const auto later=guard.Raf(next,100008.,100008.2);
    clock.Consume(later);CHECK(guard.Complete(later.ticket,100010.));
    CHECK(Near(clock.credited_seconds,.008));
    CHECK(Status(guard,Guard::TimerServices)==0);
}
void IneligibleAndEarly(){
    Begin("ineligible-early-and-invalid-timeout-is-single-use");
    {
        Guard guard;Prime(guard);
        CHECK(!guard.Timer(guard.timer_serial(),127.2,true).service);
        CHECK(Status(guard,Guard::LastDecision)==unsigned(Guard::Decision::EarlyTimer));
        CHECK(!guard.Timer(guard.timer_serial(),128.2,true).service);
        CHECK(guard.pending_raf());CHECK(!guard.timer_armed());
    }
    {
        Guard guard;Prime(guard);
        CHECK(!guard.Timer(guard.timer_serial(),std::numeric_limits<double>::quiet_NaN(),true).service);
        CHECK(Status(guard,Guard::LastDecision)==unsigned(Guard::Decision::InvalidTime));
        double status[Guard::StatusCount]{};guard.Status(status);
        for(const auto field:status)CHECK(std::isfinite(field));
        CHECK(!guard.Timer(guard.timer_serial(),130.,true).service);
    }
    {
        Guard guard;const auto request=Prime(guard);
        const auto invalid=guard.Raf(request,std::numeric_limits<double>::infinity(),130.);
        CHECK(invalid.current);CHECK(!invalid.sampleRaf);CHECK(!invalid.service);
        CHECK(Near(Status(guard,Guard::LastAcceptedTimestampMs),100.));
        const auto next=guard.Request(131.,false);
        CHECK(next!=0);const auto valid=guard.Raf(next,132.,132.2);
        CHECK(valid.service);CHECK(guard.Complete(valid.ticket,134.));
    }
}
void QuantizedTimeoutAdapter(){
    Begin("fractional-timeout-delay-rounds-up-before-dom-adapter");
    Guard guard;CHECK(guard.Configure(true)==1);guard.Start(1);
    auto request=guard.Request(98.,false);
    const auto raf=guard.Raf(request,99.,99.2);
    CHECK(raf.service);CHECK(guard.Complete(raf.ticket,100.2));
    request=guard.Request(100.7,true);
    CHECK(Near(guard.deadline_ms(),125.2));
    const double registeredAt=100.7;
    const double unrounded=guard.deadline_ms()-registeredAt;
    // Reproduce integer truncation at the browser binding. The former direct
    // 24.5 ms argument would consume the current arm at 124.7, before 125.2.
    CHECK(registeredAt+std::floor(unrounded)<guard.deadline_ms());
    const auto delay=guard.timeout_delay_ms(registeredAt);
    CHECK(Near(delay,25.));CHECK(std::floor(delay)==delay);
    CHECK(registeredAt+delay>=guard.deadline_ms());
    const auto timer=guard.Timer(guard.timer_serial(),registeredAt+std::floor(delay),true);
    CHECK(timer.service);CHECK(guard.Complete(timer.ticket,129.));
    CHECK(!guard.Timer(guard.timer_serial(),160.,true).service);CHECK(guard.pending_raf());
    CHECK(Near(guard.timeout_delay_ms(200.),0.));
}
void TimerCannotSampleDisplay(){
    Begin("skipped-real-raf-sampled-timer-never-samples-detector");
    Guard guard;CHECK(guard.Configure(true)==1);guard.Start(1);
    touhou::sdl::PresentationCadence display;
    auto request=guard.Request(99.,false);
    double lastTimestamp=100.;
    for(unsigned i=0;i<7;++i){
        lastTimestamp=100.+double(i)*1000./120.;
        const auto raf=guard.Raf(request,lastTimestamp,lastTimestamp+.5);
        CHECK(raf.service);Sample(display,raf);
        CHECK(guard.Complete(raf.ticket,lastTimestamp+3.2));
        request=guard.Request(lastTimestamp+3.8,DisplayEligible(guard,display));
    }
    CHECK(display.high_refresh);CHECK(guard.timer_armed());
    const auto fastBefore=display.fast,slowBefore=display.slow;
    const double fired=guard.deadline_ms();
    const auto timer=guard.Timer(guard.timer_serial(),fired,true);CHECK(timer.service);
    Sample(display,timer); // Intentionally pass every dispatch to the sampler.
    CHECK(display.fast==fastBefore);CHECK(display.slow==slowBefore);
    CHECK(guard.Complete(timer.ticket,fired+10.1));
    lastTimestamp=fired+11.;
    const auto near=guard.Raf(request,lastTimestamp,lastTimestamp+.2);
    CHECK(!near.service);CHECK(near.sampleRaf);Sample(display,near);
    CHECK(display.high_refresh);CHECK(display.slow==1);
    CHECK(!DisplayEligible(guard,display));
    request=guard.Request(lastTimestamp+.5,DisplayEligible(guard,display));
    CHECK(!guard.timer_armed());
    for(unsigned i=0;i<11;++i){
        lastTimestamp+=1000./30.;
        const auto slow=guard.Raf(request,lastTimestamp,lastTimestamp+.2);
        CHECK(slow.service);Sample(display,slow);
        CHECK(guard.Complete(slow.ticket,lastTimestamp+2.));
        request=guard.Request(lastTimestamp+2.2,DisplayEligible(guard,display));
        CHECK(!guard.timer_armed());
    }
    CHECK(!display.high_refresh);
}
void NaturalInputWait(){
    Begin("peer-input-wait-stays-pending-and-does-not-catch-up-wait-time");
    Guard guard;const auto request=Prime(guard);Clock clock;
    Guard::Dispatch initial;initial.service=true;initial.acceptedTimestampMs=100.;
    clock.Consume(initial);
    CHECK(clock.schedule.advance(1./60.,true)==1);
    clock.schedule.blocked(true);
    const auto timer=guard.Timer(guard.timer_serial(),130.,true);
    clock.Consume(timer,false);CHECK(guard.Complete(timer.ticket,132.));
    CHECK(clock.last_offered==1);CHECK(clock.steps==0);CHECK(clock.schedule.retry_pending());
    const auto raf=guard.Raf(request,150.,150.2);
    clock.Consume(raf,false);CHECK(guard.Complete(raf.ticket,152.));
    CHECK(clock.last_offered==1);CHECK(clock.steps==0);CHECK(clock.schedule.retry_pending());
    const auto next=guard.Request(152.2,false);
    const auto repair=guard.Raf(next,158.,158.2);
    clock.Consume(repair,true);CHECK(guard.Complete(repair.ticket,160.));
    CHECK(clock.steps==1);CHECK(clock.last_offered==1);
    CHECK(!clock.schedule.retry_pending());CHECK(Near(clock.schedule.debt_seconds(),0.));
}
void CpuDebtSurvives(){
    Begin("timer-opportunity-preserves-unfinished-cpu-debt");
    Guard guard;const auto request=Prime(guard);Clock clock;
    Guard::Dispatch initial;initial.service=true;initial.acceptedTimestampMs=100.;
    clock.Consume(initial);
    const auto timer=guard.Timer(guard.timer_serial(),150.,true);
    clock.Consume(timer,true,1);CHECK(guard.Complete(timer.ticket,162.));
    CHECK(clock.last_offered==3);CHECK(clock.last_completed==1);
    CHECK(Near(clock.schedule.debt_seconds(),2./60.));
    const auto near=guard.Raf(request,161.,163.);
    CHECK(!near.service);CHECK(near.sampleRaf);
    CHECK(Near(clock.schedule.debt_seconds(),2./60.));
    const auto next=guard.Request(163.2,false);
    const auto raf=guard.Raf(next,170.,170.2);
    clock.Consume(raf,true,1);CHECK(guard.Complete(raf.ticket,182.));
    CHECK(clock.last_offered==3);CHECK(clock.last_completed==1);
    CHECK(clock.schedule.debt_seconds()>=2./60.-1.e-8);
    CHECK(Near(clock.credited_seconds,.0702));
}
void SharedServiceClock(){
    Begin("arrival-service-clock-and-host-getter-share-one-monotonic-anchor");
    Guard guard;CHECK(guard.Configure(true)==1);guard.Start(1);Clock clock;
    auto request=guard.Request(69.,false);
    auto raf=guard.Raf(request,70.,70.);
    CHECK(raf.service);clock.Consume(raf);CHECK(guard.Complete(raf.ticket,73.));
    request=guard.Request(74.,true);
    const auto timer=guard.Timer(guard.timer_serial(),100.,true);
    CHECK(timer.service);clock.Consume(timer);
    // A second scheduled tick may already have observed time 106 inside this
    // timer callback. The raw timestamp of the later RAF will be only 105.
    CHECK(Near(guard.HostTime(clock.credited_seconds,106.,true),.036));
    CHECK(guard.Complete(timer.ticket,110.));
    const double previousEnd=guard.HostTime(clock.credited_seconds,110.,true);
    CHECK(Near(previousEnd,.040));
    raf=guard.Raf(request,105.,115.);
    CHECK(raf.service);CHECK(Near(raf.acceptedTimestampMs,115.));
    CHECK(Near(raf.realRafDeltaMs,35.)); // Raw RAF detector still sees 70 -> 105.
    clock.Consume(raf);
    const double followingEntry=guard.HostTime(clock.credited_seconds,115.,true);
    CHECK(Near(followingEntry,.045));CHECK(followingEntry>=previousEnd);
    CHECK(guard.Complete(raf.ticket,117.));

    // Ordinary RAFs may also have different timestamp-to-arrival offsets.
    // A raw stamp older than the preceding native completion remains safe
    // because both the service delta and getter use actual arrivals.
    request=guard.Request(117.2,false);raf=guard.Raf(request,113.,123.);
    CHECK(raf.service);clock.Consume(raf);
    CHECK(Near(guard.HostTime(clock.credited_seconds,130.,true),.060));
    CHECK(guard.Complete(raf.ticket,131.));
    request=guard.Request(132.,false);raf=guard.Raf(request,125.,140.);
    CHECK(raf.service);CHECK(Near(raf.acceptedTimestampMs,140.));
    clock.Consume(raf);
    CHECK(Near(guard.HostTime(clock.credited_seconds,140.,true),.070));
    CHECK(Near(guard.HostTime(clock.credited_seconds,142.,true),.072));
    CHECK(guard.Complete(raf.ticket,145.));
    request=guard.Request(146.,false);

    // Pause/resume clears the service anchor, never already-observed time.
    // That small getter floor does NOT become audio or schedule credit.
    clock.Pause();guard.ResetAfterPause();
    CHECK(Near(guard.HostTime(clock.credited_seconds,50000.,false),.072));
    guard.ResetAfterPause();clock.Pause();
    raf=guard.Raf(request,99995.,100000.);
    CHECK(raf.service);clock.Consume(raf);
    CHECK(Near(clock.credited_seconds,.070));
    CHECK(Near(guard.HostTime(clock.credited_seconds,100000.,true),.072));
    CHECK(guard.Complete(raf.ticket,100002.));
    request=guard.Request(100002.2,false);
    raf=guard.Raf(request,100003.,100008.);
    CHECK(raf.service);clock.Consume(raf);
    CHECK(Near(clock.credited_seconds,.078));
    CHECK(Near(guard.HostTime(clock.credited_seconds,100008.,true),.078));
    CHECK(guard.Complete(raf.ticket,100010.));

    // A callback that fails readiness updates the existing host's last stamp
    // but does not accept its wall delta. While not ready the getter is frozen.
    request=guard.Request(100010.2,false);
    raf=guard.Raf(request,200000.,200005.);
    CHECK(raf.service);clock.RejectReadiness(raf);
    CHECK(Near(guard.HostTime(clock.credited_seconds,200010.,false),.078));
    CHECK(guard.Complete(raf.ticket,200011.));
    request=guard.Request(200011.2,false);
    raf=guard.Raf(request,200008.,200013.);
    CHECK(raf.service);clock.Consume(raf);
    CHECK(Near(clock.credited_seconds,.086));
    CHECK(Near(clock.audio_ms+clock.audio_remainder_ms,86.));
    CHECK(Near(guard.HostTime(clock.credited_seconds,200013.,true),.086));
    CHECK(guard.Complete(raf.ticket,200015.));
    guard.Stop(2);
    CHECK(Near(guard.HostTime(clock.credited_seconds,999999.,false),.086));
    guard.Start(3);
    CHECK(Near(guard.HostTime(0.,999999.,false),0.));
}
void SlowNativeAndReentry(){
    Begin("slow-native-not-preempted-and-reentrant-arrival-rejected");
    Guard guard;const auto request=Prime(guard);
    const auto timer=guard.Timer(guard.timer_serial(),200.,true);
    CHECK(timer.service);CHECK(guard.callback_active());
    CHECK(!guard.Raf(request,201.,202.).service);
    CHECK(Status(guard,Guard::LastDecision)==unsigned(Guard::Decision::ReentrantArrival));
    CHECK(guard.callback_active());CHECK(guard.Complete(timer.ticket,300.));
    CHECK(Near(Status(guard,Guard::LastCallbackCompletedMs),300.));
    CHECK(Near(Status(guard,Guard::LastTimerLatenessMs),71.8));
    CHECK(!guard.Timer(guard.timer_serial(),301.,true).service);
    const auto late=guard.Raf(request,299.,301.);
    CHECK(!late.service);CHECK(late.sampleRaf);
    CHECK(Status(guard,Guard::TimerServices)==1);
    const auto next=guard.Request(301.2,false);
    CHECK(next!=0);
}

void ContinuousPendingRaf(){
    Begin("pending-raf-receives-repeated-completion-anchored-services");
    Guard guard;const auto request=Prime(guard);Clock clock;
    Guard::Dispatch initial;initial.service=true;initial.acceptedTimestampMs=100.;
    clock.Consume(initial);
    Guard::Token previousArm=0;
    const auto requests=Status(guard,Guard::RafRequests);
    for(unsigned i=0;i<13;++i){
        const auto arm=guard.timer_serial();const double began=guard.deadline_ms();
        CHECK(arm!=0);CHECK(arm!=previousArm);CHECK(guard.timer_current(arm));
        const auto timer=guard.Timer(arm,began,true);
        CHECK(timer.service);CHECK(!timer.sampleRaf);CHECK(guard.pending_raf());
        CHECK(guard.Rearm(timer.ticket,true)==0); // Callback is still executing.
        clock.Consume(timer);
        const double completed=began+3.;
        CHECK(guard.Complete(timer.ticket,completed));CHECK(!guard.timer_armed());
        const auto nextArm=guard.Rearm(timer.ticket,true);
        CHECK(nextArm!=0);CHECK(nextArm!=arm);CHECK(guard.timer_current(nextArm));
        CHECK(Near(guard.deadline_ms(),completed+Guard::ContinuationMs));
        CHECK(Near(guard.timeout_delay_ms(completed),17.));
        CHECK(guard.Rearm(timer.ticket,true)==0); // One offer, not two timers.
        CHECK(guard.timer_current(nextArm));CHECK(guard.request_serial()==request);
        CHECK(guard.Request(completed+.1,true)==0);
        CHECK(Status(guard,Guard::RafRequests)==requests);
        previousArm=arm;
    }
    CHECK(Status(guard,Guard::TimerServices)==13);
    CHECK(Status(guard,Guard::CompensationUsed)==1); // v2 allows the next arm.
    const auto pendingArm=guard.timer_serial();
    // The same real request finally returns after a 280 ms display gap.
    const auto raf=guard.Raf(request,380.,380.1);
    CHECK(raf.service);CHECK(raf.sampleRaf);CHECK(!guard.timer_armed());
    clock.Consume(raf);CHECK(guard.Complete(raf.ticket,383.));
    CHECK(Near(clock.credited_seconds,.2801));
    CHECK(Near(clock.audio_ms+clock.audio_remainder_ms,280.1));
    CHECK(!guard.Timer(pendingArm,400.,true).service);
    CHECK(Status(guard,Guard::TimerServices)==13);
}
void OldArmCannotClearReplacement(){
    Begin("old-queued-arm-cannot-consume-or-clear-replacement-in-same-request");
    Guard guard;const auto request=Prime(guard),firstArm=guard.timer_serial();
    const auto first=guard.Timer(firstArm,128.2,true);
    CHECK(first.service);CHECK(guard.Complete(first.ticket,131.2));
    const auto secondArm=guard.Rearm(first.ticket,true);
    CHECK(secondArm!=0);CHECK(secondArm!=firstArm);
    // This is the production adapter's handle-ownership predicate. Merely
    // matching the still-pending RAF request must not clear the new handle.
    bool replacementHandlePending=true;
    if(guard.timer_current(firstArm)&&!guard.callback_active())replacementHandlePending=false;
    CHECK(replacementHandlePending);
    CHECK(!guard.Timer(firstArm,180.,true).service);
    CHECK(guard.timer_current(secondArm));CHECK(guard.request_serial()==request);
    const auto second=guard.Timer(secondArm,181.,true);
    CHECK(second.service);CHECK(guard.callback_active());
    CHECK(!guard.Timer(firstArm,182.,true).service);
    CHECK(guard.callback_active());CHECK(!guard.Complete(first.ticket,183.));
    CHECK(guard.callback_active());CHECK(guard.Complete(second.ticket,185.));
    const auto thirdArm=guard.Rearm(second.ticket,true);
    CHECK(thirdArm!=0);CHECK(thirdArm!=secondArm);CHECK(thirdArm!=firstArm);
    CHECK(guard.Rearm(first.ticket,true)==0);CHECK(guard.timer_current(thirdArm));
}
void CompletionNeverCatchesUpTimeouts(){
    Begin("late-timer-and-slow-native-reanchor-next-deadline-after-completion");
    Guard guard;const auto request=Prime(guard),oldArm=guard.timer_serial();
    const auto late=guard.Timer(oldArm,400.,true);
    CHECK(late.service);CHECK(Status(guard,Guard::TimerServices)==1);
    CHECK(Near(Status(guard,Guard::LastTimerLatenessMs),271.8));
    CHECK(guard.Rearm(late.ticket,true)==0);
    CHECK(guard.Complete(late.ticket,700.)); // Cannot preempt this slow callback.
    const auto next=guard.Rearm(late.ticket,true);
    CHECK(next!=0);CHECK(Near(guard.deadline_ms(),700.+1000./60.));
    CHECK(Near(guard.timeout_delay_ms(700.),17.));
    CHECK(Status(guard,Guard::TimerServices)==1);CHECK(guard.request_serial()==request);
    CHECK(!guard.Timer(oldArm,716.,true).service);CHECK(guard.timer_current(next));
    const auto later=guard.Timer(next,717.,true);
    CHECK(later.service);CHECK(guard.Complete(later.ticket,724.));
    CHECK(guard.Rearm(later.ticket,true)!=0);CHECK(Near(guard.deadline_ms(),724.+1000./60.));
    CHECK(Status(guard,Guard::TimerServices)==2);
}
void RepeatedTimerThenRaf(){
    Begin("latest-timer-coverage-and-near-raf-cancel-current-arm");
    for(const bool covered:{false,true}){
        Guard guard;const auto request=Prime(guard);Clock clock;
        Guard::Dispatch initial;initial.service=true;initial.acceptedTimestampMs=100.;
        clock.Consume(initial);
        double began=0,completed=0;
        for(unsigned i=0;i<3;++i){
            began=guard.deadline_ms();
            const auto timer=guard.Timer(guard.timer_serial(),began,true);
            CHECK(timer.service);CHECK(!timer.sampleRaf);clock.Consume(timer);
            completed=began+3.;CHECK(guard.Complete(timer.ticket,completed));
            CHECK(guard.Rearm(timer.ticket,true)!=0);
        }
        const auto cancelledArm=guard.timer_serial();
        const auto raf=guard.Raf(request,covered?began-1.:completed+1.,
            covered?completed+10.:completed+1.5);
        CHECK(raf.current);CHECK(raf.sampleRaf);CHECK(!raf.service);
        CHECK(!guard.timer_armed());CHECK(!guard.pending_raf());
        CHECK(Status(guard,covered?Guard::TimestampSkips:Guard::NearSkips)==1);
        CHECK(Near(clock.credited_seconds,(began-100.)/1000.));
        const auto nextRequest=guard.Request(completed+11.,false);
        CHECK(nextRequest!=0);
        CHECK(!guard.Timer(cancelledArm,completed+26.,true).service);
        CHECK(guard.pending_raf());CHECK(guard.request_serial()==nextRequest);
        const auto following=guard.Raf(nextRequest,completed+27.,completed+27.2);
        CHECK(following.service);clock.Consume(following);
        CHECK(guard.Complete(following.ticket,completed+30.));
        CHECK(Near(clock.audio_ms+clock.audio_remainder_ms,completed+27.2-100.));
    }
}
void RepeatedEligibilityAndPause(){
    Begin("repeated-chain-stops-on-hidden-not-ready-or-in-callback-pause");
    for(const unsigned loss:{0u,1u,2u}){
        Guard guard;const auto request=Prime(guard);
        auto timer=guard.Timer(guard.timer_serial(),128.2,true);
        CHECK(timer.service);CHECK(guard.Complete(timer.ticket,131.2));
        const auto secondArm=guard.Rearm(timer.ticket,true);CHECK(secondArm!=0);
        const double secondAt=guard.deadline_ms();
        timer=guard.Timer(secondArm,secondAt,loss!=0);
        if(loss==0){
            // Visibility/readiness lost between registration and dispatch.
            CHECK(timer.current);CHECK(!timer.service);
        }else{
            CHECK(timer.service);
            if(loss==2){
                // runtimeFinish can pause and resume without changing epoch.
                guard.ResetAfterPause();guard.ResetAfterPause();
            }
            CHECK(guard.Complete(timer.ticket,secondAt+3.));
            // Either eligibility is lost after work, or pause retired the offer.
            CHECK(guard.Rearm(timer.ticket,loss==2)==0);
        }
        CHECK(!guard.timer_armed());CHECK(guard.pending_raf());
        CHECK(guard.Rearm(timer.ticket,true)==0);
        CHECK(!guard.Timer(secondArm,500.,true).service);
        CHECK(guard.request_serial()==request);
        const auto resumed=guard.Raf(request,510.,510.2);
        CHECK(resumed.service);CHECK(guard.Complete(resumed.ticket,513.));
    }
}
void RepeatedNaturalInputWait(){
    Begin("repeated-timer-opportunities-preserve-natural-peer-input-wait");
    Guard guard;Prime(guard);Clock clock;
    Guard::Dispatch initial;initial.service=true;initial.acceptedTimestampMs=100.;
    clock.Consume(initial);
    CHECK(clock.schedule.advance(1./60.,true)==1);clock.schedule.blocked(true);
    for(unsigned i=0;i<6;++i){
        const double began=guard.deadline_ms();
        const auto timer=guard.Timer(guard.timer_serial(),began,true);
        CHECK(timer.service);clock.Consume(timer,false);
        CHECK(clock.last_offered==1);CHECK(clock.steps==0);CHECK(clock.schedule.retry_pending());
        CHECK(guard.Complete(timer.ticket,began+2.));
        CHECK(guard.Rearm(timer.ticket,true)!=0);
    }
    const double began=guard.deadline_ms();
    const auto repaired=guard.Timer(guard.timer_serial(),began,true);
    CHECK(repaired.service);clock.Consume(repaired,true);
    CHECK(clock.steps==1);CHECK(clock.last_offered==1);
    CHECK(!clock.schedule.retry_pending());CHECK(Near(clock.schedule.debt_seconds(),0.));
    CHECK(Near(clock.audio_ms+clock.audio_remainder_ms,began-100.));
    CHECK(guard.Complete(repaired.ticket,began+2.));
}
void InvalidCompletionCannotRearm(){
    Begin("invalid-or-backward-completion-cannot-arm-another-timeout");
    for(const double ended:{std::numeric_limits<double>::quiet_NaN(),127.}){
        Guard guard;Prime(guard);
        const auto timer=guard.Timer(guard.timer_serial(),128.2,true);
        CHECK(timer.service);CHECK(guard.Complete(timer.ticket,ended));
        CHECK(guard.Rearm(timer.ticket,true)==0);CHECK(!guard.timer_armed());
        CHECK(guard.pending_raf());
    }
}
} // namespace
int main(){
    DefaultAndLock();NormalCadence();DelayedRaf();NoImplicitRearm();
    BothArrivalOrders();BackwardTimestamps();RetiredTimeout();RestartInsideCallback();
    PauseAndVisibility();IneligibleAndEarly();QuantizedTimeoutAdapter();TimerCannotSampleDisplay();
    NaturalInputWait();CpuDebtSurvives();SharedServiceClock();SlowNativeAndReentry();
    ContinuousPendingRaf();OldArmCannotClearReplacement();CompletionNeverCatchesUpTimeouts();
    RepeatedTimerThenRaf();RepeatedEligibilityAndPause();RepeatedNaturalInputWait();
    InvalidCompletionCannotRearm();
    std::printf("{\"schema\":\"th10-raf-deadline-guard-unit/1\",\"passed\":%s,"
        "\"cases\":%u,\"checks\":%u,\"failures\":%u,\"caseNames\":[",
        failures?"false":"true",cases,checks,failures);
    for(unsigned i=0;i<cases;++i)std::printf("%s\"%s\"",i?",":"",case_names[i]);
    std::puts("]}");return failures?1:0;
}
