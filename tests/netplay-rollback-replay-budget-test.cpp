#include <eagler/netplay/RollbackReplayBudget.hpp>

#include <cassert>
#include <cstdio>

int main()
{
    using Netplay::RollbackReplayBudget;
    assert(RollbackReplayBudget::CanContinue(0, 100'000'000));
    assert(RollbackReplayBudget::CanContinue(1, RollbackReplayBudget::SliceBudgetNs - 1));
    assert(!RollbackReplayBudget::CanContinue(1, RollbackReplayBudget::SliceBudgetNs));
    assert(!RollbackReplayBudget::CanContinue(
        RollbackReplayBudget::MaxFramesPerSlice, 0));
    std::puts("rollback replay budget: PASS 4ms/4-frame browser slice");
}
