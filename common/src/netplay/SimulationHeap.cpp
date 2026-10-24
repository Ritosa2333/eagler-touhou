// An independent, fixed-base dlmalloc mspace. Its free lists and chunk headers
// live in the journaled region, so restoring state also restores allocation.
#if defined(EAGLER_WRITE_JOURNAL) && EAGLER_WRITE_JOURNAL
#define ONLY_MSPACES 1
#define MSPACES 1
#define USE_LOCKS 0
#define HAVE_MORECORE 0
#define HAVE_MMAP 0
#define HAVE_MREMAP 0
#define NO_MALLINFO 0
#define NO_MALLOC_STATS 1
#define MALLOC_ALIGNMENT 16
#ifndef __EMSCRIPTEN__
#define DLMALLOC_EXPORT static
#endif
extern "C" {
#include "../../third_party/dlmalloc/dlmalloc.inc"

// Host-owned telemetry, outside every mspace and its undo history. The journal
// currently owns one mspace; the key also makes interleaved queries of several
// spaces exact (they simply replace this one cache slot). Warm allocation/free
// queries maintain the exact total using actual chunk sizes, not request sizes.
// This follows the allocator's existing single-threaded, USE_LOCKS=0 contract.
static void* usedSpace = nullptr;
static size_t usedBytes = 0;
static bool usedValid = false;
#if defined(EAGLER_WRITE_JOURNAL_TEST_HOOKS) && EAGLER_WRITE_JOURNAL_TEST_HOOKS
static size_t usedScans = 0;
static void* lastCreatedSpace = nullptr;
#endif

void eagler_sim_invalidate_used() {
    usedValid = false;
}

void* eagler_sim_create(void* base, size_t capacity) {
    eagler_sim_invalidate_used();
    auto space = create_mspace_with_base(base, capacity, 0);
    if (space) mspace_set_footprint_limit(space, capacity);
#if defined(EAGLER_WRITE_JOURNAL_TEST_HOOKS) && EAGLER_WRITE_JOURNAL_TEST_HOOKS
    lastCreatedSpace = space;
#endif
    return space;
}
// mallinfo reports footprint minus all free chunks, including the top/footer.
// In one fixed external segment, allocation changes that free sum by the final
// in-use chunk size; alignment fragments and free coalescing preserve the sum.
// If the allocator configuration changes, revisit this accounting invariant.
static_assert(!HAVE_MORECORE && !HAVE_MMAP && !HAVE_MREMAP && !FOOTERS &&
              !PROCEED_ON_ERROR && !USE_LOCKS);
static bool usedCanUpdate(void* space) {
    if (usedSpace != space) eagler_sim_invalidate_used();
    return usedValid;
}
static void usedAdd(void* space, void* pointer, bool update) {
    if (!pointer) { eagler_sim_invalidate_used(); return; }
    // Instrumented allocator writes can fail the journal and invalidate this
    // cache during the call. Never reactivate it merely because malloc returns.
    if (update && usedValid && usedSpace == space) {
        const size_t delta = chunksize(mem2chunk(pointer));
        if (delta > MAX_SIZE_T-usedBytes) eagler_sim_invalidate_used();
        else usedBytes += delta;
    }
}
void* eagler_sim_malloc(void* space, size_t bytes) {
    const bool update = usedCanUpdate(space);
    auto* pointer = mspace_malloc(static_cast<mspace>(space), bytes);
    usedAdd(space,pointer,update);
    return pointer;
}
void* eagler_sim_memalign(void* space, size_t alignment, size_t bytes) {
    const bool update = usedCanUpdate(space);
    auto* pointer = mspace_memalign(static_cast<mspace>(space), alignment, bytes);
    usedAdd(space,pointer,update);
    return pointer;
}
void eagler_sim_free(void* space, void* pointer) {
    const bool update = usedCanUpdate(space);
    // Save the original size before coalescing can change or move its header.
    const size_t delta = update && pointer ? chunksize(mem2chunk(pointer)) : 0;
    mspace_free(static_cast<mspace>(space), pointer);
    if (update && usedValid && usedSpace == space) {
        if (delta > usedBytes) eagler_sim_invalidate_used();
        else usedBytes -= delta;
    }
}
size_t eagler_sim_used(void* space) {
    if (!usedValid || usedSpace != space) {
        usedBytes = mspace_mallinfo(static_cast<mspace>(space)).uordblks;
        usedSpace = space;
        usedValid = true;
#if defined(EAGLER_WRITE_JOURNAL_TEST_HOOKS) && EAGLER_WRITE_JOURNAL_TEST_HOOKS
        ++usedScans;
#endif
    }
    return usedBytes;
}

#if defined(EAGLER_WRITE_JOURNAL_TEST_HOOKS) && EAGLER_WRITE_JOURNAL_TEST_HOOKS
// Independent uncached oracle and work counter; absent from production builds.
size_t eagler_sim_test_used_uncached(void* space) {
    return mspace_mallinfo(static_cast<mspace>(space)).uordblks;
}
size_t eagler_sim_test_used_scans() { return usedScans; }
void* eagler_sim_test_last_created_space() { return lastCreatedSpace; }
#endif
}
#endif // EAGLER_WRITE_JOURNAL
