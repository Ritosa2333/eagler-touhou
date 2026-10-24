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
// spaces exact (they simply replace this one cache slot). Allocator mutations
// conservatively invalidate the slot regardless of which space they affect.
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
void* eagler_sim_malloc(void* space, size_t bytes) {
    eagler_sim_invalidate_used();
    return mspace_malloc(static_cast<mspace>(space), bytes);
}
void* eagler_sim_memalign(void* space, size_t alignment, size_t bytes) {
    eagler_sim_invalidate_used();
    return mspace_memalign(static_cast<mspace>(space), alignment, bytes);
}
void eagler_sim_free(void* space, void* pointer) {
    eagler_sim_invalidate_used();
    mspace_free(static_cast<mspace>(space), pointer);
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
