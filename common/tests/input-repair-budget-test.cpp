#include <eagler/netplay/InputRepairBudget.hpp>

#include <cassert>
#include <cstdint>

int main()
{
    Netplay::InputRepairBudget budget;
    for (std::uint64_t frame = 0; frame < 10000; ++frame)
        assert(!budget.ShouldRepair(static_cast<std::uint32_t>(frame), true, frame * 16));

    budget = {};
    constexpr auto stalled = Netplay::InputRepairBudget::StalledMs;
    constexpr auto retry = Netplay::InputRepairBudget::RetryMs;
    assert(!budget.ShouldRepair(10, true, 0));
    assert(!budget.ShouldRepair(10, true, stalled - 1));
    assert(budget.ShouldRepair(10, true, stalled));
    assert(!budget.ShouldRepair(10, true, stalled + retry - 1));
    assert(budget.ShouldRepair(10, true, stalled + retry));
    assert(!budget.ShouldRepair(11, true, stalled + retry + 1));
    assert(!budget.ShouldRepair(11, true, 2 * stalled + retry));
    assert(budget.ShouldRepair(11, true, 2 * stalled + retry + 1));
    assert(!budget.ShouldRepair(11, false, 2 * stalled + retry + 2));
    assert(!budget.ShouldRepair(11, true, 1000));
    assert(!budget.ShouldRepair(11, true, 999));
    assert(budget.ShouldRepair(11, true, 999 + stalled));
    assert(!budget.ShouldRepair(0xffffffffu, true, 1150));
}
