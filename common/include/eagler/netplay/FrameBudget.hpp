#pragma once

#include <cstdint>

namespace Netplay
{
// Browser/event-loop scheduling budget for starting *additional* catch-up
// simulation ticks. One due tick is always allowed, so this never discards
// simulation debt or turns a slow callback into a skipped logical frame.
//
// The 6-tick cap and 8 ms start budget are the validated production values from
// the TH07 browser performance investigation. TH06 already shares the six-tick
// cap; consuming this helper adds the same wall-time yield rule without adding
// input buffering or changing rollback ownership.
struct FrameBudget
{
    static constexpr std::uint32_t MaxCatchupTicks = 6;
    static constexpr std::uint64_t CatchupBudgetNs = 8'000'000;

    static constexpr bool CanStartTick(std::uint32_t completedTicks,
                                       std::uint64_t elapsedNs)
    {
        return completedTicks < MaxCatchupTicks &&
               (completedTicks == 0 || elapsedNs < CatchupBudgetNs);
    }
};
} // namespace Netplay
