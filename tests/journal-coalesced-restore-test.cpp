#include <eagler/netplay/RollbackJournal.hpp>
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <random>
#include <vector>

int main()
{
    std::uint64_t saved = 0;
    for (bool fast : {false, true})
    {
        Netplay::RollbackJournal journal;
        assert(journal.Reset({8, 65536, 64, fast, true}));
        std::vector<std::uint8_t> world(32768, 37);
        const auto before = world;
        for (unsigned f = 0; f < 8; ++f)
        {
            assert(journal.BeginFrame(f));
            assert(journal.Touch(world.data(), world.size()));
            std::fill(world.begin(), world.end(), static_cast<std::uint8_t>(f));
            assert(journal.EndFrame());
        }
        assert(journal.UndoTo(0) && world == before);
        assert(journal.RestoreCopiedBytes() == world.size());
        assert(journal.RestoreSkippedBytes() == world.size() * 7);
        saved += journal.RestoreSkippedBytes();

        std::mt19937 rng(71855);
        std::array<std::vector<std::uint8_t>, 6> reference;
        for (unsigned run = 0; run < 1500; ++run)
        {
            for (unsigned f = 0; f < 6; ++f)
            {
                reference[f] = world;
                assert(journal.BeginFrame(run * 10 + f));
                std::array<bool, 32> used{};
                for (unsigned b = 0; b < 24; ++b)
                {
                    const unsigned start = rng() % 32, length = 1 + rng() % 8;
                    if (start + length > 32) continue;
                    bool overlap = false;
                    for (unsigned i = start; i < start + length; ++i) overlap |= used[i];
                    if (overlap) continue;
                    for (unsigned i = start; i < start + length; ++i) used[i] = true;
                    assert(journal.Touch(world.data() + start * 1024, length * 1024));
                    std::fill(world.begin() + start * 1024, world.begin() + (start + length) * 1024,
                              static_cast<std::uint8_t>(rng()));
                }
                assert(journal.EndFrame());
            }
            const unsigned target = rng() % 6;
            assert(journal.UndoTo(run * 10 + target));
            assert(world == reference[target]);
            journal.DiscardBefore(run * 10 + 10);
        }
    }
    std::printf("coalesced restore: PASS 3000 overlapping-history byte comparisons; %llu bytes avoided in dense cases\n",
                static_cast<unsigned long long>(saved));
}
