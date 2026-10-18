// Dedicated Emscripten host probe. Link the real common RollbackJournal.cpp;
// no replacement imports or fake batch implementation are provided here.
#if !defined(__EMSCRIPTEN__) || defined(__wasm64__)
#error "This probe requires the real Emscripten wasm32 runtime"
#endif

#include <emscripten/emscripten.h>
#include <emscripten/heap.h>

#define main journal_capture_batch_directed_main
#include "rollback-journal-capture-batch-test.cpp"
#undef main

namespace capture_wasm_probe {
using Journal = Netplay::RollbackJournal;
using Stats = Netplay::RollbackReadOnlyCaptureStats;

// The host reads this live object after a deliberate trap, without reentering
// the abandoned C++ call stack. Fail compilation if its public schema changes.
static_assert(sizeof(Stats) == 64);
static_assert(offsetof(Stats, scopes) == 0 && offsetof(Stats, deferredBlocks) == 8 &&
              offsetof(Stats, deferredBytes) == 16 && offsetof(Stats, copiedBlocks) == 24 &&
              offsetof(Stats, copiedBytes) == 32 && offsetof(Stats, flushes) == 40 &&
              offsetof(Stats, batchImports) == 48 && offsetof(Stats, maxBlocksPerFlush) == 56);

struct Fixture {
    Journal journal;
    Bytes state;
    std::array<std::uint32_t, 9> description{};
    std::array<std::uint32_t, 12> status{};
    // phase: 0 idle, 1 inside Touch, 2 Touch returned, 3 inside End, 4 End returned.
    // Then: Touch entries, Touch returns, last address, last size, last result.
    volatile std::uint32_t marks[6]{};
    std::uint32_t frame = 0;
    std::uint32_t restored = std::numeric_limits<std::uint32_t>::max();
};

// Exactly one fixture per WASM instance. The host discards the entire instance
// after a deliberate exception; no destructor or journal method runs afterward.
Fixture* fixture = nullptr;

Fixture& current() {
    CHECK(fixture != nullptr);
    return *fixture;
}
std::uint32_t pointer(const void* value) {
    return static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(value));
}
} // namespace capture_wasm_probe

extern "C" {

EMSCRIPTEN_KEEPALIVE int journal_batch_run_directed_suite() {
    return journal_capture_batch_directed_main();
}

EMSCRIPTEN_KEEPALIVE std::uint32_t journal_batch_probe_create(
    std::uint32_t state_bytes, std::uint32_t max_bytes, std::uint32_t max_blocks) {
    using namespace capture_wasm_probe;
    CHECK(fixture == nullptr);
    CHECK(state_bytes > 0 && state_bytes <= 2u * 1024u * 1024u);
    CHECK(max_bytes > 0 && max_bytes <= 4u * 1024u * 1024u);
    CHECK(max_blocks > 0 && max_blocks <= 128);
    fixture = new Fixture;
    fixture->state = seeded(state_bytes, 0x80b421e9u);
    CHECK(fixture->journal.Reset({3, max_bytes, max_blocks, true, true}));
    fixture->marks[5] = std::numeric_limits<std::uint32_t>::max();
    fixture->description = {{
        1, pointer(fixture->state.data()), state_bytes,
        pointer(&fixture->journal.ReadOnlyCaptureStats()), sizeof(Stats),
        pointer(const_cast<const std::uint32_t*>(fixture->marks)), 6,
        pointer(fixture->status.data()), static_cast<std::uint32_t>(fixture->status.size())
    }};
    return pointer(fixture->description.data());
}

EMSCRIPTEN_KEEPALIVE std::uint32_t journal_batch_probe_status() {
    using namespace capture_wasm_probe;
    auto& f = current();
    const auto& j = f.journal;
    f.status = {{
        1, j.IsFrameOpen() ? 1u : 0u, j.IsReadOnlyCaptureOpen() ? 1u : 0u,
        static_cast<std::uint32_t>(j.PendingReadOnlyCaptureBlocks()),
        static_cast<std::uint32_t>(j.PendingReadOnlyCaptureBytes()),
        j.Failed() ? 1u : 0u,
        static_cast<std::uint32_t>(j.BytesForFrame(f.frame)),
        static_cast<std::uint32_t>(j.BlocksForFrame(f.frame)),
        static_cast<std::uint32_t>(j.FrameCount()),
        static_cast<std::uint32_t>(emscripten_get_heap_size()),
        f.restored, static_cast<std::uint32_t>(j.ArenaGrowths())
    }};
    return pointer(f.status.data());
}

EMSCRIPTEN_KEEPALIVE int journal_batch_probe_begin_frame(std::uint32_t frame) {
    auto& f = capture_wasm_probe::current();
    const bool result = f.journal.BeginFrame(frame);
    if (result) f.frame = frame;
    return result ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE int journal_batch_probe_begin_capture() {
    return capture_wasm_probe::current().journal.BeginReadOnlyCapture() ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE int journal_batch_probe_touch(std::uint32_t address, std::uint32_t bytes) {
    auto& f = capture_wasm_probe::current();
    f.marks[0] = 1;
    f.marks[1] = f.marks[1] + 1;
    f.marks[3] = address;
    f.marks[4] = bytes;
    f.marks[5] = std::numeric_limits<std::uint32_t>::max();
    // Intentionally no source-bounds shim here. The real journal must drain
    // its prefix and raise the original copy's RangeError / native WASM trap.
    const bool result = f.journal.Touch(
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(address)), bytes);
    f.marks[2] = f.marks[2] + 1;
    f.marks[5] = result ? 1 : 0;
    f.marks[0] = 2;
    return result ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE int journal_batch_probe_end_capture() {
    auto& f = capture_wasm_probe::current();
    f.marks[0] = 3;
    const bool result = f.journal.EndReadOnlyCapture();
    f.marks[0] = 4;
    return result ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE int journal_batch_probe_end_frame() {
    return capture_wasm_probe::current().journal.EndFrame() ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE void journal_batch_probe_fill(
    std::uint32_t offset, std::uint32_t bytes, std::uint32_t value) {
    auto& f = capture_wasm_probe::current();
    CHECK(!f.journal.IsReadOnlyCaptureOpen());
    change(f.state, {offset, bytes}, static_cast<std::uint8_t>(value));
}

EMSCRIPTEN_KEEPALIVE int journal_batch_probe_undo(std::uint32_t frame) {
    auto& f = capture_wasm_probe::current();
    CHECK(!f.journal.IsReadOnlyCaptureOpen());
    return f.journal.UndoTo(frame, &f.restored) ? 1 : 0;
}

EMSCRIPTEN_KEEPALIVE std::uint32_t journal_batch_probe_grow(std::uint32_t pages) {
    auto& f = capture_wasm_probe::current();
    CHECK(f.journal.IsReadOnlyCaptureOpen());
    CHECK(f.journal.PendingReadOnlyCaptureBlocks() > 0);
    CHECK(pages > 0 && pages <= 16);
    const auto before = emscripten_get_heap_size();
    const std::uint64_t requested = static_cast<std::uint64_t>(before) +
                                    static_cast<std::uint64_t>(pages) * 65536u;
    CHECK(requested <= emscripten_get_heap_max());
    CHECK(emscripten_resize_heap(static_cast<std::size_t>(requested)));
    CHECK(emscripten_get_heap_size() > before);
    return static_cast<std::uint32_t>(emscripten_get_heap_size());
}

} // extern "C"
