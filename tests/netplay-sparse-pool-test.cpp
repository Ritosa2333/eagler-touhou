#include <eagler/netplay/SparsePoolCapture.hpp>
#include <eagler/netplay/RollbackJournal.hpp>

#include <array>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>

namespace
{
constexpr std::size_t Count = 128;
struct Object
{
    std::uint64_t live = 0;
    std::array<std::uint64_t, 15> state{};
};
using World = std::array<Object, Count>;
bool Same(const World &left, const World &right)
{
    return std::memcmp(left.data(), right.data(), sizeof(World)) == 0;
}
std::uint32_t randomState = 0x98453219u;
std::uint32_t Random()
{
    randomState = randomState * 1664525u + 1013904223u;
    return randomState;
}
}

int main()
{
    Netplay::SparsePoolCapture<Count> capture;
    Netplay::RollbackJournal journal;
    assert(journal.Reset({8, sizeof(World) + sizeof(Object), Count * 2}));
    const auto touch = [&](void *data, std::size_t bytes) { return journal.Touch(data, bytes); };
    const auto live = [](const Object &object) { return object.live != 0; };
    World world{};
    for (auto &object : world) object.live = 1;
    const auto denseBefore = world;
    assert(journal.BeginFrame(0));
    assert(capture.Capture(world.data(), live, touch));
    assert(journal.BlocksForFrame(0) == 1);
    assert(journal.BytesForFrame(0) == sizeof(World));
    for (auto &object : world)
    {
        // Recycling inside an already-captured run must not submit overlapping
        // subranges to the strict journal, or overwrite checkpoint-start bytes.
        object = {};
        assert(capture.TouchSlot(world.data(), &object, touch));
        object.live = 1;
        object.state[0] = Random();
    }
    assert(journal.EndFrame());
    assert(journal.BeginFrame(1, true));
    assert(capture.TouchSlot(world.data(), &world[0], touch));
    world[0].state[14] = 123;
    assert(journal.EndFrame());
    assert(journal.UndoTo(1) && Same(world, denseBefore));

    for (std::size_t i = 0; i < Count; ++i) world[i].live = i % 2;
    auto before = world;
    assert(journal.BeginFrame(0));
    assert(capture.Capture(world.data(), live, touch));
    assert(journal.BlocksForFrame(0) == Count / 2);
    assert(journal.BytesForFrame(0) == sizeof(World) / 2);
    for (std::size_t i = 0; i < Count; ++i)
    {
        assert(capture.TouchSlot(world.data(), &world[i], touch));
        world[i].live = 1;
        world[i].state[7] = i + 300;
    }
    assert(journal.BlocksForFrame(0) == Count);
    assert(capture.TouchSlot(world.data(), static_cast<Object *>(nullptr), touch));
    Object outside{};
    assert(capture.TouchSlot(world.data(), &outside, touch));
    outside.state[0] = 991;
    assert(journal.EndFrame());
    assert(journal.UndoTo(0) && Same(world, before) && outside.state[0] == 0);

    // Many independent histories with dense/fragmented layouts, spawns,
    // despawns, reuse, checkpoint extension, ring eviction and rewind to any
    // retained checkpoint. Compare ALL bytes, not only the active objects.
    unsigned rewinds = 0;
    for (unsigned epoch = 0; epoch < 400; ++epoch)
    {
        journal.DiscardBefore(0xffffffffu);
        std::vector<World> reference;
        const unsigned base = epoch * 100;
        for (unsigned checkpoint = 0; checkpoint < 12; ++checkpoint)
        {
            reference.push_back(world);
            const unsigned frame = base + checkpoint * 2;
            for (unsigned sub = 0; sub < 2; ++sub)
            {
                assert(journal.BeginFrame(frame + sub, sub != 0));
                if (!sub) assert(capture.Capture(world.data(), live, touch));
                for (auto &object : world)
                {
                    if (object.live)
                    {
                        for (auto &value : object.state) value = value * 3 + Random();
                        if ((Random() & 15u) == 0) object = {};
                    }
                }
                for (unsigned event = 0; event < 35; ++event)
                {
                    auto &slot = world[Random() % Count];
                    assert(capture.TouchSlot(world.data(), &slot, touch));
                    slot.live = 1;
                    for (auto &value : slot.state) value = Random();
                }
                assert(journal.EndFrame());
            }
        }
        const unsigned checkpoint = 4 + Random() % 8;
        unsigned restored = 0;
        assert(journal.UndoTo(base + checkpoint * 2 + 1, &restored));
        assert(restored == base + checkpoint * 2);
        assert(Same(world, reference[checkpoint]));
        ++rewinds;
        // A fresh checkpoint must reset metadata from the discarded future.
        before = world;
        assert(journal.BeginFrame(restored));
        assert(capture.Capture(world.data(), live, touch));
        for (auto &slot : world)
        {
            assert(capture.TouchSlot(world.data(), &slot, touch));
            slot.live = 1;
            slot.state[5] = 99;
        }
        assert(journal.EndFrame());
        assert(journal.UndoTo(restored) && Same(world, before));
    }
    Netplay::SparsePoolCapture<Count> failureCapture;
    World empty{};
    assert(failureCapture.Capture(empty.data(), live, touch));
    unsigned attempts = 0;
    const auto reject = [&](void *, std::size_t) { ++attempts; return false; };
    assert(!failureCapture.TouchSlot(empty.data(), &empty[3], reject));
    assert(!failureCapture.TouchSlot(empty.data(), &empty[3], reject));
    assert(attempts == 2); // failure must not mark an uncaptured slot as safe
    std::printf("sparse pool: PASS dense/fragmented bytes, %u randomized rewinds, slot reuse and failure checks\n", rewinds);
}
