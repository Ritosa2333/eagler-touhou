// Standalone deterministic storage workload, not a phone/game FPS claim.
// g++ -std=c++17 -O2 -Isrc tests/rollback-journal-benchmark.cpp \
//     src/netplay/RollbackJournal.cpp -o rollback-journal-benchmark
// Pass --require-zero-alloc to enforce allocation-free warmed history reuse.
#include <eagler/netplay/RollbackJournal.hpp>

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <new>
#include <vector>

namespace
{
bool countAllocations = false;
std::size_t allocations = 0;
std::size_t allocatedBytes = 0;
}

void *operator new(std::size_t size)
{
    if (countAllocations)
    {
        ++allocations;
        allocatedBytes += size;
    }
    if (void *memory = std::malloc(size ? size : 1))
        return memory;
    std::abort();
}
void *operator new[](std::size_t size) { return ::operator new(size); }
void operator delete(void *memory) noexcept { std::free(memory); }
void operator delete[](void *memory) noexcept { std::free(memory); }
void operator delete(void *memory, std::size_t) noexcept { std::free(memory); }
void operator delete[](void *memory, std::size_t) noexcept { std::free(memory); }

namespace
{
constexpr std::size_t BLOCKS = 1536;
constexpr std::size_t BLOCK_BYTES = 2048;
constexpr std::size_t ITERATIONS = 160;

void Check(bool result)
{
    if (!result)
    {
        std::fputs("rollback benchmark invariant failed\n", stderr);
        std::abort();
    }
}

void Capture(Netplay::RollbackJournal &journal, std::vector<unsigned char> &world,
             std::uint32_t frame, bool scattered)
{
    Check(journal.BeginFrame(frame));
    for (std::size_t index = 0; index < BLOCKS; ++index)
    {
        // A coprime stride exercises non-address-ordered spawn/owner writes.
        const std::size_t slot = scattered ? (index * 797u) % BLOCKS : index;
        auto *object = world.data() + slot * BLOCK_BYTES;
        Check(journal.Touch(object, BLOCK_BYTES));
        ++*object;
    }
    Check(journal.EndFrame());
    Check(journal.BeginFrame(frame + 1, true));
    // Typical two-tick checkpoints also see duplicate spawn/slot touches.
    for (std::size_t index = 0; index < 32; ++index)
    {
        auto *object = world.data() + index * BLOCK_BYTES;
        Check(journal.Touch(object, BLOCK_BYTES));
        ++*object;
    }
    Check(journal.EndFrame());
}

bool Run(const char *name, bool rollback, bool discard, bool scattered)
{
    Netplay::RollbackJournal journal;
    Check(journal.Reset({8, 8 * 1024 * 1024, 4096}));
    std::vector<unsigned char> world(BLOCKS * BLOCK_BYTES, 0);
    std::uint32_t frame = 0;
    for (; frame < 32; frame += 2)
        Capture(journal, world, frame, scattered);

    std::vector<double> times;
    times.reserve(ITERATIONS);
    allocations = 0;
    allocatedBytes = 0;
    countAllocations = true;
    for (std::size_t iteration = 0; iteration < ITERATIONS; ++iteration)
    {
        const auto begin = std::chrono::steady_clock::now();
        const std::uint32_t replayFrom = frame;
        const std::uint32_t forwardCheckpoints = rollback ? 3u : 1u;
        for (std::uint32_t checkpoint = 0; checkpoint < forwardCheckpoints; ++checkpoint)
        {
            Capture(journal, world, frame, scattered);
            frame += 2;
        }
        if (rollback)
        {
            std::uint32_t restored = 0;
            Check(journal.UndoTo(replayFrom + 1, &restored));
            Check(restored == replayFrom);
            for (std::uint32_t replay = restored; replay < frame; replay += 2)
                Capture(journal, world, replay, scattered);
        }
        if (discard)
            journal.DiscardBefore(frame);
        const auto end = std::chrono::steady_clock::now();
        times.push_back(std::chrono::duration<double, std::milli>(end - begin).count());
    }
    countAllocations = false;
    const std::size_t measuredAllocations = allocations;
    double sum = 0;
    for (double value : times)
        sum += value;
    std::sort(times.begin(), times.end());
    std::printf("{\"workload\":\"%s\",\"iterations\":%zu,\"snapshot_bytes\":%zu,"
                "\"allocations\":%zu,\"allocated_bytes\":%zu,\"mean_ms\":%.6f,"
                "\"p50_ms\":%.6f,\"p95_ms\":%.6f,\"max_ms\":%.6f}\n",
                name, ITERATIONS, world.size(), measuredAllocations, allocatedBytes,
                sum / times.size(), times[times.size() / 2],
                times[times.size() * 95 / 100], times.back());
    return measuredAllocations == 0;
}
}

int main(int argc, char **argv)
{
    bool zero = Run("forward-address-order", false, false, false);
    zero = Run("forward-scattered", false, false, true) && zero;
    zero = Run("rollback-six-ticks", true, false, true) && zero;
    zero = Run("discard-and-recapture", false, true, true) && zero;
    if (argc > 1 && std::strcmp(argv[1], "--require-zero-alloc") == 0 && !zero)
    {
        std::fputs("FAIL: warmed rollback storage allocated memory\n", stderr);
        return 1;
    }
    return 0;
}
