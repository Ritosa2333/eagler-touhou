#include "netplay/RollbackJournal.hpp"

#include <array>
#include <cassert>
#include <cstdio>

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

int main()
{
    TestFirstWriteWins();
    TestMultipleFrames();
    TestExtendedCheckpointAcrossTwoFrames();
    TestDiscardKeepsCheckpointContainingBoundary();
    TestSlotReuseInOneFrame();
    TestRejectsPartialOverlap();
    TestBoundsAndEviction();
    std::puts("TH06 rollback journal: PASS");
}
