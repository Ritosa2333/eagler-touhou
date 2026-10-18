#include <eagler/netplay/FrameBudget.hpp>

#include <cassert>
#include <cstdio>
#include <limits>

using Netplay::FrameBudget;

static_assert(FrameBudget::MaxCatchupTicks == 6);
static_assert(FrameBudget::CatchupBudgetNs == 8'000'000);
static_assert(FrameBudget::CanStartTick(0, std::numeric_limits<std::uint64_t>::max()));
static_assert(FrameBudget::CanStartTick(1, FrameBudget::CatchupBudgetNs - 1));
static_assert(!FrameBudget::CanStartTick(1, FrameBudget::CatchupBudgetNs));
static_assert(!FrameBudget::CanStartTick(FrameBudget::MaxCatchupTicks, 0));

int main()
{
    // Six pending fixed ticks, with an expensive first tick. The budget must
    // yield after that atomic tick and retain the remaining five for a later
    // browser callback rather than dropping them.
    unsigned debt = 6;
    unsigned logicalFrames = 0;
    unsigned callbacks = 0;
    while (debt != 0)
    {
        std::uint64_t elapsed = 0;
        unsigned completed = 0;
        while (debt != 0 && FrameBudget::CanStartTick(completed, elapsed))
        {
            elapsed += logicalFrames == 0 ? 12'000'000 : 1'000'000;
            --debt;
            ++logicalFrames;
            ++completed;
        }
        if (callbacks == 0)
            assert(completed == 1 && debt == 5);
        ++callbacks;
    }
    assert(logicalFrames == 6 && callbacks == 2);
    std::puts("frame budget: PASS validated 8ms/6-tick catch-up policy");
}
