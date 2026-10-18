#pragma once
#include <eagler/netplay/FrameBudget.hpp>
#include <eagler/netplay/PendingFrameSchedule.hpp>
#include <algorithm>
#include <cmath>

namespace th10::multiplayer {
namespace detail {
// Debt is measured in seconds of the nominal 60 Hz clock. The caller scales
// only newly elapsed wall time; changing network pace never rescales phase.
struct NetworkFrameCadence {
    static constexpr double interval=1./60.;
    static constexpr unsigned max_ticks=Netplay::FrameBudget::MaxCatchupTicks;
    static constexpr double max_debt=interval*max_ticks;
    double debt=0;

    void reset(){debt=0;}
    unsigned advance(double seconds){
        const double elapsed=std::isfinite(seconds)?std::max(0.,seconds):0.;
        debt=std::min(max_debt,debt+elapsed);
        const auto due=std::min(max_ticks,unsigned(std::floor((debt+1.e-9)/interval)));
        debt=std::max(0.,debt-due*interval);
        return due;
    }
};
}

// CPU catch-up and unfinished correction retain bounded nominal-clock debt.
// Only a live input wait uses TH09's pending-frame rule, which discards time
// spent waiting for information. CPU work must not silently slow that clock.
// This clock never captures input, skips a logical frame, or changes tick dt.
class NetworkFrameSchedule {
    using Cadence=detail::NetworkFrameCadence;
    Netplay::PendingFrameSchedule<Cadence> schedule;
public:
    static constexpr double interval=Cadence::interval;
    static constexpr unsigned max_ticks=Cadence::max_ticks;

    void reset(){schedule.reset();}
    bool retry_pending()const{return schedule.retry_pending();}
    double debt_seconds()const{return schedule.debt;}
    unsigned advance(double normalized_seconds,bool live=true){return schedule.advance(normalized_seconds,live);}
    void blocked(bool live){schedule.blocked(live);}
    void recovering(unsigned uncompleted=1){
        // A correction can begin while retrying an earlier network wait.
        // Return only this callback's offered obligation(s), never the wall
        // time already discarded by that wait. Subsequent CPU slices accrue
        // new time through the ordinary bounded cadence.
        schedule.complete();
        defer(uncompleted);
    }
    void complete(){schedule.complete();}

    // advance() has already deducted every offered tick. Return only the
    // unexecuted ticks when the callback's CPU start budget is exhausted.
    // A genuine network wait already retains one pending forward tick; never
    // refund other offered ticks on top of that network-wait obligation.
    void defer(unsigned uncompleted){
        if(schedule.retry_pending())return;
        schedule.debt=std::min(Cadence::max_debt,
            schedule.debt+std::min(uncompleted,max_ticks)*interval);
    }
    double presentation_alpha()const{
        // An already-due frame holds the current endpoint while input or CPU
        // work is pending. Its fractional remainder must not replay old motion.
        if(schedule.retry_pending()||schedule.debt+1.e-9>=interval)return 1.;
        return std::clamp(schedule.debt/interval,0.,1.);
    }
};
}
