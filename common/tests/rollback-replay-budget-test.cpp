#include <eagler/netplay/RollbackReplayBudget.hpp>

#include <cassert>
#include <cstdio>

int main()
{
    using Netplay::RollbackReplayBudget;
    static_assert(RollbackReplayBudget::SliceBudgetNs == 4'000'000);
    static_assert(RollbackReplayBudget::MaxFramesPerSlice == 4);
    assert(RollbackReplayBudget::CanContinue(0, 100'000'000));
    assert(RollbackReplayBudget::CanContinue(
        1, RollbackReplayBudget::SliceBudgetNs - 1));
    assert(!RollbackReplayBudget::CanContinue(
        1, RollbackReplayBudget::SliceBudgetNs));
    assert(!RollbackReplayBudget::CanContinue(
        RollbackReplayBudget::MaxFramesPerSlice, 0));
    std::puts("rollback replay budget: PASS experimental 4ms/4-frame slice");
}
