#pragma once
#include <eagler/netplay/FrameBudget.hpp>
#include <cstdint>

namespace th10::multiplayer {
// TH10 history replay admits whole Update + authored Draw transactions. The
// caller must finish every admitted transaction before checking this again.
// Keep the ordinary 8 ms start budget, with one bounded opportunity to finish
// the final history tick. This 12 ms threshold is admission time, not a bound
// on transaction duration or total callback time; it does not admit a forward
// tick or make an unfinished correction safe to present.
struct RecoveryBudget {
    static constexpr std::uint64_t LastTickBudgetNs=12'000'000;

    static constexpr bool CanStartTick(std::uint32_t completedTicks,
                                       std::uint32_t remainingTicks,
                                       std::uint64_t elapsedNs){
        if(!remainingTicks||completedTicks>=Netplay::FrameBudget::MaxCatchupTicks)
            return false;
        // An overdue callback still makes progress through one full history
        // transaction. Neither this exception nor the tail bypasses the cap.
        return completedTicks==0||elapsedNs<(remainingTicks==1?
            LastTickBudgetNs:Netplay::FrameBudget::CatchupBudgetNs);
    }
};
}
