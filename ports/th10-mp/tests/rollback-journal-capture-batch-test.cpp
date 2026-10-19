#include <eagler/netplay/RollbackJournal.hpp>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <vector>

// The oracle is the complete source byte array, including untouched gaps.
// Run this same source on native and WASM; it needs no title ABI or resources.
namespace {
using Netplay::RollbackJournal;
using Bytes = std::vector<std::uint8_t>;
std::uint64_t checks = 0;

[[noreturn]] void fail(const char* expression, int line) {
    std::fprintf(stderr, "rollback journal capture batch: line %d: %s\n", line, expression);
    std::abort();
}
#define CHECK(expression) do { ++checks; if (!(expression)) fail(#expression, __LINE__); } while (false)

struct Range {
    std::size_t offset;
    std::size_t bytes;
};

Bytes seeded(std::size_t bytes, std::uint32_t seed) {
    Bytes result(bytes);
    for (auto& byte : result) {
        seed = seed * 1664525u + 1013904223u;
        byte = static_cast<std::uint8_t>(seed >> 24);
    }
    return result;
}

void change(Bytes& state, Range range, std::uint8_t value) {
    CHECK(range.offset <= state.size() && range.bytes <= state.size() - range.offset);
    std::fill(state.begin() + range.offset, state.begin() + range.offset + range.bytes, value);
}

void configure(RollbackJournal& journal, bool fast, bool coalesce,
               std::size_t frames = 3, std::size_t bytes = 16384, std::size_t blocks = 16) {
    CHECK(journal.Reset({frames, bytes, blocks, fast, coalesce}));
}

void closed(const RollbackJournal& journal, bool fast) {
    CHECK(!journal.IsReadOnlyCaptureOpen());
    CHECK(journal.PendingReadOnlyCaptureBlocks() == 0);
    CHECK(journal.PendingReadOnlyCaptureBytes() == 0);
    const auto& stats = journal.ReadOnlyCaptureStats();
    CHECK(stats.deferredBlocks == stats.copiedBlocks);
    CHECK(stats.deferredBytes == stats.copiedBytes);
#if defined(__EMSCRIPTEN__) && !defined(__wasm64__)
    CHECK(stats.batchImports == (fast ? stats.flushes : 0));
#else
    (void)fast;
    CHECK(stats.batchImports == 0);
#endif
    CHECK((stats.flushes == 0) == (stats.maxBlocksPerFlush == 0));
}

// Compare immediate and deferred journals while modifying only bytes whose
// first-write protection has completed. Existing pre-window blocks must not
// be recopied from their already-mutated sources by the batch suffix walk.
void window_and_immediate_writes(bool fast, bool coalesce) {
    Bytes reference = seeded(8192, 0x163f8a41u), batched = reference;
    const Bytes original = reference;
    RollbackJournal scalar, batch;
    configure(scalar, fast, coalesce);
    configure(batch, fast, coalesce);
    CHECK(!batch.BeginReadOnlyCapture());
    CHECK(!batch.EndReadOnlyCapture());
    closed(batch, fast);
    CHECK(scalar.BeginFrame(0) && batch.BeginFrame(0));
    const Range prefix{0, 1100};
    CHECK(scalar.Touch(reference.data() + prefix.offset, prefix.bytes));
    CHECK(batch.Touch(batched.data() + prefix.offset, prefix.bytes));
    change(reference, prefix, 0x81);
    change(batched, prefix, 0x81);
    CHECK(batch.BeginReadOnlyCapture());
    const Bytes window_start = batched;
    const std::array<Range, 4> ranges{{
        {1537, 1024}, {4501, 1023}, {3073, 1025}, {7603, 37}
    }};
    for (const auto range : ranges) {
        CHECK(scalar.Touch(reference.data() + range.offset, range.bytes));
        CHECK(batch.Touch(batched.data() + range.offset, range.bytes));
    }
    CHECK(scalar.Touch(reference.data(), prefix.bytes));
    CHECK(batch.Touch(batched.data(), prefix.bytes));
    CHECK(scalar.Touch(reference.data() + ranges[0].offset, ranges[0].bytes));
    CHECK(batch.Touch(batched.data() + ranges[0].offset, ranges[0].bytes));
    CHECK(reference == window_start && batched == window_start);
    CHECK(batch.BytesForFrame(0) == scalar.BytesForFrame(0));
    CHECK(batch.BlocksForFrame(0) == scalar.BlocksForFrame(0));
    CHECK(batch.BlocksForFrame(0) == 5);
    CHECK(batch.PendingReadOnlyCaptureBlocks() == 2);
    CHECK(batch.PendingReadOnlyCaptureBytes() == 2049);
    CHECK(batch.ReadOnlyCaptureStats().copiedBlocks == 0);
    CHECK(!batch.BeginReadOnlyCapture()); // Does not replace the open window.
    CHECK(batch.PendingReadOnlyCaptureBlocks() == 2);
    CHECK(batch.EndReadOnlyCapture());
    closed(batch, fast);
    CHECK(batch.ReadOnlyCaptureStats().deferredBlocks == 2);
    CHECK(batch.ReadOnlyCaptureStats().deferredBytes == 2049);
    CHECK(batch.ReadOnlyCaptureStats().flushes == 1);
    CHECK(batch.ReadOnlyCaptureStats().maxBlocksPerFlush == 2);
    CHECK(!batch.EndReadOnlyCapture());

    for (std::size_t i = 0; i < ranges.size(); ++i) {
        change(reference, ranges[i], static_cast<std::uint8_t>(0x20 + i));
        change(batched, ranges[i], static_cast<std::uint8_t>(0x20 + i));
    }
    const Range outside{5701, 1500};
    CHECK(scalar.Touch(reference.data() + outside.offset, outside.bytes));
    CHECK(batch.Touch(batched.data() + outside.offset, outside.bytes));
    change(reference, outside, 0xc9);
    change(batched, outside, 0xc9);
    CHECK(scalar.Touch(reference.data() + outside.offset, outside.bytes));
    CHECK(batch.Touch(batched.data() + outside.offset, outside.bytes));
    CHECK(batch.ReadOnlyCaptureStats().deferredBlocks == 2);
    CHECK(reference == batched);
    CHECK(scalar.EndFrame() && batch.EndFrame());
    CHECK(scalar.UndoTo(0) && batch.UndoTo(0));
    CHECK(reference == original && batched == original);
    CHECK(scalar.ReadOnlyCaptureStats().scopes == 0);
    closed(scalar, fast);
    closed(batch, fast);
}

// Empty/small-only scopes must not issue an empty batch. EndFrame and Clear
// close an otherwise still-read-only window; Reset also resets its counters.
void scope_lifecycle(bool fast, bool coalesce) {
    Bytes state = seeded(8192, 0x84917b23u);
    const Bytes original = state;
    RollbackJournal journal;
    configure(journal, fast, coalesce);
    CHECK(journal.BeginFrame(1));
    CHECK(journal.BeginReadOnlyCapture());
    CHECK(journal.EndReadOnlyCapture());
    CHECK(journal.BeginReadOnlyCapture());
    CHECK(journal.Touch(state.data() + 1, 1023));
    CHECK(journal.PendingReadOnlyCaptureBlocks() == 0);
    CHECK(journal.EndReadOnlyCapture());
    change(state, {1, 1023}, 0x34);
    CHECK(journal.BeginReadOnlyCapture());
    CHECK(journal.Touch(state.data() + 2049, 1024));
    CHECK(journal.PendingReadOnlyCaptureBlocks() == 1);
    CHECK(!journal.EndFrame()); // Sealing must not implicitly close a capture window.
    CHECK(journal.EndReadOnlyCapture());
    CHECK(journal.EndFrame());
    closed(journal, fast);
    CHECK(journal.ReadOnlyCaptureStats().scopes == 3);
    CHECK(journal.ReadOnlyCaptureStats().flushes == 1);
    change(state, {2049, 1024}, 0x56);
    CHECK(journal.UndoTo(1));
    CHECK(state == original);

    CHECK(journal.BeginFrame(2));
    CHECK(journal.BeginReadOnlyCapture());
    CHECK(journal.Touch(state.data() + 1, 2048));
    CHECK(journal.PendingReadOnlyCaptureBlocks() == 1);
    journal.Clear();
    closed(journal, fast);
    CHECK(journal.FrameCount() == 0 && !journal.IsFrameOpen());
    CHECK(journal.ReadOnlyCaptureStats().scopes == 4);
    CHECK(journal.ReadOnlyCaptureStats().deferredBlocks == 2);
    configure(journal, fast, coalesce);
    closed(journal, fast);
    const auto& stats = journal.ReadOnlyCaptureStats();
    CHECK(stats.scopes == 0 && stats.deferredBlocks == 0 && stats.deferredBytes == 0);
    CHECK(stats.flushes == 0 && stats.batchImports == 0);
}

void extended_checkpoint(bool batch, bool fast, bool coalesce) {
    Bytes state = seeded(4096, 0x5a622e19u);
    const Bytes original = state;
    RollbackJournal journal;
    configure(journal, fast, coalesce);
    CHECK(journal.BeginFrame(10));
    if (batch) CHECK(journal.BeginReadOnlyCapture());
    CHECK(journal.Touch(state.data() + 17, 1024));
    if (batch) CHECK(journal.EndReadOnlyCapture());
    change(state, {17, 1024}, 0x41);
    CHECK(journal.EndFrame());
    CHECK(journal.BeginFrame(11, true));
    if (batch) CHECK(journal.BeginReadOnlyCapture());
    CHECK(journal.Touch(state.data() + 17, 1024)); // Keep the original first write.
    CHECK(journal.Touch(state.data() + 2049, 1025));
    if (batch) CHECK(journal.EndReadOnlyCapture());
    change(state, {17, 1024}, 0x52);
    change(state, {2049, 1025}, 0x63);
    CHECK(journal.EndFrame());
    CHECK(journal.FrameCount() == 1);
    CHECK(journal.BytesForFrame(10) == 2049 && journal.BytesForFrame(11) == 2049);
    CHECK(journal.BlocksForFrame(10) == 2 && journal.BlocksForFrame(11) == 2);
    std::uint32_t restored = 999;
    CHECK(journal.UndoTo(11, &restored));
    CHECK(restored == 10 && state == original);
    CHECK(journal.ReadOnlyCaptureStats().deferredBlocks == (batch ? 2u : 0u));
    closed(journal, fast);
}

// Force 256 KiB -> 512 KiB -> final-capacity growth while large copies are
// pending, then wrap the ring, undo, resimulate, discard and reuse the arenas.
// Full-byte snapshots serve as the independent oracle for every undo.
void growth_ring_and_reuse(bool batch, bool fast, bool coalesce) {
    constexpr std::size_t size = 720 * 1024 + 31;
    constexpr std::size_t a = 140 * 1024 + 3, b = 160 * 1024 + 5, c = 310 * 1024 + 7;
    const std::array<Range, 5> ranges{{
        {0, 3}, {3, a}, {3 + a, b}, {3 + a + b, c},
        {3 + a + b + c, size - 3 - a - b - c}
    }};
    // This order also makes storage insertion order differ from source order.
    constexpr std::array<std::size_t, 5> order{{1, 0, 2, 4, 3}};
    Bytes state = seeded(size, 0xad90356fu);
    std::array<Bytes, 3> before;
    RollbackJournal journal;
    configure(journal, fast, coalesce, 3, size, 5);
    const auto forward = [&](std::uint32_t frame, std::uint8_t salt) {
        before[frame % before.size()] = state;
        CHECK(journal.BeginFrame(frame));
        if (batch) CHECK(journal.BeginReadOnlyCapture());
        for (const auto index : order) {
            const auto range = ranges[index];
            CHECK(journal.Touch(state.data() + range.offset, range.bytes));
        }
        // The accepted byte/block limits are full; an exact duplicate must
        // still succeed before either limit check and must not enqueue again.
        CHECK(journal.Touch(state.data() + ranges[2].offset, ranges[2].bytes));
        CHECK(journal.BytesForFrame(frame) == size);
        CHECK(journal.BlocksForFrame(frame) == ranges.size());
        if (batch) CHECK(journal.EndReadOnlyCapture());
        closed(journal, fast);
        CHECK(state == before[frame % before.size()]);
        for (std::size_t i = 0; i < state.size(); ++i)
            state[i] ^= static_cast<std::uint8_t>(salt + (i * 13u + (i >> 8)));
        CHECK(journal.EndFrame());
    };
    for (std::uint32_t frame = 0; frame < 9; ++frame)
        forward(frame, static_cast<std::uint8_t>(frame + 31));
    CHECK(journal.FrameCount() == 3);
    CHECK(journal.BytesForFrame(5) == 0 && journal.BytesForFrame(6) == size);
    CHECK(journal.ArenaGrowths() == 9); // Three allocations in each of three slots.
    const auto allocations = journal.ArenaGrowths();
    const auto capture_bytes = journal.ReadOnlyCaptureStats().copiedBytes;
    const Bytes expected_seven = before[1];
    std::uint32_t restored = 999;
    CHECK(journal.UndoTo(7, &restored));
    CHECK(restored == 7 && state == expected_seven);
    CHECK(journal.FrameCount() == 1 && journal.BytesForFrame(7) == 0);
    CHECK(journal.ReadOnlyCaptureStats().copiedBytes == capture_bytes); // Undo is separate.

    for (std::uint32_t frame = 7; frame < 10; ++frame)
        forward(frame, static_cast<std::uint8_t>(frame + 113));
    CHECK(journal.ArenaGrowths() == allocations);
    journal.DiscardBefore(8);
    CHECK(journal.FrameCount() == 2 && journal.BytesForFrame(7) == 0);
    const Bytes expected_eight = before[2];
    CHECK(journal.UndoTo(8, &restored));
    CHECK(restored == 8 && state == expected_eight && journal.FrameCount() == 0);
    CHECK(journal.ArenaGrowths() == allocations);
    const auto& stats = journal.ReadOnlyCaptureStats();
    CHECK(stats.scopes == (batch ? 12u : 0u));
    CHECK(stats.deferredBlocks == (batch ? 48u : 0u));
    CHECK(stats.maxBlocksPerFlush == (batch ? 4u : 0u));
    closed(journal, fast);
}

// These use only valid memory or pre-copy integer rejection, so the same
// cases run without deliberately invoking undefined native memory accesses.
void rejected_touch_prefixes(bool fast, bool coalesce) {
    enum Failure { Null, Zero, OverlapRight, OverlapLeft, ByteLimit, BlockLimit, AddressOverflow };
    for (unsigned scenario = Null; scenario <= AddressOverflow; ++scenario) {
        Bytes reference = seeded(8192, 0xea012893u + scenario), state = reference;
        const Bytes original = state;
        RollbackJournal scalar, batch;
        const std::size_t byte_limit = scenario == ByteLimit ? 1024 : 8192;
        const std::size_t block_limit = scenario == BlockLimit ? 1 : 8;
        configure(scalar, fast, coalesce, 2, byte_limit, block_limit);
        configure(batch, fast, coalesce, 2, byte_limit, block_limit);
        CHECK(scalar.BeginFrame(5) && batch.BeginFrame(5));
        CHECK(batch.BeginReadOnlyCapture());
        CHECK(scalar.Touch(reference.data() + 2048, 1024));
        CHECK(batch.Touch(state.data() + 2048, 1024));
        CHECK(scalar.Touch(reference.data() + 2048, 1024));
        CHECK(batch.Touch(state.data() + 2048, 1024));
        CHECK(batch.PendingReadOnlyCaptureBlocks() == 1);
        const auto reject = [&](RollbackJournal& journal, Bytes& bytes) {
            switch (scenario) {
            case Null: return journal.Touch(nullptr, 1);
            case Zero: return journal.Touch(bytes.data() + 4096, 0);
            case OverlapRight: return journal.Touch(bytes.data() + 2049, 1);
            case OverlapLeft: return journal.Touch(bytes.data() + 2047, 1025);
            case ByteLimit: case BlockLimit: return journal.Touch(bytes.data() + 4096, 1);
            default:
                return journal.Touch(reinterpret_cast<void*>(
                    std::numeric_limits<std::uintptr_t>::max() - 4), 16);
            }
        };
        CHECK(!reject(scalar, reference));
        CHECK(!reject(batch, state));
        CHECK(batch.IsReadOnlyCaptureOpen());
        CHECK(batch.PendingReadOnlyCaptureBlocks() == 0);
        CHECK(batch.PendingReadOnlyCaptureBytes() == 0);
        CHECK(batch.ReadOnlyCaptureStats().copiedBlocks == 1);
        CHECK(batch.ReadOnlyCaptureStats().copiedBytes == 1024);
        const bool failed = scenario >= OverlapRight;
        CHECK(scalar.Failed() == failed && batch.Failed() == failed);
        CHECK(batch.EndReadOnlyCapture() == !failed);
        CHECK(batch.BlocksForFrame(5) == 1 && scalar.BlocksForFrame(5) == 1);
        CHECK(batch.BytesForFrame(5) == 1024 && scalar.BytesForFrame(5) == 1024);
        closed(batch, fast);
        if (!failed) {
            change(reference, {2048, 1024}, 0x7a);
            change(state, {2048, 1024}, 0x7a);
            CHECK(scalar.EndFrame() && batch.EndFrame());
            CHECK(scalar.UndoTo(5) && batch.UndoTo(5));
            CHECK(reference == original && state == original);
        } else {
            CHECK(!scalar.EndFrame() && !batch.EndFrame());
            CHECK(!scalar.UndoTo(5) && !batch.UndoTo(5));
            CHECK(!batch.BeginReadOnlyCapture());
            CHECK(state == original);
        }
        // Reset after either a recoverable rejection or a latched failure.
        configure(batch, fast, coalesce);
        CHECK(batch.ReadOnlyCaptureStats().scopes == 0);
        CHECK(batch.BeginFrame(6) && batch.BeginReadOnlyCapture());
        CHECK(batch.Touch(state.data(), state.size()));
        CHECK(batch.EndReadOnlyCapture());
        change(state, {0, state.size()}, 0x9b);
        CHECK(batch.EndFrame() && batch.UndoTo(6));
        CHECK(state == original);
        closed(batch, fast);
    }
}
} // namespace

int main() {
    for (const bool fast : {false, true}) {
        for (const bool coalesce : {false, true}) {
            window_and_immediate_writes(fast, coalesce);
            scope_lifecycle(fast, coalesce);
            rejected_touch_prefixes(fast, coalesce);
            for (const bool batch : {false, true}) {
                extended_checkpoint(batch, fast, coalesce);
                growth_ring_and_reuse(batch, fast, coalesce);
            }
        }
    }
    std::printf("rollback journal capture batch: PASS (%llu checks)\n",
                static_cast<unsigned long long>(checks));
    return 0;
}
