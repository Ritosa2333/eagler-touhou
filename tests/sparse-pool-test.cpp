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

struct CapturedRange
{
    std::size_t offset, bytes;
    bool operator==(const CapturedRange &other) const
    {
        return offset == other.offset && bytes == other.bytes;
    }
};

template<std::size_t N, class Touch>
bool CaptureFixture(Netplay::SparsePoolCapture<N> &capture, std::array<Object, N> &pool,
                    const std::uint8_t *occupied, bool packed, Touch touch)
{
    if (packed) return capture.CaptureOccupied(pool.data(), occupied, touch);
    return capture.Capture(pool.data(), [&](const Object &object) {
        return occupied[&object - pool.data()] != 0;
    }, touch);
}

template<std::size_t N>
void OrderedRuns()
{
    std::array<Object, N> pool{};
    // Every byte alignment is exercised, with nonzero guards beyond count.
    alignas(std::uint64_t) std::array<std::uint8_t, N + 7> storage{};
    Netplay::SparsePoolCapture<N> scalar, packed;
    std::uint32_t state = 0xc7351a29u + static_cast<std::uint32_t>(N);
    for (std::size_t alignment = 0; alignment < 8; ++alignment)
    {
        auto *occupied = storage.data() + alignment;
        for (unsigned pattern = 0; pattern < 9; ++pattern)
        {
            storage.fill(0x5a);
            for (std::size_t i = 0; i < N; ++i)
            {
                state = state * 1664525u + 1013904223u;
                const bool active = pattern == 0 ? false :
                    pattern == 1 ? true :
                    pattern == 2 ? i % 2 != 0 :
                    pattern == 3 ? i == N - 1 :
                    pattern == 4 ? (i < 2 || i == N / 2 || i == N - 1) :
                    (state >> 28) < (pattern == 5 ? 1u : pattern == 6 ? 4u : pattern == 7 ? 8u : 15u);
                // N=255's dense case contains all 255 nonzero byte values.
                occupied[i] = active ? static_cast<std::uint8_t>(1 + (i + alignment * 37 + pattern * 53) % 255) : 0;
            }
            for (std::size_t start = 0; start <= N; ++start)
            {
                auto expected = start;
                while (expected < N && occupied[expected] == 0) ++expected;
                assert(Netplay::NextOccupiedSlot(occupied, start, N) == expected);
            }

            std::vector<CapturedRange> scalarRuns, packedRuns;
            const auto record = [&](std::vector<CapturedRange> &runs, void *address, std::size_t bytes) {
                const auto offset = reinterpret_cast<std::uintptr_t>(address) -
                    reinterpret_cast<std::uintptr_t>(pool.data());
                assert(offset < sizeof(pool) && bytes <= sizeof(pool) - offset);
                runs.push_back({offset, bytes});
            };
            assert(CaptureFixture(scalar, pool, occupied, false, [&](void *p, std::size_t bytes) {
                record(scalarRuns, p, bytes); return true;
            }));
            assert(CaptureFixture(packed, pool, occupied, true, [&](void *p, std::size_t bytes) {
                record(packedRuns, p, bytes); return true;
            }));
            assert(scalarRuns == packedRuns); // includes exact order, start and byte length
            std::size_t liveCount = 0, capturedBytes = 0;
            for (std::size_t i = 0; i < N; ++i) liveCount += occupied[i] != 0;
            for (const auto &run : packedRuns) capturedBytes += run.bytes;
            assert(capturedBytes == liveCount * sizeof(Object));
            for (std::size_t i = 0; i < N; ++i)
            {
                unsigned scalarCalls = 0, packedCalls = 0;
                assert(scalar.TouchSlot(pool.data(), &pool[i], [&](void *, std::size_t) {
                    ++scalarCalls; return false;
                }) == (occupied[i] != 0));
                assert(packed.TouchSlot(pool.data(), &pool[i], [&](void *, std::size_t) {
                    ++packedCalls; return false;
                }) == (occupied[i] != 0));
                assert(scalarCalls == (occupied[i] ? 0u : 1u) && scalarCalls == packedCalls);
            }

            // Fail each run in a sparse layout after a successful capture. Only
            // completed runs may suppress first-write touches.
            if (pattern != 4 || alignment != 3) continue;
            const auto failureAndRetry = [&](auto &capture, bool usePacked) {
                for (std::size_t stop = 0; stop < scalarRuns.size(); ++stop)
                {
                    std::vector<CapturedRange> attempted;
                    assert(!CaptureFixture(capture, pool, occupied, usePacked, [&](void *p, std::size_t bytes) {
                        record(attempted, p, bytes); return attempted.size() <= stop;
                    }));
                    const std::vector<CapturedRange> prefix(scalarRuns.begin(), scalarRuns.begin() + stop + 1);
                    assert(attempted == prefix);
                    for (std::size_t i = 0; i < N; ++i)
                    {
                        bool covered = false;
                        for (std::size_t run = 0; run < stop; ++run)
                            covered |= i * sizeof(Object) >= scalarRuns[run].offset &&
                                i * sizeof(Object) - scalarRuns[run].offset < scalarRuns[run].bytes;
                        unsigned calls = 0;
                        const auto reject = [&](void *p, std::size_t bytes) {
                            ++calls; assert(p == &pool[i] && bytes == sizeof(Object)); return false;
                        };
                        assert(capture.TouchSlot(pool.data(), &pool[i], reject) == covered);
                        assert(capture.TouchSlot(pool.data(), &pool[i], reject) == covered);
                        assert(calls == (covered ? 0u : 2u));
                    }
                    std::vector<CapturedRange> retried;
                    assert(CaptureFixture(capture, pool, occupied, usePacked, [&](void *p, std::size_t bytes) {
                        record(retried, p, bytes); return true;
                    }));
                    assert(retried == scalarRuns);
                }
            };
            failureAndRetry(scalar, false);
            failureAndRetry(packed, true);
        }
    }
}

template<class Touch>
bool CaptureWorld(Netplay::SparsePoolCapture<Count> &capture, World &world, bool packed, Touch touch)
{
    if (!packed) return capture.Capture(world.data(), [](const Object &object) {
        return object.live != 0;
    }, touch);
    // The synthetic world's live field is its occupancy authority. Rebuild the
    // byte view at checkpoint entry, retaining all original journal mutations.
    std::array<std::uint8_t, Count> occupied{};
    for (std::size_t i = 0; i < Count; ++i)
        occupied[i] = world[i].live ? static_cast<std::uint8_t>(i + 1) : 0;
    return capture.CaptureOccupied(world.data(), occupied.data(), touch);
}

unsigned RunJournalSuite(bool packed)
{
    randomState = 0x98453219u;
    Netplay::SparsePoolCapture<Count> capture;
    Netplay::RollbackJournal journal;
    assert(journal.Reset({8, sizeof(World) + sizeof(Object), Count * 2}));
    const auto touch = [&](void *data, std::size_t bytes) { return journal.Touch(data, bytes); };
    World world{};
    for (auto &object : world) object.live = 1;
    const auto denseBefore = world;
    assert(journal.BeginFrame(0));
    assert(CaptureWorld(capture, world, packed, touch));
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
    assert(CaptureWorld(capture, world, packed, touch));
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
                if (!sub) assert(CaptureWorld(capture, world, packed, touch));
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
        assert(CaptureWorld(capture, world, packed, touch));
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
    assert(CaptureWorld(failureCapture, empty, packed, touch));
    unsigned attempts = 0;
    const auto reject = [&](void *, std::size_t) { ++attempts; return false; };
    assert(!failureCapture.TouchSlot(empty.data(), &empty[3], reject));
    assert(!failureCapture.TouchSlot(empty.data(), &empty[3], reject));
    assert(attempts == 2); // failure must not mark an uncaptured slot as safe
    return rewinds;
}
} // namespace

int main()
{
    assert(Netplay::NextOccupiedSlot(nullptr, 0, 0) == 0);
    OrderedRuns<1>(); OrderedRuns<7>(); OrderedRuns<8>(); OrderedRuns<9>();
    OrderedRuns<17>(); OrderedRuns<37>(); OrderedRuns<255>();
    unsigned rewinds = 0;
    for (unsigned mode = 0; mode < 2; ++mode) rewinds += RunJournalSuite(mode != 0);
    std::printf("sparse pool: PASS scalar/packed ordered runs, all byte alignments/nonzero values, dense/fragmented bytes, %u randomized rewinds, slot reuse and failure/retry checks\n", rewinds);
}
