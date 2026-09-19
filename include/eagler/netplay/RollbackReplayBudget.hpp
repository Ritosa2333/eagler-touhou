#pragma once

#include <cstdint>

namespace Netplay
{
// Experimental browser-event-loop budget for TH07's opt-in incremental
// reconciliation path. This does not add input delay and is not part of the
// current production timing policy. One replay frame is always allowed so an
// expensive frame cannot deadlock the reconciliation state machine.
struct RollbackReplayBudget
{
    static constexpr std::uint64_t SliceBudgetNs = 4'000'000;
    static constexpr std::uint32_t MaxFramesPerSlice = 4;

    static constexpr bool CanContinue(std::uint32_t completedFrames,
                                      std::uint64_t elapsedNs)
    {
        return completedFrames < MaxFramesPerSlice &&
               (completedFrames == 0 || elapsedNs < SliceBudgetNs);
    }
};
} // namespace Netplay
