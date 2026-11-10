#pragma once
#include "SimulationCadence.hpp"
#include <eagler/netplay/PendingFrameSchedule.hpp>
#include <eagler/netplay/FrameBudget.hpp>
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace th11::multiplayer {
// Network forward progress owns its clock independently of display cadence.
// Time is nominal 60 Hz seconds; pace only newly elapsed wall time before
// advance(). Input capture, restoration and forward ownership stay in driver.
class FrameSchedule final {
    using Cadence=SimulationCadence;
    Netplay::PendingFrameSchedule<Cadence> schedule_;
    unsigned offered_=0;
public:
    static constexpr double interval=Cadence::interval;
    static constexpr unsigned max_ticks=4; // TH11's original per-callback cap.
    static constexpr double max_debt=.1;   // TH11's original retained clock cap.
    static constexpr std::uint64_t start_budget_ns=Netplay::FrameBudget::CatchupBudgetNs;

    void reset(){schedule_.reset();offered_=0;}
    bool retry_pending()const{return schedule_.retry_pending();}
    unsigned owed()const{return offered_;}
    // Remaining nominal seconds not yet offered, including whole CPU debt.
    // complete() never clears its fractional part or rescales this phase.
    double phase()const{return schedule_.debt;}

    unsigned advance(double normalized_seconds){
        // Defensive callback settlement: never silently lose an offered tick.
        // An unresolved existing network retry remains a network wait until
        // the caller explicitly declares completion or CPU recovery.
        if(offered_){if(schedule_.retry_pending())network_wait();else cpu_yield();}
        const double elapsed=std::isfinite(normalized_seconds)?std::max(0.,normalized_seconds):0.;
        offered_=schedule_.advance(elapsed,true);return offered_;
    }
    // Call only after a NEW complete forward transaction, never per resim tick.
    bool complete(){
        if(!offered_)return false;
        --offered_;schedule_.complete();return true;
    }
    // Missing actual information retains one already-due obligation and phase.
    // Network waiting time is ignored by the next advance(), even if it is
    // that next callback which finally obtains the missing input.
    void network_wait(){
        if(!offered_)return;
        schedule_.blocked(true);offered_=0;
    }
    // CPU defer/recovery returns this callback's remaining obligations once.
    // From network retry this adds only its one offered tick, never wait time.
    void cpu_yield(){
        if(!offered_)return;
        schedule_.complete();
        schedule_.debt=std::min(max_debt,schedule_.debt+offered_*interval);
        offered_=0;
    }
    // The start budget does not preempt an atomic tick. History work must not
    // be counted as completed forward: after full restore the original first
    // due forward still gets one attempt, even if restoration used the budget.
    static constexpr bool can_start(unsigned completed_forward,std::uint64_t elapsed_ns,bool forward_pair=false){
        // Only one additional already-owed forward can amortize the final
        // GPU/audio work. Do not extend history recovery or admit a third
        // expensive tick. The caller disables this after any resimulation.
        return completed_forward<max_ticks&&
            (Netplay::FrameBudget::CanStartTick(completed_forward,elapsed_ns)||
             (forward_pair&&completed_forward==1&&elapsed_ns<16'000'000));
    }
    // Start gates between complete history transactions. The common callback
    // origin is retained; only the FINAL history tick has a short completion
    // opportunity. This is not a 12ms callback-duration guarantee.
    // Tail completion remains experimental: the actual r19 browser run did
    // not establish a stable cadence win. Default production keeps 8ms.
    static constexpr bool can_replay_next(unsigned completed,unsigned remaining,double elapsed_ms,bool tail_completion=false){
        return remaining&&(!completed||elapsed_ms<(tail_completion&&remaining==1?12.:8.));
    }
};
}
