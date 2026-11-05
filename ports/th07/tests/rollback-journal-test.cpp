#include <eagler/netplay/RollbackJournal.hpp>

#include <array>
#include <cassert>
#include <cstdio>
#include <limits>
#include <vector>

using Netplay::RollbackJournal;
using Netplay::RollbackJournalConfig;

static RollbackJournal MakeJournal(std::size_t frames = 4,
                                   std::size_t bytes = 1024,
                                   std::size_t blocks = 16)
{
    RollbackJournal journal;
    assert(journal.Reset(RollbackJournalConfig{frames, bytes, blocks}));
    return journal;
}

static void TestFirstWriteWins()
{
    auto journal = MakeJournal();
    int value = 10;
    assert(journal.BeginFrame(5));
    assert(journal.Touch(&value, sizeof(value)));
    value = 20;
    assert(journal.Touch(&value, sizeof(value)));
    value = 30;
    assert(journal.EndFrame());
    assert(journal.BlocksForFrame(5) == 1);
    assert(journal.UndoTo(5));
    assert(value == 10);
}

static void TestMultipleFrames()
{
    auto journal = MakeJournal();
    int value = 1;
    assert(journal.BeginFrame(10));
    assert(journal.Touch(&value, sizeof(value)));
    value = 2;
    assert(journal.EndFrame());
    assert(journal.BeginFrame(11));
    assert(journal.Touch(&value, sizeof(value)));
    value = 3;
    assert(journal.EndFrame());
    assert(journal.BeginFrame(12));
    assert(journal.Touch(&value, sizeof(value)));
    value = 4;
    assert(journal.EndFrame());

    assert(journal.UndoTo(11));
    assert(value == 2);
    assert(journal.FrameCount() == 1);
}

static void TestExtendedCheckpointAcrossTwoFrames()
{
    auto journal = MakeJournal();
    int persistent = 10;
    int secondFrameOnly = 40;

    assert(journal.BeginFrame(100));
    assert(journal.Touch(&persistent, sizeof(persistent)));
    persistent = 20;
    assert(journal.EndFrame());

    // Frame 101 belongs to the same checkpoint. The persistent object must
    // keep its pre-100 value, while an object first touched on frame 101 must
    // still be captured before its second-frame mutation.
    assert(journal.BeginFrame(101, true));
    assert(journal.Touch(&persistent, sizeof(persistent)));
    persistent = 30;
    assert(journal.Touch(&secondFrameOnly, sizeof(secondFrameOnly)));
    secondFrameOnly = 50;
    assert(journal.EndFrame());

    assert(journal.FrameCount() == 1);
    assert(journal.BlocksForFrame(100) == 2);
    assert(journal.BlocksForFrame(101) == 2);
    std::uint32_t replayFrom = 999;
    assert(journal.UndoTo(101, &replayFrom));
    assert(replayFrom == 100);
    assert(persistent == 10);
    assert(secondFrameOnly == 40);
}

static void TestDiscardKeepsCheckpointContainingBoundary()
{
    auto journal = MakeJournal();
    int value = 1;
    assert(journal.BeginFrame(200));
    assert(journal.Touch(&value, sizeof(value)));
    value = 2;
    assert(journal.EndFrame());
    assert(journal.BeginFrame(201, true));
    value = 3;
    assert(journal.EndFrame());
    assert(journal.BeginFrame(202));
    assert(journal.Touch(&value, sizeof(value)));
    value = 4;
    assert(journal.EndFrame());

    journal.DiscardBefore(201);
    assert(journal.FrameCount() == 2);
    std::uint32_t replayFrom = 999;
    assert(journal.UndoTo(201, &replayFrom));
    assert(replayFrom == 200);
    assert(value == 1);
}

static void TestSlotReuseInOneFrame()
{
    auto journal = MakeJournal();
    struct Slot
    {
        int active;
        int generation;
        std::array<int, 4> stale;
    } slot{0, 7, {1, 2, 3, 4}};

    const Slot original = slot;
    assert(journal.BeginFrame(20));
    assert(journal.Touch(&slot, sizeof(slot)));
    slot = Slot{1, 8, {9, 9, 9, 9}};
    slot.active = 0;
    assert(journal.Touch(&slot, sizeof(slot)));
    slot = Slot{1, 9, {8, 8, 8, 8}};
    assert(journal.EndFrame());
    assert(journal.UndoTo(20));
    assert(slot.active == original.active);
    assert(slot.generation == original.generation);
    assert(slot.stale == original.stale);
}

static void TestRejectsPartialOverlap()
{
    auto journal = MakeJournal();
    std::array<unsigned char, 32> bytes{};
    assert(journal.BeginFrame(30));
    assert(journal.Touch(bytes.data(), 16));
    assert(!journal.Touch(bytes.data() + 8, 16));
    assert(journal.Failed());
}

static void TestBoundsAndEviction()
{
    auto journal = MakeJournal(2, 8, 2);
    int a = 1;
    int b = 2;
    for (std::uint32_t frame = 1; frame <= 3; ++frame)
    {
        assert(journal.BeginFrame(frame));
        assert(journal.Touch(&a, sizeof(a)));
        a++;
        assert(journal.EndFrame());
    }
    assert(journal.FrameCount() == 2);
    assert(!journal.UndoTo(1));

    auto limited = MakeJournal(2, sizeof(int), 1);
    assert(limited.BeginFrame(1));
    assert(limited.Touch(&a, sizeof(a)));
    assert(!limited.Touch(&b, sizeof(b)));
    assert(limited.Failed());
}

static void TestAllOverlapDirectionsAndAdjacency()
{
    const std::array<std::array<std::size_t, 2>, 7> overlaps{{
        {{0, 20}}, {{20, 4}}, {{16, 8}}, {{16, 24}},
        {{32, 36}}, {{0, 100}}, {{72, 16}}
    }};
    for (const auto &range : overlaps)
    {
        auto journal = MakeJournal();
        std::array<unsigned char, 128> bytes{};
        assert(journal.BeginFrame(1));
        assert(journal.Touch(bytes.data() + 64, 16));
        assert(journal.Touch(bytes.data() + 16, 16));
        assert(!journal.Touch(bytes.data() + range[0], range[1]));
        assert(journal.Failed());
        assert(!journal.EndFrame());
    }
    auto journal = MakeJournal();
    std::array<unsigned char, 128> bytes{};
    assert(journal.BeginFrame(1));
    assert(journal.Touch(bytes.data() + 16, 16));
    assert(journal.Touch(bytes.data(), 16));
    assert(journal.Touch(bytes.data() + 32, 16));
    assert(journal.EndFrame());
    assert(journal.BlocksForFrame(1) == 3);
}

static void TestAddressOverflowIsRejectedBeforeCopy()
{
    auto journal = MakeJournal();
    assert(journal.BeginFrame(1));
    auto *invalid = reinterpret_cast<void *>(std::numeric_limits<std::uintptr_t>::max() - 3);
    assert(!journal.Touch(invalid, 8));
    assert(journal.Failed());
}

static void TestOrderedAndScatteredIndex()
{
    constexpr std::size_t count = 4093;
    for (unsigned order = 0; order < 4; ++order)
    {
        auto journal = MakeJournal(2, count * sizeof(std::uint32_t), count);
        std::array<std::uint32_t, count> values{};
        for (std::size_t index = 0; index < count; ++index)
            values[index] = static_cast<std::uint32_t>(index);
        const auto original = values;
        assert(journal.BeginFrame(1));
        for (std::size_t index = 0; index < count; ++index)
        {
            const std::size_t slot = order == 0 ? index : order == 1 ? count - 1 - index
                : order == 2 ? (index * 127) % count
                : (index % 2 == 0 ? index / 2 : count - 1 - index / 2);
            assert(journal.Touch(&values[slot], sizeof(values[slot])));
            values[slot] += 1000;
        }
        // Duplicate first-write touches are valid even at both capacity limits.
        for (std::size_t index = count; index-- > 0;)
        {
            assert(journal.Touch(&values[index], sizeof(values[index])));
            ++values[index];
        }
        assert(journal.EndFrame());
        assert(journal.BlocksForFrame(1) == count);
        assert(journal.BytesForFrame(1) == sizeof(values));
        assert(journal.UndoTo(1));
        assert(values == original);
    }
}

static void TestArenaGrowthPreservesCapturedBytes()
{
    constexpr std::size_t blockSize = 96 * 1024;
    constexpr std::size_t count = 5;
    auto journal = MakeJournal(2, blockSize * count, count);
    std::vector<unsigned char> world(blockSize * count);
    for (std::size_t index = 0; index < world.size(); ++index)
        world[index] = static_cast<unsigned char>((index * 73 + index / 997) & 255);
    const auto before = world;
    assert(journal.BeginFrame(7));
    for (std::size_t slot : {3u, 0u, 4u})
    {
        auto *address = world.data() + slot * blockSize;
        assert(journal.Touch(address, blockSize));
        for (std::size_t byte = 0; byte < blockSize; ++byte)
            address[byte] ^= 0x5a;
    }
    assert(journal.EndFrame());
    assert(journal.BeginFrame(8, true));
    for (std::size_t slot : {2u, 1u, 3u})
    {
        auto *address = world.data() + slot * blockSize;
        assert(journal.Touch(address, blockSize));
        for (std::size_t byte = 0; byte < blockSize; ++byte)
            address[byte] ^= 0xa5;
    }
    assert(journal.EndFrame());
    assert(journal.BytesForFrame(8) == world.size());
    std::uint32_t restored = 0;
    assert(journal.UndoTo(8, &restored));
    assert(restored == 7 && world == before);
    // Reuse the arena for unrelated addresses, then discard the whole ring.
    int other = 42;
    assert(journal.BeginFrame(7));
    assert(journal.Touch(&other, sizeof(other)));
    other = 11;
    assert(journal.EndFrame());
    assert(journal.UndoTo(7));
    assert(other == 42 && world == before);
    journal.Clear();
    assert(journal.FrameCount() == 0 && !journal.BeginFrame(0));
    assert(!journal.Reset({0, 1024, 16}));
    assert(journal.Reset({2, 1024, 16}));
    assert(journal.BeginFrame(0) && journal.EndFrame());
}

static void TestSnapshotFloatPairPatch()
{
    struct State { float x, y; int marker; } state{1.0f, 2.0f, 7};
    auto journal = MakeJournal(4, 1024, 16);
    for (std::uint32_t frame = 10; frame <= 12; ++frame)
    {
        assert(journal.BeginFrame(frame));
        assert(journal.Touch(&state, sizeof(state)));
        state.x += 1.0f;
        state.y += 1.0f;
        ++state.marker;
        assert(journal.EndFrame());
    }
    assert(journal.AddFloatPairToSnapshots(10, 12, &state.x, 5.0f, -1.0f));
    assert(journal.UndoTo(12));
    assert(state.x == 8.0f && state.y == 3.0f && state.marker == 9);
    assert(journal.UndoTo(10));
    assert(state.x == 1.0f && state.y == 2.0f && state.marker == 7);
}

static void TestRandomizedHistoryAgainstFullCopyReference()
{
    using World = std::array<std::uint32_t, 96>;
    struct Reference
    {
        std::uint32_t first;
        std::uint32_t last;
        World before;
    };
    constexpr std::size_t capacity = 5;
    auto journal = MakeJournal(capacity, sizeof(World), World{}.size());
    World world{};
    std::vector<Reference> reference;
    std::uint32_t nextFrame = 0;
    std::uint32_t randomState = 0x7135abcd;
    const auto random = [&]() {
        randomState = randomState * 1664525u + 1013904223u;
        return randomState;
    };

    for (unsigned step = 0; step < 2000; ++step)
    {
        const unsigned action = random() % 7;
        if (!reference.empty() && action < 2)
        {
            const std::size_t target = random() % reference.size();
            const Reference saved = reference[target];
            const auto requested = saved.first + random() % (saved.last - saved.first + 1);
            std::uint32_t restored = 0;
            assert(journal.UndoTo(requested, &restored));
            assert(restored == saved.first);
            assert(world == saved.before);
            nextFrame = restored;
            reference.erase(reference.begin() + target, reference.end());
        }
        else if (!reference.empty() && action == 2)
        {
            const auto boundary = reference[random() % reference.size()].last;
            journal.DiscardBefore(boundary);
            while (!reference.empty() && reference.front().last < boundary)
                reference.erase(reference.begin());
        }
        else
        {
            if (reference.size() == capacity)
                reference.erase(reference.begin());
            reference.push_back({nextFrame, nextFrame, world});
            const unsigned ticks = 1 + random() % 3;
            for (unsigned tick = 0; tick < ticks; ++tick)
            {
                assert(journal.BeginFrame(nextFrame, tick != 0));
                reference.back().last = nextFrame++;
                for (unsigned write = 0; write < 137; ++write)
                {
                    const std::size_t slot = random() % world.size();
                    assert(journal.Touch(&world[slot], sizeof(world[slot])));
                    world[slot] ^= random();
                }
                assert(journal.EndFrame());
            }
        }
        assert(journal.FrameCount() == reference.size());
        for (const auto &record : reference)
        {
            assert(journal.BytesForFrame(record.first) == journal.BytesForFrame(record.last));
            assert(journal.BlocksForFrame(record.first) <= world.size());
        }
    }
    if (!reference.empty())
    {
        assert(journal.UndoTo(reference.front().first));
        assert(world == reference.front().before);
    }
}

int main()
{
    TestFirstWriteWins();
    TestMultipleFrames();
    TestExtendedCheckpointAcrossTwoFrames();
    TestDiscardKeepsCheckpointContainingBoundary();
    TestSlotReuseInOneFrame();
    TestRejectsPartialOverlap();
    TestBoundsAndEviction();
    TestAllOverlapDirectionsAndAdjacency();
    TestAddressOverflowIsRejectedBeforeCopy();
    TestOrderedAndScatteredIndex();
    TestArenaGrowthPreservesCapturedBytes();
    TestSnapshotFloatPairPatch();
    TestRandomizedHistoryAgainstFullCopyReference();
    std::puts("TH07 rollback journal: PASS");
}
