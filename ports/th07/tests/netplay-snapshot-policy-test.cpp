#include <eagler/netplay/SnapshotPolicy.hpp>
#include <eagler/netplay/RollbackJournal.hpp>
#include <cassert>
#include <cstdio>
#include <limits>

int main()
{
    constexpr auto missing = std::numeric_limits<std::uint32_t>::max();
    assert(Netplay::NeedsRollbackSnapshot(101, 99, 0, false));
    assert(Netplay::NeedsRollbackSnapshot(101, 100, 2, false));
    assert(Netplay::NeedsRollbackSnapshot(101, missing, 0, false));
    assert(!Netplay::NeedsRollbackSnapshot(101, 100, 0, false));
    assert(!Netplay::NeedsRollbackSnapshot(0, missing, 0, false));
    assert(Netplay::NeedsRollbackSnapshot(101, 101, 0, true));
    unsigned combinations = 0;
    for (unsigned f = 1; f < 201; ++f)
        for (unsigned c = 0; c < 202; ++c)
            for (unsigned mask = 0; mask < 8; ++mask)
            {
                if (!Netplay::NeedsRollbackSnapshot(f, c, mask, false))
                    assert(mask == 0 && c >= f - 1);
                ++combinations;
            }
    // Journal capacity survives a verified gap; the next epoch is standalone.
    Netplay::RollbackJournal journal;
    assert(journal.Reset({8, 1024, 32}));
    int state = 0;
    for (unsigned epoch = 0; epoch < 1000; ++epoch)
    {
        const unsigned f = epoch * 20;
        assert(journal.BeginFrame(f));
        assert(journal.Touch(&state, sizeof(state)));
        const int before = state;
        state += 3;
        assert(journal.EndFrame());
        assert(journal.BeginFrame(f + 1, true));
        assert(journal.Touch(&state, sizeof(state)));
        ++state;
        assert(journal.EndFrame());
        assert(journal.UndoTo(f + 1));
        assert(state == before);
        assert(journal.BeginFrame(f));
        assert(journal.Touch(&state, sizeof(state)));
        state += 2;
        assert(journal.EndFrame());
        journal.DiscardBefore(f + 1);
        assert(journal.FrameCount() == 0);
    }
    assert(Netplay::BoundedCaptureBatch(8, 100, 101, 600) == 2);
    assert(Netplay::BoundedCaptureBatch(8, 598, 610, 600) == 2);
    assert(Netplay::BoundedCaptureBatch(8, 600, 610, 600) == 0);
    assert(Netplay::BoundedCaptureBatch(8, 100, 99, 600) == 0);
    assert(Netplay::BoundedCaptureBatch(10000000000ULL, 100, 110, 600) == 8);
    std::printf("snapshot policy: PASS %u combinations, 1000 journal epochs, bounded fresh capture\n", combinations);
}
