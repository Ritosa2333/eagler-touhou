#include <eagler/netplay/FrameBudget.hpp>

#include <cassert>
#include <cstdio>
#include <limits>

using Netplay::FrameBudget;

static_assert(FrameBudget::MaxCatchupTicks == 6);
static_assert(FrameBudget::CanStartTick(0, std::numeric_limits<std::uint64_t>::max()));
static_assert(FrameBudget::CanStartTick(1, FrameBudget::CatchupBudgetNs - 1));
static_assert(!FrameBudget::CanStartTick(1, FrameBudget::CatchupBudgetNs));
static_assert(!FrameBudget::CanStartTick(6, 0));

int main()
{
    // Six pending fixed ticks, with an expensive rollback in the first tick.
    // It must yield after that atomic tick, not drop the remaining five.
    unsigned debt = 6;
    unsigned logicalFrames = 0;
    unsigned draws = 0;
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
        if (draws == 0)
            assert(completed == 1 && debt == 5);
        ++draws;
    }
    assert(logicalFrames == 6 && draws == 2);
    std::puts("TH07 netplay frame budget: PASS");
}
