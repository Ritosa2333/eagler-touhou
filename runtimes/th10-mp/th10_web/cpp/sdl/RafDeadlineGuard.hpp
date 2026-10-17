#pragma once
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>

namespace th10::host {
// TH10-only, default-OFF scheduling experiment. It owns service timestamps,
// but no simulation debt, world, transport, renderer or browser handle. One
// pending real RAF first waits DeadlineMs, then receives successive timeout
// services at least ContinuationMs AFTER the preceding complete callback.
// Only the host may arm
// one timeout from a completed ticket; this is not an interval/catch-up loop.
// Browser/compositor starvation and slow native work remain outside this
// policy's guarantee.
class RafDeadlineGuard {
public:
    using Token=std::uintptr_t;
    static constexpr double DeadlineMs=25.;
    static constexpr double ContinuationMs=1000./60.;
    static constexpr double NearCompletionMs=4.;
    enum class Source : unsigned { None=0,Raf=1,Timer=2 };
    enum class Decision : unsigned {
        None=0,RafService=1,TimerService=2,NearRafSkipped=3,
        CoveredTimestampSkipped=4,StaleArrival=5,IneligibleTimer=6,
        Stopped=7,EarlyTimer=8,ReentrantArrival=9,InvalidTime=10
    };
    enum StatusField : unsigned {
        Version,Enabled,Locked,Running,LoopEpoch,RequestSerial,PendingRaf,
        TimerArmed,CompensationUsed,CallbackActive,LastSource,LastDecision,
        RafRequests,RafArrivals,RafServices,TimerArrivals,TimerServices,
        StaleArrivals,NearSkips,TimestampSkips,EligibilitySkips,
        LastRafTimestampMs,LastAcceptedTimestampMs,LastCallbackCompletedMs,
        LastRequestAtMs,LastDeadlineMs,LastArrivalAtMs,LastTimerLatenessMs,
        MaxTimerLatenessMs,LastRealRafDeltaMs,CallbackSequence,
        SkippedRafSamples,StatusCount
    };
    struct Ticket {
        Token epoch=0,request=0;
        std::uint64_t sequence=0;
        Source source=Source::None;
    };
    struct Dispatch {
        bool current=false,sampleRaf=false,service=false;
        double acceptedTimestampMs=-1,realRafDeltaMs=0;
        Ticket ticket{};
    };

    // Deliberately stricter than "until app close": the first loop start locks
    // this Module for its whole lifetime. A measurement stop must never unlock
    // the mode. A fresh Module is required for the other side of an A/B pair.
    int Configure(bool enabled){
        if(locked_||running_)return -1;
        enabled_=enabled;return enabled_?1:0;
    }
    bool enabled()const{return enabled_;}
    bool running()const{return running_;}
    bool callback_active()const{return callback_active_;}
    bool timer_armed()const{return timer_armed_;}
    bool pending_raf()const{return pending_raf_;}
    Token epoch()const{return epoch_;}
    Token request_serial()const{return request_serial_;}
    Token timer_serial()const{return timer_serial_;}
    // A timeout is identified by its own arm, not by the shared RAF request.
    // Host adapters must check this before clearing their current handle.
    bool timer_current(Token timer)const{
        return Current(request_serial_)&&timer_armed_&&timer!=0&&timer==timer_serial_;
    }
    Source source()const{return last_source_;}
    double deadline_ms()const{return deadline_ms_;}
    // DOM timeout delays are integral milliseconds on the deployed adapter.
    // Round UP so a fractional remaining delay cannot consume the current
    // arm just before its absolute deadline. An early arrival stops the chain.
    double timeout_delay_ms(double nowMs)const{
        return std::ceil(std::max(0.,deadline_ms_-nowMs));
    }
    double last_real_raf_delta_ms()const{return last_real_raf_delta_ms_;}
    // Service entry and this host getter use the SAME performance-time anchor.
    // A raw RAF timestamp may predate a previous callback's end or even its
    // second tick. It must never be used as this clock's next service entry.
    // Retain only already-observed host time across readiness/pause fences;
    // this floor does not credit audio, simulation delta or pending-frame debt.
    double HostTime(double elapsedSeconds,double nowMs,bool advancing){
        double candidate=elapsedSeconds;
        if(advancing&&accepted_ms_>=0&&ValidTime(nowMs))
            candidate+=std::max(0.,nowMs-accepted_ms_)/1000.;
        if(std::isfinite(candidate)&&candidate>=0)
            host_time_frontier_=std::max(host_time_frontier_,candidate);
        return host_time_frontier_;
    }

    void Start(Token epoch){
        locked_=true;running_=true;epoch_=epoch;
        pending_raf_=timer_armed_=compensation_used_=callback_active_=false;
        timer_serviced_request_=timer_chain_allowed_=rearm_available_=false;
        host_time_frontier_=0;
        last_source_=Source::None;last_decision_=Decision::None;
        ResetClocks();request_at_ms_=deadline_ms_=arrival_at_ms_=-1;
    }
    void Stop(Token retiredEpoch){
        running_=false;epoch_=retiredEpoch;
        pending_raf_=timer_armed_=compensation_used_=callback_active_=false;
        timer_serviced_request_=timer_chain_allowed_=rearm_available_=false;
        last_decision_=Decision::Stopped;
    }
    // Real RAFs continue while paused so the ordinary host can pump network
    // retirement/ACKs. Neither pause nor resume opens a new timeout chain.
    void ResetAfterPause(){
        timer_armed_=timer_chain_allowed_=rearm_available_=false;
        timer_serviced_request_=false;ResetClocks();
    }

    // Called only after the preceding RAF callback has fully returned, or
    // after consuming a skipped real RAF. Deadlines are anchored to the last
    // completed native host callback, including its synchronous runtimeFinish.
    Token Request(double requestedAtMs,bool eligible){
        if(!enabled_||!running_||pending_raf_||callback_active_||
           !ValidTime(requestedAtMs))return 0;
        if(++request_serial_==0)++request_serial_;
        ++raf_requests_;
        pending_raf_=true;compensation_used_=false;timer_serviced_request_=false;
        timer_armed_=rearm_available_=false;
        request_at_ms_=requestedAtMs;
        deadline_ms_=completed_ms_>=0?completed_ms_+DeadlineMs:-1;
        timer_chain_allowed_=eligible&&ValidTime(deadline_ms_);
        if(timer_chain_allowed_)ArmTimer();
        return request_serial_;
    }

    Dispatch Raf(Token request,double timestampMs,double arrivedAtMs){
        ++raf_arrivals_;
        Dispatch result;
        if(!Current(request))return Stale(result);
        if(callback_active_){last_decision_=Decision::ReentrantArrival;return result;}
        result.current=true;pending_raf_=false;
        timer_armed_=timer_chain_allowed_=rearm_available_=false;
        last_source_=Source::Raf;
        if(!ValidTime(timestampMs)||!ValidTime(arrivedAtMs)){
            last_decision_=Decision::InvalidTime;return result;
        }
        arrival_at_ms_=arrivedAtMs;
        // The raw RAF frontier is separate from the actual-arrival service clock.
        // Timer samples, including their short following intervals, cannot
        // manufacture high-refresh samples. Backward RAF stamps do not move
        // this frontier backward either.
        last_raf_timestamp_ms_=timestampMs;
        const double previousRaf=real_raf_frontier_ms_;
        result.sampleRaf=true;
        result.realRafDeltaMs=real_raf_frontier_ms_<0?0:
            std::max(0.,timestampMs-real_raf_frontier_ms_);
        real_raf_frontier_ms_=std::max(real_raf_frontier_ms_,timestampMs);
        last_real_raf_delta_ms_=result.realRafDeltaMs;
        if((previousRaf>=0&&timestampMs<=previousRaf)||
           (timer_coverage_ms_>=0&&timestampMs<=timer_coverage_ms_)){
            ++timestamp_skips_;++skipped_raf_samples_;
            last_decision_=Decision::CoveredTimestampSkipped;return result;
        }
        if(timer_serviced_request_&&timer_completed_ms_>=0&&
           arrivedAtMs-timer_completed_ms_<NearCompletionMs){
            ++near_skips_;++skipped_raf_samples_;
            last_decision_=Decision::NearRafSkipped;return result;
        }
        return Begin(result,Source::Raf,arrivedAtMs);
    }

    Dispatch Timer(Token timer,double arrivedAtMs,bool eligible){
        ++timer_arrivals_;
        Dispatch result;
        if(!timer_current(timer))return Stale(result);
        if(callback_active_){last_decision_=Decision::ReentrantArrival;return result;}
        result.current=true;
        timer_armed_=rearm_available_=false;compensation_used_=true;
        last_source_=Source::Timer;
        if(!ValidTime(arrivedAtMs)||!ValidTime(deadline_ms_)){
            timer_chain_allowed_=false;last_decision_=Decision::InvalidTime;return result;
        }
        arrival_at_ms_=arrivedAtMs;
        timer_lateness_ms_=arrivedAtMs-deadline_ms_;
        max_timer_lateness_ms_=std::max(max_timer_lateness_ms_,timer_lateness_ms_);
        if(arrivedAtMs<deadline_ms_){
            timer_chain_allowed_=false;last_decision_=Decision::EarlyTimer;return result;
        }
        if(!eligible){
            timer_chain_allowed_=false;
            ++eligibility_skips_;last_decision_=Decision::IneligibleTimer;return result;
        }
        return Begin(result,Source::Timer,std::max(accepted_ms_,arrivedAtMs));
    }

    // The service ticket, rather than a global "running" bit, owns completion.
    // runtimePrepare/runtimeFinish may synchronously stop and start another
    // loop; completion of that old callback must not touch the new request.
    bool Complete(const Ticket& ticket,double completedAtMs){
        if(!enabled_||!running_||!callback_active_||ticket.epoch!=epoch_||
           ticket.request!=request_serial_||ticket.sequence!=callback_sequence_||
           ticket.source!=last_source_)
            return false;
        callback_active_=false;
        const bool validCompletion=ValidTime(completedAtMs)&&completedAtMs>=accepted_ms_;
        if(ValidTime(completedAtMs))completed_ms_=std::max(completed_ms_,completedAtMs);
        if(ticket.source==Source::Timer){
            timer_serviced_request_=true;timer_completed_ms_=completed_ms_;
            rearm_available_=timer_chain_allowed_&&validCompletion;
        }
        return true;
    }

    // Only the just-completed timer may offer one new arm. Consuming the offer
    // even when eligibility is lost prevents a later caller from restarting
    // this chain without a real RAF. Pause/reset invalidates the offer too.
    // The caller must also check frame's return and its application ownership.
    Token Rearm(const Ticket& ticket,bool eligible){
        if(!Current(ticket.request)||callback_active_||timer_armed_||
           ticket.epoch!=epoch_||ticket.sequence!=callback_sequence_||
           ticket.source!=Source::Timer||!rearm_available_)return 0;
        rearm_available_=false;
        if(!eligible){timer_chain_allowed_=false;return 0;}
        if(!timer_chain_allowed_||!ValidTime(completed_ms_))return 0;
        // Never advance an expired deadline by a fixed interval. Native work
        // and runtimeFinish may have been slow; they always get a fresh gap.
        deadline_ms_=completed_ms_+ContinuationMs;
        return ArmTimer();
    }

    void Status(double* out)const{
        out[Version]=2;out[Enabled]=enabled_;out[Locked]=locked_;out[Running]=running_;
        out[LoopEpoch]=double(epoch_);out[RequestSerial]=double(request_serial_);
        out[PendingRaf]=pending_raf_;out[TimerArmed]=timer_armed_;
        // v2: at least one timer attempt was consumed for this RAF request;
        // unlike v1, this bit does not prohibit subsequent completed arms.
        out[CompensationUsed]=compensation_used_;out[CallbackActive]=callback_active_;
        out[LastSource]=unsigned(last_source_);out[LastDecision]=unsigned(last_decision_);
        out[RafRequests]=double(raf_requests_);out[RafArrivals]=double(raf_arrivals_);
        out[RafServices]=double(raf_services_);out[TimerArrivals]=double(timer_arrivals_);
        out[TimerServices]=double(timer_services_);out[StaleArrivals]=double(stale_arrivals_);
        out[NearSkips]=double(near_skips_);out[TimestampSkips]=double(timestamp_skips_);
        out[EligibilitySkips]=double(eligibility_skips_);
        out[LastRafTimestampMs]=last_raf_timestamp_ms_;out[LastAcceptedTimestampMs]=accepted_ms_;
        out[LastCallbackCompletedMs]=completed_ms_;out[LastRequestAtMs]=request_at_ms_;
        out[LastDeadlineMs]=deadline_ms_;out[LastArrivalAtMs]=arrival_at_ms_;
        out[LastTimerLatenessMs]=timer_lateness_ms_;out[MaxTimerLatenessMs]=max_timer_lateness_ms_;
        out[LastRealRafDeltaMs]=last_real_raf_delta_ms_;
        out[CallbackSequence]=double(callback_sequence_);out[SkippedRafSamples]=double(skipped_raf_samples_);
    }
private:
    bool enabled_=false,locked_=false,running_=false,pending_raf_=false;
    bool timer_armed_=false,compensation_used_=false,callback_active_=false;
    bool timer_serviced_request_=false,timer_chain_allowed_=false,rearm_available_=false;
    Token epoch_=0,request_serial_=0,timer_serial_=0;
    Source last_source_=Source::None;
    Decision last_decision_=Decision::None;
    double accepted_ms_=-1,real_raf_frontier_ms_=-1,last_raf_timestamp_ms_=-1;
    double timer_coverage_ms_=-1,host_time_frontier_=0;
    double completed_ms_=-1,timer_completed_ms_=-1,request_at_ms_=-1,deadline_ms_=-1;
    double arrival_at_ms_=-1,timer_lateness_ms_=-1,max_timer_lateness_ms_=0;
    double last_real_raf_delta_ms_=0;
    std::uint64_t raf_requests_=0,raf_arrivals_=0,raf_services_=0,timer_arrivals_=0;
    std::uint64_t timer_services_=0,stale_arrivals_=0,near_skips_=0,timestamp_skips_=0;
    std::uint64_t eligibility_skips_=0,callback_sequence_=0,skipped_raf_samples_=0;

    static bool ValidTime(double value){return std::isfinite(value)&&value>=0;}
    bool Current(Token request)const{
        return enabled_&&running_&&pending_raf_&&request!=0&&request==request_serial_;
    }
    Token ArmTimer(){
        // Do not reuse a token, including across stop/start. Exhaustion fails
        // closed instead of allowing a queued old callback to own a new arm.
        if(!ValidTime(deadline_ms_)||timer_serial_==Token(-1)){
            timer_chain_allowed_=false;return 0;
        }
        ++timer_serial_;timer_armed_=true;return timer_serial_;
    }
    void ResetClocks(){
        accepted_ms_=real_raf_frontier_ms_=last_raf_timestamp_ms_=-1;
        timer_coverage_ms_=-1;
        completed_ms_=timer_completed_ms_=-1;last_real_raf_delta_ms_=0;
    }
    Dispatch Stale(Dispatch result){
        ++stale_arrivals_;last_decision_=Decision::StaleArrival;return result;
    }
    Dispatch Begin(Dispatch result,Source source,double timestampMs){
        accepted_ms_=std::max(accepted_ms_,timestampMs);callback_active_=true;
        result.service=true;result.acceptedTimestampMs=accepted_ms_;
        result.ticket={epoch_,request_serial_,++callback_sequence_,source};
        if(source==Source::Raf){++raf_services_;last_decision_=Decision::RafService;}
        else {
            timer_coverage_ms_=accepted_ms_;
            ++timer_services_;last_decision_=Decision::TimerService;
        }
        return result;
    }
};
static_assert(RafDeadlineGuard::StatusCount==32,"RAF guard status schema");
} // namespace th10::host
